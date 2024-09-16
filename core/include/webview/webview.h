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

/// @file webview.h

#ifndef WEBVIEW_H
#define WEBVIEW_H

#include "macros.h"
#include "types.h"
#include "api.h"

#ifdef __cplusplus
#ifndef WEBVIEW_HEADER

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <functional>
#include <future>
#include <list>
#include <map>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <cstring>

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

class bad_access : public std::exception {};

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

template <typename Value, typename Error, typename Exception>
class basic_result {
public:
  using value_type = Value;
  using error_type = Error;
  using exception_type = Exception;

  basic_result() : basic_result(value_type{}) {}

  basic_result(const value_type &value) : m_value{value} {}
  basic_result(value_type &&value) : m_value{std::forward<value_type>(value)} {}

  basic_result(const error_type &error) : m_error{error} {}
  basic_result(error_type &&error) : m_error{std::forward<error_type>(error)} {}

  bool ok() const { return has_value() && !has_error(); }
  bool has_value() const { return m_value.has_value(); }
  bool has_error() const { return m_error.has_value(); }

  void ensure_ok() {
    if (!ok()) {
      throw exception_type{error()};
    }
  }

  const value_type &value() const {
    if (!has_value()) {
      throw bad_access{};
    }
    return m_value.get();
  }

  const error_type &error() const {
    if (!has_error()) {
      throw bad_access{};
    }
    return m_error.get();
  }

private:
  optional<value_type> m_value;
  optional<error_type> m_error;
};

template <typename Error, typename Exception>
class basic_result<void, Error, Exception> {
public:
  using value_type = void;
  using error_type = Error;
  using exception_type = Exception;

  basic_result() = default;

  basic_result(error_type &&error) : m_error{std::forward<error_type>(error)} {}

  bool ok() const { return !has_error(); }

  bool has_error() const { return m_error.has_value(); }

  void ensure_ok() {
    if (!ok()) {
      throw exception_type{error()};
    }
  }

  const error_type &error() const {
    if (!has_error()) {
      throw bad_access{};
    }
    return m_error.get();
  }

private:
  optional<error_type> m_error;
};

} // namespace detail

using dispatch_fn_t = std::function<void()>;

class error_info {
public:
  error_info(webview_error_t code, const std::string &message = {}) noexcept
      : m_code{code}, m_message{message} {}
  error_info() = default;

  webview_error_t code() const { return m_code; }
  const std::string &message() const { return m_message; }

private:
  webview_error_t m_code{WEBVIEW_ERROR_UNSPECIFIED};
  std::string m_message;
};

class exception : public std::exception {
public:
  exception(webview_error_t code, const std::string &message,
            std::exception_ptr cause) noexcept
      : exception{error_info{code, message}, cause} {}

  exception(webview_error_t code, const std::string &message) noexcept
      : exception{error_info{code, message}} {}

  exception(const error_info &error, std::exception_ptr cause) noexcept
      : m_error{error},
        // NOLINTNEXTLINE(bugprone-throw-keyword-missing)
        m_cause{cause} {}

  exception(const error_info &error) noexcept : m_error{error} {}

  exception() = default;

  const error_info &error() const { return m_error; }
  std::exception_ptr cause() const { return m_cause; }

  const char *what() const noexcept override {
    return m_error.message().c_str();
  }

private:
  error_info m_error{WEBVIEW_ERROR_UNSPECIFIED};
  std::exception_ptr m_cause;
};

template <typename T>
using result = detail::basic_result<T, error_info, exception>;

using noresult = detail::basic_result<void, error_info, exception>;

