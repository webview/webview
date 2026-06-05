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

#ifndef WEBVIEW_ERRORS_HH
#define WEBVIEW_ERRORS_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "errors.h"

#include <exception>
#include <string>

namespace webview {

class error_info {
public:
  error_info(webview_error_t code, const std::string &message = {}) noexcept
      : m_code{code}, m_message{message} {}
  error_info() = default;

  webview_error_t code() const { return m_code; }
  const std::string &message() const { return m_message; }

private:
  webview_error_t m_code{WEBVIEW_ERROR_UNSPECIFIED};
  std::string m_message;
};

class exception : public std::exception {
public:
  exception(webview_error_t code, const std::string &message,
            std::exception_ptr cause) noexcept
      : exception{error_info{code, message}, cause} {}

  exception(webview_error_t code, const std::string &message) noexcept
      : exception{error_info{code, message}} {}

  exception(const error_info &error, std::exception_ptr cause) noexcept
      : m_error{error},
        // NOLINTNEXTLINE(bugprone-throw-keyword-missing)
        m_cause{cause} {}

  exception(const error_info &error) noexcept : m_error{error} {}

  exception() = default;

  const error_info &error() const { return m_error; }
  std::exception_ptr cause() const { return m_cause; }

  const char *what() const noexcept override {
    return m_error.message().c_str();
  }

private:
  error_info m_error{WEBVIEW_ERROR_UNSPECIFIED};
  std::exception_ptr m_cause;
};

} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_ERRORS_HH
