# Changelog

### Migrating from v0.1.1 to v0.10.0

#### Go

1. `Webview.Open()` has been removed. Use other webview APIs to create a window, open a link and run main UI loop.
2. `Webview.Debug()` and `webview.Debugf()` have been removed. Use your favorite logging library to debug webview apps.
3. `Webview.Settings` struct has been removed. Title, URL and size are controlled via other API setters and can be updated at any time, not only when webview is created.
4. `Webview.Loop()` has been removed. Use `Run()` instead.
5. `WebView.Run()`, `WebView.Terminate()`, `WebView.SetTitle()`, `WebView.Dispatch()` stayed the same.
6. `WebView.Exit()` has been renamed to `WebView.Destroy()`
7. `WebView.SetColor()` and `WebView.SetFullScreen()` have been removed. Use `Window()` to get native window handle and probably write some Cgo code to adjust native window to your taste.
8. `Webview.Dialog` has been removed. But it is likely to be brought back as a standalone module.
9. `WebView.Eval()` remained the same.
10. `WebView.InjectCSS()` has been removed. Use eval to inject style tag with CSS inside.
11. `WebView.Bind()` kept the name, but changed the semantics. Only functions can be bound. Not the structs, like in Lorca.

#### C/C++

1. Use opaque `webview_t` type instead of `struct webview`. Size, title and URL are controlled via API setter functions. Invoke callback has been replaced with `webview_bind()` and `webview_return()` to make native function bindings inter-operate with JS.
2. If you have been using simplified `webview()` API to only open a single URL
   in a webview window - this function has been removed. You now have to create
   a new webview instance, configure and run it explicitly.
3. `webview_init()` is replaced by `webview_create()` which creates a new webview instance.
4. `webview_exit()` has been replaced with more meaningful `webview_destroy()`.
5. Main UI loop with `webview_loop()` inside has been replaced with `webview_run()` runs infinitely until the webview window is closed.
6. `webview_terminate()` remains the same.
7. `webview_dispatch()` remains the same.
8. `webview_set_title()` remains the same.
9. `webview_set_color()` has been removed. Use `webview_get_window` and native
   window APIs to control colors, transparency and other native window
   properties. At some point these APIs might be brought back.
10. `webview_set_fullscreen()` has been removed, see above.
11. `webview_dialog()` has been removed. But I'd like to see it added back as a separate independent module or library.
12. `webview_eval()` remains the same.
13. `webview_inject_css()` has been removed. Use `webview_eval()` to create style tag manually.
14. `webview_debug()` has been removed. Use whatever fits best to your programming language and environment to debug your GUI apps.
