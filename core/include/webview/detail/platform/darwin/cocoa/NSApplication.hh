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

#ifndef WEBVIEW_PLATFORM_DARWIN_COCOA_NSAPPLICATION_HH
#define WEBVIEW_PLATFORM_DARWIN_COCOA_NSAPPLICATION_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"
#include "NSString.hh"

namespace webview {
namespace detail {
namespace cocoa {

enum NSApplicationActivationPolicy : NSInteger {
  NSApplicationActivationPolicyRegular = 0
};

enum NSEventMask : NSUInteger { NSEventMaskAny = NSUIntegerMax };

enum NSModalResponse : NSInteger { NSModalResponseOK = 1 };

namespace NSRunLoopMode {
inline id NSDefaultRunLoopMode() {
  return NSString_stringWithUTF8String("kCFRunLoopDefaultMode");
}
} // namespace NSRunLoopMode

inline void NSApplication_set_delegate(id self, id delegate) {
  objc::msg_send<void>(self, objc::selector("setDelegate:"), delegate);
}

inline void NSApplication_run(id self) {
  objc::msg_send<void>(self, objc::selector("run"));
}

inline void NSApplication_stop(id self, id sender = nullptr) {
  objc::msg_send<void>(self, objc::selector("stop:"), sender);
}

inline id NSApplication_get_sharedApplication() {
  return objc::msg_send<id>(objc::get_class("NSApplication"),
                            objc::selector("sharedApplication"));
}

inline void NSApplication_sendEvent(id self, id event) {
  objc::msg_send<void>(self, objc::selector("sendEvent:"), event);
}

inline id NSApplication_nextEventMatchingMask(id self, NSEventMask mask,
                                              id expiration, id mode,
                                              bool dequeue) {
  return objc::msg_send<id>(
      self, objc::selector("nextEventMatchingMask:untilDate:inMode:dequeue:"),
      mask, expiration, mode, dequeue);
}

inline void
NSApplication_setActivationPolicy(id self,
                                  NSApplicationActivationPolicy policy) {
  objc::msg_send<void>(self, objc::selector("setActivationPolicy:"), policy);
}

inline void NSApplication_activateIgnoringOtherApps(id self, bool ignore) {
  objc::msg_send<void>(self, objc::selector("activateIgnoringOtherApps:"),
                       static_cast<BOOL>(ignore));
}

inline void NSApplication_postEvent(id self, id event, bool at_start) {
  objc::msg_send<void>(self, objc::selector("postEvent:atStart:"), event,
                       static_cast<BOOL>(at_start));
}

} // namespace cocoa
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSAPPLICATION_HH
