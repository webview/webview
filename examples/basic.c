#include "webview.h"
#include <stddef.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  webview_t w = webview_create(0, NULL);
  webview_set_title(w, "Basic Example");
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
  webview_set_html(w, "Thanks for using webview!");
  webview_run(w);
  webview_destroy(w);
  return 0;
}
