#include "webview.h"
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  webview_t w;
  webview_create_options_t options = {.minimum_required_version =
                                          WEBVIEW_VERSION};
  webview_create_with_options(&w, &options);
  webview_set_title(w, "Basic Example");
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
  webview_set_html(w, "Thanks for using webview!");
  webview_set_visibility(w, WEBVIEW_TRUE);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
