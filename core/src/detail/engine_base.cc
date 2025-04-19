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
#include "webview/detail/engine_js.hh"
#include "webview/detail/json.hh"
#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <vector>

using namespace webview::detail;

noresult engine_base::navigate(const std::string &url) {
  if (url.empty()) {
    return navigate_impl("about:blank");
  }
  return navigate_impl(url);
}
noresult engine_base::bind(const std::string &name, sync_binding_t fn) {
  auto wrapper = [this, fn](const std::string &id, const std::string &req,
                            void * /*arg*/) { resolve(id, 0, fn(req)); };
  return bind(name, wrapper, nullptr);
}
noresult engine_base::bind(const std::string &name, binding_t fn, void *arg) {
  if (bindings.count(name) > 0) {
    return error_info{WEBVIEW_ERROR_DUPLICATE};
  }
  bindings.emplace(name, binding_ctx_t(fn, arg));
  replace_bind_script();

  printf("Webview: bind: \n%s\n", engine_js::onbind(name).c_str());

  eval(engine_js::onbind(name));
  return {};
}
noresult engine_base::unbind(const std::string &name) {
  auto found = bindings.find(name);
  if (found == bindings.end()) {
    return error_info{WEBVIEW_ERROR_NOT_FOUND};
  }
  bindings.erase(found);
  replace_bind_script();

  printf("Webview: unbind: \n%s\n", engine_js::onunbind(name).c_str());

  eval(engine_js::onunbind(name));
  return {};
}
noresult engine_base::resolve(const std::string &id, int status,
                              const std::string &result) {
  const std::string &escaped_result =
      result.empty() ? "undefined" : json_escape(result);

  const std::string &promised_js =
      engine_js::onreply(id, status, escaped_result);

  return dispatch([this, promised_js] { eval(promised_js); });
}
noresult engine_base::run() { return run_impl(); }
noresult engine_base::terminate() { return terminate_impl(); }
noresult engine_base::dispatch(std::function<void()> f) {
  return dispatch_impl(f);
}
noresult engine_base::set_title(const std::string &title) {
  return set_title_impl(title);
}
noresult engine_base::set_size(int width, int height, webview_hint_t hints) {
  auto res = set_size_impl(width, height, hints);
  m_is_size_set = true;
  return res;
}
noresult engine_base::set_html(const std::string &html) {
  return set_html_impl(html);
}
noresult engine_base::init(const std::string &js) {
  add_user_script(js);
  return {};
}
noresult engine_base::eval(const std::string &js) { return eval_impl(js); }
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
  auto replacement_js = create_bind_script();
  if (m_bind_script) {
    m_bind_script = replace_user_script(*m_bind_script, replacement_js);
  } else {
    m_bind_script = add_user_script(replacement_js);
  }
}
void engine_base::on_message(const std::string &msg) {
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

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_BASE_CC