namespace detail {

// The library's version information.
constexpr const webview_version_info_t library_version_info{
    {WEBVIEW_VERSION_MAJOR, WEBVIEW_VERSION_MINOR, WEBVIEW_VERSION_PATCH},
    WEBVIEW_VERSION_NUMBER,
    WEBVIEW_VERSION_PRE_RELEASE,
    WEBVIEW_VERSION_BUILD_METADATA};

#if defined(_WIN32)
// Converts a narrow (UTF-8-encoded) string into a wide (UTF-16-encoded) string.
inline std::wstring widen_string(const std::string &input) {
  if (input.empty()) {
    return std::wstring();
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
  return std::wstring();
}

// Converts a wide (UTF-16-encoded) string into a narrow (UTF-8-encoded) string.
inline std::string narrow_string(const std::wstring &input) {
  struct wc_flags {
    enum TYPE : unsigned int {
      // WC_ERR_INVALID_CHARS
      err_invalid_chars = 0x00000080U
    };
  };
  if (input.empty()) {
    return std::string();
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
  return std::string();
}
#endif

inline int json_parse_c(const char *s, size_t sz, const char *key, size_t keysz,
                        const char **value, size_t *valuesz) {
  enum {
    JSON_STATE_VALUE,
    JSON_STATE_LITERAL,
    JSON_STATE_STRING,
    JSON_STATE_ESCAPE,
    JSON_STATE_UTF8
  } state = JSON_STATE_VALUE;
  const char *k = nullptr;
  int index = 1;
  int depth = 0;
  int utf8_bytes = 0;

  *value = nullptr;
  *valuesz = 0;

  if (key == nullptr) {
    index = static_cast<decltype(index)>(keysz);
    if (index < 0) {
      return -1;
    }
    keysz = 0;
  }

  for (; sz > 0; s++, sz--) {
    enum {
      JSON_ACTION_NONE,
      JSON_ACTION_START,
      JSON_ACTION_END,
      JSON_ACTION_START_STRUCT,
      JSON_ACTION_END_STRUCT
    } action = JSON_ACTION_NONE;
    auto c = static_cast<unsigned char>(*s);
    switch (state) {
    case JSON_STATE_VALUE:
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' ||
          c == ':') {
        continue;
      } else if (c == '"') {
        action = JSON_ACTION_START;
        state = JSON_STATE_STRING;
      } else if (c == '{' || c == '[') {
        action = JSON_ACTION_START_STRUCT;
      } else if (c == '}' || c == ']') {
        action = JSON_ACTION_END_STRUCT;
      } else if (c == 't' || c == 'f' || c == 'n' || c == '-' ||
                 (c >= '0' && c <= '9')) {
        action = JSON_ACTION_START;
        state = JSON_STATE_LITERAL;
      } else {
        return -1;
      }
      break;
    case JSON_STATE_LITERAL:
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' ||
          c == ']' || c == '}' || c == ':') {
        state = JSON_STATE_VALUE;
        s--;
        sz++;
        action = JSON_ACTION_END;
      } else if (c < 32 || c > 126) {
        return -1;
      } // fallthrough
    case JSON_STATE_STRING:
      if (c < 32 || (c > 126 && c < 192)) {
        return -1;
      } else if (c == '"') {
        action = JSON_ACTION_END;
        state = JSON_STATE_VALUE;
      } else if (c == '\\') {
        state = JSON_STATE_ESCAPE;
      } else if (c >= 192 && c < 224) {
        utf8_bytes = 1;
        state = JSON_STATE_UTF8;
      } else if (c >= 224 && c < 240) {
        utf8_bytes = 2;
        state = JSON_STATE_UTF8;
      } else if (c >= 240 && c < 247) {
        utf8_bytes = 3;
        state = JSON_STATE_UTF8;
      } else if (c >= 128 && c < 192) {
        return -1;
      }
      break;
    case JSON_STATE_ESCAPE:
      if (c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f' ||
          c == 'n' || c == 'r' || c == 't' || c == 'u') {
        state = JSON_STATE_STRING;
      } else {
        return -1;
      }
      break;
    case JSON_STATE_UTF8:
      if (c < 128 || c > 191) {
        return -1;
      }
      utf8_bytes--;
      if (utf8_bytes == 0) {
        state = JSON_STATE_STRING;
      }
      break;
    default:
      return -1;
    }

    if (action == JSON_ACTION_END_STRUCT) {
      depth--;
    }

    if (depth == 1) {
      if (action == JSON_ACTION_START || action == JSON_ACTION_START_STRUCT) {
        if (index == 0) {
          *value = s;
        } else if (keysz > 0 && index == 1) {
          k = s;
        } else {
          index--;
        }
      } else if (action == JSON_ACTION_END ||
                 action == JSON_ACTION_END_STRUCT) {
        if (*value != nullptr && index == 0) {
          *valuesz = static_cast<size_t>(s + 1 - *value);
          return 0;
        } else if (keysz > 0 && k != nullptr) {
          if (keysz == static_cast<size_t>(s - k - 1) &&
              memcmp(key, k + 1, keysz) == 0) {
            index = 0;
          } else {
            index = 2;
          }
          k = nullptr;
        }
      }
    }

    if (action == JSON_ACTION_START_STRUCT) {
      depth++;
    }
  }
  return -1;
}

constexpr bool is_json_special_char(char c) {
  return c == '"' || c == '\\' || c == '\b' || c == '\f' || c == '\n' ||
         c == '\r' || c == '\t';
}

constexpr bool is_ascii_control_char(char c) { return c >= 0 && c <= 0x1f; }

inline std::string json_escape(const std::string &s, bool add_quotes = true) {
  // Calculate the size of the resulting string.
  // Add space for the double quotes.
  size_t required_length = add_quotes ? 2 : 0;
  for (auto c : s) {
    if (is_json_special_char(c)) {
      // '\' and a single following character
      required_length += 2;
      continue;
    }
    if (is_ascii_control_char(c)) {
      // '\', 'u', 4 digits
      required_length += 6;
      continue;
    }
    ++required_length;
  }
  // Allocate memory for resulting string only once.
  std::string result;
  result.reserve(required_length);
  if (add_quotes) {
    result += '"';
  }
  // Copy string while escaping characters.
  for (auto c : s) {
    if (is_json_special_char(c)) {
      static constexpr char special_escape_table[256] =
          "\0\0\0\0\0\0\0\0btn\0fr\0\0"
          "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
          "\0\0\"\0\0\0\0\0\0\0\0\0\0\0\0\0"
          "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
          "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
          "\0\0\0\0\0\0\0\0\0\0\0\0\\";
      result += '\\';
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      result += special_escape_table[static_cast<unsigned char>(c)];
      continue;
    }
    if (is_ascii_control_char(c)) {
      // Escape as \u00xx
      static constexpr char hex_alphabet[]{"0123456789abcdef"};
      auto uc = static_cast<unsigned char>(c);
      auto h = (uc >> 4) & 0x0f;
      auto l = uc & 0x0f;
      result += "\\u00";
      // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
      result += hex_alphabet[h];
      result += hex_alphabet[l];
      // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
      continue;
    }
    result += c;
  }
  if (add_quotes) {
    result += '"';
  }
  // Should have calculated the exact amount of memory needed
  assert(required_length == result.size());
  return result;
}

inline int json_unescape(const char *s, size_t n, char *out) {
  int r = 0;
  if (*s++ != '"') {
    return -1;
  }
  while (n > 2) {
    char c = *s;
    if (c == '\\') {
      s++;
      n--;
      switch (*s) {
      case 'b':
        c = '\b';
        break;
      case 'f':
        c = '\f';
        break;
      case 'n':
        c = '\n';
        break;
      case 'r':
        c = '\r';
        break;
      case 't':
        c = '\t';
        break;
      case '\\':
        c = '\\';
        break;
      case '/':
        c = '/';
        break;
      case '\"':
        c = '\"';
        break;
      default: // TODO: support unicode decoding
        return -1;
      }
    }
    if (out != nullptr) {
      *out++ = c;
    }
    s++;
    n--;
    r++;
  }
  if (*s != '"') {
    return -1;
  }
  if (out != nullptr) {
    *out = '\0';
  }
  return r;
}

inline std::string json_parse(const std::string &s, const std::string &key,
                              const int index) {
  const char *value;
  size_t value_sz;
  if (key.empty()) {
    json_parse_c(s.c_str(), s.length(), nullptr, index, &value, &value_sz);
  } else {
    json_parse_c(s.c_str(), s.length(), key.c_str(), key.length(), &value,
                 &value_sz);
  }
  if (value != nullptr) {
    if (value[0] != '"') {
      return {value, value_sz};
    }
    int n = json_unescape(value, value_sz, nullptr);
    if (n > 0) {
      char *decoded = new char[n + 1];
      json_unescape(value, value_sz, decoded);
      std::string result(decoded, n);
      delete[] decoded;
      return result;
    }
  }
  return "";
}

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

template <typename WorkFn, typename ResultFn>
webview_error_t api_filter(WorkFn &&do_work, ResultFn &&put_result) noexcept {
  try {
    auto result = do_work();
    if (result.ok()) {
      put_result(result.value());
      return WEBVIEW_ERROR_OK;
    }
    return result.error().code();
  } catch (const exception &e) {
    return e.error().code();
  } catch (...) {
    return WEBVIEW_ERROR_UNSPECIFIED;
  }
}

template <typename WorkFn>
webview_error_t api_filter(WorkFn &&do_work) noexcept {
  try {
    auto result = do_work();
    if (result.ok()) {
      return WEBVIEW_ERROR_OK;
    }
    return result.error().code();
  } catch (const exception &e) {
    return e.error().code();
  } catch (...) {
    return WEBVIEW_ERROR_UNSPECIFIED;
  }
}

class user_script {
public:
  class impl;

