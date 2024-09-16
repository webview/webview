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

#ifndef WEBVIEW_BACKEND_WIN32_EDGE_HH
#define WEBVIEW_BACKEND_WIN32_EDGE_HH

//
// ====================================================================
//
// This implementation uses Win32 API to create a native window. It
// uses Edge/Chromium webview2 backend as a browser engine.
//
// ====================================================================
//

#include "../../errors.hh"
#include "../../types.hh"
#include "../utility/string.hh"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <shlobj.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <windows.h>

#include "WebView2.h"

#ifdef _MSC_VER
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "version.lib")
#endif

namespace webview {
namespace detail {

using msg_cb_t = std::function<void(const std::string)>;

// Parses a version string with 1-4 integral components, e.g. "1.2.3.4".
// Missing or invalid components default to 0, and excess components are ignored.
template <typename T>
std::array<unsigned int, 4>
parse_version(const std::basic_string<T> &version) noexcept {
  auto parse_component = [](auto sb, auto se) -> unsigned int {
    try {
      auto n = std::stol(std::basic_string<T>(sb, se));
      return n < 0 ? 0 : n;
    } catch (std::exception &) {
      return 0;
    }
  };
  auto end = version.end();
  auto sb = version.begin(); // subrange begin
  auto se = sb;              // subrange end
  unsigned int ci = 0;       // component index
  std::array<unsigned int, 4> components{};
  while (sb != end && se != end && ci < components.size()) {
    if (*se == static_cast<T>('.')) {
      components[ci++] = parse_component(sb, se);
      sb = ++se;
      continue;
    }
    ++se;
  }
  if (sb < se && ci < components.size()) {
    components[ci] = parse_component(sb, se);
  }
  return components;
}

template <typename T, std::size_t Length>
auto parse_version(const T (&version)[Length]) noexcept {
  return parse_version(std::basic_string<T>(version, Length));
}

inline std::wstring
get_file_version_string(const std::wstring &file_path) noexcept {
  DWORD dummy_handle; // Unused
  DWORD info_buffer_length =
      GetFileVersionInfoSizeW(file_path.c_str(), &dummy_handle);
  if (info_buffer_length == 0) {
    return std::wstring();
  }
  std::vector<char> info_buffer;
  info_buffer.reserve(info_buffer_length);
  if (!GetFileVersionInfoW(file_path.c_str(), 0, info_buffer_length,
                           info_buffer.data())) {
    return std::wstring();
  }
  auto sub_block = L"\\StringFileInfo\\040904B0\\ProductVersion";
  LPWSTR version = nullptr;
  unsigned int version_length = 0;
  if (!VerQueryValueW(info_buffer.data(), sub_block,
                      reinterpret_cast<LPVOID *>(&version), &version_length)) {
    return std::wstring();
  }
  if (!version || version_length == 0) {
    return std::wstring();
  }
  return std::wstring(version, version_length);
}

/**
 * A wrapper around COM library initialization. Calls CoInitializeEx in the
 * constructor and CoUninitialize in the destructor.
 *
 * @exception exception Thrown if CoInitializeEx has already been called with a
 * different concurrency model.
 */
class com_init_wrapper {
public:
  com_init_wrapper() = default;

  com_init_wrapper(DWORD dwCoInit) {
    // We can safely continue as long as COM was either successfully
    // initialized or already initialized.
    // RPC_E_CHANGED_MODE means that CoInitializeEx was already called with
    // a different concurrency model.
    switch (CoInitializeEx(nullptr, dwCoInit)) {
    case S_OK:
    case S_FALSE:
      m_initialized = true;
      break;
    case RPC_E_CHANGED_MODE:
      throw exception{
          WEBVIEW_ERROR_INVALID_STATE,
          "CoInitializeEx already called with a different concurrency model"};
    default:
      throw exception{WEBVIEW_ERROR_UNSPECIFIED,
                      "Unexpected result from CoInitializeEx"};
    }
  }

  ~com_init_wrapper() {
    if (m_initialized) {
      CoUninitialize();
      m_initialized = false;
    }
  }

  com_init_wrapper(const com_init_wrapper &other) = delete;
  com_init_wrapper &operator=(const com_init_wrapper &other) = delete;
  com_init_wrapper(com_init_wrapper &&other) { *this = std::move(other); }

  com_init_wrapper &operator=(com_init_wrapper &&other) {
    if (this == &other) {
      return *this;
    }
    m_initialized = std::exchange(other.m_initialized, false);
    return *this;
  }

private:
  bool m_initialized = false;
};

namespace ntdll_symbols {
using RtlGetVersion_t =
    unsigned int /*NTSTATUS*/ (WINAPI *)(RTL_OSVERSIONINFOW *);

constexpr auto RtlGetVersion = library_symbol<RtlGetVersion_t>("RtlGetVersion");
} // namespace ntdll_symbols

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

namespace shcore_symbols {
typedef enum { PROCESS_PER_MONITOR_DPI_AWARE = 2 } PROCESS_DPI_AWARENESS;
using SetProcessDpiAwareness_t = HRESULT(WINAPI *)(PROCESS_DPI_AWARENESS);

constexpr auto SetProcessDpiAwareness =
    library_symbol<SetProcessDpiAwareness_t>("SetProcessDpiAwareness");
} // namespace shcore_symbols

class reg_key {
public:
  explicit reg_key(HKEY root_key, const wchar_t *sub_key, DWORD options,
                   REGSAM sam_desired) {
    HKEY handle;
    auto status =
        RegOpenKeyExW(root_key, sub_key, options, sam_desired, &handle);
    if (status == ERROR_SUCCESS) {
      m_handle = handle;
    }
  }

  explicit reg_key(HKEY root_key, const std::wstring &sub_key, DWORD options,
                   REGSAM sam_desired)
      : reg_key(root_key, sub_key.c_str(), options, sam_desired) {}

  virtual ~reg_key() {
    if (m_handle) {
      RegCloseKey(m_handle);
      m_handle = nullptr;
    }
  }

