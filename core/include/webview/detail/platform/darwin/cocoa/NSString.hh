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

#ifndef WEBVIEW_PLATFORM_DARWIN_COCOA_NSSTRING_HH
#define WEBVIEW_PLATFORM_DARWIN_COCOA_NSSTRING_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"

#include <string>

namespace webview {
namespace detail {
namespace cocoa {

enum NSStringEncoding : NSUInteger {
  NSASCIIStringEncoding = 1,
  NSUTF8StringEncoding = 4
};

inline bool NSString_hasSuffix(id self, id suffix) {
  return static_cast<bool>(
      objc::msg_send<BOOL>(self, objc::selector("hasSuffix:"), suffix));
}

inline id NSString_alloc() {
  return objc::msg_send<id>(objc::get_class("NSString"),
                            objc::selector("alloc"));
}

inline id NSString_initWithBytes(id self, const void *bytes, NSUInteger length,
                                 NSStringEncoding encoding) {
  return objc::msg_send<id>(self,
                            objc::selector("initWithBytes:length:encoding:"),
                            bytes, length, encoding);
}

inline id NSString_stringWithUTF8String(const char *utf8_string) {
  return objc::msg_send<id>(objc::get_class("NSString"),
                            objc::selector("stringWithUTF8String:"),
                            utf8_string);
}

inline id NSString_stringWithUTF8String(const std::string &utf8_string) {
  return objc::autorelease(NSString_initWithBytes(
      NSString_alloc(), utf8_string.data(),
      static_cast<NSUInteger>(utf8_string.size()), NSUTF8StringEncoding));
}

inline const char *NSString_get_UTF8String(id self) {
  return objc::msg_send<const char *>(self, objc::selector("UTF8String"));
}

} // namespace cocoa
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSSTRING_HH
