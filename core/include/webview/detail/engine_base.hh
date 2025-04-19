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
#ifndef WEBVIEW_UNBIND_TIMEOUT
#define WEBVIEW_UNBIND_TIMEOUT 20
#endif

#include "../errors.hh"
#include "../types.h"
#include "../types.hh"
#include "json.hh"
#include "user_script.hh"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <exception>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace webview {
namespace detail {

class engine_base {
public:
  engine_base(bool owns_window) : m_owns_window{owns_window} {}

  virtual ~engine_base() = default;

  noresult navigate(const std::string &url) {
    if (url.empty()) {
      return navigate_impl("about:blank");
    }
    return navigate_impl(url);
  }

  using sync_binding_t = std::function<std::string(std::string)>;
  using binding_t = std::function<void(std::string, std::string, void *)>;

  // Synchronous bind
  noresult bind(const std::string &name, sync_binding_t fn) {
    auto wrapper = [this, fn](const std::string &id, const std::string &req,
                              void * /*arg*/) { resolve(id, 0, fn(req)); };
    skip_queue = true;
    auto res = bind(name, wrapper, nullptr);
    skip_queue = false;
    return res;
  }

  // Asynchronous bind
  noresult bind(const std::string &name, binding_t fn, void *arg) {

    auto is_unbinding = list_contains(unbind_queue, name);

    // NOLINTNEXTLINE(readability-container-contains): contains() requires C++20
    if (!is_unbinding && bindings.count(name) > 0) {
      return error_info{WEBVIEW_ERROR_DUPLICATE};
    }

    auto send_to_queue = dom_ready.load() && (is_unbinding || bind_busy.load());
    /*
    printf("Webview: bind: %s | %s | %s\n",
           send_to_queue ? "queue" : "no queue",
           dispatch_bind.load() ? "dispatch" : "no dispatch", name.c_str());
*/
    if (!skip_queue && send_to_queue) {
      list_add(bind_queue, name);
      if (bind_busy.load()) {
        return {};
      }
      bind_busy.store(true);
      bind_cv.notify_one();
      bind_queue.clear();
      return {};
    }

    auto do_work = [this, name, fn, arg] {
      promise_ids[name] = {};
      bindings.emplace(name, binding_ctx_t(fn, arg));
      replace_bind_script();
      if (!dom_ready.load()) {
        return;
      }
      // Notify that a binding was created if the init script has already
      // set things up.
      eval("if (window.__webview__) {\n\
  window.__webview__.onBind(" +
           json_escape(name) + ")\n\
}");
    };

    if (dispatch_bind.load()) {
      dispatch(do_work);
    } else {
      do_work();
    }

    return {};
  }

  noresult unbind(const std::string &name) {
    auto send_to_queue =
        !dispatch_unbind.load() &&
        (unbind_busy.load() || list_contains(has_unres_promises, name));

    printf("Webview: unbind: %s: %s\n", send_to_queue ? "queued" : "not queued",
           name.c_str());
    for (auto &name : has_unres_promises) {
      printf("Webview: unbind: unresolved: %s\n", name.c_str());
    }

    if (send_to_queue) {
      list_add(unbind_queue, name);
      if (unbind_busy.load()) {
        return {};
      };
      unbind_busy.store(true);
      unbind_cv.notify_one();
      unbind_queue.clear();

      return {};
    }

    auto found = bindings.find(name);
    if (found == bindings.end()) {
      return error_info{WEBVIEW_ERROR_NOT_FOUND};
    }
    found = bindings.end();

    const auto do_work = [this, name]() {
      printf("Webview: unbind: doing work: %s\n", name.c_str());

      list_erase(has_unres_promises, name);
      bindings.erase(name);
      replace_bind_script();
      if (!dom_ready.load()) {
        return;
      }
      // Notify that a binding was created if the init script has already
      // set things up.
      eval("if (window.__webview__) {\n\
window.__webview__.onUnbind(" +
           json_escape(name) + ")\n\
}");
    };

    if (dispatch_unbind.load()) {
      return dispatch(do_work);
    }
    do_work();
    return {};
  }

  noresult resolve(const std::string &id, int status,
                   const std::string &result) {
    // NOLINTNEXTLINE(modernize-avoid-bind): Lambda with move requires C++14
    return dispatch(std::bind(
        [id, status, this](std::string escaped_result) {
          std::string js = "window.__webview__.onReply(" + json_escape(id) +
                           ", " + std::to_string(status) + ", " +
                           escaped_result + ")";
          eval(js);
        },
        result.empty() ? "undefined" : json_escape(result)));
  }