  reg_key(const reg_key &other) = delete;
  reg_key &operator=(const reg_key &other) = delete;
  reg_key(reg_key &&other) = delete;
  reg_key &operator=(reg_key &&other) = delete;

  bool is_open() const { return !!m_handle; }
  bool get_handle() const { return m_handle; }

  template <typename Container>
  void query_bytes(const wchar_t *name, Container &result) const {
    DWORD buf_length = 0;
    // Get the size of the data in bytes.
    auto status = RegQueryValueExW(m_handle, name, nullptr, nullptr, nullptr,
                                   &buf_length);
    if (status != ERROR_SUCCESS && status != ERROR_MORE_DATA) {
      result.resize(0);
      return;
    }
    // Read the data.
    result.resize(buf_length / sizeof(typename Container::value_type));
    auto *buf = reinterpret_cast<LPBYTE>(&result[0]);
    status =
        RegQueryValueExW(m_handle, name, nullptr, nullptr, buf, &buf_length);
    if (status != ERROR_SUCCESS) {
      result.resize(0);
      return;
    }
  }

  std::wstring query_string(const wchar_t *name) const {
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

  unsigned int query_uint(const wchar_t *name,
                          unsigned int default_value) const {
    std::vector<char> data;
    query_bytes(name, data);
    if (data.size() < sizeof(DWORD)) {
      return default_value;
    }
    return static_cast<unsigned int>(*reinterpret_cast<DWORD *>(data.data()));
  }

private:
  HKEY m_handle = nullptr;
};

// Compare the specified version against the OS version.
// Returns less than 0 if the OS version is less.
// Returns 0 if the versions are equal.
// Returns greater than 0 if the specified version is greater.
inline int compare_os_version(unsigned int major, unsigned int minor,
                              unsigned int build) {
  // Use RtlGetVersion both to bypass potential issues related to
  // VerifyVersionInfo and manifests, and because both GetVersion and
  // GetVersionEx are deprecated.
  auto ntdll = native_library(L"ntdll.dll");
  if (auto fn = ntdll.get(ntdll_symbols::RtlGetVersion)) {
    RTL_OSVERSIONINFOW vi{};
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (fn(&vi) != 0) {
      return false;
    }
    if (vi.dwMajorVersion == major) {
      if (vi.dwMinorVersion == minor) {
        return static_cast<int>(vi.dwBuildNumber) - static_cast<int>(build);
      }
      return static_cast<int>(vi.dwMinorVersion) - static_cast<int>(minor);
    }
    return static_cast<int>(vi.dwMajorVersion) - static_cast<int>(major);
  }
  return false;
}

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
  constexpr const int default_dpi = 96; // USER_DEFAULT_SCREEN_DPI
  return default_dpi;
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
  auto scaled_width = scale_value_for_dpi(width, from_dpi, to_dpi);
  auto scaled_height = scale_value_for_dpi(height, from_dpi, to_dpi);
  return {scaled_width, scaled_height};
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

// Enable built-in WebView2Loader implementation by default.
#ifndef WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL
#define WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL 1
#endif

// Link WebView2Loader.dll explicitly by default only if the built-in
// implementation is enabled.
#ifndef WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK
#define WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL
#endif

// Explicit linking of WebView2Loader.dll should be used along with
// the built-in implementation.
#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1 &&                                    \
    WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK != 1
#undef WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK
#error Please set WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK=1.
#endif

#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1
// Gets the last component of a Windows native file path.
// For example, if the path is "C:\a\b" then the result is "b".
template <typename T>
std::basic_string<T>
get_last_native_path_component(const std::basic_string<T> &path) {
  auto pos = path.find_last_of(static_cast<T>('\\'));
  if (pos != std::basic_string<T>::npos) {
    return path.substr(pos + 1);
  }
  return std::basic_string<T>();
}
#endif /* WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL */

template <typename T> struct cast_info_t {
  using type = T;
  IID iid;
};

namespace mswebview2 {
static constexpr IID
    IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler{
        0x6C4819F3,
        0xC9B7,
        0x4260,
        {0x81, 0x27, 0xC9, 0xF5, 0xBD, 0xE7, 0xF6, 0x8C}};
static constexpr IID
    IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler{
        0x4E8A3389,
        0xC9D8,
        0x4BD2,
        {0xB6, 0xB5, 0x12, 0x4F, 0xEE, 0x6C, 0xC1, 0x4D}};
static constexpr IID IID_ICoreWebView2PermissionRequestedEventHandler{
    0x15E1C6A3,
    0xC72A,
    0x4DF3,
    {0x91, 0xD7, 0xD0, 0x97, 0xFB, 0xEC, 0x6B, 0xFD}};
static constexpr IID IID_ICoreWebView2WebMessageReceivedEventHandler{
    0x57213F19,
    0x00E6,
    0x49FA,
    {0x8E, 0x07, 0x89, 0x8E, 0xA0, 0x1E, 0xCB, 0xD2}};
static constexpr IID
    IID_ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler{
        0xB99369F3,
        0x9B11,
        0x47B5,
        {0xBC, 0x6F, 0x8E, 0x78, 0x95, 0xFC, 0xEA, 0x17}};

#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1
enum class webview2_runtime_type { installed = 0, embedded = 1 };

namespace webview2_symbols {
using CreateWebViewEnvironmentWithOptionsInternal_t =
    HRESULT(STDMETHODCALLTYPE *)(
        bool, webview2_runtime_type, PCWSTR, IUnknown *,
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler *);
using DllCanUnloadNow_t = HRESULT(STDMETHODCALLTYPE *)();

static constexpr auto CreateWebViewEnvironmentWithOptionsInternal =
    library_symbol<CreateWebViewEnvironmentWithOptionsInternal_t>(
        "CreateWebViewEnvironmentWithOptionsInternal");
static constexpr auto DllCanUnloadNow =
    library_symbol<DllCanUnloadNow_t>("DllCanUnloadNow");
} // namespace webview2_symbols
#endif /* WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL */

#if WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK == 1
namespace webview2_symbols {
using CreateCoreWebView2EnvironmentWithOptions_t = HRESULT(STDMETHODCALLTYPE *)(
    PCWSTR, PCWSTR, ICoreWebView2EnvironmentOptions *,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler *);
using GetAvailableCoreWebView2BrowserVersionString_t =
    HRESULT(STDMETHODCALLTYPE *)(PCWSTR, LPWSTR *);

static constexpr auto CreateCoreWebView2EnvironmentWithOptions =
    library_symbol<CreateCoreWebView2EnvironmentWithOptions_t>(
        "CreateCoreWebView2EnvironmentWithOptions");
static constexpr auto GetAvailableCoreWebView2BrowserVersionString =
    library_symbol<GetAvailableCoreWebView2BrowserVersionString_t>(
        "GetAvailableCoreWebView2BrowserVersionString");
} // namespace webview2_symbols
#endif /* WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK */

class loader {
public:
  HRESULT create_environment_with_options(
      PCWSTR browser_dir, PCWSTR user_data_dir,
      ICoreWebView2EnvironmentOptions *env_options,
      ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
          *created_handler) const {
#if WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK == 1
    if (m_lib.is_loaded()) {
      if (auto fn = m_lib.get(
              webview2_symbols::CreateCoreWebView2EnvironmentWithOptions)) {
        return fn(browser_dir, user_data_dir, env_options, created_handler);
      }
    }
#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1
    return create_environment_with_options_impl(browser_dir, user_data_dir,
                                                env_options, created_handler);
#else
    return S_FALSE;
#endif
#else
    return ::CreateCoreWebView2EnvironmentWithOptions(
        browser_dir, user_data_dir, env_options, created_handler);
#endif /* WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK */
  }

