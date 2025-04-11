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

#include "detail/engine_base.h"
#include "detail/user_script.h"

#include "errors.h"

#include <format>
#include <json/json.h>

namespace webview {

void
Webview::navigate(std::string_view url) {
   if (url.empty()) {
      return navigate_impl("about:blank");
   }

   return navigate_impl(url);
}

void
Webview::unbind(std::string_view name) {
   if (bindings_.erase(std::string{name}) != 1) {
      throw exception(error_info{error_t::WEBVIEW_ERROR_NOT_FOUND, std::string{"trying to unbind undefined binding "} + std::string{name}});
   }

   replace_bind_script();

   // Notify that a binding was created if the init script has already
   // set things up.
   eval(std::format(R"(if (window.__webview__) {{
    window.__webview__.onUnbind({})
}})",
                    js::serialize(name)));
}

void
Webview::init(std::string_view js) {
   add_user_script(js);
}

user_script*
Webview::add_user_script(std::string_view js) {
   return std::addressof(*user_scripts_.emplace(user_scripts_.end(), add_user_script_impl(js)));
}

user_script*
Webview::replace_user_script(const user_script& old_script, std::string_view new_script_code) {
   remove_all_user_scripts(user_scripts_);
   user_script* old_script_ptr{};
   for (auto& script : user_scripts_) {
      auto is_old_script = are_user_scripts_equal(script, old_script);
      script             = add_user_script_impl(is_old_script ? new_script_code : script.get_code());
      if (is_old_script) {
         old_script_ptr = std::addressof(script);
      }
   }
   return old_script_ptr;
}

void
Webview::replace_bind_script() {
   if (bind_script_) {
      bind_script_ = replace_user_script(*bind_script_, create_bind_script());
   } else {
      bind_script_ = add_user_script(create_bind_script());
   }
}

void
Webview::add_init_script(std::string_view post_fn) {
   add_user_script(create_init_script(post_fn));
}

std::string
Webview::create_init_script(std::string_view post_fn) {
   return std::format(R"(
(function() {{
   'use strict';

   function generateId() {{
      var crypto = window.crypto || window.msCrypto;
      var bytes = new Uint8Array(16);
      crypto.getRandomValues(bytes);
        
      return Array.prototype.slice.call(bytes).map(function(n) {{
         var s = n.toString(16);
         return ((s.length % 2) == 1 ? '0' : '') + s;
      }}).join('');
   }}
    
   var Webview = (function() {{
      var _promises = {{}};
      function Webview_() {{}}

      Webview_.prototype.post = function(message) {{
         return ({})(message);
      }};

      Webview_.prototype.call = function(method) {{
         var _id = generateId();
         var _params = Array.prototype.slice.call(arguments, 1);
         var promise = new Promise(function(resolve, reject) {{
            _promises[_id] = {{ resolve, reject }};
         }});

         this.post(JSON.stringify({{
            id: _id,
            method: method,
            params: JSON.stringify(_params)
         }}));

         return promise;
      }};

      Webview_.prototype.onReply = function(id, status, result) {{
         var promise = _promises[id];
         if (result !== undefined) {{
            try {{
               result = JSON.parse(result);
            }} catch (e) {{
               promise.reject(new Error("Failed to parse binding result as JSON"));
               return;
            }}
         }}

         if (status === 0) {{
            promise.resolve(result);
         }} else {{
            promise.reject(result);
         }}
      }};

      Webview_.prototype.onBind = function(name) {{
         if (window.hasOwnProperty(name)) {{
            throw new Error('Property \"' + name + '\" already exists');
         }}

         window[name] = (function() {{
            var params = [name].concat(Array.prototype.slice.call(arguments));
            return Webview_.prototype.call.apply(this, params);
         }}).bind(this);
      }};

      Webview_.prototype.onUnbind = function(name) {{
         if (!window.hasOwnProperty(name)) {{
            throw new Error('Property \"' + name + '\" does not exist');
         }}
         delete window[name];
      }};

      return Webview_;
   }})();
  
   window.__webview__ = new Webview();
}})())",
                      post_fn);
}

std::string
Webview::create_bind_script() {
   std::string js_names = "[";
   bool        first    = true;
   for (const auto& binding : bindings_) {
      if (first) {
         first = false;
      } else {
         js_names += ",";
      }
      js_names += js::serialize(binding.first);
   }
   js_names += "]";

   return std::format(R"((function() {{
    'use strict';
    var methods = {};

    methods.forEach(function(name) {{
        window.__webview__.onBind(name);
    }});
}})())",
                      js_names);
}

struct message_t {
   std::string id_;
   std::string name_;
   std::string params_;

   static constexpr js::Proto prototype{
      js::_{"id", &message_t::id_},
      js::_{"method", &message_t::name_},
      js::_{"params", &message_t::params_}
   };
};

void
Webview::on_message(std::string_view msg_) {
   auto msg = js::parse<message_t>(msg_);

   auto const& create_promise = bindings_.at(std::string{msg.name_});

   dispatch([create_promise, msg = std::move(msg)]() {
      (*create_promise)(msg.id_, msg.params_);
   });
}

void
Webview::on_window_created() { inc_window_count(); }

void
Webview::on_window_destroyed(bool skip_termination) {
   if ((dec_window_count() <= 0) && !skip_termination) {
      terminate();
   }
}

std::atomic_uint&
Webview::window_ref_count() {
   static std::atomic_uint ref_count{0};
   return ref_count;
}

unsigned int
Webview::inc_window_count() { return ++window_ref_count(); }

unsigned int
Webview::dec_window_count() {
   auto& count = window_ref_count();
   if (count > 0) {
      return --count;
   }
   return 0;
}

}  // namespace webview
