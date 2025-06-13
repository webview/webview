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

#ifndef WEBVIEW_DETAIL_PLATFORM_WINDOWS_STRING_HH
#define WEBVIEW_DETAIL_PLATFORM_WINDOWS_STRING_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <string>

namespace webview {
namespace detail {
namespace platform {
namespace windows {

struct string {
  // Converts a narrow (UTF-8-encoded) string into a wide (UTF-16-encoded) string.
  static std::wstring widen_string(const std::string &input) {
    if (input.empty()) {
      return {};
    }
    UINT cp = CP_UTF8;
    DWORD flags = MB_ERR_INVALID_CHARS;
    auto input_c = input.c_str();
    auto input_length = static_cast<int>(input.size());
    auto required_length =
        MultiByteToWideChar(cp, flags, input_c, input_length, nullptr, 0);
    if (required_length > 0) {
      std::wstring output(static_cast<std::size_t>(required_length), L'\0');
      if (MultiByteToWideChar(cp, flags, input_c, input_length, &output[0],
                              required_length) > 0) {
        return output;
      }
    }
    // Failed to convert string from UTF-8 to UTF-16
    return {};
  }

  // Converts a wide (UTF-16-encoded) string into a narrow (UTF-8-encoded) string.
  static std::string narrow_string(const std::wstring &input) {
    struct wc_flags {
      enum TYPE : unsigned int {
        // WC_ERR_INVALID_CHARS
        err_invalid_chars = 0x00000080U
      };
    };
    if (input.empty()) {
      return {};
    }
    UINT cp = CP_UTF8;
    DWORD flags = wc_flags::err_invalid_chars;
    auto input_c = input.c_str();
    auto input_length = static_cast<int>(input.size());
    auto required_length = WideCharToMultiByte(cp, flags, input_c, input_length,
                                               nullptr, 0, nullptr, nullptr);
    if (required_length > 0) {
      std::string output(static_cast<std::size_t>(required_length), '\0');
      if (WideCharToMultiByte(cp, flags, input_c, input_length, &output[0],
                              required_length, nullptr, nullptr) > 0) {
        return output;
      }
    }
    // Failed to convert string from UTF-16 to UTF-8
    return {};
  }
};

} // namespace windows
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_PLATFORM_WINDOWS_STRING_HH
