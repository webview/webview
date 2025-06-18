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
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#include "detail/platform/darwin/objc/objc.hh"
#include "detail/platform/darwin/types.hh"

namespace webview {
namespace detail {
namespace platform {
namespace darwin {

enum NSEventType : NSUInteger {
  // For macOS 10.12+; replaces NSApplicationDefined (macOS 10.0–10.12)
  // with the same value
  ApplicationDefined = 15
};
enum NSEventModifierFlags : NSUInteger {};

/// An object that contains information about an input action, such as a mouse click or a key press.
/// @see https://developer.apple.com/documentation/appkit/nsevent?language=objc
struct NSEvent {

  static id otherEventWithType(NSEventType type, NSPoint_t location,
                               NSEventModifierFlags modifier_flags,
                               NSTimeInterval_t timestamp,
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
};

} // namespace darwin
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSEVENT_HH
