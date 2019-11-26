// +build ignore

#include "webview.h"

#include <thread>
#include <unordered_map>
#include <cstring>
#include <cassert>

// =================================================================
// TEST: start app loop and terminate it.
// =================================================================
static void test_terminate() {
  webview::webview w(false, nullptr);
  w.dispatch([&]() { w.terminate(); });
  w.run();
}

// =================================================================
// TEST: use C API to create a window, run app and terminate it.
// =================================================================
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
  w = webview_create(false, NULL);
  webview_set_bounds(w, 100, 100, 480, 320, 0);
  webview_set_title(w, "Test");
  webview_navigate(w, "https://github.com/zserge/webview");
  webview_dispatch(w, cb_assert_arg, (void *)"arg");
  webview_dispatch(w, cb_terminate, NULL);
  webview_run(w);
  webview_destroy(w);
}

static void run_with_timeout(std::function<void()> fn, int timeout_ms) {
  std::atomic_flag flag_running = ATOMIC_FLAG_INIT;
  flag_running.test_and_set();
  std::thread timeout_thread([&](){
    for (int i = 0; i < timeout_ms / 100; i++) {
      if (!flag_running.test_and_set()) {
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printf("Exiting due to a timeout.\n");
    exit(1);
  });
  fn();
  flag_running.clear();
  timeout_thread.join();
}

int main(int argc, char *argv[]) {
  std::unordered_map<std::string, std::function<void()>> all_tests = {
    {"terminate", test_terminate},
    {"c_api", test_c_api},
  };
  // Without arguments run all tests, one-by-one by forking itself.
  // With a single argument - run the requested test
  if (argc == 1) {
    int failed = 0;
    for (auto test : all_tests) {
      printf("TEST: %s\n", test.first.c_str());
      int status = system((std::string(argv[0]) + " " + test.first).c_str());
      if (status == 0) {
        printf("  PASS\n");
      } else {
        printf("  FAIL: %d\n", status);
        failed = 1;
      }
    }
    return failed;
  }

  if (argc == 2) {
    auto it = all_tests.find(argv[1]);
    if (it != all_tests.end()) {
      run_with_timeout(it->second, 1000);
      return 0;
    }
  }
  printf("USAGE: %s [test name]\n", argv[0]);
  printf("Tests:\n");
  for (auto test : all_tests) {
    printf("  %s\n", test.first.c_str());
  }
  return 1;
}
