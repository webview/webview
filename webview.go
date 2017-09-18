package webview

/*
#cgo linux CFLAGS: -DWEBVIEW_GTK=1
#cgo linux pkg-config: gtk+-3.0 webkitgtk-3.0

#cgo windows CFLAGS: -DWEBVIEW_WINAPI=1
#cgo windows LDFLAGS: -lole32 -lcomctl32 -loleaut32 -luuid -mwindows

#cgo darwin CFLAGS: -DWEBVIEW_COCOA=1 -x objective-c
#cgo darwin LDFLAGS: -framework Cocoa -framework WebKit

#include <stdlib.h>
#include "webview.h"

extern void _webview_external_invoke_callback(struct webview *, char *);
extern void _webview_dispatch_go_callback(void *);
static void _webview_dispatch_cb(struct webview *w, void *arg) {
	_webview_dispatch_go_callback(arg);
}
static inline void WebviewDispatchBridge(struct webview *w, void *arg) {
	webview_dispatch(w, _webview_dispatch_cb, arg);
}
*/
import "C"
import (
	"errors"
	"sync"
	"unsafe"
)

func Open(title, url string, w, h int, resizable bool) error {
	titleStr := C.CString(title)
	defer C.free(unsafe.Pointer(titleStr))
	urlStr := C.CString(url)
	defer C.free(unsafe.Pointer(urlStr))
	resize := C.int(0)
	if resizable {
		resize = C.int(1)
	}

	r := C.webview(titleStr, urlStr, C.int(w), C.int(h), resize)
	if r != 0 {
		return errors.New("failed to create webview")
	}
	return nil
}

type ExternalInvokeCallbackFunc func(w WebView, data string)

type Settings struct {
	Title                  string
	URL                    string
	Width                  int
	Height                 int
	Resizable              bool
	ExternalInvokeCallback ExternalInvokeCallbackFunc
}

type WebView interface {
	Run()
	Loop(blocking bool) bool
	Eval(js string)
	Dispatch(func())
	Exit()
	Terminate()
}

var (
	m     sync.Mutex
	index uintptr
	fns   = map[uintptr]func(){}
	cbs   = map[*webview]ExternalInvokeCallbackFunc{}
)

type webview struct {
	w C.struct_webview
}

var _ WebView = &webview{}

func New(settings Settings) *webview {
	if settings.Width == 0 {
		settings.Width = 640
	}
	if settings.Height == 0 {
		settings.Height = 480
	}
	if settings.Title == "" {
		settings.Title = "WebView"
	}
	if settings.URL == "" {
		return nil
	}
	w := &webview{}
	w.w.width = C.int(settings.Width)
	w.w.height = C.int(settings.Height)
	w.w.title = C.CString(settings.Title)
	w.w.url = C.CString(settings.URL)
	if settings.Resizable {
		w.w.resizable = 1
	} else {
		w.w.resizable = 0
	}
	if settings.ExternalInvokeCallback != nil {
		w.w.external_invoke_cb = (C.webview_external_invoke_cb_t)(unsafe.Pointer(C._webview_external_invoke_callback))
		m.Lock()
		cbs[w] = settings.ExternalInvokeCallback
		m.Unlock()
	}
	if r := C.webview_init(&w.w); r != 0 {
		return nil
	}
	return w
}

func (w *webview) Loop(blocking bool) bool {
	block := C.int(0)
	if blocking {
		block = 1
	}
	r := C.webview_loop(&w.w, block)
	return r == 0
}

func (w *webview) Run() {
	for w.Loop(true) {
	}
}

func (w *webview) Exit() {
	C.webview_exit(&w.w)
	C.free(unsafe.Pointer(w.w.title))
	C.free(unsafe.Pointer(w.w.url))
}

func (w *webview) Dispatch(f func()) {
	m.Lock()
	for ; fns[index] != nil; index++ {
	}
	fns[index] = f
	m.Unlock()
	C.WebviewDispatchBridge(&w.w, unsafe.Pointer(index))
	m.Lock()
	delete(fns, index)
	m.Unlock()
}

//export _webview_dispatch_go_callback
func _webview_dispatch_go_callback(index unsafe.Pointer) {
	m.Lock()
	defer m.Unlock()
	fns[uintptr(index)]()
}

//export _webview_external_invoke_callback
func _webview_external_invoke_callback(w *C.struct_webview, data *C.char) {
	m.Lock()
	var (
		cb ExternalInvokeCallbackFunc
		wv WebView
	)
	for wv, cb = range cbs {
		if &wv.(*webview).w == w {
			break
		}
	}
	m.Unlock()
	cb(wv, C.GoString(data))
}

func (w *webview) Eval(js string) {
	p := C.CString(js)
	defer C.free(unsafe.Pointer(p))
	C.webview_eval(&w.w, p)
}

func (w *webview) Terminate() {
	C.webview_terminate(&w.w)
}
