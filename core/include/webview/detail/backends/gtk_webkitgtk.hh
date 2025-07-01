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

#ifndef WEBVIEW_BACKENDS_GTK_WEBKITGTK_HH
#define WEBVIEW_BACKENDS_GTK_WEBKITGTK_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../macros.h"

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

//
// ====================================================================
//
// This implementation uses webkit2gtk backend. It requires GTK and
// WebKitGTK libraries. Proper compiler flags can be retrieved via:
//
//   pkg-config --cflags --libs gtk4 webkitgtk-6.0
//   pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.1
//   pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0
//
// ====================================================================
//

#include "../../errors.hh"
#include "../../types.hh"
#include "../engine_base.hh"
#include "../platform/linux/gtk/compat.hh"
#include "../platform/linux/webkitgtk/compat.hh"
#include "../platform/linux/webkitgtk/dmabuf.hh"
#include "../user_script.hh"

#include <functional>
#include <list>
#include <memory>
#include <string>

#include <gtk/gtk.h>

#if GTK_MAJOR_VERSION >= 4

#include <jsc/jsc.h>
#include <webkit/webkit.h>

#elif GTK_MAJOR_VERSION >= 3

#include <JavaScriptCore/JavaScript.h>
#include <webkit2/webkit2.h>

#endif

#include <fcntl.h>
#include <sys/stat.h>

namespace webview {
namespace detail {

class user_script::impl {
public:
  impl(WebKitUserScript *script) : m_script{script} {
    webkit_user_script_ref(script);
  }

  ~impl() { webkit_user_script_unref(m_script); }

  impl(const impl &) = delete;
  impl &operator=(const impl &) = delete;
  impl(impl &&) = delete;
  impl &operator=(impl &&) = delete;

  WebKitUserScript *get_native() const { return m_script; }

private:
  WebKitUserScript *m_script{};
};

class gtk_webkit_engine : public engine_base {
public:
  gtk_webkit_engine(bool debug, void *window) : engine_base{!window} {
    window_init(window);
    window_settings(debug);
    dispatch_size_default();
  }

  gtk_webkit_engine(const gtk_webkit_engine &) = delete;
  gtk_webkit_engine &operator=(const gtk_webkit_engine &) = delete;
  gtk_webkit_engine(gtk_webkit_engine &&) = delete;
  gtk_webkit_engine &operator=(gtk_webkit_engine &&) = delete;

  virtual ~gtk_webkit_engine() {
    if (m_window) {
      if (owns_window()) {
        // Disconnect handlers to avoid callbacks invoked during destruction.
        g_signal_handlers_disconnect_by_data(GTK_WINDOW(m_window), this);
        gtk_window_close(GTK_WINDOW(m_window));
        on_window_destroyed(true);
      } else {
        gtk_compat::window_remove_child(GTK_WINDOW(m_window),
                                        GTK_WIDGET(m_webview));
      }
    }
    if (m_webview) {
      g_object_unref(m_webview);
    }
    if (owns_window()) {
      // Needed for the window to close immediately.
      deplete_run_loop_event_queue();
    }
  }

protected:
  result<void *> window_impl() override {
    if (m_window) {
      return m_window;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }

  result<void *> widget_impl() override {
    if (m_webview) {
      return m_webview;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }

  result<void *> browser_controller_impl() override {
    if (m_webview) {
      return m_webview;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }

  noresult run_impl() override {
    m_stop_run_loop = false;
    while (!m_stop_run_loop) {
      g_main_context_iteration(nullptr, TRUE);
    }
    return {};
  }

  noresult terminate_impl() override {
    return dispatch_impl([&] { m_stop_run_loop = true; });
  }

  noresult dispatch_impl(std::function<void()> f) override {
    g_idle_add_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)([](void *fn) -> int {
                      (*static_cast<dispatch_fn_t *>(fn))();
                      return G_SOURCE_REMOVE;
                    }),
                    new std::function<void()>(f),
                    [](void *fn) { delete static_cast<dispatch_fn_t *>(fn); });
    return {};
  }

  noresult set_title_impl(const std::string &title) override {
    gtk_window_set_title(GTK_WINDOW(m_window), title.c_str());
    return {};
  }

  noresult set_size_impl(int width, int height, webview_hint_t hints) override {
    gtk_window_set_resizable(GTK_WINDOW(m_window), hints != WEBVIEW_HINT_FIXED);
    if (hints == WEBVIEW_HINT_NONE || hints == WEBVIEW_HINT_FIXED) {
      gtk_compat::window_set_size(GTK_WINDOW(m_window), width, height);
    } else if (hints == WEBVIEW_HINT_MIN) {
      gtk_widget_set_size_request(m_window, width, height);
    } else if (hints == WEBVIEW_HINT_MAX) {
      gtk_compat::window_set_max_size(GTK_WINDOW(m_window), width, height);
    } else {
      return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, "Invalid hint"};
    }
    return window_show();
  }

  noresult navigate_impl(const std::string &url) override {
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(m_webview), url.c_str());
    return {};
  }

  noresult set_html_impl(const std::string &html) override {
    webkit_web_view_load_html(WEBKIT_WEB_VIEW(m_webview), html.c_str(),
                              nullptr);
    return {};
  }

