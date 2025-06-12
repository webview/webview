/*
 * MIT License
 *
 * Copyright (c) 2024 Steffen André Langnes
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

#ifndef WEBVIEW_TEST_DRIVER_HH
#define WEBVIEW_TEST_DRIVER_HH

#include <exception>
#include <functional>
#include <map>
#include <string>

namespace webview {

struct failure_info {
  const char *condition;
  const char *file;
  int line;
};

struct test_failure : public std::exception {
  explicit test_failure(const failure_info &info) noexcept : m_info{info} {}
  const char *what() const noexcept override { return "test failure"; }
  const failure_info &info() const noexcept { return m_info; }

private:
  failure_info m_info;
};

class test_reg {
public:
  test_reg() = default;

  test_reg(const char *name, std::function<void()> fn) noexcept
      : m_name{name}, m_fn{std::move(fn)} {}

  const std::string &name() const noexcept { return m_name; }
  void invoke() const { m_fn(); }

private:
  std::string m_name;
  std::function<void()> m_fn;
};

struct auto_test_reg {
  explicit auto_test_reg(test_reg reg) noexcept {
    tests()[reg.name()] = std::move(reg);
  }

  static std::map<std::string, test_reg> &tests();
};

// NOLINTBEGIN(cppcoreguidelines-macro-usage, misc-use-anonymous-namespace)

#define MAKE_TEST_CASE_NAME2(name, counter) name##counter
#define MAKE_TEST_CASE_NAME(name, counter) MAKE_TEST_CASE_NAME2(name, counter)

#define TEST_CASE_INTERNAL(name, counter)                                      \
  static void MAKE_TEST_CASE_NAME(webview_test_driver_case_, counter)();       \
  namespace {                                                                  \
  const ::webview::auto_test_reg                                               \
      MAKE_TEST_CASE_NAME(webview_test_driver_case_reg_, counter){             \
          {name, MAKE_TEST_CASE_NAME(webview_test_driver_case_, counter)}};    \
  }                                                                            \
  static void MAKE_TEST_CASE_NAME(webview_test_driver_case_, counter)()

#define TEST_CASE(name) TEST_CASE_INTERNAL(name, __LINE__)

#define REQUIRE(condition)                                                     \
  if (!static_cast<bool>(condition)) {                                         \
    throw ::webview::test_failure{                                             \
        ::webview::failure_info{#condition, __FILE__, __LINE__}};              \
  }

#define REQUIRE_FALSE(condition)                                               \
  if (static_cast<bool>(condition)) {                                          \
    throw ::webview::test_failure{                                             \
        ::webview::failure_info{#condition, __FILE__, __LINE__}};              \
  }

#define REQUIRE_THROW(exception, fn)                                           \
  {                                                                            \
    bool did_throw{};                                                          \
    try {                                                                      \
      fn();                                                                    \
    } catch (const exception &) {                                              \
      did_throw = true;                                                        \
    }                                                                          \
    REQUIRE(did_throw);                                                        \
  }

#define SECTION(name)

// NOLINTEND(cppcoreguidelines-macro-usage, misc-use-anonymous-namespace)

} // namespace webview

#endif // WEBVIEW_TEST_DRIVER_HH
