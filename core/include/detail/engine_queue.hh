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

#ifndef WEBVIEW_DETAIL_ENGINE_QUEUE_HH
#define WEBVIEW_DETAIL_ENGINE_QUEUE_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "detail/engine_queue_lib.hh"
#include "detail/threading/atomic.hh"
#include "detail/threading/threadsafe_lists.hh"
#include <thread>

using namespace webview::types;
using namespace webview::_lib::_detail;
namespace webview {
namespace detail {

class engine_base;
class engine_queue : protected threading::engine_lists_t,
                     protected threading::atomic_api_t {
public:
  ~engine_queue();
  engine_queue(engine_base *wv);

  /*************************************************************************
  * API nested structure for `engine_queue`.
  *
  * Note:
  * This is a constrained public interface to private internal functionality,
  * which is acceptable and idiomatic in C++ for API design.
  **************************************************************************/
  // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)

  /// API root for the engine_queue class instance.
  queue_api_t queue;

  // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

private:
  struct action_ctx_t {
    context_t bind = context_t::bind_t;
    context_t unbind = context_t::unbind_t;
    context_t eval = context_t::eval_t;
  } ctx{};

  /// @brief Constructs a thread to choreograph execution of `bind`, `unbind` or `eval` user work units.
  ///
  /// @details We do not want user work to stall the main / app thread.
  /// We need user work units to execute sequentially and atomically:
  /// - work must automatically queue until the DOM `__webview__` method is ready,
  /// - `unbind` must only execute after preceding `bind` work is completely registered,
  /// - JS promises must not be left indefinitely in a waiting state,
  /// - `eval` must wait for potentially included bound JS promises to register.
  /// - `unbind` must wait for resolution of relevant JS promises before rejecting them, and for this we set a sane timeout,
  void queue_thread_constructor();

  /// @brief Constructs a child thread for each native work unit of a bound JS promise.
  ///
  /// @details We want native promise work units to run concurrently.
  /// We do not want native promise work to stall execution of the main / app thread.
  void resolve_thread_constructor(std::string name, const std::string &id,
                                  const std::string &args);

  /// Determines if a given binding name will be bound at the time of queue execution.
  bool will_be_bound(const std::string &name) const;

  /// Adds `bind`, `unbind` or `eval` user work unit to the queue.
  noresult queue_work(const std::string &name_or_js, dispatch_fn_t fn,
                      context_t fn_ctx);

  /// A thread to concurrently choreograph user work queueing.
  std::thread queue_thread;

  /// The Webview class instance;
  engine_base *wv;

  friend struct _lib::_detail::bind_api_t;
  friend struct _lib::_detail::unbind_api_t;
  friend struct _lib::_detail::eval_api_t;
  friend struct _lib::_detail::promise_api_t;
  friend struct _lib::_detail::queue_api_t;
};

} // namespace detail
} // namespace webview
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_QUEUE_HH
