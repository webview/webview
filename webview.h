#ifndef WEBVIEW_H
#define WEBVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WEBVIEW_GTK)
#include <JavaScriptCore/JavaScript.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>

struct webview_priv {
  GtkWidget *window;
  GtkWidget *scroller;
  GtkWidget *webview;
  int loop_result;
};
#elif defined(WEBVIEW_WINAPI)
#include <windows.h>

#include <exdisp.h>
#include <mshtmhst.h>
#include <mshtml.h>

struct webview_priv {
  HWND hwnd;
  IOleObject **browser;
};
#elif defined(WEBVIEW_COCOA)
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

@interface WebViewWindowDelegate
    : NSObject <NSWindowDelegate, WebFrameLoadDelegate>
@property(nonatomic, assign) struct webview *webview;
@end

struct webview_priv {
  NSAutoreleasePool *pool;
  NSWindow *window;
  WebView *webview;
  WebViewWindowDelegate *windowDelegate;
  int loop_result;
};
#else
#error "Define one of: WEBVIEW_GTK, WEBVIEW_COCOA or WEBVIEW_WINAPI"
#endif

struct webview {
  const char *url;
  const char *title;
  int width;
  int height;
  int resizable;
  void (*external_invoke_cb)(struct webview *w, const char *arg);
  struct webview_priv priv;
  void *userdata;
};

struct webview_dispatch_arg {
  void (*f)(struct webview *w, void *arg);
  struct webview *w;
  void *arg;
};

typedef void (*webview_dispatch_fn)(struct webview *w, void *arg);

static int webview_init(struct webview *w);
static int webview_loop(struct webview *w, int blocking);
static int webview_eval(struct webview *w, const char *js);
static void webview_dispatch(struct webview *w, webview_dispatch_fn fn,
                             void *arg);
static void webview_exit(struct webview *w);

static int webview(const char *title, const char *url, int w, int h,
                   int resizable) {
  struct webview webview = {0};
  webview.title = title;
  webview.url = url;
  webview.width = w;
  webview.height = h;
  webview.resizable = resizable;
  int r = webview_init(&webview);
  if (r != 0) {
    return r;
  }
  while (webview_loop(&webview, 1) == 0)
    ;
  webview_exit(&webview);
  return 0;
}

#if defined(WEBVIEW_GTK)
static JSValueRef
webview_external_invoke_cb(JSContextRef context, JSObjectRef fn,
                           JSObjectRef thisObject, size_t argc,
                           const JSValueRef args[], JSValueRef *err) {
  (void)fn;
  (void)thisObject;
  (void)argc;
  (void)args;
  (void)err;
  struct webview *w = (struct webview *)JSObjectGetPrivate(thisObject);
  if (w->external_invoke_cb != NULL && argc == 1) {
    JSStringRef js = JSValueToStringCopy(context, args[0], NULL);
    size_t n = JSStringGetMaximumUTF8CStringSize(js);
    char *s = g_new(char, n);
    JSStringGetUTF8CString(js, s, n);
    w->external_invoke_cb(w, s);
    JSStringRelease(js);
    g_free(s);
  }
  return JSValueMakeUndefined(context);
}
static const JSStaticFunction webview_external_static_funcs[] = {
    {"invoke_", webview_external_invoke_cb, kJSPropertyAttributeReadOnly},
    {NULL, NULL, 0},
};

static const JSClassDefinition webview_external_def = {
    0,          kJSClassAttributeNone,
    "external", NULL,
    NULL,       webview_external_static_funcs,
    NULL,       NULL,
    NULL,       NULL,
    NULL,       NULL,
    NULL,       NULL,
    NULL,       NULL,
    NULL,
};

static void webview_desroy_cb(GtkWidget *widget, gpointer arg) {
  (void)widget;
  struct webview *w = (struct webview *)arg;
  webview_exit(w);
}

static gboolean webview_context_menu_cb(WebKitWebView *webview,
                                        GtkWidget *default_menu,
                                        WebKitHitTestResult *hit_test_result,
                                        gboolean triggered_with_keyboard,
                                        gpointer userdata) {
  (void)webview;
  (void)default_menu;
  (void)hit_test_result;
  (void)triggered_with_keyboard;
  (void)userdata;
  return TRUE;
}

