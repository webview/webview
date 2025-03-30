/*
 * MIT License
 *
 * Copyright (c) 2017 Serge Zaitsev
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

#pragma once

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include <string>

struct colours_t {
  int yellow;
  int red;
  int dim;
};

namespace webview {
namespace utility {

/// A static utility class to log colourised warnings and errors to the process console.
class console {
public:
  /// Logs a warning in yellow to stdout
  static void warn(std::string message);

  /// Logs an error in red to stderr
  static void error(std::string message);

private:
  /// Performs console initialisation differently for Windows and *Nix like systems
  static void init_console();

  /// Frees the console for Windows systems else the process will hang/error at exit.
  static void free_console();

  /// Sets an appropriate colour to a message string.
  static std::string set_colour(int color, std::string message);

  /// UTF console colour code int values.
  static colours_t colours;

  // Flags if the console ASCI escape mode was successfully set.
  // This is always true for *Nix systems.
  static bool is_console_mode_set;

#if defined(_WIN32)

  /// Determines if the user has already attached a console to the process.
  /// This function is lazy initialised with a local static variable cached.
  static bool user_has_console();

  /// Flags if Webview is in ownership of an attached console
  static bool wv_has_console;

#endif // defined(_WIN32)

}; // class console
} // namespace utility
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
