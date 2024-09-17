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

#ifndef WEBVIEW_MACROS_H
#define WEBVIEW_MACROS_H

/**
 * Used to specify function linkage such as extern, inline, etc.
 *
 * When @c WEBVIEW_API is not already defined, the defaults are as follows:
 *
 * - @c inline when compiling C++ code.
 * - @c extern when compiling C code.
 *
 * The following macros can be used to automatically set an appropriate
 * value for @c WEBVIEW_API:
 *
 * - Define @c WEBVIEW_BUILD_SHARED when building a shared library.
 * - Define @c WEBVIEW_SHARED when using a shared library.
 * - Define @c WEBVIEW_STATIC when building or using a static library.
 */
#ifndef WEBVIEW_API
#if defined(WEBVIEW_SHARED) || defined(WEBVIEW_BUILD_SHARED)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(WEBVIEW_BUILD_SHARED)
#define WEBVIEW_API __declspec(dllexport)
#else
#define WEBVIEW_API __declspec(dllimport)
#endif
#else
#define WEBVIEW_API __attribute__((visibility("default")))
#endif
#elif !defined(WEBVIEW_STATIC) && defined(__cplusplus)
#define WEBVIEW_API inline
#else
#define WEBVIEW_API extern
#endif
#endif

/// @name Used internally
/// @{

/// Utility macro for stringifying a macro argument.
#define WEBVIEW_STRINGIFY(x) #x

/// Utility macro for stringifying the result of a macro argument expansion.
#define WEBVIEW_EXPAND_AND_STRINGIFY(x) WEBVIEW_STRINGIFY(x)

/// @}

/// @brief Evaluates to @c TRUE for error codes indicating success or
///        additional information.
#define WEBVIEW_SUCCEEDED(error) ((int)(error) >= 0)

/// Evaluates to @c TRUE if the given error code indicates failure.
#define WEBVIEW_FAILED(error) ((int)(error) < 0)

#ifdef __cplusplus
#ifndef WEBVIEW_HEADER

#if defined(__APPLE__)
#define WEBVIEW_PLATFORM_DARWIN
#elif defined(__unix__)
#define WEBVIEW_PLATFORM_LINUX
#elif defined(_WIN32)
#define WEBVIEW_PLATFORM_WINDOWS
#else
#error "Unable to detect current platform"
#endif

#if !defined(WEBVIEW_GTK) && !defined(WEBVIEW_COCOA) && !defined(WEBVIEW_EDGE)
#if defined(WEBVIEW_PLATFORM_DARWIN)
#define WEBVIEW_COCOA
#elif defined(WEBVIEW_PLATFORM_LINUX)
#define WEBVIEW_GTK
#elif defined(WEBVIEW_PLATFORM_WINDOWS)
#define WEBVIEW_EDGE
#else
#error "please, specify webview backend"
#endif
#endif

#ifndef WEBVIEW_DEPRECATED
#if __cplusplus >= 201402L
#define WEBVIEW_DEPRECATED(reason) [[deprecated(reason)]]
#elif defined(_MSC_VER)
#define WEBVIEW_DEPRECATED(reason) __declspec(deprecated(reason))
#else
#define WEBVIEW_DEPRECATED(reason) __attribute__((deprecated(reason)))
#endif
#endif

#ifndef WEBVIEW_DEPRECATED_PRIVATE
#define WEBVIEW_DEPRECATED_PRIVATE                                             \
  WEBVIEW_DEPRECATED("Private API should not be used")
#endif

#endif // WEBVIEW_HEADER
#endif // __cplusplus

#endif // WEBVIEW_MACROS_H
