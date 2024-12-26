import lib, { JSCallback, webview_hint } from "./swigUtils.js";

const w = lib.webview_create(1, null);

lib.webview_set_title(w, "Hello Webview");
lib.webview_set_html(w, "<html>Hello Webview</html>");
lib.webview_set_size(w, 1200, 1200, webview_hint.WEBVIEW_HINT_MIN);

const callback = new JSCallback((id, userArg) => {
  console.log({ id, userArg });
});
const callbackPtr = callback.ptr;

lib.webview_dispatch(w, callbackPtr, null);

lib.webview_run(w);
