# webview

[![Join the chat at https://gitter.im/zserge/webview](https://badges.gitter.im/zserge/webview.svg)](https://gitter.im/zserge/webview?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://img.shields.io/github/workflow/status/zserge/webview/CI%20Pipeline)](https://github.com/zserge/webview)
[![GoDoc](https://godoc.org/github.com/zserge/webview?status.svg)](https://godoc.org/github.com/zserge/webview)
[![Go Report Card](https://goreportcard.com/badge/github.com/zserge/webview)](https://goreportcard.com/report/github.com/zserge/webview)


A tiny cross-platform webview library for C/C++/Golang to build modern cross-platform GUIs. Also, there are [Rust bindings](https://github.com/Boscop/webview-rs), [Python bindings](https://github.com/zserge/webview-python), [Nim bindings](https://github.com/oskca/webview), [Haskell](https://github.com/lettier/webviewhs), [C# bindings](https://github.com/webview/webview_csharp) and [Java bindings](https://github.com/shannah/webviewjar) available.

The goal of the project is to create a common HTML5 UI abstraction layer for the most widely used platforms. 

It supports two-way JavaScript bindings (to call JavaScript from C/C++/Go and to call C/C++/Go from JavaScript).

It uses Cocoa/WebKit on macOS, gtk-webkit2 on Linux and Edge on Windows 10.

## Webview for Go developers

If you are interested in writing Webview apps in C/C++, [skip to the next section](#webview-for-cc-developers).

### Getting started

Install Webview library with `go get`:

```
$ go get github.com/webview/webview
```

Import the package and start using it:

```go
package main

import "github.com/webview/webview"

func main() {
	debug := true
	w := webview.New(debug)
	defer w.Destroy()
	w.SetTitle("Minimal webview example")
	w.SetSize(800, 600, webview.HintNone)
	w.Navigate("https://en.m.wikipedia.org/wiki/Main_Page")
	w.Run()
}
```

To build the app use the following commands:

```bash
# Linux
$ go build -o webview-example && ./webview-example

# MacOS uses app bundles for GUI apps
$ mkdir -p example.app/Contents/MacOS
$ go build -o example.app/Contents/MacOS/example
$ open example.app # Or click on the app in Finder

# Windows requires special linker flags for GUI apps.
# It's also recommended to use TDM-GCC-64 compiler for CGo.
# http://tdm-gcc.tdragon.net/download
$ go build -ldflags="-H windowsgui" -o webview-example.exe
```

For more details see [godoc](https://godoc.org/github.com/zserge/webview).

### Distributing webview apps

On Linux you get a standalone executable. It will depend on GTK3 and GtkWebkit2, so if you distribute your app in DEB or RPM format include those dependencies. An application icon can be specified by providing a `.desktop` file.

On MacOS you are likely to ship an app bundle. Make the following directory structure and just zip it:

```
example.app
└── Contents
    ├── Info.plist
    ├── MacOS
    |   └── example
    └── Resources
        └── example.icns
```

Here, `Info.plist` is a [property list file](https://developer.apple.com/library/content/documentation/General/Reference/InfoPlistKeyReference/Articles/AboutInformationPropertyListFiles.html) and `*.icns` is a special icon format. You may convert PNG to icns [online](https://iconverticons.com/online/).

On Windows you probably would like to have a custom icon for your executable. It can be done by providing a resource file, compiling it and linking with it. Typically, `windres` utility is used to compile resources. Also, on Windows, `webview.dll` and `WebView2Loader.dll` must be placed into the same directory with your app executable.

Also, if you want to cross-compile your webview app - use [xgo](https://github.com/karalabe/xgo).

### Migrating from v0.1.1 to v0.10.0

1. `webview.Open()` has been removed. Use other webview APIs to create a window, open a link and run main UI loop.
2. `webview.Debug()` and `webview.Debugf()` have been removed. Use your favorite logging library to debug webview apps.
3. `webview.Settings` struct has been removed. Title, URL and size are controlled via other API setters and can be updated at any time, not only when webview is created.
4. `Webview.Loop()` has been removed. Use `Run()` instead.
5. `WebView.Run()`, `WebView.Terminate()`, `WebView.SetTitle()`, `WebView.Dispatch()` stayed the same.
6. `WebView.Exit()` has been renamed to `WebView.Destroy()`
6. `WebView.SetColor()` and `WebView.SetFullScreen()` have been removed. Use `Window()` to get native window handle and probably write some Cgo code to adjust native window to your taste.
7. `webview.Dialog` has been removed. But it is likely to be brought back as a standalone module.
8. `WebView.Eval()` remained the same.
9. `WebView.InjectCSS()` has been removed. Use eval to inject style tag with CSS inside.
10. `WebView.Bind()` kept the name, but changed the semantics. Only functions can be bound. Not the structs, like in Lorca.

## Webview for C/C++ developers

Download [webview.h](https://raw.githubusercontent.com/zserge/webview/master/webview.h) and include it in your C/C++ code:

### C++:
```c
// main.cc
#include "webview.h"
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  webview::webview w(true, nullptr);
  w.set_title("Minimal example");
  w.set_size(480, 320, WEBVIEW_HINT_NONE);
  w.navigate("https://en.m.wikipedia.org/wiki/Main_Page");
  w.run();
  return 0;
}
```
Build it:

```bash
# Linux
$ c++ main.cc `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -o webview-example
# MacOS
$ c++ main.cc -std=c++11 -framework WebKit -o webview-example
# Windows (x64)
$ c++ main.cc -mwindows -L./dll/x64 -lwebview -lWebView2Loader -o webview-example.exe
```

### C:
```c
// main .c
#include "webview.h"
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
	webview_t w = webview_create(0, NULL);
	webview_set_title(w, "Webview Example");
	webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
	webview_navigate(w, "https://en.m.wikipedia.org/wiki/Main_Page");
	webview_run(w);
	webview_destroy(w);
	return 0;
}
```
Build it:

```bash
# Linux
$ g++ main.c `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -o webview-example
# MacOS
$ g++ main.c -std=c++11 -framework WebKit -o webview-example
# Windows (x64)
$ g++ main.c -mwindows -L./dll/x64 -lwebview -lWebView2Loader -o webview-example.exe
```

On Windows it is possible to use webview library directly when compiling with cl.exe, but WebView2Loader.dll is still required. To use MinGW you may dynamically link prebuilt webview.dll (this approach is used in Cgo bindings).

Full C/C++ API is described at the top of the `webview.h` file.

### Migrating from v0.1.1 to v0.10.0

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

## Notes

Execution on OpenBSD requires `wxallowed` [mount(8)](https://man.openbsd.org/mount.8) option.
For Ubuntu Users run `sudo apt install webkit2gtk-4.0`(Try with webkit2gtk-4.0-dev if webkit2gtk-4.0 is not found) to install webkit2gtk-4.0 related items.
FreeBSD is also supported, to install webkit2 run `pkg install webkit2-gtk3`.

## License

Code is distributed under MIT license, feel free to use it in your proprietary
projects as well.

