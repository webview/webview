/*
 * MIT License
 *
 * Copyright (c) 2017 Serge Zaitsev
 * Copyright (c) 2022 Steffen André Langnes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef WEBVIEW_H
#define WEBVIEW_H

#ifndef WEBVIEW_API
#define WEBVIEW_API extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void *webview_t;

// Creates a new webview instance. If debug is non-zero - developer tools will
// be enabled (if the platform supports them). Window parameter can be a
// pointer to the native window handle. If it's non-null - then child WebView
// is embedded into the given parent window. Otherwise a new window is created.
// Depending on the platform, a GtkWindow, NSWindow or HWND pointer can be
// passed here. Returns null on failure. Creation can fail for various reasons
// such as when required runtime dependencies are missing or when window creation
// fails.
WEBVIEW_API webview_t webview_create(int debug, void *window);

// Destroys a webview and closes the native window.
WEBVIEW_API void webview_destroy(webview_t w);

// Runs the main loop until it's terminated. After this function exits - you
// must destroy the webview.
WEBVIEW_API void webview_run(webview_t w);

// Stops the main loop. It is safe to call this function from another other
// background thread.
WEBVIEW_API void webview_terminate(webview_t w);

// Posts a function to be executed on the main thread. You normally do not need
// to call this function, unless you want to tweak the native window.
WEBVIEW_API void
webview_dispatch(webview_t w, void (*fn)(webview_t w, void *arg), void *arg);

// Returns a native window handle pointer. When using GTK backend the pointer
// is GtkWindow pointer, when using Cocoa backend the pointer is NSWindow
// pointer, when using Win32 backend the pointer is HWND pointer.
WEBVIEW_API void *webview_get_window(webview_t w);

// Updates the title of the native window. Must be called from the UI thread.
WEBVIEW_API void webview_set_title(webview_t w, const char *title);

// Window size hints
#define WEBVIEW_HINT_NONE 0  // Width and height are default size
#define WEBVIEW_HINT_MIN 1   // Width and height are minimum bounds
#define WEBVIEW_HINT_MAX 2   // Width and height are maximum bounds
#define WEBVIEW_HINT_FIXED 3 // Window size can not be changed by a user
// Updates native window size. See WEBVIEW_HINT constants.
WEBVIEW_API void webview_set_size(webview_t w, int width, int height,
                                  int hints);

// Navigates webview to the given URL. URL may be a properly encoded data URI.
// Examples:
// webview_navigate(w, "https://github.com/webview/webview");
// webview_navigate(w, "data:text/html,%3Ch1%3EHello%3C%2Fh1%3E");
// webview_navigate(w, "data:text/html;base64,PGgxPkhlbGxvPC9oMT4=");
WEBVIEW_API void webview_navigate(webview_t w, const char *url);

// Set webview HTML directly.
// Example: webview_set_html(w, "<h1>Hello</h1>");
WEBVIEW_API void webview_set_html(webview_t w, const char *html);

// Injects JavaScript code at the initialization of the new page. Every time
// the webview will open a the new page - this initialization code will be
// executed. It is guaranteed that code is executed before window.onload.
WEBVIEW_API void webview_init(webview_t w, const char *js);

// Evaluates arbitrary JavaScript code. Evaluation happens asynchronously, also
// the result of the expression is ignored. Use RPC bindings if you want to
// receive notifications about the results of the evaluation.
WEBVIEW_API void webview_eval(webview_t w, const char *js);

// Binds a native C callback so that it will appear under the given name as a
// global JavaScript function. Internally it uses webview_init(). Callback
// receives a request string and a user-provided argument pointer. Request
// string is a JSON array of all the arguments passed to the JavaScript
// function.
WEBVIEW_API void webview_bind(webview_t w, const char *name,
                              void (*fn)(const char *seq, const char *req,
                                         void *arg),
                              void *arg);

// Removes a native C callback that was previously set by webview_bind.
WEBVIEW_API void webview_unbind(webview_t w, const char *name);

// Allows to return a value from the native binding. Original request pointer
// must be provided to help internal RPC engine match requests with responses.
// If status is zero - result is expected to be a valid JSON result value.
// If status is not zero - result is an error JSON object.
WEBVIEW_API void webview_return(webview_t w, const char *seq, int status,
                                const char *result);

#ifdef __cplusplus
}

#ifndef WEBVIEW_HEADER

#if !defined(WEBVIEW_GTK) && !defined(WEBVIEW_COCOA) && !defined(WEBVIEW_EDGE)
#if defined(__APPLE__)
#define WEBVIEW_COCOA
#elif defined(__unix__)
#define WEBVIEW_GTK
#elif defined(_WIN32)
#define WEBVIEW_EDGE
#else
#error "please, specify webview backend"
#endif
#endif

#ifndef WEBVIEW_DEPRECATED
#if __cplusplus >= 201402L
#define WEBVIEW_DEPRECATED(reason) [[deprecated(reason)]]
#elif defined(_MSC_VER)
#define WEBVIEW_DEPRECATED(reason) __declspec(deprecated(reason))
#else
#define WEBVIEW_DEPRECATED(reason) __attribute__((deprecated(reason)))
#endif
#endif

#ifndef WEBVIEW_DEPRECATED_PRIVATE
#define WEBVIEW_DEPRECATED_PRIVATE                                             \
  WEBVIEW_DEPRECATED("Private API should not be used")
#endif

#include <array>
#include <atomic>
#include <functional>
#include <future>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <cstring>

namespace webview {

using dispatch_fn_t = std::function<void()>;

namespace detail {

inline int json_parse_c(const char *s, size_t sz, const char *key, size_t keysz,
                        const char **value, size_t *valuesz) {
  enum {
    JSON_STATE_VALUE,
    JSON_STATE_LITERAL,
    JSON_STATE_STRING,
    JSON_STATE_ESCAPE,
    JSON_STATE_UTF8
  } state = JSON_STATE_VALUE;
  const char *k = NULL;
  int index = 1;
  int depth = 0;
  int utf8_bytes = 0;

  if (key == NULL) {
    index = keysz;
    keysz = 0;
  }

  *value = NULL;
  *valuesz = 0;

  for (; sz > 0; s++, sz--) {
    enum {
      JSON_ACTION_NONE,
      JSON_ACTION_START,
      JSON_ACTION_END,
      JSON_ACTION_START_STRUCT,
      JSON_ACTION_END_STRUCT
    } action = JSON_ACTION_NONE;
    unsigned char c = *s;
    switch (state) {
    case JSON_STATE_VALUE:
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' ||
          c == ':') {
        continue;
      } else if (c == '"') {
        action = JSON_ACTION_START;
        state = JSON_STATE_STRING;
      } else if (c == '{' || c == '[') {
        action = JSON_ACTION_START_STRUCT;
      } else if (c == '}' || c == ']') {
        action = JSON_ACTION_END_STRUCT;
      } else if (c == 't' || c == 'f' || c == 'n' || c == '-' ||
                 (c >= '0' && c <= '9')) {
        action = JSON_ACTION_START;
        state = JSON_STATE_LITERAL;
      } else {
        return -1;
      }
      break;
    case JSON_STATE_LITERAL:
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' ||
          c == ']' || c == '}' || c == ':') {
        state = JSON_STATE_VALUE;
        s--;
        sz++;
        action = JSON_ACTION_END;
      } else if (c < 32 || c > 126) {
        return -1;
      } // fallthrough
    case JSON_STATE_STRING:
      if (c < 32 || (c > 126 && c < 192)) {
        return -1;
      } else if (c == '"') {
        action = JSON_ACTION_END;
        state = JSON_STATE_VALUE;
      } else if (c == '\\') {
        state = JSON_STATE_ESCAPE;
      } else if (c >= 192 && c < 224) {
        utf8_bytes = 1;
        state = JSON_STATE_UTF8;
      } else if (c >= 224 && c < 240) {
        utf8_bytes = 2;
        state = JSON_STATE_UTF8;
      } else if (c >= 240 && c < 247) {
        utf8_bytes = 3;
        state = JSON_STATE_UTF8;
      } else if (c >= 128 && c < 192) {
        return -1;
      }
      break;
    case JSON_STATE_ESCAPE:
      if (c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f' ||
          c == 'n' || c == 'r' || c == 't' || c == 'u') {
        state = JSON_STATE_STRING;
      } else {
        return -1;
      }
      break;
    case JSON_STATE_UTF8:
      if (c < 128 || c > 191) {
        return -1;
      }
      utf8_bytes--;
      if (utf8_bytes == 0) {
        state = JSON_STATE_STRING;
      }
      break;
    default:
      return -1;
    }

    if (action == JSON_ACTION_END_STRUCT) {
      depth--;
    }

    if (depth == 1) {
      if (action == JSON_ACTION_START || action == JSON_ACTION_START_STRUCT) {
        if (index == 0) {
          *value = s;
        } else if (keysz > 0 && index == 1) {
          k = s;
        } else {
          index--;
        }
      } else if (action == JSON_ACTION_END ||
                 action == JSON_ACTION_END_STRUCT) {
        if (*value != NULL && index == 0) {
          *valuesz = (size_t)(s + 1 - *value);
          return 0;
        } else if (keysz > 0 && k != NULL) {
          if (keysz == (size_t)(s - k - 1) && memcmp(key, k + 1, keysz) == 0) {
            index = 0;
          } else {
            index = 2;
          }
          k = NULL;
        }
      }
    }

    if (action == JSON_ACTION_START_STRUCT) {
      depth++;
    }
  }
  return -1;
}

inline std::string json_escape(const std::string &s) {
  // TODO: implement
  return '"' + s + '"';
}

inline int json_unescape(const char *s, size_t n, char *out) {
  int r = 0;
  if (*s++ != '"') {
    return -1;
  }
  while (n > 2) {
    char c = *s;
    if (c == '\\') {
      s++;
      n--;
      switch (*s) {
      case 'b':
        c = '\b';
        break;
      case 'f':
        c = '\f';
        break;
      case 'n':
        c = '\n';
        break;
      case 'r':
        c = '\r';
        break;
      case 't':
        c = '\t';
        break;
      case '\\':
        c = '\\';
        break;
      case '/':
        c = '/';
        break;
      case '\"':
        c = '\"';
        break;
      default: // TODO: support unicode decoding
        return -1;
      }
    }
    if (out != NULL) {
      *out++ = c;
    }
    s++;
    n--;
    r++;
  }
  if (*s != '"') {
    return -1;
  }
  if (out != NULL) {
    *out = '\0';
  }
  return r;
}

inline std::string json_parse(const std::string &s, const std::string &key,
                              const int index) {
  const char *value;
  size_t value_sz;
  if (key == "") {
    json_parse_c(s.c_str(), s.length(), nullptr, index, &value, &value_sz);
  } else {
    json_parse_c(s.c_str(), s.length(), key.c_str(), key.length(), &value,
                 &value_sz);
  }
  if (value != nullptr) {
    if (value[0] != '"') {
      return std::string(value, value_sz);
    }
    int n = json_unescape(value, value_sz, nullptr);
    if (n > 0) {
      char *decoded = new char[n + 1];
      json_unescape(value, value_sz, decoded);
      std::string result(decoded, n);
      delete[] decoded;
      return result;
    }
  }
  return "";
}

} // namespace detail

WEBVIEW_DEPRECATED_PRIVATE
inline int json_parse_c(const char *s, size_t sz, const char *key, size_t keysz,
                        const char **value, size_t *valuesz) {
  return detail::json_parse_c(s, sz, key, keysz, value, valuesz);
}

WEBVIEW_DEPRECATED_PRIVATE
inline std::string json_escape(const std::string &s) {
  return detail::json_escape(s);
}

WEBVIEW_DEPRECATED_PRIVATE
inline int json_unescape(const char *s, size_t n, char *out) {
  return detail::json_unescape(s, n, out);
}

WEBVIEW_DEPRECATED_PRIVATE
inline std::string json_parse(const std::string &s, const std::string &key,
                              const int index) {
  return detail::json_parse(s, key, index);
}

} // namespace webview

#if defined(WEBVIEW_GTK)
//
// ====================================================================
//
// This implementation uses webkit2gtk backend. It requires gtk+3.0 and
// webkit2gtk-4.0 libraries. Proper compiler flags can be retrieved via:
//
//   pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0
//
// ====================================================================
//
#include <JavaScriptCore/JavaScript.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

namespace webview {
namespace detail {

class gtk_webkit_engine {
public:
  gtk_webkit_engine(bool debug, void *window)
      : m_window(static_cast<GtkWidget *>(window)) {
    if (gtk_init_check(0, NULL) == FALSE) {
      return;
    }
    m_window = static_cast<GtkWidget *>(window);
    if (m_window == nullptr) {
      m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    }
    g_signal_connect(G_OBJECT(m_window), "destroy",
                     G_CALLBACK(+[](GtkWidget *, gpointer arg) {
                       static_cast<gtk_webkit_engine *>(arg)->terminate();
                     }),
                     this);
    // Initialize webview widget
    m_webview = webkit_web_view_new();
    WebKitUserContentManager *manager =
        webkit_web_view_get_user_content_manager(WEBKIT_WEB_VIEW(m_webview));
    g_signal_connect(manager, "script-message-received::external",
                     G_CALLBACK(+[](WebKitUserContentManager *,
                                    WebKitJavascriptResult *r, gpointer arg) {
                       auto *w = static_cast<gtk_webkit_engine *>(arg);
                       char *s = get_string_from_js_result(r);
                       w->on_message(s);
                       g_free(s);
                     }),
                     this);
    webkit_user_content_manager_register_script_message_handler(manager,
                                                                "external");
    init("window.external={invoke:function(s){window.webkit.messageHandlers."
         "external.postMessage(s);}}");

    gtk_container_add(GTK_CONTAINER(m_window), GTK_WIDGET(m_webview));
    gtk_widget_grab_focus(GTK_WIDGET(m_webview));

    WebKitSettings *settings =
        webkit_web_view_get_settings(WEBKIT_WEB_VIEW(m_webview));
    webkit_settings_set_javascript_can_access_clipboard(settings, true);
    if (debug) {
      webkit_settings_set_enable_write_console_messages_to_stdout(settings,
                                                                  true);
      webkit_settings_set_enable_developer_extras(settings, true);
    }

    gtk_widget_show_all(m_window);
  }
  virtual ~gtk_webkit_engine() = default;
  void *window() { return (void *)m_window; }
  void run() { gtk_main(); }
  void terminate() { gtk_main_quit(); }
  void dispatch(std::function<void()> f) {
    g_idle_add_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)([](void *f) -> int {
                      (*static_cast<dispatch_fn_t *>(f))();
                      return G_SOURCE_REMOVE;
                    }),
                    new std::function<void()>(f),
                    [](void *f) { delete static_cast<dispatch_fn_t *>(f); });
  }

  void set_title(const std::string &title) {
    gtk_window_set_title(GTK_WINDOW(m_window), title.c_str());
  }

  void set_size(int width, int height, int hints) {
    gtk_window_set_resizable(GTK_WINDOW(m_window), hints != WEBVIEW_HINT_FIXED);
    if (hints == WEBVIEW_HINT_NONE) {
      gtk_window_resize(GTK_WINDOW(m_window), width, height);
    } else if (hints == WEBVIEW_HINT_FIXED) {
      gtk_widget_set_size_request(m_window, width, height);
    } else {
      GdkGeometry g;
      g.min_width = g.max_width = width;
      g.min_height = g.max_height = height;
      GdkWindowHints h =
          (hints == WEBVIEW_HINT_MIN ? GDK_HINT_MIN_SIZE : GDK_HINT_MAX_SIZE);
      // This defines either MIN_SIZE, or MAX_SIZE, but not both:
      gtk_window_set_geometry_hints(GTK_WINDOW(m_window), nullptr, &g, h);
    }
  }

  void navigate(const std::string &url) {
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(m_webview), url.c_str());
  }

  void set_html(const std::string &html) {
    webkit_web_view_load_html(WEBKIT_WEB_VIEW(m_webview), html.c_str(), NULL);
  }

  void init(const std::string &js) {
    WebKitUserContentManager *manager =
        webkit_web_view_get_user_content_manager(WEBKIT_WEB_VIEW(m_webview));
    webkit_user_content_manager_add_script(
        manager, webkit_user_script_new(
                     js.c_str(), WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
                     WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START, NULL, NULL));
  }

  void eval(const std::string &js) {
    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), js.c_str(), NULL,
                                   NULL, NULL);
  }

private:
  virtual void on_message(const std::string &msg) = 0;

  static char *get_string_from_js_result(WebKitJavascriptResult *r) {
    char *s;
#if WEBKIT_MAJOR_VERSION >= 2 && WEBKIT_MINOR_VERSION >= 22
    JSCValue *value = webkit_javascript_result_get_js_value(r);
    s = jsc_value_to_string(value);
#else
    JSGlobalContextRef ctx = webkit_javascript_result_get_global_context(r);
    JSValueRef value = webkit_javascript_result_get_value(r);
    JSStringRef js = JSValueToStringCopy(ctx, value, NULL);
    size_t n = JSStringGetMaximumUTF8CStringSize(js);
    s = g_new(char, n);
    JSStringGetUTF8CString(js, s, n);
    JSStringRelease(js);
#endif
    return s;
  }

  GtkWidget *m_window;
  GtkWidget *m_webview;
};

} // namespace detail

using browser_engine = detail::gtk_webkit_engine;

} // namespace webview

#elif defined(WEBVIEW_COCOA)

//
// ====================================================================
//
// This implementation uses Cocoa WKWebView backend on macOS. It is
// written using ObjC runtime and uses WKWebView class as a browser runtime.
// You should pass "-framework Webkit" flag to the compiler.
//
// ====================================================================
//

#include <CoreGraphics/CoreGraphics.h>
#include <objc/NSObjCRuntime.h>
#include <objc/objc-runtime.h>

namespace webview {
namespace detail {
namespace objc {

// A convenient template function for unconditionally casting the specified
// C-like function into a function that can be called with the given return
// type and arguments. Caller takes full responsibility for ensuring that
// the function call is valid. It is assumed that the function will not
// throw exceptions.
template <typename Result, typename Callable, typename... Args>
Result invoke(Callable callable, Args... args) noexcept {
  return reinterpret_cast<Result (*)(Args...)>(callable)(args...);
}

// Calls objc_msgSend.
template <typename Result, typename... Args>
Result msg_send(Args... args) noexcept {
  return invoke<Result>(objc_msgSend, args...);
}

} // namespace objc

enum NSBackingStoreType : NSUInteger { NSBackingStoreBuffered = 2 };

enum NSWindowStyleMask : NSUInteger {
  NSWindowStyleMaskTitled = 1,
  NSWindowStyleMaskClosable = 2,
  NSWindowStyleMaskMiniaturizable = 4,
  NSWindowStyleMaskResizable = 8
};

enum NSApplicationActivationPolicy : NSInteger {
  NSApplicationActivationPolicyRegular = 0
};

enum WKUserScriptInjectionTime : NSInteger {
  WKUserScriptInjectionTimeAtDocumentStart = 0
};

enum NSModalResponse : NSInteger { NSModalResponseOK = 1 };

// Convenient conversion of string literals.
id operator"" _cls(const char *s, std::size_t) { return (id)objc_getClass(s); }
SEL operator"" _sel(const char *s, std::size_t) { return sel_registerName(s); }
id operator"" _str(const char *s, std::size_t) {
  return objc::msg_send<id>("NSString"_cls, "stringWithUTF8String:"_sel, s);
}

class cocoa_wkwebview_engine {
public:
  cocoa_wkwebview_engine(bool debug, void *window)
      : m_debug{debug}, m_parent_window{window} {
    auto app = get_shared_application();
    auto delegate = create_app_delegate();
    objc_setAssociatedObject(delegate, "webview", (id)this,
                             OBJC_ASSOCIATION_ASSIGN);
    objc::msg_send<void>(app, "setDelegate:"_sel, delegate);

    // Start the main run loop so that the app delegate gets the
    // NSApplicationDidFinishLaunchingNotification notification after the run
    // loop has started in order to perform further initialization.
    // We need to return from this constructor so this run loop is only
    // temporary.
    objc::msg_send<void>(app, "run"_sel);
  }
  virtual ~cocoa_wkwebview_engine() = default;
  void *window() { return (void *)m_window; }
  void terminate() {
    auto app = get_shared_application();
    objc::msg_send<void>(app, "terminate:"_sel, nullptr);
  }
  void run() {
    auto app = get_shared_application();
    objc::msg_send<void>(app, "run"_sel);
  }
  void dispatch(std::function<void()> f) {
    dispatch_async_f(dispatch_get_main_queue(), new dispatch_fn_t(f),
                     (dispatch_function_t)([](void *arg) {
                       auto f = static_cast<dispatch_fn_t *>(arg);
                       (*f)();
                       delete f;
                     }));
  }
  void set_title(const std::string &title) {
    objc::msg_send<void>(m_window, "setTitle:"_sel,
                         objc::msg_send<id>("NSString"_cls,
                                            "stringWithUTF8String:"_sel,
                                            title.c_str()));
  }
  void set_size(int width, int height, int hints) {
    auto style = static_cast<NSWindowStyleMask>(
        NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable);
    if (hints != WEBVIEW_HINT_FIXED) {
      style =
          static_cast<NSWindowStyleMask>(style | NSWindowStyleMaskResizable);
    }
    objc::msg_send<void>(m_window, "setStyleMask:"_sel, style);

    if (hints == WEBVIEW_HINT_MIN) {
      objc::msg_send<void>(m_window, "setContentMinSize:"_sel,
                           CGSizeMake(width, height));
    } else if (hints == WEBVIEW_HINT_MAX) {
      objc::msg_send<void>(m_window, "setContentMaxSize:"_sel,
                           CGSizeMake(width, height));
    } else {
      objc::msg_send<void>(m_window, "setFrame:display:animate:"_sel,
                           CGRectMake(0, 0, width, height), YES, NO);
    }
    objc::msg_send<void>(m_window, "center"_sel);
  }
  void navigate(const std::string &url) {
    auto nsurl = objc::msg_send<id>(
        "NSURL"_cls, "URLWithString:"_sel,
        objc::msg_send<id>("NSString"_cls, "stringWithUTF8String:"_sel,
                           url.c_str()));

    objc::msg_send<void>(
        m_webview, "loadRequest:"_sel,
        objc::msg_send<id>("NSURLRequest"_cls, "requestWithURL:"_sel, nsurl));
  }
  void set_html(const std::string &html) {
    objc::msg_send<void>(m_webview, "loadHTMLString:baseURL:"_sel,
                         objc::msg_send<id>("NSString"_cls,
                                            "stringWithUTF8String:"_sel,
                                            html.c_str()),
                         nullptr);
  }
  void init(const std::string &js) {
    // Equivalent Obj-C:
    // [m_manager addUserScript:[[WKUserScript alloc] initWithSource:[NSString stringWithUTF8String:js.c_str()] injectionTime:WKUserScriptInjectionTimeAtDocumentStart forMainFrameOnly:YES]]
    objc::msg_send<void>(
        m_manager, "addUserScript:"_sel,
        objc::msg_send<id>(objc::msg_send<id>("WKUserScript"_cls, "alloc"_sel),
                           "initWithSource:injectionTime:forMainFrameOnly:"_sel,
                           objc::msg_send<id>("NSString"_cls,
                                              "stringWithUTF8String:"_sel,
                                              js.c_str()),
                           WKUserScriptInjectionTimeAtDocumentStart, YES));
  }
  void eval(const std::string &js) {
    objc::msg_send<void>(m_webview, "evaluateJavaScript:completionHandler:"_sel,
                         objc::msg_send<id>("NSString"_cls,
                                            "stringWithUTF8String:"_sel,
                                            js.c_str()),
                         nullptr);
  }

private:
  virtual void on_message(const std::string &msg) = 0;
  static id create_app_delegate() {
    auto cls =
        objc_allocateClassPair((Class) "NSResponder"_cls, "AppDelegate", 0);
    class_addProtocol(cls, objc_getProtocol("NSTouchBarProvider"));
    class_addMethod(cls, "applicationShouldTerminateAfterLastWindowClosed:"_sel,
                    (IMP)(+[](id, SEL, id) -> BOOL { return 1; }), "c@:@");
    class_addMethod(
        cls, "userContentController:didReceiveScriptMessage:"_sel,
        (IMP)(+[](id self, SEL, id, id msg) {
          auto w = get_associated_webview(self);
          w->on_message(((const char *(*)(id, SEL))objc_msgSend)(
              objc::msg_send<id>(msg, "body"_sel), "UTF8String"_sel));
        }),
        "v@:@@");
    class_addMethod(cls, "applicationDidFinishLaunching:"_sel,
                    (IMP)(+[](id self, SEL, id notification) {
                      auto app = objc::msg_send<id>(notification, "object"_sel);
                      auto w = get_associated_webview(self);
                      w->on_application_did_finish_launching(self, app);
                    }),
                    "v@:@");
    objc_registerClassPair(cls);
    return objc::msg_send<id>((id)cls, "new"_sel);
  }
  static id create_webkit_ui_delegate() {
    auto cls =
        objc_allocateClassPair((Class) "NSObject"_cls, "WebkitUIDelegate", 0);
    class_addProtocol(cls, objc_getProtocol("WKUIDelegate"));
    class_addMethod(
        cls,
        "webView:runOpenPanelWithParameters:initiatedByFrame:completionHandler:"_sel,
        (IMP)(+[](id, SEL, id, id parameters, id, id completion_handler) {
          auto allows_multiple_selection =
              objc::msg_send<BOOL>(parameters, "allowsMultipleSelection"_sel);
          auto allows_directories =
              objc::msg_send<BOOL>(parameters, "allowsDirectories"_sel);

          // Show a panel for selecting files.
          auto panel = objc::msg_send<id>("NSOpenPanel"_cls, "openPanel"_sel);
          objc::msg_send<void>(panel, "setCanChooseFiles:"_sel, YES);
          objc::msg_send<void>(panel, "setCanChooseDirectories:"_sel,
                               allows_directories);
          objc::msg_send<void>(panel, "setAllowsMultipleSelection:"_sel,
                               allows_multiple_selection);
          auto modal_response =
              objc::msg_send<NSModalResponse>(panel, "runModal"_sel);

          // Get the URLs for the selected files. If the modal was canceled
          // then we pass null to the completion handler to signify
          // cancellation.
          id urls = modal_response == NSModalResponseOK
                        ? objc::msg_send<id>(panel, "URLs"_sel)
                        : nullptr;

          // Invoke the completion handler block.
          auto sig = objc::msg_send<id>("NSMethodSignature"_cls,
                                        "signatureWithObjCTypes:"_sel, "v@?@");
          auto invocation = objc::msg_send<id>(
              "NSInvocation"_cls, "invocationWithMethodSignature:"_sel, sig);
          objc::msg_send<void>(invocation, "setTarget:"_sel,
                               completion_handler);
          objc::msg_send<void>(invocation, "setArgument:atIndex:"_sel, &urls,
                               1);
          objc::msg_send<void>(invocation, "invoke"_sel);
        }),
        "v@:@@@@");
    objc_registerClassPair(cls);
    return objc::msg_send<id>((id)cls, "new"_sel);
  }
  static id get_shared_application() {
    return objc::msg_send<id>("NSApplication"_cls, "sharedApplication"_sel);
  }
  static cocoa_wkwebview_engine *get_associated_webview(id object) {
    auto w =
        (cocoa_wkwebview_engine *)objc_getAssociatedObject(object, "webview");
    assert(w);
    return w;
  }
  static id get_main_bundle() noexcept {
    return objc::msg_send<id>("NSBundle"_cls, "mainBundle"_sel);
  }
  static bool is_app_bundled() noexcept {
    auto bundle = get_main_bundle();
    if (!bundle) {
      return false;
    }
    auto bundle_path = objc::msg_send<id>(bundle, "bundlePath"_sel);
    auto bundled =
        objc::msg_send<BOOL>(bundle_path, "hasSuffix:"_sel, ".app"_str);
    return !!bundled;
  }
  void on_application_did_finish_launching(id delegate, id app) {
    // Stop the main run loop so that we can return
    // from the constructor.
    objc::msg_send<void>(app, "stop:"_sel, nullptr);

    // Activate the app if it is not bundled.
    // Bundled apps launched from Finder are activated automatically but
    // otherwise not. Activating the app even when it has been launched from
    // Finder does not seem to be harmful but calling this function is rarely
    // needed as proper activation is normally taken care of for us.
    // Bundled apps have a default activation policy of
    // NSApplicationActivationPolicyRegular while non-bundled apps have a
    // default activation policy of NSApplicationActivationPolicyProhibited.
    if (!is_app_bundled()) {
      // "setActivationPolicy:" must be invoked before
      // "activateIgnoringOtherApps:" for activation to work.
      objc::msg_send<void>(app, "setActivationPolicy:"_sel,
                           NSApplicationActivationPolicyRegular);
      // Activate the app regardless of other active apps.
      // This can be obtrusive so we only do it when necessary.
      objc::msg_send<void>(app, "activateIgnoringOtherApps:"_sel, YES);
    }

    // Main window
    if (!m_parent_window) {
      m_window = objc::msg_send<id>("NSWindow"_cls, "alloc"_sel);
      auto style = NSWindowStyleMaskTitled;
      m_window = objc::msg_send<id>(
          m_window, "initWithContentRect:styleMask:backing:defer:"_sel,
          CGRectMake(0, 0, 0, 0), style, NSBackingStoreBuffered, NO);
    } else {
      m_window = (id)m_parent_window;
    }

    // Webview
    auto config = objc::msg_send<id>("WKWebViewConfiguration"_cls, "new"_sel);
    m_manager = objc::msg_send<id>(config, "userContentController"_sel);
    m_webview = objc::msg_send<id>("WKWebView"_cls, "alloc"_sel);

    if (m_debug) {
      // Equivalent Obj-C:
      // [[config preferences] setValue:@YES forKey:@"developerExtrasEnabled"];
      objc::msg_send<id>(
          objc::msg_send<id>(config, "preferences"_sel), "setValue:forKey:"_sel,
          objc::msg_send<id>("NSNumber"_cls, "numberWithBool:"_sel, YES),
          "developerExtrasEnabled"_str);
    }

    // Equivalent Obj-C:
    // [[config preferences] setValue:@YES forKey:@"fullScreenEnabled"];
    objc::msg_send<id>(
        objc::msg_send<id>(config, "preferences"_sel), "setValue:forKey:"_sel,
        objc::msg_send<id>("NSNumber"_cls, "numberWithBool:"_sel, YES),
        "fullScreenEnabled"_str);

    // Equivalent Obj-C:
    // [[config preferences] setValue:@YES forKey:@"javaScriptCanAccessClipboard"];
    objc::msg_send<id>(
        objc::msg_send<id>(config, "preferences"_sel), "setValue:forKey:"_sel,
        objc::msg_send<id>("NSNumber"_cls, "numberWithBool:"_sel, YES),
        "javaScriptCanAccessClipboard"_str);

    // Equivalent Obj-C:
    // [[config preferences] setValue:@YES forKey:@"DOMPasteAllowed"];
    objc::msg_send<id>(
        objc::msg_send<id>(config, "preferences"_sel), "setValue:forKey:"_sel,
        objc::msg_send<id>("NSNumber"_cls, "numberWithBool:"_sel, YES),
        "DOMPasteAllowed"_str);

    auto ui_delegate = create_webkit_ui_delegate();
    objc::msg_send<void>(m_webview, "initWithFrame:configuration:"_sel,
                         CGRectMake(0, 0, 0, 0), config);
    objc::msg_send<void>(m_webview, "setUIDelegate:"_sel, ui_delegate);
    objc::msg_send<void>(m_manager, "addScriptMessageHandler:name:"_sel,
                         delegate, "external"_str);

    init(R"script(
      window.external = {
        invoke: function(s) {
          window.webkit.messageHandlers.external.postMessage(s);
        },
      };
      )script");
    objc::msg_send<void>(m_window, "setContentView:"_sel, m_webview);
    objc::msg_send<void>(m_window, "makeKeyAndOrderFront:"_sel, nullptr);
  }
  bool m_debug;
  void *m_parent_window;
  id m_window;
  id m_webview;
  id m_manager;
};

} // namespace detail

using browser_engine = detail::cocoa_wkwebview_engine;

} // namespace webview

#elif defined(WEBVIEW_EDGE)

//
// ====================================================================
//
// This implementation uses Win32 API to create a native window. It
// uses Edge/Chromium webview2 backend as a browser engine.
//
// ====================================================================
//

#define WIN32_LEAN_AND_MEAN
#include <shlobj.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <windows.h>

#include "WebView2.h"

#ifdef _MSC_VER
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")
#endif

namespace webview {
namespace detail {

using msg_cb_t = std::function<void(const std::string)>;

// Converts a narrow (UTF-8-encoded) string into a wide (UTF-16-encoded) string.
inline std::wstring widen_string(const std::string &input) {
  if (input.empty()) {
    return std::wstring();
  }
  UINT cp = CP_UTF8;
  DWORD flags = MB_ERR_INVALID_CHARS;
  auto input_c = input.c_str();
  auto input_length = static_cast<int>(input.size());
  auto required_length =
      MultiByteToWideChar(cp, flags, input_c, input_length, nullptr, 0);
  if (required_length > 0) {
    std::wstring output(static_cast<std::size_t>(required_length), L'\0');
    if (MultiByteToWideChar(cp, flags, input_c, input_length, &output[0],
                            required_length) > 0) {
      return output;
    }
  }
  // Failed to convert string from UTF-8 to UTF-16
  return std::wstring();
}

// Converts a wide (UTF-16-encoded) string into a narrow (UTF-8-encoded) string.
inline std::string narrow_string(const std::wstring &input) {
  if (input.empty()) {
    return std::string();
  }
  UINT cp = CP_UTF8;
  DWORD flags = WC_ERR_INVALID_CHARS;
  auto input_c = input.c_str();
  auto input_length = static_cast<int>(input.size());
  auto required_length = WideCharToMultiByte(cp, flags, input_c, input_length,
                                             nullptr, 0, nullptr, nullptr);
  if (required_length > 0) {
    std::string output(static_cast<std::size_t>(required_length), '\0');
    if (WideCharToMultiByte(cp, flags, input_c, input_length, &output[0],
                            required_length, nullptr, nullptr) > 0) {
      return output;
    }
  }
  // Failed to convert string from UTF-16 to UTF-8
  return std::string();
}

// A wrapper around COM library initialization. Calls CoInitializeEx in the
// constructor and CoUninitialize in the destructor.
class com_init_wrapper {
public:
  com_init_wrapper(DWORD dwCoInit) {
    // We can safely continue as long as COM was either successfully
    // initialized or already initialized.
    // RPC_E_CHANGED_MODE means that CoInitializeEx was already called with
    // a different concurrency model.
    switch (CoInitializeEx(nullptr, dwCoInit)) {
    case S_OK:
    case S_FALSE:
      m_initialized = true;
      break;
    }
  }

  ~com_init_wrapper() {
    if (m_initialized) {
      CoUninitialize();
      m_initialized = false;
    }
  }

  com_init_wrapper(const com_init_wrapper &other) = delete;
  com_init_wrapper &operator=(const com_init_wrapper &other) = delete;
  com_init_wrapper(com_init_wrapper &&other) = delete;
  com_init_wrapper &operator=(com_init_wrapper &&other) = delete;

  bool is_initialized() const { return m_initialized; }

private:
  bool m_initialized = false;
};

// Holds a symbol name and associated type for code clarity.
template <typename T> class library_symbol {
public:
  using type = T;

  constexpr explicit library_symbol(const char *name) : m_name(name) {}
  constexpr const char *get_name() const { return m_name; }

private:
  const char *m_name;
};

// Loads a native shared library and allows one to get addresses for those
// symbols.
class native_library {
public:
  explicit native_library(const wchar_t *name) : m_handle(LoadLibraryW(name)) {}

  ~native_library() {
    if (m_handle) {
      FreeLibrary(m_handle);
      m_handle = nullptr;
    }
  }

  native_library(const native_library &other) = delete;
  native_library &operator=(const native_library &other) = delete;
  native_library(native_library &&other) = default;
  native_library &operator=(native_library &&other) = default;

  // Returns true if the library is currently loaded; otherwise false.
  operator bool() const { return is_loaded(); }

  // Get the address for the specified symbol or nullptr if not found.
  template <typename Symbol>
  typename Symbol::type get(const Symbol &symbol) const {
    if (is_loaded()) {
      return reinterpret_cast<typename Symbol::type>(
          GetProcAddress(m_handle, symbol.get_name()));
    }
    return nullptr;
  }

  // Returns true if the library is currently loaded; otherwise false.
  bool is_loaded() const { return !!m_handle; }

private:
  HMODULE m_handle = nullptr;
};

struct user32_symbols {
  using DPI_AWARENESS_CONTEXT = HANDLE;
  using SetProcessDpiAwarenessContext_t = BOOL(WINAPI *)(DPI_AWARENESS_CONTEXT);

  static constexpr auto SetProcessDpiAwarenessContext =
      library_symbol<SetProcessDpiAwarenessContext_t>(
          "SetProcessDpiAwarenessContext");
  static constexpr auto SetProcessDPIAware =
      library_symbol<decltype(&::SetProcessDPIAware)>("SetProcessDPIAware");
};

struct shcore_symbols {
  typedef enum { PROCESS_PER_MONITOR_DPI_AWARE = 2 } PROCESS_DPI_AWARENESS;
  using SetProcessDpiAwareness_t = HRESULT(WINAPI *)(PROCESS_DPI_AWARENESS);

  static constexpr auto SetProcessDpiAwareness =
      library_symbol<SetProcessDpiAwareness_t>("SetProcessDpiAwareness");
};

inline bool enable_dpi_awareness() {
  auto user32 = native_library(L"user32.dll");
  if (auto fn = user32.get(user32_symbols::SetProcessDpiAwarenessContext)) {
    if (fn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)) {
      return true;
    }
    return GetLastError() == ERROR_ACCESS_DENIED;
  }
  if (auto shcore = native_library(L"shcore.dll")) {
    if (auto fn = shcore.get(shcore_symbols::SetProcessDpiAwareness)) {
      auto result = fn(shcore_symbols::PROCESS_PER_MONITOR_DPI_AWARE);
      return result == S_OK || result == E_ACCESSDENIED;
    }
  }
  if (auto fn = user32.get(user32_symbols::SetProcessDPIAware)) {
    return !!fn();
  }
  return true;
}

class webview2_com_handler
    : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler,
      public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
      public ICoreWebView2WebMessageReceivedEventHandler,
      public ICoreWebView2PermissionRequestedEventHandler {
  using webview2_com_handler_cb_t =
      std::function<void(ICoreWebView2Controller *, ICoreWebView2 *webview)>;

public:
  webview2_com_handler(HWND hwnd, msg_cb_t msgCb, webview2_com_handler_cb_t cb)
      : m_window(hwnd), m_msgCb(msgCb), m_cb(cb) {}

  virtual ~webview2_com_handler() = default;
  webview2_com_handler(const webview2_com_handler &other) = delete;
  webview2_com_handler &operator=(const webview2_com_handler &other) = delete;
  webview2_com_handler(webview2_com_handler &&other) = delete;
  webview2_com_handler &operator=(webview2_com_handler &&other) = delete;

  ULONG STDMETHODCALLTYPE AddRef() { return ++m_ref_count; }
  ULONG STDMETHODCALLTYPE Release() {
    if (m_ref_count > 1) {
      return --m_ref_count;
    }
    delete this;
    return 0;
  }
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID *ppv) {
    if (!ppv) {
      return E_POINTER;
    }
    *ppv = nullptr;
    return E_NOINTERFACE;
  }
  HRESULT STDMETHODCALLTYPE Invoke(HRESULT res, ICoreWebView2Environment *env) {
    env->CreateCoreWebView2Controller(m_window, this);
    return S_OK;
  }
  HRESULT STDMETHODCALLTYPE Invoke(HRESULT res,
                                   ICoreWebView2Controller *controller) {
    ICoreWebView2 *webview;
    ::EventRegistrationToken token;
    controller->get_CoreWebView2(&webview);
    webview->add_WebMessageReceived(this, &token);
    webview->add_PermissionRequested(this, &token);

    m_cb(controller, webview);
    return S_OK;
  }
  HRESULT STDMETHODCALLTYPE Invoke(
      ICoreWebView2 *sender, ICoreWebView2WebMessageReceivedEventArgs *args) {
    LPWSTR message;
    args->TryGetWebMessageAsString(&message);
    m_msgCb(narrow_string(message));
    sender->PostWebMessageAsString(message);

    CoTaskMemFree(message);
    return S_OK;
  }
  HRESULT STDMETHODCALLTYPE Invoke(
      ICoreWebView2 *sender, ICoreWebView2PermissionRequestedEventArgs *args) {
    COREWEBVIEW2_PERMISSION_KIND kind;
    args->get_PermissionKind(&kind);
    if (kind == COREWEBVIEW2_PERMISSION_KIND_CLIPBOARD_READ) {
      args->put_State(COREWEBVIEW2_PERMISSION_STATE_ALLOW);
    }
    return S_OK;
  }

private:
  HWND m_window;
  msg_cb_t m_msgCb;
  webview2_com_handler_cb_t m_cb;
  std::atomic<ULONG> m_ref_count{1};
};

class win32_edge_engine {
public:
  win32_edge_engine(bool debug, void *window) {
    if (!is_webview2_available()) {
      return;
    }
    if (!m_com_init.is_initialized()) {
      return;
    }
    enable_dpi_awareness();
    if (window == nullptr) {
      HINSTANCE hInstance = GetModuleHandle(nullptr);
      HICON icon = (HICON)LoadImage(
          hInstance, IDI_APPLICATION, IMAGE_ICON, GetSystemMetrics(SM_CXICON),
          GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);

      WNDCLASSEXW wc;
      ZeroMemory(&wc, sizeof(WNDCLASSEX));
      wc.cbSize = sizeof(WNDCLASSEX);
      wc.hInstance = hInstance;
      wc.lpszClassName = L"webview";
      wc.hIcon = icon;
      wc.lpfnWndProc =
          (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
            auto w = (win32_edge_engine *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            switch (msg) {
            case WM_SIZE:
              w->resize(hwnd);
              break;
            case WM_CLOSE:
              DestroyWindow(hwnd);
              break;
            case WM_DESTROY:
              w->terminate();
              break;
            case WM_GETMINMAXINFO: {
              auto lpmmi = (LPMINMAXINFO)lp;
              if (w == nullptr) {
                return 0;
              }
              if (w->m_maxsz.x > 0 && w->m_maxsz.y > 0) {
                lpmmi->ptMaxSize = w->m_maxsz;
                lpmmi->ptMaxTrackSize = w->m_maxsz;
              }
              if (w->m_minsz.x > 0 && w->m_minsz.y > 0) {
                lpmmi->ptMinTrackSize = w->m_minsz;
              }
            } break;
            default:
              return DefWindowProcW(hwnd, msg, wp, lp);
            }
            return 0;
          });
      RegisterClassExW(&wc);
      m_window = CreateWindowW(L"webview", L"", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr,
                               nullptr, hInstance, nullptr);
      if (m_window == nullptr) {
        return;
      }
      SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);
    } else {
      m_window = *(static_cast<HWND *>(window));
    }

    ShowWindow(m_window, SW_SHOW);
    UpdateWindow(m_window);
    SetFocus(m_window);

    auto cb =
        std::bind(&win32_edge_engine::on_message, this, std::placeholders::_1);

    embed(m_window, debug, cb);
    resize(m_window);
    m_controller->MoveFocus(COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC);
  }

  virtual ~win32_edge_engine() {
    if (m_com_handler) {
      m_com_handler->Release();
      m_com_handler = nullptr;
    }
    if (m_webview) {
      m_webview->Release();
      m_webview = nullptr;
    }
    if (m_controller) {
      m_controller->Release();
      m_controller = nullptr;
    }
  }

  win32_edge_engine(const win32_edge_engine &other) = delete;
  win32_edge_engine &operator=(const win32_edge_engine &other) = delete;
  win32_edge_engine(win32_edge_engine &&other) = delete;
  win32_edge_engine &operator=(win32_edge_engine &&other) = delete;

  void run() {
    MSG msg;
    BOOL res;
    while ((res = GetMessage(&msg, nullptr, 0, 0)) != -1) {
      if (msg.hwnd) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        continue;
      }
      if (msg.message == WM_APP) {
        auto f = (dispatch_fn_t *)(msg.lParam);
        (*f)();
        delete f;
      } else if (msg.message == WM_QUIT) {
        return;
      }
    }
  }
  void *window() { return (void *)m_window; }
  void terminate() { PostQuitMessage(0); }
  void dispatch(dispatch_fn_t f) {
    PostThreadMessage(m_main_thread, WM_APP, 0, (LPARAM) new dispatch_fn_t(f));
  }

  void set_title(const std::string &title) {
    SetWindowTextW(m_window, widen_string(title).c_str());
  }

  void set_size(int width, int height, int hints) {
    auto style = GetWindowLong(m_window, GWL_STYLE);
    if (hints == WEBVIEW_HINT_FIXED) {
      style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    } else {
      style |= (WS_THICKFRAME | WS_MAXIMIZEBOX);
    }
    SetWindowLong(m_window, GWL_STYLE, style);

    if (hints == WEBVIEW_HINT_MAX) {
      m_maxsz.x = width;
      m_maxsz.y = height;
    } else if (hints == WEBVIEW_HINT_MIN) {
      m_minsz.x = width;
      m_minsz.y = height;
    } else {
      RECT r;
      r.left = r.top = 0;
      r.right = width;
      r.bottom = height;
      AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, 0);
      SetWindowPos(
          m_window, NULL, r.left, r.top, r.right - r.left, r.bottom - r.top,
          SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_FRAMECHANGED);
      resize(m_window);
    }
  }

  void navigate(const std::string &url) {
    auto wurl = widen_string(url);
    m_webview->Navigate(wurl.c_str());
  }

  void init(const std::string &js) {
    auto wjs = widen_string(js);
    m_webview->AddScriptToExecuteOnDocumentCreated(wjs.c_str(), nullptr);
  }

  void eval(const std::string &js) {
    auto wjs = widen_string(js);
    m_webview->ExecuteScript(wjs.c_str(), nullptr);
  }

  void set_html(const std::string &html) {
    m_webview->NavigateToString(widen_string(html).c_str());
  }

private:
  bool embed(HWND wnd, bool debug, msg_cb_t cb) {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    flag.test_and_set();

    wchar_t currentExePath[MAX_PATH];
    GetModuleFileNameW(NULL, currentExePath, MAX_PATH);
    wchar_t *currentExeName = PathFindFileNameW(currentExePath);

    wchar_t dataPath[MAX_PATH];
    if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, dataPath))) {
      return false;
    }
    wchar_t userDataFolder[MAX_PATH];
    PathCombineW(userDataFolder, dataPath, currentExeName);

    m_com_handler = new webview2_com_handler(
        wnd, cb,
        [&](ICoreWebView2Controller *controller, ICoreWebView2 *webview) {
          controller->AddRef();
          webview->AddRef();
          m_controller = controller;
          m_webview = webview;
          flag.clear();
        });
    HRESULT res = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, userDataFolder, nullptr, m_com_handler);
    if (res != S_OK) {
      return false;
    }
    MSG msg = {};
    while (flag.test_and_set() && GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    ICoreWebView2Settings *settings = nullptr;
    res = m_webview->get_Settings(&settings);
    if (res != S_OK) {
      return false;
    }
    res = settings->put_AreDevToolsEnabled(debug ? TRUE : FALSE);
    if (res != S_OK) {
      return false;
    }
    init("window.external={invoke:s=>window.chrome.webview.postMessage(s)}");
    return true;
  }

  void resize(HWND wnd) {
    if (m_controller == nullptr) {
      return;
    }
    RECT bounds;
    GetClientRect(wnd, &bounds);
    m_controller->put_Bounds(bounds);
  }

  static bool is_webview2_available() noexcept {
    LPWSTR version_info = nullptr;
    auto res =
        GetAvailableCoreWebView2BrowserVersionString(nullptr, &version_info);
    // The result will be equal to HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
    // if the WebView2 runtime is not installed.
    auto ok = SUCCEEDED(res) && version_info;
    if (version_info) {
      CoTaskMemFree(version_info);
    }
    return ok;
  }

  virtual void on_message(const std::string &msg) = 0;

  // The app is expected to call CoInitializeEx before
  // CreateCoreWebView2EnvironmentWithOptions.
  // Source: https://docs.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/webview2-idl#createcorewebview2environmentwithoptions
  com_init_wrapper m_com_init{COINIT_APARTMENTTHREADED};
  HWND m_window = NULL;
  POINT m_minsz = POINT{0, 0};
  POINT m_maxsz = POINT{0, 0};
  DWORD m_main_thread = GetCurrentThreadId();
  ICoreWebView2 *m_webview = nullptr;
  ICoreWebView2Controller *m_controller = nullptr;
  webview2_com_handler *m_com_handler = nullptr;
};

} // namespace detail

using browser_engine = detail::win32_edge_engine;

} // namespace webview

#endif /* WEBVIEW_GTK, WEBVIEW_COCOA, WEBVIEW_EDGE */

