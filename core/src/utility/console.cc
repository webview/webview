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

#ifndef WEBVIEW_UTILITY_CONSOLE_CC
#define WEBVIEW_UTILITY_CONSOLE_CC
#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "webview/utility/console.hh"
#include <iostream>

using namespace webview::utility;

std::mutex console::mutex;
console_colours_t console::colours{93, 91, 90};
console_prefix_t console::prefix{
    "[WEBVIEW][WARNING]: ", "[WEBVIEW][ERROR]: ", "[WEBVIEW][INFO]: "};

void console::warn(std::string message) {
  std::lock_guard<std::mutex> lock(mutex);
  capture_console();
  message = set_colour(colours.yellow, prefix.warn + message);
  printf("%s\n", message.c_str());
  free_console();
};

void console::error(std::string message) {
  std::lock_guard<std::mutex> lock(mutex);
  capture_console();
  message = set_colour(colours.red, prefix.error + message);
  static_cast<void>(fprintf(stderr, "%s\n", message.c_str()));
  free_console();
};

void console::info(std::string message) {
  std::lock_guard<std::mutex> lock(mutex);
  capture_console();
  message = set_colour(colours.dim, prefix.info + message);
  printf("%s\n", message.c_str());
  free_console();
};

std::string console::set_colour(int color, std::string message) {
  // Windows failed to register ASCI escape console mode, so don't
  // try to format the message string.
  if (!stat_out_modes) {
    return message;
  };
  message = "\033[" + std::to_string(color) + "m" + message + "\033[0m";
  return message;
}

#if !defined(_WIN32)

void console::capture_console() {
  std::cout.flush();
  std::cerr.flush();
};

void console::free_console() {}
void console::attach_console(std::string /*attach_location*/) {}
bool console::stat_out_modes{true};

#else // !defined(_WIN32)

void console::attach_console(std::string attach_location) {
  if (user_owns_console() || wv_owns_console) {
    return;
  };
  auto message = "Webview will attach a console to the Windows process.\nIf "
                 "you want to mangage the console yourself, direct your "
                 "outputs and call `AttachConsole` before `" +
                 attach_location + "` in your code.";

  // The `info` method includes the initialising call to `capture_console()`.
  info(message);
};

void console::capture_console() {
  if (user_owns_console()) {
    std::cout.flush();
    std::cerr.flush();
    get_set_o_handles();
    get_ex_console_modes();
    set_evtp_modes();
    return;
  }

  // We attach/free the Webview owned console after every print operation.
  // This avoids needing to free the console in the Webview class destructor,
  // thus improving encapsulation of this class.
  //
  // Console output usage should remain minimal, and performance overhead negligable.
  auto got_console = AttachConsole(ATTACH_PARENT_PROCESS);
  if (!got_console || wv_owns_console) {
    return;
  }
  get_set_o_handles();
  redirect_o_stream(stdout);
  redirect_o_stream(stderr);
  get_ex_console_modes();
  set_evtp_modes();
  wv_owns_console = true;
};

void console::free_console() {
  if (user_owns_console()) {
    reset_user_modes();
    return;
  }
  FreeConsole();
};

bool console::user_owns_console() {
  static const bool has_console = [] {
    return GetConsoleWindow() != nullptr;
  }();
  return has_console;
}

void console::get_ex_console_modes() {
  GetConsoleMode(h_out, &out_mode);
  GetConsoleMode(h_err, &err_mode);
}

void console::get_set_o_handles() {
  if (h_out && h_err) {
    return;
  }
  h_out = GetStdHandle(STD_OUTPUT_HANDLE);
  h_err = GetStdHandle(STD_ERROR_HANDLE);
  if (!user_owns_console()) {
    return;
  }
  if (h_out == INVALID_HANDLE_VALUE) {
    redirect_o_stream(stdout);
    h_out = GetStdHandle(STD_OUTPUT_HANDLE);
  }
  if (h_err == INVALID_HANDLE_VALUE) {
    redirect_o_stream(stderr);
    h_err = GetStdHandle(STD_OUTPUT_HANDLE);
  }
}

void console::reset_user_modes() {
  SetConsoleMode(h_out, out_mode);
  SetConsoleMode(h_err, err_mode);
}

void console::set_evtp_modes() {
  // Webview only needs to set console modes once.
  if (wv_owns_console) {
    return;
  }
  BOOL stat_out_mode = 0;
  BOOL stat_err_mode = 0;
  if (has_evtp_mode(out_mode)) {
    stat_out_mode = 1;
  } else {
    DWORD new_mode = out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    stat_out_mode = SetConsoleMode(h_out, new_mode);
  }
  if (has_evtp_mode(err_mode)) {
    stat_err_mode = 1;
  } else {
    DWORD new_mode = err_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    stat_out_mode = SetConsoleMode(h_out, new_mode);
  };
  stat_out_modes = (stat_out_mode != 0) && (stat_err_mode != 0);
}

bool console::has_evtp_mode(DWORD dw_mode) {
  return (dw_mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}

void console::redirect_o_stream(_iobuf *stream) {
  static_cast<void>(freopen_s(&dummy_out, "CONOUT$", "w", stream));
}

bool console::wv_owns_console{};
bool console::stat_out_modes{};
DWORD console::out_mode = 0;
DWORD console::err_mode = 0;
HANDLE console::h_out;
HANDLE console::h_err;
FILE *console::dummy_out;

#endif // !defined(_WIN32) #else
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_UTILITY_CONSOLE_CC
