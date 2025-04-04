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

#include "webview/errors.h"
#include <mutex>
#include <string>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifdef _MSC_VER
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 //Win XP
#endif                      // _WIN32_WINNT
#endif                      // _MSC_VER
#include <io.h>
#include <windows.h>
struct console_modes_t {
  DWORD out;
  DWORD err;
};
struct console_handles_t {
  HANDLE out;
  HANDLE err;
};
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

/// A static utility class that enables printing colourised warnings, errors and info
/// to the process console.
class console {
public:
  /// Prints an info message in dim to the console stdout.
  static void info(std::string message);

  /// Prints a warning message in yellow to the console stdout.
  static void warn(std::string message);

  /// Prints an error message in red to the console stderr.
  /// @par[in] err Optionally provide a Webview error code.
  static void error(std::string message,
                    webview_error_t err = WEBVIEW_ERROR_UNSPECIFIED);

  /// Attaches a Webview owned console to the  Windows process if no user console is already attached.
  /// It informs the user of this process level change.
  /// NOOP in *Nix.
  static void attach_console(std::string attach_location);

private:
  /// Prepares the console for printing.
  /// The internal implementation varies between Windows and *Nix.
  static void capture_console();

  /// Frees the console on Windows.
  /// - prevents hang/error at process exit if Webview owns console.
  /// - resets user managed console state if user owns console.
  /// NOOP for *Nix
  static void free_console();

  /// Sets an ASCI escaped console colour to a message string.
  static std::string set_colour(int color, std::string message);

  /// ASCI colour int values.
  static console_colours_t colours;

  /// Message prefix strings.
  static console_prefix_t prefix;

  // Flags if the Windows console ASCI escape mode succeeded.
  // This is initiated as always `true` for *Nix systems.
  static bool stat_out_modes;

  static std::mutex mutex;

#if defined(_WIN32)

  /// Gets the existing console modes for `stdout` and `stderr`
  /// and stores them to class properties.
  static void get_ex_console_modes();

  /// Re-sets the user's Windows console modes back to stored original states
  /// for `stdout` and `stderr`.
  static void reset_user_modes();

  /// Attempts to set `ENABLE_VIRTUAL_TERMINAL_PROCESSING`
  /// (enabling ASCI escaped colours) on modes for `stdout` and `stderr`.
  /// @return `true` if successful
  static bool set_evtp_modes();

  /// Checks if `ENABLE_VIRTUAL_TERMINAL_PROCESSING` is already set on a mode.
  static bool has_evtp_mode(DWORD dw_mode);

  /// Reopens an output stream to point to an attached console.
  static void redirect_o_stream(_iobuf *stream);

  /// Gets the `stdout` and `stderr` file handles and
  /// assigns them to class properties.
  static void get_output_handles();

  /// Determines if the user owns the console attached to the process.
  /// @return Lazily initiated static const flag
  static bool user_owns_console();

  /// The stored existing console modes.
  static console_modes_t modes;

  /// Handles to the console out files.
  static console_handles_t handles;

  /// Flags if Webview owns the console.
  static bool wv_owns_console;

#endif // defined(_WIN32)

}; // class console
} // namespace utility
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_UTILITY_CONSOLE_HH
