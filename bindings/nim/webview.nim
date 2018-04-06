{.passC: "-DWEBVIEW_STATIC -DWEBVIEW_IMPLEMENTATION".}
{.passC: "-I" & currentSourcePath().substr(0, high(currentSourcePath()) - 11) & "../.." .}
when defined(linux):
  {.passC: "-DWEBVIEW_GTK=1 " &
          staticExec"pkg-config --cflags gtk+-3.0 webkit2gtk-4.0".}
  {.passL: staticExec"pkg-config --libs gtk+-3.0 webkit2gtk-4.0".}
elif defined(windows):
  {.passC: "-DWEBVIEW_WINAPI=1".}
  {.passL: "-lole32 -lcomctl32 -loleaut32 -luuid -lgdi32".}
elif defined(macosx):
  {.passC: "-DWEBVIEW_COCOA=1 -x objective-c".}
  {.passL: "-framework Cocoa -framework WebKit".}

import tables, strutils
import macros
import json

##
## Hight level api and javascript bindings to easy bidirectonal 
## message passsing for ``nim`` and the ``webview`` .
##

type
  ExternalInvokeCb* = proc (w: Webview; arg: string)
  WebviewPrivObj{.importc: "struct webview_priv", header: "webview.h", bycopy.} = object
  WebviewObj* {.importc: "struct webview", header: "webview.h", bycopy.} = object
    url* {.importc: "url".}: cstring
    title* {.importc: "title".}: cstring
    width* {.importc: "width".}: cint
    height* {.importc: "height".}: cint
    resizable* {.importc: "resizable".}: cint
    debug* {.importc: "debug".}: cint
    invokeCb {.importc: "external_invoke_cb".}: pointer
    priv {.importc: "priv".}: WebviewPrivObj
    userdata {.importc: "userdata".}: pointer
  Webview* = ptr WebviewObj

  DialogType* {.size: sizeof(cint).} = enum
    dtOpen = 0, dtSave = 1, dtAlert = 2
  
const
  dFlagFile* = 0
  dFlagDir* = 1
  dFlagInfo* = 1 shl 1
  dFlagWarn* = 2 shl 1
  dFlagError* = 3 shl 1
  dFlagAlertMask* = 3 shl 1
  
type
  DispatchFn* = proc()
  
proc init*(w: Webview): cint {.importc: "webview_init", header: "webview.h".}
proc loop*(w: Webview; blocking: cint): cint {.importc: "webview_loop", header: "webview.h".}
proc eval*(w: Webview; js: cstring): cint {.importc: "webview_eval", header: "webview.h".}
proc injectCss*(w: Webview; css: cstring): cint {.importc: "webview_inject_css", header: "webview.h".}
proc setTitle*(w: Webview; title: cstring) {.importc: "webview_set_title", header: "webview.h".}
proc setColor*(w: Webview; r,g,b,a: uint8) {.importc: "webview_set_color", header: "webview.h".}
proc setFullscreen*(w: Webview; fullscreen: cint) {.importc: "webview_set_fullscreen", header: "webview.h".}
proc dialog*(w: Webview; dlgtype: DialogType; flags: cint; title: cstring; arg: cstring; result: cstring; resultsz: csize) {.
    importc: "webview_dialog", header: "webview.h".}
proc dispatch(w: Webview; fn: pointer; arg: pointer) {.importc: "webview_dispatch", header: "webview.h".}
proc terminate*(w: Webview) {.importc: "webview_terminate", header: "webview.h".}
proc exit*(w: Webview) {.importc: "webview_exit", header: "webview.h".}
proc debug*(format: cstring) {.varargs, importc: "webview_debug", header: "webview.h".}
proc printLog*(s: cstring) {.importc: "webview_print_log", header: "webview.h".}
proc webview*(title: cstring; url: cstring; w: cint; h: cint; resizable: cint): cint {.importc: "webview", header: "webview.h".}

type
  # ExternalProc[P, R] = proc(param: P, ret: var R): int
  CallHook = proc(params: string): string # json -> proc -> json
  MethodInfo* = object
    scope*: string
    name*: string
    args*: string # json string

# for bindProc
var eps = newTable[Webview, TableRef[string, TableRef[string, CallHook]]]()

# easy callbacks
var cbs = newTable[Webview, ExternalInvokeCb]()

