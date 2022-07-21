//bin/echo; [ $(uname) = "Darwin" ] && FLAGS="-framework Webkit" || FLAGS="$(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)" ; c++ "$0" $FLAGS -std=c++11 -Wall -Wextra -pedantic -g -o webview_test && ./webview_test ; exit
// +build ignore

#include "webview.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <thread>
#include <unordered_map>

static webview_create_options_t get_create_options() {
  return webview::create_options_builder{}
      .minimum_required_version(WEBVIEW_VERSION)
      .build();
}

// =================================================================
// TEST: start app loop and terminate it.
// =================================================================
static void test_terminate() {
  auto options = get_create_options();
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
  webview_error_t err = webview_set_size(w, 480, 320, 0);
  assert(err == WEBVIEW_ERROR_OK);
  err = webview_set_title(w, "Test");
  assert(err == WEBVIEW_ERROR_OK);
  err = webview_navigate(w, "https://github.com/zserge/webview");
  assert(err == WEBVIEW_ERROR_OK);
  err = webview_dispatch(w, cb_assert_arg, (void *)"arg");
  assert(err == WEBVIEW_ERROR_OK);
  err = webview_dispatch(w, cb_terminate, nullptr);
  assert(err == WEBVIEW_ERROR_OK);
  err = webview_run(w);
  assert(err == WEBVIEW_ERROR_OK);
  err = webview_destroy(w);
  assert(err == WEBVIEW_ERROR_OK);
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

  webview_t w = webview_create(WEBVIEW_FALSE, nullptr); // NOLINT

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

  continue_c_api_test(w);
}

// =================================================================
// TEST: webview_create_with_options().
// =================================================================
static void test_c_api_create_with_options() {
  auto options = get_create_options();
  webview_t w = nullptr;
  assert(webview_create_with_options(&w, &options) == WEBVIEW_ERROR_OK);
  continue_c_api_test(w);
}

// =================================================================
// TEST: ensure that C API can return error codes.
// =================================================================
static void test_c_api_error_codes() {
  auto err = webview_create_with_options(nullptr, nullptr);
  assert(err == WEBVIEW_ERROR_INVALID_ARGUMENT);
}

