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

#ifndef WEBVIEW_PLATFORM_LINUX_WEBKITGTK_COMPAT_HH
#define WEBVIEW_PLATFORM_LINUX_WEBKITGTK_COMPAT_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include <functional>
#include <string>

#include <gtk/gtk.h>

#if GTK_MAJOR_VERSION >= 4

#include <jsc/jsc.h>
#include <webkit/webkit.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/x11/gdkx.h>
#endif

#elif GTK_MAJOR_VERSION >= 3

#include <JavaScriptCore/JavaScript.h>
#include <webkit2/webkit2.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#endif

namespace webview {
namespace detail {

/**
 * WebKitGTK compatibility helper class.
 */
class webkitgtk_compat {
public:
#if GTK_MAJOR_VERSION >= 4
  using wk_handler_js_value_t = JSCValue;
#else
  using wk_handler_js_value_t = WebKitJavascriptResult;
#endif

  using on_script_message_received_t =
      std::function<void(WebKitUserContentManager *, const std::string &)>;
  static void
  connect_script_message_received(WebKitUserContentManager *manager,
                                  const std::string &handler_name,
                                  on_script_message_received_t handler) {
    std::string signal_name = "script-message-received::";
    signal_name += handler_name;

    auto callback = +[](WebKitUserContentManager *manager,
                        wk_handler_js_value_t *r, gpointer arg) {
      auto *handler = static_cast<on_script_message_received_t *>(arg);
      (*handler)(manager, get_string_from_js_result(r));
    };

    auto deleter = +[](gpointer data, GClosure *) {
      delete static_cast<on_script_message_received_t *>(data);
    };

    g_signal_connect_data(manager, signal_name.c_str(), G_CALLBACK(callback),
                          new on_script_message_received_t{handler}, deleter,
                          static_cast<GConnectFlags>(0) /*G_CONNECT_DEFAULT*/);
  }

  static std::string get_string_from_js_result(JSCValue *r) {
    char *cs = jsc_value_to_string(r);
    std::string s{cs};
    g_free(cs);
    return s;
  }

#if GTK_MAJOR_VERSION < 4
  static std::string get_string_from_js_result(WebKitJavascriptResult *r) {
#if (WEBKIT_MAJOR_VERSION == 2 && WEBKIT_MINOR_VERSION >= 22) ||               \
    WEBKIT_MAJOR_VERSION > 2
    JSCValue *value = webkit_javascript_result_get_js_value(r);
    return get_string_from_js_result(value);
#else
    JSGlobalContextRef ctx = webkit_javascript_result_get_global_context(r);
    JSValueRef value = webkit_javascript_result_get_value(r);
    JSStringRef js = JSValueToStringCopy(ctx, value, nullptr);
    size_t n = JSStringGetMaximumUTF8CStringSize(js);
    char *cs = g_new(char, n);
    JSStringGetUTF8CString(js, cs, n);
    JSStringRelease(js);
    std::string s{cs};
    g_free(cs);
    return s;
#endif
  }
#endif

  static void user_content_manager_register_script_message_handler(
      WebKitUserContentManager *manager, const gchar *name) {
#if GTK_MAJOR_VERSION >= 4
    webkit_user_content_manager_register_script_message_handler(manager, name,
                                                                nullptr);
#else
    webkit_user_content_manager_register_script_message_handler(manager, name);
#endif
  }
};

} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_LINUX_WEBKITGTK_COMPAT_HH
