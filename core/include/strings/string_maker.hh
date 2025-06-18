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

#ifndef WEBVIEW_STRINGS_STRING_MAKER_HH
#define WEBVIEW_STRINGS_STRING_MAKER_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "detail/user/user_script.hh"
#include <map>
#include <string>
#include <vector>

using namespace webview::detail::user;
namespace webview {
namespace _lib {
namespace _strings {

struct tokens_t {
  const std::string str;
  const std::string intval;
  const std::string var;
  const std::string id;
  const std::string status;
  const std::string result;
  const std::string post_fn;
  const std::string js_names;
  const std::string user_js;
  const std::string what;
};

struct sys_flags_t {
  const std::string sysop;
  const std::string testop;
};
struct sys_ops_t {
  std::string dom_ready;
  std::string webview_ready;
  std::string bind_done;
  std::string unbind_done;
  std::string js_eval_start;
};

namespace _frontend {

/* Frontend
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* JS
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

/// Tokenised JS function strings
struct js_string_t {
  js_string_t() noexcept = default;

  /// Returns a tokenised JS function string for `unbind` which notifies that
  /// a binding was destroyed after the init script has already set things up.
  std::string onunbind(const std::string &name) const;

  /// Returns a tokenised JS function string for `bind` which notifies that
  /// a binding was created after the init script has already set things up.
  std::string onbind(const std::string &name) const;

  /// Returns a tokenised JS function string for a promise resolve/reject.
  std::string onreply(const std::string &id, int status,
                      const std::string &result) const;

  /// Returns a tokenised JS string for the Webview frontend init function.
  std::string init(const std::string &post_fn) const;

  /// Returns a tokenised JS string for the Webview frontend `bind` functions.
  std::string bind(std::vector<std::string> &bound_names) const;

  /// Wraps user JS to notify the native code when eval is ready.
  std::string eval_wrapper(const std::string &user_js) const;

  /// Wraps user init JS to prevent uncaught errors.
  std::string user_init_wrapper(const std::string &user_js) const;

  /// Returns a platform specific window message handler to register.
  std::string post_fnc() const;
};

/* Message strings
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

/// Tokenised error message strings
struct error_message_t {
  error_message_t() noexcept = default;

  /// Returns a tokenised error string for invlaid arg being passed to a C API call.
  std::string api_invalid_arg(const std::string &arg,
                              const std::string &api_name) const;

  /// Returns a tokenised error string for missing arg being passed to a C API call.
  std::string api_missing_arg(const std::string &arg,
                              const std::string &api_name) const;

  /// Returns a tokenised error string for nullptr being passed to a C API call.
  std::string api_null_w(const std::string &api_name) const;

  /// Returns a tokenised error string for rejecting a promise if a callback binding was unbound.
  std::string reject_unbound(const std::string &id,
                             const std::string &name) const;

  /// Returns a tokenised error string for rejecting a promise if a native callback has an uncaught exception.
  std::string uncaught_exception(const std::string &name,
                                 const std::string &what) const;

  /// Returns a tokenised error string for native callbacks in detached threads after webview terminates.
  std::string webview_terminated(const std::string &name) const;
};

} // namespace _frontend
} // namespace _strings
} // namespace _lib
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_STRINGS_STRING_MAKER_HH