  user_script(const std::string &code, std::unique_ptr<impl> &&impl_)
      : m_code{code}, m_impl{std::move(impl_)} {}

  user_script(const user_script &other) = delete;
  user_script &operator=(const user_script &other) = delete;
  user_script(user_script &&other) noexcept { *this = std::move(other); }

  user_script &operator=(user_script &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    m_code = std::move(other.m_code);
    m_impl = std::move(other.m_impl);
    return *this;
  }

  const std::string &get_code() const { return m_code; }

  impl &get_impl() { return *m_impl; }

  const impl &get_impl() const { return *m_impl; }

private:
  std::string m_code;
  std::unique_ptr<impl> m_impl;
};

class engine_base {
public:
  virtual ~engine_base() = default;

  noresult navigate(const std::string &url) {
    if (url.empty()) {
      return navigate_impl("about:blank");
    }
    return navigate_impl(url);
  }

  using binding_t = std::function<void(std::string, std::string, void *)>;
  class binding_ctx_t {
  public:
    binding_ctx_t(binding_t callback, void *arg)
        : m_callback(callback), m_arg(arg) {}
    void call(std::string id, std::string args) const {
      if (m_callback) {
        m_callback(id, args, m_arg);
      }
    }

  private:
    // This function is called upon execution of the bound JS function
    binding_t m_callback;
    // This user-supplied argument is passed to the callback
    void *m_arg;
  };

