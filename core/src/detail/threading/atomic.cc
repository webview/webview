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

#ifndef WEBVIEW_DETAIL_THREADING_ATOMIC_CC
#define WEBVIEW_DETAIL_THREADING_ATOMIC_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "detail/threading/atomic.hh"

using namespace webview::detail::threading;
using namespace webview::_lib::_detail::_threading;

/* Nested_API_lib
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* Nested API to get and set if the Webview JS and window DOM is ready
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

bool atomic_dom_ready_t::webview_ready() const {
  return self->is_webview_ready.load();
};
void atomic_dom_ready_t::webview_ready(bool flag) {
  self->is_webview_ready.store(flag);
};
bool atomic_dom_ready_t::ready() const { return self->is_dom_ready.load(); };
void atomic_dom_ready_t::ready(bool flag) {
  self->is_dom_ready.store(flag);
  self->cv.queue.notify_one();
};

/* Nested API to get and set if various queue operations have completed
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

bool atomic_done_t::bind() const { return self->bind_done.load(); }
void atomic_done_t::bind(bool val) {
  self->bind_done.store(val);
  self->cv.bind.notify_one();
}
bool atomic_done_t::unbind() const { return self->unbind_done.load(); }
void atomic_done_t::unbind(bool val) {
  self->unbind_done.store(val);
  self->cv.unbind.notify_one();
}
bool atomic_done_t::eval() const { return self->eval_done.load(); }
void atomic_done_t::eval(bool val) {
  self->eval_done.store(val);
  self->cv.eval.notify_one();
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆  
   * Nested_API_lib
 * -----------------------------------------------------------------------------------------------------------
 * Root API 
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

bool atomic_api_t::api_root_t::terminating() const {
  return self->is_terminating.load();
};
bool atomic_api_t::api_root_t::AND(std::initializer_list<bool> flags) const {
  if (self->atomic.terminating()) {
    return true;
  };
  auto res = true;
  for (auto &flag : flags) {
    if (!flag) {
      res = false;
      break;
    }
  }
  return res;
};

atomic_api_t::cv_api_t::cv_api_t()
    : all{&queue, &bind, &eval, &unbind, &unbind_timeout} {};
void atomic_api_t::cv_api_t::notify_all() {
  for (auto &this_cv : all) {
    this_cv->notify_all();
  }
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * Root API */

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_THREADING_ATOMIC_CC