  HRESULT
  get_available_browser_version_string(PCWSTR browser_dir,
                                       LPWSTR *version) const {
#if WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK == 1
    if (m_lib.is_loaded()) {
      if (auto fn = m_lib.get(
              webview2_symbols::GetAvailableCoreWebView2BrowserVersionString)) {
        return fn(browser_dir, version);
      }
    }
#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1
    return get_available_browser_version_string_impl(browser_dir, version);
#else
    return S_FALSE;
#endif
#else
    return ::GetAvailableCoreWebView2BrowserVersionString(browser_dir, version);
#endif /* WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK */
  }

private:
#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1
  struct client_info_t {
    bool found = false;
    std::wstring dll_path;
    std::wstring version;
    webview2_runtime_type runtime_type;
  };

  HRESULT create_environment_with_options_impl(
      PCWSTR browser_dir, PCWSTR user_data_dir,
      ICoreWebView2EnvironmentOptions *env_options,
      ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
          *created_handler) const {
    auto found_client = find_available_client(browser_dir);
    if (!found_client.found) {
      return -1;
    }
    auto client_dll = native_library(found_client.dll_path.c_str());
    if (auto fn = client_dll.get(
            webview2_symbols::CreateWebViewEnvironmentWithOptionsInternal)) {
      return fn(true, found_client.runtime_type, user_data_dir, env_options,
                created_handler);
    }
    if (auto fn = client_dll.get(webview2_symbols::DllCanUnloadNow)) {
      if (!fn()) {
        client_dll.detach();
      }
    }
    return ERROR_SUCCESS;
  }

  HRESULT
  get_available_browser_version_string_impl(PCWSTR browser_dir,
                                            LPWSTR *version) const {
    if (!version) {
      return -1;
    }
    auto found_client = find_available_client(browser_dir);
    if (!found_client.found) {
      return -1;
    }
    auto info_length_bytes =
        found_client.version.size() * sizeof(found_client.version[0]);
    auto info = static_cast<LPWSTR>(CoTaskMemAlloc(info_length_bytes));
    if (!info) {
      return -1;
    }
    CopyMemory(info, found_client.version.c_str(), info_length_bytes);
    *version = info;
    return 0;
  }

  client_info_t find_available_client(PCWSTR browser_dir) const {
    if (browser_dir) {
      return find_embedded_client(api_version, browser_dir);
    }
    auto found_client =
        find_installed_client(api_version, true, default_release_channel_guid);
    if (!found_client.found) {
      found_client = find_installed_client(api_version, false,
                                           default_release_channel_guid);
    }
    return found_client;
  }

  std::wstring make_client_dll_path(const std::wstring &dir) const {
    auto dll_path = dir;
    if (!dll_path.empty()) {
      auto last_char = dir[dir.size() - 1];
      if (last_char != L'\\' && last_char != L'/') {
        dll_path += L'\\';
      }
    }
    dll_path += L"EBWebView\\";
#if defined(_M_X64) || defined(__x86_64__)
    dll_path += L"x64";
#elif defined(_M_IX86) || defined(__i386__)
    dll_path += L"x86";
#elif defined(_M_ARM64) || defined(__aarch64__)
    dll_path += L"arm64";
#else
#error WebView2 integration for this platform is not yet supported.
#endif
    dll_path += L"\\EmbeddedBrowserWebView.dll";
    return dll_path;
  }

  client_info_t
  find_installed_client(unsigned int min_api_version, bool system,
                        const std::wstring &release_channel) const {
    std::wstring sub_key = client_state_reg_sub_key;
    sub_key += release_channel;
    auto root_key = system ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
    reg_key key(root_key, sub_key, 0, KEY_READ | KEY_WOW64_32KEY);
    if (!key.is_open()) {
      return {};
    }
    auto ebwebview_value = key.query_string(L"EBWebView");

    auto client_version_string =
        get_last_native_path_component(ebwebview_value);
    auto client_version = parse_version(client_version_string);
    if (client_version[2] < min_api_version) {
      // Our API version is greater than the runtime API version.
      return {};
    }

    auto client_dll_path = make_client_dll_path(ebwebview_value);
    return {true, client_dll_path, client_version_string,
            webview2_runtime_type::installed};
  }

