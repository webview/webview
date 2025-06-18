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

#ifndef WEBVIEW_DETAIL_ENGINE_BASE_CC
#define WEBVIEW_DETAIL_ENGINE_BASE_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "detail/engine_base.hh"
#include "detail/threading/thread_detector.hh"
#include "log/console_log.hh"
#include "log/trace_log.hh"
#include "strings/string_api.hh"
#include "tests/test_helper.hh"

using namespace webview::strings;
using namespace webview::log;
using namespace webview::tests;
using namespace webview::detail;
using namespace webview::detail::threading;

/* PUBLIC 
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* Constructor
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

engine_base::engine_base(bool owns_window)
    : engine_queue{this}, m_owns_window{owns_window} {
  if (!thread::is_main_thread()) {
    throw exception{WEBVIEW_ERROR_BAD_API_CALL,
                    "Webview must be created from the main thread."};
  };
}

/* API Methods
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

noresult engine_base::navigate(const std::string &url) {
  auto do_work = [this, url] {
    if (url.empty()) {
      navigate_impl("about:blank");
    } else {
      navigate_impl(url);
    }
  };
  if (thread::is_main_thread()) {
    do_work();
    return {};
  } else {
    return dispatch_impl(do_work);
  }
}

WEBVIEW_DEPRECATED(DEPRECATE_WEBVIEW_SYNCHRONOUS_BIND)
noresult engine_base::bind(const std::string & /*name*/,
                           sync_binding_t /*fn*/) {
  console.error("Synchronous bind is deprecated, and WILL cause undefined "
                "behaviour. This function is now a no-op.\nPlease use the "
                "standard bind function signature.");
  return noresult{WEBVIEW_ERROR_BAD_API_CALL};
}

noresult engine_base::bind(const std::string &name, binding_t fn, void *arg) {
  trace::base.bind.start(name);
  if (queue.bind.is_duplicate(name)) {
    auto msg = "\"" + name + "\" was not bound because it was already bound.";
    if (thread::is_main_thread()) {
      console.warn(msg + "\nRe-binding is not supported from the main thread.");
    } else {
      console.warn(msg);
    }
    return error_info{WEBVIEW_ERROR_DUPLICATE};
  }

  auto do_work = [this, name, fn, arg] {
    log::trace::base.bind.work(name);
    list.bindings.emplace(name, fn, arg);
    replace_bind_script();
    eval(string::js.onbind(name), true);
  };
  // The user may want to bind before running so that they can use
  // bindings in `webview_init` or `webview_set_html`.
  // In this scenario, we execute `bind` directly.
  if (thread::is_main_thread() && !atomic.dom.ready()) {
    do_work();
    return {};
  }
  return queue.bind.enqueue(do_work, name);
}

