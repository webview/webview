#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace webview {

namespace http {

struct response_t {
   std::vector<char>                                 body;
   std::string                                       reasonPhrase;
   int                                               statusCode;
   std::unordered_multimap<std::string, std::string> headers;
};

struct request_t {
   std::function<std::string()>                      getContent;
   std::string                                       uri;
   std::string                                       method;
   std::unordered_multimap<std::string, std::string> headers;
};

}  // namespace http
}  // namespace webview