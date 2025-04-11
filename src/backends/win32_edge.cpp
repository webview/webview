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

#include "detail/backends/win32_edge.h"
#include "detail/platform/windows/dpi.h"
#include "detail/platform/windows/theme.h"

#include <format>
#include <regex>

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)

namespace webview {
user_script::impl::impl(const std::wstring& id, const std::wstring& code)
   : id_{id}
   , code_{code} {}

namespace detail {

webview2_com_handler::webview2_com_handler(HWND hwnd, msg_cb_t msgCb, webview2_com_handler_cb_t cb)
   : window_(hwnd)
   , msgCb_(msgCb)
   , cb_(cb) {}

ULONG STDMETHODCALLTYPE
webview2_com_handler::AddRef() { return ++ref_count_; }

ULONG STDMETHODCALLTYPE
webview2_com_handler::Release() {
   if (ref_count_ > 1) {
      return --ref_count_;
   }
   delete this;
   return 0;
}

HRESULT STDMETHODCALLTYPE
webview2_com_handler::QueryInterface(REFIID riid, LPVOID* ppv) {
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

   if (cast_if_equal_iid(this, riid, controller_completed, ppv) || cast_if_equal_iid(this, riid, environment_completed, ppv) || cast_if_equal_iid(this, riid, message_received, ppv) || cast_if_equal_iid(this, riid, permission_requested, ppv)) {
      return S_OK;
   }

   return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE
webview2_com_handler::Invoke(HRESULT res, ICoreWebView2Environment* env) {
   if (SUCCEEDED(res)) {
      res = env->CreateCoreWebView2Controller(window_, this);
      if (SUCCEEDED(res)) {
         return S_OK;
      }
   }
   try_create_environment();
   return S_OK;
}

HRESULT STDMETHODCALLTYPE
webview2_com_handler::Invoke(HRESULT res, ICoreWebView2Controller* controller) {
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

   ICoreWebView2*           webview;
   ::EventRegistrationToken token;
   controller->get_CoreWebView2(&webview);
   webview->add_WebMessageReceived(this, &token);
   webview->add_PermissionRequested(this, &token);

   cb_(controller, webview);
   return S_OK;
}

HRESULT STDMETHODCALLTYPE
webview2_com_handler::Invoke(ICoreWebView2* /*sender*/, ICoreWebView2WebMessageReceivedEventArgs* args) {
   LPWSTR message{};
   auto   res = args->TryGetWebMessageAsString(&message);
   if (SUCCEEDED(res)) {
      msgCb_(narrow_string(message));
   }

   CoTaskMemFree(message);
   return S_OK;
}

HRESULT STDMETHODCALLTYPE
webview2_com_handler::Invoke(ICoreWebView2* /*sender*/, ICoreWebView2PermissionRequestedEventArgs* args) {
   COREWEBVIEW2_PERMISSION_KIND kind;
   args->get_PermissionKind(&kind);
   if (kind == COREWEBVIEW2_PERMISSION_KIND_CLIPBOARD_READ) {
      args->put_State(COREWEBVIEW2_PERMISSION_STATE_ALLOW);
   }
   return S_OK;
}

// Set the function that will perform the initiating logic for creating
// the WebView2 environment.
void
webview2_com_handler::set_attempt_handler(std::function<HRESULT()>&& attempt_handler) noexcept {
   attempt_handler_ = std::move(attempt_handler);
}

// Retry creating a WebView2 environment.
// The initiating logic for creating the environment is defined by the
// caller of set_attempt_handler().
void
webview2_com_handler::try_create_environment() noexcept {
   // WebView creation fails with HRESULT_FROM_WIN32(ERROR_INVALID_STATE) if
   // a running instance using the same user data folder exists, and the
   // Environment objects have different EnvironmentOptions.
   // Source: https://docs.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/icorewebview2environment?view=webview2-1.0.1150.38
   if (attempts_ < max_attempts_) {
      ++attempts_;
      auto res = attempt_handler_();
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
   cb_(nullptr, nullptr);
}

webview2_user_script_added_handler::webview2_user_script_added_handler(callback_fn const& cb)
   : cb_{cb} {}

ULONG STDMETHODCALLTYPE
webview2_user_script_added_handler::AddRef() {
   return ++ref_count_;
}

ULONG STDMETHODCALLTYPE
webview2_user_script_added_handler::Release() {
   if (ref_count_ > 1) {
      return --ref_count_;
   }
   delete this;
   return 0;
}

HRESULT STDMETHODCALLTYPE
webview2_user_script_added_handler::QueryInterface(REFIID riid, LPVOID* ppv) {
   using namespace mswebview2::cast_info;

   if (!ppv) {
      return E_POINTER;
   }

   if (cast_if_equal_iid(this, riid, add_script_to_execute_on_document_created_completed, ppv)) {
      return S_OK;
   }

   return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE
webview2_user_script_added_handler::Invoke(HRESULT res, LPCWSTR id) {
   cb_(res, id);
   return S_OK;
}

void
win32_edge_engine::set_schemes_option(
   std::vector<std::string> const&                         schemes,
   Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions> options
) {
   Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions4> options4;

   auto const result = options.As(&options4);
   if (result != S_OK) {
      throw exception{
         error_t::WEBVIEW_ERROR_UNSPECIFIED,
         std::format("Could not set options: {}", std::to_string(result))
      };
   }

   std::vector<Microsoft::WRL::ComPtr<ICoreWebView2CustomSchemeRegistration>>
                                                       webSchemes{};
   std::vector<ICoreWebView2CustomSchemeRegistration*> data;

   for (auto const& scheme : schemes) {
      auto const wscheme = widen_string(scheme);
      auto const schemeReg =
         Microsoft::WRL::Make<CoreWebView2CustomSchemeRegistration>(
            wscheme.c_str()
         );

      std::array<wchar_t const*, 1> origins{L"*"};
      schemeReg->SetAllowedOrigins(static_cast<UINT32>(origins.size()), origins.data());
      schemeReg->put_TreatAsSecure(true);
      schemeReg->put_HasAuthorityComponent(true);

      webSchemes.emplace_back(schemeReg);
      data.emplace_back(schemeReg.Get());
   }

   auto ptr = data.data();
   options4->SetCustomSchemeRegistrations(static_cast<UINT32>(data.size()), ptr);
}

win32_edge_engine::win32_edge_engine(
   bool                                                    debug,
   HWND                                                    window,
   Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions> options,
   DWORD                                                   style,
   std::string_view                                        user_data_dir
)
   : owns_window_{!window} {
   if (!is_webview2_available()) {
      throw exception{error_t::WEBVIEW_ERROR_MISSING_DEPENDENCY, "WebView2 is unavailable"};
   }

   HINSTANCE hInstance = GetModuleHandle(nullptr);

   if (owns_window_) {
      com_init_ = com_init_wrapper{COINIT_APARTMENTTHREADED};
      enable_dpi_awareness();

      auto icon = static_cast<HICON>(LoadImage(
         hInstance, IDI_APPLICATION, IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR
      ));

      // Create a top-level window.
      WNDCLASSEXW wc;
      ZeroMemory(&wc, sizeof(WNDCLASSEX));
      wc.cbSize        = sizeof(WNDCLASSEX);
      wc.hInstance     = hInstance;
      wc.lpszClassName = L"webview";
      wc.hIcon         = icon;
      wc.lpfnWndProc   = (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
         win32_edge_engine* w{};

         if (msg == WM_NCCREATE) {
            auto* lpcs{reinterpret_cast<LPCREATESTRUCT>(lp)};
            w          = static_cast<win32_edge_engine*>(lpcs->lpCreateParams);
            w->window_ = hwnd;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
            enable_non_client_dpi_scaling_if_needed(hwnd);
            apply_window_theme(hwnd);
         } else {
            w = reinterpret_cast<win32_edge_engine*>(
               GetWindowLongPtrW(hwnd, GWLP_USERDATA)
            );
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
               w->window_ = nullptr;
               SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
               w->on_window_destroyed();
               break;
            case WM_GETMINMAXINFO: {
               auto lpmmi = (LPMINMAXINFO)lp;
               if (w->maxsz_.x > 0 && w->maxsz_.y > 0) {
                  lpmmi->ptMaxSize      = w->maxsz_;
                  lpmmi->ptMaxTrackSize = w->maxsz_;
               }
               if (w->minsz_.x > 0 && w->minsz_.y > 0) {
                  lpmmi->ptMinTrackSize = w->minsz_;
               }
            } break;
            case 0x02E4 /*WM_GETDPISCALEDSIZE*/: {
               auto  dpi = static_cast<int>(wp);
               auto* size{reinterpret_cast<SIZE*>(lp)};
               *size = w->get_scaled_size(w->dpi_, dpi);
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
               auto* area = reinterpret_cast<const wchar_t*>(lp);
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

      CreateWindowW(L"webview", L"", style ? style : WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, nullptr, nullptr, hInstance, this);
      if (!window_) {
         throw exception{error_t::WEBVIEW_ERROR_INVALID_STATE, "Window is null"};
      }
      on_window_created();

      dpi_                               = get_window_dpi(window_);
      constexpr const int initial_width  = 640;
      constexpr const int initial_height = 480;
      set_size(initial_width, initial_height, Hint::NONE);
   } else {
      window_ = window;
      dpi_    = get_window_dpi(window_);
   }

   // Create a window that WebView2 will be embedded into.
   WNDCLASSEXW widget_wc{};
   widget_wc.cbSize        = sizeof(WNDCLASSEX);
   widget_wc.hInstance     = hInstance;
   widget_wc.lpszClassName = L"webview_widget";
   widget_wc.lpfnWndProc   = (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
      win32_edge_engine* w{};

      if (msg == WM_NCCREATE) {
         auto* lpcs{reinterpret_cast<LPCREATESTRUCT>(lp)};
         w          = static_cast<win32_edge_engine*>(lpcs->lpCreateParams);
         w->widget_ = hwnd;
         SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
      } else {
         w = reinterpret_cast<win32_edge_engine*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA)
         );
      }

      if (!w) {
         return DefWindowProcW(hwnd, msg, wp, lp);
      }

      switch (msg) {
         case WM_SIZE:
            w->resize_webview();
            break;
         case WM_DESTROY:
            w->widget_ = nullptr;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            break;
         default:
            return DefWindowProcW(hwnd, msg, wp, lp);
      }
      return 0;
   });
   RegisterClassExW(&widget_wc);
   CreateWindowExW(WS_EX_CONTROLPARENT, L"webview_widget", nullptr, WS_CHILD, 0, 0, 0, 0, window_, nullptr, hInstance, this);
   if (!widget_) {
      throw exception{error_t::WEBVIEW_ERROR_INVALID_STATE, "Widget window is null"};
   }

   // Create a message-only window for internal messaging.
   WNDCLASSEXW message_wc{};
   message_wc.cbSize        = sizeof(WNDCLASSEX);
   message_wc.hInstance     = hInstance;
   message_wc.lpszClassName = L"webview_message";
   message_wc.lpfnWndProc   = (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
      win32_edge_engine* w{};

      if (msg == WM_NCCREATE) {
         auto* lpcs{reinterpret_cast<LPCREATESTRUCT>(lp)};
         w                  = static_cast<win32_edge_engine*>(lpcs->lpCreateParams);
         w->message_window_ = hwnd;
         SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
      } else {
         w = reinterpret_cast<win32_edge_engine*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA)
         );
      }

      if (!w) {
         return DefWindowProcW(hwnd, msg, wp, lp);
      }

      switch (msg) {
         case WM_APP:
            if (auto f = reinterpret_cast<std::function<void()>*>(lp)) {
               (*f)();
               delete f;
            }
            break;
         case WM_DESTROY:
            w->message_window_ = nullptr;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            break;
         default:
            return DefWindowProcW(hwnd, msg, wp, lp);
      }
      return 0;
   });
   RegisterClassExW(&message_wc);
   CreateWindowExW(0, L"webview_message", nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInstance, this);
   if (!message_window_) {
      throw exception{error_t::WEBVIEW_ERROR_INVALID_STATE, "Message window is null"};
   }

   if (owns_window_) {
      ShowWindow(window_, SW_SHOW);
      UpdateWindow(window_);
      SetFocus(window_);
   }

   auto cb =
      std::bind(&win32_edge_engine::on_message, this, std::placeholders::_1);

   embed(widget_, debug, cb, options, user_data_dir);
}

win32_edge_engine::~win32_edge_engine() {
   if (com_handler_) {
      com_handler_->Release();
      com_handler_ = nullptr;
   }
   if (webview_) {
      webview_->Release();
      webview_ = nullptr;
   }
   if (controller_) {
      controller_->Release();
      controller_ = nullptr;
   }
   // Replace wndproc to avoid callbacks and other bad things during
   // destruction.
   auto wndproc = reinterpret_cast<LONG_PTR>(
      +[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
         return DefWindowProcW(hwnd, msg, wp, lp);
      }
   );
   if (widget_) {
      SetWindowLongPtrW(widget_, GWLP_WNDPROC, wndproc);
   }
   if (window_ && owns_window_) {
      SetWindowLongPtrW(window_, GWLP_WNDPROC, wndproc);
   }
   if (widget_) {
      DestroyWindow(widget_);
      widget_ = nullptr;
   }
   if (window_) {
      if (owns_window_) {
         DestroyWindow(window_);
         on_window_destroyed(true);
      }
      window_ = nullptr;
   }
   if (owns_window_) {
      // Not strictly needed for windows to close immediately but aligns
      // behavior across backends.
      deplete_run_loop_event_queue();
   }
   // We need the message window in order to deplete the event queue.
   if (message_window_) {
      SetWindowLongPtrW(message_window_, GWLP_WNDPROC, wndproc);
      DestroyWindow(message_window_);
      message_window_ = nullptr;
   }
}

void
win32_edge_engine::run() {
   MSG msg;
   while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
}

HWND
win32_edge_engine::window() const {
   if (window_) {
      return window_;
   }

   throw exception{error_t::WEBVIEW_ERROR_INVALID_STATE};
}

HWND
win32_edge_engine::widget() const {
   if (widget_) {
      return widget_;
   }
   throw exception{error_t::WEBVIEW_ERROR_INVALID_STATE};
}

ICoreWebView2Controller*
win32_edge_engine::browser_controller() const {
   if (controller_) {
      return controller_;
   }
   throw exception{error_t::WEBVIEW_ERROR_INVALID_STATE};
}

void
win32_edge_engine::terminate() {
   PostQuitMessage(0);
}

void
win32_edge_engine::dispatch(std::function<void()>&& f) {
   PostMessageW(message_window_, WM_APP, 0, (LPARAM) new std::function<void()>(std::move(f)));
}

void
win32_edge_engine::set_title(std::string_view title) {
   SetWindowTextW(window_, widen_string(title).c_str());
}

void
win32_edge_engine::set_size(int width, int height, Hint hints) {
   auto style = GetWindowLong(window_, GWL_STYLE);

   if (hints == Hint::FIXED) {
      style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
   } else {
      style |= (WS_THICKFRAME | WS_MAXIMIZEBOX);
   }

   SetWindowLong(window_, GWL_STYLE, style);

   if (hints == Hint::MAX) {
      maxsz_.x = width;
      maxsz_.y = height;
   } else if (hints == Hint::MIN) {
      minsz_.x = width;
      minsz_.y = height;
   } else {
      auto dpi = get_window_dpi(window_);
      dpi_     = dpi;
      auto scaled_size =
         scale_size(width, height, get_default_window_dpi(), dpi);
      auto frame_size =
         make_window_frame_size(window_, scaled_size.cx, scaled_size.cy, dpi);
      SetWindowPos(window_, nullptr, 0, 0, frame_size.cx, frame_size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_FRAMECHANGED);
   }
}

void
win32_edge_engine::navigate_impl(std::string_view url) {
   auto wurl = widen_string(url);
   webview_->Navigate(wurl.c_str());
}

void
win32_edge_engine::open_dev_tools() {
   webview_->OpenDevToolsWindow();
}

void
win32_edge_engine::register_url_handler(std::string const& filter, url_handler_t&& handler) {
   auto wfilter = widen_string(filter);
   auto result  = webview_->AddWebResourceRequestedFilter(
      wfilter.c_str(), COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL
   );

   if (result != S_OK) {
      throw exception{
         error_t::WEBVIEW_ERROR_UNSPECIFIED,
         std::format(
            "Could not AddWebResourceRequestedFilter: {} for scheme: {}",
            std::to_string(result),
            filter
         )
      };
   }

   auto const reg = [&wfilter]() constexpr {
      std::wstring res;

      for (std::size_t i = 0; i < wfilter.length(); ++i) {
         if ((wfilter[i] == L'*' || wfilter[i] == L'?') && (i == 0 || wfilter[i - 1] != L'\\')) {
            if (i != 0) {
               res.pop_back();
            }
            res += std::wstring{L"."} + wfilter[i];
         } else {
            res = wfilter[i];
         }
      }

      return res;
   }();

   handlers_.emplace(reg, std::move(handler));
}

void
win32_edge_engine::install_ressource_handler() {
   ::EventRegistrationToken token;

   auto const result = webview_->add_WebResourceRequested(
      Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(
         [this](ICoreWebView2*, ICoreWebView2WebResourceRequestedEventArgs* args) {
            COREWEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext;

            auto result = args->get_ResourceContext(&resourceContext);
            if (result != S_OK) {
               return result;
            }

            Microsoft::WRL::ComPtr<ICoreWebView2WebResourceRequest>
               webViewRequest;
            args->get_Request(&webViewRequest);

            const auto wuri = [&webViewRequest]() {
               LPWSTR uri;
               webViewRequest->get_Uri(&uri);
               std::wstring wuri{uri};
               CoTaskMemFree(uri);

               return wuri;
            }();

            for (auto const& handler : handlers_) {
               if (std::regex_match(wuri, std::wregex{handler.first})) {
                  auto const request =
                     make_request(narrow_string(wuri), resourceContext, webViewRequest.Get());
                  auto const response =
                     make_response(handler.second(request), result);

                  if (result != S_OK) {
                     return result;
                  }

                  return args->put_Response(response.Get());
               }
            }

            return S_OK;
         }
      )
         .Get(),
      &token
   );

   if (result != S_OK) {
      throw exception{error_t::WEBVIEW_ERROR_UNSPECIFIED, std::format("Could not install resource handler: {}", std::to_string(result))};
   }
}

//---------------------------------------------------------------------------------------------------------------------
Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse>
win32_edge_engine::make_response(http::response_t const& responseData, HRESULT& result) {
   Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse> response;
   Microsoft::WRL::ComPtr<ICoreWebView2_2>                  wv22;
   result = webview_->QueryInterface(IID_PPV_ARGS(&wv22));

   Microsoft::WRL::ComPtr<ICoreWebView2Environment> environment;
   wv22->get_Environment(&environment);

   if (result != S_OK) {
      return {};
   }

   std::wstring responseHeaders;
   for (auto const& [key, value] : responseData.headers) {
      responseHeaders +=
         widen_string(key) + L": " + widen_string(value) + L"\r\n";
   }

   if (!responseHeaders.empty()) {
      responseHeaders.pop_back();
      responseHeaders.pop_back();
   }

   Microsoft::WRL::ComPtr<IStream> stream;
   stream.Attach(SHCreateMemStream(
      reinterpret_cast<const BYTE*>(responseData.body.data()),
      static_cast<UINT>(responseData.body.size())
   ));

   const auto phrase = widen_string(responseData.reasonPhrase);
   result            = environment->CreateWebResourceResponse(
      stream.Get(), responseData.statusCode, phrase.c_str(), responseHeaders.c_str(), &response
   );

   return response;
}

//---------------------------------------------------------------------------------------------------------------------
http::request_t
win32_edge_engine::make_request(std::string const& uri, COREWEBVIEW2_WEB_RESOURCE_CONTEXT, ICoreWebView2WebResourceRequest* webViewRequest) {
   return http::request_t{
      .getContent = [webViewRequest,
                     contentMemo = std::string{}]() mutable {
         if (!contentMemo.empty()) {
            return contentMemo;
         }

         Microsoft::WRL::ComPtr<IStream> stream;
         webViewRequest->get_Content(&stream);

         if (!stream) {
            return contentMemo;
         }

         // FIXME: Dont read the whole thing into memory, if possible via streaming.
         ULONG bytesRead = 0;
         do {
            std::array<char, 1024> buffer{};
            stream->Read(buffer.data(), 1024, &bytesRead);
            contentMemo.append(buffer.data(), bytesRead);
         } while (bytesRead == 1024);
         return contentMemo; },
      .uri        = uri,
      .method =
         [webViewRequest]() {
            LPWSTR method;
            webViewRequest->get_Method(&method);
            std::wstring methodW{method};
            CoTaskMemFree(method);
            return narrow_string(methodW);
         }(),
      .headers =
         [webViewRequest]() {
            ICoreWebView2HttpRequestHeaders* headers;
            webViewRequest->get_Headers(&headers);

            Microsoft::WRL::ComPtr<ICoreWebView2HttpHeadersCollectionIterator>
               iterator;
            headers->GetIterator(&iterator);

            std::unordered_multimap<std::string, std::string> headersMap;
            for (BOOL hasCurrent;
                 SUCCEEDED(iterator->get_HasCurrentHeader(&hasCurrent)) && hasCurrent;) {
               LPWSTR name;
               LPWSTR value;
               iterator->GetCurrentHeader(&name, &value);
               std::wstring nameW{name};
               std::wstring valueW{value};
               CoTaskMemFree(name);
               CoTaskMemFree(value);

               headersMap.emplace(narrow_string(nameW), narrow_string(valueW));

               BOOL hasNext = FALSE;
               if (FAILED(iterator->MoveNext(&hasNext)) || !hasNext) {
                  break;
               }
            }
            return headersMap;
         }()
   };
}

void
win32_edge_engine::eval(std::string_view js) {
   // TODO: Skip if no content has begun loading yet. Can't check with
   //       ICoreWebView2::get_Source because it returns "about:blank".
   auto wjs = widen_string(js);
   webview_->ExecuteScript(wjs.c_str(), nullptr);
}

void
win32_edge_engine::set_html(std::string_view html) {
   webview_->NavigateToString(widen_string(html).c_str());
}

user_script
win32_edge_engine::add_user_script_impl(std::string_view js) {
   auto                               wjs = widen_string(js);
   std::wstring                       script_id;
   bool                               done{};
   webview2_user_script_added_handler handler{[&script_id, &done](HRESULT res, LPCWSTR id) {
      if (SUCCEEDED(res)) {
         script_id = id;
      }
      done = true;
   }};
   auto                               res =
      webview_->AddScriptToExecuteOnDocumentCreated(wjs.c_str(), &handler);
   if (SUCCEEDED(res)) {
      // Sadly we need to pump the even loop in order to get the script ID.
      while (!done) {
         deplete_run_loop_event_queue();
      }
   }
   // TODO: There's a non-zero chance that we didn't get the script ID.
   //       We need to convey the error somehow.
   return {
      js, user_script::impl_ptr{new user_script::impl{script_id, wjs}, [](user_script::impl* p) { delete p; }}
   };
}

void
win32_edge_engine::remove_all_user_scripts(std::list<user_script> const& scripts) {
   for (const auto& script : scripts) {
      const auto& id = script.get_impl().get_id();
      webview_->RemoveScriptToExecuteOnDocumentCreated(id.c_str());
   }
}

bool
win32_edge_engine::are_user_scripts_equal(user_script const& first, user_script const& second) {
   const auto& first_id  = first.get_impl().get_id();
   const auto& second_id = second.get_impl().get_id();
   return first_id == second_id;
}

void
win32_edge_engine::embed(HWND wnd, bool debug, msg_cb_t cb, Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions> options, std::string_view user_data_dir) {
   std::atomic_flag flag = ATOMIC_FLAG_INIT;
   flag.test_and_set();

   std::wstring currentExePath;
   currentExePath.reserve(MAX_PATH);
   currentExePath.resize(GetModuleFileNameW(nullptr, currentExePath.data(), currentExePath.capacity()));

   std::wstring currentExeName{PathFindFileNameW(currentExePath.c_str())};

   com_handler_ = new webview2_com_handler(
      wnd, cb, [&](ICoreWebView2Controller* controller, ICoreWebView2* webview) {
         if (!controller || !webview) {
            flag.clear();
            return;
         }
         controller->AddRef();
         webview->AddRef();
         controller_ = controller;
         webview_    = webview;
         flag.clear();
      }
   );

   auto const wuser_data_dir = widen_string(user_data_dir);

   com_handler_->set_attempt_handler([&] {
      return webview2_loader_.create_environment_with_options(
         nullptr, wuser_data_dir.size() ? wuser_data_dir.c_str() : nullptr, options.Get(), com_handler_
      );
   });
   com_handler_->try_create_environment();

   // Pump the message loop until WebView2 has finished initialization.
   bool got_quit_msg = false;
   MSG  msg;
   while (flag.test_and_set() && GetMessageW(&msg, nullptr, 0, 0) >= 0) {
      if (msg.message == WM_QUIT) {
         got_quit_msg = true;
         break;
      }
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   if (got_quit_msg) {
      throw exception{error_t::WEBVIEW_ERROR_CANCELED};
   }
   if (!controller_ || !webview_) {
      throw exception{error_t::WEBVIEW_ERROR_INVALID_STATE};
   }
   ICoreWebView2Settings* settings = nullptr;
   auto                   res      = webview_->get_Settings(&settings);
   if (res != S_OK) {
      throw exception{error_t::WEBVIEW_ERROR_UNSPECIFIED, "get_Settings failed"};
   }
   res = settings->put_AreDevToolsEnabled(debug ? TRUE : FALSE);
   if (res != S_OK) {
      throw exception{error_t::WEBVIEW_ERROR_UNSPECIFIED, "put_AreDevToolsEnabled failed"};
   }
   res = settings->put_IsStatusBarEnabled(FALSE);
   if (res != S_OK) {
      throw exception{error_t::WEBVIEW_ERROR_UNSPECIFIED, "put_IsStatusBarEnabled failed"};
   }
   add_init_script(R"_(function(message) {
   return window.chrome.webview.postMessage(message);
})_");
   resize_webview();
   controller_->put_IsVisible(TRUE);
   ShowWindow(widget_, SW_SHOW);
   UpdateWindow(widget_);
   if (owns_window_) {
      focus_webview();
   }
}

void
win32_edge_engine::resize_widget() {
   if (widget_) {
      RECT r{};
      if (GetClientRect(GetParent(widget_), &r)) {
         MoveWindow(widget_, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
      }
   }
}

void
win32_edge_engine::resize_webview() {
   if (widget_ && controller_) {
      RECT bounds{};
      if (GetClientRect(widget_, &bounds)) {
         controller_->put_Bounds(bounds);
      }
   }
}

void
win32_edge_engine::focus_webview() {
   if (controller_) {
      controller_->MoveFocus(COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC);
   }
}

bool
win32_edge_engine::is_webview2_available() const noexcept {
   LPWSTR version_info = nullptr;
   auto   res          = webview2_loader_.get_available_browser_version_string(
      nullptr, &version_info
   );
   // The result will be equal to HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
   // if the WebView2 runtime is not installed.
   auto ok = SUCCEEDED(res) && version_info;
   if (version_info) {
      CoTaskMemFree(version_info);
   }
   return ok;
}

void
win32_edge_engine::on_dpi_changed(int dpi) {
   auto scaled_size = get_scaled_size(dpi_, dpi);
   auto frame_size =
      make_window_frame_size(window_, scaled_size.cx, scaled_size.cy, dpi);
   SetWindowPos(window_, nullptr, 0, 0, frame_size.cx, frame_size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_FRAMECHANGED);
   dpi_ = dpi;
}

SIZE
win32_edge_engine::get_size() const {
   RECT bounds;
   GetClientRect(window_, &bounds);
   auto width  = bounds.right - bounds.left;
   auto height = bounds.bottom - bounds.top;
   return {width, height};
}

SIZE
win32_edge_engine::get_scaled_size(int from_dpi, int to_dpi) const {
   auto size = get_size();
   return scale_size(size.cx, size.cy, from_dpi, to_dpi);
}

void
win32_edge_engine::on_system_setting_change(const wchar_t* area) {
   // Detect light/dark mode change in system.
   if (lstrcmpW(area, L"ImmersiveColorSet") == 0) {
      apply_window_theme(window_);
   }
}

// Blocks while depleting the run loop of events.
void
win32_edge_engine::deplete_run_loop_event_queue() {
   bool done{};
   dispatch([&done] { done = true; });
   while (!done) {
      MSG msg;
      if (GetMessageW(&msg, nullptr, 0, 0) > 0) {
         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
   }
}

}  // namespace detail
}  // namespace webview

#endif  // defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