proc generalExternalInvokeCallback(w: Webview, arg: cstring) {.exportc.} =
  var handled = false
  if eps.hasKey(w):
    try:
      var mi = parseJson($arg).to(MethodInfo)
      if hasKey(eps[w], mi.scope) and hasKey(eps[w][mi.scope], mi.name):
        discard eps[w][mi.scope][mi.name](mi.args) # TODO handle return values using js callbacks
        handled = true
    except:
      echo getCurrentExceptionMsg()
  elif cbs.hasKey(w): 
    cbs[w](w, $arg)
    handled = true
  if handled == false:
    echo "external invode:'", arg, "' not handled"

proc `externalInvokeCB=`*(w: Webview, cb: ExternalInvokeCb)=
  ## Set external invoke callback for webview
  cbs[w] = cb

proc newWebView*(title="WebView", url="", 
                 width=640, height=480, 
                 resizable=true, debug=false,
                 cb:ExternalInvokeCb=nil): Webview =
  ## ``newWebView`` creates and opens a new webview window using the given settings. 
  ## This function will do webview ``init``
  var w = cast[Webview](alloc0(sizeof(WebviewObj)))
  w.title = title
  w.url = url
  w.width = width.cint
  w.height = height.cint
  w.resizable = if resizable: 1 else: 0
  w.debug = if debug: 1 else: 0
  w.invokeCb = generalExternalInvokeCallback
  if cb != nil:
    w.externalInvokeCB=cb
  if w.init() != 0: return nil
  return w

# for dispatch

var dispatchTable = newTable[int, DispatchFn]()

proc generalDispatchProc(w: Webview, arg: pointer) {.exportc.} =
  let idx = cast[int](arg)
  let fn = dispatchTable[idx]
  fn()

proc dispatch*(w: Webview, fn: DispatchFn) =
  let idx = dispatchTable.len()+1
  dispatchTable[idx] = fn
  dispatch(w, generalDispatchProc, cast[pointer](idx))

proc dialog*(w :Webview, dlgType: DialogType, dlgFlag: int, title, arg: string): string =
  ## dialog() opens a system dialog of the given type and title. String
  ## argument can be provided for certain dialogs, such as alert boxes. For
  ## alert boxes argument is a message inside the dialog box.
  let maxPath = 4096
  let resultPtr = cast[cstring](alloc0(maxPath))
  defer: dealloc(resultPtr)
  w.dialog(dlgType, dlgFlag.cint, title.cstring, arg.cstring, resultPtr, maxPath.csize) 
  return $resultPtr

proc msg*(w: Webview, title, msg: string) =
  ## Show one message box
  discard w.dialog(dtAlert, 0, title, msg)

proc info*(w: Webview, title, msg: string) =
  ## Show one alert box
  discard w.dialog(dtAlert, dFlagInfo, title, msg)

proc warn*(w: Webview, title, msg: string) =
  ## Show one warn box
  discard w.dialog(dtAlert, dFlagWarn, title, msg)

proc error*(w: Webview, title, msg: string) =
  ## Show one error box
  discard w.dialog(dtAlert, dFlagError, title, msg)

proc dialogOpen*(w: Webview, title="Open File", flag=dFlagFile): string =
  ## Opens a dialog that requests filenames from the user. Returns ""
  ## if the user closed the dialog without selecting a file. 
  return w.dialog(dtOpen, flag, title, "")

proc dialogSave*(w: Webview, title="Save File", flag=dFlagFile): string =
  ## Opens a dialog that requests a filename to save to from the user.
  ## Returns "" if the user closed the dialog without selecting a file.
  return w.dialog(dtSave, flag, title, "")

proc setFullscreen*(w: Webview, fullscree=true): bool =
  ## setFullscreen according to `fullscree` and return `fullscree`
  if fullscree: setFullscreen(w, 1) else: setFullscreen(w, 0)
  return fullscree

proc run*(w: Webview)=
  ## ``run`` starts the main UI loop until the user closes the webview window or
  ## Terminate() is called.
  while w.loop(1) == 0:
    discard

proc open*(title="WebView", url="", width=640, height=480, resizable=true):int {.discardable.} =
  ## Open is a simplified API to open a single native window with a full-size webview in
  ## it. It can be helpful if you want to communicate with the core app using XHR
  ## or WebSockets (as opposed to using JavaScript bindings).
  ##
  ## Window appearance can be customized using title, width, height and resizable parameters.
  ## URL must be provided and can user either a http or https protocol, or be a
  ## local file:// URL. On some platforms "data:" URLs are also supported
  ## (Linux/MacOS).
  webview(title.cstring, url.cstring, width.cint, height.cint, (if resizable: 1 else: 0).cint)

