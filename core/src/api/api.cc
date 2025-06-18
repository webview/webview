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

#ifndef WEBVIEW_API_CC
#define WEBVIEW_API_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "api/api.h"
#include "api/api_lib.hh"
#include "errors/errors.h"
#include "log/console_log.hh"
#include "strings/string_api.hh"
#include "types/types.hh"

using namespace webview::strings;
using namespace webview::log;
using namespace webview::_lib::_api;

/* PUBLIC C API implementations
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

WEBVIEW_API webview_t webview_create(int debug, void *wnd) {
  webview_cc *w{};
  auto err = api_filter(
      [=]() -> result<webview_cc *> {
        return new webview_cc{static_cast<bool>(debug), wnd};
      },
      [&](webview_cc *w_) { w = w_; });
  if (err == WEBVIEW_ERROR_OK) {
    return w;
  }
  return nullptr;
}

WEBVIEW_DEPRECATED(DEPRECATE_WEBVIEW_DESTROY)
WEBVIEW_API webview_error_t webview_destroy(webview_t w) {
  console.warn("Use of deprecated API call `webview_destroy`");
  if (terminate_destroy_controller.terminated) {
    return {};
  };
  if (!w) {
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT,
                      string::err.api_null_w("webview_destroy"), true}
        .code();
  }
  auto res = webview_terminate(w);
  terminate_destroy_controller.destroyed = true;
  return res;
}

WEBVIEW_API webview_error_t webview_run(webview_t w) {
  if (!w) {
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT,
                      string::err.api_null_w("webview_run"), true}
        .code();
  }

  return api_filter([=] { return cast_to_webview(w)->run(); });
}

WEBVIEW_API webview_error_t webview_terminate(webview_t w) {
  if (terminate_destroy_controller.destroyed) {
    return {};
  };
  terminate_destroy_controller.terminated = true;
  if (!w) {
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT,
                      string::err.api_null_w("webview_terminate"), true}
        .code();
  }

  return api_filter([=] { return cast_to_webview(w)->terminate(); });
}

WEBVIEW_DEPRECATED(DEPRECATE_WEBVIEW_DISPATCH)
WEBVIEW_API webview_error_t webview_dispatch(webview_t w,
                                             void (*fn)(webview_t, void *),
                                             void *arg) {
  if (!w || !fn) {
    auto message = !w ? string::err.api_null_w("webview_dispatch")
                      : string::err.api_missing_arg("fn", "webview_dispatch");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  IGNORE_DEPRECATED_DECLARATIONS
  return api_filter(
      [=] { return cast_to_webview(w)->dispatch([=]() { fn(w, arg); }); });
  RESTORE_IGNORED_WARNINGS
}

WEBVIEW_API void *webview_get_window(webview_t w) {
  if (!w) {
    console.error(string::err.api_null_w("webview_get_window"),
                  WEBVIEW_ERROR_INVALID_ARGUMENT);
    return nullptr;
  }
  void *window = nullptr;
  auto err = api_filter([=] { return cast_to_webview(w)->window(); },
                        [&](void *value) { window = value; });
  if (err == WEBVIEW_ERROR_OK) {
    return window;
  }
  return nullptr;
}

WEBVIEW_API void *webview_get_native_handle(webview_t w,
                                            webview_native_handle_kind_t kind) {
  if (!w) {
    console.error(string::err.api_null_w("webview_get_native_handle"),
                  WEBVIEW_ERROR_INVALID_ARGUMENT);
    return nullptr;
  }
  void *handle{};
  auto err = api_filter(
      [=]() -> result<void *> {
        auto *w_ = cast_to_webview(w);
        switch (kind) {
        case WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW:
          return w_->window();
        case WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET:
          return w_->widget();
        case WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER:
          return w_->browser_controller();
        default:
          return error_info{
              WEBVIEW_ERROR_INVALID_ARGUMENT,
              string::err.api_invalid_arg("kind", "webview_get_native_handle")};
        }
      },
      [&](void *handle_) { handle = handle_; });
  if (err == WEBVIEW_ERROR_OK) {
    return handle;
  }
  return nullptr;
}

WEBVIEW_API webview_error_t webview_set_title(webview_t w, const char *title) {
  if (!w || !title) {
    auto message =
        !w ? string::err.api_null_w("webview_set_title")
           : string::err.api_missing_arg("title", "webview_set_title");

    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter([=] { return cast_to_webview(w)->set_title(title); });
}

WEBVIEW_API webview_error_t webview_set_size(webview_t w, int width, int height,
                                             webview_hint_t hints) {
  if (!w) {
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT,
                      string::err.api_null_w("webview_set_size"), true}
        .code();
  }
  return api_filter(
      [=] { return cast_to_webview(w)->set_size(width, height, hints); });
}

WEBVIEW_API webview_error_t webview_navigate(webview_t w, const char *url) {
  if (!w || !url) {
    auto message = !w ? string::err.api_null_w("webview_navigate")
                      : string::err.api_missing_arg("url", "webview_navigate");

    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }
  if (!url) {
    console.error("Missing `url` argument to `webview_navigate`",
                  WEBVIEW_ERROR_INVALID_ARGUMENT);
    return WEBVIEW_ERROR_INVALID_ARGUMENT;
  }
  return api_filter([=] { return cast_to_webview(w)->navigate(url); });
}

WEBVIEW_API webview_error_t webview_set_html(webview_t w, const char *html) {
  if (!w || !html) {
    auto message = !w ? string::err.api_null_w("webview_set_html")
                      : string::err.api_missing_arg("html", "webview_set_html");

    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter([=] { return cast_to_webview(w)->set_html(html); });
}

WEBVIEW_API webview_error_t webview_init(webview_t w, const char *js) {
  if (!w || !js) {
    auto message = !w ? string::err.api_null_w("webview_init")
                      : string::err.api_missing_arg("js", "webview_init");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter([=] { return cast_to_webview(w)->init(js); });
}

WEBVIEW_API webview_error_t webview_eval(webview_t w, const char *js) {
  if (!w || !js) {
    auto message = !w ? string::err.api_null_w("webview_eval")
                      : string::err.api_missing_arg("js", "webview_eval");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter([=] { return cast_to_webview(w)->eval(js); });
}

WEBVIEW_API webview_error_t webview_bind(webview_t w, const char *name,
                                         void (*fn)(const char *id,
                                                    const char *req, void *arg),
                                         void *arg) {
  if (!w || !name || !fn) {
    auto message = !w      ? string::err.api_null_w("webview_bind")
                   : !name ? string::err.api_missing_arg("name", "webview_bind")
                           : string::err.api_missing_arg("fn", "webview_bind");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
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
  if (!w || !name) {
    auto message = !w ? string::err.api_null_w("webview_unbind")
                      : string::err.api_missing_arg("name", "webview_unbind");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter([=] { return cast_to_webview(w)->unbind(name); });
}

WEBVIEW_API webview_error_t webview_return(webview_t w, const char *id,
                                           int status, const char *result) {
  if (!w || !id || !result) {
    auto message =
        !w    ? string::err.api_null_w("webview_return")
        : !id ? string::err.api_missing_arg("id", "webview_return")
              : string::err.api_missing_arg("result", "webview_return");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter(
      [=] { return cast_to_webview(w)->resolve(id, status, result); });
}
WEBVIEW_API webview_error_t webview_return_int(webview_t w, const char *id,
                                               int status, int result) {
  if (!w || !id) {
    auto message = !w ? string::err.api_null_w("webview_return")
                      : string::err.api_missing_arg("id", "webview_return");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter(
      [=] { return cast_to_webview(w)->resolve(id, status, result); });
}
WEBVIEW_API webview_error_t webview_return_float(webview_t w, const char *id,
                                                 int status, float result) {
  if (!w || !id) {
    auto message = !w ? string::err.api_null_w("webview_return")
                      : string::err.api_missing_arg("id", "webview_return");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter(
      [=] { return cast_to_webview(w)->resolve(id, status, result); });
}
WEBVIEW_API webview_error_t webview_return_bool(webview_t w, const char *id,
                                                int status, bool result) {
  if (!w || !id) {
    auto message = !w ? string::err.api_null_w("webview_return")
                      : string::err.api_missing_arg("id", "webview_return");
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, message, true}.code();
  }

  return api_filter(
      [=] { return cast_to_webview(w)->resolve(id, status, result); });
}

WEBVIEW_API const webview_version_info_t *webview_version(void) {
  return &library_version_info;
}

WEBVIEW_API webview_error_t json_parse(char **buffer, const char *json_str,
                                       const char *key, int index) {
  std::string parsed_res;
  auto err_mess = "Failed to JSON parse the string";
  try {
    parsed_res = json::parse(json_str, key, index);
  } catch (...) {
    console.error(err_mess, WEBVIEW_ERROR_UNSPECIFIED);
    return WEBVIEW_ERROR_UNSPECIFIED;
  }
  return alloc_string_buffer(buffer, parsed_res, err_mess);
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * PUBLIC C API implementations
 * ----------------------------------------------------------------------------------------------------------- 
 * API lib implementations (api/api_lib.hh)
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

template <typename WorkFn, typename ResultFn>
webview_error_t
webview::_lib::_api::api_filter(WorkFn &&do_work,
                                ResultFn &&put_result) noexcept {
  try {
    auto result = do_work();
    if (result.ok()) {
      put_result(result.value());
      return WEBVIEW_ERROR_OK;
    }
    auto message = result.error().message();
    message = message.empty() ? "Unspecified API error." : message;
    console.error(message, result.error().code());
    return result.error().code();
  } catch (const exception &e) {
    auto message = e.error().message();
    message = message.empty() ? "Unspecified API error." : message;
    console.error(message, e.error().code());
    return e.error().code();
  } catch (...) {
    console.error("Unspecified API error.", WEBVIEW_ERROR_UNSPECIFIED);
    return WEBVIEW_ERROR_UNSPECIFIED;
  }
}

template <typename WorkFn>
webview_error_t webview::_lib::_api::api_filter(WorkFn &&do_work) noexcept {
  try {
    auto result = do_work();
    if (result.ok()) {
      return WEBVIEW_ERROR_OK;
    }
    auto message = result.error().message();
    message = message.empty() ? "Unspecified API error." : message;
    console.error(message, result.error().code());
    return result.error().code();
    return result.error().code();
  } catch (const exception &e) {
    auto message = e.error().message();
    message = message.empty() ? "Unspecified API error." : message;
    console.error(message, e.error().code());
    return e.error().code();
  } catch (...) {
    console.error("Unspecified API error.", WEBVIEW_ERROR_UNSPECIFIED);
    return WEBVIEW_ERROR_UNSPECIFIED;
  }
}

inline webview_cc *webview::_lib::_api::cast_to_webview(void *w) {
  if (!w) {
    throw exception{WEBVIEW_ERROR_INVALID_ARGUMENT,
                    "Cannot cast null pointer to webview instance"};
  }
  return static_cast<webview_cc *>(w);
}

IGNORE_UNUSED_PARAMETERS
// NOLINTBEGIN(misc-unused-parameters)
webview_error_t
webview::_lib::_api::alloc_string_buffer(char **buffer, const std::string &str,
                                         const std::string &err_mess) {
  auto size = str.size() + 1;
  // NOLINTNEXTLINE(hicpp-no-malloc, cppcoreguidelines-no-malloc)
  *buffer = static_cast<char *>(malloc(size));
#ifdef _MSC_VER
  auto res = strcpy_s(*buffer, size, str.c_str());
  if (res != 0) {
    console.error(std::string("MSVC: ") + err_mess, WEBVIEW_ERROR_UNSPECIFIED);
    return WEBVIEW_ERROR_UNSPECIFIED;
  }
#else
  strcpy(*buffer, str.c_str());
#endif
  return WEBVIEW_ERROR_OK;
}
// NOLINTEND(misc-unused-parameters)
RESTORE_IGNORED_WARNINGS

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * API lib implementations */

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_API_CC
