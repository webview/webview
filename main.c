#include "webview.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("USAGE: %s <html|url>\n", argv[0]);
    return 1;
  }
  int r = webview("WebView example", argv[1], 800, 600, 1);
  printf("exiting...\n");
  return r;
}
