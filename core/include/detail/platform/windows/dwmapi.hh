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

#ifndef WEBVIEW_PLATFORM_WINDOWS_DWMAPI_HH
#define WEBVIEW_PLATFORM_WINDOWS_DWMAPI_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS)

#include "../../native_library.hh"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

namespace webview {
namespace detail {
namespace dwmapi_symbols {

typedef enum {
  // This undocumented value is used instead of DWMWA_USE_IMMERSIVE_DARK_MODE
  // on Windows 10 older than build 19041 (2004/20H1).
  DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_V10_0_19041 = 19,
  // Documented as being supported since Windows 11 build 22000 (21H2) but it
  // works since Windows 10 build 19041 (2004/20H1).
  DWMWA_USE_IMMERSIVE_DARK_MODE = 20
} DWMWINDOWATTRIBUTE;

using DwmSetWindowAttribute_t = HRESULT(WINAPI *)(HWND, DWORD, LPCVOID, DWORD);

constexpr auto DwmSetWindowAttribute =
    library_symbol<DwmSetWindowAttribute_t>("DwmSetWindowAttribute");

} // namespace dwmapi_symbols
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_WINDOWS_DWMAPI_HH
