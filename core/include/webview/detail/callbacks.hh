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

#ifndef WEBVIEW_DETAIL_DECIDE_POLICY_NAVIGATION_HH
#define WEBVIEW_DETAIL_DECIDE_POLICY_NAVIGATION_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace webview {
namespace detail {

template <typename Signature> class callback_ctx_t {
  using callback_t = std::function<Signature>;
  using return_type = typename callback_t::result_type;

  // Type used for the DefaultReturn non-type template parameter.
  // A non-type template parameter can't have type void, so fall back to
  // int in that case (the overload is never usable for void anyway).
  using default_return_t =
      typename std::conditional<std::is_void<return_type>::value, int,
                                return_type>::type;

public:
  callback_ctx_t() = default;
  explicit callback_ctx_t(callback_t callback, void *arg) noexcept
      : m_callback(std::move(callback)), m_arg(arg) {}

  // Calls the callback and returns its result, or DefaultReturn if no
  // callback is set. The `m_arg` is added as last parameter.
  // Usage: ctx.call<false>(url);
  template <default_return_t DefaultReturn, typename... Args>
  return_type call(Args &&...args) const {
    static_assert(!std::is_void<return_type>::value,
                  "DefaultReturn cannot be used with void-returning callbacks");
    if (!m_callback) {
      return DefaultReturn;
    }
    return m_callback(std::forward<Args>(args)..., m_arg);
  }

  // Calls the callback without using its return value.
  // The `m_arg` is added as last parameter.
  template <typename... Args> void call(Args &&...args) const {
    if (m_callback) {
      m_callback(std::forward<Args>(args)..., m_arg);
    }
  }
  operator bool() const { return bool(m_callback); }

private:
  callback_t m_callback;
  void *m_arg{};
};

using decide_policy_navigation_t =
    std::function<bool(const char *url, bool, void *arg)>;
using decide_policy_navigation_ctx_t =
    callback_ctx_t<bool(const char *url, bool, void *arg)>;
using navigation_error_t = std::function<bool(int httpErrorCode, void *arg)>;
using navigation_error_ctx_t =
    callback_ctx_t<bool(int httpErrorCode, void *arg)>;

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_DECIDE_POLICY_NAVIGATION_HH
