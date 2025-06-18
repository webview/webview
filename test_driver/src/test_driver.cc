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

#include "test_driver.hh"
#include <iomanip>
#include <iostream>

using namespace webview::test;
using namespace webview::test::driver;

std::map<std::string, test_reg> &auto_test_reg::tests() {
  static std::map<std::string, test_reg> instance;
  return instance;
}

int driver::cmd_help() {
  std::cout << "Usage: program [--[help|list]|test_name].\n";
  return 0;
}

int driver::cmd_list() {
  auto &tests{auto_test_reg::tests()};
  if (tests.empty()) {
    std::cerr << "No tests found.\n";
    return failure_exit_codes::no_tests_found;
  }
  for (const auto &test : tests) {
    std::cout << test.second.name() << '\n';
  }
  return failure_exit_codes::success;
}

int driver::cmd_run_test(const std::string &test_name) {
  auto &tests{auto_test_reg::tests()};
  auto found{tests.find(test_name)};
  if (found == tests.end()) {
    std::cerr << "Test not found: " << test_name << '\n';
    return failure_exit_codes::test_not_found;
  }
  auto exit_code{failure_exit_codes::failure};
  const auto &test{found->second};
  try {
    test.invoke();
    exit_code = failure_exit_codes::success;
  } catch (const test_failure &e) {
    const auto &info{e.info()};
    std::cerr << "Test \"" << test.name() << "\" failed [" << info.condition
              << "] at " << info.file << ":" << info.line << ".\n";
    exit_code = failure_exit_codes::failure;
  } catch (const std::exception &e) {
    std::cerr << "Test \"" << test.name()
              << "\" threw exception with message \"" << e.what() << "\".\n";
    exit_code = failure_exit_codes::exception_thrown;
  }
  return exit_code;
}

int driver::cmd_run_all_tests() {
  auto &tests{auto_test_reg::tests()};
  if (tests.empty()) {
    return failure_exit_codes::no_tests_found;
  }
  size_t name_width{};
  for (const auto &it : tests) {
    const auto &test{it.second};
    const auto name_size{test.name().size()};
    name_width = std::max(name_size, name_width);
  }
  size_t success_count{};
  for (const auto &it : tests) {
    const auto &test{it.second};
    std::cout << test.name() << std::setfill('.')
              << std::setw(static_cast<int>(name_width - test.name().size()))
              << "" << std::setw(0) << ": ";
    try {
      test.invoke();
      ++success_count;
      std::cout << "OK\n";
    } catch (const test_failure &e) {
      const auto &info{e.info()};
      std::cout << "FAIL\n";
      std::cerr << "Test \"" << test.name() << "\" failed [" << info.condition
                << "] at " << info.file << ":" << info.line << ".\n";
    } catch (const std::exception &e) {
      std::cout << "FAIL\n";
      std::cerr << "Test \"" << test.name()
                << "\" threw exception with message \"" << e.what() << "\".\n";
    }
  }
  if (success_count == tests.size()) {
    return failure_exit_codes::success;
  }
  return failure_exit_codes::failure;
}
