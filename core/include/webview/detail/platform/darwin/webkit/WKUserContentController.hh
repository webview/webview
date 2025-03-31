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

#ifndef WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKUSERCONTENTCONTROLLER_HH
#define WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKUSERCONTENTCONTROLLER_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"
#include "literals.hh"

namespace webview {
namespace detail {
namespace webkit {

inline void WKUserContentController_addScriptMessageHandler(id self, id handler,
                                                            id name) {
  objc::msg_send<void>(self, "addScriptMessageHandler:name:"_sel, handler,
                       name);
}

inline void WKUserContentController_addUserScript(id self, id user_script) {
  objc::msg_send<void>(self, "addUserScript:"_sel, user_script);
}

inline void WKUserContentController_removeAllUserScripts(id self) {
  objc::msg_send<id>(self, "removeAllUserScripts"_sel);
}

} // namespace webkit
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_WEBKIT_WKUSERCONTENTCONTROLLER_HH