namespace webview {

class webview : public browser_engine {
public:
  webview(bool debug = false, void *wnd = nullptr)
      : browser_engine(debug, wnd) {}

  void navigate(const std::string &url) {
    if (url == "") {
      browser_engine::navigate("about:blank");
      return;
    }
    browser_engine::navigate(url);
  }

  using binding_t = std::function<void(std::string, std::string, void *)>;
  class binding_ctx_t {
  public:
    binding_ctx_t(binding_t *callback, void *arg, bool sync = true)
        : callback(callback), arg(arg), sync(sync) {}
    // This function is called upon execution of the bound JS function
    binding_t *callback;
    // This user-supplied argument is passed to the callback
    void *arg;
    // This boolean expresses whether or not this binding is synchronous or asynchronous
    // Async bindings require the user to call the resolve function, sync bindings don't
    bool sync;
  };

  using sync_binding_t = std::function<std::string(std::string)>;
  using sync_binding_ctx_t = std::pair<webview *, sync_binding_t>;

  // Synchronous bind
  void bind(const std::string &name, sync_binding_t fn) {
    if (bindings.count(name) == 0) {
      bindings[name] = new binding_ctx_t(
          new binding_t(
              [](const std::string &seq, const std::string &req, void *arg) {
                auto pair = static_cast<sync_binding_ctx_t *>(arg);
                pair->first->resolve(seq, 0, pair->second(req));
              }),
          new sync_binding_ctx_t(this, fn));
      bind_js(name);
    }
  }

