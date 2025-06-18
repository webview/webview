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

#ifndef WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKUSERSCRIPT_HH
#define WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKUSERSCRIPT_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#include "detail/platform/darwin/objc/objc.hh"

namespace webview {
namespace detail {
namespace platform {
namespace darwin {

/// A script that the web view injects into a webpage.
/// @see https://developer.apple.com/documentation/webkit/wkuserscript?language=objc
struct WKUserScript {

  enum InjectionTime : NSInteger { AtDocumentStart = 0 };

  static id alloc() {
    return objc::msg_send<id>(objc::get_class("WKUserScript"),
                              objc::selector("alloc"));
  }

  static id initWithSource(id self, id source, InjectionTime injection_time,
                           bool for_main_frame_only) {
    return objc::msg_send<id>(
        self, objc::selector("initWithSource:injectionTime:forMainFrameOnly:"),
        source, injection_time, static_cast<BOOL>(for_main_frame_only));
  }

  static id withSource(id source, InjectionTime injection_time,
                       bool for_main_frame_only) {
    return objc::autorelease(
        initWithSource(alloc(), source, injection_time, for_main_frame_only));
  }
};

} // namespace darwin
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKUSERSCRIPT_HH
