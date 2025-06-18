/*
 * MIT License
 *
 * Copyright (c) 2025 Michael Jonker
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

#ifndef WEBVIEW_STRINGS_STRING_MAKER_CC
#define WEBVIEW_STRINGS_STRING_MAKER_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "strings/string_maker.hh"
#include "detail/user/user_script.hh"
#include "strings/json.hh"
#include "strings/string_api.hh"
#include "strings/string_templates.hh"

using namespace webview::detail::user;
using namespace webview::strings;
using namespace webview::_lib::_strings;
using namespace webview::_lib::_strings::_frontend;
using namespace webview::_lib::_templates;

/* Frontend
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* JS
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

std::string js_string_t::onunbind(const std::string &name) const {
  return string::tokenise(TEMPLATE_ON_UNBIND_JS(), string::string::tokens.str,
                          name);
}
std::string js_string_t::onbind(const std::string &name) const {
  return string::tokenise(TEMPLATE_ON_BIND_JS(), string::tokens.str, name);
}
std::string js_string_t::onreply(const std::string &id, int status,
                                 const std::string &escaped_result) const {
  strg_replacements_t replacements = {
      {string::tokens.id, id},
      {string::tokens.status, std::to_string(status)},
      {string::tokens.result, escaped_result}};
  return string::tokenise(TEMPLATE_ON_REPLY_JS(), replacements);
}
std::string js_string_t::init(const std::string &post_fn) const {
  return string::tokenise(TEMPLATE_WEVBIEW_INIT_JS(), string::tokens.post_fn,
                          post_fn);
}
std::string
js_string_t::bind(std::map<std::string, binding_ctx_t> &bindings) const {
  std::vector<std::string> bound_names;
  bound_names.reserve(bindings.size());
  std::transform(bindings.begin(), bindings.end(),
                 std::back_inserter(bound_names),
                 [](const std::pair<std::string, binding_ctx_t> &pair) {
                   return pair.first;
                 });
  auto js_names = json::to_list(bound_names);
  return string::tokenise(TEMPLATE_BIND_JS(), string::tokens.js_names,
                          js_names);
}
std::string js_string_t::eval_wrapper(const std::string &user_js) const {
  return string::tokenise(TEMPLATE_EVAL_WRAPPER_JS(), string::tokens.user_js,
                          user_js);
}
std::string js_string_t::user_init_wrapper(const std::string &user_js) const {
  return string::tokenise(TEMPLATE_USER_INIT_WRAPPER_JS(),
                          string::tokens.user_js, user_js);
}

std::string js_string_t::post_fnc() const { return TEMPLATE_POST_FUNCTION(); };

/* Message strings
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

std::string error_message_t::reject_unbound(const std::string &id,
                                            const std::string &name) const {
  strg_replacements_t replacements = {{string::tokens.id, id},
                                      {string::tokens.str, name}};
  return string::tokenise(TEMPLATE_REJECT_UNBOUND_M(), replacements);
}
std::string error_message_t::uncaught_exception(const std::string &name,
                                                const std::string &what) const {
  strg_replacements_t replacements = {{string::tokens.str, name},
                                      {string::tokens.what, what}};
  return string::tokenise(TEMPLATE_UNCAUGHT_EXP_M(), replacements);
}
std::string error_message_t::webview_terminated(const std::string &name) const {
  return string::tokenise(TEMPLATE_WEBVIEW_TERMINATED_M(), string::tokens.str,
                          name);
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * Frontend */

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_STRINGS_STRING_MAKER_CC