  result<void *> window() { return window_impl(); }
  result<void *> widget() { return widget_impl(); }
  result<void *> browser_controller() { return browser_controller_impl(); }
  noresult run() { return run_impl(); }
  noresult terminate() {
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);

    is_terminating.store(true);

    for (auto &resolved : is_promise_resolved) {
      auto &resolved_flag = resolved.second;
      if (!resolved_flag.load()) {
        resolved_flag.store(true);
      };
    }

    bind_busy.store(true);
    unbind_busy.store(true);
    eval_busy.store(true);
    resolver_busy.store(true);

    bind_cv.notify_one();
    unbind_cv.notify_one();
    eval_cv.notify_one();
    resolver_cv.notify_all();

    bind_cv.wait(lock, thread_acquire(&bind_thread));
    unbind_cv.wait(lock, thread_acquire(&unbind_thread));
    eval_cv.wait(lock, thread_acquire(&eval_thread));

    bind_thread.join();
    unbind_thread.join();
    eval_thread.join();

    return terminate_impl();
  }
  noresult dispatch(std::function<void()> f) { return dispatch_impl(f); }
  noresult set_title(const std::string &title) { return set_title_impl(title); }

  noresult set_size(int width, int height, webview_hint_t hints) {
    auto res = set_size_impl(width, height, hints);
    m_is_size_set = true;
    return res;
  }

  noresult set_html(const std::string &html) { return set_html_impl(html); }

  noresult init(const std::string &js) {
    add_user_script(js);
    return {};
  }

  noresult eval(const std::string &js) {

    register_eval_promises(js);

    printf("\nWebview: eval:\n%s\n\n", js.c_str());

    auto send_to_queue =
        !dispatch_eval.load() && (!dom_ready.load() || eval_busy.load());

    if (!skip_queue && send_to_queue) {
      list_add(eval_queue, js);
      if (eval_busy.load()) {
        return {};
      };
      eval_busy.store(true);
      eval_cv.notify_one();
      eval_queue.clear();
      return {};
    }
    /*
    printf("Webview: eval: %s\n%s\n",
           dispatch_eval.load() ? "dispatch" : "no dispatch", js.c_str());
*/
    return dispatch_eval.load() ? dispatch([this, js] { eval_impl(js); })
                                : eval_impl(js);
  }

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

  virtual user_script *add_user_script(const std::string &js) {
    return std::addressof(*m_user_scripts.emplace(m_user_scripts.end(),
                                                  add_user_script_impl(js)));
  }

  virtual user_script add_user_script_impl(const std::string &js) = 0;

  virtual void
  remove_all_user_scripts_impl(const std::list<user_script> &scripts) = 0;

  virtual bool are_user_scripts_equal_impl(const user_script &first,
                                           const user_script &second) = 0;

  virtual user_script *replace_user_script(const user_script &old_script,
                                           const std::string &new_script_code) {
    remove_all_user_scripts_impl(m_user_scripts);
    user_script *old_script_ptr{};
    for (auto &script : m_user_scripts) {
      auto is_old_script = are_user_scripts_equal_impl(script, old_script);
      script = add_user_script_impl(is_old_script ? new_script_code
                                                  : script.get_code());
      if (is_old_script) {
        old_script_ptr = std::addressof(script);
      }
    }
    return old_script_ptr;
  }

  void replace_bind_script() {
    if (m_bind_script) {
      m_bind_script = replace_user_script(*m_bind_script, create_bind_script());
    } else {
      m_bind_script = add_user_script(create_bind_script());
    }
  }

  void add_init_script(const std::string &post_fn) {
    add_user_script(create_init_script(post_fn));
    m_is_init_script_added = true;
  }

