#include "webview.h"

int main() {
  webview::webview w(false, nullptr);
  w.set_title("Example");
  w.set_size(480, 320, true);
  w.navigate("https://github.com/zserge/webview");
  w.run();
  return 0;
}
