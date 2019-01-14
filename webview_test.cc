#include "webview.h"

static void test_terminate() {
  webview::webview w("", false, nullptr);
  w.dispatch([&]() { w.terminate(); });
  w.run();
}

int main() {
  test_terminate();
  return 0;
}