  // Asynchronous bind
  void bind(const std::string &name, binding_t f, void *arg) {
    if (bindings.count(name) == 0) {
      bindings[name] = new binding_ctx_t(new binding_t(f), arg, false);
      bind_js(name);
    }
  }

  void unbind(const std::string &name) {
    if (bindings.find(name) != bindings.end()) {
      auto js = "delete window['" + name + "'];";
      init(js);
      eval(js);
      delete bindings[name]->callback;
      if (bindings[name]->sync) {
        delete static_cast<sync_binding_ctx_t *>(bindings[name]->arg);
      }
      delete bindings[name];
      bindings.erase(name);
    }
  }

  void resolve(const std::string &seq, int status, const std::string &result) {
    dispatch([seq, status, result, this]() {
      if (status == 0) {
        eval("window._rpc[" + seq + "].resolve(" + result +
             "); delete window._rpc[" + seq + "]");
      } else {
        eval("window._rpc[" + seq + "].reject(" + result +
             "); delete window._rpc[" + seq + "]");
      }
    });
  }

private:
  void bind_js(const std::string &name) {
    auto js = "(function() { var name = '" + name + "';" + R"(
      var RPC = window._rpc = (window._rpc || {nextSeq: 1});
      window[name] = function() {
        var seq = RPC.nextSeq++;
        var promise = new Promise(function(resolve, reject) {
          RPC[seq] = {
            resolve: resolve,
            reject: reject,
          };
        });
        window.external.invoke(JSON.stringify({
          id: seq,
          method: name,
          params: Array.prototype.slice.call(arguments),
        }));
        return promise;
      }
    })())";
    init(js);
    eval(js);
  }

  void on_message(const std::string &msg) {
    auto seq = detail::json_parse(msg, "id", 0);
    auto name = detail::json_parse(msg, "method", 0);
    auto args = detail::json_parse(msg, "params", 0);
    if (bindings.find(name) == bindings.end()) {
      return;
    }
    auto fn = bindings[name];
    (*fn->callback)(seq, args, fn->arg);
  }

  std::map<std::string, binding_ctx_t *> bindings;
};
} // namespace webview

