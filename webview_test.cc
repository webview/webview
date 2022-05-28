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
  auto J = webview::detail::json_parse;
  // Valid input with expected output
  assert(J(R"({"foo":"bar"})", "foo", -1) == "bar");
  assert(J(R"({"foo":""})", "foo", -1) == "");
  assert(J(R"({"foo":{}")", "foo", -1) == "{}");
  assert(J(R"({"foo": {"bar": 1}})", "foo", -1) == R"({"bar": 1})");
  assert(J(R"(["foo", "bar", "baz"])", "", 0) == "foo");
  assert(J(R"(["foo", "bar", "baz"])", "", 2) == "baz");
  // Valid UTF-8 with expected output
  assert(J(R"({"フー":"バー"})", "フー", -1) == "バー");
  assert(J(R"(["フー", "バー", "バズ"])", "", 2) == "バズ");
  // Invalid input with valid output - should probably fail
  assert(J(R"({"foo":"bar")", "foo", -1) == "bar");
  // Valid input with other invalid parameters - should fail
  assert(J(R"([])", "", 0) == "");
  assert(J(R"({})", "foo", -1) == "");
  assert(J(R"(["foo", "bar", "baz"])", "", -1) == "");
  assert(J(R"(["foo"])", "", 1234) == "");
  assert(J(R"(["foo"])", "", -1234) == "");
  // Invalid input - should fail
  assert(J("", "", 0) == "");
  assert(J("", "foo", -1) == "");
  assert(J(R"({"foo":")", "foo", -1) == "");
  assert(J(R"({"foo":{)", "foo", -1) == "");
  assert(J(R"({"foo":{")", "foo", -1) == "");
  assert(J(R"(}")", "foo", -1) == "");
  assert(J(R"({}}")", "foo", -1) == "");
  assert(J(R"("foo)", "foo", -1) == "");
  assert(J(R"(foo)", "foo", -1) == "");
  assert(J(R"({{[[""foo""]]}})", "", 1234) == "");
  assert(J("bad", "", 0) == "");
  assert(J("bad", "foo", -1) == "");
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
// TEST: ensure that percent-encoding works.
// =================================================================
static void test_percent_encode() {
  using webview::detail::percent_encode;
  // Generate a string with all the possible ASCII characters
  std::string input(256, '\0');
  for (std::size_t i = 0; i < input.size(); ++i) {
    input[i] = static_cast<char>(i);
  }
  static auto output = "%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F"
                       "%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D%1E%1F"
                       "%20%21%22%23%24%25%26%27%28%29%2A%2B%2C%2D%2E%2F"
                       "%30%31%32%33%34%35%36%37%38%39%3A%3B%3C%3D%3E%3F"
                       "%40%41%42%43%44%45%46%47%48%49%4A%4B%4C%4D%4E%4F"
                       "%50%51%52%53%54%55%56%57%58%59%5A%5B%5C%5D%5E%5F"
                       "%60%61%62%63%64%65%66%67%68%69%6A%6B%6C%6D%6E%6F"
                       "%70%71%72%73%74%75%76%77%78%79%7A%7B%7C%7D%7E%7F"
                       "%80%81%82%83%84%85%86%87%88%89%8A%8B%8C%8D%8E%8F"
                       "%90%91%92%93%94%95%96%97%98%99%9A%9B%9C%9D%9E%9F"
                       "%A0%A1%A2%A3%A4%A5%A6%A7%A8%A9%AA%AB%AC%AD%AE%AF"
                       "%B0%B1%B2%B3%B4%B5%B6%B7%B8%B9%BA%BB%BC%BD%BE%BF"
                       "%C0%C1%C2%C3%C4%C5%C6%C7%C8%C9%CA%CB%CC%CD%CE%CF"
                       "%D0%D1%D2%D3%D4%D5%D6%D7%D8%D9%DA%DB%DC%DD%DE%DF"
                       "%E0%E1%E2%E3%E4%E5%E6%E7%E8%E9%EA%EB%EC%ED%EE%EF"
                       "%F0%F1%F2%F3%F4%F5%F6%F7%F8%F9%FA%FB%FC%FD%FE%FF";
  assert(percent_encode(input) == output);
  assert(percent_encode("foo") == "%66%6F%6F");
  assert(percent_encode("フー") == "%E3%83%95%E3%83%BC");
  assert(percent_encode("æøå") == "%C3%A6%C3%B8%C3%A5");
}

int main(int argc, char *argv[]) {
  std::unordered_map<std::string, std::function<void()>> all_tests = {
      {"terminate", test_terminate},
      {"c_api", test_c_api},
      {"bidir_comms", test_bidir_comms},
      {"json", test_json},
      {"percent_encode", test_percent_encode}};
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
      run_with_timeout(it->second, 60000);
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
