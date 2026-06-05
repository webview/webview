%module webview 
%{
#include "webview.h"
%}
#define WEBVIEW_HINT_NONE 0
#define WEBVIEW_HINT_MIN 1
#define WEBVIEW_HINT_MAX 2
#define WEBVIEW_HINT_FIXED 3
typedef void *webview_t;
extern webview_t webview_create(int debug, void *window);
extern void webview_destroy(webview_t w);
extern void webview_run(webview_t w);
extern void webview_terminate(webview_t w);
extern void webview_dispatch(webview_t w, void (*fn)(webview_t w, void *arg), void *arg);
extern void *webview_get_window(webview_t w);
extern void *webview_get_native_handle(webview_t w, webview_native_handle_kind_t kind);
extern void webview_set_title(webview_t w, const char *title);
extern void webview_set_size(webview_t w, int width, int height, webview_hint_t hints);
extern void webview_navigate(webview_t w, const char *url);
extern void webview_set_html(webview_t w, const char *html);
extern void webview_init(webview_t w, const char *js);
extern void webview_eval(webview_t w, const char *js);
extern void webview_bind(webview_t w, const char *name, void (*fn)(const char *seq, const char *req, void *arg), void *arg);
extern void webview_unbind(webview_t w, const char *name);
extern void webview_return(webview_t w, const char *seq, int status, const char *result);