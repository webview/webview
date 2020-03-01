package webview

/*
#cgo linux openbsd freebsd CXXFLAGS: -DWEBVIEW_GTK -std=c++11
#cgo linux openbsd freebsd pkg-config: gtk+-3.0 webkit2gtk-4.0

#cgo darwin CXXFLAGS: -DWEBVIEW_COCOA -std=c++11
#cgo darwin LDFLAGS: -framework WebKit

#cgo windows CXXFLAGS: -std=c++11
#cgo windows,amd64 LDFLAGS: -L./dll/x64 -lwebview -lWebView2Loader
#cgo windows,386 LDFLAGS: -L./dll/x86 -lwebview -lWebView2Loader

#define WEBVIEW_HEADER
#include "webview.h"

#include <stdlib.h>
#include <stdint.h>

extern void _webviewDispatchGoCallback(void *);
static inline void _webview_dispatch_cb(webview_t w, void *arg) {
	_webviewDispatchGoCallback(arg);
}
static inline void CgoWebViewDispatch(webview_t w, uintptr_t arg) {
	webview_dispatch(w, _webview_dispatch_cb, (void *)arg);
}

struct binding_context {
	webview_t w;
	uintptr_t index;
};
extern void _webviewBindingGoCallback(webview_t, char *, char *, uintptr_t);
static inline void _webview_binding_cb(const char *id, const char *req, void *arg) {
	struct binding_context *ctx = (struct binding_context *) arg;
	_webviewBindingGoCallback(ctx->w, (char *)id, (char *)req, ctx->index);
}
static inline void CgoWebViewBind(webview_t w, const char *name, uintptr_t index) {
	struct binding_context *ctx = calloc(1, sizeof(struct binding_context));
	ctx->w = w;
	ctx->index = index;
	webview_bind(w, name, _webview_binding_cb, (void *)ctx);
}
*/
import "C"
import (
	"encoding/json"
	"errors"
	"reflect"
	"runtime"
	"sync"
	"unsafe"
)

func init() {
	// Ensure that main.main is called from the main thread
	runtime.LockOSThread()
}

type Hint int

const (
	HintNone  = C.WEBVIEW_HINT_NONE
	HintFixed = C.WEBVIEW_HINT_FIXED
	HintMin   = C.WEBVIEW_HINT_MIN
	HintMax   = C.WEBVIEW_HINT_MAX
)

type WebView interface {
	Run()
	Terminate()
	Dispatch(f func())
	Destroy()
	Window() unsafe.Pointer
	SetTitle(title string)
	SetSize(w int, h int, hint Hint)
	Navigate(url string)
	Init(js string)
	Eval(js string)
	Bind(name string, v interface{}) error
}

type webview struct {
	w C.webview_t
}

var (
	m        sync.Mutex
	index    uintptr
	dispatch = map[uintptr]func(){}
	bindings = map[uintptr]func(id, req string) (interface{}, error){}
)

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

func (w *webview) SetSize(width int, height int, hint Hint) {
	C.webview_set_size(w.w, C.int(width), C.int(height), C.int(hint))
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

func (w *webview) Dispatch(f func()) {
	m.Lock()
	for ; dispatch[index] != nil; index++ {
	}
	dispatch[index] = f
	m.Unlock()
	C.CgoWebViewDispatch(w.w, C.uintptr_t(index))
}

//export _webviewDispatchGoCallback
func _webviewDispatchGoCallback(index unsafe.Pointer) {
	m.Lock()
	f := dispatch[uintptr(index)]
	delete(dispatch, uintptr(index))
	m.Unlock()
	f()
}

//export _webviewBindingGoCallback
func _webviewBindingGoCallback(w C.webview_t, id *C.char, req *C.char, index uintptr) {
	m.Lock()
	f := bindings[uintptr(index)]
	delete(bindings, uintptr(index))
	m.Unlock()
	jsString := func(v interface{}) string { b, _ := json.Marshal(v); return string(b) }
	status, result := 0, ""
	if res, err := f(C.GoString(id), C.GoString(req)); err != nil {
		status = -1
		result = jsString(err.Error())
	} else if b, err := json.Marshal(res); err != nil {
		status = -1
		result = jsString(err.Error())
	} else {
		status = 0
		result = string(b)
	}
	s := C.CString(result)
	defer C.free(unsafe.Pointer(s))
	C.webview_return(w, id, C.int(status), s)
}

func (w *webview) Bind(name string, f interface{}) error {
	v := reflect.ValueOf(f)
	// f must be a function
	if v.Kind() != reflect.Func {
		return errors.New("only functions can be bound")
	}
	// f must return either value and error or just error
	if n := v.Type().NumOut(); n > 2 {
		return errors.New("function may only return a value or a value+error")
	}

	binding := func(id, req string) (interface{}, error) {
		raw := []json.RawMessage{}
		if err := json.Unmarshal([]byte(req), &raw); err != nil {
			return nil, err
		}
		if len(raw) != v.Type().NumIn() {
			return nil, errors.New("function arguments mismatch")
		}
		args := []reflect.Value{}
		for i := range raw {
			arg := reflect.New(v.Type().In(i))
			if err := json.Unmarshal(raw[i], arg.Interface()); err != nil {
				return nil, err
			}
			args = append(args, arg.Elem())
		}
		errorType := reflect.TypeOf((*error)(nil)).Elem()
		res := v.Call(args)
		switch len(res) {
		case 0:
			// No results from the function, just return nil
			return nil, nil
		case 1:
			// One result may be a value, or an error
			if res[0].Type().Implements(errorType) {
				if res[0].Interface() != nil {
					return nil, res[0].Interface().(error)
				}
				return nil, nil
			}
			return res[0].Interface(), nil
		case 2:
			// Two results: first one is value, second is error
			if !res[1].Type().Implements(errorType) {
				return nil, errors.New("second return value must be an error")
			}
			if res[1].Interface() == nil {
				return res[0].Interface(), nil
			}
			return res[0].Interface(), res[1].Interface().(error)
		default:
			return nil, errors.New("unexpected number of return values")
		}
	}

	m.Lock()
	for ; bindings[index] != nil; index++ {
	}
	bindings[index] = binding
	m.Unlock()
	cname := C.CString(name)
	defer C.free(unsafe.Pointer(cname))
	C.CgoWebViewBind(w.w, cname, C.uintptr_t(index))
	return nil
}