  client_info_t find_embedded_client(unsigned int min_api_version,
                                     const std::wstring &dir) const {
    auto client_dll_path = make_client_dll_path(dir);

    auto client_version_string = get_file_version_string(client_dll_path);
    auto client_version = parse_version(client_version_string);
    if (client_version[2] < min_api_version) {
      // Our API version is greater than the runtime API version.
      return {};
    }

    return {true, client_dll_path, client_version_string,
            webview2_runtime_type::embedded};
  }

  // The minimum WebView2 API version we need regardless of the SDK release
  // actually used. The number comes from the SDK release version,
  // e.g. 1.0.1150.38. To be safe the SDK should have a number that is greater
  // than or equal to this number. The Edge browser webview client must
  // have a number greater than or equal to this number.
  static constexpr unsigned int api_version = 1150;

  static constexpr auto client_state_reg_sub_key =
      L"SOFTWARE\\Microsoft\\EdgeUpdate\\ClientState\\";

  // GUID for the stable release channel.
  static constexpr auto stable_release_guid =
      L"{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}";

  static constexpr auto default_release_channel_guid = stable_release_guid;
#endif /* WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL */

#if WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK == 1
  native_library m_lib{L"WebView2Loader.dll"};
#endif
};

namespace cast_info {
static constexpr auto controller_completed =
    cast_info_t<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>{
        IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler};

static constexpr auto environment_completed =
    cast_info_t<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>{
        IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler};

static constexpr auto message_received =
    cast_info_t<ICoreWebView2WebMessageReceivedEventHandler>{
        IID_ICoreWebView2WebMessageReceivedEventHandler};

static constexpr auto permission_requested =
    cast_info_t<ICoreWebView2PermissionRequestedEventHandler>{
        IID_ICoreWebView2PermissionRequestedEventHandler};

static constexpr auto add_script_to_execute_on_document_created_completed =
    cast_info_t<
        ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>{
        IID_ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler};
} // namespace cast_info
} // namespace mswebview2

// Checks whether the specified IID equals the IID of the specified type and
// if so casts the "this" pointer to T and returns it. Returns nullptr on
// mismatching IIDs.
// If ppv is specified then the pointer will also be assigned to *ppv.
template <typename From, typename To>
To *cast_if_equal_iid(From *from, REFIID riid, const cast_info_t<To> &info,
                      LPVOID *ppv = nullptr) noexcept {
  To *ptr = nullptr;
  if (IsEqualIID(riid, info.iid)) {
    ptr = static_cast<To *>(from);
    ptr->AddRef();
  }
  if (ppv) {
    *ppv = ptr;
  }
  return ptr;
}

class webview2_com_handler
    : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler,
      public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
      public ICoreWebView2WebMessageReceivedEventHandler,
      public ICoreWebView2PermissionRequestedEventHandler {
  using webview2_com_handler_cb_t =
      std::function<void(ICoreWebView2Controller *, ICoreWebView2 *webview)>;

public:
  webview2_com_handler(HWND hwnd, msg_cb_t msgCb, webview2_com_handler_cb_t cb)
      : m_window(hwnd), m_msgCb(msgCb), m_cb(cb) {}

  virtual ~webview2_com_handler() = default;
  webview2_com_handler(const webview2_com_handler &other) = delete;
  webview2_com_handler &operator=(const webview2_com_handler &other) = delete;
  webview2_com_handler(webview2_com_handler &&other) = delete;
  webview2_com_handler &operator=(webview2_com_handler &&other) = delete;

  ULONG STDMETHODCALLTYPE AddRef() { return ++m_ref_count; }
  ULONG STDMETHODCALLTYPE Release() {
    if (m_ref_count > 1) {
      return --m_ref_count;
    }
    delete this;
    return 0;
  }
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID *ppv) {
    using namespace mswebview2::cast_info;

    if (!ppv) {
      return E_POINTER;
    }

    // All of the COM interfaces we implement should be added here regardless
    // of whether they are required.
    // This is just to be on the safe side in case the WebView2 Runtime ever
    // requests a pointer to an interface we implement.
    // The WebView2 Runtime must at the very least be able to get a pointer to
    // ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler when we use
    // our custom WebView2 loader implementation, and observations have shown
    // that it is the only interface requested in this case. None have been
    // observed to be requested when using the official WebView2 loader.

    if (cast_if_equal_iid(this, riid, controller_completed, ppv) ||
        cast_if_equal_iid(this, riid, environment_completed, ppv) ||
        cast_if_equal_iid(this, riid, message_received, ppv) ||
        cast_if_equal_iid(this, riid, permission_requested, ppv)) {
      return S_OK;
    }

    return E_NOINTERFACE;
  }
  HRESULT STDMETHODCALLTYPE Invoke(HRESULT res, ICoreWebView2Environment *env) {
    if (SUCCEEDED(res)) {
      res = env->CreateCoreWebView2Controller(m_window, this);
      if (SUCCEEDED(res)) {
        return S_OK;
      }
    }
    try_create_environment();
    return S_OK;
  }
  HRESULT STDMETHODCALLTYPE Invoke(HRESULT res,
                                   ICoreWebView2Controller *controller) {
    if (FAILED(res)) {
      // See try_create_environment() regarding
      // HRESULT_FROM_WIN32(ERROR_INVALID_STATE).
      // The result is E_ABORT if the parent window has been destroyed already.
      switch (res) {
      case HRESULT_FROM_WIN32(ERROR_INVALID_STATE):
      case E_ABORT:
        return S_OK;
      }
      try_create_environment();
      return S_OK;
    }

    ICoreWebView2 *webview;
    ::EventRegistrationToken token;
    controller->get_CoreWebView2(&webview);
    webview->add_WebMessageReceived(this, &token);
    webview->add_PermissionRequested(this, &token);

    m_cb(controller, webview);
    return S_OK;
  }
  HRESULT STDMETHODCALLTYPE
  Invoke(ICoreWebView2 * /*sender*/,
         ICoreWebView2WebMessageReceivedEventArgs *args) {
    LPWSTR message{};
    auto res = args->TryGetWebMessageAsString(&message);
    if (SUCCEEDED(res)) {
      m_msgCb(narrow_string(message));
    }

    CoTaskMemFree(message);
    return S_OK;
  }
  HRESULT STDMETHODCALLTYPE
  Invoke(ICoreWebView2 * /*sender*/,
         ICoreWebView2PermissionRequestedEventArgs *args) {
    COREWEBVIEW2_PERMISSION_KIND kind;
    args->get_PermissionKind(&kind);
    if (kind == COREWEBVIEW2_PERMISSION_KIND_CLIPBOARD_READ) {
      args->put_State(COREWEBVIEW2_PERMISSION_STATE_ALLOW);
    }
    return S_OK;
  }

