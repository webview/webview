package webview

/*
#cgo linux openbsd freebsd CXXFLAGS: -DWEBVIEW_GTK
#cgo linux openbsd freebsd pkg-config: gtk+-3.0 webkit2gtk-4.0

#cgo darwin CXXFLAGS: -DWEBVIEW_COCOA
#cgo darwin LDFLAGS: -framework WebKit

#cgo windows CXXFLAGS: -DWEBVIEW_MSHTML
#cgo windows LDFLAGS: -lole32 -lcomctl32 -loleaut32 -luuid -lgdi32

#define WEBVIEW_HEADER
#include "webview.h"

#include <stdlib.h>
*/
import "C"
import (
	"runtime"
	"unsafe"
)

func init() {
	// Ensure that main.main is called from the main thread
	runtime.LockOSThread()
}

type WebView interface {
	Run()
	Terminate()
	Navigate(url string)
	SetTitle(title string)
	Window() unsafe.Pointer
	Init(js string)
	Eval(js string)
	Destroy()
	/*
		SetBounds(x, y, width, height int)
		Bounds() (x, y, width, height int)
		Dispatch(f func())
		Bind(name string, v interface{})
	*/
}

type webview struct {
	w C.webview_t
}

func boolToInt(b bool) C.int {
	if b {
		return 1
	}
	return 0
}

func New(debug bool) WebView { return NewWindow(debug, nil) }

func NewWindow(debug bool, window unsafe.Pointer) WebView {
	w := &webview{}
	w.w = C.webview_create(boolToInt(debug), window)
	return w
}

func (w *webview) Destroy() {
	C.webview_destroy(w.w)
}

func (w *webview) Run() {
	C.webview_run(w.w)
}

func (w *webview) Terminate() {
	C.webview_terminate(w.w)
}

func (w *webview) Window() unsafe.Pointer {
	return C.webview_get_window(w.w)
}

func (w *webview) Navigate(url string) {
	s := C.CString(url)
	defer C.free(unsafe.Pointer(s))
	C.webview_navigate(w.w, s)
}

func (w *webview) SetTitle(title string) {
	s := C.CString(title)
	defer C.free(unsafe.Pointer(s))
	C.webview_set_title(w.w, s)
}

func (w *webview) Init(js string) {
	s := C.CString(js)
	defer C.free(unsafe.Pointer(s))
	C.webview_init(w.w, s)
}

func (w *webview) Eval(js string) {
	s := C.CString(js)
	defer C.free(unsafe.Pointer(s))
	C.webview_eval(w.w, s)
}
