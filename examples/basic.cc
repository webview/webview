#include "webview.h"

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  auto options = webview::create_options_builder{}
                     .minimum_required_version(WEBVIEW_VERSION)
                     .build();
  webview::webview w(options);
  w.set_title("Basic Example");
  w.set_size(480, 320, WEBVIEW_HINT_NONE);
  w.set_html("Thanks for using webview!");
  w.run();
  return 0;
}
