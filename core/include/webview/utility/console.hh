#pragma once

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include <string>

struct colours_t {
  int yellow;
  int red;
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