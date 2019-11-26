// +build ignore

#include "webview.h"

#include <thread>
#include <cstring>
#include <cassert>

static void test_terminate() {
  printf("TEST: terminate app main loop\n");
  webview::webview w(false, nullptr);
  w.dispatch([&]() { w.terminate(); });
  w.run();
}

static void cb_assert_arg(webview_t w, void *arg) {
  assert(w != NULL);
  assert(memcmp(arg, "arg", 3) == 0);
}
static void cb_terminate(webview_t w, void *arg) {
  assert(arg == NULL);
  webview_terminate(w);
}
static void test_c_api() {
  webview_t w;
  printf("TEST: C API\n");
  w = webview_create(false, NULL);
  webview_set_bounds(w, 100, 100, 480, 320, 0);
  webview_set_title(w, "Test");
  webview_navigate(w, "https://github.com/zserge/webview");
  webview_dispatch(w, cb_assert_arg, (void *)"arg");
  webview_dispatch(w, cb_terminate, NULL);
  webview_run(w);
  webview_destroy(w);
}

int main() {
  std::atomic_flag flag_running = ATOMIC_FLAG_INIT;
  flag_running.test_and_set();
  std::thread timeout_thread([&](){
    for (int i = 0; i < 50; i++) {
      if (!flag_running.test_and_set()) {
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printf("Exiting due to a timeout.\n");
    exit(1);
  });
  test_terminate();
  test_c_api();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag_running.clear();
  timeout_thread.join();
  return 0;
}
