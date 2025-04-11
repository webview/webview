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

#ifndef WEBVIEW_BACKENDS_WIN32_EDGE_HH
#define WEBVIEW_BACKENDS_WIN32_EDGE_HH

#include "../../macros.h"

#include "../../http.h"
#include <map>

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#   if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

//
// ====================================================================
//
// This implementation uses Win32 API to create a native window. It
// uses Edge/Chromium webview2 backend as a browser engine.
//
// ====================================================================
//

#      include "../engine_base.h"
#      include "../platform/windows/com_init_wrapper.h"
#      include "../platform/windows/webview2/loader.h"
#      include "../user_script.h"

#      include <Windows.h>
#      include <atomic>
#      include <functional>
#      include <list>

#      ifndef WIN32_LEAN_AND_MEAN
#         define WIN32_LEAN_AND_MEAN
#      endif

#      include <objbase.h>
#      include <ShlObj.h>
#      include <Shlwapi.h>
#      include <wrl.h>
#      include <intsafe.h>
#      include <windef.h>
#      include <wrl/client.h>
#      include <wrl/implements.h>
#      include <WebView2EnvironmentOptions.h>

#      ifdef _MSC_VER
#         pragma comment(lib, "ole32.lib")
#         pragma comment(lib, "shell32.lib")
#         pragma comment(lib, "shlwapi.lib")
#         pragma comment(lib, "user32.lib")
#         pragma comment(lib, "version.lib")
#      endif

namespace webview {
class user_script::impl {
public:
   impl(const std::wstring& id, const std::wstring& code);

   impl(const impl&)            = delete;
   impl& operator=(const impl&) = delete;
   impl(impl&&)                 = delete;
   impl& operator=(impl&&)      = delete;

   const std::wstring& get_id() const { return id_; }
   const std::wstring& get_code() const { return code_; }

private:
   std::wstring id_;
   std::wstring code_;
};

namespace detail {

using msg_cb_t = std::function<void(const std::string)>;

class webview2_com_handler
   : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
   , public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
   , public ICoreWebView2WebMessageReceivedEventHandler
   , public ICoreWebView2PermissionRequestedEventHandler {
   using webview2_com_handler_cb_t =
      std::function<void(ICoreWebView2Controller*, ICoreWebView2* webview)>;

public:
   webview2_com_handler(HWND hwnd, msg_cb_t msgCb, webview2_com_handler_cb_t cb);

   virtual ~webview2_com_handler()                                    = default;
   webview2_com_handler(const webview2_com_handler& other)            = delete;
   webview2_com_handler& operator=(const webview2_com_handler& other) = delete;
   webview2_com_handler(webview2_com_handler&& other)                 = delete;
   webview2_com_handler& operator=(webview2_com_handler&& other)      = delete;

   ULONG STDMETHODCALLTYPE   AddRef();
   ULONG STDMETHODCALLTYPE   Release();
   HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv);
   HRESULT STDMETHODCALLTYPE Invoke(HRESULT res, ICoreWebView2Environment* env);
   HRESULT STDMETHODCALLTYPE Invoke(HRESULT res, ICoreWebView2Controller* controller);
   HRESULT STDMETHODCALLTYPE
   Invoke(ICoreWebView2* /*sender*/, ICoreWebView2WebMessageReceivedEventArgs* args);
   HRESULT STDMETHODCALLTYPE
   Invoke(ICoreWebView2* /*sender*/, ICoreWebView2PermissionRequestedEventArgs* args);

   // Set the function that will perform the initiating logic for creating
   // the WebView2 environment.
   void set_attempt_handler(std::function<HRESULT()>&& attempt_handler) noexcept;

