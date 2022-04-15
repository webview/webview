# webview

[![Join the chat at https://gitter.im/zserge/webview](https://badges.gitter.im/zserge/webview.svg)](https://gitter.im/zserge/webview?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://img.shields.io/github/workflow/status/webview/webview/CI%20Pipeline)](https://github.com/webview/webview)
[![GoDoc](https://godoc.org/github.com/webview/webview?status.svg)](https://godoc.org/github.com/webview/webview)
[![Go Report Card](https://goreportcard.com/badge/github.com/webview/webview)](https://goreportcard.com/report/github.com/webview/webview)

A tiny cross-platform webview library for C/C++/Go to build modern cross-platform GUIs.

The goal of the project is to create a common HTML5 UI abstraction layer for the most widely used platforms.

It supports two-way JavaScript bindings (to call JavaScript from C/C++/Go and to call C/C++/Go from JavaScript).

It uses Cocoa/WebKit on macOS, gtk-webkit2 on Linux and Edge on Windows 10.

## Bindings

This repository contains bindings for C, C++, and Go. Bindings for other languages are maintained separately.

* [Rust](https://github.com/Boscop/webview-rs)
* [Python](https://github.com/zserge/webview-python)
* [Nim](https://github.com/oskca/webview)
* [Haskell](https://github.com/lettier/webviewhs)
* [C#](https://github.com/webview/webview_csharp)
* [Janet](https://github.com/janet-lang/webview)
* [Crystal](https://github.com/naqvis/webview)
* [Java](https://github.com/shannah/webviewjar)
* [Ruby](https://github.com/Maaarcocr/webview_ruby)
* [Pascal](http://github.com/PierceNg/fpwebview)

Instructions for [Go](#webview-for-go-developers) and [C/C++](#webview-for-cc-developers) are included below.

## Webview for Go developers

### Getting started

Install this library with `go get`:

```sh
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

Build the app:

```sh
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

For more details see [godoc](https://godoc.org/github.com/webview/webview).

### Distributing webview apps

On Linux you get a standalone executable. It depends on GTK3 and GtkWebkit2. Include those dependencies if you distribute in a package like DEB or RPM. An application icon can be specified by providing a `.desktop` file.

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

Here, `Info.plist` is a [property list file](https://developer.apple.com/library/content/documentation/General/Reference/InfoPlistKeyReference/Articles/AboutInformationPropertyListFiles.html) and `*.icns` is a special icon format. You can convert PNG to icns [online](https://iconverticons.com/online/) or with a tool like `icnsutils`.

On Windows, you can use a custom icon by providing a resource file, compiling it and linking with it. Typically, `windres` is used to compile resources.

Also, on Windows, `webview.dll` and `WebView2Loader.dll` must be placed into the same directory with the app executable.

To cross-compile a webview app - use [xgo](https://github.com/karalabe/xgo).

### Known issues

#### Accessing localhost on Windows

If Edge (Chromium) isn't installed on the target machine, webview will use a UWP application context which disallows loopback by default. To enable it you need to run the following command from a command prompt with admin priviledges:

```sh
CheckNetIsolation.exe LoopbackExempt -a -n="Microsoft.Win32WebViewHost_cw5n1h2txyewy"
```

For app distribution we recommend automating this in your installer.

### Migrating from v0.1.1 to v0.10.0

1. `webview.Open()` has been removed. Use other webview APIs to create a window, open a link and run main UI loop.
2. `webview.Debug()` and `webview.Debugf()` have been removed. Use your favorite logging library to debug webview apps.
3. `webview.Settings` struct has been removed. Title, URL and size are controlled via other API setters and can be updated at any time, not only when webview is created.
4. `Webview.Loop()` has been removed. Use `Run()` instead.
5. `WebView.Run()`, `WebView.Terminate()`, `WebView.SetTitle()`, `WebView.Dispatch()` stayed the same.
6. `WebView.Exit()` has been renamed to `WebView.Destroy()`
7. `WebView.SetColor()` and `WebView.SetFullScreen()` have been removed. Use `Window()` to get native window handle and probably write some Cgo code to adjust native window to your taste.
8. `webview.Dialog` has been removed. But it is likely to be brought back as a standalone module.
9. `WebView.Eval()` remained the same.
10. `WebView.InjectCSS()` has been removed. Use eval to inject style tag with CSS inside.
11. `WebView.Bind()` kept the name, but changed the semantics. Only functions can be bound. Not the structs, like in Lorca.

## Webview for C/C++ developers

Download [webview.h](https://raw.githubusercontent.com/webview/webview/master/webview.h) and include it in your C/C++ code:

### C++

```c++
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

```sh
# Linux
$ c++ main.cc `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -o webview-example
# MacOS
$ c++ main.cc -std=c++11 -framework WebKit -o webview-example
# Windows (x64)
$ c++ main.cc -mwindows -L./dll/x64 -lwebview -lWebView2Loader -o webview-example.exe
```

### C

```c
// main.c
#include "webview.h"
#include <stddef.h>

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

Define C++ flags for the platform:

```sh
# Linux
$ CPPFLAGS="`pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0` -lstdc++"
# MacOS
$ CPPFLAGS="-std=c++11 -framework WebKit"
# Windows (x64)
$ CPPFLAGS="-mwindows -L./dll/x64 -lwebview -lWebView2Loader"
```

Build it:

```sh
$ g++ -c $CPPFLAGS webview.cc -o webview.o  # build webview
$ gcc -c main.c -o main.o  # build C program
$ g++ main.o webview.o $CPPFLAGS -o webview-example  # link them together
```

For a complete C example see: https://github.com/webview/webview_c


On Windows it is possible to use webview library directly when compiling with cl.exe, but WebView2Loader.dll is still required. To use MinGW you may dynamically link the prebuilt webview.dll (this approach is used in Cgo bindings).

Full C/C++ API is described at the top of `webview.h`.

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

- A webview is not a full web browser and thus does not support `alert`, `confirm` and `prompt` dialogs. Additionally, `console.*` methods are not supported.
- Ubuntu users need to install the `webkit2gtk-4.0` as development dependency via `sudo apt install webkit2gtk-4.0`. If the package can't be found `webkit2gtk-4.0-dev` may be used instead.
- FreeBSD is also supported via webkit2 which may be installed by running `pkg install webkit2-gtk3`.
- Execution on OpenBSD requires `wxallowed` [mount(8)](https://man.openbsd.org/mount.8) option.
- Calling `Eval()` or `Dispatch()` before `Run()` does not work, because the webview instance has only been configured, but not started yet. 

## License

Code is distributed under MIT license, feel free to use it in your proprietary
projects as well.
