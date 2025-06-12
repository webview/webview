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

#ifndef WEBVIEW_PLATFORM_DARWIN_COCOA_NSEVENT_HH
#define WEBVIEW_PLATFORM_DARWIN_COCOA_NSEVENT_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"
#include "NSPoint.hh"
#include "types.hh"

namespace webview {
namespace detail {
namespace cocoa {

enum NSEventType : NSUInteger {
  // For macOS 10.12+; replaces NSApplicationDefined (macOS 10.0–10.12)
  // with the same value
  NSEventTypeApplicationDefined = 15
};

enum NSEventModifierFlags : NSUInteger {};

inline id NSEvent_otherEventWithType(NSEventType type, NSPoint location,
                                     NSEventModifierFlags modifier_flags,
                                     NSTimeInterval timestamp,
                                     NSInteger window_number, id context,
                                     short subtype, NSInteger data1,
                                     NSInteger data2) {
  return objc::msg_send<id>(
      objc::get_class("NSEvent"),
      objc::selector("otherEventWithType:location:modifierFlags:timestamp:"
                     "windowNumber:context:subtype:data1:data2:"),
      type, location, modifier_flags, timestamp, window_number, context,
      subtype, data1, data2);
}

} // namespace cocoa
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSEVENT_HH
