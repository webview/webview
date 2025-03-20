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

#ifndef WEBVIEW_DETAIL_BASIC_RESULT_HH
#define WEBVIEW_DETAIL_BASIC_RESULT_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "exceptions.hh"
#include "optional.hh"

#include <utility>

namespace webview {
namespace detail {

template <typename Value, typename Error, typename Exception>
class basic_result {
public:
  using value_type = Value;
  using error_type = Error;
  using exception_type = Exception;

  basic_result() : basic_result(value_type{}) {}

  basic_result(const value_type &value) : m_value{value} {}
  basic_result(value_type &&value) : m_value{std::forward<value_type>(value)} {}

  basic_result(const error_type &error) : m_error{error} {}
  basic_result(error_type &&error) : m_error{std::forward<error_type>(error)} {}

  bool ok() const { return has_value() && !has_error(); }
  bool has_value() const { return m_value.has_value(); }
  bool has_error() const { return m_error.has_value(); }

  void ensure_ok() {
    if (!ok()) {
      throw exception_type{error()};
    }
  }

  const value_type &value() const {
    if (!has_value()) {
      throw bad_access{};
    }
    return m_value.get();
  }

  const error_type &error() const {
    if (!has_error()) {
      throw bad_access{};
    }
    return m_error.get();
  }

private:
  optional<value_type> m_value;
  optional<error_type> m_error;
};

template <typename Error, typename Exception>
class basic_result<void, Error, Exception> {
public:
  using value_type = void;
  using error_type = Error;
  using exception_type = Exception;

  basic_result() = default;

  basic_result(error_type &&error) : m_error{std::forward<error_type>(error)} {}

  bool ok() const { return !has_error(); }

  bool has_error() const { return m_error.has_value(); }

  void ensure_ok() {
    if (!ok()) {
      throw exception_type{error()};
    }
  }

  const error_type &error() const {
    if (!has_error()) {
      throw bad_access{};
    }
    return m_error.get();
  }

private:
  optional<error_type> m_error;
};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_BASIC_RESULT_HH
