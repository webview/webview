#include "bind_lib.h"
#include "webview.h"
#include <chrono>
#include <iostream>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
  try {
    webview_cc wv(true, nullptr);
    int count = 0;

    wv.set_title("Bind Example");
    wv.set_size(480, 320, WEBVIEW_HINT_NONE);

    // A binding that counts up or down and immediately returns the new value.
    wv.bind(
        "count",
        [&](const std::string &id, const std::string &req, void * /*arg*/) {
          // We use the inbuilt Webview JSON utility from webview::strings::json
          auto increment = std::stoi(json::parse(req, "", 0));
          wv.resolve(id, 0, count += increment);
        },
        nullptr);

    // A binding that does computational work and returns the result at a later time.
    wv.bind(
        "compute",
        [&](const std::string &id, const std::string & /*req*/,
            void * /*arg*/) {
          // Simulate load.
          std::this_thread::sleep_for(std::chrono::seconds(3));
          wv.resolve(id, 0, answer_machine_result());
        },
        nullptr);

    wv.set_html(WEBVIEW_BIND_EXAMPLE_HTML);
    wv.run();
  } catch (const webview::errors::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
