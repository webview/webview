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

#ifndef WEBVIEW_PLATFORM_DARWIN_COCOA_COCOA_HH
#define WEBVIEW_PLATFORM_DARWIN_COCOA_COCOA_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

// IWYU pragma: begin_exports
#include "detail/platform/darwin/cocoa/NSApplication.hh"
#include "detail/platform/darwin/cocoa/NSBundle.hh"
#include "detail/platform/darwin/cocoa/NSEvent.hh"
#include "detail/platform/darwin/cocoa/NSInvocation.hh"
#include "detail/platform/darwin/cocoa/NSMethodSignature.hh"
#include "detail/platform/darwin/cocoa/NSNotification.hh"
#include "detail/platform/darwin/cocoa/NSNumber.hh"
#include "detail/platform/darwin/cocoa/NSObject.hh"
#include "detail/platform/darwin/cocoa/NSOpenPanel.hh"
#include "detail/platform/darwin/cocoa/NSPoint.hh"
#include "detail/platform/darwin/cocoa/NSRect.hh"
#include "detail/platform/darwin/cocoa/NSSavePanel.hh"
#include "detail/platform/darwin/cocoa/NSSize.hh"
#include "detail/platform/darwin/cocoa/NSString.hh"
#include "detail/platform/darwin/cocoa/NSURL.hh"
#include "detail/platform/darwin/cocoa/NSURLRequest.hh"
#include "detail/platform/darwin/cocoa/NSValue.hh"
#include "detail/platform/darwin/cocoa/NSView.hh"
#include "detail/platform/darwin/cocoa/NSWindow.hh"
// IWYU pragma: end_exports

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_COCOA_HH
