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

#include "../objc.hh"
#include "NSRect.hh"
#include "types.hh"

#include <string>

namespace webview {
namespace detail {
namespace cocoa {

inline id NSWindow_new(NSRect content_rect, NSWindowStyleMask style,
                       NSBackingStoreType backing_store_type, bool defer) {
  using namespace objc::literals;
  return objc::msg_send<id>(objc::msg_send<id>("NSWindow"_cls, "alloc"_sel),
                            "initWithContentRect:styleMask:backing:defer:"_sel,
                            content_rect, style, backing_store_type,
                            static_cast<BOOL>(defer));
}

inline void NSWindow_close(id window) {
  using namespace objc::literals;
  objc::msg_send<void>(window, "close"_sel);
}

inline NSRect NSWindow_get_frame(id window) {
  using namespace objc::literals;
  return objc::msg_send_stret<NSRect>(window, "frame"_sel);
}

inline void NSWindow_set_frame(id window, NSRect frame_rect, bool display,
                               bool animate) {
  using namespace objc::literals;
  objc::msg_send<void>(window, "setFrame:display:animate:"_sel, frame_rect,
                       static_cast<BOOL>(display), static_cast<BOOL>(animate));
}

inline void NSWindow_set_style_mask(id window, NSWindowStyleMask style) {
  using namespace objc::literals;
  objc::msg_send<void>(window, "setStyleMask:"_sel, style);
}

inline void NSWindow_set_title(id window, const std::string &title) {
  using namespace objc::literals;
  objc::autoreleasepool arp;
  objc::msg_send<void>(window, "setTitle:"_sel,
                       objc::msg_send<id>("NSString"_cls,
                                          "stringWithUTF8String:"_sel,
                                          title.c_str()));
}

inline void NSWindow_make_key_and_order_front(id window, id sender = nullptr) {
  using namespace objc::literals;
  objc::msg_send<void>(window, "makeKeyAndOrderFront:"_sel, sender);
}

} // namespace cocoa
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSWINDOW_HH