  std::string create_init_script(const std::string &post_fn) {
    auto js = std::string{} + "(function() {\n\
  'use strict';\n\
  function generateId() {\n\
    var crypto = window.crypto || window.msCrypto;\n\
    var bytes = new Uint8Array(16);\n\
    crypto.getRandomValues(bytes);\n\
    return Array.prototype.slice.call(bytes).map(function(n) {\n\
      var s = n.toString(16);\n\
      return ((s.length % 2) == 1 ? '0' : '') + s;\n\
    }).join('');\n\
  }\n\
  var Webview = (function() {\n\
    var _promises = {};\n\
    function Webview_() {}\n\
    Webview_.prototype.post = function(message) {\n\
      return (" +
              post_fn + ")(message);\n\
    };\n\
    Webview_.prototype.call = function(method) {\n\
      var _id = generateId();\n\
      var _params = Array.prototype.slice.call(arguments, 1);\n\
      var promise = new Promise(function(resolve, reject) {\n\
        _promises[_id] = { resolve, reject };\n\
      });\n\
      this.post(JSON.stringify({\n\
        id: _id,\n\
        method: method,\n\
        params: _params\n\
      }));\n\
      return promise;\n\
    };\n\
    Webview_.prototype.onReply = function(id, status, result) {\n\
      var promise = _promises[id];\n\
      if (result !== undefined) {\n\
        try {\n\
          result = JSON.parse(result);\n\
        } catch (e) {\n\
          promise.reject(new Error(\"Failed to parse binding result as JSON\"));\n\
          return;\n\
        }\n\
      }\n\
      if (status === 0) {\n\
        promise.resolve(result);\n\
      } else {\n\
        promise.reject(result);\n\
      }\n\
    };\n\
    Webview_.prototype.onBind = function(name) {\n\
      if (window.hasOwnProperty(name)) {\n\
        throw new Error('Property \"' + name + '\" already exists');\n\
      }\n\
      window[name] = (function() {\n\
        var params = [name].concat(Array.prototype.slice.call(arguments));\n\
        return Webview_.prototype.call.apply(this, params);\n\
      }).bind(this);\n\
    };\n\
    Webview_.prototype.onUnbind = function(name) {\n\
      if (!window.hasOwnProperty(name)) {\n\
        throw new Error('Property \"' + name + '\" does not exist');\n\
      }\n\
      delete window[name];\n\
    };\n\
    return Webview_;\n\
  })();\n\
  window.__webview__ = new Webview();\n\
  __webview__.post(\n\
    JSON.stringify({\n\
      id: \"DOM_ready\",\n\
      method: \"NOOP\",\n\
      params: [],\n\
    })\n\
  );\n\
})()";
    return js;
  }

  std::string create_bind_script() {
    std::string js_names = "[";
    bool first = true;
    for (const auto &binding : bindings) {
      if (first) {
        first = false;
      } else {
        js_names += ",";
      }
      js_names += json_escape(binding.first);
    }
    js_names += "]";

    auto js = std::string{} + "(function() {\n\
  'use strict';\n\
  var methods = " +
              js_names + ";\n\
  methods.forEach(function(name) {\n\
    window.__webview__.onBind(name);\n\
  });\n\
})()";
    return js;
  }

  virtual void on_message(const std::string &msg) {
    auto name = json_parse(msg, "method", 0);
    auto id = json_parse(msg, "id", 0);

    if (id == "DOM_ready") {
      printf("Webview: on_message: DOM is ready\n");
      dom_ready.store(true);
      dom_cv.notify_all();
      return;
    }

    auto found = bindings.find(name);
    if (found == bindings.end()) {
      auto message =
          "Promise %s was rejected because binding `" + name + "` got unbound.";
      resolve(id, 1, json_escape(message));
      return;
    }
    found = bindings.end();

    list_add_unique(has_unres_promises, name);
    list_add(promise_ids[name], id);
    unbind_cv.notify_one();

    std::thread resolver = std::thread(&engine_base::resolve_thread_constructor,
                                       this, name, msg, id);
    resolver.detach();
  }

  virtual void on_window_created() { inc_window_count(); }

  virtual void on_window_destroyed(bool skip_termination = false) {
    if (dec_window_count() <= 0) {
      if (!skip_termination) {
        terminate();
      }
    }
  }

  // Runs the event loop until the currently queued events have been processed.
  void deplete_run_loop_event_queue() {
    bool done{};
    dispatch([&] { done = true; });
    run_event_loop_while([&] { return !done; });
  }

  // Runs the event loop while the passed-in function returns true.
  virtual void run_event_loop_while(std::function<bool()> fn) = 0;

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

  void set_default_size_guard(bool guarded) { m_is_size_set = guarded; }

  bool owns_window() const { return m_owns_window; }

private:
  static std::atomic_uint &window_ref_count() {
    static std::atomic_uint ref_count{0};
    return ref_count;
  }

  static unsigned int inc_window_count() { return ++window_ref_count(); }

  static unsigned int dec_window_count() {
    auto &count = window_ref_count();
    if (count > 0) {
      return --count;
    }
    return 0;
  }

