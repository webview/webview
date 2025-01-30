"use strict";
const libWv = require("bindings")("./webview.napi.node");
/** Size hints for use with `webview_set_size` */
var webview_hint;
(function (webview_hint) {
    webview_hint[webview_hint["WEBVIEW_HINT_NONE"] = 0] = "WEBVIEW_HINT_NONE";
    webview_hint[webview_hint["WEBVIEW_HINT_MIN"] = 1] = "WEBVIEW_HINT_MIN";
    webview_hint[webview_hint["WEBVIEW_HINT_MAX"] = 2] = "WEBVIEW_HINT_MAX";
    webview_hint[webview_hint["WEBVIEW_HINT_FIXED"] = 3] = "WEBVIEW_HINT_FIXED";
})(webview_hint || (webview_hint = {}));
/** Webview handle kinds for use with `webview_get_native_handle` */
var webview_native_handle_kind;
(function (webview_native_handle_kind) {
    webview_native_handle_kind[webview_native_handle_kind["WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW"] = 0] = "WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW";
    webview_native_handle_kind[webview_native_handle_kind["WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET"] = 1] = "WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET";
    webview_native_handle_kind[webview_native_handle_kind["WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER"] = 2] = "WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER";
})(webview_native_handle_kind || (webview_native_handle_kind = {}));
module.exports = {
    libWv,
    webview_hint,
    webview_native_handle_kind,
};
