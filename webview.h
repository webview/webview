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

// The current library major version.
#define WEBVIEW_MAJOR_VERSION 1
// The current library minor version.
#define WEBVIEW_MINOR_VERSION 11
// The current library patch version.
#define WEBVIEW_PATCH_VERSION 0

// The current library API major version.
#define WEBVIEW_API_MAJOR_VERSION 1
// The current library API minor version.
#define WEBVIEW_API_MINOR_VERSION 0
// The current library API patch version.
#define WEBVIEW_API_PATCH_VERSION 0

#ifndef WEBVIEW_DEPRECATED
#if defined(__cplusplus) && __cplusplus >= 201402L
#define WEBVIEW_DEPRECATED(reason) [[deprecated(reason)]]
#elif defined(_MSC_VER)
#define WEBVIEW_DEPRECATED(reason) __declspec(deprecated(reason))
#else
#define WEBVIEW_DEPRECATED(reason) __attribute__((deprecated(reason)))
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void *webview_t;

// Boolean type where 1 is true and 0 is false. Non-zero shall be regarded as true.
typedef enum { WEBVIEW_FALSE, WEBVIEW_TRUE } webview_bool_t;

// Holds the MAJOR.MINOR.PATCH components of a version number.
typedef struct {
  unsigned short major;
  unsigned short minor;
  unsigned short patch;
} webview_version_t;

// Options for webview_create_with_options().
typedef struct {
  // Size of this struct.
  unsigned int struct_size;
  // Library API version.
  webview_version_t api_version;
  // Enables debug/developer tools for supported browser engines if set to
  // WEBVIEW_TRUE.
  webview_bool_t debug;
  // An optional native window handle of a window in which to embed the
  // underlying webview window. Depending on the platform it should be a
  // pointer to GtkWindow, NSWindow or HWND.
  void *window;
} webview_create_options_t;

// Error codes returned to callers of the API.
typedef enum {
  // OK/Success. All functions that return an error code will return this if
  // the operation succeeded.
  webview_error_ok = 0,
  // Internal error.
  webview_error_internal = 1000,
  // Invalid state.
  webview_error_invalid_state = 1001,
  // Invalid argument.
  webview_error_invalid_argument = 1002,
  // API: The API version requested is newer than what's supported by the library.
  webview_error_api_version_too_new = 2000,
  // API: The API version requested is no longer supported by the library.
  webview_error_api_version_too_old = 2001
} webview_error_code_t;

// Window size hints.
typedef enum {
  // Width and height are default size.
  WEBVIEW_HINT_NONE = 0,
  // Width and height are minimum bounds.
  WEBVIEW_HINT_MIN = 1,
  // Width and height are maximum bounds.
  WEBVIEW_HINT_MAX = 2,
  // Window size can not be changed by a user.
  WEBVIEW_HINT_FIXED = 3
} webview_hint_t;

// Creates a new webview instance. If debug is non-zero - developer tools will
// be enabled (if the platform supports them). Window parameter can be a
// pointer to the native window handle. If it's non-null - then child WebView
// is embedded into the given parent window. Otherwise a new window is created.
// Depending on the platform, a GtkWindow, NSWindow or HWND pointer can be
// passed here.
// This function is kept for backward-compatibility and retains the behavior
// of API version 1. New code should use webview_create_with_options() instead.
WEBVIEW_DEPRECATED("Please use webview_create_with_options instead")
WEBVIEW_API webview_t webview_create(int debug, void *window);

// Creates a new webview instance with the specified options and returns the
// new instance in the webview parameter.
// Noteworthy error codes:
//  - webview_error_api_version_too_old
//  - webview_error_api_version_too_new
WEBVIEW_API webview_error_code_t webview_create_with_options(
    webview_t *webview, const webview_create_options_t *options);

// Destroys a webview and closes the native window.
// Always returns webview_error_ok given correct input.
WEBVIEW_API webview_error_code_t webview_destroy(webview_t w);