  // Set the function that will perform the initiating logic for creating
  // the WebView2 environment.
  void set_attempt_handler(std::function<HRESULT()> attempt_handler) noexcept {
    m_attempt_handler = attempt_handler;
  }

  // Retry creating a WebView2 environment.
  // The initiating logic for creating the environment is defined by the
  // caller of set_attempt_handler().
  void try_create_environment() noexcept {
    // WebView creation fails with HRESULT_FROM_WIN32(ERROR_INVALID_STATE) if
    // a running instance using the same user data folder exists, and the
    // Environment objects have different EnvironmentOptions.
    // Source: https://docs.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/icorewebview2environment?view=webview2-1.0.1150.38
    if (m_attempts < m_max_attempts) {
      ++m_attempts;
      auto res = m_attempt_handler();
      if (SUCCEEDED(res)) {
        return;
      }
      // Not entirely sure if this error code only applies to
      // CreateCoreWebView2Controller so we check here as well.
      if (res == HRESULT_FROM_WIN32(ERROR_INVALID_STATE)) {
        return;
      }
      try_create_environment();
      return;
    }
    // Give up.
    m_cb(nullptr, nullptr);
  }

private:
  HWND m_window;
  msg_cb_t m_msgCb;
  webview2_com_handler_cb_t m_cb;
  std::atomic<ULONG> m_ref_count{1};
  std::function<HRESULT()> m_attempt_handler;
  unsigned int m_max_attempts = 5;
  unsigned int m_attempts = 0;
};

class webview2_user_script_added_handler
    : public ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler {
public:
  using callback_fn = std::function<void(HRESULT errorCode, LPCWSTR id)>;

  webview2_user_script_added_handler(callback_fn cb) : m_cb{cb} {}

  virtual ~webview2_user_script_added_handler() = default;
  webview2_user_script_added_handler(
      const webview2_user_script_added_handler &other) = delete;
  webview2_user_script_added_handler &
  operator=(const webview2_user_script_added_handler &other) = delete;
  webview2_user_script_added_handler(
      webview2_user_script_added_handler &&other) = delete;
  webview2_user_script_added_handler &
  operator=(webview2_user_script_added_handler &&other) = delete;

  ULONG STDMETHODCALLTYPE AddRef() { return ++m_ref_count; }
  ULONG STDMETHODCALLTYPE Release() {
    if (m_ref_count > 1) {
      return --m_ref_count;
    }
    delete this;
    return 0;
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID *ppv) {
    using namespace mswebview2::cast_info;

    if (!ppv) {
      return E_POINTER;
    }

    if (cast_if_equal_iid(this, riid,
                          add_script_to_execute_on_document_created_completed,
                          ppv)) {
      return S_OK;
    }

    return E_NOINTERFACE;
  }

  HRESULT STDMETHODCALLTYPE Invoke(HRESULT res, LPCWSTR id) {
    m_cb(res, id);
    return S_OK;
  }

private:
  callback_fn m_cb;
  std::atomic<ULONG> m_ref_count{1};
};

class user_script::impl {
public:
  impl(const std::wstring &id, const std::wstring &code)
      : m_id{id}, m_code{code} {}

  impl(const impl &) = delete;
  impl &operator=(const impl &) = delete;
  impl(impl &&) = delete;
  impl &operator=(impl &&) = delete;

