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

#ifndef WEBVIEW_DETAIL_ENGINE_QUEUE_CC
#define WEBVIEW_DETAIL_ENGINE_QUEUE_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "detail/engine_queue.hh"
#include "detail/threading/thread_detector.hh"
#include "errors/errors.h"
#include "log/trace_log.hh"
#include "strings/string_api.hh"
#include <cstdio>

using namespace webview::types;
using namespace webview::strings;
using namespace webview::log;
using namespace webview::_lib::_detail;

/* Nested_API_lib
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* Nested API for bind operations
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

noresult bind_api_t::enqueue(dispatch_fn_t fn, const std::string &name) {
  return self->queue_work(name, fn, self->ctx.bind);
};
bool bind_api_t::is_duplicate(const std::string &name) const {
  return self->will_be_bound(name);
};

/* Nested API for unbind operations
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

bool unbind_api_t::not_found(const std::string &name) const {
  return !self->will_be_bound(name);
};
noresult unbind_api_t::enqueue(dispatch_fn_t fn, const std::string &name) {
  return self->queue_work(name, fn, self->ctx.unbind);
};

/* Nested API for eval operations
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

noresult eval_api_t::enqueue(dispatch_fn_t fn, const std::string &js) {
  return self->queue_work(js, fn, self->ctx.eval);
};

/* Nested API for promise operations
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void promise_api_t::resolving(const std::string &name, const std::string &id) {
  self->list.unresolved_promises.remove_id(name, id);
  if (self->list.unresolved_promises.empty(name)) {
    self->cv.unbind_timeout.notify_one();
    self->list.unresolved_promises.erase(name);
  }
};
void promise_api_t::resolve(const std::string &name, const std::string &id,
                            const std::string &args) {
  self->list.id_name_map.set(id, name);
  self->list.unresolved_promises.add_id(name, id);
  self->cv.unbind_timeout.notify_one();
  // We send the user defined native callback work to a detached thread.
  // @todo High frequency user operations may consume all the process threads, thus degrading overall performance.
  // Consider:
  // - Thread management and pooling
  // - Upgrade Webview to C++20 to allow thread termination on promise rejection or termination (`std::stop_source`, `std::jthread`)
  // - Explore offloading native user work to Wasm in the browser.
  std::thread resolver = std::thread(&engine_queue::resolve_thread_constructor,
                                     self, name, id, args);
  // We daemonise (detach) the thread because C++11 has no mechanism to terminate threaded code execution,
  // thus we cannot join the thread at termination or promise rejection.
  resolver.detach();
}
bool promise_api_t::exec_system_message(const std::string &id,
                                        const std::string &method) {
  if (id != string::flags.sysop) {
    return false;
  };
  if (method == string::ops.webview_ready) {
    trace::queue.notify.on_message(method);
    self->atomic.dom.webview_ready(true);
  }
  if (method == string::ops.dom_ready) {
    trace::queue.notify.on_message(method);
    self->atomic.dom.ready(true);
  }
  if (method == string::ops.bind_done) {
    trace::queue.notify.on_message(method);
    self->atomic.done.bind(true);
  }
  if (method == string::ops.unbind_done) {
    trace::queue.notify.on_message(method);
    self->atomic.done.unbind(true);
  }
  if (method == string::ops.js_eval_start) {
    trace::queue.notify.on_message(method);
    self->atomic.done.eval(true);
  }
  return true;
}

/* Root API for engine_queue operations
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void queue_api_t::terminate() {
  self->is_terminating.store(true);
  self->cv.notify_all();
  if (!self->queue_thread.joinable()) {
    throw exception{WEBVIEW_ERROR_UNSPECIFIED, "\
Could not join the queue thread.\n\
This is an issue with Webview. Please report it at https://github.com/webview/webview/issues"};
  }
};

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆  
 * Nested_API_lib
 * -----------------------------------------------------------------------------------------------------------
 * PUBLIC 
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* Constructor / Destructor
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

engine_queue::~engine_queue() { queue_thread.join(); }
engine_queue::engine_queue(engine_base *wv) : queue{this}, wv(wv) {
  queue_thread = std::thread(&engine_queue::queue_thread_constructor, this);
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * PUBLIC
 * ----------------------------------------------------------------------------------------------------------- 
 * PRIVATE 
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

bool engine_queue::will_be_bound(const std::string &name) const {
  if (thread::is_main_thread() && list.bindings.count(name) > 0) {
    return true;
  }
  auto i = list.pending.indices(name);
  auto is_bound = list.bindings.count(name) > 0;
  if (is_bound) {
    auto will_be_unbound = i.unbind_i > -1 && i.unbind_i > i.bind_i;
    return !will_be_unbound;
  } else {
    auto will_be_bound = i.bind_i > -1 && i.bind_i > i.unbind_i;
    return will_be_bound;
  };
};
noresult engine_queue::queue_work(const std::string &name_or_js,
                                  dispatch_fn_t fn, context_t fn_ctx) {
  const auto &name = name_or_js;
  if (fn_ctx == ctx.bind) {
    list.pending.push_back("bind-" + name);
  }
  if (fn_ctx == ctx.unbind) {
    list.pending.push_back("unbind-" + name);
  }
  list.queue.push_back(fn_ctx, fn, name_or_js);
  trace::queue.enqueue.added(char(fn_ctx), list.queue.size(), name_or_js);
  cv.queue.notify_one();
  return {};
};

  /* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
   * PRIVATE */

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_ENGINE_QUEUE_CC
