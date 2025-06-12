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

#ifndef WEBVIEW_DETAIL_OPTIONAL_HH
#define WEBVIEW_DETAIL_OPTIONAL_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "exceptions.hh"

#include <new>
#include <type_traits>
#include <utility>

namespace webview {
namespace detail {

template <typename T> class optional {
public:
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
  optional() = default;

  optional(const T &other) noexcept : m_has_data{true} {
    new (&m_data) T{other};
  }

  optional(T &&other) noexcept : m_has_data{true} {
    new (&m_data) T{std::move(other)};
  }

  optional(const optional<T> &other) noexcept { *this = other; }

  optional &operator=(const optional<T> &other) noexcept {
    if (this == &other) {
      return *this;
    }
    m_has_data = other.has_value();
    if (m_has_data) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      new (&m_data) T{*reinterpret_cast<const T *>(&other.m_data)};
    }
    return *this;
  }

  optional(optional<T> &&other) noexcept { *this = std::move(other); }

  optional &operator=(optional<T> &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    m_has_data = other.has_value();
    if (m_has_data) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      new (&m_data) T{std::move(*reinterpret_cast<T *>(&other.m_data))};
    }
    return *this;
  }

  ~optional() {
    if (m_has_data) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<T *>(&m_data)->~T();
    }
  }

  const T &get() const {
    if (!m_has_data) {
      throw bad_access{};
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return *reinterpret_cast<const T *>(&m_data);
  }

  T &get() {
    if (!m_has_data) {
      throw bad_access{};
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return *reinterpret_cast<T *>(&m_data);
  }

  bool has_value() const { return m_has_data; }

private:
  // NOLINTNEXTLINE(bugprone-sizeof-expression): pointer to aggregate is OK
  typename std::aligned_storage<sizeof(T), alignof(T)>::type m_data;
  bool m_has_data{};
};

template <> class optional<void> {};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_OPTIONAL_HH
