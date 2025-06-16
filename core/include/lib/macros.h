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

#ifndef WEBVIEW_LIB_MACROS_H
#define WEBVIEW_LIB_MACROS_H

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
#if (defined(_WIN32) || defined(__CYGWIN__)) &&                                \
    (defined(WEBVIEW_SHARED) || defined(WEBVIEW_BUILD_SHARED))
// Windows shared
#if defined(WEBVIEW_BUILD_SHARED)
#define WEBVIEW_API __declspec(dllexport)
#else
#define WEBVIEW_API __declspec(dllimport)
#endif

#elif defined(WEBVIEW_SHARED) || defined(WEBVIEW_BUILD_SHARED)
// NIX shared
#define WEBVIEW_API __attribute__((visibility("default")))

#elif !defined(WEBVIEW_STATIC) && defined(__cplusplus)
// C++ code
#define WEBVIEW_API inline

#else
// C code
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
#endif // WEBVIEW_HEADER
#endif // __cplusplus
#if defined(__cplusplus) && __cplusplus >= 201402L
#ifndef WEBVIEW_DEPRECATED
#define WEBVIEW_DEPRECATED(reason) [[deprecated(reason)]]
#endif
#elif defined(__GNUC__) || defined(__clang__)
#ifndef WEBVIEW_DEPRECATED
#define WEBVIEW_DEPRECATED(reason) __attribute__((deprecated(reason)))
#endif
#elif defined(_MSC_VER)
#ifndef WEBVIEW_DEPRECATED
#define WEBVIEW_DEPRECATED(reason) __declspec(deprecated(reason))
#endif
#else
#ifndef WEBVIEW_DEPRECATED
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define WEBVIEW_DEPRECATED(reason)
#endif
#endif

#ifndef WEBVIEW_DEPRECATED_PRIVATE
#define WEBVIEW_DEPRECATED_PRIVATE                                             \
  WEBVIEW_DEPRECATED("Private API should not be used")
#endif

#ifndef DEPRECATE_WEBVIEW_DISPATCH
#define DEPRECATE_WEBVIEW_DISPATCH                                             \
  "since 0.12.2\n\
Webview now guarantees thread-safety, so dispatch is redundant and will be removed in a future release.\n\
Please call all API methods directly."
#endif

#ifndef DEPRECATE_WEBVIEW_DESTROY
#define DEPRECATE_WEBVIEW_DESTROY                                              \
  "since 0.12.2\n\
This legacy API call was violating RAII. It now redirects to `webview_terminate` and will be removed in a future release.\n\
Please use `webview_terminate` instead and omit the use of `webview_destroy` after `webview_run`."
#endif

#ifndef DEPRECATE_WEBVIEW_WEBVIEW
#define DEPRECATE_WEBVIEW_WEBVIEW                                              \
  "since 0.12.1\n\n\
`webview::webview` is ambiguous for compilers and humans, so it will be removed in a future release.\n\
Please use `webview_cc` from the Global namespace instead."
#endif

#ifdef _MSC_VER
#define RESTORE_IGNORED_WARNINGS __pragma(warning(pop))
#else
#define RESTORE_IGNORED_WARNINGS _Pragma("GCC diagnostic pop")
#endif

#ifdef _MSC_VER
#define IGNORE_UNUSED_FUNCTIONS                                                \
  __pragma(warning(push)) __pragma(warning(disable : 4505))
#else
#define IGNORE_UNUSED_FUNCTIONS                                                \
  _Pragma("GCC diagnostic push")                                               \
      _Pragma("GCC diagnostic ignored \"-Wunused-function\"")
#endif

#ifdef _MSC_VER
#define IGNORE_DEPRECATED_DECLARATIONS                                         \
  __pragma(warning(push)) __pragma(warning(disable : 4996))
#else
#define IGNORE_DEPRECATED_DECLARATIONS                                         \
  _Pragma("GCC diagnostic push")                                               \
      _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#endif

#endif // WEBVIEW_LIB_MACROS_H
