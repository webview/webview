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

using binding_t = std::function<void(std::string, std::string, void *)>;
using sync_binding_t = std::function<std::string(std::string)>;

namespace webview {
namespace detail {

class binding_ctx_t {
public:
  /**
   * user-defined type for the internal `webview_bind` API
   * callback function and arg handling
   */
  binding_ctx_t(binding_t callback, void *arg);
  /**
   * Executes the bound JS function
   */
  void call(std::string id, std::string args) const;

private:
  /** 
  * This function is called upon execution of the bound JS function 
  */
  binding_t m_callback;
  /**
   * This user-supplied argument is passed to the callback
   */
  void *m_arg;
};
/**
 * Common internal API methods for all three backend Webview constructors:
 * - cocoa_webkit
 * - gtk_webkitgtk
 * - win32_edge
 */
class engine_base {
public:
  virtual ~engine_base();

  /** Internal API implementation of `webview_navigate` */
  noresult navigate(const std::string &url);

  /** Internal API implementation of synchronous`webview_bind` */
  noresult bind(const std::string &name, sync_binding_t fn);

  /** Internal API implementation of asynchronous`webview_bind` */
  noresult bind(const std::string &name, binding_t fn, void *arg);

  /** Internal API implementation of `webview_unbind` */
  noresult unbind(const std::string &name);

  noresult resolve(const std::string &id, int status,
                   const std::string &result);
  result<void *> window();
  result<void *> widget();
  result<void *> browser_controller();
  noresult run();
  noresult terminate();
  noresult dispatch(std::function<void()> f);
  noresult set_title(const std::string &title);

  noresult set_size(int width, int height, webview_hint_t hints);

  noresult set_html(const std::string &html);

  noresult init(const std::string &js);

  noresult eval(const std::string &js);

protected:
  virtual noresult navigate_impl(const std::string &url) = 0;
  virtual result<void *> window_impl() = 0;
  virtual result<void *> widget_impl() = 0;
  virtual result<void *> browser_controller_impl() = 0;
  virtual noresult run_impl() = 0;
  virtual noresult terminate_impl() = 0;
  virtual noresult dispatch_impl(std::function<void()> f) = 0;
  virtual noresult set_title_impl(const std::string &title) = 0;
  virtual noresult set_size_impl(int width, int height,
                                 webview_hint_t hints) = 0;
  virtual noresult set_html_impl(const std::string &html) = 0;
  virtual noresult eval_impl(const std::string &js) = 0;

  virtual user_script *add_user_script(const std::string &js);

  virtual user_script add_user_script_impl(const std::string &js) = 0;

  virtual void
  remove_all_user_scripts_impl(const std::list<user_script> &scripts) = 0;

  virtual bool are_user_scripts_equal_impl(const user_script &first,
                                           const user_script &second) = 0;

  virtual user_script *replace_user_script(const user_script &old_script,
                                           const std::string &new_script_code);

  void replace_bind_script();

  void add_init_script(const std::string &post_fn);

  std::string create_init_script(const std::string &post_fn);

  std::string create_bind_script();

  virtual void on_message(const std::string &msg);

  virtual void on_window_created();

  virtual void on_window_destroyed(bool skip_termination = false);

  /**
   * Various internal backend scenarious require instant event queue depletion.
   * This runs the event loop until the currently queued events have been processed.
   */
  void deplete_run_loop_event_queue();

  /**
   * Various internal backend sccenarios require temporary running of the event loop.
   * This runs the event loop while the passed-in function returns true.
   */
  virtual void run_event_loop_while(std::function<bool()> fn) = 0;

  void dispatch_size_default();
  void set_default_size_guard(bool guarded);
  void set_owns_window(bool owns_window);
  bool owns_window() const;

private:
  static std::atomic_uint &window_ref_count();

  static unsigned int inc_window_count();

  static unsigned int dec_window_count();

  /** A map of user defined bound functions */
  std::map<std::string, binding_ctx_t> bindings;
  user_script *m_bind_script{};
  std::list<user_script> m_user_scripts;

  /** Flags if the initial system script has been bound to the window */
  bool m_is_init_script_added{};
  /**
   * Flags if the initial window size has been set.
   * This acts as a guard against the default size overriding a user set size
   */
  bool m_is_size_set{};
  bool m_owns_window{};
  /** The default window width */
  static const int m_initial_width = 640;
  /** The default window height */
  static const int m_initial_height = 480;
};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_BASE_HH
