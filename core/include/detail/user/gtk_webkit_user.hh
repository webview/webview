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

#ifndef WEBVIEW_DETAIL_USER_GTK_WEBKITGTK_USER_HH
#define WEBVIEW_DETAIL_USER_GTK_WEBKITGTK_USER_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)
#include "detail/platform/linux/gtk/compat.hh"
#include "detail/platform/linux/webkitgtk/compat.hh"
#include "detail/platform/linux/webkitgtk/dmabuf.hh"
#include "detail/user/user_script.hh"

#if GTK_MAJOR_VERSION >= 4
#include <jsc/jsc.h>
#include <webkit/webkit.h>

#elif GTK_MAJOR_VERSION >= 3
#include <JavaScriptCore/JavaScript.h>
#include <webkit2/webkit2.h>

#endif

namespace webview {
namespace detail {
namespace user {

class user_script::impl {
public:
  impl(WebKitUserScript *script) : m_script{script} {
    webkit_user_script_ref(script);
  }

  ~impl() { webkit_user_script_unref(m_script); }

  impl(const impl &) = delete;
  impl &operator=(const impl &) = delete;
  impl(impl &&) = delete;
  impl &operator=(impl &&) = delete;

  WebKitUserScript *get_native() const { return m_script; }

private:
  WebKitUserScript *m_script{};
};

} // namespace user
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_USER_GTK_WEBKITGTK_USER_HH