WEBVIEW_API webview_t webview_create(int debug, void *wnd) {
  auto w = new webview::webview(debug, wnd);
  if (!w->window()) {
    delete w;
    return nullptr;
  }
  return w;
}

WEBVIEW_API void webview_destroy(webview_t w) {
  delete static_cast<webview::webview *>(w);
}

WEBVIEW_API void webview_run(webview_t w) {
  static_cast<webview::webview *>(w)->run();
}

WEBVIEW_API void webview_terminate(webview_t w) {
  static_cast<webview::webview *>(w)->terminate();
}

WEBVIEW_API void webview_dispatch(webview_t w, void (*fn)(webview_t, void *),
                                  void *arg) {
  static_cast<webview::webview *>(w)->dispatch([=]() { fn(w, arg); });
}

WEBVIEW_API void *webview_get_window(webview_t w) {
  return static_cast<webview::webview *>(w)->window();
}

WEBVIEW_API void webview_set_title(webview_t w, const char *title) {
  static_cast<webview::webview *>(w)->set_title(title);
}

WEBVIEW_API void webview_set_size(webview_t w, int width, int height,
                                  int hints) {
  static_cast<webview::webview *>(w)->set_size(width, height, hints);
}

WEBVIEW_API void webview_navigate(webview_t w, const char *url) {
  static_cast<webview::webview *>(w)->navigate(url);
}

WEBVIEW_API void webview_set_html(webview_t w, const char *html) {
  static_cast<webview::webview *>(w)->set_html(html);
}

WEBVIEW_API void webview_init(webview_t w, const char *js) {
  static_cast<webview::webview *>(w)->init(js);
}

WEBVIEW_API void webview_eval(webview_t w, const char *js) {
  static_cast<webview::webview *>(w)->eval(js);
}

WEBVIEW_API void webview_bind(webview_t w, const char *name,
                              void (*fn)(const char *seq, const char *req,
                                         void *arg),
                              void *arg) {
  static_cast<webview::webview *>(w)->bind(
      name,
      [=](const std::string &seq, const std::string &req, void *arg) {
        fn(seq.c_str(), req.c_str(), arg);
      },
      arg);
}

WEBVIEW_API void webview_unbind(webview_t w, const char *name) {
  static_cast<webview::webview *>(w)->unbind(name);
}

WEBVIEW_API void webview_return(webview_t w, const char *seq, int status,
                                const char *result) {
  static_cast<webview::webview *>(w)->resolve(seq, status, result);
}

#endif /* WEBVIEW_HEADER */
#endif /* __cplusplus */
#endif /* WEBVIEW_H */
