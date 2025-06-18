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

#ifndef WEBVIEW_LOG_TRACE_LOG_HH
#define WEBVIEW_LOG_TRACE_LOG_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "log/ansi_log.hh"
#include "types/types.hh"
#include <string>

using namespace webview::types;
namespace webview {
namespace _lib {
namespace _log {

/* Common API ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

/// String manipulation methods and ANSI colour references
struct trace_tools_t : public ansi_t {
protected:
  static time_point_t get_now();
  static std::string get_ctx(char scp);
  static long elapsed_ms(time_point_t start, time_point_t end);
  static std::string bool_s(bool flag);
  static std::string escape_s(const std::string &text);
  static std::string num_s(size_t val);
};

/// A generic `print_here` method for user input
class print_here_t : protected trace_tools_t {
public:
  ~print_here_t() = default;
  print_here_t(const std::string &prefix, const std::string &postfix)
      : prefix(prefix), postfix(postfix) {}

  void print_here(const std::string &message) const;

private:
  const std::string prefix;
  const std::string postfix;
};

/* Engine_base API ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

/// Common base shared print methods
struct base_print_t : public print_here_t {
public:
  base_print_t(const std::string &prefix, const std::string &postfix)
      : print_here_t{prefix, postfix}, prefix{prefix}, postfix(postfix) {}
  void start(const std::string &name) const;
  void work(const std::string &name) const;
  void done(const std::string &name) const;

private:
  std::string prefix;
  std::string postfix;
};

/// Base bind methods
class base_bind_t {
public:
  ~base_bind_t() = default;
  base_bind_t(const std::string &prefix,
              const std::string &postfix = "   BIND: ")
      : bind{prefix, postfix} {};

  struct wrapper_t : public base_print_t {
    wrapper_t(const std::string &prefix, const std::string &postfix)
        : base_print_t{prefix, postfix} {};
  } bind;
};

/// Base unbind methods
class base_unbind_t {
public:
  ~base_unbind_t() = default;
  base_unbind_t(const std::string &prefix,
                const std::string &postfix = " UNBIND: ")
      : unbind{prefix, postfix} {};

  struct wrapper_t : public base_print_t {
    wrapper_t(const std::string &prefix, const std::string &postfix)
        : base_print_t{prefix, postfix} {};
  } unbind;
};

/// Base eval methods
class base_eval_t {
public:
  ~base_eval_t() = default;
  base_eval_t(const std::string &prefix,
              const std::string &postfix = "   EVAL: ")
      : eval{prefix, postfix} {};

  struct wrapper_t : public print_here_t {
    wrapper_t(const std::string &prefix, const std::string &postfix)
        : print_here_t{prefix, postfix}, prefix(prefix), postfix(postfix) {}
    void start(const std::string &js, bool skip_queue) const;
    void work(const std::string &js, bool skip_queue) const;
    void done(bool done, const std::string &js) const;

  private:
    std::string prefix;
    std::string postfix;
  } eval;
};

/// Base root API
class base_trace_t : public base_bind_t,
                     public base_unbind_t,
                     public base_eval_t,
                     public print_here_t {
public:
  ~base_trace_t() = default;
  base_trace_t(const std::string &prefix,
               const std::string &postfix = " BASE: ")
      : base_bind_t(prefix + postfix),
        base_unbind_t(prefix + postfix),
        base_eval_t(prefix + postfix),
        print_here_t(prefix, postfix) {}
};

/* Tests API ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

/// Tests root API
class tests_trace_t : public print_here_t {
public:
  ~tests_trace_t() = default;
  tests_trace_t(const std::string &prefix,
                const std::string &postfix = "TESTS: ")
      : print_here_t(prefix, postfix) {}
};

} // namespace _log
} // namespace _lib

using namespace _lib::_log;
namespace log {

/* Root API ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

class trace {
public:
  /// Trace log methods for `engine_base` functionality
  static const base_trace_t &base;
  /// Trace log methods for test functionality
  static const tests_trace_t &test;

private:
  static const std::string &prefix() {
    static const std::string prefix_instance = "WEBVIEW: ";
    return prefix_instance;
  };
  static const base_trace_t &get_base() noexcept {
    static const base_trace_t base_instance = {prefix()};
    return base_instance;
  }
  static const tests_trace_t &get_tests() noexcept {
    static const tests_trace_t tests_instance = {prefix()};
    return tests_instance;
  }
};

} // namespace log
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_LOG_TRACE_LOG_HH