// =================================================================
// TEST: webview_library_version().
// =================================================================
static void test_c_api_library_version() {
  assert(webview_version() == WEBVIEW_VERSION);
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
    auto options = webview::create_options_builder{}
                       .minimum_required_version(WEBVIEW_VERSION)
                       .debug()
                       .build();
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
// TEST: unbind errors.
// =================================================================
static void test_unbind_errors() {
  auto options = get_create_options();
  webview::webview w(options);
  auto fn = [](const std::string &) -> std::string { return ""; };
  // Unbinding non-existing binding should throw.
  try {
    w.unbind("hello");
    assert(false);
  } catch (const webview::webview_exception &e) {
    assert(e.code() == WEBVIEW_ERROR_NOT_FOUND);
  }
  // Should be able to bind.
  w.bind("hello", fn);
  // Should be able to unbind.
  w.unbind("hello");
}

// =================================================================
// TEST: async bind errors.
// =================================================================
static void test_async_bind_error() {
  auto options = get_create_options();
  webview::webview w(options);
  auto fn = [](const std::string &) -> std::string { return ""; };
  // Should be able to bind.
  w.bind("hello", fn);
  // Binding again with the same name should throw.
  try {
    w.bind("hello", fn);
    assert(false);
  } catch (const webview::webview_exception &e) {
    assert(e.code() == WEBVIEW_ERROR_DUPLICATE);
  }
}

// =================================================================
// TEST: sync bind errors.
// =================================================================
static void test_sync_bind_error() {
  auto options = get_create_options();
  webview::webview w(options);
  auto fn = [](std::string, std::string, void *) {};
  // Should be able to bind.
  w.bind("hello", fn, nullptr);
  // Binding again with the same name should throw.
  try {
    w.bind("hello", fn, nullptr);
    assert(false);
  } catch (const webview::webview_exception &e) {
    assert(e.code() == WEBVIEW_ERROR_DUPLICATE);
  }
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

// =================================================================
// TEST: validate_create_options().
// =================================================================
static void test_validate_create_options() {
  using namespace webview::detail;
  {
    auto options = webview::create_options_builder{}.build();
    validate_create_options(options);
  }
  {
    auto options = webview::create_options_builder{}
                       .minimum_required_version(min_supported_version - 1)
                       .build();
    try {
      validate_create_options(options);
      assert(false);
    } catch (const webview::webview_exception &e) {
      assert(e.code() == WEBVIEW_ERROR_VERSION_TOO_OLD);
    }
  }
  {
    auto options = webview::create_options_builder{}
                       .minimum_required_version(WEBVIEW_VERSION + 1)
                       .build();
    try {
      validate_create_options(options);
      assert(false);
    } catch (const webview::webview_exception &e) {
      assert(e.code() == WEBVIEW_ERROR_VERSION_TOO_RECENT);
    }
  }
}

// =================================================================
// TEST: apply_webview_create_options_compatibility().
// =================================================================
static void test_apply_webview_create_options_compatibility() {
  using namespace webview::detail;
  auto options = webview::create_options_builder{}.build();
  assert(options.visible == WEBVIEW_FALSE);
  options = apply_webview_create_options_compatibility(options);
  assert(options.visible == WEBVIEW_TRUE);
}

// =================================================================
// TEST: ensure that version packing and unpacking works.
// =================================================================
static void test_packed_version() {
  auto max_uint32 = 4294967295U;
  {
    // Packing numbers exceeding 10 bits should truncate.
    // Unused bits should not be set.
    auto version = WEBVIEW_PACK_VERSION(max_uint32, max_uint32, max_uint32);
    assert(version == 1073741823);
  }
  {
    // Unpacking numbers exceeding 10 bits should truncate.
    // All bits except unused bits should be set.
    assert(WEBVIEW_UNPACK_MAJOR_VERSION(max_uint32) == 1023);
    assert(WEBVIEW_UNPACK_MINOR_VERSION(max_uint32) == 1023);
    assert(WEBVIEW_UNPACK_PATCH_VERSION(max_uint32) == 1023);
  }
  {
    auto version = WEBVIEW_PACK_VERSION(1, 1, 1);
    // The first bit of each version component should be set.
    assert(version == 1049601);
    assert(WEBVIEW_UNPACK_MAJOR_VERSION(version) == 1);
    assert(WEBVIEW_UNPACK_MINOR_VERSION(version) == 1);
    assert(WEBVIEW_UNPACK_PATCH_VERSION(version) == 1);
  }
  {
    auto version = WEBVIEW_PACK_VERSION(0, 0, 0);
    assert(version == 0);
    assert(WEBVIEW_UNPACK_MAJOR_VERSION(version) == 0);
    assert(WEBVIEW_UNPACK_MINOR_VERSION(version) == 0);
    assert(WEBVIEW_UNPACK_PATCH_VERSION(version) == 0);
  }
  {
    auto version = WEBVIEW_PACK_VERSION(3, 7, 15);
    assert(version == 3152911);
    assert(WEBVIEW_UNPACK_MAJOR_VERSION(version) == 3);
    assert(WEBVIEW_UNPACK_MINOR_VERSION(version) == 7);
    assert(WEBVIEW_UNPACK_PATCH_VERSION(version) == 15);
  }
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
      {"apply_webview_create_options_compatibility",
       test_apply_webview_create_options_compatibility},
      {"terminate", test_terminate},
      {"c_api_create", test_c_api_create},
      {"c_api_create_with_options", test_c_api_create_with_options},
      {"c_api_error_codes", test_c_api_error_codes},
      {"c_api_library_version", test_c_api_library_version},
      {"bidir_comms", test_bidir_comms},
      {"json", test_json},
      {"validate_create_options", test_validate_create_options},
      {"packed_version", test_packed_version},
      {"unbind_errors", test_unbind_errors},
      {"async_bind_error", test_async_bind_error},
      {"sync_bind_error", test_sync_bind_error}};
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
