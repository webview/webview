#include "webview/webview.h"

#ifdef _WIN32
#include <windows.h>
#endif

static int on_close() {
#ifdef _WIN32
  int result = MessageBoxW(nullptr, L"Are you sure you want to close?",
                           L"Confirm Close", MB_YESNO | MB_ICONQUESTION);
  return result == IDYES;
#else
  // On macOS/Linux, a real app would show a native dialog.
  // For this example, we always allow closing.
  return 1;
#endif
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
  try {
    webview::webview w(false, nullptr);
    w.set_title("On Close Example");
    w.set_size(480, 320, WEBVIEW_HINT_NONE);
    w.set_on_close(on_close);
    w.set_html("Close this window to see the confirmation dialog.");
    w.run();
  } catch (const webview::exception &e) {
    return 1;
  }

  return 0;
}
