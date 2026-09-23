/*
 * MIT License
 *
 * Copyright (c) 2017 Serge Zaitsev
 * Copyright (c) 2022 Steffen André Langnes
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

#ifndef WEBVIEW_PLATFORM_DARWIN_COCOA_NSLOG_HH
#define WEBVIEW_PLATFORM_DARWIN_COCOA_NSLOG_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

#include "../objc/objc.hh"

namespace webview {
namespace detail {
namespace cocoa {

inline void NSLog(const char *format, ...) {
  objc::autoreleasepool arp;
  va_list args;
  va_start(args, format);
  const char *p = format;
  while (*p) {
    if (*p == '%') {
      p++;
      if (*p == '\0') {
        break;
      }
      if (*p == '@') {
        void *obj = va_arg(args, void *);
        id description = objc::msg_send<id>(obj, objc::selector("description"));
        if (description) {
          const char *str = objc::msg_send<const char *>(
              description, objc::selector("UTF8String"));
          fputs(str, stdout);
        } else {
          fputs("(null)", stdout);
        }
        p++;
      } else {
        // Standard specifiers: Parse it, fetch arg, print
        const char *start_spec = p - 1; // Include the '%'
        const char *curr = p;
        // Flags
        while (*curr == '-' || *curr == '+' || *curr == ' ' || *curr == '#' ||
               *curr == '0') {
          curr++;
        }
        // Width
        while (*curr >= '0' && *curr <= '9') {
          curr++;
        }
        // Precision
        if (*curr == '.') {
          curr++;
          while (*curr >= '0' && *curr <= '9') {
            curr++;
          }
        }
        // Length
        while (*curr == 'h' || *curr == 'l' || *curr == 'L' || *curr == 'z' ||
               *curr == 't' || *curr == 'j') {
          curr++;
        }
        char conv = *curr;
        if (conv == '\0') {
          break;
        }
        // Note: va_arg promotes float to double, int to int, etc.
        switch (conv) {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X': {
          // If 'l' is in the spec, use long, else int.
          bool is_long = false;
          const char *len_ptr = p;
          while (*len_ptr == 'h' || *len_ptr == 'l' || *len_ptr == 'L' ||
                 *len_ptr == 'z' || *len_ptr == 't' || *len_ptr == 'j') {
            if (*len_ptr == 'l' || *len_ptr == 'L') {
              is_long = true;
            }
            len_ptr++;
          }

          if (is_long) {
            long val = va_arg(args, long);
            // Construct a temp string for dynamic format string.
            char temp_fmt[64];
            int len = (int)(curr - start_spec) + 1;
            if (len >= 64) {
              len = 63;
            }
            strncpy(temp_fmt, start_spec, len);
            temp_fmt[len] = '\0';
            if (conv == 'd' || conv == 'i') {
              printf(temp_fmt, val);
            } else if (conv == 'u' || conv == 'x' || conv == 'X') {
              printf(temp_fmt, (unsigned long)val);
            } else {
              printf(temp_fmt, val); // fallback
            }
          } else {
            int val = va_arg(args, int);
            char temp_fmt[64];
            int len = (int)(curr - start_spec) + 1;
            if (len >= 64) {
              len = 63;
            }
            strncpy(temp_fmt, start_spec, len);
            temp_fmt[len] = '\0';
            if (conv == 'd' || conv == 'i') {
              printf(temp_fmt, val);
            } else if (conv == 'u' || conv == 'x' || conv == 'X') {
              printf(temp_fmt, (unsigned int)val);
            } else {
              printf(temp_fmt, val);
            }
          }
          break;
        }
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A': { // double (float promoted)
          double val = va_arg(args, double);
          char temp_fmt[64];
          int len = (int)(curr - start_spec) + 1;
          if (len >= 64) {
            len = 63;
          }
          strncpy(temp_fmt, start_spec, len);
          temp_fmt[len] = '\0';
          printf(temp_fmt, val);
          break;
        }
        case 's': { // char*
          const char *str = va_arg(args, const char *);
          char temp_fmt[64];
          int len = (int)(curr - start_spec) + 1;
          if (len >= 64) {
            len = 63;
          }
          strncpy(temp_fmt, start_spec, len);
          temp_fmt[len] = '\0';
          printf(temp_fmt, str ? str : "(null)");
          break;
        }
        case 'p': { // void*
          const void *ptr = va_arg(args, void *);
          char temp_fmt[64];
          int len = (int)(curr - start_spec) + 1;
          if (len >= 64) {
            len = 63;
          }
          strncpy(temp_fmt, start_spec, len);
          temp_fmt[len] = '\0';
          printf(temp_fmt, ptr);
          break;
        }
        case 'c': { // int (char promoted)
          int val = va_arg(args, int);
          char temp_fmt[64];
          int len = (int)(curr - start_spec) + 1;
          if (len >= 64) {
            len = 63;
          }
          strncpy(temp_fmt, start_spec, len);
          temp_fmt[len] = '\0';
          printf(temp_fmt, val);
          break;
        }
        default:
          putchar(conv);
          break;
        }
        p = curr + 1; // Move past the conversion char
      }
    } else {
      putchar(*p);
      p++;
    }
  }

  putchar('\n');
  fflush(stdout);
  va_end(args);
}

} // namespace cocoa
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_COCOA_NSLOG_HH
