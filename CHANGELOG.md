### v0.10.0 release

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