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

#ifndef WEBVIEW_BACKEND_GTK_WEBKITGTK_HH
#define WEBVIEW_BACKEND_GTK_WEBKITGTK_HH

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
#include "../user_script.hh"

#include <cstdlib>
#include <functional>
#include <list>
#include <memory>
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

#include <fcntl.h>
#include <sys/stat.h>

namespace webview {
namespace detail {

// Namespace containing workaround for WebKit 2.42 when using NVIDIA GPU
// driver.
// See WebKit bug: https://bugs.webkit.org/show_bug.cgi?id=261874
// Please remove all of the code in this namespace when it's no longer needed.
namespace webkit_dmabuf {

// Get environment variable. Not thread-safe.
static inline std::string get_env(const std::string &name) {
  auto *value = std::getenv(name.c_str());
  if (value) {
    return {value};
  }
  return {};
}

// Set environment variable. Not thread-safe.
static inline void set_env(const std::string &name, const std::string &value) {
  ::setenv(name.c_str(), value.c_str(), 1);
}

// Checks whether the NVIDIA GPU driver is used based on whether the kernel
// module is loaded.
static inline bool is_using_nvidia_driver() {
  struct ::stat buffer {};
  if (::stat("/sys/module/nvidia", &buffer) != 0) {
    return false;
  }
  return S_ISDIR(buffer.st_mode);
}

// Checks whether the windowing system is Wayland.
static inline bool is_wayland_display() {
  if (!get_env("WAYLAND_DISPLAY").empty()) {
    return true;
  }
  if (get_env("XDG_SESSION_TYPE") == "wayland") {
    return true;
  }
  if (get_env("DESKTOP_SESSION").find("wayland") != std::string::npos) {
    return true;
  }
  return false;
}

// Checks whether the GDK X11 backend is used.
// See: https://docs.gtk.org/gdk3/class.DisplayManager.html
static inline bool is_gdk_x11_backend() {
#ifdef GDK_WINDOWING_X11
  auto *gdk_display = gdk_display_get_default();
  return GDK_IS_X11_DISPLAY(gdk_display); // NOLINT(misc-const-correctness)
#else
  return false;
#endif
}

// Checks whether WebKit is affected by bug when using DMA-BUF renderer.
// Returns true if all of the following conditions are met:
//  - WebKit version is >= 2.42 (please narrow this down when there's a fix).
//  - Environment variables are empty or not set:
//    - WEBKIT_DISABLE_DMABUF_RENDERER
//  - Windowing system is not Wayland.
//  - GDK backend is X11.
//  - NVIDIA GPU driver is used.
static inline bool is_webkit_dmabuf_bugged() {
  auto wk_major = webkit_get_major_version();
  auto wk_minor = webkit_get_minor_version();
  // TODO: Narrow down affected WebKit version when there's a fixed version
  auto is_affected_wk_version = wk_major == 2 && wk_minor >= 42;
  if (!is_affected_wk_version) {
    return false;
  }
  if (!get_env("WEBKIT_DISABLE_DMABUF_RENDERER").empty()) {
    return false;
  }
  if (is_wayland_display()) {
    return false;
  }
  if (!is_gdk_x11_backend()) {
    return false;
  }
  if (!is_using_nvidia_driver()) {
    return false;
  }
  return true;
}

// Applies workaround for WebKit DMA-BUF bug if needed.
// See WebKit bug: https://bugs.webkit.org/show_bug.cgi?id=261874
static inline void apply_webkit_dmabuf_workaround() {
  if (!is_webkit_dmabuf_bugged()) {
    return;
  }
  set_env("WEBKIT_DISABLE_DMABUF_RENDERER", "1");
}
} // namespace webkit_dmabuf

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

/**
 * GTK compatibility helper class.
 */
class gtk_compat {
public:
  static gboolean init_check() {
#if GTK_MAJOR_VERSION >= 4
    return gtk_init_check();
#else
    return gtk_init_check(nullptr, nullptr);
#endif
  }

  static GtkWidget *window_new() {
#if GTK_MAJOR_VERSION >= 4
    return gtk_window_new();
#else
    return gtk_window_new(GTK_WINDOW_TOPLEVEL);
#endif
  }

  static void window_set_child(GtkWindow *window, GtkWidget *widget) {
#if GTK_MAJOR_VERSION >= 4
    gtk_window_set_child(window, widget);
#else
    gtk_container_add(GTK_CONTAINER(window), widget);
#endif
  }

