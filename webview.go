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

extern void _webview_external_invoke_callback(void *, void *);

static inline void CgoWebViewFree(void *w) {
	free((void *)((struct webview *)w)->title);
	free((void *)((struct webview *)w)->url);
	free(w);
}

static inline void *CgoWebViewCreate(int width, int height, char *title, char *url, int resizable) {
	struct webview *w = (struct webview *) malloc(sizeof(*w));
	w->width = width;
	w->height = height;
	w->title = title;
	w->url = url;
	w->resizable = resizable;
	w->external_invoke_cb = (webview_external_invoke_cb_t) _webview_external_invoke_callback;
	if (webview_init(w) != 0) {
		CgoWebViewFree(w);
		return NULL;
	}
	return (void *)w;
}

static inline int CgoWebViewLoop(void *w, int blocking) {
	return webview_loop((struct webview *)w, blocking);
}

static inline void CgoWebViewTerminate(void *w) {
	webview_terminate((struct webview *)w);
}

static inline void CgoWebViewExit(void *w) {
	webview_exit((struct webview *)w);
}

static inline int CgoWebViewEval(void *w, char *js) {
	return webview_eval((struct webview *)w, js);
}

extern void _webview_dispatch_go_callback(void *);
static inline void _webview_dispatch_cb(struct webview *w, void *arg) {
	_webview_dispatch_go_callback(arg);
}
static inline void CgoWebViewDispatch(void *w, void *arg) {
	webview_dispatch((struct webview *)w, _webview_dispatch_cb, arg);
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
	cbs   = map[WebView]ExternalInvokeCallbackFunc{}
)

type webview struct {
	w unsafe.Pointer
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
	resizable := 0
	if settings.Resizable {
		resizable = 1
	}
	w := &webview{}
	w.w = C.CgoWebViewCreate(C.int(settings.Width), C.int(settings.Height), C.CString(settings.Title), C.CString(settings.URL), C.int(resizable))
	if settings.ExternalInvokeCallback != nil {
		m.Lock()
		cbs[w] = settings.ExternalInvokeCallback
		m.Unlock()
	}
	return w
}

func (w *webview) Loop(blocking bool) bool {
	block := C.int(0)
	if blocking {
		block = 1
	}
	return C.CgoWebViewLoop(w.w, block) == 0
}

func (w *webview) Run() {
	for w.Loop(true) {
	}
}

func (w *webview) Exit() {
	C.CgoWebViewExit(w.w)
}

func (w *webview) Dispatch(f func()) {
	m.Lock()
	for ; fns[index] != nil; index++ {
	}
	fns[index] = f
	m.Unlock()
	C.CgoWebViewDispatch(w.w, unsafe.Pointer(index))
	m.Lock()
	delete(fns, index)
	m.Unlock()
}

func (w *webview) Eval(js string) {
	p := C.CString(js)
	defer C.free(unsafe.Pointer(p))
	C.CgoWebViewEval(w.w, p)
}

func (w *webview) Terminate() {
	C.CgoWebViewTerminate(w.w)
}

//export _webview_dispatch_go_callback
func _webview_dispatch_go_callback(index unsafe.Pointer) {
	m.Lock()
	defer m.Unlock()
	fns[uintptr(index)]()
}

//export _webview_external_invoke_callback
func _webview_external_invoke_callback(w unsafe.Pointer, data unsafe.Pointer) {
	m.Lock()
	var (
		cb ExternalInvokeCallbackFunc
		wv WebView
	)
	for wv, cb = range cbs {
		if wv.(*webview).w == w {
			break
		}
	}
	m.Unlock()
	cb(wv, C.GoString((*C.char)(data)))
}