static void webview_window_object_cleared_cb(WebKitWebView *webview,
                                             WebKitWebFrame *frame,
                                             gpointer context,
                                             gpointer window_object,
                                             gpointer arg) {
  (void)webview;
  (void)frame;
  (void)window_object;
  JSClassRef cls = JSClassCreate(&webview_external_def);
  JSObjectRef obj = JSObjectMake(context, cls, arg);
  JSObjectRef glob = JSContextGetGlobalObject(context);
  JSStringRef s = JSStringCreateWithUTF8CString("external");
  JSObjectSetProperty(context, glob, s, obj, kJSPropertyAttributeNone, NULL);
}

static int webview_init(struct webview *w) {
  if (gtk_init_check(0, NULL) == FALSE) {
    return -1;
  }

  w->priv.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(w->priv.window), w->title);

  if (w->resizable) {
    gtk_window_set_default_size(GTK_WINDOW(w->priv.window), w->width,
                                w->height);
  } else {
    gtk_widget_set_size_request(w->priv.window, w->width, w->height);
  }
  gtk_window_set_resizable(GTK_WINDOW(w->priv.window), !!w->resizable);
  gtk_window_set_position(GTK_WINDOW(w->priv.window), GTK_WIN_POS_CENTER);

  w->priv.scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(w->priv.window), w->priv.scroller);

  w->priv.webview = webkit_web_view_new();
  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(w->priv.webview), w->url);
  gtk_container_add(GTK_CONTAINER(w->priv.scroller), w->priv.webview);

  gtk_widget_show_all(w->priv.window);

  g_signal_connect(G_OBJECT(w->priv.window), "destroy",
                   G_CALLBACK(webview_desroy_cb), w);
  g_signal_connect(G_OBJECT(w->priv.webview), "context-menu",
                   G_CALLBACK(webview_context_menu_cb), w);
  g_signal_connect(G_OBJECT(w->priv.webview), "window-object-cleared",
                   G_CALLBACK(webview_window_object_cleared_cb), w);
  return 0;
}

static int webview_loop(struct webview *w, int blocking) {
  gtk_main_iteration_do(blocking);
  return w->priv.loop_result;
}

static int webview_eval(struct webview *w, const char *js) {
  webkit_web_view_execute_script(WEBKIT_WEB_VIEW(w->priv.webview), js);
  return 0;
}

static gboolean webview_dispatch_wrapper(gpointer userdata) {
  struct webview_dispatch_arg *arg = (struct webview_dispatch_arg *)userdata;
  (*(arg->f))(arg->w, arg->arg);
  g_free(arg);
  return FALSE;
}

static void webview_dispatch(struct webview *w,
                             void (*f)(struct webview *w, void *arg),
                             void *arg) {
  struct webview_dispatch_arg *context =
      (struct webview_dispatch_arg *)g_new(struct webview_dispatch_arg *, 1);
  context->w = w;
  context->arg = arg;
  context->f = f;
  gdk_threads_add_idle(webview_dispatch_wrapper, context);
}

static void webview_exit(struct webview *w) { w->priv.loop_result = -1; }

#endif /* WEBVIEW_GTK */

#if defined(WEBVIEW_WINAPI)

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

typedef struct {
  IOleInPlaceFrame frame;
  HWND window;
} _IOleInPlaceFrameEx;

typedef struct {
  IOleInPlaceSite inplace;
  _IOleInPlaceFrameEx frame;
} _IOleInPlaceSiteEx;

typedef struct { IDocHostUIHandler ui; } _IDocHostUIHandlerEx;

typedef struct {
  IOleClientSite client;
  _IOleInPlaceSiteEx inplace;
  _IDocHostUIHandlerEx ui;
  IDispatch external;
} _IOleClientSiteEx;

