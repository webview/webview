#pragma once
#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "webview/utility/console.hh"
#include <iostream>

using namespace webview::utility;

void console::warn(std::string message) {
  init_console();
  std::string prefix = "[WEBVIEW][WARNING]: ";
  message = set_colour(colours.yellow, prefix + message);
  printf("%s\n", message.c_str());
  free_console();
};

void console::error(std::string message) {
  init_console();
  std::string prefix = "[WEBVIEW][ERROR]: ";
  message = set_colour(colours.red, prefix + message);
  std::cerr << message << "\n";
  free_console();
};

std::string console::set_colour(int color, std::string message) {
  if (!is_console_mode_set) {
    return message;
  };
  message = "\x1B[" + std::to_string(color) + "m" + message + "\033[0m";
  return message;
}

colours_t console::colours{33, 31};

#if defined(_WIN32)

#include <handleapi.h>
#include <io.h>
#include <windows.h>

void console::init_console() {
  std::cout.flush();
  std::cerr.flush();
  if (user_has_console()) {
    return;
  }
  /* 
   * We attach/free the console after every log operation to avoid deeply embedding the console system into 
   * the Webview class destructor. 
   * Console output should be minimal, and performance overhead for this negligable.
   */
  AttachConsole(ATTACH_PARENT_PROCESS);

  if (wv_has_console) {
    return;
  }
  auto std_out = freopen("CONOUT$", "w", stdout);
  auto std_err = freopen("CONOUT$", "w", stderr);
  /*
   * Below here we enable virtual terminal processing to attempt getting ASCI escaped colours in legacy
   * Windows systems.
   */
  auto fd_out = _fileno(std_out);
  auto fd_err = _fileno(std_err);
  auto h_out = reinterpret_cast<HANDLE>(_get_osfhandle(fd_out));
  auto h_err = reinterpret_cast<HANDLE>(_get_osfhandle(fd_err));
  DWORD dwMode = 0;
  auto stat_out =
      SetConsoleMode(h_out, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  auto stat_err =
      SetConsoleMode(h_err, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

  is_console_mode_set = stat_out && stat_err;

  wv_has_console = true;
};

void console::free_console() {
  if (!user_has_console()) {
    FreeConsole();
  }
};

bool console::user_has_console() {
  static const bool has_console = [] {
    return GetConsoleWindow() != nullptr;
  }();
  return has_console;
}

bool console::wv_has_console{};

bool console::is_console_mode_set{};

#else

void console::init_console() {
  std::cout.flush();
  std::cerr.flush();
};

void console::free_console() {}

bool console::is_console_mode_set = true;

#endif // defined(_WIN32) #else
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)