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

#ifndef WEBVIEW_PLATFORM_DARWIN_COCOA_NSVIEW_HH
#define WEBVIEW_PLATFORM_DARWIN_COCOA_NSVIEW_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#include "detail/platform/darwin/objc/objc.hh"
#include "detail/platform/darwin/types.hh"

namespace webview {
namespace detail {
namespace platform {
namespace darwin {

/// Constants that specify the autoresizing behaviors for views.
/// @see https://developer.apple.com/documentation/appkit/nsview/autoresizingmask-swift.struct?language=objc
struct NSAutoresizing {

  enum MaskOptions : NSUInteger {
    MinXMargin = 1,
    WidthSizable = 2,
    MaxXMargin = 4,
    MinYMargin = 8,
    HeightSizable = 16,
    MaxYMargin = 32
  };
};

/// The infrastructure for drawing, printing, and handling events in an app.
/// @see https://developer.apple.com/documentation/appkit/nsview?language=objc
struct NSView {

  static id alloc() {
    return objc::msg_send<id>(objc::get_class("NSView"),
                              objc::selector("alloc"));
  }

  static id initWithFrame(id self, NSRect_t frame_rect) {
    return objc::msg_send<id>(self, objc::selector("initWithFrame:"),
                              frame_rect);
  }

  static id withFrame(NSRect_t frame_rect) {
    return objc::autorelease(initWithFrame(alloc(), frame_rect));
  }

  static void set_autoresizesSubviews(id self, bool resizes) {
    objc::msg_send<void>(self, objc::selector("setAutoresizesSubviews:"),
                         static_cast<BOOL>(resizes));
  }

  static void addSubview(id self, id subview) {
    objc::msg_send<void>(self, objc::selector("addSubview:"), subview);
  }

  static NSRect_t get_bounds(id self) {
    return objc::msg_send_stret<NSRect_t>(self, objc::selector("bounds"));
  }

  static void set_frame(id self, NSRect_t frame) {
    objc::msg_send<void>(self, objc::selector("setFrame:"), frame);
  }

  static void set_autoresizingMask(id self, NSAutoresizing::MaskOptions mask) {
    objc::msg_send<void>(self, objc::selector("setAutoresizingMask:"), mask);
  }
};

} // namespace darwin
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSVIEW_HH
