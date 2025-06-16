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

#ifndef WEBVIEW_TEST_HELPER_CC
#define WEBVIEW_TEST_HELPER_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "tests/test_helper.hh"
#include "detail/engine_base.hh"
#include "tests/test_templates.hh"
#include <iostream>

using namespace webview::strings;
using namespace webview::tests;

void tester::set_value(const std::string &val) {
  std::lock_guard<std::mutex> lock(mtx());
  string_value() = val;
  eval_values();
  if (worker_proceed().load()) {
    cv().notify_all();
  }
}

void tester::expect_value(const std::string &value) {
  std::lock_guard<std::mutex> lock(mtx());
  string_expected_value() = value;
  eval_values();
}

bool tester::values_match() { return worker_proceed().load(); }

std::string tester::get_value() {
  std::lock_guard<std::mutex> lock(mtx());
  return string_value();
}

void tester::ping_value(const std::string &value, webview_cc &wv) {
  std::lock_guard<std::mutex> lock(mtx());
  wv.dispatch([&, value] { wv.eval(test_js.post_value(value)); });
}

std::chrono::seconds tester::seconds(int seconds) {
  return std::chrono::seconds(seconds);
}

std::string tester::res_string(const std::string &prefix, bool val) {
  return prefix + ": " + (val ? "true" : "false");
}
std::string tester::res_string(const std::string &prefix, int val) {
  return prefix + ": " + std::to_string(val);
}

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_TEST_HELPER_CC
