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

#ifndef WEBVIEW_DETAIL_THREADING_RESOLVE_THREAD_CONSTRUCTOR_CC
#define WEBVIEW_DETAIL_THREADING_RESOLVE_THREAD_CONSTRUCTOR_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "detail/engine_base.hh"
#include "detail/engine_queue.hh"
#include "strings/string_api.hh"

using namespace webview::detail;
using namespace webview::strings;

void engine_queue::resolve_thread_constructor(std::string name,
                                              const std::string &id,
                                              const std::string &args) {
  if (atomic.terminating()) {
    return;
  }
  try {
    list.bindings.at(name).call(id, args);
  } catch (const std::exception &err_) {
    if (atomic.terminating()) {
      return;
    }
    auto err = string::err.uncaught_exception(name, err_.what());
    wv->reject(id, err);
  } catch (...) {
    perror(string::err.webview_terminated(name).c_str());
  };
}

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_THREADING_RESOLVE_THREAD_CONSTRUCTOR_CC