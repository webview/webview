# Migration

## v0.11.0 to v0.12.0

Some CMake targets names have been replaced in order to match the exported/installed target names:

Old target         | Replacement
----------         | -----------
`webview::headers` | `webview::core`
`webview::shared`  | `webview::core_shared`
`webview::static`  | `webview::core_static`

## v0.10.0 to v0.11.0

### New Defaults for `WEBVIEW_API`

Language | Old default | New default
-------- | ----------- | -----------
C++      | `extern`    | `inline`
C        | `extern`    | `extern`

If you relied on the old default being `extern` when using a C++ compiler then you should either define `WEBVIEW_STATIC` or `WEBVIEW_API=extern`.

### Behavior of `webview_return()`

`webview_return()` no longer evaluates the passed-in result as JavaScript but instead parses it as JSON. The new behavior is consistent with the documented behavior while the old behavior wasn't. Use `webview_eval()` if you need to evaluate JavaScript code.

### Application Lifecycle

If you relied on the library's management of the application lifecycle when passing in an existing window to the library then you should now either manage the lifecycle by yourself or let the library create the window.

Specific things that are now only done by the library when the window is created for you:

Platform | What
-------- | ----
Linux    | Call to `gtk_init_check()`.
Windows  | COM initialization and DPI awareness enablement.

## v0.1.1 to v0.10.0

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
