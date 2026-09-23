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

#ifndef WEBVIEW_BACKENDS_WEBVIEW2_LOADER_HH
#define WEBVIEW_BACKENDS_WEBVIEW2_LOADER_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

#include "../../../native_library.hh"
#include "../iid.hh"
#include "../reg_key.hh"
#include "../version.hh"

#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <objbase.h>

#include "WebView2.h" // amalgamate(skip)

#ifdef _MSC_VER
#pragma comment(lib, "ole32.lib")
#endif

#if defined(__MINGW32__)
// Let MinGW‑w64 emulate the Microsoft‑specific __uuidof/__CRT_UUID_DECL
// mechanism that the Windows SDK uses to expose
// COM interface identifiers (IIDs) as C++ constants.
#if defined(__cplusplus) && (USE___UUIDOF == 0)
#undef __CRT_UUID_DECL
#define __CRT_UUID_DECL(type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)       \
  extern "C++" {                                                               \
  template <> inline const GUID &__mingw_uuidof<type>() {                      \
    static const IID __uuid_inst = {                                           \
        l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}};                          \
    return __uuid_inst;                                                        \
  }                                                                            \
  template <> inline const GUID &__mingw_uuidof<type *>() {                    \
    return __mingw_uuidof<type>();                                             \
  }                                                                            \
  }
#else
#define __CRT_UUID_DECL(type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)
#endif
// __CRT_UUID_DECL(ICoreWebView2_2,  0x9E8F0CF8, 0xE670, 0x4B5E, 0xB2, 0xBC, 0x73, 0xE0, 0x61, 0xE3, 0x18, 0x4C)
// __CRT_UUID_DECL(ICoreWebView2_3,  0xA0D6DF20, 0x3B92, 0x416D, 0xAA, 0x0C, 0x43, 0x7A, 0x9C, 0x72, 0x78, 0x57)
// __CRT_UUID_DECL(ICoreWebView2_4,  0x20d02d59, 0x6df2, 0x42dc, 0xbd, 0x06, 0xf9, 0x8a, 0x69, 0x4b, 0x13, 0x02)
// __CRT_UUID_DECL(ICoreWebView2_5,  0xbedb11b8, 0xd63c, 0x11eb, 0xb8, 0xbc, 0x02, 0x42, 0xac, 0x13, 0x00, 0x03)
// __CRT_UUID_DECL(ICoreWebView2_6,  0x499aadac, 0xd92c, 0x4589, 0x8a, 0x75, 0x11, 0x1b, 0xfc, 0x16, 0x77, 0x95)
// __CRT_UUID_DECL(ICoreWebView2_7,  0x79c24d83, 0x09a3, 0x45ae, 0x94, 0x18, 0x48, 0x7f, 0x32, 0xa5, 0x87, 0x40)
// __CRT_UUID_DECL(ICoreWebView2_8,  0xE9632730, 0x6E1E, 0x43AB, 0xB7, 0xB8, 0x7B, 0x2C, 0x9E, 0x62, 0xE0, 0x94)
// __CRT_UUID_DECL(ICoreWebView2_9,  0x4d7b2eab, 0x9fdc, 0x468d, 0xb9, 0x98, 0xa9, 0x26, 0x0b, 0x5e, 0xd6, 0x51)
__CRT_UUID_DECL(ICoreWebView2_10, 0xb1690564, 0x6f5a, 0x4983, 0x8e, 0x48, 0x31,
                0xd1, 0x14, 0x3f, 0xec, 0xdb)
// __CRT_UUID_DECL(ICoreWebView2_11, 0x0be78e56, 0xc193, 0x4051, 0xb9, 0x43, 0x23, 0xb4, 0x60, 0xc0, 0x8b, 0xdb)
// __CRT_UUID_DECL(ICoreWebView2_12, 0x35D69927, 0xBCFA, 0x4566, 0x93, 0x49, 0x6B, 0x3E, 0x0D, 0x15, 0x4C, 0xAC)
// __CRT_UUID_DECL(ICoreWebView2_13, 0xF75F09A8, 0x667E, 0x4983, 0x88, 0xD6, 0xC8, 0x77, 0x3F, 0x31, 0x5E, 0x84)
// __CRT_UUID_DECL(ICoreWebView2_14, 0x6DAA4F10, 0x4A90, 0x4753, 0x88, 0x98, 0x77, 0xC5, 0xDF, 0x53, 0x41, 0x65)
// __CRT_UUID_DECL(ICoreWebView2_15, 0x517B2D1D, 0x7DAE, 0x4A66, 0xA4, 0xF4, 0x10, 0x35, 0x2F, 0xFB, 0x95, 0x18)
// __CRT_UUID_DECL(ICoreWebView2_16, 0x0EB34DC9, 0x9F91, 0x41E1, 0x86, 0x39, 0x95, 0xCD, 0x59, 0x43, 0x90, 0x6B)
// __CRT_UUID_DECL(ICoreWebView2_17, 0x702E75D4, 0xFD44, 0x434D, 0x9D, 0x70, 0x1A, 0x68, 0xA6, 0xB1, 0x19, 0x2A)
// __CRT_UUID_DECL(ICoreWebView2_18, 0x7A626017, 0x28BE, 0x49B2, 0xB8, 0x65, 0x3B, 0xA2, 0xB3, 0x52, 0x2D, 0x90)
// __CRT_UUID_DECL(ICoreWebView2_19, 0x6921F954, 0x79B0, 0x437F, 0xA9, 0x97, 0xC8, 0x58, 0x11, 0x89, 0x7C, 0x68)
// __CRT_UUID_DECL(ICoreWebView2_20, 0xb4bc1926, 0x7305, 0x11ee, 0xb9, 0x62, 0x02, 0x42, 0xac, 0x12, 0x00, 0x02)
#endif

namespace webview {
namespace detail {

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
#endif // WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL

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
#endif // WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL

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
#endif // WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK

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
#endif // WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK
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
#endif // WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK
  }

private:
#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1
  struct client_info_t {
    bool found{};
    std::wstring dll_path;
    std::wstring version;
    webview2_runtime_type runtime_type{};

    client_info_t() = default;

    client_info_t(bool found, std::wstring dll_path, std::wstring version,
                  webview2_runtime_type runtime_type)
        : found{found},
          dll_path{std::move(dll_path)},
          version{std::move(version)},
          runtime_type{runtime_type} {}
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
    auto client_dll = native_library(found_client.dll_path);
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
    return {true, std::move(client_dll_path), std::move(client_version_string),
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

    return {true, std::move(client_dll_path), std::move(client_version_string),
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
#endif // WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL

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
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_BACKENDS_WEBVIEW2_LOADER_HH