const
  jsTemplate = """
if (typeof $2 === 'undefined') {
	$2 = {};
}
$2.$1 = function(arg) {
	window.external.invoke(
    JSON.stringify(
      {scope: "$2", name: "$1", args: JSON.stringify(arg)}
    )
  );
};
"""
  jsTemplateOnlyArg = """
if (typeof $2 === 'undefined') {
	$2 = {};
}
$2.$1 = function(arg) {
	window.external.invoke(
    JSON.stringify(
      {scope: "$2", name: "$1", args: JSON.stringify(arg)}
    )
  );
};
"""
  jsTemplateNoArg = """
if (typeof $2 === 'undefined') {
	$2 = {};
}
$2.$1 = function() {
	window.external.invoke(
    JSON.stringify(
      {scope: "$2", name: "$1", args: ""}
    )
  );
};
"""

proc bindProc*[P, R](w: Webview, scope, name: string, p: (proc(param: P): R)) =
  proc hook(hookParam: string): string =
    var 
      paramVal: P
      retVal: R
    try:
      let jnode = parseJson(hookParam)
      echo jnode
      paramVal = jnode.to(P)
    except:
      return "parse args failed: " & getCurrentExceptionMsg()
    retVal = p(paramVal)
    return $(%*retVal) # ==> json
  discard eps.hasKeyOrPut(w, newTable[string, TableRef[string, CallHook]]())
  discard hasKeyOrPut(eps[w], scope, newTable[string, CallHook]())
  eps[w][scope][name] = hook
  # TODO eval jscode
  w.dispatch(proc() = discard w.eval(jsTemplate%[name, scope]))

proc bindProcNoArg*(w: Webview, scope, name: string, p: proc()) =
  ## ugly hack or macro will fail
  proc hook(hookParam: string): string =
    p()
    return ""
  discard eps.hasKeyOrPut(w, newTable[string, TableRef[string, CallHook]]())
  discard hasKeyOrPut(eps[w], scope, newTable[string, CallHook]())
  eps[w][scope][name] = hook
  # TODO eval jscode
  w.dispatch(proc() = discard w.eval(jsTemplateNoArg%[name, scope]))

proc bindProc*[P](w: Webview, scope, name: string, p: proc(arg:P)) =
  proc hook(hookParam: string): string =
    var 
      paramVal: P
    try:
      let jnode = parseJson(hookParam)
      paramVal = jnode.to(P)
    except:
      return "parse args failed: " & getCurrentExceptionMsg()
    p(paramVal)
    return ""
  discard eps.hasKeyOrPut(w, newTable[string, TableRef[string, CallHook]]())
  discard hasKeyOrPut(eps[w], scope, newTable[string, CallHook]()) 
  eps[w][scope][name] = hook
  # TODO eval jscode
  w.dispatch(proc() = discard w.eval(jsTemplateOnlyArg%[name, scope]))

macro bindProcs*(w: Webview, scope: string, n: untyped): untyped =
  ## bind procs like:
  ##
  ## .. code-block:: nim
  ## 
  ##    proc fn[T, U](arg: T): U
  ##    proc fn[T](arg: T)
  ##    proc fn()
  ##
  ## to webview ``w``, in scope ``scope``
  ## then you can invode in js side, like this:
  ##
  ## .. code-block:: js
  ## 
  ##    scope.fn(arg)
  ##
  expectKind(n, nnkStmtList)
  let body = n
  for def in n:
    expectKind(def, nnkProcDef)
    let params = def.params()
    let fname = $def[0]
    # expectKind(params[0], nnkSym)
    if params.len() == 1 and params[0].kind() == nnkEmpty: # no args
      body.add(newCall("bindProcNoArg", w, scope, newLit(fname), newIdentNode(fname)))
      continue 
    if params.len() > 2 :
      error("""only proc like `proc fn[T, U](arg: T): U` or 
              `proc fn[T](arg: T)` or 
              `proc()`
            is allowed""", 
            def)
    body.add(newCall("bindProc", w, scope, newLit(fname), newIdentNode(fname)))
  result = newBlockStmt(body)
  echo repr result
