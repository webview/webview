const libWv = require("bindings")("./webview.napi.node");

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

module.exports = {
  libWv,
  webview_hint,
  webview_native_handle_kind,
};