  using sync_binding_t = std::function<std::string(std::string)>;

  // Synchronous bind
  noresult bind(const std::string &name, sync_binding_t fn) {
    auto wrapper = [this, fn](const std::string &id, const std::string &req,
                              void * /*arg*/) { resolve(id, 0, fn(req)); };
    return bind(name, wrapper, nullptr);
  }

  // Asynchronous bind
  noresult bind(const std::string &name, binding_t fn, void *arg) {
    // NOLINTNEXTLINE(readability-container-contains): contains() requires C++20
    if (bindings.count(name) > 0) {
      return error_info{WEBVIEW_ERROR_DUPLICATE};
    }
    bindings.emplace(name, binding_ctx_t(fn, arg));
    replace_bind_script();
    // Notify that a binding was created if the init script has already
    // set things up.
    eval("if (window.__webview__) {\n\
window.__webview__.onBind(" +
         json_escape(name) + ")\n\
}");
    return {};
  }

  noresult unbind(const std::string &name) {
    auto found = bindings.find(name);
    if (found == bindings.end()) {
      return error_info{WEBVIEW_ERROR_NOT_FOUND};
    }
    bindings.erase(found);
    replace_bind_script();
    // Notify that a binding was created if the init script has already
    // set things up.
    eval("if (window.__webview__) {\n\
window.__webview__.onUnbind(" +
         json_escape(name) + ")\n\
}");
    return {};
  }

  noresult resolve(const std::string &id, int status,
                   const std::string &result) {
    // NOLINTNEXTLINE(modernize-avoid-bind): Lambda with move requires C++14
    return dispatch(std::bind(
        [id, status, this](std::string escaped_result) {
          std::string js = "window.__webview__.onReply(" + json_escape(id) +
                           ", " + std::to_string(status) + ", " +
                           escaped_result + ")";
          eval(js);
        },
        result.empty() ? "undefined" : json_escape(result)));
  }

  result<void *> window() { return window_impl(); }
  result<void *> widget() { return widget_impl(); }
  result<void *> browser_controller() { return browser_controller_impl(); }
  noresult run() { return run_impl(); }
  noresult terminate() { return terminate_impl(); }
  noresult dispatch(std::function<void()> f) { return dispatch_impl(f); }
  noresult set_title(const std::string &title) { return set_title_impl(title); }

