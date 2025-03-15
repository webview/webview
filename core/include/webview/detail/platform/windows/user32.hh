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

#ifndef WEBVIEW_PLATFORM_WINDOWS_USER32_HH
#define WEBVIEW_PLATFORM_WINDOWS_USER32_HH

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
namespace user32_symbols {

using DPI_AWARENESS_CONTEXT = HANDLE;
using SetProcessDpiAwarenessContext_t = BOOL(WINAPI *)(DPI_AWARENESS_CONTEXT);
using SetProcessDPIAware_t = BOOL(WINAPI *)();
using GetDpiForWindow_t = UINT(WINAPI *)(HWND);
using EnableNonClientDpiScaling_t = BOOL(WINAPI *)(HWND);
using AdjustWindowRectExForDpi_t = BOOL(WINAPI *)(LPRECT, DWORD, BOOL, DWORD,
                                                  UINT);
using GetWindowDpiAwarenessContext_t = DPI_AWARENESS_CONTEXT(WINAPI *)(HWND);
using AreDpiAwarenessContextsEqual_t = BOOL(WINAPI *)(DPI_AWARENESS_CONTEXT,
                                                      DPI_AWARENESS_CONTEXT);

// Use intptr_t as the underlying type because we need to
// reinterpret_cast<DPI_AWARENESS_CONTEXT> which is a pointer.
// Available since Windows 10, version 1607
enum class dpi_awareness : intptr_t {
  per_monitor_v2_aware = -4, // Available since Windows 10, version 1703
  per_monitor_aware = -3
};

constexpr auto SetProcessDpiAwarenessContext =
    library_symbol<SetProcessDpiAwarenessContext_t>(
        "SetProcessDpiAwarenessContext");
constexpr auto SetProcessDPIAware =
    library_symbol<SetProcessDPIAware_t>("SetProcessDPIAware");
constexpr auto GetDpiForWindow =
    library_symbol<GetDpiForWindow_t>("GetDpiForWindow");
constexpr auto EnableNonClientDpiScaling =
    library_symbol<EnableNonClientDpiScaling_t>("EnableNonClientDpiScaling");
constexpr auto AdjustWindowRectExForDpi =
    library_symbol<AdjustWindowRectExForDpi_t>("AdjustWindowRectExForDpi");
constexpr auto GetWindowDpiAwarenessContext =
    library_symbol<GetWindowDpiAwarenessContext_t>(
        "GetWindowDpiAwarenessContext");
constexpr auto AreDpiAwarenessContextsEqual =
    library_symbol<AreDpiAwarenessContextsEqual_t>(
        "AreDpiAwarenessContextsEqual");

} // namespace user32_symbols
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_WINDOWS_USER32_HH
