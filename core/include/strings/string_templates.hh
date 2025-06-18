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

#ifndef WEBVIEW_STRINGS_STRING_TEMPLATES_HH
#define WEBVIEW_STRINGS_STRING_TEMPLATES_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "errors/errors.hh"
#include "lib/macros.h"
#include <string>

namespace webview {
namespace _lib {
namespace _templates {

/* JS
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

const std::string &TEMPLATE_POST_FUNCTION() {

#if defined(WEBVIEW_PLATFORM_LINUX) || defined(WEBVIEW_PLATFORM_DARWIN)
  static std::string tmplt =
      "return window.webkit.messageHandlers.__webview__.postMessage(message);";
  return tmplt;
#elif defined(WEBVIEW_PLATFORM_WINDOWS)
  static std::string tmplt =
      "return window.chrome.webview.postMessage(message);";
  return tmplt;
#else
  throw errors::exception(WEBVIEW_ERROR_UNSPECIFIED,
                          "Failed to post function for unknown OS.");
#endif
}

const std::string &TEMPLATE_WEVBIEW_INIT_JS() {
  static std::string tmplt = R"(
(function () {
  "use strict";
  function generateId() {
    var crypto = window.crypto || window.msCrypto;
    var bytes = new Uint8Array(16);
    crypto.getRandomValues(bytes);
    return Array.prototype.slice
      .call(bytes)
      .map(function (n) {
        var s = n.toString(16);
        return (s.length % 2 == 1 ? "0" : "") + s;
      })
      .join("");
  }

    const sandboxHtml = `
<!DOCTYPE html>
<html>
<head>
  <script>
    window.addEventListener('message', function (e) {
      let isExecutable;
      try {
        result = eval(e.data);
        isExecutable = true;
      } catch (err) {
        isExecutable = false;
      }
      if(isExecutable) return e.source.postMessage('executable', '*');
      try {
        const decodedString = decodeURIComponent(e.data);
        result = eval(decodedString);
        isExecutable = true;
      } catch (err) {
        isExecutable = false;
      }
      if(isExecutable) {
        e.source.postMessage('executable', '*');
      } else {
        e.source.postMessage('not executable', '*');
      }
    });
  </script>
</head>
<body></body>
</html>
  `;

  var Webview = (function () {
    var _promises = {};
    function Webview_() {}

    Webview_.prototype.post = function (message) {
      _post_fn_;
    };

    Webview_.prototype.sysop = function (command) {
      this.post(
        JSON.stringify({
          id: "_sysop",
          method: command,
          params: [],
        })
      );
    };

    Webview_.prototype.call = function (method) {
      var _id = generateId();
      var _params = Array.prototype.slice.call(arguments, 1);
      var promise = new Promise(function (resolve, reject) {
        _promises[_id] = { resolve, reject };
      });
      this.post(
        JSON.stringify({
          id: _id,
          method: method,
          params: _params,
        })
      );
      return promise;
    };

    Webview_.prototype.unsafeError = function (type, result) {
      return `Unsafe ${type} passed to a binding return:
"${result}"
Consider "\"double quoting\"" your input string if this was intentional`;
    };

    Webview_.prototype.parseJSON = function (result) {
      try {
        return JSON.parse(result);
      } catch (err) {
        return false;
      }
    };

    Webview_.prototype.parseNumber = function (result) {
      const num = Number(result);
      return Number.isNaN(num) ? false : num;
    };

    Webview_.prototype.scriptRegex = /<script\b[^>]*>([\s\S]*?)<\/script>/gi;
    Webview_.prototype.containsHTMLScript = function (result) {
      const decoded_result = decodeURIComponent(result);
      return !!decoded_result.match(this.scriptRegex);
    };

    Webview_.prototype.jsCodeSandbox = function () {
      const iframe = document.createElement("iframe");
      iframe.hidden = true;
      iframe.sandbox = "allow-scripts";
      iframe.src = `data:text/html;charset=utf-8,${encodeURIComponent(sandboxHtml)}`;
      document.body.appendChild(iframe);
      return iframe;
    };

    Webview_.prototype.resolver = function (status, id, result) {
      var promise = _promises[id];
      if (status === 0) {
        result === undefined ? promise.resolve() : promise.resolve(result);
      } else {
        result === undefined ? promise.reject() : promise.reject(result);
      }
    };

    Webview_.prototype.onReply = function (id, status, result) {
      let val;

      // parse for bool or undefined
      if (result === undefined) return this.resolver(status, id, result);
      if (result === "true") return this.resolver(status, id, true);
      if (result === "false") return this.resolver(status, id, false);

      // parse for Number
      val = this.parseNumber(result);
      if (val !== false) return this.resolver(status, id, val);

      // parse for JSON
      val = this.parseJSON(result);
      if (!!val) return this.resolver(status, id, val);
      console.log("Parsing for script tags:", result)
      // parse for script tags
      if (this.containsHTMLScript(result)) return this.resolver(1, id, this.unsafeError("HTML with <script>", result));

      // check for JS code in sandbox
      const iframe = this.jsCodeSandbox();
      const messageHandler = ("message", (e) => {
        if (e.source !== iframe.contentWindow) return;
        switch (e.data) {
          case "executable":
            this.resolver(1, id, this.unsafeError("evaluable JS", result));
            break;
          case "not executable":
            this.resolver(status, id, result);
            break;
        }
        document.body.removeChild(iframe);
        window.removeEventListener("message", messageHandler);
      });
      window.addEventListener("message", messageHandler);
      iframe.addEventListener("load", () => {
        iframe.contentWindow.postMessage(result, "*");
      });

    };

    Webview_.prototype.onBind = function (name) {
      if (window.hasOwnProperty(name)) {
        throw new Error(`Binding '${name}' already exists`);
      }
      window[name] = function () {
        var params = [name].concat(Array.prototype.slice.call(arguments));
        return Webview_.prototype.call.apply(this, params);
      }.bind(this);
    };

    Webview_.prototype.onUnbind = function (name) {
      if (!window.hasOwnProperty(name)) {
        throw new Error(`Binding '${name}' does not exist`);
      }
      delete window[name];
    };
    return Webview_;
  })();

  window.__webview__ = new Webview();
  
})();
)";
  return tmplt;
}

const std::string &TEMPLATE_ON_BIND_JS() {
  static std::string tmplt = R"(
if (window.__webview__) {
  try {
    window.__webview__.onBind('_str_');
  } catch(err) {
    console.error(err);
  } finally {
    window.__webview__.sysop('_bind_done');
  }
}
)";
  return tmplt;
}

const std::string &TEMPLATE_ON_UNBIND_JS() {
  static std::string tmplt = R"(
if (window.__webview__) {
  try {
    window.__webview__.onUnbind('_str_');
  } catch (err) {
    console.error(err);
  } finally {
    window.__webview__.sysop('_unbind_done');
  }
}
)";
  return tmplt;
}

const std::string &TEMPLATE_ON_REPLY_JS() {
  static std::string tmplt = R"(
window.__webview__.onReply('_id_', _status_, _result_)
)";
  return tmplt;
}

const std::string &TEMPLATE_BIND_JS() {
  static std::string tmplt = R"(
(function() {
  'use strict';
  var methods = _js_names_;
  methods.forEach(function(name) {
    window.__webview__.onBind(name);
  });
})()
)";
  return tmplt;
}

const std::string &TEMPLATE_EVAL_WRAPPER_JS() {
  static std::string tmplt = R"(
try {
  // User JS
  // ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
_user_js_
  // ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
  // User JS

} catch (err) {
  console.error(err);
} finally {
  window.__webview__.sysop('_frontend_eval_ready');
}
)";
  return tmplt;
}

const std::string &TEMPLATE_USER_INIT_WRAPPER_JS() {
  static std::string tmplt = R"(
try {
  // User JS
  // ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
_user_js_
  // ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
  // User JS

} catch (err) {
  console.error(err);
}
)";
  return tmplt;
}

/* Message strings
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

const std::string &TEMPLATE_INVALID_ARG_M() {
  static std::string tmplt = "Invlaid `_var_` argument passed to `_str_`";
  return tmplt;
}

const std::string &TEMPLATE_MISSING_ARG_M() {
  static std::string tmplt = "Missing `_var_` argument passed to `_str_`";
  return tmplt;
}

const std::string &TEMPLATE_NULL_W_M() {
  static std::string tmplt = "Cannot cast null pointer in argument `w` to a "
                             "webview instance for `_str_`.";
  return tmplt;
}

const std::string &TEMPLATE_REJECT_UNBOUND_M() {
  static std::string tmplt =
      "Promise id '_id_' was rejected because function '_str_' was unbound.";
  return tmplt;
}

const std::string &TEMPLATE_UNCAUGHT_EXP_M() {
  static std::string tmplt =
      R"(Uncaught exception from native user callback function '_str_':
_what_)";
  return tmplt;
}
const std::string &TEMPLATE_WEBVIEW_TERMINATED_M() {
  static std::string tmplt = "Native user callback function '_str_' failed "
                             "because Webview terminated "
                             "before it could complete.";
  return tmplt;
}

} // namespace _templates
} // namespace _lib
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_STRINGS_STRING_TEMPLATES_HH