noresult engine_base::unbind(const std::string &name) {
  trace::base.unbind.start(name);
  if (queue.unbind.not_found(name)) {
    return error_info{WEBVIEW_ERROR_NOT_FOUND};
  }

  auto do_work = [this, name]() {
    log::trace::base.unbind.work(name);
    eval(string::js.onunbind(name), true);
    list.bindings.erase(name);
    replace_bind_script();
  };
  return queue.unbind.enqueue(do_work, name);
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value &&
                            !std::is_same<T, bool>::value,
                        noresult>::type
engine_base::resolve(const std::string &id, int status, T result) {
  return resolve(id, status, std::to_string(result));
};
template <typename T>
typename std::enable_if<std::is_same<T, bool>::value, noresult>::type
engine_base::resolve(const std::string &id, int status, T result) {
  std::string bool_string = result ? "true" : "false";
  return resolve(id, status, bool_string);
};
noresult engine_base::resolve(const std::string &id, int status,
                              const std::string &result) {
  // Get the promise binding name and
  // notify the queue that the promise is resolving.
  std::string name = list.id_name_map.get(id);
  queue.promises.resolving(name, id);
  list.id_name_map.erase(id);

  auto res_m = result.empty() ? "undefined" : result;
  auto action = status == 0 ? "resolving" : "rejecting";
  auto message = "Bound function \"" + name + "\" is " + action + " promise " +
                 id + " with result: " + res_m;
  status == 0 ? log::console.info(message) : log::console.warn(message);

  auto res_escaped = result.empty() ? "undefined" : json::escape(result);
  auto js = string::js.onreply(id, status, res_escaped);
  return eval(js, true);
}

noresult engine_base::reject(const std::string &id, const std::string &err) {
  return resolve(id, 1, err);
}

noresult engine_base::run() {
  if (!thread::is_main_thread()) {
    console.error("Webview must be run from the main thread.",
                  WEBVIEW_ERROR_BAD_API_CALL);
    return noresult{WEBVIEW_ERROR_BAD_API_CALL};
  };
  return run_impl();
}

noresult engine_base::terminate() {
  // The legacy `webview_destroy` is deprecated, and now re-directs here to `terminate`.
  // If the browser window was manually closed however, we do not want legacy user
  // `webview_destroy` calls to cause undefined behaviour, so we guard against it.
  if (destructor_called()) {
    return {};
  };
  return dispatch_impl([this] {
    queue.terminate();
    terminate_impl();
  });
}

WEBVIEW_DEPRECATED(DEPRECATE_WEBVIEW_DISPATCH)
noresult engine_base::dispatch(std::function<void()> f) {
  console.warn("Use of deprecated API call `webview_dispatch`.");
  if (!thread::is_main_thread()) {
    return dispatch_impl(f);
  } else {
    f();
    return {};
  };
}

noresult engine_base::set_title(const std::string &title) {
  auto do_work = [this, title] { set_title_impl(title); };
  if (thread::is_main_thread()) {
    do_work();
    return {};
  } else {
    return dispatch_impl(do_work);
  };
}

noresult engine_base::set_size(int width, int height, webview_hint_t hints) {
  auto do_work = [this, width, height, hints] {
    set_size_impl(width, height, hints);
    m_is_size_set = true;
  };
  if (thread::is_main_thread()) {
    do_work();
    return {};
  } else {
    return dispatch_impl(do_work);
  };
}

noresult engine_base::set_html(const std::string &html) {
  auto do_work = [this, html] { set_html_impl(html); };
  if (thread::is_main_thread()) {
    do_work();
    return {};
  } else {
    return dispatch_impl(do_work);
  };
}

noresult engine_base::init(const std::string &js) {
  if (!thread::is_main_thread()) {
    console.error("Webview init must be called from the main thread.",
                  WEBVIEW_ERROR_BAD_API_CALL);
    return noresult{WEBVIEW_ERROR_BAD_API_CALL};
  };
  add_user_script_impl(js);
  return {};
}

noresult engine_base::eval(const std::string &js, bool skip_queue) {
  log::trace::base.eval.start(js, skip_queue);
  auto do_work = [this, js, skip_queue] {
    if (!skip_queue) {
      auto wrapped_js = string::js.eval_wrapper(js);
      trace::base.eval.work(wrapped_js, skip_queue);
      if (thread::is_main_thread()) {
        eval_impl(wrapped_js);
      } else {
        dispatch_impl([this, wrapped_js] { eval_impl(wrapped_js); });
      }
    } else {
      log::trace::base.eval.work(js, skip_queue);
      if (thread::is_main_thread()) {
        eval_impl(js);
      } else {
        dispatch_impl([this, js] { eval_impl(js); });
      }
    }
  };
  if (!skip_queue) {
    return queue.eval.enqueue(do_work, js);
  }
  do_work();
  return {};
}
/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * PUBLIC
 * ----------------------------------------------------------------------------------------------------------- 
 * PROTECTED 
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

void engine_base::replace_bind_script() {
  auto replacement_js = create_bind_script();
  if (m_bind_script) {
    m_bind_script =
        list.m_user_scripts.replace(*m_bind_script, replacement_js, this);
  } else {
    m_bind_script = list.m_user_scripts.add(replacement_js, this);
  }
}

void engine_base::add_init_script() {
  auto init_js = string::js.init(string::js.post_fnc());
  list.m_user_scripts.add(init_js, this);
  m_is_init_script_sent = true;
}

std::string engine_base::create_bind_script() {
  std::vector<std::string> bound_names;
  list.bindings.get_names(bound_names);
  return string::js.bind(bound_names);
}

void engine_base::on_message(const std::string &msg) {
  auto id = json::parse(msg, "id", 0);
  auto name = json::parse(msg, "method", 0);

  // Guard for functional tests JS value checking
  if (id == string::flags.testop) {
    tester::set_value(name);
    return;
  }

  if (queue.promises.exec_system_message(id, name)) {
    return;
  }
  if (!list.bindings.has_name(name)) {
    auto message = string::err.reject_unbound(id, name);
    reject(id, message);
    return;
  }
  auto args = json::parse(msg, "params", 0);
  queue.promises.resolve(name, id, args);
}

void engine_base::on_window_created() { inc_window_count(); }

void engine_base::on_window_destroyed(bool skip_termination) {
  if (dec_window_count() <= 0) {
    if (!skip_termination) {
      terminate();
    }
  }
}

void engine_base::deplete_run_loop_event_queue() {
  bool done{};
  dispatch_impl([&] { done = true; });
  run_event_loop_while([&] { return !done; });
}

void engine_base::dispatch_size_default() {
  if (!owns_window() || !m_is_init_script_added) {
    return;
  };
  dispatch_impl([this]() {
    if (!m_is_size_set) {
      set_size(m_initial_width, m_initial_height, WEBVIEW_HINT_NONE);
    }
  });
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * PROTECTED
 * ----------------------------------------------------------------------------------------------------------- 
 * PRIVATE
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

std::atomic_uint &engine_base::window_ref_count() {
  static std::atomic_uint ref_count{0};
  return ref_count;
}

unsigned int engine_base::inc_window_count() { return ++window_ref_count(); }

unsigned int engine_base::dec_window_count() {
  auto &count = window_ref_count();
  if (count > 0) {
    return --count;
  }
  return 0;
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * PRIVATE */

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_BASE_CC
