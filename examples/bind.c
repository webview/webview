#include "webview.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

// Only used to suppress warnings caused by unused parameters.
#define UNUSED(x) (void)x

// Creates a thread with the given start routine and argument passed to
// the start routine. Returns 0 on success and -1 on failure.
int thread_create(void *(*start_routine)(void *), void *arg) {
#ifdef _WIN32
  HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start_routine,
                               arg, 0, NULL);
  if (thread) {
    CloseHandle(thread);
    return 0;
  }
  return -1;
#else
  pthread_t thread;
  int error = pthread_create(&thread, NULL, start_routine, arg);
  if (error == 0) {
    pthread_detach(thread);
    return 0;
  }
  return -1;
#endif
}

// Make the current thread sleep for the given number of seconds.
void thread_sleep(int seconds) {
#ifdef _WIN32
  Sleep(seconds * 1000);
#else
  sleep(seconds);
#endif
}

typedef struct {
  webview_t w;
  unsigned int count;
} context_t;

static const char html[] =
    "<button id=\"increment\">Tap me</button>\n"
    "<div>You tapped <span id=\"count\">0</span> time(s).</div>\n"
    "<button id=\"compute\">Compute</button>\n"
    "<div>Result of computation: <span id=\"compute-result\">0</span></div>\n"
    "<script>\n"
    "  const [incrementElement, countElement, computeElement, "
    "computeResultElement] =\n"
    "    document.querySelectorAll(\"#increment, #count, #compute, "
    "#compute-result\");\n"
    "  document.addEventListener(\"DOMContentLoaded\", () => {\n"
    "    incrementElement.addEventListener(\"click\", () => {\n"
    "      window.increment().then(result => {\n"
    "        countElement.textContent = result.count;\n"
    "      });\n"
    "    });\n"
    "    computeElement.addEventListener(\"click\", () => {\n"
    "      computeElement.disabled = true;\n"
    "      window.compute(6, 7).then(result => {\n"
    "        computeResultElement.textContent = result;\n"
    "        computeElement.disabled = false;\n"
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

typedef struct {
  webview_t w;
  char *seq;
  char *req;
} compute_thread_params_t;

compute_thread_params_t *
compute_thread_params_create(webview_t w, const char *seq, const char *req) {
  compute_thread_params_t *params =
      (compute_thread_params_t *)malloc(sizeof(compute_thread_params_t));
  params->w = w;
  params->seq = (char *)malloc(strlen(seq) + 1);
  params->req = (char *)malloc(strlen(req) + 1);
  strcpy(params->seq, seq);
  strcpy(params->req, req);
  return params;
}

void compute_thread_params_free(compute_thread_params_t *p) {
  free(p->req);
  free(p->seq);
  free(p);
}

void *compute_thread_proc(void *arg) {
  compute_thread_params_t *params = (compute_thread_params_t *)arg;
  // Simulate load.
  thread_sleep(1);
  // Either imagine that params->req is parsed here or use your own JSON parser.
  const char *result = "42";
  webview_return(params->w, params->seq, 0, result);
  compute_thread_params_free(params);
  return NULL;
}

void compute(const char *seq, const char *req, void *arg) {
  context_t *context = (context_t *)arg;
  compute_thread_params_t *params =
      compute_thread_params_create(context->w, seq, req);
  // Create a thread and forget about it for the sake of simplicity.
  if (thread_create(compute_thread_proc, params) != 0) {
    compute_thread_params_free(params);
  }
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
  (void)hInst;
  (void)hPrevInst;
  (void)lpCmdLine;
  (void)nCmdShow;
#else
int main() {
#endif
  webview_t w = webview_create(0, NULL);
  context_t context = {.w = w, .count = 0};
  webview_set_title(w, "Bind Example");
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);

  // A binding that increments a value and immediately returns the new value.
  webview_bind(w, "increment", increment, &context);

  // An binding that creates a new thread and returns the result at a later time.
  webview_bind(w, "compute", compute, &context);

  webview_set_html(w, html);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
