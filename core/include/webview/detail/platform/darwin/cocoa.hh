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

#if !defined(WEBVIEW_PLATFORM_DARWIN_COCOA_HH) &&                              \
    defined(WEBVIEW_PLATFORM_DARWIN)
#define WEBVIEW_PLATFORM_DARWIN_COCOA_HH

#include <objc/NSObjCRuntime.h>

namespace webview {
namespace detail {

enum NSBackingStoreType : NSUInteger { NSBackingStoreBuffered = 2 };

enum NSWindowStyleMask : NSUInteger {
  NSWindowStyleMaskTitled = 1,
  NSWindowStyleMaskClosable = 2,
  NSWindowStyleMaskMiniaturizable = 4,
  NSWindowStyleMaskResizable = 8
};

enum NSApplicationActivationPolicy : NSInteger {
  NSApplicationActivationPolicyRegular = 0
};

enum NSModalResponse : NSInteger { NSModalResponseOK = 1 };

enum NSAutoresizingMaskOptions : NSUInteger {
  NSViewMinXMargin = 1,
  NSViewWidthSizable = 2,
  NSViewMaxXMargin = 4,
  NSViewMinYMargin = 8,
  NSViewHeightSizable = 16,
  NSViewMaxYMargin = 32
};

} // namespace detail
} // namespace webview

#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_HH
