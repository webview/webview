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

#ifndef WEBVIEW_DETAIL_NATIVE_LIBRARY_HH
#define WEBVIEW_DETAIL_NATIVE_LIBRARY_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "utility/string.hh"

#include <string>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace webview {
namespace detail {

// Holds a symbol name and associated type for code clarity.
template <typename T> class library_symbol {
public:
  using type = T;

  constexpr explicit library_symbol(const char *name) : m_name(name) {}
  constexpr const char *get_name() const { return m_name; }

private:
  const char *m_name;
};

// Loads a native shared library and allows one to get addresses for those
// symbols.
class native_library {
public:
  native_library() = default;

  explicit native_library(const std::string &name)
      : m_handle{load_library(name)} {}

#ifdef _WIN32
  explicit native_library(const std::wstring &name)
      : m_handle{load_library(name)} {}
#endif

  ~native_library() {
    if (m_handle) {
#ifdef _WIN32
      FreeLibrary(m_handle);
#else
      dlclose(m_handle);
#endif
      m_handle = nullptr;
    }
  }

  native_library(const native_library &other) = delete;
  native_library &operator=(const native_library &other) = delete;
  native_library(native_library &&other) noexcept { *this = std::move(other); }

  native_library &operator=(native_library &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    m_handle = other.m_handle;
    other.m_handle = nullptr;
    return *this;
  }

  // Returns true if the library is currently loaded; otherwise false.
  operator bool() const { return is_loaded(); }

  // Get the address for the specified symbol or nullptr if not found.
  template <typename Symbol>
  typename Symbol::type get(const Symbol &symbol) const {
    if (is_loaded()) {
      // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
#ifdef _WIN32
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
      return reinterpret_cast<typename Symbol::type>(
          GetProcAddress(m_handle, symbol.get_name()));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#else
      return reinterpret_cast<typename Symbol::type>(
          dlsym(m_handle, symbol.get_name()));
#endif
      // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    }
    return nullptr;
  }

  // Returns true if the library is currently loaded; otherwise false.
  bool is_loaded() const { return !!m_handle; }

  void detach() { m_handle = nullptr; }

  // Returns true if the library by the given name is currently loaded; otherwise false.
  static inline bool is_loaded(const std::string &name) {
#ifdef _WIN32
    auto handle = GetModuleHandleW(widen_string(name).c_str());
#else
    auto handle = dlopen(name.c_str(), RTLD_NOW | RTLD_NOLOAD);
    if (handle) {
      dlclose(handle);
    }
#endif
    return !!handle;
  }

private:
#ifdef _WIN32
  using mod_handle_t = HMODULE;
#else
  using mod_handle_t = void *;
#endif

  static inline mod_handle_t load_library(const std::string &name) {
#ifdef _WIN32
    return load_library(widen_string(name));
#else
    return dlopen(name.c_str(), RTLD_NOW);
#endif
  }

#ifdef _WIN32
  static inline mod_handle_t load_library(const std::wstring &name) {
    return LoadLibraryW(name.c_str());
  }
#endif

  mod_handle_t m_handle{};
};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_NATIVE_LIBRARY_HH
