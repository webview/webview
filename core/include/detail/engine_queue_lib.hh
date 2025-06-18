/*
 * MIT License
 *
 * Copyright (c) 2022 Steffen André Langnes
 * Copyright (c) 2025 Michael Jonker 
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

#ifndef WEBVIEW_DETAIL_ENGINE_QUEUE_LIB_HH
#define WEBVIEW_DETAIL_ENGINE_QUEUE_LIB_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "types/types.hh"

using namespace webview::types;
namespace webview {
namespace detail {
// forward declare the `engine_queue` class
class engine_queue;
} // namespace detail
namespace _lib {
namespace _detail {
using engine_queue = webview::detail::engine_queue;
/// Nested API structure for bind operations
struct bind_api_t : nested_api_t<engine_queue> {
  bind_api_t(engine_queue *self) : nested_api_t(self) {}

  /// Puts a user `bind` work unit onto the queue.
  noresult enqueue(dispatch_fn_t fn, const std::string &name);
  /// Indicates if adding a `bind` to the queue is an error, eg. duplicate name.
  bool is_duplicate(const std::string &name) const;
};
/// Nested API structure for unbind operations
struct unbind_api_t : nested_api_t<engine_queue> {
  unbind_api_t(engine_queue *self) : nested_api_t(self) {}

  /// Puts a user `unbind` work unit onto the queue.
  noresult enqueue(dispatch_fn_t fn, const std::string &name);
  /// Indicates if adding an `unbind` to the queue is an error, eg. bind doesn't exist.
  bool not_found(const std::string &name) const;
};
/// Nested API structure for eval operations
struct eval_api_t : nested_api_t<engine_queue> {
  eval_api_t(engine_queue *self) : nested_api_t(self) {}

  /// Puts a user `eval` work unit onto the queue.
  noresult enqueue(dispatch_fn_t fn, const std::string &js);
};
/// Nested API structure for promise operations
struct promise_api_t : nested_api_t<engine_queue> {
  promise_api_t(engine_queue *self) : nested_api_t(self) {}

  /// Takes queue action for a resolved promise
  void resolving(const std::string &name, const std::string &id);
  /// Sends the native work unit of a promise to a concurrent thread.
  void resolve(const std::string &name, const std::string &id,
               const std::string &args);
  /// Relays notifications from the frontend to relevant queue methods.
  bool exec_system_message(const std::string &id, const std::string &method);
};

/// Root API for engine_queue operations
struct queue_api_t : nested_api_t<engine_queue> {
  queue_api_t(engine_queue *self) : nested_api_t{self} {};

  //API for bind operations
  bind_api_t bind{this->self};
  //API for unbind operations
  unbind_api_t unbind{this->self};
  //API for eval operations
  eval_api_t eval{this->self};
  //API for promise operations
  promise_api_t promises{this->self};
  /// Shuts down the queue loop and release resources
  void terminate();
};

} // namespace _detail
} // namespace _lib
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_QUEUE_LIB_HH
