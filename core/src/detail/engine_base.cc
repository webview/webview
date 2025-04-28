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

#include "webview/detail/engine_base.hh"
#include "webview/detail/engine_frontend.hh"
#include "webview/detail/json.hh"
#include <algorithm>
#include <functional>
#include <iterator>
#include <string>

namespace webview {

// Container: `engine_base` PUBLIC implementations
namespace detail {

engine_base::engine_base(bool owns_window) : m_owns_window(owns_window) {}

noresult engine_base::navigate(str_arg_t url) {
  if (url.empty()) {
    return navigate_impl("about:blank");
  }
  return navigate_impl(url);
}

noresult engine_base::bind(str_arg_t name, sync_binding_t fn) {
  auto wrapper = [this, fn](str_arg_t id, str_arg_t req, void * /*arg*/) {
    resolve(id, 0, fn(req));
  };
  return bind(name, wrapper, nullptr);
}

noresult engine_base::bind(str_arg_t name, binding_t fn, void *arg) {
  if (bindings.count(name) > 0) {
    return error_info{WEBVIEW_ERROR_DUPLICATE};
  }
  bindings.emplace(name, binding_ctx_t(fn, arg));
  replace_bind_script();
  eval(engine_js::onbind(name));
  return {};
}

noresult engine_base::unbind(str_arg_t name) {
  auto found = bindings.find(name);
  if (found == bindings.end()) {
    return error_info{WEBVIEW_ERROR_NOT_FOUND};
  }
  bindings.erase(found);
  replace_bind_script();
  eval(engine_js::onunbind(name));
  return {};
}

noresult engine_base::resolve(str_arg_t id, int status, str_arg_t result) {
  str_arg_t escaped_result = result.empty() ? "undefined" : json_escape(result);
  str_arg_t promised_js = engine_js::onreply(id, status, escaped_result);

  return dispatch([this, promised_js] { eval(promised_js); });
}

noresult engine_base::run() { return run_impl(); }

noresult engine_base::terminate() { return terminate_impl(); }

noresult engine_base::dispatch(std::function<void()> f) {
  return dispatch_impl(f);
}

noresult engine_base::set_title(str_arg_t title) {
  return set_title_impl(title);
}

noresult engine_base::set_size(int width, int height, webview_hint_t hints) {
  auto res = set_size_impl(width, height, hints);
  m_is_size_set = true;
  return res;
}

noresult engine_base::set_html(str_arg_t html) { return set_html_impl(html); }

noresult engine_base::init(str_arg_t js) {
  add_user_script(js);
  return {};
}
noresult engine_base::eval(str_arg_t js) { return eval_impl(js); }

} // namespace detail

// Container: `engine_base` PROTECTED implementations
namespace detail {

user_script *engine_base::add_user_script(str_arg_t js) {
  return std::addressof(
      *m_user_scripts.emplace(m_user_scripts.end(), add_user_script_impl(js)));
}

user_script *engine_base::replace_user_script(const user_script &old_script,
                                              str_arg_t new_script_code) {
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
  auto replacement_js = create_bind_script();
  if (m_bind_script) {
    m_bind_script = replace_user_script(*m_bind_script, replacement_js);
  } else {
    m_bind_script = add_user_script(replacement_js);
  }
}

void engine_base::add_init_script(str_arg_t post_fn) {
  add_user_script(frontend.js.init(post_fn));
  m_is_init_script_sent = true;
}

std::string engine_base::create_bind_script() {
  std::vector<std::string> bound_names;
  bound_names.reserve(bindings.size());
  std::transform(bindings.begin(), bindings.end(),
                 std::back_inserter(bound_names),
                 [](const std::pair<std::string, binding_ctx_t> &pair) {
                   return pair.first;
                 });
  return frontend.js.bind(bound_names);
}

void engine_base::on_message(str_arg_t msg) {
  auto id = json_parse(msg, "id", 0);
  auto name = json_parse(msg, "method", 0);
  auto args = json_parse(msg, "params", 0);
  auto found = bindings.find(name);
  if (found == bindings.end()) {
    return;
  }
  const auto &context = found->second;
  dispatch([=] { context.call(id, args); });
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
  dispatch([&] { done = true; });
  run_event_loop_while([&] { return !done; });
}

void engine_base::dispatch_size_default() {
  if (!owns_window() || !m_is_init_script_sent) {
    return;
  };
  dispatch([this]() {
    if (!m_is_size_set) {
      set_size(m_initial_width, m_initial_height, WEBVIEW_HINT_NONE);
    }
  });
}

void engine_base::set_default_size_guard(bool guarded) {
  m_is_size_set = guarded;
}

bool engine_base::owns_window() const { return m_owns_window; }

} // namespace detail

// Container: `engine_base` PRIVATE implementations
namespace detail {

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

} // namespace detail
} // namespace webview
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_BASE_CC