// Runs the main loop until it's terminated. After this function exits - you
// must destroy the webview.
WEBVIEW_API webview_error_code_t webview_run(webview_t w);

// Stops the main loop. It is safe to call this function from another other
// background thread.
WEBVIEW_API webview_error_code_t webview_terminate(webview_t w);

// Posts a function to be executed on the main thread. You normally do not need
// to call this function, unless you want to tweak the native window.
WEBVIEW_API webview_error_code_t
webview_dispatch(webview_t w, void (*fn)(webview_t w, void *arg), void *arg);

// Returns a native window handle pointer. When using GTK backend the pointer
// is GtkWindow pointer, when using Cocoa backend the pointer is NSWindow
// pointer, when using Win32 backend the pointer is HWND pointer.
WEBVIEW_API void *webview_get_window(webview_t w);

// Updates the title of the native window. Must be called from the UI thread.
WEBVIEW_API webview_error_code_t webview_set_title(webview_t w,
                                                   const char *title);

// Updates native window size. See WEBVIEW_HINT constants.
WEBVIEW_API webview_error_code_t webview_set_size(webview_t w, int width,
                                                  int height, int hints);

// Navigates webview to the given URL. URL may be a properly encoded data URI.
// Examples:
// webview_navigate(w, "https://github.com/webview/webview");
// webview_navigate(w, "data:text/html,%3Ch1%3EHello%3C%2Fh1%3E");
// webview_navigate(w, "data:text/html;base64,PGgxPkhlbGxvPC9oMT4=");
WEBVIEW_API webview_error_code_t webview_navigate(webview_t w, const char *url);

// Set webview HTML directly.
// Example: webview_set_html(w, "<h1>Hello</h1>");
WEBVIEW_API webview_error_code_t webview_set_html(webview_t w,
                                                  const char *html);

// Injects JavaScript code at the initialization of the new page. Every time
// the webview will open a the new page - this initialization code will be
// executed. It is guaranteed that code is executed before window.onload.
WEBVIEW_API webview_error_code_t webview_init(webview_t w, const char *js);

// Evaluates arbitrary JavaScript code. Evaluation happens asynchronously, also
// the result of the expression is ignored. Use RPC bindings if you want to
// receive notifications about the results of the evaluation.
WEBVIEW_API webview_error_code_t webview_eval(webview_t w, const char *js);

// Binds a native C callback so that it will appear under the given name as a
// global JavaScript function. Internally it uses webview_init(). Callback
// receives a request string and a user-provided argument pointer. Request
// string is a JSON array of all the arguments passed to the JavaScript
// function.
WEBVIEW_API webview_error_code_t webview_bind(
    webview_t w, const char *name,
    void (*fn)(const char *seq, const char *req, void *arg), void *arg);

// Removes a native C callback that was previously set by webview_bind.
WEBVIEW_API webview_error_code_t webview_unbind(webview_t w, const char *name);

// Allows to return a value from the native binding. Original request pointer
// must be provided to help internal RPC engine match requests with responses.
// If status is zero - result is expected to be a valid JSON result value.
// If status is not zero - result is an error JSON object.
WEBVIEW_API webview_error_code_t webview_return(webview_t w, const char *seq,
                                                int status, const char *result);

// Get the version of the library.
WEBVIEW_API webview_version_t webview_library_version();

// Get the API version compiled into the library.
WEBVIEW_API webview_version_t webview_api_version();

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

#ifndef WEBVIEW_DEPRECATED_PRIVATE
#define WEBVIEW_DEPRECATED_PRIVATE                                             \
  WEBVIEW_DEPRECATED("Private API should not be used")
#endif

#include <array>
#include <atomic>
#include <cstring>
#include <functional>
#include <future>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace webview {

// The current library version.
constexpr webview_version_t library_version{
    WEBVIEW_MAJOR_VERSION, WEBVIEW_MINOR_VERSION, WEBVIEW_PATCH_VERSION};

