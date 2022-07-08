//bin/echo; [ $(uname) = "Darwin" ] && FLAGS="-framework Webkit" || FLAGS="$(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)" ; c++ "$0" $FLAGS -std=c++11 -Wall -Wextra -pedantic -g -o webview_test && ./webview_test ; exit
// +build ignore

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
  webview_create_options_t options{};
  options.struct_size = sizeof(options);
  options.api_version = webview::api_version;
  webview::webview w(options);
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

static void continue_c_api_test(webview_t w) {
  assert(w);
  webview_set_size(w, 480, 320, 0);
  webview_set_title(w, "Test");
  webview_navigate(w, "https://github.com/zserge/webview");
  webview_dispatch(w, cb_assert_arg, (void *)"arg");
  webview_dispatch(w, cb_terminate, nullptr);
  webview_run(w);
  webview_destroy(w);
}

static void test_c_api_create() {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

  continue_c_api_test(webview_create(false, nullptr)); // NOLINT

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
}

static void test_c_api_create_with_options() {
  webview_create_options_t options{};
  options.struct_size = sizeof(options);
  options.api_version = webview::api_version;
  webview_t w = nullptr;
  assert(webview_create_with_options(&w, &options) == WEBVIEW_ERROR_OK);
  continue_c_api_test(w);
}

// =================================================================
// TEST: ensure that C API can return error codes.
// =================================================================
static void test_c_api_error_codes() {
  webview_error_t ec;

  ec = webview_create_with_options(nullptr, nullptr);
  assert(ec == WEBVIEW_ERROR_INVALID_ARGUMENT);

  auto min_version = webview::detail::min_api_version;
  webview_t w;
  webview_create_options_t options{};

  options.struct_size = sizeof(options);
  options.api_version = min_version;

  options.api_version.major = min_version.major - 1;
  ec = webview_create_with_options(&w, &options);
  assert(ec == WEBVIEW_ERROR_API_VERSION_TOO_OLD);

  options.api_version.major = WEBVIEW_API_MAJOR_VERSION + 1;
  ec = webview_create_with_options(&w, &options);
  assert(ec == WEBVIEW_ERROR_API_VERSION_TOO_RECENT);

  options.api_version = min_version;
  ec = webview_create_with_options(&w, &options);
  assert(ec == WEBVIEW_ERROR_OK);

  ec = webview_destroy(w);
  assert(ec == WEBVIEW_ERROR_OK);
}

// =================================================================
// TEST: webview_api_version().
// =================================================================
static void test_c_api_version() {
  using webview::api_version;
  using webview::detail::compare_versions;
  assert(compare_versions(webview_api_version(), api_version) == 0);
}

// =================================================================
// TEST: webview_library_version().
// =================================================================
static void test_c_api_library_version() {
  using webview::library_version;
  using webview::detail::compare_versions;
  assert(compare_versions(webview_library_version(), library_version) == 0);
}

// =================================================================
// TEST: compare_versions().
// =================================================================
static void test_compare_versions() {
  using webview::detail::compare_versions;
  assert(compare_versions({0, 0, 0}, {0, 0, 0}) == 0);
  assert(compare_versions({0, 0, 1}, {0, 0, 1}) == 0);
  assert(compare_versions({0, 1, 0}, {0, 1, 0}) == 0);
  assert(compare_versions({1, 0, 0}, {1, 0, 0}) == 0);
  assert(compare_versions({1, 1, 1}, {1, 1, 1}) == 0);
  assert(compare_versions({0, 0, 1}, {0, 0, 0}) > 0);
  assert(compare_versions({0, 1, 0}, {0, 0, 1}) > 0);
  assert(compare_versions({1, 0, 0}, {0, 1, 1}) > 0);
}

// =================================================================
// TEST: ensure that JS code can call native code and vice versa.
// =================================================================
struct test_webview : webview::browser_engine {
  using cb_t = std::function<void(test_webview *, int, const std::string &)>;
  test_webview(cb_t cb) : webview::browser_engine(create_options()), m_cb(cb) {}
  void on_message(const std::string &msg) override { m_cb(this, i++, msg); }
  int i = 0;
  cb_t m_cb;

private:
  static webview_create_options_t create_options() {
    webview_create_options_t options{};
    options.struct_size = sizeof(options);
    options.api_version = webview::api_version;
    options.debug = WEBVIEW_TRUE;
    return options;
  }
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
  try {
    fn();
  } catch (const webview::webview_exception &e) {
    std::cout << "A webview exception was thrown with error code " << e.code()
              << ": " << e.what() << std::endl;
    exit(1);
  } catch (const std::exception &e) {
    std::cout << "An exception was thrown: " << e.what() << std::endl;
    exit(1);
  } catch (...) {
    std::cout << "An unknown exception was thrown." << std::endl;
    exit(1);
  }
  flag_running.clear();
  timeout_thread.join();
}

#if _WIN32
// =================================================================
// TEST: ensure that narrow/wide string conversion works on Windows.
// =================================================================
static void test_win32_narrow_wide_string_conversion() {
  using namespace webview::detail;
  assert(widen_string("") == L"");
  assert(narrow_string(L"") == "");
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
      {"terminate", test_terminate},
      {"c_api_create", test_c_api_create},
      {"c_api_create_with_options", test_c_api_create_with_options},
      {"c_api_error_codes", test_c_api_error_codes},
      {"c_api_version", test_c_api_version},
      {"c_api_library_version", test_c_api_library_version},
      {"compare_versions", test_compare_versions},
      {"bidir_comms", test_bidir_comms},
      {"json", test_json}};
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
