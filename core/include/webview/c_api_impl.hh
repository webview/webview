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

#ifndef WEBVIEW_C_API_IMPL_HH
#define WEBVIEW_C_API_IMPL_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "backends.hh"
#include "errors.h"
#include "errors.hh"
#include "json_deprecated.hh"
#include "macros.h"
#include "types.h"
#include "utility/console.hh"
#include "utility/info.hh"
#include "version.h"

using namespace webview::utility;

namespace webview {
namespace detail {

// The library's version information.
constexpr const webview_version_info_t library_version_info{
    {WEBVIEW_VERSION_MAJOR, WEBVIEW_VERSION_MINOR, WEBVIEW_VERSION_PATCH},
    WEBVIEW_VERSION_NUMBER,
    WEBVIEW_VERSION_PRE_RELEASE,
    WEBVIEW_VERSION_BUILD_METADATA};

template <typename WorkFn, typename ResultFn>
webview_error_t api_filter(WorkFn &&do_work, ResultFn &&put_result) noexcept {
  try {
    auto result = do_work();
    if (result.ok()) {
      put_result(result.value());
      return WEBVIEW_ERROR_OK;
    }
    return result.error().code();
  } catch (const exception &e) {
    auto err = e.error().code();
    auto message = "Webview threw an exception while fetching and returning a "
                   "value from a Webview public API call";
    console::error(message, err);
    return err;
  } catch (...) {
    /* 
     * TODO
     * Platform errors are presently not directed to C/C++ exceptions, so this block is likely to 
     * never be reached until platform exception handling is improved elsewhere.
     */
    auto message = info::os_type() +
                   " threw an exception while fetching and "
                   "returning a value from a Webview public API call";
    console::error(message);
    return WEBVIEW_ERROR_UNSPECIFIED;
  }
}

template <typename WorkFn>
webview_error_t api_filter(WorkFn &&do_work) noexcept {
  try {
    auto result = do_work();
    if (result.ok()) {
      return WEBVIEW_ERROR_OK;
    }
    return result.error().code();
  } catch (const exception &e) {
    auto err = e.error().code();
    auto message =
        "Webview threw an exception while executing a Webview public API call";
    console::error(message, err);
    return err;
  } catch (...) {
    /* 
     * TODO
     * Platform errors are presently not directed to C/C++ exceptions, so this block is likely to 
     * never be reached until platform exception handling is improved elsewhere.
     */
    auto message =
        info::os_type() +
        " threw an exception while executing a Webview public API call";
    console::error(message);
    return WEBVIEW_ERROR_UNSPECIFIED;
  }
}

inline webview *cast_to_webview(void *w) {
  if (!w) {
    auto message = "Cannot cast null pointer to webview instance";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    throw exception{err, message};
  }
  return static_cast<webview *>(w);
}

} // namespace detail
} // namespace webview

WEBVIEW_API webview_t webview_create(int debug, void *wnd) {
  using namespace webview::detail;
  console::attach_console("webview_create");
  webview::webview *w{};
  auto err = api_filter(
      [=]() -> webview::result<webview::webview *> {
        return new webview::webview{static_cast<bool>(debug), wnd};
      },
      [&](webview::webview *w_) { w = w_; });
  if (err == WEBVIEW_ERROR_OK) {
    return w;
  }
  console::error("`webview_create`", err);
  return nullptr;
}

WEBVIEW_API webview_error_t webview_destroy(webview_t w) {
  using namespace webview::detail;
  return api_filter([=]() -> webview::noresult {
    delete cast_to_webview(w);
    return {};
  });
}

WEBVIEW_API webview_error_t webview_run(webview_t w) {
  using namespace webview::detail;
  return api_filter([=] { return cast_to_webview(w)->run(); });
}

WEBVIEW_API webview_error_t webview_terminate(webview_t w) {
  using namespace webview::detail;
  return api_filter([=] { return cast_to_webview(w)->terminate(); });
}

WEBVIEW_API webview_error_t webview_dispatch(webview_t w,
                                             void (*fn)(webview_t, void *),
                                             void *arg) {
  using namespace webview::detail;
  if (!fn) {
    auto message = "Invalid argument `fn` passed to `webview_dispatch`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
  }
  return api_filter(
      [=] { return cast_to_webview(w)->dispatch([=]() { fn(w, arg); }); });
}

WEBVIEW_API void *webview_get_window(webview_t w) {
  using namespace webview::detail;
  void *window = nullptr;
  auto err = api_filter([=] { return cast_to_webview(w)->window(); },
                        [&](void *value) { window = value; });
  if (err == WEBVIEW_ERROR_OK) {
    return window;
  }
  console::error("`webview_get_window`", err);
  return nullptr;
}