// The current library API version.
constexpr webview_version_t api_version{WEBVIEW_API_MAJOR_VERSION,
                                        WEBVIEW_API_MINOR_VERSION,
                                        WEBVIEW_API_PATCH_VERSION};

class webview_exception : public std::domain_error {
public:
  webview_exception(webview_error_code_t code, const std::string &reason)
      : domain_error(reason), m_code(code) {}
  explicit webview_exception(webview_error_code_t code)
      : domain_error(std::string()), m_code(code) {}
  webview_exception() : domain_error(std::string()) {}

  webview_error_code_t code() const { return m_code; }
  std::exception_ptr cause() const { return m_cause; }

private:
  webview_error_code_t m_code = webview_error_internal;
  std::exception_ptr m_cause;
};

using dispatch_fn_t = std::function<void()>;

namespace detail {

// The minimum API version supported by the library.
constexpr webview_version_t min_api_version{1, 0, 0};

// Compares two versions.
// Returns:
//   - 1 if the first version is greater than the second version.
//   - 0 if the versions are equal.
//   - -1 if the first version is less (lower) than the second version.
inline int compare_versions(const webview_version_t &first,
                            const webview_version_t &second) {
  if (first.major != second.major) {
    return first.major > second.major ? 1 : -1;
  }
  if (first.minor != second.minor) {
    return first.minor > second.minor ? 1 : -1;
  }
  if (first.patch != second.patch) {
    return first.patch > second.patch ? 1 : -1;
  }
  return 0;
}

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

inline webview_create_options_t migrate_webview_create_options(int debug,
                                                               void *wnd) {
  webview_create_options_t options{};
  options.api_version = api_version;
  options.debug = static_cast<webview_bool_t>(debug);
  options.window = wnd;
  return options;
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
  explicit gtk_webkit_engine(const webview_create_options_t &options)
      : m_window(static_cast<GtkWidget *>(options->window)) {
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
#include <objc/objc-runtime.h>

#define NSBackingStoreBuffered 2

#define NSWindowStyleMaskResizable 8
#define NSWindowStyleMaskMiniaturizable 4
#define NSWindowStyleMaskTitled 1
#define NSWindowStyleMaskClosable 2

#define NSApplicationActivationPolicyRegular 0

#define WKUserScriptInjectionTimeAtDocumentStart 0

namespace webview {
namespace detail {

// Helpers to avoid too much typing
id operator"" _cls(const char *s, std::size_t) { return (id)objc_getClass(s); }
SEL operator"" _sel(const char *s, std::size_t) { return sel_registerName(s); }
id operator"" _str(const char *s, std::size_t) {
  return ((id(*)(id, SEL, const char *))objc_msgSend)(
      "NSString"_cls, "stringWithUTF8String:"_sel, s);
}

class cocoa_wkwebview_engine {
public:
  explicit cocoa_wkwebview_engine(const webview_create_options_t &options) {
    // Application
    id app = ((id(*)(id, SEL))objc_msgSend)("NSApplication"_cls,
                                            "sharedApplication"_sel);
    ((void (*)(id, SEL, long))objc_msgSend)(
        app, "setActivationPolicy:"_sel, NSApplicationActivationPolicyRegular);

    // Delegate
    auto cls =
        objc_allocateClassPair((Class) "NSResponder"_cls, "AppDelegate", 0);
    class_addProtocol(cls, objc_getProtocol("NSTouchBarProvider"));
    class_addMethod(cls, "applicationShouldTerminateAfterLastWindowClosed:"_sel,
                    (IMP)(+[](id, SEL, id) -> BOOL { return 1; }), "c@:@");
    class_addMethod(cls, "userContentController:didReceiveScriptMessage:"_sel,
                    (IMP)(+[](id self, SEL, id, id msg) {
                      auto w =
                          (cocoa_wkwebview_engine *)objc_getAssociatedObject(
                              self, "webview");
                      assert(w);
                      w->on_message(((const char *(*)(id, SEL))objc_msgSend)(
                          ((id(*)(id, SEL))objc_msgSend)(msg, "body"_sel),
                          "UTF8String"_sel));
                    }),
                    "v@:@@");
    objc_registerClassPair(cls);

    auto delegate = ((id(*)(id, SEL))objc_msgSend)((id)cls, "new"_sel);
    objc_setAssociatedObject(delegate, "webview", (id)this,
                             OBJC_ASSOCIATION_ASSIGN);
    ((void (*)(id, SEL, id))objc_msgSend)(app, sel_registerName("setDelegate:"),
                                          delegate);

    // Main window
    if (!options->window) {
      m_window = ((id(*)(id, SEL))objc_msgSend)("NSWindow"_cls, "alloc"_sel);
      unsigned int style = NSWindowStyleMaskTitled;
      m_window =
          ((id(*)(id, SEL, CGRect, int, unsigned long, int))objc_msgSend)(
              m_window, "initWithContentRect:styleMask:backing:defer:"_sel,
              CGRectMake(0, 0, 0, 0), style, NSBackingStoreBuffered, 0);
    } else {
      m_window = (id)options->window;
    }

    // Webview
    auto config =
        ((id(*)(id, SEL))objc_msgSend)("WKWebViewConfiguration"_cls, "new"_sel);
    m_manager =
        ((id(*)(id, SEL))objc_msgSend)(config, "userContentController"_sel);
    m_webview = ((id(*)(id, SEL))objc_msgSend)("WKWebView"_cls, "alloc"_sel);

    if (options->debug) {
      // Equivalent Obj-C:
      // [[config preferences] setValue:@YES forKey:@"developerExtrasEnabled"];
      ((id(*)(id, SEL, id, id))objc_msgSend)(
          ((id(*)(id, SEL))objc_msgSend)(config, "preferences"_sel),
          "setValue:forKey:"_sel,
          ((id(*)(id, SEL, BOOL))objc_msgSend)("NSNumber"_cls,
                                               "numberWithBool:"_sel, 1),
          "developerExtrasEnabled"_str);
    }

    // Equivalent Obj-C:
    // [[config preferences] setValue:@YES forKey:@"fullScreenEnabled"];
    ((id(*)(id, SEL, id, id))objc_msgSend)(
        ((id(*)(id, SEL))objc_msgSend)(config, "preferences"_sel),
        "setValue:forKey:"_sel,
        ((id(*)(id, SEL, BOOL))objc_msgSend)("NSNumber"_cls,
                                             "numberWithBool:"_sel, 1),
        "fullScreenEnabled"_str);

    // Equivalent Obj-C:
    // [[config preferences] setValue:@YES forKey:@"javaScriptCanAccessClipboard"];
    ((id(*)(id, SEL, id, id))objc_msgSend)(
        ((id(*)(id, SEL))objc_msgSend)(config, "preferences"_sel),
        "setValue:forKey:"_sel,
        ((id(*)(id, SEL, BOOL))objc_msgSend)("NSNumber"_cls,
                                             "numberWithBool:"_sel, 1),
        "javaScriptCanAccessClipboard"_str);

    // Equivalent Obj-C:
    // [[config preferences] setValue:@YES forKey:@"DOMPasteAllowed"];
    ((id(*)(id, SEL, id, id))objc_msgSend)(
        ((id(*)(id, SEL))objc_msgSend)(config, "preferences"_sel),
        "setValue:forKey:"_sel,
        ((id(*)(id, SEL, BOOL))objc_msgSend)("NSNumber"_cls,
                                             "numberWithBool:"_sel, 1),
        "DOMPasteAllowed"_str);

    ((void (*)(id, SEL, CGRect, id))objc_msgSend)(
        m_webview, "initWithFrame:configuration:"_sel, CGRectMake(0, 0, 0, 0),
        config);
    ((void (*)(id, SEL, id, id))objc_msgSend)(
        m_manager, "addScriptMessageHandler:name:"_sel, delegate,
        "external"_str);

    init(R"script(
                      window.external = {
                        invoke: function(s) {
                          window.webkit.messageHandlers.external.postMessage(s);
                        },
                      };
                     )script");
    ((void (*)(id, SEL, id))objc_msgSend)(m_window, "setContentView:"_sel,
                                          m_webview);
    ((void (*)(id, SEL, id))objc_msgSend)(m_window, "makeKeyAndOrderFront:"_sel,
                                          nullptr);
  }
  virtual ~cocoa_wkwebview_engine() { close(); }
  void *window() { return (void *)m_window; }
  void terminate() {
    close();
    ((void (*)(id, SEL, id))objc_msgSend)("NSApp"_cls, "terminate:"_sel,
                                          nullptr);
  }
  void run() {
    id app = ((id(*)(id, SEL))objc_msgSend)("NSApplication"_cls,
                                            "sharedApplication"_sel);
    dispatch([&]() {
      ((void (*)(id, SEL, BOOL))objc_msgSend)(
          app, "activateIgnoringOtherApps:"_sel, 1);
    });
    ((void (*)(id, SEL))objc_msgSend)(app, "run"_sel);
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
    ((void (*)(id, SEL, id))objc_msgSend)(
        m_window, "setTitle:"_sel,
        ((id(*)(id, SEL, const char *))objc_msgSend)(
            "NSString"_cls, "stringWithUTF8String:"_sel, title.c_str()));
  }
  void set_size(int width, int height, int hints) {
    auto style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                 NSWindowStyleMaskMiniaturizable;
    if (hints != WEBVIEW_HINT_FIXED) {
      style = style | NSWindowStyleMaskResizable;
    }
    ((void (*)(id, SEL, unsigned long))objc_msgSend)(
        m_window, "setStyleMask:"_sel, style);

    if (hints == WEBVIEW_HINT_MIN) {
      ((void (*)(id, SEL, CGSize))objc_msgSend)(
          m_window, "setContentMinSize:"_sel, CGSizeMake(width, height));
    } else if (hints == WEBVIEW_HINT_MAX) {
      ((void (*)(id, SEL, CGSize))objc_msgSend)(
          m_window, "setContentMaxSize:"_sel, CGSizeMake(width, height));
    } else {
      ((void (*)(id, SEL, CGRect, BOOL, BOOL))objc_msgSend)(
          m_window, "setFrame:display:animate:"_sel,
          CGRectMake(0, 0, width, height), 1, 0);
    }
    ((void (*)(id, SEL))objc_msgSend)(m_window, "center"_sel);
  }
  void navigate(const std::string &url) {
    auto nsurl = ((id(*)(id, SEL, id))objc_msgSend)(
        "NSURL"_cls, "URLWithString:"_sel,
        ((id(*)(id, SEL, const char *))objc_msgSend)(
            "NSString"_cls, "stringWithUTF8String:"_sel, url.c_str()));

    ((void (*)(id, SEL, id))objc_msgSend)(
        m_webview, "loadRequest:"_sel,
        ((id(*)(id, SEL, id))objc_msgSend)("NSURLRequest"_cls,
                                           "requestWithURL:"_sel, nsurl));
  }
  void set_html(const std::string &html) {
    ((void (*)(id, SEL, id, id))objc_msgSend)(
        m_webview, "loadHTMLString:baseURL:"_sel,
        ((id(*)(id, SEL, const char *))objc_msgSend)(
            "NSString"_cls, "stringWithUTF8String:"_sel, html.c_str()),
        nullptr);
  }
  void init(const std::string &js) {
    // Equivalent Obj-C:
    // [m_manager addUserScript:[[WKUserScript alloc] initWithSource:[NSString stringWithUTF8String:js.c_str()] injectionTime:WKUserScriptInjectionTimeAtDocumentStart forMainFrameOnly:YES]]
    ((void (*)(id, SEL, id))objc_msgSend)(
        m_manager, "addUserScript:"_sel,
        ((id(*)(id, SEL, id, long, BOOL))objc_msgSend)(
            ((id(*)(id, SEL))objc_msgSend)("WKUserScript"_cls, "alloc"_sel),
            "initWithSource:injectionTime:forMainFrameOnly:"_sel,
            ((id(*)(id, SEL, const char *))objc_msgSend)(
                "NSString"_cls, "stringWithUTF8String:"_sel, js.c_str()),
            WKUserScriptInjectionTimeAtDocumentStart, 1));
  }
  void eval(const std::string &js) {
    ((void (*)(id, SEL, id, id))objc_msgSend)(
        m_webview, "evaluateJavaScript:completionHandler:"_sel,
        ((id(*)(id, SEL, const char *))objc_msgSend)(
            "NSString"_cls, "stringWithUTF8String:"_sel, js.c_str()),
        nullptr);
  }

private:
  virtual void on_message(const std::string &msg) = 0;
  void close() { ((void (*)(id, SEL))objc_msgSend)(m_window, "close"_sel); }
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

class win32_edge_engine {
public:
  explicit win32_edge_engine(const webview_create_options_t &options) {
    enable_dpi_awareness();

    if (!options.window) {
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
      if (!RegisterClassExW(&wc)) {
        throw webview_exception();
      }
      m_window = CreateWindowW(L"webview", L"", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr,
                               nullptr, hInstance, nullptr);
      if (!m_window) {
        throw webview_exception();
      }
      SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);
    } else {
      m_window = *(static_cast<HWND *>(options.window));
    }

    ShowWindow(m_window, SW_SHOW);
    UpdateWindow(m_window);
    SetFocus(m_window);

    auto cb =
        std::bind(&win32_edge_engine::on_message, this, std::placeholders::_1);

    embed(m_window, options.debug, cb);
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
    while ((res = GetMessage(&msg, nullptr, 0, 0)) != 0) {
      if (res == -1) {
        throw webview_exception();
      }
      switch (msg.message) {
      case app_window_message::dispatch: {
        auto f = (dispatch_fn_t *)(msg.lParam);
        (*f)();
        delete f;
      } break;
      case WM_QUIT:
        return;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  void *window() { return (void *)m_window; }
  void terminate() { PostQuitMessage(0); }
  void dispatch(dispatch_fn_t f) {
    PostThreadMessage(m_main_thread, app_window_message::dispatch, 0,
                      (LPARAM) new dispatch_fn_t(f));
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
  struct app_window_message {
    enum type : UINT {
      dispatch = WM_APP,
      webview_ready = WM_APP + 1,
      webview_initialization_failed = WM_APP + 2
    };
  };

  void embed(HWND wnd, bool debug, msg_cb_t cb) {
    wchar_t currentExePath[MAX_PATH];
    GetModuleFileNameW(NULL, currentExePath, MAX_PATH);
    wchar_t *currentExeName = PathFindFileNameW(currentExePath);

    wchar_t dataPath[MAX_PATH];
    if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, dataPath))) {
      throw webview_exception();
    }
    wchar_t userDataFolder[MAX_PATH];
    PathCombineW(userDataFolder, dataPath, currentExeName);

    m_com_handler = new webview2_com_handler(
        wnd, cb,
        [this, wnd](ICoreWebView2Controller *controller,
                    ICoreWebView2 *webview) {
          controller->AddRef();
          webview->AddRef();
          m_controller = controller;
          m_webview = webview;
          PostMessage(wnd, app_window_message::webview_ready, 0, 0);
        });
    HRESULT create_result = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, userDataFolder, nullptr, m_com_handler);
    if (!SUCCEEDED(create_result)) {
      // See specific errors we could detect:
      // https://docs.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/icorewebview2environment
      throw webview_exception();
    }

    MSG msg;
    BOOL bres;
    bool is_initialized = false;
    while (!is_initialized && (bres = GetMessage(&msg, nullptr, 0, 0)) != 0) {
      if (bres == -1) {
        throw webview_exception();
      }
      switch (msg.message) {
      case app_window_message::webview_ready: {
        if (!m_controller) {
          throw webview_exception(webview_error_invalid_state);
        }
        is_initialized = true;
        break;
      }
      case app_window_message::webview_initialization_failed:
        throw webview_exception();
      default:
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        break;
      }
    }
    init("window.external={invoke:s=>window.chrome.webview.postMessage(s)}");
  }

  void resize(HWND wnd) {
    if (m_controller == nullptr) {
      return;
    }
    RECT bounds;
    GetClientRect(wnd, &bounds);
    m_controller->put_Bounds(bounds);
  }

  virtual void on_message(const std::string &msg) = 0;

  class webview2_com_handler
      : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler,
        public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
        public ICoreWebView2WebMessageReceivedEventHandler,
        public ICoreWebView2PermissionRequestedEventHandler {
    using webview2_com_handler_cb_t =
        std::function<void(ICoreWebView2Controller *, ICoreWebView2 *webview)>;

  public:
    webview2_com_handler(HWND hwnd, msg_cb_t msgCb,
                         webview2_com_handler_cb_t cb)
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
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT res,
                                     ICoreWebView2Environment *env) {
      if (!SUCCEEDED(res)) {
        PostMessage(m_window, app_window_message::webview_initialization_failed,
                    0, 0);
        return res;
      }
      // Loop either until successful or while the error is ERROR_INVALID_STATE,
      // as suggested in the WebView2 docs:
      // https://docs.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/icorewebview2environment
      // TODO: The docs also recommends starting over from creating the
      // environment.
      while (true) {
        res = env->CreateCoreWebView2Controller(m_window, this);
        if (SUCCEEDED(res)) {
          break;
        } else if (res != HRESULT_FROM_WIN32(ERROR_INVALID_STATE)) {
          PostMessage(m_window,
                      app_window_message::webview_initialization_failed, 0, 0);
          return res;
        }
      }
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT res,
                                     ICoreWebView2Controller *controller) {
      if (!SUCCEEDED(res)) {
        PostMessage(m_window, app_window_message::webview_initialization_failed,
                    0, 0);
        return res;
      }

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
    HRESULT STDMETHODCALLTYPE
    Invoke(ICoreWebView2 *sender,
           ICoreWebView2PermissionRequestedEventArgs *args) {
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
    std::atomic<ULONG> m_ref_count = 1;
  };

  HWND m_window;
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
  WEBVIEW_DEPRECATED(
      "Please use the constructor that takes options as a struct")
  webview(bool debug = false, void *wnd = nullptr)
      : browser_engine(detail::migrate_webview_create_options(debug, wnd)) {}

  explicit webview(const webview_create_options_t &options)
      : browser_engine(options) {}

  void navigate(const std::string &url) {
    if (url == "") {
      browser_engine::navigate("about:blank");
      return;
    }
    browser_engine::navigate(url);
  }

  using binding_t = std::function<void(std::string, std::string, void *)>;
  using binding_ctx_t = std::pair<binding_t *, void *>;

  using sync_binding_t = std::function<std::string(std::string)>;
  using sync_binding_ctx_t = std::pair<webview *, sync_binding_t>;

  void bind(const std::string &name, sync_binding_t fn) {
    bind(
        name,
        [](const std::string &seq, const std::string &req, void *arg) {
          auto pair = static_cast<sync_binding_ctx_t *>(arg);
          pair->first->resolve(seq, 0, pair->second(req));
        },
        new sync_binding_ctx_t(this, fn));
  }

  void bind(const std::string &name, binding_t f, void *arg) {
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
    bindings[name] = new binding_ctx_t(new binding_t(f), arg);
  }

  void unbind(const std::string &name) {
    if (bindings.find(name) != bindings.end()) {
      auto js = "delete window['" + name + "'];";
      init(js);
      eval(js);
      delete bindings[name]->first;
      delete static_cast<sync_binding_ctx_t *>(bindings[name]->second);
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
  void on_message(const std::string &msg) {
    auto seq = detail::json_parse(msg, "id", 0);
    auto name = detail::json_parse(msg, "method", 0);
    auto args = detail::json_parse(msg, "params", 0);
    if (bindings.find(name) == bindings.end()) {
      return;
    }
    auto fn = bindings[name];
    (*fn->first)(seq, args, fn->second);
  }
  std::map<std::string, binding_ctx_t *> bindings;
};
} // namespace webview

WEBVIEW_API webview_t webview_create(int debug, void *wnd) {
  auto options = webview::detail::migrate_webview_create_options(debug, wnd);
  webview_t w = nullptr;
  if (webview_create_with_options(&w, &options) != webview_error_ok) {
    return nullptr;
  }
  return w;
}

WEBVIEW_API webview_error_code_t webview_create_with_options(
    webview_t *w, const webview_create_options_t *options) {
  if (!w || !options) {
    return webview_error_invalid_argument;
  }
  if (webview::detail::compare_versions(options->api_version,
                                        webview::detail::min_api_version) < 0) {
    return webview_error_api_version_too_old;
  }
  if (webview::detail::compare_versions(options->api_version,
                                        webview::api_version) > 0) {
    return webview_error_api_version_too_new;
  }
  try {
    *w = new webview::webview(*options);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_destroy(webview_t w) {
  if (!w) {
    return webview_error_invalid_argument;
  }
  delete static_cast<webview::webview *>(w);
  return webview_error_ok;
}

WEBVIEW_API webview_error_code_t webview_run(webview_t w) {
  if (!w) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->run();
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_terminate(webview_t w) {
  if (!w) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->terminate();
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_dispatch(webview_t w,
                                                  void (*fn)(webview_t, void *),
                                                  void *arg) {
  if (!w || !fn) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->dispatch([=]() { fn(w, arg); });
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API void *webview_get_window(webview_t w) {
  if (!w) {
    return nullptr;
  }
  return static_cast<webview::webview *>(w)->window();
}

WEBVIEW_API webview_error_code_t webview_set_title(webview_t w,
                                                   const char *title) {
  if (!w || !title) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->set_title(title);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_set_size(webview_t w, int width,
                                                  int height, int hints) {
  if (!w) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->set_size(width, height, hints);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_navigate(webview_t w,
                                                  const char *url) {
  if (!w || !url) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->navigate(url);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_set_html(webview_t w,
                                                  const char *html) {
  if (!w || !html) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->set_html(html);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_init(webview_t w, const char *js) {
  if (!w || !js) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->init(js);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_eval(webview_t w, const char *js) {
  if (!w || !js) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->eval(js);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_bind(
    webview_t w, const char *name,
    void (*fn)(const char *seq, const char *req, void *arg), void *arg) {
  if (!w || !name || !fn) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->bind(
        name,
        [=](const std::string &seq, const std::string &req, void *arg) {
          fn(seq.c_str(), req.c_str(), arg);
        },
        arg);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_unbind(webview_t w, const char *name) {
  if (!w || !name) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->unbind(name);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_error_code_t webview_return(webview_t w, const char *seq,
                                                int status,
                                                const char *result) {
  if (!w || !seq || !result) {
    return webview_error_invalid_argument;
  }
  try {
    static_cast<webview::webview *>(w)->resolve(seq, status, result);
    return webview_error_ok;
  } catch (const webview::webview_exception &e) {
    return e.code();
  }
}

WEBVIEW_API webview_version_t webview_library_version() {
  return webview::library_version;
}

WEBVIEW_API webview_version_t webview_api_version() {
  return webview::api_version;
}

#endif /* WEBVIEW_HEADER */
#endif /* __cplusplus */
#endif /* WEBVIEW_H */
