#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

#include "../webview.h"

static void usage(char *app) { printf("USAGE: %s <html|url>\n", app); }

int main(int argc, char *argv[]) {
  int w = 800;
  int h = 600;
  int resize = 0;
  int opt;

  while ((opt = getopt(argc, argv, "w:h:r")) != -1) {
    switch (opt) {
    case 'w':
      w = atoi(optarg);
      break;
    case 'h':
      h = atoi(optarg);
      break;
    case 'r':
      resize = 1;
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

  webview("WebView example", argv[optind], w, h, resize);
  printf("exiting...\n");
  return 0;
}