  const std::wstring &get_id() const { return m_id; }
  const std::wstring &get_code() const { return m_code; }

private:
  std::wstring m_id;
  std::wstring m_code;
};

class win32_edge_engine : public engine_base {
public:
  win32_edge_engine(bool debug, void *window) : m_owns_window{!window} {
    if (!is_webview2_available()) {
      throw exception{WEBVIEW_ERROR_MISSING_DEPENDENCY,
                      "WebView2 is unavailable"};
    }

    HINSTANCE hInstance = GetModuleHandle(nullptr);

    if (m_owns_window) {
      m_com_init = {COINIT_APARTMENTTHREADED};
      enable_dpi_awareness();

      HICON icon = (HICON)LoadImage(
          hInstance, IDI_APPLICATION, IMAGE_ICON, GetSystemMetrics(SM_CXICON),
          GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);

      // Create a top-level window.
      WNDCLASSEXW wc;
      ZeroMemory(&wc, sizeof(WNDCLASSEX));
      wc.cbSize = sizeof(WNDCLASSEX);
      wc.hInstance = hInstance;
      wc.lpszClassName = L"webview";
      wc.hIcon = icon;
      wc.lpfnWndProc = (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp,
                                     LPARAM lp) -> LRESULT {
        win32_edge_engine *w{};

        if (msg == WM_NCCREATE) {
          auto *lpcs{reinterpret_cast<LPCREATESTRUCT>(lp)};
          w = static_cast<win32_edge_engine *>(lpcs->lpCreateParams);
          w->m_window = hwnd;
          SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
          enable_non_client_dpi_scaling_if_needed(hwnd);
          apply_window_theme(hwnd);
        } else {
          w = reinterpret_cast<win32_edge_engine *>(
              GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (!w) {
          return DefWindowProcW(hwnd, msg, wp, lp);
        }

        switch (msg) {
        case WM_SIZE:
          w->resize_widget();
          break;
        case WM_CLOSE:
          DestroyWindow(hwnd);
          break;
        case WM_DESTROY:
          w->m_window = nullptr;
          SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
          w->on_window_destroyed();
          break;
        case WM_GETMINMAXINFO: {
          auto lpmmi = (LPMINMAXINFO)lp;
          if (w->m_maxsz.x > 0 && w->m_maxsz.y > 0) {
            lpmmi->ptMaxSize = w->m_maxsz;
            lpmmi->ptMaxTrackSize = w->m_maxsz;
          }
          if (w->m_minsz.x > 0 && w->m_minsz.y > 0) {
            lpmmi->ptMinTrackSize = w->m_minsz;
          }
        } break;
        case 0x02E4 /*WM_GETDPISCALEDSIZE*/: {
          auto dpi = static_cast<int>(wp);
          auto *size{reinterpret_cast<SIZE *>(lp)};
          *size = w->get_scaled_size(w->m_dpi, dpi);
          return TRUE;
        }
        case 0x02E0 /*WM_DPICHANGED*/: {
          // Windows 10: The size we get here is exactly what we supplied to WM_GETDPISCALEDSIZE.
          // Windows 11: The size we get here is NOT what we supplied to WM_GETDPISCALEDSIZE.
          // Due to this difference, don't use the suggested bounds.
          auto dpi = static_cast<int>(HIWORD(wp));
          w->on_dpi_changed(dpi);
          break;
        }
        case WM_SETTINGCHANGE: {
          auto *area = reinterpret_cast<const wchar_t *>(lp);
          if (area) {
            w->on_system_setting_change(area);
          }
          break;
        }
        case WM_ACTIVATE:
          if (LOWORD(wp) != WA_INACTIVE) {
            w->focus_webview();
          }
          break;
        default:
          return DefWindowProcW(hwnd, msg, wp, lp);
        }
        return 0;
      });
      RegisterClassExW(&wc);

      CreateWindowW(L"webview", L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                    CW_USEDEFAULT, 0, 0, nullptr, nullptr, hInstance, this);
      if (!m_window) {
        throw exception{WEBVIEW_ERROR_INVALID_STATE, "Window is null"};
      }
      on_window_created();

      m_dpi = get_window_dpi(m_window);
      constexpr const int initial_width = 640;
      constexpr const int initial_height = 480;
      set_size(initial_width, initial_height, WEBVIEW_HINT_NONE);
    } else {
      m_window = IsWindow(static_cast<HWND>(window))
                     ? static_cast<HWND>(window)
                     : *(static_cast<HWND *>(window));
      m_dpi = get_window_dpi(m_window);
    }

    // Create a window that WebView2 will be embedded into.
    WNDCLASSEXW widget_wc{};
    widget_wc.cbSize = sizeof(WNDCLASSEX);
    widget_wc.hInstance = hInstance;
    widget_wc.lpszClassName = L"webview_widget";
    widget_wc.lpfnWndProc = (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp,
                                          LPARAM lp) -> LRESULT {
      win32_edge_engine *w{};

      if (msg == WM_NCCREATE) {
        auto *lpcs{reinterpret_cast<LPCREATESTRUCT>(lp)};
        w = static_cast<win32_edge_engine *>(lpcs->lpCreateParams);
        w->m_widget = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
      } else {
        w = reinterpret_cast<win32_edge_engine *>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
      }

      if (!w) {
        return DefWindowProcW(hwnd, msg, wp, lp);
      }

      switch (msg) {
      case WM_SIZE:
        w->resize_webview();
        break;
      case WM_DESTROY:
        w->m_widget = nullptr;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        break;
      default:
        return DefWindowProcW(hwnd, msg, wp, lp);
      }
      return 0;
    });
    RegisterClassExW(&widget_wc);
    CreateWindowExW(WS_EX_CONTROLPARENT, L"webview_widget", nullptr, WS_CHILD,
                    0, 0, 0, 0, m_window, nullptr, hInstance, this);
    if (!m_widget) {
      throw exception{WEBVIEW_ERROR_INVALID_STATE, "Widget window is null"};
    }

    // Create a message-only window for internal messaging.
    WNDCLASSEXW message_wc{};
    message_wc.cbSize = sizeof(WNDCLASSEX);
    message_wc.hInstance = hInstance;
    message_wc.lpszClassName = L"webview_message";
    message_wc.lpfnWndProc = (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp,
                                           LPARAM lp) -> LRESULT {
      win32_edge_engine *w{};

      if (msg == WM_NCCREATE) {
        auto *lpcs{reinterpret_cast<LPCREATESTRUCT>(lp)};
        w = static_cast<win32_edge_engine *>(lpcs->lpCreateParams);
        w->m_message_window = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
      } else {
        w = reinterpret_cast<win32_edge_engine *>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
      }

      if (!w) {
        return DefWindowProcW(hwnd, msg, wp, lp);
      }

      switch (msg) {
      case WM_APP:
        if (auto f = (dispatch_fn_t *)(lp)) {
          (*f)();
          delete f;
        }
        break;
      case WM_DESTROY:
        w->m_message_window = nullptr;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        break;
      default:
        return DefWindowProcW(hwnd, msg, wp, lp);
      }
      return 0;
    });
    RegisterClassExW(&message_wc);
    CreateWindowExW(0, L"webview_message", nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE,
                    nullptr, hInstance, this);
    if (!m_message_window) {
      throw exception{WEBVIEW_ERROR_INVALID_STATE, "Message window is null"};
    }

    if (m_owns_window) {
      ShowWindow(m_window, SW_SHOW);
      UpdateWindow(m_window);
      SetFocus(m_window);
    }

