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

#ifndef WEBVIEW_DETAIL_USER_SCRIPT_HH
#define WEBVIEW_DETAIL_USER_SCRIPT_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace webview {
namespace detail {

/* ************************************************************************
 * Handlers and types for managing the Webview
 * `bind` -> JS call().then() -> `resolve`
 * execution and promise resolution lifecycle.
 **************************************************************************/

/// Type definition for a native promise callback function,
/// ie. `void callback("id", "params", arg)`;
using binding_t = std::function<void(std::string, std::string, void *)>;

/// Handlers for a bound JS function / native promise resolution lifecycle.
class user_script {
public:
  class impl;
  using impl_deleter = std::function<void(impl *)>;
  using impl_ptr = std::unique_ptr<impl, impl_deleter>;

  user_script(const std::string &code, impl_ptr &&impl_)
      : m_code{code}, m_impl{std::move(impl_)} {}

  user_script(const user_script &other) = delete;
  user_script &operator=(const user_script &other) = delete;
  user_script(user_script &&other) noexcept { *this = std::move(other); }

  user_script &operator=(user_script &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    m_code = std::move(other.m_code);
    m_impl = std::move(other.m_impl);
    return *this;
  }

  const std::string &get_code() const { return m_code; }

  impl &get_impl() { return *m_impl; }

  const impl &get_impl() const { return *m_impl; }

private:
  std::string m_code;
  impl_ptr m_impl;
};

/// A type for handling a bound JS promise resolution on the native side.
class binding_ctx_t {
public:
  binding_ctx_t(binding_t callback, void *arg)
      : m_callback(callback), m_arg(arg) {}
  /// Executes the bound JS function native promise resolution callback.
  void call(std::string id, std::string args) const {
    if (m_callback) {
      m_callback(id, args, m_arg);
    }
  }

private:
  /// This native function is called upon execution of the bound JS function.
  binding_t m_callback;
  /// This user-supplied argument is passed to the native callback function,
  /// ie. `webview_bind(w, "name", fn, **arg**);`
  void *m_arg;
};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_USER_SCRIPT_HH
