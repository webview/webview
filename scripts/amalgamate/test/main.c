#include "webview_amalgamation.h"
#include <stddef.h>

int main(void) {
  webview_t w = webview_create(0, NULL);
  webview_destroy(w);
  return 0;
}