    auto cb =
        std::bind(&win32_edge_engine::on_message, this, std::placeholders::_1);

    embed(m_widget, debug, cb).ensure_ok();
  }

  virtual ~win32_edge_engine() {
    if (m_com_handler) {
      m_com_handler->Release();
      m_com_handler = nullptr;
    }
    if (m_webview) {
      m_webview->Release();
      m_webview = nullptr;
    }
    if (m_controller) {
      m_controller->Release();
      m_controller = nullptr;
    }
    // Replace wndproc to avoid callbacks and other bad things during
    // destruction.
    auto wndproc = reinterpret_cast<LONG_PTR>(
        +[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
          return DefWindowProcW(hwnd, msg, wp, lp);
        });
    if (m_widget) {
      SetWindowLongPtrW(m_widget, GWLP_WNDPROC, wndproc);
    }
    if (m_window && m_owns_window) {
      SetWindowLongPtrW(m_window, GWLP_WNDPROC, wndproc);
    }
    if (m_widget) {
      DestroyWindow(m_widget);
      m_widget = nullptr;
    }
    if (m_window) {
      if (m_owns_window) {
        DestroyWindow(m_window);
        on_window_destroyed(true);
      }
      m_window = nullptr;
    }
    if (m_owns_window) {
      // Not strictly needed for windows to close immediately but aligns
      // behavior across backends.
      deplete_run_loop_event_queue();
    }
    // We need the message window in order to deplete the event queue.
    if (m_message_window) {
      SetWindowLongPtrW(m_message_window, GWLP_WNDPROC, wndproc);
      DestroyWindow(m_message_window);
      m_message_window = nullptr;
    }
  }

