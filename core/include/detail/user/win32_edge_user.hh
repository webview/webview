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

#ifndef WEBVIEW_USER_WIN32_EDGE_USER_HH
#define WEBVIEW_USER_WIN32_EDGE_USER_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
#include "detail/platform/windows/iid.hh"
#include "detail/platform/windows/webview2/loader.hh"
#include "detail/user/user_script.hh"
#include <atomic>

using namespace webview::detail::platform::windows;
namespace webview {
namespace detail {
namespace user {

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

    if (iid::cast_if_equal_iid(
            this, riid, mswebview2::cast_info::controller_completed(), ppv) ||
        iid::cast_if_equal_iid(
            this, riid, mswebview2::cast_info::environment_completed(), ppv) ||
        iid::cast_if_equal_iid(
            this, riid, mswebview2::cast_info::message_received(), ppv) ||
        iid::cast_if_equal_iid(
            this, riid, mswebview2::cast_info::permission_requested(), ppv)) {
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
      m_msgCb(win_string::narrow_string(message));
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
      // Wait for m_sleep_ms before trying again.
      Sleep(m_sleep_ms);
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
  unsigned int m_max_attempts = 60;
  unsigned int m_sleep_ms = 200;
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

    if (!ppv) {
      return E_POINTER;
    }

    if (iid::cast_if_equal_iid(
            this, riid,
            mswebview2::cast_info::
                add_script_to_execute_on_document_created_completed(),
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

} // namespace user
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS) && defined(WEBVIEW_EDGE)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_USER_WIN32_EDGE_USER_HH