  noresult set_size(int width, int height, webview_hint_t hints) {
    return set_size_impl(width, height, hints);
  }

  noresult set_html(const std::string &html) { return set_html_impl(html); }

  noresult init(const std::string &js) {
    add_user_script(js);
    return {};
  }

  noresult eval(const std::string &js) { return eval_impl(js); }

protected:
  virtual noresult navigate_impl(const std::string &url) = 0;
  virtual result<void *> window_impl() = 0;
  virtual result<void *> widget_impl() = 0;
  virtual result<void *> browser_controller_impl() = 0;
  virtual noresult run_impl() = 0;
  virtual noresult terminate_impl() = 0;
  virtual noresult dispatch_impl(std::function<void()> f) = 0;
  virtual noresult set_title_impl(const std::string &title) = 0;
  virtual noresult set_size_impl(int width, int height,
                                 webview_hint_t hints) = 0;
  virtual noresult set_html_impl(const std::string &html) = 0;
  virtual noresult eval_impl(const std::string &js) = 0;

  virtual user_script *add_user_script(const std::string &js) {
    return std::addressof(*m_user_scripts.emplace(m_user_scripts.end(),
                                                  add_user_script_impl(js)));
  }

  virtual user_script add_user_script_impl(const std::string &js) = 0;

  virtual void
  remove_all_user_scripts_impl(const std::list<user_script> &scripts) = 0;

  virtual bool are_user_scripts_equal_impl(const user_script &first,
                                           const user_script &second) = 0;

  virtual user_script *replace_user_script(const user_script &old_script,
                                           const std::string &new_script_code) {
    remove_all_user_scripts_impl(m_user_scripts);
    user_script *old_script_ptr{};
    for (auto &script : m_user_scripts) {
      auto is_old_script = are_user_scripts_equal_impl(script, old_script);
      script = add_user_script_impl(is_old_script ? new_script_code
                                                  : script.get_code());
      if (is_old_script) {
        old_script_ptr = std::addressof(script);
      }
    }
    return old_script_ptr;
  }

  void replace_bind_script() {
    if (m_bind_script) {
      m_bind_script = replace_user_script(*m_bind_script, create_bind_script());
    } else {
      m_bind_script = add_user_script(create_bind_script());
    }
  }

  void add_init_script(const std::string &post_fn) {
    add_user_script(create_init_script(post_fn));
  }

  std::string create_init_script(const std::string &post_fn) {
    auto js = std::string{} + "(function() {\n\
  'use strict';\n\
  function generateId() {\n\
    var crypto = window.crypto || window.msCrypto;\n\
    var bytes = new Uint8Array(16);\n\
    crypto.getRandomValues(bytes);\n\
    return Array.prototype.slice.call(bytes).map(function(n) {\n\
      return n.toString(16).padStart(2, '0');\n\
    }).join('');\n\
  }\n\
  var Webview = (function() {\n\
    var _promises = {};\n\
    function Webview_() {}\n\
    Webview_.prototype.post = function(message) {\n\
      return (" +
              post_fn + ")(message);\n\
    };\n\
    Webview_.prototype.call = function(method) {\n\
      var _id = generateId();\n\
      var _params = Array.prototype.slice.call(arguments, 1);\n\
      var promise = new Promise(function(resolve, reject) {\n\
        _promises[_id] = { resolve, reject };\n\
      });\n\
      this.post(JSON.stringify({\n\
        id: _id,\n\
        method: method,\n\
        params: _params\n\
      }));\n\
      return promise;\n\
    };\n\
    Webview_.prototype.onReply = function(id, status, result) {\n\
      var promise = _promises[id];\n\
      if (result !== undefined) {\n\
        try {\n\
          result = JSON.parse(result);\n\
        } catch {\n\
          promise.reject(new Error(\"Failed to parse binding result as JSON\"));\n\
          return;\n\
        }\n\
      }\n\
      if (status === 0) {\n\
        promise.resolve(result);\n\
      } else {\n\
        promise.reject(result);\n\
      }\n\
    };\n\
    Webview_.prototype.onBind = function(name) {\n\
      if (Object.hasOwn(window, name)) {\n\
        throw new Error('Property \"' + name + '\" already exists');\n\
      }\n\
      window[name] = (function() {\n\
        var params = [name].concat(Array.prototype.slice.call(arguments));\n\
        return Webview_.prototype.call.apply(this, params);\n\
      }).bind(this);\n\
    };\n\
    Webview_.prototype.onUnbind = function(name) {\n\
      if (!Object.hasOwn(window, name)) {\n\
        throw new Error('Property \"' + name + '\" does not exist');\n\
      }\n\
      delete window[name];\n\
    };\n\
    return Webview_;\n\
  })();\n\
  window.__webview__ = new Webview();\n\
})()";
    return js;
  }

