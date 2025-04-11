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

#ifndef WEBVIEW_DETAIL_USER_SCRIPT_HH
#define WEBVIEW_DETAIL_USER_SCRIPT_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#   include <functional>
#   include <memory>
#   include <string>

namespace webview {

class user_script {
public:
   class impl;
   using impl_deleter = std::function<void(impl*)>;
   using impl_ptr     = std::unique_ptr<impl, impl_deleter>;

   user_script(std::string_view code, impl_ptr&& impl_);

   user_script(const user_script& other)            = delete;
   user_script& operator=(const user_script& other) = delete;
   user_script(user_script&& other) noexcept;
   user_script& operator=(user_script&& other) noexcept;

   ~user_script() = default;

   std::string const& get_code() const { return code_; }

   impl&       get_impl() { return *impl_; }
   impl const& get_impl() const { return *impl_; }

private:
   std::string code_;
   impl_ptr    impl_;
};

}  // namespace webview

#endif  // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif  // WEBVIEW_DETAIL_USER_SCRIPT_HH
