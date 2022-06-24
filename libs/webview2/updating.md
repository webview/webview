# Updating WebView2

Run `update.bat VERSION` where `VERSION` is either the version of the `Microsoft.Web.WebView2` NuGet package or `latest`.

The `include` directory should be used as a header search path and the generated `select_webview2.h` header is the header to `#include` in order to include the versioned WebView2 header when `WEBVIEW_USE_EMBEDDED_WEBVIEW2` is defined.

There are a few customzation points:

* Define `WEBVIEW_NO_EMBEDDED_WEBVIEW2` to prevent inclusion of the embedded WebView2 header.
* Define `WEBVIEW_EMBEDDED_WEBVIEW2_HEADER` with a custom path to `WebView2.h`.
