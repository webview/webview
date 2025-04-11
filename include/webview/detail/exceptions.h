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

#ifndef WEBVIEW_DETAIL_EXCEPTIONS_HH
#define WEBVIEW_DETAIL_EXCEPTIONS_HH

#include <stdexcept>
#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#    include <exception>

namespace webview {

class bad_access : public std::runtime_error {};

namespace binding {
class duplicate : public std::runtime_error {
    explicit duplicate(std::string_view name)
          : std::runtime_error(("Binding " + std::string{name} + " already exists !").c_str()) {}
};

class not_found : public std::runtime_error {
    explicit not_found(std::string_view name)
          : std::runtime_error(("Binding " + std::string{name} + " not found !").c_str()) {}
};
}  // namespace binding

}  // namespace webview

#endif  // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif  // WEBVIEW_DETAIL_EXCEPTIONS_HH
