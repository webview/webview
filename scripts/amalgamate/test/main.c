#include "../generated/webview_amalgamation.h"
#include <stddef.h>

int main(void) {
  webview_t w = webview_create(0, NULL);
  webview_terminate(w);
  webview_run(w);
  // We temporarily leave this deprecated API call here
  // to confirm that a clean shut-down has not been affected.
  IGNORE_DEPRECATED_DECLARATIONS
  webview_destroy(w);
  RESTORE_IGNORED_WARNINGS
  return 0;
}
