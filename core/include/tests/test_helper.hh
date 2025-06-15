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

#ifndef WEBVIEW_TEST_HELPER_HH
#define WEBVIEW_TEST_HELPER_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "api/api_lib.hh"
#include <atomic>
#include <condition_variable>

namespace webview {
using namespace detail;
namespace tests {

/// Test utilities class
class tester {
public:
  static std::condition_variable &cv() {
    static std::condition_variable cv_;
    return cv_;
  }

  /// Sets the returned test value.
  static void set_value(const std::string &val);

  /// Sets the expected returned test value.
  static void expect_value(const std::string &value);

  /// Gets the returned test value.
  static std::string get_value();

  /// Flag indicating if the returned value matches the expected value.
  static bool values_match();

  /// Pings the frontend with a test value.
  static void ping_value(const std::string &value, webview_cc &wv);

  /// Returns the given timespan in std::chrono::seconds
  static std::chrono::seconds seconds(int seconds);

  /// Format helper for a stringified "prefix: result" message
  static std::string res_string(const std::string &prefix, bool val);
  static std::string res_string(const std::string &prefix, int val);

private:
  static std::mutex &mtx() {
    static std::mutex mtx;
    return mtx;
  };
  static std::string &string_value() {
    static std::string val;
    return val;
  }
  static std::string &string_expected_value() {
    static std::string val;
    return val;
  }
  static std::atomic_bool &worker_proceed() {
    static std::atomic_bool val{};
    return val;
  }
  static std::atomic_bool &resolve_on_main_thread_() {
    static std::atomic_bool val{};
    return val;
  }
  /// Checks if the expected and stored values match
  static void eval_values() {
    worker_proceed().store(string_expected_value() == string_value());
  }
};

} // namespace tests
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_TEST_HELPER_HH
