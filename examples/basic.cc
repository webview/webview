#include "webview.h"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  webview::webview w(false, nullptr);
  w.set_title("Basic Example");
  w.set_size(480, 320, WEBVIEW_HINT_NONE);
  w.set_html("Thanks for using webview!");
  w.run();
  return 0;
}
