/*
 * MIT License
 *
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

#ifndef WEBVIEW_DETAIL_THREADING_QUEUE_THREAD_CONSTRUCTOR_CC
#define WEBVIEW_DETAIL_THREADING_QUEUE_THREAD_CONSTRUCTOR_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "detail/engine_base.hh"
#include "detail/engine_queue.hh"
#include "log/trace_log.hh"
#include "strings/string_api.hh"
#include <mutex>

#ifndef WEBVIEW_UNBIND_TIMEOUT
static const unsigned int webview_unbind_timeout = 40;
#else
static const unsigned int webview_unbind_timeout = WEBVIEW_UNBIND_TIMEOUT;
#endif

using namespace webview::log;
using namespace webview::strings;
using namespace webview::detail;

void engine_queue::queue_thread_constructor() {
  std::mutex queue_thread_mtx;
  std::unique_lock<std::mutex> lock(queue_thread_mtx);

  while (!atomic.terminating()) {
    trace::queue.loop.wait(list.queue.size(), list.queue.empty(),
                           atomic.dom.ready());
    cv.queue.wait(lock, [this] {
      return atomic.AND({atomic.dom.ready(), !list.queue.empty()});
    });
    if (atomic.terminating()) {
      return;
    }

    trace::queue.loop.start(list.queue.size());
    auto action = list.queue.front();
    context_t &work_ctx = action.ctx;
    std::string name = action.name_or_js;
    auto &work_fn = action.work_fn;

    // `bind` user work unit
    if (work_ctx == ctx.bind) {
      trace::queue.bind.start(name);
      work_fn();
      trace::queue.bind.wait(name);
      cv.bind.wait(lock, [this] { return atomic.AND({atomic.done.bind()}); });
      if (atomic.terminating()) {
        return;
      }

      trace::queue.bind.done(atomic.done.bind(), name);
      list.pending.pop_front();
      atomic.done.bind(false);
    }

    // `unbind` user work unit
    if (work_ctx == ctx.unbind) {
      trace::queue.unbind.wait(name);
      auto timeout = std::chrono::milliseconds(webview_unbind_timeout);
      cv.unbind_timeout.wait_for(lock, timeout, [this, name] {
        return atomic.AND({list.unresolved_promises.empty(name)});
      });
      if (atomic.terminating()) {
        return;
      }
      trace::queue.unbind.start(name);
      auto promises = list.unresolved_promises.get_copy(name);
      for (auto &id : promises) {
        auto err = string::err.reject_unbound(id, name);
        wv->reject(id, err);
      }

      work_fn();
      cv.unbind.wait(lock,
                     [this] { return atomic.AND({atomic.done.unbind()}); });
      if (atomic.terminating()) {
        return;
      }
      trace::queue.unbind.done(atomic.done.unbind(), name);
      list.pending.pop_front();
      atomic.done.unbind(false);
    }

    // `eval` user work unit
    if (work_ctx == ctx.eval) {
      trace::queue.eval.start();
      work_fn();
      trace::queue.eval.wait();
      cv.eval.wait(lock, [this] { return atomic.AND({atomic.done.eval()}); });
      if (atomic.terminating()) {
        return;
      }
      trace::queue.eval.done(atomic.done.eval());
      atomic.done.eval(false);
    }

    trace::queue.loop.end();
    list.queue.pop_front();
  }
}

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_THREADING_QUEUE_THREAD_CONSTRUCTOR_CC
