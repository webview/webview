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

#ifndef WEBVIEW_PLATFORM_WINDOWS_THEME_HH
#define WEBVIEW_PLATFORM_WINDOWS_THEME_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS)

#include "../../native_library.hh"
#include "dwmapi.hh"
#include "reg_key.hh"

namespace webview {
namespace detail {

inline bool is_dark_theme_enabled() {
  constexpr auto *sub_key =
      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
  reg_key key(HKEY_CURRENT_USER, sub_key, 0, KEY_READ);
  if (!key.is_open()) {
    // Default is light theme
    return false;
  }
  return key.query_uint(L"AppsUseLightTheme", 1) == 0;
}

inline void apply_window_theme(HWND window) {
  auto dark_theme_enabled = is_dark_theme_enabled();

  // Use "immersive dark mode" on systems that support it.
  // Changes the color of the window's title bar (light or dark).
  BOOL use_dark_mode{dark_theme_enabled ? TRUE : FALSE};
  static native_library dwmapi{L"dwmapi.dll"};
  if (auto fn = dwmapi.get(dwmapi_symbols::DwmSetWindowAttribute)) {
    // Try the modern, documented attribute before the older, undocumented one.
    if (fn(window, dwmapi_symbols::DWMWA_USE_IMMERSIVE_DARK_MODE,
           &use_dark_mode, sizeof(use_dark_mode)) != S_OK) {
      fn(window,
         dwmapi_symbols::DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_V10_0_19041,
         &use_dark_mode, sizeof(use_dark_mode));
    }
  }
}

} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_WINDOWS_THEME_HH
