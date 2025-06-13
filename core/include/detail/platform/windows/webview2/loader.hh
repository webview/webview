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
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
#ifdef _MSC_VER
#pragma comment(lib, "ole32.lib")
#endif
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
#include "WebView2.h" // amalgamate(skip)
#include "detail/platform/windows/native_library.hh"
#include "detail/platform/windows/reg_key.hh"
#include "detail/platform/windows/types.hh"
#include "detail/platform/windows/version.hh"
#include <objbase.h>
#include <string>

namespace webview {
namespace detail {
namespace platform {
namespace windows {

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

struct mswebview2 {
private:
  static const IID &
  IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler();
  static const IID &
  IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler();
  static const IID &IID_ICoreWebView2PermissionRequestedEventHandler();
  static const IID &IID_ICoreWebView2WebMessageReceivedEventHandler();
  static const IID &
  IID_ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler();

  using create_internal_t =
      library_symbol<CreateWebViewEnvironmentWithOptionsInternal_t>;
  using dll_can_unload_t = library_symbol<DllCanUnloadNow_t>;
  using create_with_options_t =
      library_symbol<CreateCoreWebView2EnvironmentWithOptions_t>;
  using get_version_string_t =
      library_symbol<GetAvailableCoreWebView2BrowserVersionString_t>;

  struct symbols {

#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1
    static const create_internal_t &
    CreateWebViewEnvironmentWithOptionsInternal();
    static const dll_can_unload_t &DllCanUnloadNow();
#endif // WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL

#if WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK == 1
    static const create_with_options_t &
    CreateCoreWebView2EnvironmentWithOptions();
    static const get_version_string_t &
    GetAvailableCoreWebView2BrowserVersionString();
#endif // WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK
  };

public:
  class loader {
  public:
    HRESULT create_environment_with_options(
        PCWSTR browser_dir, PCWSTR user_data_dir,
        ICoreWebView2EnvironmentOptions *env_options,
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
            *created_handler) const;

    HRESULT
    get_available_browser_version_string(PCWSTR browser_dir,
                                         LPWSTR *version) const;

  private:
#if WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL == 1
    struct client_info_t {
      bool found{};
      std::wstring dll_path;
      std::wstring version;
      runtime_type runtime_type{};

      client_info_t() = default;

      client_info_t(bool found, std::wstring dll_path, std::wstring version,
                    enum runtime_type runtime_type)
          : found{found},
            dll_path{std::move(dll_path)},
            version{std::move(version)},
            runtime_type{runtime_type} {}
    };

    HRESULT create_environment_with_options_impl(
        PCWSTR browser_dir, PCWSTR user_data_dir,
        ICoreWebView2EnvironmentOptions *env_options,
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
            *created_handler) const;

    HRESULT
    get_available_browser_version_string_impl(PCWSTR browser_dir,
                                              LPWSTR *version) const;

    client_info_t find_available_client(PCWSTR browser_dir) const;

    std::wstring make_client_dll_path(const std::wstring &dir) const;

    client_info_t
    find_installed_client(unsigned int min_api_version, bool system,
                          const std::wstring &release_channel) const;

    client_info_t find_embedded_client(unsigned int min_api_version,
                                       const std::wstring &dir) const;

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

  struct cast_info {
    static cast_info_t<
        ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>
    controller_completed() {
      return {IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler()};
    }

    static cast_info_t<
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>
    environment_completed() {
      return {IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler()};
    }

    static cast_info_t<ICoreWebView2WebMessageReceivedEventHandler>
    message_received() {
      return {IID_ICoreWebView2WebMessageReceivedEventHandler()};
    }

    static cast_info_t<ICoreWebView2PermissionRequestedEventHandler>
    permission_requested() {
      return {IID_ICoreWebView2PermissionRequestedEventHandler()};
    }

    static cast_info_t<
        ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>
    add_script_to_execute_on_document_created_completed() {
      return {
          IID_ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler()};
    }
  };
};

} // namespace windows
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_BACKENDS_WEBVIEW2_LOADER_HH