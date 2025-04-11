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

#pragma once

#include "engine_base.h"
#include "../errors.h"

#include <promise/promise.h>
#include <json/json.h>

#include <exception>
#include <format>
#include <string_view>
#include <tuple>

namespace webview {

template <class Promise>
void
Webview::bind(std::string_view name, Promise&& promise) {
   if (!bindings_
           .emplace(
              name,
              std::make_shared<binding_t>([this, promise = std::forward<Promise>(promise)](std::string_view id, std::string_view js_args) {
                 using return_t = promise::return_t<promise::return_t<decltype(promise)>>;
                 using args_t   = promise::args_t<decltype(promise)>;

                 try {
                    auto args = [&]() constexpr {
                       if constexpr (std::tuple_size_v<args_t>) {
                          return js::parse<args_t>(js_args);
                       } else {
                          return std::tuple{};
                       }
                    }();

                    std::apply([&]<class... Args>(Args&&... args) constexpr {
                       if constexpr (std::is_void_v<return_t>) {
                          make_promise(promise, std::forward<Args>(args)...)
                             .then([id = std::string{id}, this]() -> ::Promise<void> {
                                co_return dispatch([id = js::serialize(id), this]() {
                                   eval("window.__webview__.onReply(" + id + ", 0, undefined)");
                                });
                             })
                             .catch_([id = std::string{id}, this](js::serializable_exception const& exc) -> ::Promise<void> {
                                co_return dispatch([id = js::serialize(id), exception = exc.serialize(), this]() {
                                   eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize(exception) + ")");
                                });
                             })
                             .catch_([id = std::string{id}, this](std::exception const& exc) -> ::Promise<void> {
                                co_return dispatch([id = js::serialize(id), exception = js::serialize<std::string_view>(exc.what()), this]() {
                                   eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize(exception) + ")");
                                });
                             })
                             .catch_([id = std::string{id}, this](std::exception_ptr) -> ::Promise<void> {
                                co_return dispatch([id = js::serialize(id), this]() {
                                   eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize<std::string_view>(R"("unknown exception")") + ")");
                                });
                             })
                             .detach();

                       } else {
                          make_promise(promise, std::forward<Args>(args)...)
                             .then([id{std::string{id}}, this](return_t const& result) -> ::Promise<void> {
                                co_return dispatch([id = js::serialize(id), result = js::serialize(result), this]() {
                                   eval("window.__webview__.onReply(" + id + ", 0, " + js::serialize(result) + ")");
                                });
                             })
                             .catch_([id{std::string{id}}, this](js::serializable_exception const& exc) -> ::Promise<void> {
                                co_return dispatch([id = js::serialize(id), exception = exc.serialize(), this]() {
                                   eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize(exception) + ")");
                                });
                             })
                             .catch_([id = std::string{id}, this](std::exception const& exc) -> ::Promise<void> {
                                co_return dispatch([id = js::serialize(id), exception = js::serialize<std::string_view>(exc.what()), this]() {
                                   eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize(exception) + ")");
                                });
                             })
                             .catch_([id = std::string{id}, this](std::exception_ptr) -> ::Promise<void> {
                                co_return dispatch([id = js::serialize(id), this]() {
                                   eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize<std::string_view>(R"("unknown exception")") + ")");
                                });
                             })
                             .detach();
                       }
                    },
                               args);
                 } catch (js::serializable_exception const& exc) {
                    dispatch([id = js::serialize(id), exception = exc.serialize(), this]() {
                       eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize(exception) + ")");
                    });
                 } catch (std::exception const& exc) {
                    dispatch([id = js::serialize(id), exception = js::serialize(std::string_view{exc.what()}), this]() {
                       eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize(exception) + ")");
                    });
                 } catch (...) {
                    dispatch([id = js::serialize(id), this]() {
                       eval("window.__webview__.onReply(" + id + ", 1, " + js::serialize<std::string_view>(R"("unknown exception")") + ")");
                    });
                 }
              })
           )
           .second) {
      throw exception(error_t::WEBVIEW_ERROR_DUPLICATE, name);
   }

   replace_bind_script();

   // Notify that a binding was created if the init script has already
   // set things up.
   eval(std::format(R"(if (window.__webview__) {{
       window.__webview__.onBind({})
    }})",
                    js::serialize(name)));
}
}  // namespace webview