  typedef std::vector<std::string> list_t;

  class binding_ctx_t {
  public:
    binding_ctx_t(binding_t callback, void *arg)
        : m_callback(callback), m_arg(arg) {}
    void call(std::string id, std::string args) const {
      if (m_callback) {
        m_callback(id, args, m_arg);
      }
    }
    // This function is called upon execution of the bound JS function
    binding_t m_callback;
    // This user-supplied argument is passed to the callback
    void *m_arg;
  };

  std::function<bool()> atomic_acquire(std::atomic_bool *flag,
                                       bool expected = true) {
    return [flag, expected]() {
      return flag->load(std::memory_order_acquire) == expected;
    };
  }
  std::function<bool()> thread_acquire(std::thread *thread) {
    return [thread]() { return thread->joinable(); };
  }

  /// Parses the JS eval string for calls to bound functions.
  /// Matches are added to a list for `webview_unbind` to process appropriately.
  void register_eval_promises(const std::string &js) {
    for (auto &promises : promise_ids) {
      auto const &name = promises.first;

      if (list_contains(has_unres_promises, name)) {
        return;
      }
      auto const &needle = "" + name + "(";
      auto found = js.find(needle) != std::string::npos;
      if (found) {
        list_add_unique(has_unres_promises, name);
      }
    }
  }

  bool list_contains(list_t &list, const std::string &value) {
    auto pos = std::find(list.begin(), list.end(), value);
    auto found = pos != list.end();
    if (found) {
      pos = list.end();
    };
    return found;
  }

  void list_erase(list_t &list, const std::string &value) {
    list.erase(std::remove(list.begin(), list.end(), value), list.end());
  }

  void list_add(list_t &list, const std::string &value) {
    list.emplace_back(value);
  }

  void list_add_unique(list_t &list, const std::string &value) {
    if (!list_contains(list, value)) {
      list.emplace_back(value);
    }
  }

  /// Creates a thread that allows eval operations to wait for bind operations
  /// and DOM readiness to complete without stalling the main/app thread.
  void eval_thread_constructor() {
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);

