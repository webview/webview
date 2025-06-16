#include "../generated/webview_amalgamation.h"
#include <stddef.h>

int main(void) {
  webview_t w = webview_create(0, NULL);
  webview_terminate(w);
  webview_run(w);
  return 0;
}