   // Retry creating a WebView2 environment.
   // The initiating logic for creating the environment is defined by the
   // caller of set_attempt_handler().
   void try_create_environment() noexcept;

private:
   HWND                      window_;
   msg_cb_t                  msgCb_;
   webview2_com_handler_cb_t cb_;
   std::atomic<ULONG>        ref_count_{1};
   std::function<HRESULT()>  attempt_handler_;
   unsigned int              max_attempts_ = 5;
   unsigned int              attempts_     = 0;
};

class webview2_user_script_added_handler
   : public ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler {
public:
   using callback_fn = std::function<void(HRESULT errorCode, LPCWSTR id)>;

   explicit webview2_user_script_added_handler(callback_fn const& cb);

   virtual ~webview2_user_script_added_handler()                                                  = default;
   webview2_user_script_added_handler(const webview2_user_script_added_handler& other)            = delete;
   webview2_user_script_added_handler& operator=(const webview2_user_script_added_handler& other) = delete;
   webview2_user_script_added_handler(webview2_user_script_added_handler&& other)                 = delete;
   webview2_user_script_added_handler& operator=(webview2_user_script_added_handler&& other)      = delete;

   ULONG STDMETHODCALLTYPE AddRef();
   ULONG STDMETHODCALLTYPE Release();

   HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv);
   HRESULT STDMETHODCALLTYPE Invoke(HRESULT res, LPCWSTR id);

private:
   callback_fn        cb_;
   std::atomic<ULONG> ref_count_{1};
};

class win32_edge_engine final : public Webview {
public:
   static auto make_options() {
      return Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
   }

   static void set_schemes_option(
      std::vector<std::string> const&                         schemes,
      Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions> options
   );

   win32_edge_engine(
      bool                                                    debug,
      HWND                                                    window,
      Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions> options       = nullptr,
      DWORD                                                   style         = 0,
      std::string_view                                        user_data_dir = ""
   );

   ~win32_edge_engine() final;

   win32_edge_engine(const win32_edge_engine& other)            = delete;
   win32_edge_engine& operator=(const win32_edge_engine& other) = delete;
   win32_edge_engine(win32_edge_engine&& other)                 = delete;
   win32_edge_engine& operator=(win32_edge_engine&& other)      = delete;

   HWND                     window() const;
   HWND                     widget() const;
   ICoreWebView2Controller* browser_controller() const;

   void register_url_handler(std::string const& filter, url_handler_t&& handler) final;
   void install_ressource_handler() final;

   void set_title(std::string_view title) final;
   void set_size(int width, int height, Hint hints) final;

   void run() final;
   void terminate() final;

   void eval(std::string_view js) final;
   void set_html(std::string_view html) final;

private:
   void dispatch(std::function<void()>&& f) final;

   void navigate_impl(std::string_view url) final;

   void open_dev_tools() final;

   std::multimap<std::wstring, url_handler_t, std::less<>> handlers_;

   //---------------------------------------------------------------------------------------------------------------------
   Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse>
   make_response(http::response_t const& responseData, HRESULT& result);

   //---------------------------------------------------------------------------------------------------------------------
   http::request_t make_request(std::string const& uri, COREWEBVIEW2_WEB_RESOURCE_CONTEXT, ICoreWebView2WebResourceRequest* webViewRequest);

   user_script add_user_script_impl(std::string_view js) final;
   void        remove_all_user_scripts(std::list<user_script> const& scripts) final;
   bool        are_user_scripts_equal(user_script const& first, user_script const& second) final;

private:
   void
   embed(HWND wnd, bool debug, msg_cb_t cb, Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions> options, std::string_view user_data_dir);

   void resize_widget();
   void resize_webview();
   void focus_webview();

   bool is_webview2_available() const noexcept;

   void on_dpi_changed(int dpi);

   SIZE get_size() const;
   SIZE get_scaled_size(int from_dpi, int to_dpi) const;

   void on_system_setting_change(const wchar_t* area);

   // Blocks while depleting the run loop of events.
   void deplete_run_loop_event_queue();

   // The app is expected to call CoInitializeEx before
   // CreateCoreWebView2EnvironmentWithOptions.
   // Source: https://docs.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/webview2-idl#createcorewebview2environmentwithoptions
   com_init_wrapper         com_init_;
   HWND                     window_         = nullptr;
   HWND                     widget_         = nullptr;
   HWND                     message_window_ = nullptr;
   POINT                    minsz_          = POINT{0, 0};
   POINT                    maxsz_          = POINT{0, 0};
   DWORD                    main_thread_    = GetCurrentThreadId();
   ICoreWebView2*           webview_        = nullptr;
   ICoreWebView2Controller* controller_     = nullptr;
   webview2_com_handler*    com_handler_    = nullptr;
   mswebview2::loader       webview2_loader_;
   int                      dpi_{};
   bool                     owns_window_{};
};

}  // namespace detail

using browser_engine = detail::win32_edge_engine;

}  // namespace webview

#   endif  // defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
#endif     // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif     // WEBVIEW_BACKENDS_WIN32_EDGE_HH
