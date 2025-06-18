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

#ifndef WEBVIEW_PLATFORM_DARWIN_COCOA_NSOPENPANEL_HH
#define WEBVIEW_PLATFORM_DARWIN_COCOA_NSOPENPANEL_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#include "detail/platform/darwin/objc/objc.hh"

namespace webview {
namespace detail {
namespace platform {
namespace darwin {

/// A panel that prompts the user to select a file to open.
/// @see https://developer.apple.com/documentation/appkit/nsopenpanel?language=objc
struct NSOpenPanel {

  static id openPanel() {
    return objc::msg_send<id>(objc::get_class("NSOpenPanel"),
                              objc::selector("openPanel"));
  }

  static void set_canChooseFiles(id self, bool value) {
    objc::msg_send<void>(self, objc::selector("setCanChooseFiles:"),
                         static_cast<BOOL>(value));
  }

  static void set_canChooseDirectories(id self, bool value) {
    objc::msg_send<void>(self, objc::selector("setCanChooseDirectories:"),
                         static_cast<BOOL>(value));
  }

  static void set_allowsMultipleSelection(id self, bool value) {
    objc::msg_send<void>(self, objc::selector("setAllowsMultipleSelection:"),
                         static_cast<BOOL>(value));
  }

  static id get_URLs(id self) {
    return objc::msg_send<id>(self, objc::selector("URLs"));
  }
};

} // namespace darwin
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSOPENPANEL_HH
