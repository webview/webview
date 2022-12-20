//bin/echo; [ $(uname) = "Darwin" ] && FLAGS="-framework Webkit" || FLAGS="$(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)" ; c++ "$0" $FLAGS -std=c++11 -Wall -Wextra -pedantic -g -o webview_test && ./webview_test ; exit
// +build ignore

#define WEBVIEW_VERSION_MAJOR 1
#define WEBVIEW_VERSION_MINOR 2
#define WEBVIEW_VERSION_PATCH 3
#define WEBVIEW_VERSION_PRE_RELEASE "-test"
#define WEBVIEW_VERSION_BUILD_METADATA "+gaabbccd"

#include "webview.h"

#include <cassert>
#include <cstdint>
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
// TEST: use C API to test binding and unbinding.
// =================================================================

static void test_c_api_bind() {
  struct context_t {
    webview_t w;
    unsigned int number;
  } context{};
  auto test = +[](const char *seq, const char *req, void *arg) {
    auto increment =
        +[](const char * /*seq*/, const char * /*req*/, void *arg) {
          ++static_cast<context_t *>(arg)->number;
        };
    auto context = static_cast<context_t *>(arg);
    std::string req_(req);
    // Bind and increment number.
    if (req_ == "[0]") {
      assert(context->number == 0);
      webview_bind(context->w, "increment", increment, context);
      webview_return(
          context->w, seq, 0,
          "(() => {try{window.increment()}catch{}window.test(1)})()");
      return;
    }
    // Unbind and make sure that we cannot increment even if we try.
    if (req_ == "[1]") {
      assert(context->number == 1);
      webview_unbind(context->w, "increment");
      webview_return(
          context->w, seq, 0,
          "(() => {try{window.increment()}catch{}window.test(2)})()");
      return;
    }
    // Number should not have changed but we can bind again and change the number.
    if (req_ == "[2]") {
      assert(context->number == 1);
      webview_bind(context->w, "increment", increment, context);
      webview_return(
          context->w, seq, 0,
          "(() => {try{window.increment()}catch{}window.test(3)})()");
      return;
    }
    // Finish test.
    if (req_ == "[3]") {
      assert(context->number == 2);
      webview_terminate(context->w);
      return;
    }
    assert(!"Should not reach here");
  };
  auto html = "<script>\n"
              "  window.test(0);\n"
              "</script>";
  auto w = webview_create(false, nullptr);
  context.w = w;
  // Attempting to remove non-existing binding is OK
  webview_unbind(w, "test");
  webview_bind(w, "test", test, &context);
  // Attempting to bind multiple times only binds once
  webview_bind(w, "test", test, &context);
  webview_set_html(w, html);
  webview_run(w);
}

// =================================================================
// TEST: test synchronous binding and unbinding.
// =================================================================

static void test_sync_bind() {
  unsigned int number = 0;
  webview::webview w(false, nullptr);
  auto test = [&](const std::string &req) -> std::string {
    auto increment = [&](const std::string & /*req*/) -> std::string {
      ++number;
      return "";
    };
    // Bind and increment number.
    if (req == "[0]") {
      assert(number == 0);
      w.bind("increment", increment);
      return "(() => {try{window.increment()}catch{}window.test(1)})()";
    }
    // Unbind and make sure that we cannot increment even if we try.
    if (req == "[1]") {
      assert(number == 1);
      w.unbind("increment");
      return "(() => {try{window.increment()}catch{}window.test(2)})()";
    }
    // Number should not have changed but we can bind again and change the number.
    if (req == "[2]") {
      assert(number == 1);
      w.bind("increment", increment);
      return "(() => {try{window.increment()}catch{}window.test(3)})()";
    }
    // Finish test.
    if (req == "[3]") {
      assert(number == 2);
      w.terminate();
      return "";
    }
    assert(!"Should not reach here");
    return "";
  };
  auto html = "<script>\n"
              "  window.test(0);\n"
              "</script>";
  // Attempting to remove non-existing binding is OK
  w.unbind("test");
  w.bind("test", test);
  // Attempting to bind multiple times only binds once
  w.bind("test", test);
  w.set_html(html);
  w.run();
}

