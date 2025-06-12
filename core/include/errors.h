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
typedef enum {
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
} webview_error_t;

/// @}

#endif // WEBVIEW_ERRORS_H
