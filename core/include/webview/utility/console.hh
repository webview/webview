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

#ifndef WEBVIEW_UTILITY_CONSOLE_HH
#define WEBVIEW_UTILITY_CONSOLE_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include <mutex>
#include <string>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifdef _MSC_VER
#define _WIN32_WINNT 0x0601
#endif // _MSC_VER

#include <io.h>
#include <windows.h>

#endif // defined(_WIN32)

struct console_colours_t {
  int yellow;
  int red;
  int dim;
};
struct console_prefix_t {
  const char *warn;
  const char *error;
  const char *info;
};

namespace webview {
namespace utility {

/// A static utility class to prints colourised warnings and errors to the process console.
class console {
public:
  /// Prints info in dim to the console stdout.
  static void info(std::string message);

  /// Prints a warning in yellow to the console stdout.
  static void warn(std::string message);

  /// Prints an error in red to the console stderr.
  static void error(std::string message);

  /// Attaches a Windows console to Webview if no user console is already attached.
  static void attach(std::string attach_location);

private:
  /// Prepares the console for printing.
  static void capture_console();

  /// Frees the console for Windows systems.
  /// - prevents hang/error at exit.
  /// - resets user managed console state.
  static void free_console();

  /// Sets an appropriate colour to a message string.
  static std::string set_colour(int color, std::string message);

  /// UTF console colour code int values.
  static console_colours_t colours;

  /// Message prefix strings.
  static console_prefix_t prefix;

  // Flags if the Windows console ASCI escape mode succeeded.
  // This is always true for *Nix systems.
  static bool mode_set_success;

  static std::mutex mutex;

#if defined(_WIN32)

  /// Determines if the user owns a console attached to the process.
  static bool user_owns_console();

  /// Gets the existing Windows console modes for `stdout` and `stderr`.
  static void get_ex_modes();

  /// Sets the user's Windows console mode for `stdout` and `stderr`
  /// back to original states.
  static void reset_user_modes();

  /// Attempts to enable virtual terminal processing (for ASCI escaped colours)
  /// in all Windows consoles for `stdout` and `stderr`.
  static void set_evtp_modes();

  /// Checks if enable virtual terminal processing is already set on a mode.
  static bool has_evtp_mode(DWORD dw_mode);

  /// Reopens `stdout` and `stderr` to point to a newly attached console.
  static void redirect_stream(_iobuf *stream);

  /// Flags if Webview is in ownership of an attached console
  static bool wv_owns_console;

  /// The existing console `stdout` mode.
  static DWORD out_mode;

  /// The existing console `stderr` mode.
  static DWORD err_mode;

  /// Handle to `stdout`.
  static HANDLE h_out;

  /// Handle to `stderr`.
  static HANDLE h_err;

  /// Dummy out file
  static FILE *dummy_out;

#endif // defined(_WIN32)

}; // class console
} // namespace utility
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_UTILITY_CONSOLE_HH