  std::string create_bind_script() {
    std::string js_names = "[";
    bool first = true;
    for (const auto &binding : bindings) {
      if (first) {
        first = false;
      } else {
        js_names += ",";
      }
      js_names += json_escape(binding.first);
    }
    js_names += "]";

    auto js = std::string{} + "(function() {\n\
  'use strict';\n\
  var methods = " +
              js_names + ";\n\
  methods.forEach(function(name) {\n\
    window.__webview__.onBind(name);\n\
  });\n\
})()";
    return js;
  }

  virtual void on_message(const std::string &msg) {
    auto id = json_parse(msg, "id", 0);
    auto name = json_parse(msg, "method", 0);
    auto args = json_parse(msg, "params", 0);
    auto found = bindings.find(name);
    if (found == bindings.end()) {
      return;
    }
    const auto &context = found->second;
    dispatch([=] { context.call(id, args); });
  }

  virtual void on_window_created() { inc_window_count(); }

  virtual void on_window_destroyed(bool skip_termination = false) {
    if (dec_window_count() <= 0) {
      if (!skip_termination) {
        terminate();
      }
    }
  }

private:
  static std::atomic_uint &window_ref_count() {
    static std::atomic_uint ref_count{0};
    return ref_count;
  }

  static unsigned int inc_window_count() { return ++window_ref_count(); }

  static unsigned int dec_window_count() {
    auto &count = window_ref_count();
    if (count > 0) {
      return --count;
    }
    return 0;
  }

  std::map<std::string, binding_ctx_t> bindings;
  user_script *m_bind_script{};
  std::list<user_script> m_user_scripts;
};

} // namespace detail

WEBVIEW_DEPRECATED_PRIVATE
inline int json_parse_c(const char *s, size_t sz, const char *key, size_t keysz,
                        const char **value, size_t *valuesz) {
  return detail::json_parse_c(s, sz, key, keysz, value, valuesz);
}

WEBVIEW_DEPRECATED_PRIVATE
inline std::string json_escape(const std::string &s) {
  return detail::json_escape(s);
}

WEBVIEW_DEPRECATED_PRIVATE
inline int json_unescape(const char *s, size_t n, char *out) {
  return detail::json_unescape(s, n, out);
}

WEBVIEW_DEPRECATED_PRIVATE
inline std::string json_parse(const std::string &s, const std::string &key,
                              const int index) {
  return detail::json_parse(s, key, index);
}

} // namespace webview

#if defined(WEBVIEW_GTK)
#include "detail/backends/gtk_webkitgtk.hh"
#elif defined(WEBVIEW_COCOA)
#include "detail/backends/cocoa_webkit.hh"
#elif defined(WEBVIEW_EDGE)
#include "detail/backends/win32_edge.hh"
#endif

namespace webview {
using webview = browser_engine;

namespace detail {

inline webview *cast_to_webview(void *w) {
  if (!w) {
    throw exception{WEBVIEW_ERROR_INVALID_ARGUMENT,
                    "Cannot cast null pointer to webview instance"};
  }
  return static_cast<webview *>(w);
}

} // namespace detail
} // namespace webview

WEBVIEW_API webview_t webview_create(int debug, void *wnd) {
  using namespace webview::detail;
  webview::webview *w{};
  auto err = api_filter(
      [=]() -> webview::result<webview::webview *> {
        return new webview::webview{static_cast<bool>(debug), wnd};
      },
      [&](webview::webview *w_) { w = w_; });
  if (err == WEBVIEW_ERROR_OK) {
    return w;
  }
  return nullptr;
}

WEBVIEW_API webview_error_t webview_destroy(webview_t w) {
  using namespace webview::detail;
  return api_filter([=]() -> webview::noresult {
    delete cast_to_webview(w);
    return {};
  });
}

