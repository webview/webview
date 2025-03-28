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

#include "../objc.hh"
#include <objc/objc.h>

namespace webview {
namespace detail {
namespace cocoa {

inline void NSApplication_set_delegate(id app, id delegate) {
  using namespace objc::literals;
  objc::msg_send<void>(app, "setDelegate:"_sel, delegate);
}

inline void NSApplication_run(id app) {
  using namespace objc::literals;
  objc::msg_send<void>(app, "run"_sel);
}

inline void NSApplication_stop(id app, id sender = nullptr) {
  using namespace objc::literals;
  objc::msg_send<void>(app, "stop:"_sel, sender);
}

inline id NSApplication_get_shared_application() {
  using namespace objc::literals;
  return objc::msg_send<id>("NSApplication"_cls, "sharedApplication"_sel);
}

inline void NSApplication_send_event(id app, id event) {
  using namespace objc::literals;
  objc::msg_send<void>(app, "sendEvent:"_sel, event);
}

} // namespace cocoa
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSAPPLICATION_HH
