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

#ifndef WEBVIEW_DETAIL_PLATFORM_REG_KEY_CC
#define WEBVIEW_DETAIL_PLATFORM_REG_KEY_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
#include "detail/platform/windows/reg_key.hh"

using namespace webview::detail::platform::windows;

template <typename Container>
void reg_key::query_bytes(const wchar_t *name, Container &result) const {
  DWORD buf_length = 0;
  // Get the size of the data in bytes.
  auto status =
      RegQueryValueExW(m_handle, name, nullptr, nullptr, nullptr, &buf_length);
  if (status != ERROR_SUCCESS && status != ERROR_MORE_DATA) {
    result.resize(0);
    return;
  }
  // Read the data.
  result.resize(buf_length / sizeof(typename Container::value_type));
  auto *buf = reinterpret_cast<LPBYTE>(&result[0]);
  status = RegQueryValueExW(m_handle, name, nullptr, nullptr, buf, &buf_length);
  if (status != ERROR_SUCCESS) {
    result.resize(0);
    return;
  }
}

std::wstring reg_key::query_string(const wchar_t *name) const {
  std::wstring result;
  query_bytes(name, result);
  // Remove trailing null-characters.
  for (std::size_t length = result.size(); length > 0; --length) {
    if (result[length - 1] != 0) {
      result.resize(length);
      break;
    }
  }
  return result;
}

unsigned int reg_key::query_uint(const wchar_t *name,
                                 unsigned int default_value) const {
  std::vector<char> data;
  query_bytes(name, data);
  if (data.size() < sizeof(DWORD)) {
    return default_value;
  }
  return static_cast<unsigned int>(*reinterpret_cast<DWORD *>(data.data()));
}

#endif // defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_PLATFORM_REG_KEY_CC