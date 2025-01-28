const wv = require("./webview.napi.node");
const lib = require("./jscallback.node");

/** Size hints for use with `webview_set_size` */
enum webview_hint {
  WEBVIEW_HINT_NONE,
  WEBVIEW_HINT_MIN,
  WEBVIEW_HINT_MAX,
  WEBVIEW_HINT_FIXED,
}
/** Webview handle kinds for use with `webview_get_native_handle` */
enum webview_native_handle_kind {
  WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW,
  WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET,
  WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER,
}
type cbKind_t = "bind" | "dispatch";

class JsCallback extends lib.JsCallback {
  /** An external pointer to the instance native callback function */
  ptr;

  /** The unique identifier of this `JsCallback` instance */
  uid;

  /**
   * Instance values and methods for working with Webview callback functions.
   * @param w - The Webview instance pointer
   * @param cbKind - "bind" or "dispatch"
   * @param callback - The user defined JS callback function
   */
  constructor(w: bigint, cbKind: cbKind_t, callback: Function) {
    if (cbKind === "bind") callback = wrapBindCb(w, callback);
    if (cbKind === "dispatch") callback = wrapDispatchCb(callback);

    super(callback);

    if (cbKind === "bind") this.ptr = super.bindPtr;
    if (cbKind === "dispatch") this.ptr = super.dispatchPtr;
    this.uid = super.cbUid;
  }

  /**
   * Releases the memory associated with this `JsCallback` instance.
   * This method must explicitly be called by the user to avoid memory leaks.
   * */
  close() {
    if (this.ptr === null) return;
    this.ptr = null;
    super.cleanup();
  }

  /** Initiates a user provided argument in the native callback instance.
   *  The `JsCallback` instance UID piggy-backs on the Webview user `arg` parameter ID.
   *  @returns `Object` The `arg` ID and the instance UID piggy-backed.
   */
  arg = (arg?: any) => {
    const argId = super.argId;
    if (argId === null) return isClosing(this.uid);
    super.setArg({ arg }, argId);
    return { cbUid: this.uid, argId };
  };
}

/** Returns the expected Webview `dispatch` cb shape. */
function wrapDispatchCb(callback: Function) {
  return function (w: bigint, arg: any) {
    callback(w, arg);
  };
}

/** Returns the expected Webview `bind` cb shape.  */
function wrapBindCb(w: bigint, callback: Function) {
  return function (id: string, req: string, arg: any) {
    try {
      const args = JSON.parse(req);
      let result = callback(...args, arg); // The bound user provided `arg` is added to the back of the arguments.
      const isPromise = result instanceof Promise;
      if (!isPromise) return processResult(result);
      result = result
        .then((result) => processResult)
        .catch((err) => {
          throw err;
        });
      function processResult(result) {
        result = typeof result !== "undefined" ? JSON.stringify(result) : "";
        wv.webview_return(w, id, 0, result);
      }
    } catch (err) {
      err = typeof err !== "undefined" ? JSON.stringify(err) : "";
      wv.webview_return(w, id, 1, err);
    }
  };
}

/** The native cb instance is being destroyed, so return a `null` argId */
function isClosing(cbUid: number) {
  const message = `A cb function was called after it's native instance (cbUid: ${cbUid}) was closed`;
  console.warn(message);
  return { cbUid, argId: null };
}

export { webview_hint, webview_native_handle_kind, JsCallback };
export default wv;