WEBVIEW_API void *webview_get_native_handle(webview_t w,
                                            webview_native_handle_kind_t kind) {
  using namespace webview::detail;
  void *handle{};
  auto err = api_filter(
      [=]() -> webview::result<void *> {
        auto *w_ = cast_to_webview(w);
        switch (kind) {
        case WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW:
          return w_->window();
        case WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET:
          return w_->widget();
        case WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER:
          return w_->browser_controller();
        default:
          return webview::error_info{WEBVIEW_ERROR_INVALID_ARGUMENT};
        }
      },
      [&](void *handle_) { handle = handle_; });
  if (err == WEBVIEW_ERROR_OK) {
    return handle;
  }
  console::error("`webview_get_native_handle`", err);
  return nullptr;
}

WEBVIEW_API webview_error_t webview_set_title(webview_t w, const char *title) {
  using namespace webview::detail;
  if (!title) {
    auto message = "Invalid argument `title` passed to `webview_set_title`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
  }
  return api_filter([=] { return cast_to_webview(w)->set_title(title); });
}

WEBVIEW_API webview_error_t webview_set_size(webview_t w, int width, int height,
                                             webview_hint_t hints) {
  using namespace webview::detail;
  return api_filter(
      [=] { return cast_to_webview(w)->set_size(width, height, hints); });
}

WEBVIEW_API webview_error_t webview_navigate(webview_t w, const char *url) {
  using namespace webview::detail;
  if (!url) {
    auto message = "Invalid argument `url` passed to `webview_navigate`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
  }
  return api_filter([=] { return cast_to_webview(w)->navigate(url); });
}

WEBVIEW_API webview_error_t webview_set_html(webview_t w, const char *html) {
  using namespace webview::detail;
  if (!html) {
    auto message = "Invalid argument `html` passed to `webview_set_html`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
  }
  return api_filter([=] { return cast_to_webview(w)->set_html(html); });
}

WEBVIEW_API webview_error_t webview_init(webview_t w, const char *js) {
  using namespace webview::detail;
  if (!js) {
    auto message = "Invalid argument `js` passed to `webview_init`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
  }
  return api_filter([=] { return cast_to_webview(w)->init(js); });
}

WEBVIEW_API webview_error_t webview_eval(webview_t w, const char *js) {
  using namespace webview::detail;
  if (!js) {
    auto message = "Invalid argument `js` passed to `webview_eval`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
  }
  return api_filter([=] { return cast_to_webview(w)->eval(js); });
}

WEBVIEW_API webview_error_t webview_bind(webview_t w, const char *name,
                                         void (*fn)(const char *id,
                                                    const char *req, void *arg),
                                         void *arg) {
  using namespace webview::detail;
  if (!name || !fn) {
    std::string invalid_arg;
    if (!name && !fn) {
      invalid_arg = "s `name` and `fn`";
    } else if (!name) {
      invalid_arg = " `name`";
    } else if (!fn) {
      invalid_arg = " `fn`";
    }
    auto message =
        "Invalid argument" + invalid_arg + " passed to `webview_eval`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
  }
  return api_filter([=] {
    return cast_to_webview(w)->bind(
        name,
        [=](const std::string &seq, const std::string &req, void *arg_) {
          fn(seq.c_str(), req.c_str(), arg_);
        },
        arg);
  });
}

WEBVIEW_API webview_error_t webview_unbind(webview_t w, const char *name) {
  using namespace webview::detail;
  if (!name) {
    auto message = "Invalid argument `name` passed to `webview_unbind`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
  }
  return api_filter([=] { return cast_to_webview(w)->unbind(name); });
}

WEBVIEW_API webview_error_t webview_return(webview_t w, const char *id,
                                           int status, const char *result) {
  using namespace webview::detail;
  if (!id || !result) {
    std::string invalid_arg;
    if (!id && !result) {
      invalid_arg = "s `id` and `result`";
    } else if (!id) {
      invalid_arg = " `id`";
    } else if (!result) {
      invalid_arg = " `result`";
    }
    auto message =
        "Invalid argument" + invalid_arg + " passed to `webview_return`";
    auto err = WEBVIEW_ERROR_INVALID_ARGUMENT;
    console::error(message, err);
    return err;
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter(
      [=] { return cast_to_webview(w)->resolve(id, status, result); });
}

WEBVIEW_API const webview_version_info_t *webview_version(void) {
  return &webview::detail::library_version_info;
}

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_C_API_IMPL_HH