  win32_edge_engine(const win32_edge_engine &other) = delete;
  win32_edge_engine &operator=(const win32_edge_engine &other) = delete;
  win32_edge_engine(win32_edge_engine &&other) = delete;
  win32_edge_engine &operator=(win32_edge_engine &&other) = delete;

protected:
  noresult run_impl() override {
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
    return {};
  }
  result<void *> window_impl() override {
    if (m_window) {
      return m_window;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }
  result<void *> widget_impl() override {
    if (m_widget) {
      return m_widget;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }
  result<void *> browser_controller_impl() override {
    if (m_controller) {
      return m_controller;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }
  noresult terminate_impl() override {
    PostQuitMessage(0);
    return {};
  }
  noresult dispatch_impl(dispatch_fn_t f) override {
    PostMessageW(m_message_window, WM_APP, 0, (LPARAM) new dispatch_fn_t(f));
    return {};
  }

  noresult set_title_impl(const std::string &title) override {
    SetWindowTextW(m_window, widen_string(title).c_str());
    return {};
  }

  noresult set_size_impl(int width, int height, webview_hint_t hints) override {
    auto style = GetWindowLong(m_window, GWL_STYLE);
    if (hints == WEBVIEW_HINT_FIXED) {
      style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    } else {
      style |= (WS_THICKFRAME | WS_MAXIMIZEBOX);
    }
    SetWindowLong(m_window, GWL_STYLE, style);

    if (hints == WEBVIEW_HINT_MAX) {
      m_maxsz.x = width;
      m_maxsz.y = height;
    } else if (hints == WEBVIEW_HINT_MIN) {
      m_minsz.x = width;
      m_minsz.y = height;
    } else {
      auto dpi = get_window_dpi(m_window);
      m_dpi = dpi;
      auto scaled_size =
          scale_size(width, height, get_default_window_dpi(), dpi);
      auto frame_size =
          make_window_frame_size(m_window, scaled_size.cx, scaled_size.cy, dpi);
      SetWindowPos(m_window, nullptr, 0, 0, frame_size.cx, frame_size.cy,
                   SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE |
                       SWP_FRAMECHANGED);
    }
    return {};
  }

  noresult navigate_impl(const std::string &url) override {
    auto wurl = widen_string(url);
    m_webview->Navigate(wurl.c_str());
    return {};
  }

  noresult eval_impl(const std::string &js) override {
    // TODO: Skip if no content has begun loading yet. Can't check with
    //       ICoreWebView2::get_Source because it returns "about:blank".
    auto wjs = widen_string(js);
    m_webview->ExecuteScript(wjs.c_str(), nullptr);
    return {};
  }

  noresult set_html_impl(const std::string &html) override {
    m_webview->NavigateToString(widen_string(html).c_str());
    return {};
  }

  user_script add_user_script_impl(const std::string &js) override {
    auto wjs = widen_string(js);
    std::wstring script_id;
    bool done{};
    webview2_user_script_added_handler handler{[&](HRESULT res, LPCWSTR id) {
      if (SUCCEEDED(res)) {
        script_id = id;
      }
      done = true;
    }};
    auto res =
        m_webview->AddScriptToExecuteOnDocumentCreated(wjs.c_str(), &handler);
    if (SUCCEEDED(res)) {
      // Sadly we need to pump the even loop in order to get the script ID.
      while (!done) {
        deplete_run_loop_event_queue();
      }
    }
    // TODO: There's a non-zero chance that we didn't get the script ID.
    //       We need to convey the error somehow.
    return user_script{js, std::unique_ptr<user_script::impl>{
                               new user_script::impl{script_id, wjs}}};
  }

  void
  remove_all_user_scripts_impl(const std::list<user_script> &scripts) override {
    for (const auto &script : scripts) {
      const auto &id = script.get_impl().get_id();
      m_webview->RemoveScriptToExecuteOnDocumentCreated(id.c_str());
    }
  }

  bool are_user_scripts_equal_impl(const user_script &first,
                                   const user_script &second) override {
    const auto &first_id = first.get_impl().get_id();
    const auto &second_id = second.get_impl().get_id();
    return first_id == second_id;
  }

private:
  noresult embed(HWND wnd, bool debug, msg_cb_t cb) {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    flag.test_and_set();

    wchar_t currentExePath[MAX_PATH];
    GetModuleFileNameW(nullptr, currentExePath, MAX_PATH);
    wchar_t *currentExeName = PathFindFileNameW(currentExePath);

    wchar_t dataPath[MAX_PATH];
    if (!SUCCEEDED(
            SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, dataPath))) {
      return error_info{WEBVIEW_ERROR_UNSPECIFIED, "SHGetFolderPathW failed"};
    }
    wchar_t userDataFolder[MAX_PATH];
    PathCombineW(userDataFolder, dataPath, currentExeName);

    m_com_handler = new webview2_com_handler(
        wnd, cb,
        [&](ICoreWebView2Controller *controller, ICoreWebView2 *webview) {
          if (!controller || !webview) {
            flag.clear();
            return;
          }
          controller->AddRef();
          webview->AddRef();
          m_controller = controller;
          m_webview = webview;
          flag.clear();
        });

    m_com_handler->set_attempt_handler([&] {
      return m_webview2_loader.create_environment_with_options(
          nullptr, userDataFolder, nullptr, m_com_handler);
    });
    m_com_handler->try_create_environment();

    // Pump the message loop until WebView2 has finished initialization.
    bool got_quit_msg = false;
    MSG msg;
    while (flag.test_and_set() && GetMessageW(&msg, nullptr, 0, 0) >= 0) {
      if (msg.message == WM_QUIT) {
        got_quit_msg = true;
        break;
      }
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
    if (got_quit_msg) {
      return error_info{WEBVIEW_ERROR_CANCELED};
    }
    if (!m_controller || !m_webview) {
      return error_info{WEBVIEW_ERROR_INVALID_STATE};
    }
    ICoreWebView2Settings *settings = nullptr;
    auto res = m_webview->get_Settings(&settings);
    if (res != S_OK) {
      return error_info{WEBVIEW_ERROR_UNSPECIFIED, "get_Settings failed"};
    }
    res = settings->put_AreDevToolsEnabled(debug ? TRUE : FALSE);
    if (res != S_OK) {
      return error_info{WEBVIEW_ERROR_UNSPECIFIED,
                        "put_AreDevToolsEnabled failed"};
    }
    res = settings->put_IsStatusBarEnabled(FALSE);
    if (res != S_OK) {
      return error_info{WEBVIEW_ERROR_UNSPECIFIED,
                        "put_IsStatusBarEnabled failed"};
    }
    add_init_script("function(message) {\n\
  return window.chrome.webview.postMessage(message);\n\
}");
    resize_webview();
    m_controller->put_IsVisible(TRUE);
    ShowWindow(m_widget, SW_SHOW);
    UpdateWindow(m_widget);
    if (m_owns_window) {
      focus_webview();
    }
    return {};
  }

  void resize_widget() {
    if (m_widget) {
      RECT r{};
      if (GetClientRect(GetParent(m_widget), &r)) {
        MoveWindow(m_widget, r.left, r.top, r.right - r.left, r.bottom - r.top,
                   TRUE);
      }
    }
  }

  void resize_webview() {
    if (m_widget && m_controller) {
      RECT bounds{};
      if (GetClientRect(m_widget, &bounds)) {
        m_controller->put_Bounds(bounds);
      }
    }
  }

  void focus_webview() {
    if (m_controller) {
      m_controller->MoveFocus(COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC);
    }
  }

  bool is_webview2_available() const noexcept {
    LPWSTR version_info = nullptr;
    auto res = m_webview2_loader.get_available_browser_version_string(
        nullptr, &version_info);
    // The result will be equal to HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
    // if the WebView2 runtime is not installed.
    auto ok = SUCCEEDED(res) && version_info;
    if (version_info) {
      CoTaskMemFree(version_info);
    }
    return ok;
  }

  void on_dpi_changed(int dpi) {
    auto scaled_size = get_scaled_size(m_dpi, dpi);
    auto frame_size =
        make_window_frame_size(m_window, scaled_size.cx, scaled_size.cy, dpi);
    SetWindowPos(m_window, nullptr, 0, 0, frame_size.cx, frame_size.cy,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_FRAMECHANGED);
    m_dpi = dpi;
  }

  SIZE get_size() const {
    RECT bounds;
    GetClientRect(m_window, &bounds);
    auto width = bounds.right - bounds.left;
    auto height = bounds.bottom - bounds.top;
    return {width, height};
  }

  SIZE get_scaled_size(int from_dpi, int to_dpi) const {
    auto size = get_size();
    return scale_size(size.cx, size.cy, from_dpi, to_dpi);
  }

  void on_system_setting_change(const wchar_t *area) {
    // Detect light/dark mode change in system.
    if (lstrcmpW(area, L"ImmersiveColorSet") == 0) {
      apply_window_theme(m_window);
    }
  }

  // Blocks while depleting the run loop of events.
  void deplete_run_loop_event_queue() {
    bool done{};
    dispatch([&] { done = true; });
    while (!done) {
      MSG msg;
      if (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
      }
    }
  }

  // The app is expected to call CoInitializeEx before
  // CreateCoreWebView2EnvironmentWithOptions.
  // Source: https://docs.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/webview2-idl#createcorewebview2environmentwithoptions
  com_init_wrapper m_com_init;
  HWND m_window = nullptr;
  HWND m_widget = nullptr;
  HWND m_message_window = nullptr;
  POINT m_minsz = POINT{0, 0};
  POINT m_maxsz = POINT{0, 0};
  DWORD m_main_thread = GetCurrentThreadId();
  ICoreWebView2 *m_webview = nullptr;
  ICoreWebView2Controller *m_controller = nullptr;
  webview2_com_handler *m_com_handler = nullptr;
  mswebview2::loader m_webview2_loader;
  int m_dpi{};
  bool m_owns_window{};
};

} // namespace detail

using browser_engine = detail::win32_edge_engine;

} // namespace webview

#endif /* WEBVIEW_BACKEND_WIN32_EDGE_H */
