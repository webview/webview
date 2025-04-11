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

#pragma once

#include "../http.h"
#include "user_script.h"

#include <unordered_map>
#include <atomic>
#include <functional>
#include <string>

namespace webview {

/// Window size hints
enum class Hint {
   /// Width and height are default size.
   NONE,
   /// Width and height are minimum bounds.
   MIN,
   /// Width and height are maximum bounds.
   MAX,
   /// Window size can not be changed by a user.
   FIXED
};

using url_handler_t = std::function<http::response_t(http::request_t const& request)>;
using binding_t     = std::function<void(std::string_view id, std::string_view args)>;

class Webview {
public:
   virtual ~Webview() = default;

   void navigate(std::string_view url);

   virtual void register_url_handler(std::string const& filter, url_handler_t&& handler) = 0;

   template <class Promise>
   void bind(std::string_view name, Promise&& promise);
   void unbind(std::string_view name);

   virtual void run()                               = 0;
   virtual void terminate()                         = 0;
   virtual void dispatch(std::function<void()>&& f) = 0;
   virtual void set_title(std::string_view title)   = 0;

   virtual void set_size(int width, int height, Hint hints) = 0;
   virtual void set_html(std::string_view html)             = 0;

   void init(std::string_view js);

   virtual void eval(std::string_view js)   = 0;
   virtual void open_dev_tools()            = 0;
   virtual void install_ressource_handler() = 0;

protected:
   virtual void navigate_impl(std::string_view url) = 0;

   virtual user_script* add_user_script(std::string_view js);
   virtual user_script  add_user_script_impl(std::string_view js) = 0;
   virtual user_script* replace_user_script(user_script const& old_script, std::string_view new_script_code);
   virtual void         remove_all_user_scripts(std::list<user_script> const& scripts)              = 0;
   virtual bool         are_user_scripts_equal(user_script const& first, user_script const& second) = 0;

   void        replace_bind_script();
   void        add_init_script(std::string_view post_fn);
   std::string create_init_script(std::string_view post_fn);
   std::string create_bind_script();

   virtual void on_message(std::string_view msg);
   virtual void on_window_created();

   virtual void on_window_destroyed(bool skip_termination = false);

private:
   static std::atomic_uint& window_ref_count();
   static unsigned int      inc_window_count();
   static unsigned int      dec_window_count();

   using bindings_t = std::unordered_map<std::string, std::shared_ptr<binding_t>>;
   bindings_t             bindings_{};
   user_script*           bind_script_{};
   std::list<user_script> user_scripts_;
};

}  // namespace webview

#include "engine_base.inl"