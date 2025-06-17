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

#ifndef WEBVIEW_STRINGS_STRING_API_HH
#define WEBVIEW_STRINGS_STRING_API_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "lib/macros.h"
#include "strings/json.hh"
#include "strings/string_maker.hh"
#include "types/types.hh"
#include <regex>

using namespace webview::types;
using namespace webview::_lib::_strings;
using namespace webview::_lib::_strings::_frontend;
namespace webview {
namespace strings {

/// API for system strings
struct string {
  /// JS system strings.
  static const js_string_t js;
  /// Error message strings
  static const error_message_t err;
  /// Replacement token values for templating
  static const tokens_t tokens;
  /// Flags for different kinds of frontend messaging
  static const sys_flags_t flags;
  /// Performs string replacement for tokens.
  /// @todo REGEX is probably going to be optimal for performance
  static std::string tokenise(const std::string &tmplate,
                              const std::string &token,
                              const std::string &tkn_replcmnt);
  static std::string tokenise(const std::string &tmplate,
                              strg_replacements_t &replacements);
  /// Trims whitespace from start / end of a string;
  static std::string trim(const std::string &str);
};

const js_string_t string::js{};
const error_message_t string::err{};
/*
  * Do not change any value below without explicitly updating the string templates.
  * NB!!
  */

// NOLINTBEGIN(cert-err58-cpp)
const tokens_t string::tokens{
    "_str_",    "_int_",     "_var_",      "_id_",      "_status_",
    "_result_", "_post_fn_", "_js_names_", "_user_js_", "_what_"};
const sys_flags_t string::flags{"_sysop", "_testop"};
// NOLINTEND(cert-err58-cpp)

/*
  * END NB!!
  * Do not change any value above without explicitly updating the string templates.
  */

/// API for JSON operations
struct json {

  /// Get a value from a JSON string by object key or array index
  /// @param string The JSON string to be parsed.
  /// @param key For JSON arrays, pass "", else the name of the object key for which the value is to be returned.
  /// @param index For JSON arrays, pass the index for which the value is to be returned, else 0.
  static std::string parse(const std::string &string, const std::string &key,
                           const int index) {
    return json_lib::json_parse(string, key, index);
  }
  /// JSON escape a string.
  /// @param add_quotes flag that will wrap the escaped string in `"` quotes.
  static std::string escape(const std::string &string, bool add_quotes = true) {
    return json_lib::json_escape(string, add_quotes);
  }
  /// Parses a vector into a JSON array string.
  static std::string to_list(std::vector<std::string> &binding_names) {
    return json_lib::json_to_list(binding_names);
  }
};

} // namespace strings
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_STRINGS_STRING_API_HH
