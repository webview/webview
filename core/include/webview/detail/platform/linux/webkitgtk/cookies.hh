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

#ifndef WEBVIEW_PLATFORM_LINUX_WEBKITGTK_COOKIES_HH
#define WEBVIEW_PLATFORM_LINUX_WEBKITGTK_COOKIES_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"
#include "../../../../types.hh"
#include "../../../cookie.hh"

#if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)

#include <functional>
#include <string>

#include <gtk/gtk.h>

#if GTK_MAJOR_VERSION >= 4

#include <webkit/webkit.h>

#elif GTK_MAJOR_VERSION >= 3

#include <webkit2/webkit2.h>

#endif

namespace webview {
namespace detail {

typedef struct {
  webview::detail::cookie_data data;
  bool failure{};
  GMainLoop *loop{};
  int pending_ops = 0;
  std::function<void(bool)> on_complete;
} CookieUserData;

/**
 * WebKitGTK cookies helper class.
 */
class webkitgtk_cookies {
public:
  static WebKitCookieManager *get_cookie_manager(WebKitWebView *webview) {
#if GTK_MAJOR_VERSION >= 4
    WebKitNetworkSession *network_session =
        webkit_web_view_get_network_session(webview);
    return webkit_network_session_get_cookie_manager(network_session);
#elif GTK_MAJOR_VERSION >= 3
    WebKitWebContext *web_context = webkit_web_view_get_context(webview);
    return webkit_web_context_get_cookie_manager(web_context);
#endif
    return {};
  }

  static noresult add_cookie(WebKitWebView *webview,
                             const cookie_data &cookieData) {
#if GTK_MAJOR_VERSION >= 3
    WebKitCookieManager *cookie_manager = get_cookie_manager(webview);
    SoupCookie *cookie = soup_cookie_new(
        cookieData.get_name().c_str(), cookieData.get_value().c_str(),
        cookieData.get_domain().c_str(), cookieData.get_path().c_str(),
        cookieData.get_maxage());
    if (!cookie) {
      return error_info{WEBVIEW_ERROR_UNSPECIFIED, "Failed to create cookie"};
    }
    soup_cookie_set_http_only(cookie, cookieData.get_httpOnly());
    soup_cookie_set_secure(cookie, cookieData.get_secure());

    if (cookieData.get_sameSite() ==
        cookie_data::same_site_values::SAME_SITE_LAX) {
      soup_cookie_set_same_site_policy(cookie, SOUP_SAME_SITE_POLICY_LAX);
    } else if (cookieData.get_sameSite() ==
               cookie_data::same_site_values::SAME_SITE_STRICT) {
      soup_cookie_set_same_site_policy(cookie, SOUP_SAME_SITE_POLICY_STRICT);
    } else {
      soup_cookie_set_same_site_policy(cookie, SOUP_SAME_SITE_POLICY_NONE);
    }

    webkit_cookie_manager_add_cookie(
        cookie_manager, cookie, nullptr,
        (GAsyncReadyCallback)webkitgtk_cookies::on_cookie_added_async, nullptr);
    soup_cookie_free(cookie);
#else
#warning "cookies are not implemented for GTK < 3"
#endif
    return {};
  }

  static noresult delete_cookie(WebKitWebView *webview,
                                const cookie_data &cookieData) {
#if GTK_MAJOR_VERSION >= 3
    WebKitCookieManager *cookie_manager =
        webkitgtk_cookies::get_cookie_manager(webview);
    const gchar *uri = webkit_web_view_get_uri(webview);

    CookieUserData *data = new CookieUserData{};
    data->failure = false;
    data->data = cookieData;
    webkit_cookie_manager_get_cookies(
        cookie_manager, uri, nullptr,
        (GAsyncReadyCallback)webkitgtk_cookies::cookies_to_delete_callback,
        data);
#else
#warning "cookies are not implemented for GTK < 3"
#endif
    return {};
  }

#if GTK_MAJOR_VERSION >= 3
  static void on_cookie_added_async(GObject *source_object,
                                    GAsyncResult *result, gpointer user_data) {
    (void)user_data;
    WebKitCookieManager *manager = WEBKIT_COOKIE_MANAGER(source_object);
    webkit_cookie_manager_add_cookie_finish(manager, result, nullptr);
  }

  static void cookies_to_delete_callback(GObject *source_object,
                                         GAsyncResult *result,
                                         void *user_data) {
    CookieUserData *data = static_cast<CookieUserData *>(user_data);
    WebKitCookieManager *cookie_manager = WEBKIT_COOKIE_MANAGER(source_object);
    GError *err = nullptr;
    GList *cookies =
        webkit_cookie_manager_get_cookies_finish(cookie_manager, result, &err);
    if (err) {
      data->failure = true;
      g_error_free(err);
    }
    data->pending_ops = 1;
    for (GList *l = cookies; l != nullptr; l = l->next) {
      SoupCookie *cookie = static_cast<SoupCookie *>(l->data);
      if (soup_cookie_get_name(cookie) == data->data.get_name() &&
          soup_cookie_get_domain(cookie) == data->data.get_domain()) {
        ++data->pending_ops;
        webkit_cookie_manager_delete_cookie(
            cookie_manager, cookie, nullptr,
            (GAsyncReadyCallback)webkitgtk_cookies::delete_cookie_callback,
            data);
      }
      soup_cookie_free(cookie);
    }
    g_list_free(cookies);
    if (--data->pending_ops == 0) {
      finish_cookie_op(data);
    }
  }

  static void delete_cookie_callback(GObject *source_object,
                                     GAsyncResult *result, void *user_data) {
    CookieUserData *data = static_cast<CookieUserData *>(user_data);
    WebKitCookieManager *cookie_manager = WEBKIT_COOKIE_MANAGER(source_object);
    GError *err = nullptr;
    webkit_cookie_manager_delete_cookie_finish(cookie_manager, result, &err);
    if (err) {
      data->failure = true;
      g_error_free(err);
    }
    if (--data->pending_ops == 0) {
      finish_cookie_op(data);
    }
  }

  // Called by every async-ready callback when all pending ops are done.
  static void finish_cookie_op(CookieUserData *data) {
    if (data->on_complete) {
      data->on_complete(data->failure);
    }
    delete data;
  }
#endif
};

} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_GTK)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_LINUX_WEBKITGTK_COOKIES_HH
