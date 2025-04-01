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

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"

namespace webview {
namespace detail {
namespace webkit {

enum WKUserScriptInjectionTime : NSInteger {
  WKUserScriptInjectionTimeAtDocumentStart = 0
};

inline id WKUserScript_alloc() {
  return objc::msg_send<id>(objc::get_class("WKUserScript"),
                            objc::selector("alloc"));
}

inline id WKUserScript_initWithSource(id self, id source,
                                      WKUserScriptInjectionTime injection_time,
                                      bool for_main_frame_only) {
  return objc::msg_send<id>(
      self, objc::selector("initWithSource:injectionTime:forMainFrameOnly:"),
      source, injection_time, static_cast<BOOL>(for_main_frame_only));
}

inline id WKUserScript_withSource(id source,
                                  WKUserScriptInjectionTime injection_time,
                                  bool for_main_frame_only) {
  return objc::autorelease(WKUserScript_initWithSource(
      WKUserScript_alloc(), source, injection_time, for_main_frame_only));
}

} // namespace webkit
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKUSERSCRIPT_HH
