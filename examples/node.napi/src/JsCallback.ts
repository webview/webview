type cbKind_t = "bind" | "dispatch";

const libCb = require("bindings")("./jscallback.node");
const { libWv: wv } = require("./webview.napi.js");

const cbMap = new Map<Number, JsCallback>();

class JsCallback extends libCb.JsCallback {
  /** An external pointer to the native instance callback function */
  ptr;
  /** The unique identifier of this `JsCallback` instance */
  uid;
  /** Argument counter for the instance */
  argCount = 0;

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
    this.uid = super.cbUid;

    if (cbKind === "bind") this.ptr = super.bindPtr;
    if (cbKind === "dispatch") this.ptr = super.dispatchPtr;

    cbMap.set(this.uid, this);
  }

  /**
   * Releases the memory associated with this `JsCallback` instance.
   * This method must explicitly be called by the user to avoid memory leaks.
   * */
  close(isShutdown?: boolean) {
    if (this.ptr === null) return;
    this.ptr = null;
    isShutdown ? super.destroy(true) : super.destroy();
    cbMap.delete(this.uid);
  }

  /**
   * Initiates a user provided argument in the native callback instance.
   *  The `JsCallback` instance UID piggy-backs on the Webview user `arg` parameter ID.
   *  @returns `Object` The `arg` ID and the instance UID piggy-backed.
   */
  arg = (arg?: any) => {
    if (!super.setArg) return isClosing(this.uid);
    const argId = this.argCount;
    this.argCount++;
    super.setArg({ arg }, argId);

    return { cbUid: this.uid, argId };
  };

  /**
   * Destroys active cb instances then ends the process.
   * @param code
   */
  static destroy(code: number) {
    cbMap.forEach((self) => {
      self.close(true);
    });
    process.exit(code);
  }
}

/** Wraps the user cb in the expected Webview `dispatch` cb shape. */
function wrapDispatchCb(callback: Function) {
  return function (w: bigint, arg: any) {
    callback(w, arg);
  };
}

/** Wraps the user cb in the expected Webview `bind` cb shape.  */
function wrapBindCb(w: bigint, callback: Function) {
  return function (id: string, req: string, arg: any) {
    try {
      const args = JSON.parse(req);
      let result = callback(...args, arg); // The bound user provided `arg` is added to the back of the arguments.
      const isPromise = result instanceof Promise;
      if (!isPromise) return processResult(result);
      result = result
        .then((result: any) => processResult)
        .catch((err: Error) => {
          throw err;
        });
      function processResult(result: any) {
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

module.exports = {
  JsCallback,
};
