/*
 * MIT License
 *
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

#ifndef WEBVIEW_STRINGS_STRING_API_CC
#define WEBVIEW_STRINGS_STRING_API_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "strings/string_api.hh"

using namespace webview::strings;

std::string string::tokenise(const std::string &tmplate,
                             const std::string &token,
                             const std::string &tkn_replcmnt) {
  if (token.empty()) {
    return tmplate;
  }
  std::string tokenised_string = tmplate;
  size_t start_pos = 0;
  while ((start_pos = tokenised_string.find(token, start_pos)) !=
         std::string::npos) {
    tokenised_string.replace(start_pos, token.length(), tkn_replcmnt);
    start_pos += tkn_replcmnt.length();
  }
  return tokenised_string;
}

std::string string::tokenise(const std::string &tmplate,
                             strg_replacements_t &replacements) {
  std::string tokenised_string = tmplate;
  for (auto &replacement : replacements) {
    auto string_ =
        tokenise(tokenised_string, replacement.token, replacement.tkn_replcmnt);
    tokenised_string = string_;
  };
  return tokenised_string;
}

IGNORE_UNUSED_FUNCTIONS
std::string string::trim(const std::string &str) {
  return std::regex_replace(std::regex_replace(str, std::regex("^\\s+"), ""),
                            std::regex("\\s+$"), "");
}
RESTORE_IGNORED_WARNINGS

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_STRINGS_STRING_API_CC