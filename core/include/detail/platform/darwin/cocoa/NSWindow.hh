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

#ifndef WEBVIEW_PLATFORM_DARWIN_COCOA_NSWINDOW_HH
#define WEBVIEW_PLATFORM_DARWIN_COCOA_NSWINDOW_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#include "detail/platform/darwin/objc/objc.hh"
#include "detail/platform/darwin/types.hh"
#include "types/types.hh"
#include <string>

using namespace webview::types;
namespace webview {
namespace detail {
namespace platform {
namespace darwin {

/// Constants that specify how the window device buffers the drawing done in a window.
/// @see https://developer.apple.com/documentation/appkit/nswindow/backingstoretype?language=objc
enum NSBackingStoreType : NSUInteger { NSBackingStoreBuffered = 2 };

/// A window that an app displays on the screen.
/// @see https://developer.apple.com/documentation/appkit/nswindow?language=objc
struct NSWindow {

  enum StyleMask : NSUInteger {
    StyleMaskTitled = 1,
    StyleMaskClosable = 2,
    StyleMaskMiniaturizable = 4,
    StyleMaskResizable = 8
  };

  static id alloc() {
    return objc::msg_send<id>(objc::get_class("NSWindow"),
                              objc::selector("alloc"));
  }

  static id initWithContentRect(id self, NSRect_t content_rect, StyleMask style,
                                NSBackingStoreType backing_store_type,
                                bool defer) {
    return objc::msg_send<id>(
        self, objc::selector("initWithContentRect:styleMask:backing:defer:"),
        content_rect, style, backing_store_type, static_cast<BOOL>(defer));
  }

  static id withContentRect(NSRect_t content_rect, StyleMask style,
                            NSBackingStoreType backing_store_type, bool defer) {
    return objc::autorelease(initWithContentRect(alloc(), content_rect, style,
                                                 backing_store_type, defer));
  }

  static void close(id self) {
    objc::msg_send<void>(self, objc::selector("close"));
  }

  static NSRect_t get_frame(id self) {
    return objc::msg_send_stret<NSRect_t>(self, objc::selector("frame"));
  }

  static void setFrame(id self, NSRect_t frame_rect, bool display,
                       bool animate) {
    objc::msg_send<void>(self, objc::selector("setFrame:display:animate:"),
                         frame_rect, static_cast<BOOL>(display),
                         static_cast<BOOL>(animate));
  }

  static void set_styleMask(id self, StyleMask style) {
    objc::msg_send<void>(self, objc::selector("setStyleMask:"), style);
  }

  static void set_title(id self, const std::string &title) {
    objc::autoreleasepool arp;
    objc::msg_send<void>(
        self, objc::selector("setTitle:"),
        objc::msg_send<id>(objc::get_class("NSString"),
                           objc::selector("stringWithUTF8String:"),
                           title.c_str()));
  }

  static void makeKeyAndOrderFront(id self, id sender = nullptr) {
    objc::msg_send<void>(self, objc::selector("makeKeyAndOrderFront:"), sender);
  }

  static void set_contentView(id self, id view) {
    objc::msg_send<void>(self, objc::selector("setContentView:"), view);
  }

  static id get_contentView(id self) {
    return objc::msg_send<id>(self, objc::selector("contentView"));
  }

  static void set_delegate(id self, id delegate) {
    objc::msg_send<void>(self, objc::selector("setDelegate:"), delegate);
  }

  static void center(id self) {
    objc::msg_send<void>(self, objc::selector("center"));
  }

  static void set_contentMinSize(id self, NSSize_t size) {
    objc::msg_send<void>(self, objc::selector("setContentMinSize:"), size);
  }

  static void set_contentMaxSize(id self, NSSize_t size) {
    objc::msg_send<void>(self, objc::selector("setContentMaxSize:"), size);
  }
};

} // namespace darwin
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSWINDOW_HH