  noresult eval_impl(const std::string &js) override {
    // URI is null before content has begun loading.
    if (!webkit_web_view_get_uri(WEBKIT_WEB_VIEW(m_webview))) {
      return {};
    }
#if (WEBKIT_MAJOR_VERSION == 2 && WEBKIT_MINOR_VERSION >= 40) ||               \
    WEBKIT_MAJOR_VERSION > 2
    webkit_web_view_evaluate_javascript(WEBKIT_WEB_VIEW(m_webview), js.c_str(),
                                        static_cast<gssize>(js.size()), nullptr,
                                        nullptr, nullptr, nullptr, nullptr);
#else
    webkit_web_view_run_javascript(WEBKIT_WEB_VIEW(m_webview), js.c_str(),
                                   nullptr, nullptr, nullptr);
#endif
    return {};
  }

  user_script add_user_script_impl(const std::string &js) override {
    auto *wk_script = webkit_user_script_new(
        js.c_str(), WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
        WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START, nullptr, nullptr);
    webkit_user_content_manager_add_script(m_user_content_manager, wk_script);
    user_script script{
        js, user_script::impl_ptr{new user_script::impl{wk_script},
                                  [](user_script::impl *p) { delete p; }}};
    webkit_user_script_unref(wk_script);
    return script;
  }

  void remove_all_user_scripts_impl(
      const std::list<user_script> & /*scripts*/) override {
    webkit_user_content_manager_remove_all_scripts(m_user_content_manager);
  }

  bool are_user_scripts_equal_impl(const user_script &first,
                                   const user_script &second) override {
    auto *wk_first = first.get_impl().get_native();
    auto *wk_second = second.get_impl().get_native();
    return wk_first == wk_second;
  }

private:
#if GTK_MAJOR_VERSION >= 4
  static char *get_string_from_js_result(JSCValue *r) {
    return jsc_value_to_string(r);
  }
#else
  static char *get_string_from_js_result(WebKitJavascriptResult *r) {
    char *s;
#if (WEBKIT_MAJOR_VERSION == 2 && WEBKIT_MINOR_VERSION >= 22) ||               \
    WEBKIT_MAJOR_VERSION > 2
    JSCValue *value = webkit_javascript_result_get_js_value(r);
    s = jsc_value_to_string(value);
#else
    JSGlobalContextRef ctx = webkit_javascript_result_get_global_context(r);
    JSValueRef value = webkit_javascript_result_get_value(r);
    JSStringRef js = JSValueToStringCopy(ctx, value, nullptr);
    size_t n = JSStringGetMaximumUTF8CStringSize(js);
    s = g_new(char, n);
    JSStringGetUTF8CString(js, s, n);
    JSStringRelease(js);
#endif
    return s;
  }
#endif

  void window_init(void *window) {
    m_window = static_cast<GtkWidget *>(window);
    if (owns_window()) {
      if (!gtk_compat::init_check()) {
        throw exception{WEBVIEW_ERROR_UNSPECIFIED, "GTK init failed"};
      }
      m_window = gtk_compat::window_new();
      on_window_created();
      auto on_window_destroy = +[](GtkWidget *, gpointer arg) {
        auto *w = static_cast<gtk_webkit_engine *>(arg);
        w->m_window = nullptr;
        w->on_window_destroyed();
      };
      g_signal_connect(G_OBJECT(m_window), "destroy",
                       G_CALLBACK(on_window_destroy), this);
    }
    webkit_dmabuf::apply_webkit_dmabuf_workaround();
    // Initialize webview widget
    m_webview = webkit_web_view_new();
    g_object_ref_sink(m_webview);
    WebKitUserContentManager *manager = m_user_content_manager =
        webkit_web_view_get_user_content_manager(WEBKIT_WEB_VIEW(m_webview));
    webkitgtk_compat::connect_script_message_received(
        manager, "__webview__",
        [this](WebKitUserContentManager *, const std::string &r) {
          on_message(r);
        });
    webkitgtk_compat::user_content_manager_register_script_message_handler(
        manager, "__webview__");
    add_init_script("function(message) {\n\
  return window.webkit.messageHandlers.__webview__.postMessage(message);\n\
}");
  }

  void window_settings(bool debug) {
    WebKitSettings *settings =
        webkit_web_view_get_settings(WEBKIT_WEB_VIEW(m_webview));
    webkit_settings_set_javascript_can_access_clipboard(settings, true);
    if (debug) {
      webkit_settings_set_enable_write_console_messages_to_stdout(settings,
                                                                  true);
      webkit_settings_set_enable_developer_extras(settings, true);
    }
  }

  noresult window_show() {
    if (m_is_window_shown) {
      return {};
    }
    gtk_compat::window_set_child(GTK_WINDOW(m_window), GTK_WIDGET(m_webview));
    gtk_compat::widget_set_visible(GTK_WIDGET(m_webview), true);

    if (owns_window()) {
      gtk_widget_grab_focus(GTK_WIDGET(m_webview));
      gtk_compat::widget_set_visible(GTK_WIDGET(m_window), true);
    }
    m_is_window_shown = true;
    return {};
  }

  void run_event_loop_while(std::function<bool()> fn) override {
    while (fn()) {
      g_main_context_iteration(nullptr, TRUE);
    }
  }

  GtkWidget *m_window{};
  GtkWidget *m_webview{};
  WebKitUserContentManager *m_user_content_manager{};
  bool m_stop_run_loop{};
  bool m_is_window_shown{};
};

} // namespace detail

using browser_engine = detail::gtk_webkit_engine;

} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_BACKENDS_GTK_WEBKITGTK_HH
