# webview

[![Build Status](https://travis-ci.org/zserge/webview.svg?branch=master)](https://travis-ci.org/zserge/webview)
[![Build status](https://ci.appveyor.com/api/projects/status/ksii33qx18d94h6v?svg=true)](https://ci.appveyor.com/project/zserge/webview)
[![GoDoc](https://godoc.org/github.com/zserge/webview?status.svg)](https://godoc.org/github.com/zserge/webview)
[![Go Report Card](https://goreportcard.com/badge/github.com/zserge/webview)](https://goreportcard.com/report/github.com/zserge/webview)


A tiny cross-platform webview library for C/C++/Golang.

It uses Cocoa/WebKit on macOS, gtk-webkit on Linux and good old MSHTML on Windows.

<p align="center"><img alt="linux" src="examples/todo-go/screenshots/screenshots.png"></p>

## API

WebView library prrovides C API. For Go API please see
[godoc](https://godoc.org/github.com/zserge/webview).

### Simple API

For the most simple use cases there is only one function:

```
// C (#include "webview.h")
int webview(const char *title, const char *url,	int width, int height, int resizable);
// Go (package github.com/zserge/webview)
func Open(title, url string, w, h int, resizable bool) error
```

The following URL schemes are supported:

* `http://` and `https://`, no surprises here.
* `file:///` can be useful if you want to unpack HTML/CSS assets to some
  temporary directory and point a webview to open index.html from there.
* `data:text/html,<html>...</html>` allows to pass short HTML data inline
  without using a web server or pulluting the file system. Furhter
  modifications of the webview contents can be done via JavaScript bindings.

If you write in C/C++ and have choosen a regular http URL scheme, you can use
Mongoose or any other web server/framework you like.

You may also use Go web server running on a random port:

```go
ln, err := net.Listen("tcp", "127.0.0.1:0")
if err != nil {
	log.Fatal(err)
}
defer ln.Close()
go func() {
 	// Set up your http server here
	log.Fatal(http.Serve(ln, nil))
}()
webview.Open("Hello", "http://"+ln.Addr().String(), 400, 300, false)
```

### App lifecycle API

If you want to have more control over the app you can use the following functions:

```c
  struct webview webview = {
      .title = title,
      .url = url,
      .width = w,
      .height = h,
      .resizable = resizable,
  };
  /* Create webview window using the provided options */
  webview_init(&webview);
  /* Main app loop, can be either blocking or non-blocking */
  while (webview_loop(&webview, blocking) == 0);
  /* Destroy webview window, often exits the app */
  webview_exit(&webview);

  /* To change window title later: */
  webview_set_title(&webview, "New title");

  /* To terminate the webview main loop: */
  webview_terminate(&webview);
```

### Two-way bindings API

To evaluate arbitrary javascript code use the following C function:

```c
webview_eval(&webview, "alert('hello, world');");
```

There is also a special callback (`webview.external_invoke_cb`) that can be invoked from javascript:

```javascript
// C
void my_cb(struct webview *w, const char *arg) {
	...
}

// JS (note the trailing underscore)
window.external.invoke_('some arg');
// Exactly one string argument must be provided, to pass more complex objects
// serialize them to JSON and parse it in C. To pass binary data consider using
// base64.
window.external.invoke_(JSON.stringify({fn: 'sum', x: 5, y: 3}));
```

In Go you can reduce the boilerplate by using `WebView.Bind()` method. It binds
an existing Go struct or struct pointer so that it appears under the same name
in JS, all its methods and fields are accessible directly from JS. `Bind()`
also returns a function that may be used to update the JS "mirrored" object
when you have asynchronously modified Go struct.

You may find a `counter-go` example that shows how to bind a Go "controller"
and use it from the various JavaScript UI frameworks, such as Vue.js, Preact or
Picodom.

### Multithreading support

Webview library is meant to be used from a single UI thread only. So if you
want to call `webview_eval` or `webview_terminate` from some background thread
- you have to use `webview_dispatch` to post some arbitrary function with some
context to be executed inside the main UI thread:

```c
// This function will be executed on the UI thread
void render(struct webview *w, void *arg) {
  webview_eval(w, ......);
}

// Dispatch render() function from another thread:
webview_dispatch(w, render, some_arg);
```

## License

Code is distributed under MIT license, feel free to use it in your proprietary
projects as well.

## Notes

Execution on OpenBSD requires `wxallowed` [mount(8)](https://man.openbsd.org/mount.8) option.
