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
 * Queue API 
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* Common queue shared print methods
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void queue_print_t::start(const std::string &name) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.blue;
  auto postfix_m = postfix + ": ";
  auto start_m = bold(this_c, "START   ") + "| " + bold(this_c, escape_s(name));
  print_ansi(this_c, prefix + postfix_m + start_m);
#endif
}
void queue_print_t::wait(const std::string &name) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.yellow_dim;
  auto postfix_m = postfix + ": ";
  auto wait_m = "WAIT    | " + bold(this_c, escape_s(name));
  print_ansi(this_c, prefix + postfix_m + wait_m);
#endif
}
void queue_print_t::done(bool done, const std::string &name) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.blue;
  auto postfix_m = postfix + ": ";
  auto done_m =
      "DONE    | " + bold(this_c, escape_s(name)) + " : " + bool_s(done);
  print_ansi(this_c, prefix + postfix_m + done_m);
#endif
}

/* Queue eval methods 
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void queue_eval_t::wrapper_t::start() const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.blue;
  auto postfix_m = postfix + ": ";
  auto start_m = bold(this_c, "START") + "   | js ...";
  print_ansi(this_c, prefix + postfix_m + start_m);
#endif
}
void queue_eval_t::wrapper_t::wait() const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.yellow;
  auto postfix_m = postfix + ": ";
  auto wait_m = bold(this_c, "WAIT") + "    | js ...";
  print_ansi(this_c, prefix + postfix_m + wait_m);
#endif
}
void queue_eval_t::wrapper_t::done(bool done) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.blue;
  auto postfix_m = postfix + ": ";
  auto done_m = bold(this_c, "DONE") + "    | " + bool_s(done);
  print_ansi(this_c, prefix + postfix_m + done_m);
#endif
}

/* Queue loop methods 
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void queue_loop_t::wrapper_t::wait(size_t size, bool empty,
                                   bool dom_ready) const {
#if WEBVIEW_LOG_TRACE
  set_loop_wait_ts();
  auto this_col = ansi.green;
  auto waiting = bold(this_col, "WAITING ");
  auto size_s = "| queue size: " + std::to_string(size) + " ";
  auto empty_s = "| queue_empty: " + bool_s(empty) + " ";
  auto dom_s = "| dom_ready: " + bool_s(dom_ready) + " ";
  auto time_m = "| total process time: " + total_elapsed() + " ";

  print_ansi(this_col,
             prefix + postfix + waiting + time_m + size_s + empty_s + dom_s);
#endif
}
void queue_loop_t::wrapper_t::start(size_t size) const {
#if WEBVIEW_LOG_TRACE
  set_loop_start_ts();
  auto this_col = ansi.green;
  auto start = bold(this_col, "START   ");
  auto time_m = "| waited for: " + wait_elapsed() + " ";
  auto size_m = "| queue size: " + std::to_string(size);

  print_ansi(this_col, prefix + postfix + start + time_m + size_m + "\n");
#endif
}
void queue_loop_t::wrapper_t::end() const {
#if WEBVIEW_LOG_TRACE
  set_loop_end_ts();
  auto this_col = ansi.green;
  auto end = bold(this_col, "END     ");
  auto time_m = "| loop process time: " + loop_elapsed();
  print_ansi(this_col, "\n" + prefix + postfix + end + time_m);
#endif
}
void queue_loop_t::wrapper_t::set_loop_wait_ts() const {
#if WEBVIEW_LOG_TRACE
  loop_wait_ts = get_now();
#endif
}
void queue_loop_t::wrapper_t::set_loop_start_ts() const {
#if WEBVIEW_LOG_TRACE
  loop_start_ts = get_now();
#endif
}
void queue_loop_t::wrapper_t::set_loop_end_ts() const {
#if WEBVIEW_LOG_TRACE
  loop_end_ts = get_now();
#endif
}
std::string queue_loop_t::wrapper_t::total_elapsed() const {
#if WEBVIEW_LOG_TRACE
  auto elapsed = elapsed_ms(process_ts, loop_wait_ts);
  return std::to_string(elapsed) + "ms";
#else
  return "";
#endif
}
std::string queue_loop_t::wrapper_t::wait_elapsed() const {
#if WEBVIEW_LOG_TRACE
  auto elapsed = elapsed_ms(loop_wait_ts, loop_start_ts);
  return std::to_string(elapsed) + "ms";
#else
  return "";
#endif
}
std::string queue_loop_t::wrapper_t::loop_elapsed() const {
#if WEBVIEW_LOG_TRACE
  auto elapsed = elapsed_ms(loop_start_ts, loop_end_ts);
  return std::to_string(elapsed) + "ms";
#else
  return "";
#endif
}

/* Queue message notification methods
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void queue_notify_t::wrapper_t::on_message(const std::string &method) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.yellow_dim;
  auto mess_m = "message | ";
  auto method_m = bold(this_c, escape_s(method));
  print_ansi(this_c, prefix + postfix + mess_m + method_m);
#endif
}

/* Queue enqueue methods
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void queue_enqueue_t::wrapper_t::added(char scp, size_t size,
                                       const std::string &name_or_js) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.default_c;
  auto ctx_m = get_ctx(scp) + " | ";
  auto name_or_js_m =
      scp == 'e' ? "js ..." : bold(this_c, escape_s(name_or_js));
  auto size_m = " | queue size: " + num_s(size);
  print_ansi(this_c, prefix + postfix + ctx_m + name_or_js_m + size_m);
#endif
}
void queue_enqueue_t::wrapper_t::added(char scp, size_t size) const {
#if WEBVIEW_LOG_TRACE
  auto this_c = ansi.default_c;
  auto ctx_m = get_ctx(scp);
  auto size_m = "queue size: " + num_s(size) + " | ";
  auto queued_m = bold(this_c, "queueing " + ctx_m);
  print_ansi(this_c, prefix + postfix + size_m + queued_m);
#endif
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * Queue API
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
const queue_trace_t &webview::log::trace::queue = get_queue();
const tests_trace_t &webview::log::trace::test = get_tests();

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * Root API */

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_LOG_TRACE_LOG_CC
