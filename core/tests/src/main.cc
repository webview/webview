/*
 * MIT License
 *
 * Copyright (c) 2024 Steffen André Langnes
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

#include "test_driver.hh"
#include <deque>
#include <iostream>
#include <string>

using namespace webview::test::driver;

#if defined(_WIN32) && defined(TEST_FUNCTIONAL)
#include <windows.h>
namespace {
std::deque<std::string> get_args(int argc, LPWSTR *argv_) {
  std::deque<std::string> result;
  for (int i = 0; i < argc; ++i) {
    int size = WideCharToMultiByte(CP_UTF8, 0, argv_[i], -1, nullptr, 0,
                                   nullptr, nullptr);
    std::string arg(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, argv_[i], -1, &arg[0], size, nullptr,
                        nullptr);
    result.push_back(arg);
  }
  return result;
};
} // namespace
int WINAPI WinMain(HINSTANCE /**/, HINSTANCE /**/, LPSTR /**/, int /**/) {
  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  const std::deque<std::string> args = get_args(argc, argv);
  LocalFree(static_cast<HLOCAL>(argv));
#else
int main(int argc, const char *argv[]) {
  const std::deque<std::string> args{argv, argv + argc};
#endif
  try {
    if (args.size() > 1) {
      const std::string &arg{args.at(1)};
      if (arg == "--help") {
        return cmd_help();
      }
      if (arg == "--list") {
        return cmd_list();
      }
      return cmd_run_test(arg);
    }
    return cmd_run_all_tests();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return failure_exit_codes::exception_thrown;
  } catch (...) {
    std::cerr << "Unknown error.\n";
    return failure_exit_codes::exception_thrown;
  }
}
