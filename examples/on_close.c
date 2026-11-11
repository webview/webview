#include "webview/webview.h"
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#endif

static int on_close(void) {
#ifdef _WIN32
  int result = MessageBoxW(NULL, L"Are you sure you want to close?",
                           L"Confirm Close", MB_YESNO | MB_ICONQUESTION);
  return result == IDYES;
#else
  // On macOS/Linux, a real app would show a native dialog.
  // For this example, we always allow closing.
  return 1;
#endif
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
  (void)hInst;
  (void)hPrevInst;
  (void)lpCmdLine;
  (void)nCmdShow;
#else
int main(void) {
#endif
  webview_t w = webview_create(0, NULL);
  webview_set_title(w, "On Close Example");
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
  webview_set_on_close(w, on_close);
  webview_set_html(w, "Close this window to see the confirmation dialog.");
  webview_run(w);
  webview_destroy(w);
  return 0;
}
