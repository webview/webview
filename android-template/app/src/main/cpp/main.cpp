#include "webview.h"

#ifdef ANDROID
int android_main(void *android_app) {
  webview::webview w(true, android_app);
#elif WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
  webview::webview w(true, nullptr);
#else
int main() {
  webview::webview w(true, nullptr);
#endif
  w.init("(function() { console.log('this is init 1, inside function'); })()");
  w.set_title("Minimal example"); // ignored, not implemented yet
  w.set_size(480, 320, WEBVIEW_HINT_NONE); // ignored, not implemented yet
  w.init("console.log('this is init 2');");
  w.bind("noop", [](std::string noopMsg) -> std::string {
    return noopMsg;
  });
  w.bind("foo", [&w](std::string fooMsg) -> std::string {
    w.eval("document.querySelector('h1').style.background = 'green';");
    return fooMsg;
  });
  w.init("console.log('this is init 3');");
  w.navigate("index.html");
  w.eval("console.log('this is eval');");
  w.run();
  return 0;
}
