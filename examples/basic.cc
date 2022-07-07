#include "webview.h"

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  webview_create_options_t options{};
  options.struct_size = sizeof(options);
  options.api_version = webview::api_version;
  webview::webview w(options);
  w.set_title("Basic Example");
  w.set_size(480, 320, WEBVIEW_HINT_NONE);
  w.set_html("Thanks for using webview!");
  w.run();
  return 0;
}
