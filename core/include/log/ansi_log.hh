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

#ifndef WEBVIEW_LOG_ANSI_LOG_HH
#define WEBVIEW_LOG_ANSI_LOG_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"
#include "types/types.hh"
#include <iostream>
#include <mutex>
#include <string>

using namespace webview::types;
namespace webview {
namespace _lib {
namespace _log {

static std::string to_ansi_string(std::initializer_list<int> codes) {
  std::string ansi_string;
  for (size_t i = 0; i < codes.size(); ++i) {
    auto code = std::to_string(*(codes.begin() + i));
    if (i == 0 && codes.size() == 1) {
      ansi_string += "\033[" + code;
    } else if (i == 0) {
      ansi_string += "\033[" + code + ";";
    } else if (i == codes.size() - 1) {
      ansi_string += code;
    } else {
      ansi_string += code + ";";
    }
  };
  ansi_string += "m";
  return ansi_string;
}

class ansi_t {
public:
  struct ansi_colours_t {
    const std::string yellow = yellow_s();
    const std::string yellow_dim = yellow_dim_s();
    const std::string green = green_s();
    const std::string red = red_s();
    const std::string blue = blue_s();
    const std::string blue_dark = blue_dark_s();
    const std::string magenta = magenta_s();
    const std::string default_c = default_c_s();
    const std::string bold = bold_s();
    const std::string dim = dim_s();
  } ansi{};

  IGNORE_UNUSED_PARAMETERS
  // NOLINTBEGIN(misc-unused-parameters)

  std::string bold(const std::string &this_col, const std::string &text) const {
#if WEBVIEW_LOG_ANSI
    return ansi.bold + text + ansi.default_c + this_col;
#else
    return text;
#endif
  };
  std::string dim(const std::string &this_col, const std::string &text) const {
#if WEBVIEW_LOG_ANSI
    return ansi.dim + text + ansi.default_c + this_col;
#else
    return text;
#endif
  };
  void print_ansi(const std::string &this_col,
                  const std::string &message) const {
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
#if WEBVIEW_LOG_ANSI
    std::cout << this_col << message << ansi.default_c << std::endl;
#else
    std::cout << message << std::endl;
#endif
  };

  RESTORE_IGNORED_WARNINGS

  // NOLINTEND(misc-unused-parameters)

private:
  static const std::string &yellow_s() {
    static const std::string instance = to_ansi_string({33});
    return instance;
  }
  static const std::string &yellow_dim_s() {
    static const std::string instance = to_ansi_string({33, 2});
    return instance;
  }
  static const std::string &green_s() {
    static const std::string instance = to_ansi_string({92});
    return instance;
  }
  static const std::string &red_s() {
    static const std::string instance = to_ansi_string({91});
    return instance;
  }
  static const std::string &blue_s() {
    static const std::string instance = to_ansi_string({94});
    return instance;
  }
  static const std::string &blue_dark_s() {
    static const std::string instance = to_ansi_string({34});
    return instance;
  }
  static const std::string &magenta_s() {
    static const std::string instance = to_ansi_string({95});
    return instance;
  }
  static const std::string &default_c_s() {
    static const std::string instance = to_ansi_string({0});
    return instance;
  }
  static const std::string &bold_s() {
    static const std::string instance = to_ansi_string({1});
    return instance;
  }
  static const std::string &dim_s() {
    static const std::string instance = to_ansi_string({90});
    return instance;
  }
};

} // namespace _log
} // namespace _lib
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_LOG_ANSI_LOG_HH
