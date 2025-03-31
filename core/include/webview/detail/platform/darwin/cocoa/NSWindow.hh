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

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"
#include "NSRect.hh"
#include "NSSize.hh"
#include "literals.hh"

#include <string>

namespace webview {
namespace detail {
namespace cocoa {

enum NSBackingStoreType : NSUInteger { NSBackingStoreBuffered = 2 };

enum NSWindowStyleMask : NSUInteger {
  NSWindowStyleMaskTitled = 1,
  NSWindowStyleMaskClosable = 2,
  NSWindowStyleMaskMiniaturizable = 4,
  NSWindowStyleMaskResizable = 8
};

inline id NSWindow_alloc() {
  return objc::msg_send<id>("NSWindow"_cls, "alloc"_sel);
}

inline id NSWindow_initWithContentRect(id self, NSRect content_rect,
                                       NSWindowStyleMask style,
                                       NSBackingStoreType backing_store_type,
                                       bool defer) {
  return objc::msg_send<id>(
      self, "initWithContentRect:styleMask:backing:defer:"_sel, content_rect,
      style, backing_store_type, static_cast<BOOL>(defer));
}

inline id NSWindow_withContentRect(NSRect content_rect, NSWindowStyleMask style,
                                   NSBackingStoreType backing_store_type,
                                   bool defer) {
  return objc::autorelease(NSWindow_initWithContentRect(
      NSWindow_alloc(), content_rect, style, backing_store_type, defer));
}

inline void NSWindow_close(id self) { objc::msg_send<void>(self, "close"_sel); }

inline NSRect NSWindow_get_frame(id self) {
  return objc::msg_send_stret<NSRect>(self, "frame"_sel);
}

inline void NSWindow_setFrame(id self, NSRect frame_rect, bool display,
                              bool animate) {
  objc::msg_send<void>(self, "setFrame:display:animate:"_sel, frame_rect,
                       static_cast<BOOL>(display), static_cast<BOOL>(animate));
}

inline void NSWindow_set_styleMask(id self, NSWindowStyleMask style) {
  objc::msg_send<void>(self, "setStyleMask:"_sel, style);
}

inline void NSWindow_set_title(id self, const std::string &title) {
  objc::autoreleasepool arp;
  objc::msg_send<void>(self, "setTitle:"_sel,
                       objc::msg_send<id>("NSString"_cls,
                                          "stringWithUTF8String:"_sel,
                                          title.c_str()));
}

inline void NSWindow_makeKeyAndOrderFront(id self, id sender = nullptr) {
  objc::msg_send<void>(self, "makeKeyAndOrderFront:"_sel, sender);
}

inline void NSWindow_set_contentView(id self, id view) {
  objc::msg_send<void>(self, "setContentView:"_sel, view);
}

inline id NSWindow_get_contentView(id self) {
  return objc::msg_send<id>(self, "contentView"_sel);
}

inline void NSWindow_set_delegate(id self, id delegate) {
  objc::msg_send<void>(self, "setDelegate:"_sel, delegate);
}

inline void NSWindow_center(id self) {
  objc::msg_send<void>(self, "center"_sel);
}

inline void NSWindow_set_contentMinSize(id self, NSSize size) {
  objc::msg_send<void>(self, "setContentMinSize:"_sel, size);
}

inline void NSWindow_set_contentMaxSize(id self, NSSize size) {
  objc::msg_send<void>(self, "setContentMaxSize:"_sel, size);
}

} // namespace cocoa
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSWINDOW_HH
