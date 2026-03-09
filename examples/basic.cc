#include "webview/webview.h"

#include <iostream>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
#ifndef WEBVIEW_HAS_NO_EXCEPTIONS
  try {
#endif
    webview::webview w(false, nullptr);
    w.set_title("Basic Example");
    w.set_size(480, 320, WEBVIEW_HINT_NONE);
    w.set_html("Thanks for using webview!");
    w.run();
#ifndef WEBVIEW_HAS_NO_EXCEPTIONS
  } catch (const webview::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
#endif

  return 0;
}
