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

#ifndef WEBVIEW_PLATFORM_WINDOWS_VERSION_HH
#define WEBVIEW_PLATFORM_WINDOWS_VERSION_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS)
#ifdef _MSC_VER
#pragma comment(lib, "version.lib")
#endif
#include <array>
#include <string>

namespace webview {
namespace detail {
namespace platform {
namespace windows {

struct version {
  // Parses a version string with 1-4 integral components, e.g. "1.2.3.4".
  // Missing or invalid components default to 0, and excess components are ignored.
  template <typename T>
  static std::array<unsigned int, 4>
  parse_version(const std::basic_string<T> &version) noexcept;

  template <typename T, std::size_t Length>
  static std::array<unsigned int, 4>
  parse_version(const T (&version)[Length]) noexcept;

  static std::wstring
  get_file_version_string(const std::wstring &file_path) noexcept;

  // Compare the specified version against the OS version.
  // Returns less than 0 if the OS version is less.
  // Returns 0 if the versions are equal.
  // Returns greater than 0 if the specified version is greater.
  static int compare_os_version(unsigned int major, unsigned int minor,
                                unsigned int build);
};

} // namespace windows
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_WINDOWS_VERSION_HH
