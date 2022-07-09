#include "webview.h"
#include <string.h>

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  webview_t w;
  webview_create_options_t options = {
      .struct_size = sizeof(options),
      .api_version = {WEBVIEW_API_MAJOR_VERSION, WEBVIEW_API_MINOR_VERSION,
                      WEBVIEW_API_PATCH_VERSION}};
  webview_create_with_options(&w, &options);
  webview_set_title(w, "Basic Example");
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
  webview_set_html(w, "Thanks for using webview!");
  webview_run(w);
  webview_destroy(w);
  return 0;
}