WEBVIEW_API webview_error_t webview_run(webview_t w) {
  using namespace webview::detail;
  return api_filter([=] { return cast_to_webview(w)->run(); });
}

WEBVIEW_API webview_error_t webview_terminate(webview_t w) {
  using namespace webview::detail;
  return api_filter([=] { return cast_to_webview(w)->terminate(); });
}

WEBVIEW_API webview_error_t webview_dispatch(webview_t w,
                                             void (*fn)(webview_t, void *),
                                             void *arg) {
  using namespace webview::detail;
  if (!fn) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter(
      [=] { return cast_to_webview(w)->dispatch([=]() { fn(w, arg); }); });
}

WEBVIEW_API void *webview_get_window(webview_t w) {
  using namespace webview::detail;
  void *window = nullptr;
  auto err = api_filter([=] { return cast_to_webview(w)->window(); },
                        [&](void *value) { window = value; });
  if (err == WEBVIEW_ERROR_OK) {
    return window;
  }
  return nullptr;
}

WEBVIEW_API void *webview_get_native_handle(webview_t w,
                                            webview_native_handle_kind_t kind) {
  using namespace webview::detail;
  void *handle{};
  auto err = api_filter(
      [=]() -> webview::result<void *> {
        auto *w_ = cast_to_webview(w);
        switch (kind) {
        case WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW:
          return w_->window();
        case WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET:
          return w_->widget();
        case WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER:
          return w_->browser_controller();
        default:
          return webview::error_info{WEBVIEW_ERROR_INVALID_ARGUMENT};
        }
      },
      [&](void *handle_) { handle = handle_; });
  if (err == WEBVIEW_ERROR_OK) {
    return handle;
  }
  return nullptr;
}

WEBVIEW_API webview_error_t webview_set_title(webview_t w, const char *title) {
  using namespace webview::detail;
  if (!title) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter([=] { return cast_to_webview(w)->set_title(title); });
}

WEBVIEW_API webview_error_t webview_set_size(webview_t w, int width, int height,
                                             webview_hint_t hints) {
  using namespace webview::detail;
  return api_filter(
      [=] { return cast_to_webview(w)->set_size(width, height, hints); });
}

WEBVIEW_API webview_error_t webview_navigate(webview_t w, const char *url) {
  using namespace webview::detail;
  if (!url) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter([=] { return cast_to_webview(w)->navigate(url); });
}

WEBVIEW_API webview_error_t webview_set_html(webview_t w, const char *html) {
  using namespace webview::detail;
  if (!html) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter([=] { return cast_to_webview(w)->set_html(html); });
}

WEBVIEW_API webview_error_t webview_init(webview_t w, const char *js) {
  using namespace webview::detail;
  if (!js) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter([=] { return cast_to_webview(w)->init(js); });
}

WEBVIEW_API webview_error_t webview_eval(webview_t w, const char *js) {
  using namespace webview::detail;
  if (!js) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter([=] { return cast_to_webview(w)->eval(js); });
}

WEBVIEW_API webview_error_t webview_bind(webview_t w, const char *name,
                                         void (*fn)(const char *id,
                                                    const char *req, void *arg),
                                         void *arg) {
  using namespace webview::detail;
  if (!name || !fn) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter([=] {
    return cast_to_webview(w)->bind(
        name,
        [=](const std::string &seq, const std::string &req, void *arg_) {
          fn(seq.c_str(), req.c_str(), arg_);
        },
        arg);
  });
}

WEBVIEW_API webview_error_t webview_unbind(webview_t w, const char *name) {
  using namespace webview::detail;
  if (!name) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter([=] { return cast_to_webview(w)->unbind(name); });
}

WEBVIEW_API webview_error_t webview_return(webview_t w, const char *id,
                                           int status, const char *result) {
  using namespace webview::detail;
  if (!id || !result) {
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter(
      [=] { return cast_to_webview(w)->resolve(id, status, result); });
}

WEBVIEW_API const webview_version_info_t *webview_version(void) {
  return &webview::detail::library_version_info;
}

#endif /* WEBVIEW_HEADER */
#endif /* __cplusplus */
#endif /* WEBVIEW_H */