    while (true) {
      eval_cv.wait(lock, atomic_acquire(&eval_busy));
      if (is_terminating.load()) {
        break;
      }
      printf("Webview: eval thread: BUSY\n");
      dom_cv.wait(lock, atomic_acquire(&dom_ready));
      printf("Webview: eval thread: DOM_ready\n");
      //eval_cv.wait(lock, atomic_acquire(&bind_busy, false));

      list_t active_queue = eval_queue;

      dispatch_eval.store(true);
      for (auto &js : active_queue) {
        eval(js);
      }
      if (eval_queue.empty()) {
        printf("Webview: eval thread: EMPTY\n");
        //eval_queue.shrink_to_fit();
        eval_busy.store(false);
        dispatch_eval.store(false);
      }
    }
  }

  /// Creates a thread that allows bind operations to wait for corresponding
  /// unbind operations to complete without stalling the main/app thread.
  void bind_thread_constructor() {
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);

    while (true) {
      bind_cv.wait(lock, atomic_acquire(&bind_busy));
      if (is_terminating.load()) {
        break;
      }
      //bind_cv.wait(lock, atomic_acquire(&unbind_busy, false));
      list_t active_queue = bind_queue;

      dispatch_bind.store(true);
      for (auto &name : active_queue) {
        auto ctx = bindings.find(name)->second;
        auto fn = ctx.m_callback;
        auto arg = ctx.m_arg;

        bind(name, fn, arg);
      }
      if (bind_queue.empty()) {
        //bind_queue.shrink_to_fit();
        dispatch_bind.store(false);
        bind_busy.store(false);
      }
    }
  }

  /// Creates a thread that allows native unresolved promise work units to
  /// resolve/reject before unbinding without stalling the main/app thread.
  void unbind_thread_constructor() {
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);

    const auto timeout = std::chrono::milliseconds(WEBVIEW_UNBIND_TIMEOUT);
    const auto get_rej_message = [&](std::string name) {
      std::string message =
          "The native callback function `" + name + "` timed out after " +
          std::to_string(WEBVIEW_UNBIND_TIMEOUT) +
          "ms while unbinding.\nYou can adjust the default timeout when "
          "compiling your application:\nWEBVIEW_UNBIND_TIMEOUT=`int value`";
      return message;
    };

    while (true) {
      unbind_cv.wait(lock, atomic_acquire(&unbind_busy));
      if (is_terminating.load()) {
        break;
      }
      dom_cv.wait(lock, atomic_acquire(&dom_ready));

      list_t active_queue = unbind_queue;

      dispatch_unbind.store(true);
      for (auto &name : active_queue) {
        if (!list_contains(has_unres_promises, name)) {
          unbind(name);
          return;
        }

        if (promise_ids[name].empty()) {
          unbind_cv.wait_for(lock, timeout,
                             [&] { return !promise_ids[name].empty(); });
        }
        auto latest_promise_id = promise_ids[name].back();
        resolver_cv.wait_for(
            lock, timeout,
            atomic_acquire(&is_promise_resolved[latest_promise_id]));

        for (auto &id : promise_ids[name]) {
          auto found = is_promise_resolved.find(id);
          auto is_resolved = found->second.load();
          found = is_promise_resolved.end();

          if (!is_resolved) {
            resolve(id, 1, json_escape(get_rej_message(name)));
          }
        }

        unbind(name);
      }

      if (unbind_queue.empty()) {
        //unbind_queue.shrink_to_fit();
        dispatch_unbind.store(false);
        unbind_busy.store(false);
        //bind_cv.notify_one();
      }
    }
  }

  /// Creates a child thread for each native work unit of a bound JS promise.
  /// We want native promise work units to run in parralel.
  /// We do not want native promise work units to stall the main/app thread.
  void resolve_thread_constructor(const std::string &name,
                                  const std::string &msg,
                                  const std::string &id) {
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);

    auto args = json_parse(msg, "params", 0);
    is_promise_resolved[id].store(false);

    try {
      auto found = bindings.find(name);
      const auto &context = found->second;
      context.call(id, args);
      found = bindings.end();

    } catch (const std::exception &err) {
      auto e_message =
          "Uncaught exception from native user callback function `" + name +
          "`:\n" + std::string(err.what());

      dispatch(
          [this, id, e_message] { resolve(id, 1, json_escape(e_message)); });

    } catch (...) {
      auto e_message =
          "\nNative user callback function `%s` failed "
          "because Webview terminated before it could complete.\n\n";

      perror(e_message);
    };

    is_promise_resolved[id].store(true);
    resolver_cv.notify_all();

    list_erase(promise_ids[name], id);
    is_promise_resolved.erase(id);
  }

  std::map<std::string, binding_ctx_t> bindings;
  user_script *m_bind_script{};
  std::list<user_script> m_user_scripts;

  bool m_is_init_script_added{};
  bool m_is_size_set{};
  bool m_owns_window{};
  static const int m_initial_width = 640;
  static const int m_initial_height = 480;

  std::atomic_bool dom_ready{};
  std::atomic_bool is_terminating{};
  /// Flag that spins the eval thread event loop
  std::atomic_bool eval_busy{};
  /// Flag that spins the unbind thread event loop
  std::atomic_bool unbind_busy{};
  /// Flag that spins the bind thread event loop
  std::atomic_bool bind_busy{};
  /// Flag that spins the user native callback thread event loop
  std::atomic_bool resolver_busy{};
  /// Flag to advise unbind to dispatch to main thread
  std::atomic_bool dispatch_unbind{};
  /// Flag to advise bind to dispatch to main thread
  std::atomic_bool dispatch_bind{};
  /// Flag to advise eval to dispatch to main thread
  std::atomic_bool dispatch_eval{};

  bool skip_queue{};

  std::condition_variable resolver_cv;
  std::condition_variable unbind_cv;
  std::condition_variable bind_cv;
  std::condition_variable eval_cv;
  std::condition_variable dom_cv;

  /// Flag that releases a wait when a promise resolves
  std::map<std::string, std::atomic_bool> is_promise_resolved;
  /// Keeps a list of unresolved promise id's
  std::map<std::string, list_t> promise_ids;

  /// An order list of pending bind operations
  list_t bind_queue;
  /// An ordered list of pending unbind operations
  list_t unbind_queue;
  /// An ordered list of pending eval operations
  list_t eval_queue;
  /// A list of binding names that have unresolved promises
  list_t has_unres_promises;

  std::thread unbind_thread =
      std::thread(&engine_base::unbind_thread_constructor, this);

  std::thread eval_thread =
      std::thread(&engine_base::eval_thread_constructor, this);

  std::thread bind_thread =
      std::thread(&engine_base::bind_thread_constructor, this);
};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_BASE_HH
