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

#ifndef WEBVIEW_DETAIL_ENGINE_BASE_HH
#define WEBVIEW_DETAIL_ENGINE_BASE_HH
#include "webview/detail/engine_js.hh"
#include <algorithm>
#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../types.h"
#include "../types.hh"
#include "user_script.hh"
#include <atomic>
#include <list>
#include <map>

namespace webview {
namespace detail {
using noresult = webview::noresult;
using binding_t = std::function<void(std::string, std::string, void *)>;
using sync_binding_t = std::function<std::string(std::string)>;

/// Common internal API methods for all three Webview platform classes:
/// - cocoa_webkit
/// - gtk_webkitgtk
/// - win32_edge
class engine_base {
public:
  engine_base(bool owns_window) : m_owns_window{owns_window} {}

  virtual ~engine_base() = default;

  /// Internal API implementation of public \ref webview_navigate
  noresult navigate(const std::string &url);

  /// Internal API implementation of public \ref webview_bind (synchronous)
  noresult bind(const std::string &name, sync_binding_t fn);
  /// Internal API implementation of public \ref webview_bind (asynchronous)
  noresult bind(const std::string &name, binding_t fn, void *arg);
  /// Internal API implementation of public \ref webview_unbind
  noresult unbind(const std::string &name);
  /// Internal API implementation of public \ref webview_return
  noresult resolve(const std::string &id, int status,
                   const std::string &result);
  /// Internal API implementation of public \ref webview_get_window
  result<void *> window() { return window_impl(); }
  /// Internal API implementation part of public \ref webview_get_native_handle
  result<void *> widget() { return widget_impl(); }
  /// Internal API implementation part of public \ref webview_get_native_handle
  result<void *> browser_controller() { return browser_controller_impl(); }
  /// Internal API implementation of public \ref webview_run
  noresult run();
  /// Internal API implementation of public \ref webview_terminate
  noresult terminate();
  /// Internal API implementation of public \ref webview_dispatch
  noresult dispatch(std::function<void()> f);
  /// Internal API implementation of public \ref webview_set_title
  noresult set_title(const std::string &title);
  /// Internal API implementation of public \ref webview_set_size
  noresult set_size(int width, int height, webview_hint_t hints);
  /// Internal API implementation of public \ref webview_set_html
  noresult set_html(const std::string &html);
  /// Internal API implementation of public \ref webview_init
  noresult init(const std::string &js);
  /// Internal API implementation of public \ref webview_eval
  noresult eval(const std::string &js);

protected:
  /// Backend specific implementation for \ref navigate
  virtual noresult navigate_impl(const std::string &url) = 0;
  /// Backend specific implementation for \ref window
  virtual result<void *> window_impl() = 0;
  /// Backend specific implementation for \ref widget
  virtual result<void *> widget_impl() = 0;
  /// Backend specific implementation for \ref browser_controller
  virtual result<void *> browser_controller_impl() = 0;
  /// Backend specific implementation for \ref run
  virtual noresult run_impl() = 0;
  /// Backend specific implementation for \ref terminate
  virtual noresult terminate_impl() = 0;
  /// Backend specific implementation for \ref dispatch
  virtual noresult dispatch_impl(std::function<void()> f) = 0;
  /// Backend specific implementation for \ref set_title
  virtual noresult set_title_impl(const std::string &title) = 0;
  /// Backend specific implementation for \ref set_size
  virtual noresult set_size_impl(int width, int height,
                                 webview_hint_t hints) = 0;
  /// Backend specific implementation for \ref set_html
  virtual noresult set_html_impl(const std::string &html) = 0;
  /// Backend specific implementation for \ref eval
  virtual noresult eval_impl(const std::string &js) = 0;

