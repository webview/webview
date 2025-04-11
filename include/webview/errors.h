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

#ifndef WEBVIEW_ERRORS_H
#define WEBVIEW_ERRORS_H

#include <exception>
#include <string>

namespace webview {

/// @name Errors
/// @{

/**
 * @brief Error codes returned to callers of the API.
 *
 * The following codes are commonly used in the library:
 * - @c WEBVIEW_ERROR_OK
 * - @c WEBVIEW_ERROR_UNSPECIFIED
 * - @c WEBVIEW_ERROR_INVALID_ARGUMENT
 * - @c WEBVIEW_ERROR_INVALID_STATE
 *
 * With the exception of @c WEBVIEW_ERROR_OK which is normally expected,
 * the other common codes do not normally need to be handled specifically.
 * Refer to specific functions regarding handling of other codes.
 */
enum class error_t {
   /// Missing dependency.
   WEBVIEW_ERROR_MISSING_DEPENDENCY = -5,
   /// Operation canceled.
   WEBVIEW_ERROR_CANCELED = -4,
   /// Invalid state detected.
   WEBVIEW_ERROR_INVALID_STATE = -3,
   /// One or more invalid arguments have been specified e.g. in a function call.
   WEBVIEW_ERROR_INVALID_ARGUMENT = -2,
   /// An unspecified error occurred. A more specific error code may be needed.
   WEBVIEW_ERROR_UNSPECIFIED = -1,
   /// OK/Success. Functions that return error codes will typically return this
   /// to signify successful operations.
   WEBVIEW_ERROR_OK = 0,
   /// Signifies that something already exists.
   WEBVIEW_ERROR_DUPLICATE = 1,
   /// Signifies that something does not exist.
   WEBVIEW_ERROR_NOT_FOUND = 2
};

class error_info {
public:
   error_info(error_t code, std::string_view message = {}) noexcept
      : code_{code}
      , message_{message} {}
   error_info() = default;

   error_t            code() const { return code_; }
   std::string const& message() const { return message_; }

private:
   error_t     code_{error_t::WEBVIEW_ERROR_UNSPECIFIED};
   std::string message_;
};

class exception : public std::exception {
public:
   exception(error_t code, std::string_view message, std::exception_ptr cause) noexcept
      : exception{error_info{code, message}, cause} {}

   exception(error_t code, std::string_view message) noexcept
      : exception{error_info{code, message}} {}

   exception(error_info error, std::exception_ptr cause) noexcept
      : error_{error}
      ,
      // NOLINTNEXTLINE(bugprone-throw-keyword-missing)
      cause_{cause} {}

   explicit exception(error_info error) noexcept
      : error_{error} {}

   exception() = default;

   error_info const&  error() const { return error_; }
   std::exception_ptr cause() const { return cause_; }

   const char* what() const noexcept override {
      return error_.message().c_str();
   }

private:
   error_info         error_{error_t::WEBVIEW_ERROR_UNSPECIFIED};
   std::exception_ptr cause_;
};

}  // namespace webview
/// @}

#endif  // WEBVIEW_ERRORS_H