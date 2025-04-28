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

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../types.h"
#include "../types.hh"
#include "user_script.hh"
#include <atomic>
#include <list>
#include <map>

namespace webview {

// Container: Global header declarations
namespace detail {

using webview::noresult;
/// Internally used callback function type for messaging in the promise resolution
/// native / JS round trip
using sync_binding_t = std::function<std::string(std::string)>;
/// Shorthand for the commonly used `string` parameter to improve code readability.
using str_arg_t = const std::string &;
/// Shorthand type for callback lamda functions ie `bind`, `unbind` and `eval`.
using do_work_t = std::function<void()>;
/// Forward declaration of \file ./engine_queue.hh to prevent a circular dependency.
class engine_queue;

} // namespace detail

// Container: `engine_base` class
namespace detail {

/// Common internal API methods for all three Webview platform classes:
/// - cocoa_webkit
/// - gtk_webkitgtk
/// - win32_edge
class engine_base {

public:
  virtual ~engine_base() = default;
  engine_base(bool owns_window);

  /// Internal API implementation of public \ref webview_navigate
  noresult navigate(str_arg_t url);
  /// Internal API implementation of public \ref webview_bind (synchronous)
  noresult bind(str_arg_t name, sync_binding_t fn);
  /// Internal API implementation of public \ref webview_bind (asynchronous)
  noresult bind(str_arg_t name, binding_t fn, void *arg);
  /// Internal API implementation of public \ref webview_unbind
  noresult unbind(str_arg_t name);
  /// Internal API implementation of public \ref webview_return
  noresult resolve(str_arg_t id, int status, str_arg_t result);
  /// Helper to reject a promise
  noresult reject(str_arg_t id, str_arg_t err);
  /// Internal API implementation of public \ref webview_get_window
  result<void *> window();
  /// Internal API implementation part of public \ref webview_get_native_handle
  result<void *> widget();
  /// Internal API implementation part of public \ref webview_get_native_handle
  result<void *> browser_controller();
  /// Internal API implementation of public \ref webview_run
  noresult run();
  /// Internal API implementation of public \ref webview_terminate
  noresult terminate();
  /// Internal API implementation of public \ref webview_dispatch
  noresult dispatch(std::function<void()> f);
  /// Internal API implementation of public \ref webview_set_title
  noresult set_title(str_arg_t title);
  /// Internal API implementation of public \ref webview_set_size
  noresult set_size(int width, int height, webview_hint_t hints);
  /// Internal API implementation of public \ref webview_set_html
  noresult set_html(str_arg_t html);
  /// Internal API implementation of public \ref webview_init
  noresult init(str_arg_t js);
  /// Internal API implementation of public \ref webview_eval
  noresult eval(str_arg_t js);

protected:
  /// Platform specific implementation for \ref navigate
  virtual noresult navigate_impl(str_arg_t url) = 0;
  /// Platform specific implementation for \ref window
  virtual result<void *> window_impl() = 0;
  /// Platform specific implementation for \ref widget
  virtual result<void *> widget_impl() = 0;
  /// Platform specific implementation for \ref browser_controller
  virtual result<void *> browser_controller_impl() = 0;
  /// Platform specific implementation for \ref run
  virtual noresult run_impl() = 0;
  /// Platform specific implementation for \ref terminate
  virtual noresult terminate_impl() = 0;
  /// Platform specific implementation for \ref dispatch
  virtual noresult dispatch_impl(std::function<void()> f) = 0;
  /// Platform specific implementation for \ref set_title
  virtual noresult set_title_impl(str_arg_t title) = 0;
  /// Platform specific implementation for \ref set_size
  virtual noresult set_size_impl(int width, int height,
                                 webview_hint_t hints) = 0;
  /// Platform specific implementation for \ref set_html
  virtual noresult set_html_impl(str_arg_t html) = 0;
  /// Platform specific implementation for \ref eval
  virtual noresult eval_impl(str_arg_t js) = 0;

  /// Adds a bound user function to Webview native code.
  virtual user_script *add_user_script(str_arg_t js);
  /// Platform specific implementation of \ref add_user_script to add a bound user JS function.
  virtual user_script add_user_script_impl(str_arg_t js) = 0;
  /// Platform specific implementation to remove all bound JS user functions from the Webview script.
  virtual void
  remove_all_user_scripts_impl(const std::list<user_script> &scripts) = 0;
  /// Platform specific method to compare equality of two bound user functions.
  virtual bool are_user_scripts_equal_impl(const user_script &first,
                                           const user_script &second) = 0;
  /// Replaces a bound user script in Webview native code.
  virtual user_script *replace_user_script(const user_script &old_script,
                                           str_arg_t new_script_code);
  /// Updates the JS `bind` script in the frontend window.
  void replace_bind_script();
  /// Adds the JS Webview script to the frontend window
  void add_init_script(str_arg_t post_fn);
  // Creates a `bind` JS script string for the frontend window.
  std::string create_bind_script();
  /// Handler for messages from the frontend window to the native Webview process.
  virtual void on_message(str_arg_t msg);
  /// Handler to increment the browser window count
  virtual void on_window_created();
  /// Handler to decrement the browser window count
  virtual void on_window_destroyed(bool skip_termination = false);
  /// Various internal backend scenarios require platform event queue depletion.
  /// This spins the platform event loop until queued events have been processed.
  void deplete_run_loop_event_queue();
  /// Various internal backend sccenarios require partial platform event queue depletion.
  /// This spins the platform event loop until the callback function returns true.
  virtual void run_event_loop_while(std::function<bool()> fn) = 0;
  /// Adds the default window size event to the platform event queue.
  /// This is guarded from execution if the user sets a preferred size.
  void dispatch_size_default();
  /// Explicitly toggle the guard for the setting of the default window size.
  /// This may be needed to moderate behaviour during premature platform event queue depletion.
  void set_default_size_guard(bool guarded);
  /// Gets a flag for whether the Webview window is embedded, or is owned by the user process.
  bool owns_window() const;

private:
  /// Keeps track of the number of platform window instances.
  static std::atomic_uint &window_ref_count();
  /// Increments the reference number of platform window instances.
  static unsigned int inc_window_count();
  /// Decrements the reference number of platform window instances.
  static unsigned int dec_window_count();
  /// A map of bound JS user functions.
  std::map<std::string, binding_ctx_t> bindings;
  /// A reference to the currently active JS bound script in the platform window.
  user_script *m_bind_script{};
  /// A list of references to bound user scripts.
  std::list<user_script> m_user_scripts;
  /// Flags if the JS Webview code has been sent to the platform window yet.
  bool m_is_init_script_sent{};
  /// Flags if the initial platform window size has been set.
  /// This acts as a guard against the default size overriding a user set size
  bool m_is_size_set{};
  /// Flags whether the Webview window is embedded or owned by the user process.
  bool m_owns_window{};
  /// The default platform window width
  static const int m_initial_width = 640;
  /// The default platform window height
  static const int m_initial_height = 480;
};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_BASE_HH
