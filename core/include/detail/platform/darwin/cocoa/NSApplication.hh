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
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#include "detail/platform/darwin/cocoa/NSString.hh"
#include "detail/platform/darwin/objc/objc.hh"
#include "detail/platform/darwin/types.hh"

namespace webview {
namespace detail {
namespace platform {
namespace darwin {

enum NSEventMask : NSUInteger { NSEventMaskAny = NSUIntegerMax };
enum NSModalResponse : NSInteger { NSModalResponseOK = 1 };
struct NSRunLoopMode {
  static id NSDefaultRunLoopMode() {
    return NSString::stringWithUTF8String("kCFRunLoopDefaultMode");
  }
};

/// An object that manages an app’s main event loop and resources used by all of that app’s objects.
/// @see https://developer.apple.com/documentation/appkit/nsapplication
struct NSApplication {

  enum ActivationPolicy : NSInteger { ActivationPolicyRegular = 0 };

  static void set_delegate(id self, id delegate) {
    objc::msg_send<void>(self, objc::selector("setDelegate:"), delegate);
  }

  static void run(id self) {
    objc::msg_send<void>(self, objc::selector("run"));
  }

  static void stop(id self, id sender = nullptr) {
    objc::msg_send<void>(self, objc::selector("stop:"), sender);
  }

  static id get_sharedApplication() {
    return objc::msg_send<id>(objc::get_class("NSApplication"),
                              objc::selector("sharedApplication"));
  }

  static void sendEvent(id self, id event) {
    objc::msg_send<void>(self, objc::selector("sendEvent:"), event);
  }

  static id nextEventMatchingMask(id self, NSEventMask mask, id expiration,
                                  id mode, bool dequeue) {
    return objc::msg_send<id>(
        self, objc::selector("nextEventMatchingMask:untilDate:inMode:dequeue:"),
        mask, expiration, mode, dequeue);
  }

  static void setActivationPolicy(id self, ActivationPolicy policy) {
    objc::msg_send<void>(self, objc::selector("setActivationPolicy:"), policy);
  }

  static void activateIgnoringOtherApps(id self, bool ignore) {
    objc::msg_send<void>(self, objc::selector("activateIgnoringOtherApps:"),
                         static_cast<BOOL>(ignore));
  }

  static void postEvent(id self, id event, bool at_start) {
    objc::msg_send<void>(self, objc::selector("postEvent:atStart:"), event,
                         static_cast<BOOL>(at_start));
  }
};

} // namespace darwin
} // namespace platform
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSAPPLICATION_HH