  static void window_remove_child(GtkWindow *window, GtkWidget *widget) {
#if GTK_MAJOR_VERSION >= 4
    if (gtk_window_get_child(window) == widget) {
      gtk_window_set_child(window, nullptr);
    }
#else
    gtk_container_remove(GTK_CONTAINER(window), widget);
#endif
  }

  static void widget_set_visible(GtkWidget *widget, bool visible) {
#if GTK_MAJOR_VERSION >= 4
    gtk_widget_set_visible(widget, visible ? TRUE : FALSE);
#else
    if (visible) {
      gtk_widget_show(widget);
    } else {
      gtk_widget_hide(widget);
    }
#endif
  }

  static void window_set_size(GtkWindow *window, int width, int height) {
#if GTK_MAJOR_VERSION >= 4
    gtk_window_set_default_size(window, width, height);
#else
    gtk_window_resize(window, width, height);
#endif
  }

  static void window_set_max_size(GtkWindow *window, int width, int height) {
// X11-specific features are available in GTK 3 but not GTK 4
#if GTK_MAJOR_VERSION < 4
    GdkGeometry g{};
    g.max_width = width;
    g.max_height = height;
    GdkWindowHints h = GDK_HINT_MAX_SIZE;
    gtk_window_set_geometry_hints(GTK_WINDOW(window), nullptr, &g, h);
#else
    // Avoid "unused parameter" warnings
    (void)window;
    (void)width;
    (void)height;
#endif
  }
};

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

class gtk_webkit_engine : public engine_base {
public:
  gtk_webkit_engine(bool debug, void *window)
      : m_owns_window{!window}, m_window(static_cast<GtkWidget *>(window)) {
    if (m_owns_window) {
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

    gtk_compat::window_set_child(GTK_WINDOW(m_window), GTK_WIDGET(m_webview));
    gtk_compat::widget_set_visible(GTK_WIDGET(m_webview), true);

    WebKitSettings *settings =
        webkit_web_view_get_settings(WEBKIT_WEB_VIEW(m_webview));
    webkit_settings_set_javascript_can_access_clipboard(settings, true);
    if (debug) {
      webkit_settings_set_enable_write_console_messages_to_stdout(settings,
                                                                  true);
      webkit_settings_set_enable_developer_extras(settings, true);
    }

    if (m_owns_window) {
      gtk_widget_grab_focus(GTK_WIDGET(m_webview));
      gtk_compat::widget_set_visible(GTK_WIDGET(m_window), true);
    }
  }

  gtk_webkit_engine(const gtk_webkit_engine &) = delete;
  gtk_webkit_engine &operator=(const gtk_webkit_engine &) = delete;
  gtk_webkit_engine(gtk_webkit_engine &&) = delete;
  gtk_webkit_engine &operator=(gtk_webkit_engine &&) = delete;

  virtual ~gtk_webkit_engine() {
    if (m_window) {
      if (m_owns_window) {
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
    if (m_owns_window) {
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
    if (hints == WEBVIEW_HINT_NONE) {
      gtk_compat::window_set_size(GTK_WINDOW(m_window), width, height);
    } else if (hints == WEBVIEW_HINT_FIXED || hints == WEBVIEW_HINT_MIN) {
      gtk_widget_set_size_request(m_window, width, height);
    } else if (hints == WEBVIEW_HINT_MAX) {
      gtk_compat::window_set_max_size(GTK_WINDOW(m_window), width, height);
    }
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, "Invalid hint"};
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
    user_script script{js, std::unique_ptr<user_script::impl>{
                               new user_script::impl{wk_script}}};
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

  // Blocks while depleting the run loop of events.
  void deplete_run_loop_event_queue() {
    bool done{};
    dispatch([&] { done = true; });
    while (!done) {
      g_main_context_iteration(nullptr, TRUE);
    }
  }

  bool m_owns_window{};
  GtkWidget *m_window{};
  GtkWidget *m_webview{};
  WebKitUserContentManager *m_user_content_manager{};
  bool m_stop_run_loop{};
};

} // namespace detail

using browser_engine = detail::gtk_webkit_engine;

} // namespace webview

#endif // WEBVIEW_BACKEND_GTK_WEBKITGTK_H