static HRESULT STDMETHODCALLTYPE JS_QueryInterface(IDispatch FAR *This,
                                                   REFIID riid,
                                                   LPVOID FAR *ppvObj) {
  if (!memcmp(riid, &IID_IDispatch, sizeof(GUID))) {
    *ppvObj = This;
    return S_OK;
  }
  *ppvObj = 0;
  return E_NOINTERFACE;
}
static ULONG STDMETHODCALLTYPE JS_AddRef(IDispatch FAR *This) { return 1; }
static ULONG STDMETHODCALLTYPE JS_Release(IDispatch FAR *This) { return 1; }
static HRESULT STDMETHODCALLTYPE JS_GetTypeInfoCount(IDispatch FAR *This,
                                                     UINT *pctinfo) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE JS_GetTypeInfo(IDispatch FAR *This,
                                                UINT iTInfo, LCID lcid,
                                                ITypeInfo **ppTInfo) {
  return S_OK;
}
#define WEBVIEW_JS_INVOKE_ID 0x1000
static HRESULT STDMETHODCALLTYPE JS_GetIDsOfNames(IDispatch FAR *This,
                                                  REFIID riid,
                                                  LPOLESTR *rgszNames,
                                                  UINT cNames, LCID lcid,
                                                  DISPID *rgDispId) {
  if (cNames != 1) {
    return S_FALSE;
  }
  if (wcscmp(rgszNames[0], L"invoke_") == 0) {
    rgDispId[0] = WEBVIEW_JS_INVOKE_ID;
    return S_OK;
  }
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE
JS_Invoke(IDispatch FAR *This, DISPID dispIdMember, REFIID riid, LCID lcid,
          WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult,
          EXCEPINFO *pExcepInfo, UINT *puArgErr) {
  size_t offset = (size_t) & ((_IOleClientSiteEx *)NULL)->external;
  _IOleClientSiteEx *ex = (_IOleClientSiteEx *)((char *)(This)-offset);
  struct webview *w = (struct webview *)GetWindowLongPtr(
      ex->inplace.frame.window, GWLP_USERDATA);
  if (pDispParams->cArgs == 1 && pDispParams->rgvarg[0].vt == VT_BSTR) {
    BSTR bstr = pDispParams->rgvarg[0].bstrVal;
    int n = WideCharToMultiByte(CP_UTF8, 0, &bstr[0], -1, NULL, 0, NULL, NULL);
    char *s = (char *)GlobalAlloc(GMEM_FIXED, n);
    if (s != NULL) {
      WideCharToMultiByte(CP_UTF8, 0, &bstr[0], -1, &s[0], n, NULL, NULL);
      if (dispIdMember == WEBVIEW_JS_INVOKE_ID) {
        if (w->external_invoke_cb != NULL) {
          w->external_invoke_cb(w, s);
        }
      } else {
        return S_FALSE;
      }
      GlobalFree(s);
    }
  }
  return S_OK;
}

static IDispatchVtbl ExternalDispatchTable = {
    JS_QueryInterface, JS_AddRef,        JS_Release, JS_GetTypeInfoCount,
    JS_GetTypeInfo,    JS_GetIDsOfNames, JS_Invoke};

static ULONG STDMETHODCALLTYPE Site_AddRef(IOleClientSite FAR *This) {
  return 1;
}
static ULONG STDMETHODCALLTYPE Site_Release(IOleClientSite FAR *This) {
  return 1;
}
static HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite FAR *This) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite FAR *This,
                                                 DWORD dwAssign,
                                                 DWORD dwWhichMoniker,
                                                 IMoniker **ppmk) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
Site_GetContainer(IOleClientSite FAR *This, LPOLECONTAINER FAR *ppContainer) {
  *ppContainer = 0;
  return E_NOINTERFACE;
}
static HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite FAR *This) {
  return NOERROR;
}
static HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite FAR *This,
                                                   BOOL fShow) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
