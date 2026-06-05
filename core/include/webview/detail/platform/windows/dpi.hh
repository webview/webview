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

#ifndef WEBVIEW_PLATFORM_WINDOWS_DPI_HH
#define WEBVIEW_PLATFORM_WINDOWS_DPI_HH

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

#include "../../native_library.hh"
#include "shcore.hh"
#include "user32.hh"
#include "version.hh"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif

namespace webview {
namespace detail {

inline bool is_per_monitor_v2_awareness_available() {
  // Windows 10, version 1703
  return compare_os_version(10, 0, 15063) >= 0;
}

inline bool enable_dpi_awareness() {
  auto user32 = native_library(L"user32.dll");
  if (auto fn = user32.get(user32_symbols::SetProcessDpiAwarenessContext)) {
    auto dpi_awareness =
        reinterpret_cast<user32_symbols::DPI_AWARENESS_CONTEXT>(
            is_per_monitor_v2_awareness_available()
                ? user32_symbols::dpi_awareness::per_monitor_v2_aware
                : user32_symbols::dpi_awareness::per_monitor_aware);
    if (fn(dpi_awareness)) {
      return true;
    }
    return GetLastError() == ERROR_ACCESS_DENIED;
  }
  if (auto shcore = native_library(L"shcore.dll")) {
    if (auto fn = shcore.get(shcore_symbols::SetProcessDpiAwareness)) {
      auto result = fn(shcore_symbols::PROCESS_PER_MONITOR_DPI_AWARE);
      return result == S_OK || result == E_ACCESSDENIED;
    }
  }
  if (auto fn = user32.get(user32_symbols::SetProcessDPIAware)) {
    return !!fn();
  }
  return true;
}

inline bool enable_non_client_dpi_scaling_if_needed(HWND window) {
  auto user32 = native_library(L"user32.dll");
  auto get_ctx_fn = user32.get(user32_symbols::GetWindowDpiAwarenessContext);
  if (!get_ctx_fn) {
    return true;
  }
  auto awareness = get_ctx_fn(window);
  if (!awareness) {
    return false;
  }
  auto ctx_equal_fn = user32.get(user32_symbols::AreDpiAwarenessContextsEqual);
  if (!ctx_equal_fn) {
    return true;
  }
  // EnableNonClientDpiScaling is only needed with per monitor v1 awareness.
  auto per_monitor = reinterpret_cast<user32_symbols::DPI_AWARENESS_CONTEXT>(
      user32_symbols::dpi_awareness::per_monitor_aware);
  if (!ctx_equal_fn(awareness, per_monitor)) {
    return true;
  }
  auto enable_fn = user32.get(user32_symbols::EnableNonClientDpiScaling);
  if (!enable_fn) {
    return true;
  }
  return !!enable_fn(window);
}

constexpr int get_default_window_dpi() {
  return 96; // USER_DEFAULT_SCREEN_DPI
}

inline int get_window_dpi(HWND window) {
  auto user32 = native_library(L"user32.dll");
  if (auto fn = user32.get(user32_symbols::GetDpiForWindow)) {
    auto dpi = static_cast<int>(fn(window));
    return dpi;
  }
  return get_default_window_dpi();
}

constexpr int scale_value_for_dpi(int value, int from_dpi, int to_dpi) {
  return (value * to_dpi) / from_dpi;
}

constexpr SIZE scale_size(int width, int height, int from_dpi, int to_dpi) {
  return {scale_value_for_dpi(width, from_dpi, to_dpi),
          scale_value_for_dpi(height, from_dpi, to_dpi)};
}

inline SIZE make_window_frame_size(HWND window, int width, int height,
                                   int dpi) {
  auto style = GetWindowLong(window, GWL_STYLE);
  RECT r{0, 0, width, height};
  auto user32 = native_library(L"user32.dll");
  if (auto fn = user32.get(user32_symbols::AdjustWindowRectExForDpi)) {
    fn(&r, style, FALSE, 0, static_cast<UINT>(dpi));
  } else {
    AdjustWindowRect(&r, style, 0);
  }
  auto frame_width = r.right - r.left;
  auto frame_height = r.bottom - r.top;
  return {frame_width, frame_height};
}

} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_WINDOWS_DPI_HH
