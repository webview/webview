#include "webview.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#endif

// Only used to suppress warnings caused by unused parameters.
#define UNUSED(x) (void)x

typedef struct {
  webview_t w;
  unsigned int count;
} context_t;

static const char html[] =
    "<button id=\"increment\">Tap me</button>\n"
    "<div>You tapped <span id=\"count\">0</span> time(s).</div>\n"
    "<script>\n"
    "  const [incrementElement, countElement] =\n"
    "    document.querySelectorAll(\"#increment, #count\");\n"
    "  document.addEventListener(\"DOMContentLoaded\", () => {\n"
    "    incrementElement.addEventListener(\"click\", () => {\n"
    "      window.increment().then(result => {\n"
    "        countElement.textContent = result.count;\n"
    "      });\n"
    "    });\n"
    "  });\n"
    "</script>";

void increment(const char *seq, const char *req, void *arg) {
  UNUSED(req);
  context_t *context = (context_t *)arg;
  char count_string[10] = {0};
  sprintf(count_string, "%u", ++context->count);
  char result[21] = {0};
  strcat(result, "{\"count\": ");
  strcat(result, count_string);
  strcat(result, "}");
  webview_return(context->w, seq, 0, result);
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  webview_t w = webview_create(0, NULL);
  context_t context = {.w = w, .count = 0};
  webview_set_title(w, "Bind Example");
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
  webview_bind(w, "increment", increment, &context);
  webview_set_html(w, html);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
