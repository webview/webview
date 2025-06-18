#ifndef WEBVIEW_EXAMPLES_BIND_LIB
#define WEBVIEW_EXAMPLES_BIND_LIB

#define WEBVIEW_BIND_EXAMPLE_HTML                                              \
  "<div>\n"                                                                    \
  "  <button id=\"increment\">+</button>\n"                                    \
  "  <button id=\"decrement\">−</button>\n"                                  \
  "  <span>Counter: <span id=\"counterResult\">0</span></span>\n"              \
  "</div>\n"                                                                   \
  "<hr />\n"                                                                   \
  "<div>\n"                                                                    \
  "  <button id=\"compute\">Compute</button>\n"                                \
  "  <span>Result: <span id=\"computeResult\">(not started)</span></span>\n"   \
  "</div>\n"                                                                   \
  "<script type=\"module\">\n"                                                 \
  "  const getElements = ids => Object.assign({}, ...ids.map(\n"               \
  "    id => ({ [id]: document.getElementById(id) })));\n"                     \
  "    const ui = getElements([\n"                                             \
  "    \"increment\", \"decrement\", \"counterResult\", \"compute\",\n"        \
  "    \"computeResult\"\n"                                                    \
  "  ]);\n"                                                                    \
  "  ui.increment.addEventListener(\"click\", async () => {\n"                 \
  "    ui.counterResult.textContent = await window.count(1);\n"                \
  "  });\n"                                                                    \
  "  ui.decrement.addEventListener(\"click\", async () => {\n"                 \
  "    ui.counterResult.textContent = await window.count(-1);\n"               \
  "  });\n"                                                                    \
  "  ui.compute.addEventListener(\"click\", async () => {\n"                   \
  "    ui.compute.disabled = true;\n"                                          \
  "    ui.computeResult.innerHTML = \"(pending)\";\n"                          \
  "    ui.computeResult.innerHTML = await window.compute();\n"                 \
  "    ui.compute.disabled = false;\n"                                         \
  "  });\n"                                                                    \
  "</script>"

#define ANSWER_MACHINE_HTML                                                    \
  "<br><p><strong>42:</strong> is the answer.</p><p>Would you like to "        \
  "know the question, Dave?</p>"

#if defined(__cplusplus)
#include <random>
#include <string>

std::string answer_machine_result() { return ANSWER_MACHINE_HTML; }

#else // !defined(__cplusplus)
#ifdef _WIN32
#include <windows.h>

#include <synchapi.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Make the current thread sleep for the given number of seconds.
void thread_sleep(int seconds) {
#ifdef _WIN32
  Sleep(seconds * 1000);
#else
  sleep(seconds);
#endif
}

int int_length(int n) {
  int count = 0;
  if (n == 0) {
    return 2;
  }
  if (n < 0) {
    count++;
    n = abs(n);
  }
  while (n != 0) {
    n /= 10;
    count++;
  }
  return count + 1;
}

int to_int(void *str_) {
  const char *str = (const char *)str_;
  int val = atoi(str); // NOLINT(cert-err34-c)
  return val;
}

char *to_string(int value) {
  int len = snprintf(NULL, 0, "%d", value);
  char *buffer = malloc(len + 1);
  (void)sprintf(buffer, "%d", value);
  return buffer;
}

char *answer_machine_result(void) { return ANSWER_MACHINE_HTML; }

#endif // !defined(__cplusplus)
#endif // WEBVIEW_EXAMPLES_BIND_LIB