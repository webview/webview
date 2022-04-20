//bin/echo; [ $(uname) = "Darwin" ] && FLAGS="-framework Webkit" || FLAGS="$(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)" ; c++ "$0" $FLAGS -std=c++11 -Wall -Wextra -pedantic -g -o webview_test && ./webview_test ; exit
// +build ignore

#include "webview.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
#include <unordered_map>

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
  assert(w != nullptr);
  assert(memcmp(arg, "arg", 3) == 0);
}
static void cb_terminate(webview_t w, void *arg) {
  assert(arg == nullptr);
  webview_terminate(w);
}
static void test_c_api() {
  webview_t w;
  w = webview_create(false, nullptr);
  webview_set_size(w, 480, 320, 0);
  webview_set_title(w, "Test");
  webview_navigate(w, "https://github.com/zserge/webview");
  webview_dispatch(w, cb_assert_arg, (void *)"arg");
  webview_dispatch(w, cb_terminate, nullptr);
  webview_run(w);
  webview_destroy(w);
}

// =================================================================
// TEST: ensure that JS code can call native code and vice versa.
// =================================================================
struct test_webview : webview::browser_engine {
  using cb_t = std::function<void(test_webview *, int, const std::string &)>;
  test_webview(cb_t cb) : webview::browser_engine(true, nullptr), m_cb(cb) {}
  void on_message(const std::string &msg) override { m_cb(this, i++, msg); }
  int i = 0;
  cb_t m_cb;
};

static void test_bidir_comms() {
  test_webview browser([](test_webview *w, int i, const std::string &msg) {
    std::cout << msg << std::endl;
    switch (i) {
    case 0:
      assert(msg == "loaded");
      w->eval("window.external.invoke('exiting ' + window.x)");
      break;
    case 1:
      assert(msg == "exiting 42");
      w->terminate();
      break;
    default:
      assert(0);
    }
  });
  browser.init(R"(
    window.x = 42;
    window.onload = () => {
      window.external.invoke('loaded');
    };
  )");
  browser.navigate("data:text/html,%3Chtml%3Ehello%3C%2Fhtml%3E");
  browser.run();
}

// =================================================================
// TEST: ensure that JSON parsing works.
// =================================================================
static void test_json() {
  auto J = webview::json_parse;
  assert(J(R"({"foo":"bar"})", "foo", -1) == "bar");
  assert(J(R"({"foo":""})", "foo", -1) == "");
  assert(J(R"({"foo": {"bar": 1}})", "foo", -1) == R"({"bar": 1})");
  assert(J(R"(["foo", "bar", "baz"])", "", 0) == "foo");
  assert(J(R"(["foo", "bar", "baz"])", "", 2) == "baz");
}

static void run_with_timeout(std::function<void()> fn, int timeout_ms) {
  std::atomic_flag flag_running = ATOMIC_FLAG_INIT;
  flag_running.test_and_set();
  std::thread timeout_thread([&]() {
    for (int i = 0; i < timeout_ms / 100; i++) {
      if (!flag_running.test_and_set()) {
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Exiting due to a timeout." << std::endl;
    exit(1);
  });
  fn();
  flag_running.clear();
  timeout_thread.join();
}

// =================================================================
// TEST: ensure that hex2nibble works.
// =================================================================
static void test_hex2nibble() {
  using namespace webview;
  static const std::pair<char, unsigned char> alphabet[]{
      {'0', 0x0}, {'1', 0x1}, {'2', 0x2}, {'3', 0x3}, {'4', 0x4}, {'5', 0x5},
      {'6', 0x6}, {'7', 0x7}, {'8', 0x8}, {'9', 0x9}, {'a', 0xA}, {'b', 0xB},
      {'c', 0xC}, {'d', 0xD}, {'e', 0xE}, {'f', 0xF}, {'A', 0xA}, {'B', 0xB},
      {'C', 0xC}, {'D', 0xD}, {'E', 0xE}, {'F', 0xF}, {'x', 0x0}};
  for (const auto &kv : alphabet) {
    assert(hex2nibble(kv.first) == kv.second);
  }
}

// =================================================================
// TEST: ensure that hex2byte works.
// =================================================================
static void test_hex2byte() {
  using namespace webview;
  static const std::pair<const char *, unsigned char> alphabet[]{
      {"00", 0x00}, {"12", 0x12}, {"34", 0x34}, {"56", 0x56}, {"78", 0x78},
      {"9A", 0x9A}, {"BC", 0xBC}, {"DE", 0xDE}, {"FF", 0xFF}, {"ab", 0xAB},
      {"cd", 0xCD}, {"ef", 0xEF}, {"0x", 0x00}, {"x0", 0x00}, {"xx", 0x00}};
  for (const auto &kv : alphabet) {
    assert(hex2byte(kv.first) == kv.second);
  }
}

// =================================================================
// TEST: ensure that hex2char works.
// =================================================================
static void test_hex2char() {
  using namespace webview;
  static const std::pair<const char *, char> alphabet[]{
      {"00", '\0'}, {"30", '0'}, {"39", '9'},    {"41", 'A'}, {"5A", 'Z'},
      {"61", 'a'},  {"7A", 'z'}, {"FF", '\xff'}, {"xx", '\0'}};
  for (const auto &kv : alphabet) {
    assert(hex2char(kv.first) == kv.second);
  }
}

int main(int argc, char *argv[]) {
  std::unordered_map<std::string, std::function<void()>> all_tests = {
      {"terminate", test_terminate},     {"c_api", test_c_api},
      {"bidir_comms", test_bidir_comms}, {"json", test_json},
      {"hex2nibble", test_hex2nibble},   {"hex2byte", test_hex2byte},
      {"hex2char", test_hex2char},
  };
  // Without arguments run all tests, one-by-one by forking itself.
  // With a single argument - run the requested test
  if (argc == 1) {
    int failed = 0;
    for (const auto &test : all_tests) {
      std::cout << "TEST: " << test.first << std::endl;
      int status = system((std::string(argv[0]) + " " + test.first).c_str());
      if (status == 0) {
        std::cout << "  PASS " << std::endl;
      } else {
        std::cout << "  FAIL: " << status << std::endl;
        failed = 1;
      }
    }
    return failed;
  }

  if (argc == 2) {
    auto it = all_tests.find(argv[1]);
    if (it != all_tests.end()) {
      run_with_timeout(it->second, 5000);
      return 0;
    }
  }
  std::cout << "USAGE: " << argv[0] << " [test name]" << std::endl;
  std::cout << "Tests: " << std::endl;
  for (const auto &test : all_tests) {
    std::cout << "  " << test.first << std::endl;
  }
  return 1;
}
