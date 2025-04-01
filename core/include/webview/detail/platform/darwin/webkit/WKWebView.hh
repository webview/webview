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

#ifndef WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKWEBVIEW_HH
#define WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKWEBVIEW_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"

#include <CoreGraphics/CoreGraphics.h>

namespace webview {
namespace detail {
namespace webkit {

inline id WKWebView_alloc() {
  return objc::msg_send<id>(objc::get_class("WKWebView"),
                            objc::selector("alloc"));
}

inline id WKWebView_initWithFrame(id self, CGRect frame, id configuration) {
  return objc::msg_send<id>(self,
                            objc::selector("initWithFrame:configuration:"),
                            frame, configuration);
}

inline id WKWebView_withFrame(CGRect frame, id configuration) {
  return objc::autorelease(
      WKWebView_initWithFrame(WKWebView_alloc(), frame, configuration));
}

inline id WKWebView_get_UIDelegate(id self) {
  return objc::msg_send<id>(self, objc::selector("UIDelegate"));
}

inline void WKWebView_set_UIDelegate(id self, id ui_delegate) {
  objc::msg_send<void>(self, objc::selector("setUIDelegate:"), ui_delegate);
}

inline id WKWebView_loadHTMLString(id self, id string, id base_url) {
  return objc::msg_send<id>(self, objc::selector("loadHTMLString:baseURL:"),
                            string, base_url);
}

inline id WKWebView_get_URL(id self) {
  return objc::msg_send<id>(self, objc::selector("URL"));
}

inline id WKWebView_loadRequest(id self, id request) {
  return objc::msg_send<id>(self, objc::selector("loadRequest:"), request);
}

inline void WKWebView_evaluateJavaScript(id self, id js_string,
                                         const void *completion_handler) {
  return objc::msg_send<void>(
      self, objc::selector("evaluateJavaScript:completionHandler:"), js_string,
      completion_handler);
}

inline void WKWebView_set_inspectable(id self, bool inspectable) {
#if defined(__has_builtin)
#if __has_builtin(__builtin_available)
  if (__builtin_available(macOS 13.3, iOS 16.4, tvOS 16.4, *)) {
    objc::msg_send<void>(self, objc::selector("setInspectable:"),
                         static_cast<BOOL>(inspectable));
  }
#else
#error __builtin_available not supported by compiler
#endif
#else
#error __has_builtin not supported by compiler
#endif
}

} // namespace webkit
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKWEBVIEW_HH
