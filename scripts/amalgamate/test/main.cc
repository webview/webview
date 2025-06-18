#include "../generated/webview_amalgamation.h"

int main() {
  webview_cc wv(false, nullptr);
  wv.terminate();
  wv.run();
  return 0;
}
