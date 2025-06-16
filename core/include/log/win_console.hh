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

#ifndef WEBVIEW_LOG_WIN_CONSOLE_HH
#define WEBVIEW_LOG_WIN_CONSOLE_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"

#if defined(WEBVIEW_PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <cstdio>
#include <cstdlib>
#include <windows.h>

namespace webview {
namespace log {

/// Inititialises the Windows console.
/// When a Windows app is started in GUI mode (Winmain instead of main),
/// the console output needs to be explicitly sent to `stdout` and `sterr`,
class win_console {

  static FILE *fp;
  static bool is_env_var_set(const char *varname) {
#ifdef _MSC_VER
    size_t requiredSize = 0;
    errno_t err = getenv_s(&requiredSize, nullptr, 0, varname);
    return (err == 0 && requiredSize > 0);
#else
    // For non-MSVC, use standard getenv
    return std::getenv(varname) != nullptr;
#endif
  }

public:
  ~win_console() { static_cast<void>(fclose(fp)); }
  win_console() = default;

  static void init() {
    if (GetConsoleWindow() != nullptr) {
      return;
    }
    if (GetConsoleCP() != 0) {
      return;
    }
    if (is_env_var_set("TEST")) {
      return;
    }
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
      SetConsoleOutputCP(65001);
      static_cast<void>(freopen_s(&fp, "CONOUT$", "w", stdout));
      static_cast<void>(freopen_s(&fp, "CONOUT$", "w", stderr));
    }
  }
};
FILE *win_console::fp;

} // namespace log
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_WINDOWS)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_LOG_WIN_CONSOLE_HH