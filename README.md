# webview

[![Build Status](https://travis-ci.org/zserge/webview.svg?branch=master)](https://travis-ci.org/zserge/webview)
[![Build status](https://ci.appveyor.com/api/projects/status/ksii33qx18d94h6v?svg=true)](https://ci.appveyor.com/project/zserge/webview)
[![GoDoc](https://godoc.org/github.com/zserge/webview?status.svg)](https://godoc.org/github.com/zserge/webview)
[![Go Report Card](https://goreportcard.com/badge/github.com/zserge/webview)](https://goreportcard.com/report/github.com/zserge/webview)


A tiny cross-platform webview library for C/C++/Golang to build modern cross-platform GUI.

It supports two-way JavaScript bindings (to call JavaScript from C/C++/Go and to call C/C++/Go from JavaScript).

It uses Cocoa/WebKit on macOS, gtk-webkit2 on Linux and MSHTML (IE10/11) on Windows.

<p align="center"><img alt="linux" src="examples/todo-go/screenshots/screenshots.png"></p>

## Webview for Go developers

If you are interested in writing Webview apps in C/C++ - skip to the next section.

The program is based on the original GitHub contribution code  [github.com/zserge/webview](https://github.com/zserge/webview)

### Getting started

Install Webview library with `go get`:

```
$ go get github.com/NiuStar/webview
```

Import the package and start using it:

```go
package main

import (
	"github.com/NiuStar/webview"
	"strings"
	"nqc.cn/fmt"
)

func main() {
	w = webview.New(webview.Settings{
		Width:  800,
		Height: 600,
		Title:  "Simple canvas demo",
		URL:    "https://pm.cninct.com/html/test.html",
		ExternalInvokeCallback: handleRPC,
		ExternalFinishLoadForFrameCallback:didFinishLoadForFrame,
	})
	defer w.Exit()
	//
	w.Run()
	// Open wikipedia in a 800x600 resizable window
}

func handleRPC(w webview.WebView, data string) {
	fmt.Println("data:",data)
}


func didFinishLoadForFrame(w webview.WebView) {

	fmt.Start()
	if w.IsLoading() {
		fmt.Println("didFinishLoadForFrame IsLoading")
	} else {

		fmt.Println("didFinishLoadForFrame  not IsLoading",w.LoadUrl())

		fmt.Println("Cookie:",w.GetCookie())

		cookie := w.GetCookie()

		w.ClearCache()

		list := strings.Split(cookie,"&")

		c := make(map[string]interface{})

		for _,value := range list {
			l := strings.Split(value,"=")
			if len(l) > 0 {
				c[l[0]] = l[1]
			}
		}
		if strings.EqualFold( c["name"].(string) ,"PHPSESSID") {

			PHPSESSID = c["value"].(string)
		}
		w.StringByEvaluatingJavaScriptFromString(`alert('这里是JS中alert弹出的message')`)
	}

	fmt.Over()
}
```

## News

Adding three methods

1、StringByEvaluatingJavaScriptFromString

The ObjectC call javascript.

```Go
w.StringByEvaluatingJavaScriptFromString(`alert('这里是JS中alert弹出的message')`)
```

2、ClearCache

clear the cookies and cache

```go
w.ClearCache()
```

3、GetCookie

```
		cookie := w.GetCookie()
		
		list := strings.Split(cookie,"&")

		c := make(map[string]interface{})

		for _,value := range list {
			l := strings.Split(value,"=")
			if len(l) > 0 {
				c[l[0]] = l[1]
			}
		}
		if strings.EqualFold( c["name"].(string) ,"PHPSESSID") {
			PHPSESSID = c["value"].(string)
		}
```

4、LoadUrl

5、IsLoading

6、callback method didFinishLoadForFrame

When the frame is loaded at the end of the call

7、Add alert

you can call javascript alert 



## Notes

Execution on OpenBSD requires `wxallowed` [mount(8)](https://man.openbsd.org/mount.8) option.

FreeBSD is also supported, to install webkit2 run `pkg install webkit2-gtk3`.

## License

Code is distributed under MIT license, feel free to use it in your proprietary
projects as well.
