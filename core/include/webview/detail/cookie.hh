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

#ifndef WEBVIEW_DETAIL_COOKIE_HH
#define WEBVIEW_DETAIL_COOKIE_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include <string>
#include <vector>

namespace webview {
namespace detail {

class cookie_data {
public:
  enum class same_site_values {
    SAME_SITE_NONE = 0,
    SAME_SITE_LAX = 1,
    SAME_SITE_STRICT = 2,
  };
  cookie_data(const std::string &cookie = {}) { parse(cookie); }

  const std::string &get_name() const { return m_name; }
  const std::string &get_value() const { return m_value; }
  const std::string &get_domain() const { return m_domain; }
  int get_maxage() const { return m_maxage; }
  const std::string &get_path() const { return m_path; }
  bool get_httpOnly() const { return m_httpOnly; }
  bool get_secure() const { return m_secure; }
  same_site_values get_sameSite() const { return m_sameSite; }
  std::string get_sameSiteString() const {
    switch (m_sameSite) {
    case cookie_data::same_site_values::SAME_SITE_STRICT:
      return "strict";
    case cookie_data::same_site_values::SAME_SITE_LAX:
      return "lax";
    default:
      return "none";
    }
  }

  bool isValidForAdd() const {
    return !m_name.empty() && !m_value.empty() && !m_domain.empty() &&
           !m_path.empty();
  }
  bool forDelete() const {
    return !m_name.empty() && m_value.empty() && !m_domain.empty();
  }

private:
  void parse(const std::string &cookie) {
    std::vector<std::string> parts = split(cookie);
    if (parts.empty()) {
      return;
    }
    // First part: NAME=VALUE
    size_t eq = parts[0].find('=');
    if (eq != std::string::npos) {
      m_name = trim(parts[0].substr(0, eq));
      m_value = trim(parts[0].substr(eq + 1));
    }
    // Remaining parts: attributes
    for (size_t i = 1; i < parts.size(); ++i) {
      eq = parts[i].find('=');
      if (eq == std::string::npos) {
        if (trim(parts[i]) == "HttpOnly") {
          m_httpOnly = true;
        } else if (trim(parts[i]) == "Secure") {
          m_secure = true;
        }
        continue;
      }
      std::string key = trim(parts[i].substr(0, eq));
      std::string val = trim(parts[i].substr(eq + 1));
      if (key == "Domain") {
        m_domain = val;
      } else if (key == "Path") {
        m_path = val;
      } else if (key == "Max-Age") {
        m_maxage = std::stoi(val);
      } else if (key == "SameSite") {
        if (val == "Lax") {
          m_sameSite = same_site_values::SAME_SITE_LAX;
        } else if (val == "Strict") {
          m_sameSite = same_site_values::SAME_SITE_STRICT;
        }
      }
    }
  }

  static std::vector<std::string> split(const std::string &str) {
    std::vector<std::string> tokens;
    size_t start{};
    size_t end = str.find(';');
    while (end != std::string::npos) {
      tokens.push_back(str.substr(start, end - start));
      start = end + 1;
      end = str.find(';', start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
  }

  static std::string trim(const std::string &str) {
    size_t start{};
    while (start < str.size() &&
           std::isspace(static_cast<unsigned char>(str[start]))) {
      ++start;
    }
    if (start == str.size()) {
      return {}; // All whitespace
    }
    size_t end = str.size() - 1;
    while (end > start && std::isspace(static_cast<unsigned char>(str[end]))) {
      --end;
    }
    return str.substr(start, end - start + 1);
  }

  std::string m_name;
  std::string m_value;
  std::string m_domain;
  int m_maxage{-1};
  std::string m_path;
  bool m_httpOnly{};
  bool m_secure{};
  same_site_values m_sameSite{same_site_values::SAME_SITE_LAX};
};

} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_COOKIE_HH
