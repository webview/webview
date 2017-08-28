#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

#include "../webview.h"

static void usage(char *app) { printf("USAGE: %s <html|url>\n", app); }

static void external_invoke_cb() { printf("external_invoke_cb()\n"); }

int main(int argc, char *argv[]) {
  int opt;
  struct webview webview = {
      .title = "WebView example",
      .width = 800,
      .height = 600,
      .resizable = 0,
      .external_invoke_cb = external_invoke_cb,
  };

  while ((opt = getopt(argc, argv, "w:h:r")) != -1) {
    switch (opt) {
    case 'w':
      webview.width = atoi(optarg);
      break;
    case 'h':
      webview.height = atoi(optarg);
      break;
    case 'r':
      webview.resizable = 1;
      break;
    default:
      usage(argv[0]);
      return 0;
    }
  }

  if (optind != argc - 1) {
    usage(argv[0]);
    return 1;
  }

  webview.url = argv[optind];

  webview_init(&webview);

  while (webview_loop(&webview, 1) == 0)
    ;
  printf("exiting...\n");
  webview_exit(&webview);
  return 0;
}
