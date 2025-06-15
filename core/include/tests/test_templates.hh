/*
 * MIT License
 *
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

#ifndef WEBVIEW_TEST_TEMPLATES_HH
#define WEBVIEW_TEST_TEMPLATES_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "types/types.hh"

/* Tests Templates
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇
 */

/* Tests HTML
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

#define TEMPLATE_TEST_STRING_RETURNS_HTML                                      \
  "\n\
<html><body>\n\
  <div>_str_</div>\n\
  <script>\n\
  try {\n\
    window.loadData()\n\
      .then((val) => window.endTest(0))\n\
      .catch((err) => {\n\
        console.error(err);\n\
        window.endTest(1);\n\
      });\n\
  } catch (err) {\n\
    console.error(err);\n\
    window.endTest(2);\n\
  }\n\
  </script>\n\
</body></html>\n\
"

/* Tests JS
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

#define TEMPLATE_TEST_BIND_UNBIND_INIT_JS                                      \
  "\n\
window.test(0)\n\
  .then(m => console.log(m))\n\
  .catch(err => console.error(err));\n\
"

#define TEMPLATE_TEST_VALUE_WRAPPER_JS                                         \
  "\n\
window.__webview__.post(\n\
  JSON.stringify({\n\
    id: '_testop',\n\
    method: `_str_`\n\
  })\n\
)\n\
"

#define TEMPLATE_TEST_BIND_UNBIND_JS                                           \
  "\n\
try {\n\
  window.increment()\n\
    .then(m => {\n\
      console.log(m);\n\
      window.test(_int_)\n\
        .then(m => console.log(m))\n\
        .catch(err => console.error(err));\n\
    })\n\
    .catch(err => {\n\
      console.error(err);\n\
      window.test(_int_, 1)\n\
        .then(m => console.log(m))\n\
        .catch(err => console.error(err));\n\
    })\n\
} catch (err) {\n\
  console.error(err);\n\
  window.test(_int_, 1)\n\
    .then(m => console.log(m))\n\
    .catch(err => console.error(err));\n\
}\n\
"

#define TEMPLATE_TEST_ONLOAD_JS                                                \
  "\n\
window.x = 42;\n\
window.onload = () => {\n\
  _post_fn_\n\
}\n\
"

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆
 * Tests Templates */

using namespace webview::types;
namespace webview {
namespace _lib {
namespace _strings {
namespace _tests {

/* Tests JS
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

struct test_js_t {
  test_js_t() noexcept = default;

  std::string init(const std::string &init_value) const;

  /// Wraps a string value in evaluable JS
  std::string post_value(const std::string &value) const;

  std::string bind_unbind(unsigned int result) const;

  std::string bind_unbind_init() const;
};

/* Tests HTML
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

struct test_html_t {
  test_html_t() noexcept = default;

  std::string string_returned(const std::string &title) const;
  std::string navigate_encoded() const;
};

} // namespace _tests
} // namespace _strings
} // namespace _lib

using namespace _lib::_strings::_tests;
namespace strings {

/// API for test HTML strings
static constexpr test_html_t test_html{};
/// API for test JS strings
static constexpr test_js_t test_js{};

} // namespace strings
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_TEST_TEMPLATES_HH