  /// Adds a bound user function to Webview native code.
  virtual user_script *add_user_script(const std::string &js);
  /// Backend specific implementation of \ref add_user_script to add a bound user JS function.
  virtual user_script add_user_script_impl(const std::string &js) = 0;
  /// Backend specific implementation to remove all bound JS user functions from the Webview script.
  virtual void
  remove_all_user_scripts_impl(const std::list<user_script> &scripts) = 0;
  /// Backend specific method to compare equality of two bound user functions.
  virtual bool are_user_scripts_equal_impl(const user_script &first,
                                           const user_script &second) = 0;
  /// Replaces a bound user script in Webview native code.
  virtual user_script *replace_user_script(const user_script &old_script,
                                           const std::string &new_script_code);
  /// Updates the JS `bind` script portion in the backend window.
  void replace_bind_script();
  /// Adds the JS Webview script to the backend window
  void add_init_script(const std::string &post_fn) {
    add_user_script(engine_js::init(post_fn));
    m_is_init_script_added = true;
  }
  // Creates a `bind` JS script string for the backend window.
  std::string create_bind_script() {
    std::vector<std::string> bound_names;
    bound_names.reserve(bindings.size());
    std::transform(bindings.begin(), bindings.end(),
                   std::back_inserter(bound_names),
                   [](const std::pair<std::string, binding_ctx_t> &pair) {
                     return pair.first;
                   });
    return engine_js::bind(bound_names);
  }

  /// Handler for messages from the backend window to the native Webview process.
  virtual void on_message(const std::string &msg);
  /// Handler to increment the browser window count
  virtual void on_window_created() { inc_window_count(); }
  /// Handler to decrement the browser window count
  virtual void on_window_destroyed(bool skip_termination = false) {
    if (dec_window_count() <= 0) {
      if (!skip_termination) {
        terminate();
      }
    }
  }

  /// Various internal backend scenarios require backend event queue depletion.
  /// This spins the event loop until queued events have been processed.
  void deplete_run_loop_event_queue() {
    bool done{};
    dispatch([&] { done = true; });
    run_event_loop_while([&] { return !done; });
  }
  /// Various internal backend sccenarios require partial backend event queue depletion.
  /// This backend specific method spins the event loop until the callback function returns true.
  virtual void run_event_loop_while(std::function<bool()> fn) = 0;

  /// Adds the default window size event to the backend queue.
  /// This is guarded from execution if the user sets a preferred size.
  void dispatch_size_default() {
    if (!owns_window() || !m_is_init_script_added) {
      return;
    };
    dispatch([this]() {
      if (!m_is_size_set) {
        set_size(m_initial_width, m_initial_height, WEBVIEW_HINT_NONE);
      }
    });
  }
  /// Explicitly toggle the guard for the setting of the default window size.
  /// This may be needed to moderate behaviour during premature backend event queue depletion.
  void set_default_size_guard(bool guarded) { m_is_size_set = guarded; }
  /// Gets a flag for whether the Webview window is embedded, or is owned by the user process.
  bool owns_window() const { return m_owns_window; }

private:
  class binding_ctx_t;

  /// Keeps track of the number of backend window instances.
  static std::atomic_uint &window_ref_count() {
    static std::atomic_uint ref_count{0};
    return ref_count;
  }
  /// Increments the reference number of backend window instances.
  static unsigned int inc_window_count() { return ++window_ref_count(); }
  /// Decrements the reference number of backend window instances.
  static unsigned int dec_window_count() {
    auto &count = window_ref_count();
    if (count > 0) {
      return --count;
    }
    return 0;
  }

  /// A map of bound JS user functions.
  std::map<std::string, binding_ctx_t> bindings;
  /// A reference to the currently active JS binding script in the backend window.
  user_script *m_bind_script{};
  /// A list of references to bound user scripts.
  std::list<user_script> m_user_scripts;
  /// Flags if the JS Webview code has been added to the backend window yet.
  bool m_is_init_script_added{};
  /// Flags if the initial window size has been set.
  /// This acts as a guard against the default size overriding a user set size
  bool m_is_size_set{};
  /// Flags whether the Webview window is embedded or owned by the user process.
  bool m_owns_window{};

  /// The default backend window width
  static const int m_initial_width = 640;
  /// The default backend window height
  static const int m_initial_height = 480;

  /// A type for handling the native callback side of a JS promise resolution.
  class binding_ctx_t {
  public:
    binding_ctx_t(binding_t callback, void *arg)
        : m_callback(callback), m_arg(arg) {}
    // Executes the bound JS function
    void call(std::string id, std::string args) const {
      if (m_callback) {
        m_callback(id, args, m_arg);
      }
    }

  private:
    // This function is called upon execution of the bound JS function
    binding_t m_callback;
    // This user-supplied argument is passed to the callback
    void *m_arg;
  };
};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_BASE_HH