// =================================================================
// TEST: webview_version().
// =================================================================
static void test_c_api_version() {
  auto vi = webview_version();
  assert(vi);
  assert(vi->version.major == 1);
  assert(vi->version.minor == 2);
  assert(vi->version.patch == 3);
  assert(std::string(vi->version_number) == "1.2.3");
  assert(std::string(vi->pre_release) == "-test");
  assert(std::string(vi->build_metadata) == "+gaabbccd");
  // The function should return the same pointer when called again.
  assert(webview_version() == vi);
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
  assert(J(R"({"foo":""})", "foo", -1).empty());
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
  assert(J(R"([])", "", 0).empty());
  assert(J(R"({})", "foo", -1).empty());
  assert(J(R"(["foo", "bar", "baz"])", "", -1).empty());
  assert(J(R"(["foo"])", "", 1234).empty());
  assert(J(R"(["foo"])", "", -1234).empty());
  // Invalid input - should fail
  assert(J("", "", 0).empty());
  assert(J("", "foo", -1).empty());
  assert(J(R"({"foo":")", "foo", -1).empty());
  assert(J(R"({"foo":{)", "foo", -1).empty());
  assert(J(R"({"foo":{")", "foo", -1).empty());
  assert(J(R"(}")", "foo", -1).empty());
  assert(J(R"({}}")", "foo", -1).empty());
  assert(J(R"("foo)", "foo", -1).empty());
  assert(J(R"(foo)", "foo", -1).empty());
  assert(J(R"({{[[""foo""]]}})", "", 1234).empty());
  assert(J("bad", "", 0).empty());
  assert(J("bad", "foo", -1).empty());
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

#if _WIN32
// =================================================================
// TEST: ensure that narrow/wide string conversion works on Windows.
// =================================================================
static void test_win32_narrow_wide_string_conversion() {
  using namespace webview::detail;
  assert(widen_string("").empty());
  assert(narrow_string(L"").empty());
  assert(widen_string("foo") == L"foo");
  assert(narrow_string(L"foo") == "foo");
  assert(widen_string("フー") == L"フー");
  assert(narrow_string(L"フー") == "フー");
  assert(widen_string("æøå") == L"æøå");
  assert(narrow_string(L"æøå") == "æøå");
  // Unicode number for the smiley face below: U+1F600
  assert(widen_string("😀") == L"😀");
  assert(narrow_string(L"😀") == "😀");
  // Ensure that elements of wide string are correct
  {
    auto s = widen_string("😀");
    assert(s.size() == 2);
    assert(static_cast<std::uint16_t>(s[0]) == 0xD83D);
    assert(static_cast<std::uint16_t>(s[1]) == 0xDE00);
  }
  // Ensure that elements of narrow string are correct
  {
    auto s = narrow_string(L"😀");
    assert(s.size() == 4);
    assert(static_cast<std::uint8_t>(s[0]) == 0xf0);
    assert(static_cast<std::uint8_t>(s[1]) == 0x9f);
    assert(static_cast<std::uint8_t>(s[2]) == 0x98);
    assert(static_cast<std::uint8_t>(s[3]) == 0x80);
  }
  // Null-characters must also be converted
  assert(widen_string(std::string(2, '\0')) == std::wstring(2, L'\0'));
  assert(narrow_string(std::wstring(2, L'\0')) == std::string(2, '\0'));
}
#endif

int main(int argc, char *argv[]) {
  std::unordered_map<std::string, std::function<void()>> all_tests = {
      {"terminate", test_terminate},     {"c_api", test_c_api},
      {"c_api_bind", test_c_api_bind},   {"c_api_version", test_c_api_version},
      {"bidir_comms", test_bidir_comms}, {"json", test_json},
      {"sync_bind", test_sync_bind}};
#if _WIN32
  all_tests.emplace("win32_narrow_wide_string_conversion",
                    test_win32_narrow_wide_string_conversion);
#endif
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
