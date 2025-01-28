import { createRequire } from "node:module";
var __require = /* @__PURE__ */ createRequire(import.meta.url);

// JsCallback.ts
var wv = __require("./webview.napi.node");
var lib = __require("./jscallback.node");
var webview_hint;
((webview_hint2) => {
  webview_hint2[webview_hint2["WEBVIEW_HINT_NONE"] = 0] = "WEBVIEW_HINT_NONE";
  webview_hint2[webview_hint2["WEBVIEW_HINT_MIN"] = 1] = "WEBVIEW_HINT_MIN";
  webview_hint2[webview_hint2["WEBVIEW_HINT_MAX"] = 2] = "WEBVIEW_HINT_MAX";
  webview_hint2[webview_hint2["WEBVIEW_HINT_FIXED"] = 3] = "WEBVIEW_HINT_FIXED";
})(webview_hint ||= {});
var webview_native_handle_kind;
((webview_native_handle_kind2) => {
  webview_native_handle_kind2[webview_native_handle_kind2["WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW"] = 0] = "WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW";
  webview_native_handle_kind2[webview_native_handle_kind2["WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET"] = 1] = "WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET";
  webview_native_handle_kind2[webview_native_handle_kind2["WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER"] = 2] = "WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER";
})(webview_native_handle_kind ||= {});

class JsCallback extends lib.JsCallback {
  ptr;
  uid;
  constructor(w, cbKind, callback) {
    if (cbKind === "bind")
      callback = wrapBindCb(w, callback);
    if (cbKind === "dispatch")
      callback = wrapDispatchCb(callback);
    super(callback);
    if (cbKind === "bind")
      this.ptr = super.bindPtr;
    if (cbKind === "dispatch")
      this.ptr = super.dispatchPtr;
    this.uid = super.cbUid;
  }
  close() {
    if (this.ptr === null)
      return;
    this.ptr = null;
    super.cleanup();
  }
  arg = (arg) => {
    const argId = super.argId;
    if (argId === null)
      return isClosing(this.uid);
    super.setArg({ arg }, argId);
    return { cbUid: this.uid, argId };
  };
}
function wrapDispatchCb(callback) {
  return function(w, arg) {
    callback(w, arg);
  };
}
function wrapBindCb(w, callback) {
  return function(id, req, arg) {
    try {
      let processResult = function(result2) {
        result2 = typeof result2 !== "undefined" ? JSON.stringify(result2) : "";
        wv.webview_return(w, id, 0, result2);
      };
      const args = JSON.parse(req);
      let result = callback(...args, arg);
      const isPromise = result instanceof Promise;
      if (!isPromise)
        return processResult(result);
      result = result.then((result2) => processResult).catch((err) => {
        throw err;
      });
    } catch (err) {
      err = typeof err !== "undefined" ? JSON.stringify(err) : "";
      wv.webview_return(w, id, 1, err);
    }
  };
}
function isClosing(cbUid) {
  const message = `A cb function was called after it's native instance (cbUid: ${cbUid}) was closed`;
  console.warn(message);
  return { cbUid, argId: null };
}
var JsCallback_default = wv;
export {
  webview_native_handle_kind,
  webview_hint,
  JsCallback_default as default,
  JsCallback
};
