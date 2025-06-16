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

#ifndef WEBVIEW_LOG_CONSOLE_LOG_HH
#define WEBVIEW_LOG_CONSOLE_LOG_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "log/ansi_log.hh"
#include "types/types.h"
#include "types/types.hh"

using namespace webview::types;
namespace webview {
namespace _lib {
namespace _log {

struct util_t {
  std::string get_handle_kind(webview_native_handle_kind_t kind) const {
    switch (kind) {
    case WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW:
      return "window";
    case WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET:
      return "widget";
    case WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER:
      return "browser_controller";
    default:
      return "unknown";
    }
  }
};

class console_t : ansi_t {
public:
  console_t() noexcept = default;

  IGNORE_UNUSED_PARAMETERS

  /// Prints an info message in dim to the console stdout.
  void info(const std::string &message) const {
#if WEBVIEW_LOG
    print_ansi(ansi.dim, "WEBVIEW: INFO | " + message);
#endif
  };

  /// Prints a warning message in yellow to the console stdout.
  void warn(const std::string &message) const {
#if WEBVIEW_LOG
    print_ansi(ansi.yellow, "WEBVIEW: WARNING | " + message);
#endif
  };

  /// Prints an error message in red to the console stderr.
  /// @par[in] err Optionally provide a Webview error code.
  void error(const std::string &message,
             webview_error_t err = WEBVIEW_ERROR_UNSPECIFIED) const {
#if WEBVIEW_LOG
    print_ansi(ansi.red,
               "WEBVIEW: ERROR (" + std::to_string(err) + ") | " + message);
#endif
  };

  RESTORE_IGNORED_WARNINGS

  util_t util{};
};

} // namespace _log
} // namespace _lib

using namespace _lib::_log;
namespace log {

static const console_t console{};

} // namespace log
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_LOG_CONSOLE_LOG_HH
