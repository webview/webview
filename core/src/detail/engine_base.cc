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

engine_base::engine_base(bool owns_window) : m_owns_window{owns_window} {
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

// Synchronous bind
noresult engine_base::bind(const std::string &name, sync_binding_t fn) {
  auto wrapper = [this, fn](const std::string &id, const std::string &req,
                            void * /*arg*/) { resolve(id, 0, fn(req)); };
  return bind(name, wrapper, nullptr);
}

// Asynchronous bind
noresult engine_base::bind(const std::string &name, binding_t fn, void *arg) {
  // NOLINTNEXTLINE(readability-container-contains): contains() requires C++20
  if (bindings.count(name) > 0) {
    return error_info{WEBVIEW_ERROR_DUPLICATE, "Bind (duplicate): binding \"" +
                                                   name +
                                                   "\" was already bound."};
  }

  auto do_work = [this, name, fn, arg] {
    bindings.emplace(name, binding_ctx_t(fn, arg));
    replace_bind_script();
    // Notify that a binding was created if the init script has already
    // set things up.
    eval(string::js.onbind(name));
  };

  if (thread::is_main_thread()) {
    do_work();
    return {};
  } else {
    return dispatch_impl(do_work);
  }
}

noresult engine_base::unbind(const std::string &name) {
  auto found = bindings.find(name);
  if (found == bindings.end()) {
    return error_info{WEBVIEW_ERROR_NOT_FOUND,
                      "Unbind: binding \"" + name + "\" was not found."};
  }

  auto do_work = [this, name, &found]() {
    bindings.erase(found);
    replace_bind_script();
    // Notify that a binding was created if the init script has already
    // set things up.
    eval(string::js.onunbind(name));
  };

  if (thread::is_main_thread()) {
    do_work();
    return {};
  } else {
    return dispatch_impl(do_work);
  }
}

noresult engine_base::resolve(const std::string &id, int status,
                              const std::string &result) {
  if (status == 1) {
    console.warn("Promise id \"" + id + "\" was rejected.");
  }
  // NOLINTNEXTLINE(modernize-avoid-bind): Lambda with move requires C++14
  return dispatch_impl(std::bind(
      [id, status, this](std::string escaped_result) {
        std::string js = string::js.onreply(id, status, escaped_result);
        eval(js);
      },
      result.empty() ? "undefined" : json::escape(result)));
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
  return dispatch_impl([this] { terminate_impl(); });
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
  add_user_script(js);
  return {};
}

noresult engine_base::eval(const std::string &js) {
  auto do_work = [this, js] { eval_impl(js); };
  if (thread::is_main_thread()) {
    do_work();
    return {};
  } else {
    return dispatch_impl(do_work);
  };
}
/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * PUBLIC
 * ----------------------------------------------------------------------------------------------------------- 
 * PROTECTED 
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

user_script *engine_base::add_user_script(const std::string &js) {
  return std::addressof(
      *m_user_scripts.emplace(m_user_scripts.end(), add_user_script_impl(js)));
}

user_script *
engine_base::replace_user_script(const user_script &old_script,
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

void engine_base::replace_bind_script() {
  if (m_bind_script) {
    m_bind_script =
        replace_user_script(*m_bind_script, string::js.bind(bindings));
  } else {
    m_bind_script = add_user_script(string::js.bind(bindings));
  }
}

void engine_base::add_init_script() {
  add_user_script(string::js.init(string::js.post_fnc()));
  m_is_init_script_added = true;
}

void engine_base::on_message(const std::string &msg) {
  auto id = json::parse(msg, "id", 0);
  auto name = json::parse(msg, "method", 0);

  // Guard for functional tests JS value checking
  if (id == string::flags.testop) {
    tester::set_value(name);
    return;
  }

  auto args = json::parse(msg, "params", 0);
  auto found = bindings.find(name);
  if (found == bindings.end()) {
    return;
  }
  const auto &context = found->second;
  dispatch_impl([=] { context.call(id, args); });
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
