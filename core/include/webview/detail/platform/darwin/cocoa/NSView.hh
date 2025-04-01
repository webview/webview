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

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"
#include "NSRect.hh"
#include "literals.hh"

namespace webview {
namespace detail {
namespace cocoa {

enum NSAutoresizingMaskOptions : NSUInteger {
  NSViewMinXMargin = 1,
  NSViewWidthSizable = 2,
  NSViewMaxXMargin = 4,
  NSViewMinYMargin = 8,
  NSViewHeightSizable = 16,
  NSViewMaxYMargin = 32
};

inline id NSView_alloc() {
  return objc::msg_send<id>("NSView"_cls, "alloc"_sel);
}

inline id NSView_initWithFrame(id self, NSRect frame_rect) {
  return objc::msg_send<id>(self, "initWithFrame:"_sel, frame_rect);
}

inline id NSView_withFrame(NSRect frame_rect) {
  return objc::autorelease(NSView_initWithFrame(NSView_alloc(), frame_rect));
}

inline void NSView_set_autoresizesSubviews(id self, bool resizes) {
  objc::msg_send<void>(self, "setAutoresizesSubviews:"_sel,
                       static_cast<BOOL>(resizes));
}

inline void NSView_addSubview(id self, id subview) {
  objc::msg_send<void>(self, "addSubview:"_sel, subview);
}

inline NSRect NSView_get_bounds(id self) {
  return objc::msg_send_stret<NSRect>(self, "bounds"_sel);
}

inline void NSView_set_frame(id self, NSRect frame) {
  objc::msg_send<void>(self, "setFrame:"_sel, frame);
}

inline void NSView_set_autoresizingMask(id self,
                                        NSAutoresizingMaskOptions mask) {
  objc::msg_send<void>(self, "setAutoresizingMask:"_sel, mask);
}

} // namespace cocoa
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSVIEW_HH
