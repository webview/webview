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

#ifndef WEBVIEW_PLATFORM_WINDOWS_COM_INIT_WRAPPER_HH
#define WEBVIEW_PLATFORM_WINDOWS_COM_INIT_WRAPPER_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS)

//
// ====================================================================
//
// This implementation uses Win32 API to create a native window. It
// uses Edge/Chromium webview2 backend as a browser engine.
//
// ====================================================================
//

#include "../../../errors.hh"

#include <utility>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <objbase.h>

#ifdef _MSC_VER
#pragma comment(lib, "ole32.lib")
#endif

namespace webview {
namespace detail {

/**
 * A wrapper around COM library initialization. Calls CoInitializeEx in the
 * constructor and CoUninitialize in the destructor.
 *
 * @exception exception Thrown if CoInitializeEx has already been called with a
 * different concurrency model.
 */
class com_init_wrapper {
public:
  com_init_wrapper() = default;

  com_init_wrapper(DWORD dwCoInit) {
    // We can safely continue as long as COM was either successfully
    // initialized or already initialized.
    // RPC_E_CHANGED_MODE means that CoInitializeEx was already called with
    // a different concurrency model.
    switch (CoInitializeEx(nullptr, dwCoInit)) {
    case S_OK:
    case S_FALSE:
      m_initialized = true;
      break;
    case RPC_E_CHANGED_MODE:
      throw exception{
          WEBVIEW_ERROR_INVALID_STATE,
          "CoInitializeEx already called with a different concurrency model"};
    default:
      throw exception{WEBVIEW_ERROR_UNSPECIFIED,
                      "Unexpected result from CoInitializeEx"};
    }
  }

  ~com_init_wrapper() {
    if (m_initialized) {
      CoUninitialize();
      m_initialized = false;
    }
  }

  com_init_wrapper(const com_init_wrapper &other) = delete;
  com_init_wrapper &operator=(const com_init_wrapper &other) = delete;

  com_init_wrapper(com_init_wrapper &&other) noexcept {
    *this = std::move(other);
  }

  com_init_wrapper &operator=(com_init_wrapper &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    m_initialized = other.m_initialized;
    other.m_initialized = false;
    return *this;
  }

private:
  bool m_initialized = false;
};

} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_WINDOWS_COM_INIT_WRAPPER_HH
