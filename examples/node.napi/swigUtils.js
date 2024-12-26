import { createRequire } from "node:module";
var __require = /* @__PURE__ */ createRequire(import.meta.url);

// swigUtils.ts
var libwebview = __require("./webview.napi.node");
var webview_hint;
((webview_hint2) => {
  webview_hint2[webview_hint2["WEBVIEW_HINT_NONE"] = libwebview.WEBVIEW_HINT_NONE] = "WEBVIEW_HINT_NONE";
  webview_hint2[webview_hint2["WEBVIEW_HINT_MIN"] = libwebview.WEBVIEW_HINT_MIN] = "WEBVIEW_HINT_MIN";
  webview_hint2[webview_hint2["WEBVIEW_HINT_MAX"] = libwebview.WEBVIEW_HINT_MAX] = "WEBVIEW_HINT_MAX";
  webview_hint2[webview_hint2["WEBVIEW_HINT_FIXED"] = libwebview.WEBVIEW_HINT_FIXED] = "WEBVIEW_HINT_FIXED";
})(webview_hint ||= {});

class JSCallback extends libwebview.JSCallback {
  ptr;
  constructor(callback) {
    super(callback);
    this.ptr = super.getPointer();
  }
  close() {
    this.ptr = null;
    super.free();
  }
}
var swigUtils_default = libwebview;
export {
  webview_hint,
  swigUtils_default as default,
  JSCallback
};
