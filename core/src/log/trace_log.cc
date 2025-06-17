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

#ifndef WEBVIEW_LOG_TRACE_LOG_CC
#define WEBVIEW_LOG_TRACE_LOG_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "log/trace_log.hh"
#include "lib/macros.h"
#include <iostream>

using namespace webview::types;
using namespace webview::_lib::_log;

/* Common API utils
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

time_point_t trace_tools_t::get_now() {
  return std::chrono::steady_clock::now();
}
std::string trace_tools_t::get_ctx(char scp) {
  if (scp == 'b') {
    return "bind   ";
  }
  if (scp == 'u') {
    return "unbind ";
  }
  return "eval   ";
}
long trace_tools_t::elapsed_ms(time_point_t start, time_point_t end) {
  return static_cast<long int>(
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count());
}
std::string trace_tools_t::bool_s(bool flag) { return flag ? "true" : "false"; }
std::string trace_tools_t::escape_s(const std::string &text) {
  return "\"" + text + "\"";
}
std::string trace_tools_t::num_s(size_t val) { return std::to_string(val); }

IGNORE_UNUSED_PARAMETERS

void print_here_t::print_here(const std::string &message) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.magenta;
  auto here_m = bold(this_c, "here") + ": ";
  auto message_ = ansi.default_c + message;
  print_ansi(this_c, prefix + postfix + here_m + message_);
#endif
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆  
 * Common API utils
 * -----------------------------------------------------------------------------------------------------------
 * engine Base API 
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* Common base shared print methods
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void base_print_t::start(const std::string &name) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.default_c;
  auto got_m = "got     | ";
  auto name_m = bold(this_c, escape_s(name));
  print_ansi(this_c, prefix + postfix + got_m + name_m);
#endif
}
void base_print_t::work(const std::string &name) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.blue_dark;
  auto work_m = bold(this_c, "WORK    ") + "| ";
  auto name_m = bold(this_c, escape_s(name));
  print_ansi(this_c, prefix + postfix + work_m + name_m);
#endif
}
void base_print_t::done(const std::string &name) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.blue;
  auto done_m = bold(this_c, "work done") + ": ";
  auto name_m = escape_s(name);
  print_ansi(this_c, prefix + postfix + done_m + name_m);
#endif
}

/* Base eval methods
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void base_eval_t::wrapper_t::start(const std::string &js,
                                   bool skip_queue) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.default_c;
  auto got_m = "got js  | ";
  auto skip_m = bold(this_c, skip_queue ? "skip queue" : "queue");
  auto js_string = skip_queue ? "" : dim(this_c, "\n\
// User JS\n\
// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓\n\
" + js + "\
// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑\n\
// User JS");
  auto m = prefix + postfix + got_m + skip_m + js_string;
  print_ansi(this_c, m);
#endif
}
void base_eval_t::wrapper_t::work(const std::string &js,
                                  bool skip_queue) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.blue_dark;
  auto work_m = bold(this_c, "WORK") + "    | js " +=
      skip_queue ? "skipped queue" : "from queue";
  auto m = prefix + postfix + work_m + dim(this_c, js);
  print_ansi(this_c, m);
#endif
}
void base_eval_t::wrapper_t::done(bool done, const std::string &js) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.blue;
  auto done_m = bold(this_c, "work done") + ": " + bool_s(done) + " | js ...\n";
  auto js_m = dim(this_c, js);
  print_ansi(this_c, prefix + postfix + done_m + js_m);
#endif
}

RESTORE_IGNORED_WARNINGS

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆  
 * engine Base API 
 * -----------------------------------------------------------------------------------------------------------
 * Root API
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

const base_trace_t &webview::log::trace::base = get_base();
const tests_trace_t &webview::log::trace::test = get_tests();

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * Root API */

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_LOG_TRACE_LOG_CC