Site_RequestNewObjectLayout(IOleClientSite FAR *This) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite FAR *This,
                                                     REFIID riid,
                                                     void **ppvObject) {
  if (!memcmp(riid, &IID_IUnknown, sizeof(GUID)) ||
      !memcmp(riid, &IID_IOleClientSite, sizeof(GUID)))
    *ppvObject = &((_IOleClientSiteEx *)This)->client;
  else if (!memcmp(riid, &IID_IOleInPlaceSite, sizeof(GUID)))
    *ppvObject = &((_IOleClientSiteEx *)This)->inplace;
  else if (!memcmp(riid, &IID_IDocHostUIHandler, sizeof(GUID)))
    *ppvObject = &((_IOleClientSiteEx *)This)->ui;
  else {
    *ppvObject = 0;
    return (E_NOINTERFACE);
  }
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE InPlace_QueryInterface(
    IOleInPlaceSite FAR *This, REFIID riid, LPVOID FAR *ppvObj) {
  return (Site_QueryInterface(
      (IOleClientSite *)((char *)This - sizeof(IOleClientSite)), riid, ppvObj));
}
static ULONG STDMETHODCALLTYPE InPlace_AddRef(IOleInPlaceSite FAR *This) {
  return 1;
}
static ULONG STDMETHODCALLTYPE InPlace_Release(IOleInPlaceSite FAR *This) {
  return 1;
}
static HRESULT STDMETHODCALLTYPE InPlace_GetWindow(IOleInPlaceSite FAR *This,
                                                   HWND FAR *lphwnd) {
  *lphwnd = ((_IOleInPlaceSiteEx FAR *)This)->frame.window;
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_ContextSensitiveHelp(IOleInPlaceSite FAR *This, BOOL fEnterMode) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
InPlace_CanInPlaceActivate(IOleInPlaceSite FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnInPlaceActivate(IOleInPlaceSite FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnUIActivate(IOleInPlaceSite FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE InPlace_GetWindowContext(
    IOleInPlaceSite FAR *This, LPOLEINPLACEFRAME FAR *lplpFrame,
    LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect,
    LPOLEINPLACEFRAMEINFO lpFrameInfo) {
  *lplpFrame = (LPOLEINPLACEFRAME) & ((_IOleInPlaceSiteEx *)This)->frame;
  *lplpDoc = 0;
  lpFrameInfo->fMDIApp = FALSE;
  lpFrameInfo->hwndFrame = ((_IOleInPlaceFrameEx *)*lplpFrame)->window;
  lpFrameInfo->haccel = 0;
  lpFrameInfo->cAccelEntries = 0;
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE InPlace_Scroll(IOleInPlaceSite FAR *This,
                                                SIZE scrollExtent) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnUIDeactivate(IOleInPlaceSite FAR *This, BOOL fUndoable) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnInPlaceDeactivate(IOleInPlaceSite FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_DiscardUndoState(IOleInPlaceSite FAR *This) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
InPlace_DeactivateAndUndo(IOleInPlaceSite FAR *This) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnPosRectChange(IOleInPlaceSite FAR *This, LPCRECT lprcPosRect) {
  IOleObject *browserObject;
  IOleInPlaceObject *inplace;
  browserObject = *((IOleObject **)((char *)This - sizeof(IOleObject *) -
                                    sizeof(IOleClientSite)));
  if (!browserObject->lpVtbl->QueryInterface(
          browserObject, &IID_IOleInPlaceObject, (void **)&inplace)) {
    inplace->lpVtbl->SetObjectRects(inplace, lprcPosRect, lprcPosRect);
    inplace->lpVtbl->Release(inplace);
  }
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE Frame_QueryInterface(
    IOleInPlaceFrame FAR *This, REFIID riid, LPVOID FAR *ppvObj) {
  return E_NOTIMPL;
}
static ULONG STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame FAR *This) {
  return 1;
}
static ULONG STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame FAR *This) {
  return 1;
}
static HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame FAR *This,
                                                 HWND FAR *lphwnd) {
  *lphwnd = ((_IOleInPlaceFrameEx *)This)->window;
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
Frame_ContextSensitiveHelp(IOleInPlaceFrame FAR *This, BOOL fEnterMode) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame FAR *This,
                                                 LPRECT lprectBorder) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(
    IOleInPlaceFrame FAR *This, LPCBORDERWIDTHS pborderwidths) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(
    IOleInPlaceFrame FAR *This, LPCBORDERWIDTHS pborderwidths) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(
    IOleInPlaceFrame FAR *This, IOleInPlaceActiveObject *pActiveObject,
    LPCOLESTR pszObjName) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
Frame_InsertMenus(IOleInPlaceFrame FAR *This, HMENU hmenuShared,
                  LPOLEMENUGROUPWIDTHS lpMenuWidths) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame FAR *This,
                                               HMENU hmenuShared,
                                               HOLEMENU holemenu,
                                               HWND hwndActiveObject) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame FAR *This,
                                                   HMENU hmenuShared) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame FAR *This,
                                                     LPCOLESTR pszStatusText) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
Frame_EnableModeless(IOleInPlaceFrame FAR *This, BOOL fEnable) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
Frame_TranslateAccelerator(IOleInPlaceFrame FAR *This, LPMSG lpmsg, WORD wID) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE UI_QueryInterface(IDocHostUIHandler FAR *This,
                                                   REFIID riid,
                                                   LPVOID FAR *ppvObj) {
  return (Site_QueryInterface((IOleClientSite *)((char *)This -
                                                 sizeof(IOleClientSite) -
                                                 sizeof(_IOleInPlaceSiteEx)),
                              riid, ppvObj));
}
static ULONG STDMETHODCALLTYPE UI_AddRef(IDocHostUIHandler FAR *This) {
  return 1;
}
static ULONG STDMETHODCALLTYPE UI_Release(IDocHostUIHandler FAR *This) {
  return 1;
}
static HRESULT STDMETHODCALLTYPE UI_ShowContextMenu(
    IDocHostUIHandler FAR *This, DWORD dwID, POINT __RPC_FAR *ppt,
    IUnknown __RPC_FAR *pcmdtReserved, IDispatch __RPC_FAR *pdispReserved) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_GetHostInfo(IDocHostUIHandler FAR *This, DOCHOSTUIINFO __RPC_FAR *pInfo) {
  pInfo->cbSize = sizeof(DOCHOSTUIINFO);
  pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;
  pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_ShowUI(
    IDocHostUIHandler FAR *This, DWORD dwID,
    IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
    IOleCommandTarget __RPC_FAR *pCommandTarget,
    IOleInPlaceFrame __RPC_FAR *pFrame, IOleInPlaceUIWindow __RPC_FAR *pDoc) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_HideUI(IDocHostUIHandler FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_UpdateUI(IDocHostUIHandler FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_EnableModeless(IDocHostUIHandler FAR *This,
                                                   BOOL fEnable) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_OnDocWindowActivate(IDocHostUIHandler FAR *This, BOOL fActivate) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_OnFrameWindowActivate(IDocHostUIHandler FAR *This, BOOL fActivate) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_ResizeBorder(IDocHostUIHandler FAR *This, LPCRECT prcBorder,
                IOleInPlaceUIWindow __RPC_FAR *pUIWindow, BOOL fRameWindow) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_TranslateAccelerator(IDocHostUIHandler FAR *This, LPMSG lpMsg,
                        const GUID __RPC_FAR *pguidCmdGroup, DWORD nCmdID) {
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE UI_GetOptionKeyPath(
    IDocHostUIHandler FAR *This, LPOLESTR __RPC_FAR *pchKey, DWORD dw) {
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE UI_GetDropTarget(
    IDocHostUIHandler FAR *This, IDropTarget __RPC_FAR *pDropTarget,
    IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget) {
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE UI_GetExternal(
    IDocHostUIHandler FAR *This, IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) {
  *ppDispatch = (IDispatch *)(This + 1);
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_TranslateUrl(
    IDocHostUIHandler FAR *This, DWORD dwTranslate, OLECHAR __RPC_FAR *pchURLIn,
    OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut) {
  *ppchURLOut = 0;
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE
UI_FilterDataObject(IDocHostUIHandler FAR *This, IDataObject __RPC_FAR *pDO,
                    IDataObject __RPC_FAR *__RPC_FAR *ppDORet) {
  *ppDORet = 0;
  return S_FALSE;
}

static const TCHAR *classname = "WebView";
static const SAFEARRAYBOUND ArrayBound = {1, 0};

static IOleClientSiteVtbl MyIOleClientSiteTable = {
    Site_QueryInterface, Site_AddRef,       Site_Release,
    Site_SaveObject,     Site_GetMoniker,   Site_GetContainer,
    Site_ShowObject,     Site_OnShowWindow, Site_RequestNewObjectLayout};
static IOleInPlaceSiteVtbl MyIOleInPlaceSiteTable = {
    InPlace_QueryInterface,
    InPlace_AddRef,
    InPlace_Release,
    InPlace_GetWindow,
    InPlace_ContextSensitiveHelp,
    InPlace_CanInPlaceActivate,
    InPlace_OnInPlaceActivate,
    InPlace_OnUIActivate,
    InPlace_GetWindowContext,
    InPlace_Scroll,
    InPlace_OnUIDeactivate,
    InPlace_OnInPlaceDeactivate,
    InPlace_DiscardUndoState,
    InPlace_DeactivateAndUndo,
    InPlace_OnPosRectChange};

static IOleInPlaceFrameVtbl MyIOleInPlaceFrameTable = {
    Frame_QueryInterface,
    Frame_AddRef,
    Frame_Release,
    Frame_GetWindow,
    Frame_ContextSensitiveHelp,
    Frame_GetBorder,
    Frame_RequestBorderSpace,
    Frame_SetBorderSpace,
    Frame_SetActiveObject,
    Frame_InsertMenus,
    Frame_SetMenu,
    Frame_RemoveMenus,
    Frame_SetStatusText,
    Frame_EnableModeless,
    Frame_TranslateAccelerator};

static IDocHostUIHandlerVtbl MyIDocHostUIHandlerTable = {
    UI_QueryInterface,
    UI_AddRef,
    UI_Release,
    UI_ShowContextMenu,
    UI_GetHostInfo,
    UI_ShowUI,
    UI_HideUI,
    UI_UpdateUI,
    UI_EnableModeless,
    UI_OnDocWindowActivate,
    UI_OnFrameWindowActivate,
    UI_ResizeBorder,
    UI_TranslateAccelerator,
    UI_GetOptionKeyPath,
    UI_GetDropTarget,
    UI_GetExternal,
    UI_TranslateUrl,
    UI_FilterDataObject};

static void UnEmbedBrowserObject(struct webview *w) {
  if (w->priv.browser != NULL) {
    (*w->priv.browser)->lpVtbl->Close(*w->priv.browser, OLECLOSE_NOSAVE);
    (*w->priv.browser)->lpVtbl->Release(*w->priv.browser);
    GlobalFree(w->priv.browser);
    w->priv.browser = NULL;
  }
}

static int EmbedBrowserObject(struct webview *w) {
  IWebBrowser2 *webBrowser2 = NULL;
  LPCLASSFACTORY pClassFactory = NULL;
  IOleObject **browser = (IOleObject **)GlobalAlloc(
      GMEM_FIXED, sizeof(IOleObject *) + sizeof(_IOleClientSiteEx));
  if (browser == NULL) {
    goto error;
  }
  w->priv.browser = browser;

  _IOleClientSiteEx *_iOleClientSiteEx = (_IOleClientSiteEx *)(browser + 1);
  _iOleClientSiteEx->client.lpVtbl = &MyIOleClientSiteTable;
  _iOleClientSiteEx->inplace.inplace.lpVtbl = &MyIOleInPlaceSiteTable;
  _iOleClientSiteEx->inplace.frame.frame.lpVtbl = &MyIOleInPlaceFrameTable;
  _iOleClientSiteEx->inplace.frame.window = w->priv.hwnd;
  _iOleClientSiteEx->ui.ui.lpVtbl = &MyIDocHostUIHandlerTable;
  _iOleClientSiteEx->external.lpVtbl = &ExternalDispatchTable;

  if (CoGetClassObject(&CLSID_WebBrowser,
                       CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, NULL,
                       &IID_IClassFactory, (void **)&pClassFactory) != S_OK) {
    goto error;
  }

  if (pClassFactory == NULL) {
    goto error;
  }

  if (pClassFactory->lpVtbl->CreateInstance(pClassFactory, 0, &IID_IOleObject,
                                            (void **)browser) != S_OK) {
    goto error;
  }
  pClassFactory->lpVtbl->Release(pClassFactory);
  if ((*browser)->lpVtbl->SetClientSite(
          *browser, (IOleClientSite *)_iOleClientSiteEx) != S_OK) {
    goto error;
  }
  (*browser)->lpVtbl->SetHostNames(*browser, L"My Host Name", 0);

  if (OleSetContainedObject((struct IUnknown *)(*browser), TRUE) != S_OK) {
    goto error;
  }
  RECT rect;
  GetClientRect(w->priv.hwnd, &rect);
  if ((*browser)->lpVtbl->DoVerb((*browser), OLEIVERB_SHOW, NULL,
                                 (IOleClientSite *)_iOleClientSiteEx, -1,
                                 w->priv.hwnd, &rect) != S_OK) {
    goto error;
  }
  if ((*browser)->lpVtbl->QueryInterface((*browser), &IID_IWebBrowser2,
                                         (void **)&webBrowser2) != S_OK) {
    goto error;
  }

  webBrowser2->lpVtbl->put_Left(webBrowser2, 0);
  webBrowser2->lpVtbl->put_Top(webBrowser2, 0);
  webBrowser2->lpVtbl->put_Width(webBrowser2, rect.right);
  webBrowser2->lpVtbl->put_Height(webBrowser2, rect.bottom);
  webBrowser2->lpVtbl->Release(webBrowser2);

  return 0;
error:
  UnEmbedBrowserObject(w);
  if (pClassFactory != NULL) {
    pClassFactory->lpVtbl->Release(pClassFactory);
  }
  if (browser != NULL) {
    GlobalFree(browser);
  }
  return -1;
}

static long DisplayHTMLPage(struct webview *w) {
  IWebBrowser2 *webBrowser2;
  VARIANT myURL;
  IOleObject *browserObject;
  browserObject = *w->priv.browser;
  if (!browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2,
                                             (void **)&webBrowser2)) {
    LPCSTR webPageName = (LPCSTR)w->url;
    VariantInit(&myURL);
    myURL.vt = VT_BSTR;
#ifndef UNICODE
    {
      wchar_t *buffer;
      DWORD size;
      size = MultiByteToWideChar(CP_ACP, 0, webPageName, -1, 0, 0);
      if (!(buffer =
                (wchar_t *)GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size)))
        goto badalloc;
      MultiByteToWideChar(CP_ACP, 0, webPageName, -1, buffer, size);
      myURL.bstrVal = SysAllocString(buffer);
      GlobalFree(buffer);
    }
#else
    myURL.bstrVal = SysAllocString(webPageName);
#endif
    if (!myURL.bstrVal) {
    badalloc:
      webBrowser2->lpVtbl->Release(webBrowser2);
      return (-6);
    }
    webBrowser2->lpVtbl->Navigate2(webBrowser2, &myURL, 0, 0, 0, 0);
    VariantClear(&myURL);
    webBrowser2->lpVtbl->Release(webBrowser2);
    return (0);
  }
  return (-5);
}

static LRESULT CALLBACK wndproc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam) {
  struct webview *w = (struct webview *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  switch (uMsg) {
  case WM_CREATE:
    w = (struct webview *)((CREATESTRUCT *)lParam)->lpCreateParams;
    w->priv.hwnd = hwnd;
    return EmbedBrowserObject(w);
  case WM_DESTROY:
    UnEmbedBrowserObject(w);
    PostQuitMessage(0);
    return TRUE;
  case WM_SIZE: {
    IWebBrowser2 *webBrowser2;
    IOleObject *browser = *w->priv.browser;
    if (!browser->lpVtbl->QueryInterface(browser, &IID_IWebBrowser2,
                                         (void **)&webBrowser2)) {
      RECT rect;
      GetClientRect(hwnd, &rect);
      webBrowser2->lpVtbl->put_Width(webBrowser2, rect.right);
      webBrowser2->lpVtbl->put_Height(webBrowser2, rect.bottom);
    }
    return TRUE;
  }
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static int webview_init(struct webview *w) {
  WNDCLASSEX wc;
  HINSTANCE hInstance;
  STARTUPINFO info;
  DWORD style;
  RECT rect;

  hInstance = GetModuleHandle(NULL);
  if (hInstance == NULL) {
    return -1;
  }
  GetStartupInfo(&info);
  if (OleInitialize(NULL) != S_OK) {
    return -1;
  }
  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.hInstance = hInstance;
  wc.lpfnWndProc = wndproc;
  wc.lpszClassName = classname;
  RegisterClassEx(&wc);

  style = WS_OVERLAPPEDWINDOW;
  if (!w->resizable) {
    style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
  }

  GetClientRect(GetDesktopWindow(), &rect);
  rect.left = (rect.right / 2) - (w->width / 2);
  rect.top = (rect.bottom / 2) - (w->height / 2);

  w->priv.hwnd = CreateWindowEx(0, classname, w->title, style, rect.left,
                                rect.top, w->width, w->height, HWND_DESKTOP,
                                NULL, hInstance, (void *)w);
  if (w->priv.hwnd == 0) {
    OleUninitialize();
    return -1;
  }

  SetWindowLongPtr(w->priv.hwnd, GWLP_USERDATA, (LONG_PTR)w);

  DisplayHTMLPage(w);

  SetWindowText(w->priv.hwnd, w->title);
  ShowWindow(w->priv.hwnd, info.wShowWindow);
  UpdateWindow(w->priv.hwnd);
  SetFocus(w->priv.hwnd);

  return 0;
}

static int webview_loop(struct webview *w, int blocking) {
  MSG msg;
  if (blocking) {
    GetMessage(&msg, 0, 0, 0);
  } else {
    PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
  }
  if (msg.message == WM_QUIT) {
    return -1;
  }
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  return 0;
}

static int webview_eval(struct webview *w, const char *js) {
  IWebBrowser2 *webBrowser2;
  IHTMLDocument2 *htmlDoc2;
  IDispatch *docDispatch;
  IDispatch *scriptDispatch;
  if ((*w->priv.browser)
          ->lpVtbl->QueryInterface((*w->priv.browser), &IID_IWebBrowser2,
                                   (void **)&webBrowser2) != S_OK) {
    return -1;
  }

  if (webBrowser2->lpVtbl->get_Document(webBrowser2, &docDispatch) != S_OK) {
    return -1;
  }
  if (docDispatch->lpVtbl->QueryInterface(docDispatch, &IID_IHTMLDocument2,
                                          (void **)&htmlDoc2) != S_OK) {
    return -1;
  }
  if (htmlDoc2->lpVtbl->get_Script(htmlDoc2, &scriptDispatch) != S_OK) {
    return -1;
  }
  DISPID dispid;
  BSTR evalStr = L"eval";
  if (scriptDispatch->lpVtbl->GetIDsOfNames(scriptDispatch, &IID_NULL, &evalStr,
                                            1, LOCALE_SYSTEM_DEFAULT,
                                            &dispid) != S_OK) {
    return -1;
  }

  DISPPARAMS params;
  VARIANT arg;
  VARIANT result;
  EXCEPINFO excepInfo;
  UINT nArgErr = (UINT)-1;
  params.cArgs = 1;
  params.cNamedArgs = 0;
  params.rgvarg = &arg;
  arg.vt = VT_BSTR;
  static const char *prologue = "(function(){";
  static const char *epilogue = ";})();";
  int n = strlen(prologue) + strlen(epilogue) + strlen(js) + 1;
  char *eval = (char *)malloc(n);
  strcpy(eval, prologue);
  strcat(eval, js);
  strcat(eval, epilogue);
  int m = MultiByteToWideChar(CP_ACP, 0, eval, -1, 0, 0);
  wchar_t *buf = (wchar_t *)GlobalAlloc(GMEM_FIXED, m * sizeof(wchar_t));
  MultiByteToWideChar(CP_ACP, 0, eval, -1, buf, m);
  arg.bstrVal = SysAllocString(buf);
  if (scriptDispatch->lpVtbl->Invoke(scriptDispatch, dispid, &IID_NULL, 0,
                                     DISPATCH_METHOD, &params, &result,
                                     &excepInfo, &nArgErr) != S_OK) {
    return -1;
  }
  SysFreeString(arg.bstrVal);
  free(eval);
  scriptDispatch->lpVtbl->Release(scriptDispatch);
  htmlDoc2->lpVtbl->Release(htmlDoc2);
  docDispatch->lpVtbl->Release(docDispatch);
  return 0;
}

static void webview_exit(struct webview *w) { OleUninitialize(); }

#endif /* WEBVIEW_WINAPI */

#if defined(WEBVIEW_COCOA)
@implementation WebViewWindowDelegate
- (void)windowWillClose:(NSNotification *)notification {
  self.webview->priv.loop_result = -1;
}
+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector {
  return NO;
}
- (void)webView:(WebView *)webView
    didClearWindowObject:(WebScriptObject *)windowScriptObject
                forFrame:(WebFrame *)frame {
  [windowScriptObject setValue:self forKey:@"external"];
}
- (void)invoke:(NSString *)arg {
  if (self.webview->external_invoke_cb != NULL) {
    self.webview->external_invoke_cb(self.webview, [arg UTF8String]);
  }
}
@end

static int webview_init(struct webview *w) {
  w->priv.pool = [[NSAutoreleasePool alloc] init];
  [NSApplication sharedApplication];

  NSString *nsTitle = [NSString stringWithUTF8String:w->title];
  NSRect r = NSMakeRect(0, 0, w->width, w->height);
  NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                     NSWindowStyleMaskMiniaturizable;
  if (w->resizable) {
    style = style | NSWindowStyleMaskResizable;
  }
  w->priv.window = [[NSWindow alloc] initWithContentRect:r
                                               styleMask:style
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
  [w->priv.window autorelease];
  [w->priv.window setTitle:nsTitle];
  w->priv.windowDelegate = [WebViewWindowDelegate new];
  w->priv.windowDelegate.webview = w;
  [w->priv.window setDelegate:w->priv.windowDelegate];

  w->priv.webview =
      [[WebView alloc] initWithFrame:r frameName:@"WebView" groupName:nil];
  NSURL *nsURL = [NSURL URLWithString:[NSString stringWithUTF8String:w->url]];
  [[w->priv.webview mainFrame] loadRequest:[NSURLRequest requestWithURL:nsURL]];

  [w->priv.webview setAutoresizesSubviews:YES];
  [w->priv.webview
      setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
  w->priv.webview.frameLoadDelegate = w->priv.windowDelegate;
  [[w->priv.window contentView] addSubview:w->priv.webview];

  [w->priv.window orderFrontRegardless];
  return 0;
}

static int webview_loop(struct webview *w, int blocking) {
  NSDate *until = (blocking ? [NSDate distantFuture] : [NSDate distantPast]);
  NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                      untilDate:until
                                         inMode:NSDefaultRunLoopMode
                                        dequeue:YES];
  if (event) {
    [NSApp sendEvent:event];
  }
  return w->priv.loop_result;
}

static int webview_eval(struct webview *w, const char *js) {
  NSString *nsJS = [NSString stringWithUTF8String:js];
  [[w->priv.webview windowScriptObject] evaluateWebScript:nsJS];
  return 0;
}

static void webview_exit(struct webview *w) { [NSApp terminate:NSApp]; }

#endif /* WEBVIEW_COCOA */

#ifdef __cplusplus
}
#endif

#endif /* WEBVIEW_H */
