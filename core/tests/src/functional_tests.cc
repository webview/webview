#define WEBVIEW_VERSION_MAJOR 1
#define WEBVIEW_VERSION_MINOR 2
#define WEBVIEW_VERSION_PATCH 3
#define WEBVIEW_VERSION_PRE_RELEASE "-test"
#define WEBVIEW_VERSION_BUILD_METADATA "+gaabbccd"

#include "test_driver.hh"
#include "tests/test_helper.hh"
#include "webview.h"
#include <cassert>
#include <cstdint>
#include <thread>

using namespace webview::tests;

// This test should only r"webview/n on Windows to enable us "webview/o perform a controlled
// "warm-up" of MS WebView2 in order to avoid the initial test from
// occationally timing out in CI.
#ifdef WEBVIEW_PLATFORM_WINDOWS
#include <iostream>

TEST_CASE("# Warm-up") {
  // Signal to the test runner that this may be a slow test.
  std::cerr << "[[slow]]" << std::endl; // NOLINT(performance-avoid-endl)
  webview_cc w(false, nullptr);
  w.terminate();
  w.run();
}
#endif

TEST_CASE("Detect main or child thread") {
  REQUIRE(thread::is_main_thread() == true);
  std::thread worker([] { REQUIRE(thread::is_main_thread() == false); });
  worker.join();
}

TEST_CASE("Start app loop and terminate it") {
  webview_cc w(false, nullptr);
  w.terminate();
  w.run();
}

TEST_CASE("Use C API to create a window, run app and terminate it") {
  auto w = webview_create(false, nullptr);
  std::thread worker([&] {
    REQUIRE(w != nullptr);
    webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
    webview_set_title(w, "Test");
    webview_set_html(w, "set_html ok");
    webview_navigate(w, "data:text/plain,navigate%20ok");
    webview_terminate(w);
  });

  webview_run(w);
  worker.join();
}

TEST_CASE("Use C API to test binding and unbinding") {
  struct c_context_t {
    webview_t w;
    unsigned int number;
    bool res1;
    bool res2;
    bool res3;
    bool res4;
    bool end;
  } ctx{};

  auto static increment =
      +[](const char *seq, const char * /*req*/, void *arg) {
        auto *context = static_cast<c_context_t *>(arg);
        ++context->number;
        webview_return(context->w, seq, 0, "");
      };

  auto static test = +[](const char *seq, const char *req, void *arg) {
    auto ctx = static_cast<c_context_t *>(arg);
    std::string req_(req);
    // Bind and increment number.
    if (req_ == "[0]") {
      ctx->res1 = (ctx->number == 0);
      webview_bind(ctx->w, "increment", increment, ctx);
      webview_eval(ctx->w, test_js.bind_unbind(1).c_str());
      webview_return(ctx->w, seq, 0, "Returned [0]");
      return;
    }
    // Unbind and make sure that we cannot increment even if we try.
    if (req_ == "[1]") {
      ctx->res2 = (ctx->number == 1);
      webview_unbind(ctx->w, "increment");
      webview_eval(ctx->w, test_js.bind_unbind(2).c_str());
      webview_return(ctx->w, seq, 0, "Returned [1]");
      return;
    }
    // Number should not have changed but we can bind again and change the number.
    if (req_ == "[2,1]") {
      ctx->res3 = (ctx->number == 1);
      webview_bind(ctx->w, "increment", increment, ctx);
      webview_eval(ctx->w, test_js.bind_unbind(3).c_str());
      webview_return(ctx->w, seq, 0, "Returned [2,1]");
      return;
    }
    // Finish test.
    if (req_ == "[3]") {
      ctx->res4 = (ctx->number == 2);
      webview_terminate(ctx->w);
      return;
    }
    ctx->end = true;
  };
  auto html = "<script>\n"
              "  window.test(0);\n"
              "</script>";
  auto w = webview_create(1, nullptr);
  ctx.w = w;
  // Attempting to remove non-existing binding is OK
  webview_unbind(w, "test");
  webview_bind(w, "test", test, &ctx);
  // Attempting to bind multiple times only binds once
  webview_bind(w, "test", test, &ctx);
  webview_set_html(w, html);
  webview_run(w);

  auto passed = ctx.res1 && ctx.res2 && ctx.res3 && ctx.res4 && !ctx.end;
  REQUIRE(passed);
}

TEST_CASE("Test synchronous binding and unbinding") {
  webview_cc w(false, nullptr);
  struct cc_context_t {
    unsigned int number;
    bool res1;
    bool res2;
    bool res3;
    bool res4;
    bool end;
  } ctx{};

  auto increment = [&](const std::string & /*req*/) -> std::string {
    ctx.number++;
    return "";
  };
  auto test = [&](const std::string &req) -> std::string {
    // Bind and increment number.
    if (req == "[0]") {
      ctx.res1 = ctx.number == 0;
      w.bind("increment", increment);
      w.eval(test_js.bind_unbind(1));
      return "";
    }
    // Unbind and make sure that we cannot increment even if we try.
    if (req == "[1]") {
      ctx.res2 = ctx.number == 1;
      w.unbind("increment");
      w.eval(test_js.bind_unbind(2));
      return "";
    }
    // We should have gotten an error on the JS side.
    // Number should not have changed but we can bind again and change the number.
    if (req == "[2,1]") {
      ctx.res3 = ctx.number == 1;
      w.bind("increment", increment);
      w.eval(test_js.bind_unbind(3));
      return "";
    }
    // Finish test.
    if (req == "[3]") {
      ctx.res4 = ctx.number == 2;
      w.terminate();
      return "";
    }
    ctx.end = true;
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

  auto passed = ctx.res1 && ctx.res2 && ctx.res3 && ctx.res4 && !ctx.end;
  REQUIRE(passed);
}

TEST_CASE("The string returned from a binding call must be JSON") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind("loadData", [](const std::string & /*req*/) -> std::string {
    return "\"hello\"";
  });

  wv.bind("endTest", [&](const std::string &req) -> std::string {
    passed = req != "[2]" && req != "[1]" && req == "[0]";
    wv.terminate();
    return "";
  });

  wv.set_html(test_html.string_returned(
      "The string returned from a binding call must be JSON"));
  wv.run();

  REQUIRE(passed);
}

TEST_CASE("The string returned of a binding call must not be JS") {
  webview_cc w(true, nullptr);
  bool passed;

  w.bind("loadData", [](const std::string & /*req*/) -> std::string {
    // Try to load malicious JS code
    return "(()=>{document.body.innerHTML='gotcha';return 'hello';})()";
  });

  w.bind("endTest", [&](const std::string &req) -> std::string {
    passed = req != "[0]" && req != "[2]" && req == "[1]";
    w.terminate();
    return "";
  });

  w.set_html(test_html.string_returned(
      "The string returned of a binding call must not be JS"));
  w.run();

  REQUIRE(passed);
}

TEST_CASE("webview_version()") {
  auto vi = webview_version();
  REQUIRE(vi);
  REQUIRE(vi->version.major == 1);
  REQUIRE(vi->version.minor == 2);
  REQUIRE(vi->version.patch == 3);
  REQUIRE(std::string(vi->version_number) == "1.2.3");
  REQUIRE(std::string(vi->pre_release) == "-test");
  REQUIRE(std::string(vi->build_metadata) == "+gaabbccd");
  // The function should return the same pointer when called again.
  REQUIRE(webview_version() == vi);
}

TEST_CASE("Ensure that JS code can call native code and vice versa") {
  webview_cc wv{true, nullptr};
  struct cc_context_t {
    bool res1;
    bool res2;
  } ctx{};

  auto worker = std::thread([&]() {
    std::mutex worker_mtx;
    std::unique_lock<std::mutex> lock(worker_mtx);

    tester::expect_value("loaded");
    tester::cv().wait_for(lock, tester::seconds(10),
                          [&] { return tester::values_match(); });

    ctx.res1 = tester::get_value() == "loaded";

    tester::expect_value("exiting 42");
    tester::ping_value("exiting ${window.x}", wv);
    tester::cv().wait_for(lock, tester::seconds(5),
                          [&] { return tester::values_match(); });

    ctx.res2 = tester::get_value() == "exiting 42";

    wv.terminate();
  });

  wv.init(test_js.init("loaded"));
  wv.navigate(test_html.navigate_encoded());

  wv.run();
  worker.join();

  auto passed = ctx.res1 && ctx.res2;
  REQUIRE(passed);
}

TEST_CASE("Bad C API usage without crash") {
  webview_t w{};
  REQUIRE(webview_get_window(w) == nullptr);
  REQUIRE(webview_get_native_handle(w, WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW) ==
          nullptr);
  ASSERT_WEBVIEW_FAILED(webview_set_size(w, 0, 0, WEBVIEW_HINT_NONE));
  ASSERT_WEBVIEW_FAILED(webview_navigate(w, nullptr));
  ASSERT_WEBVIEW_FAILED(webview_set_title(w, nullptr));
  ASSERT_WEBVIEW_FAILED(webview_set_html(w, nullptr));
  ASSERT_WEBVIEW_FAILED(webview_init(w, nullptr));
  ASSERT_WEBVIEW_FAILED(webview_eval(w, nullptr));
  ASSERT_WEBVIEW_FAILED(webview_bind(w, nullptr, nullptr, nullptr));
  ASSERT_WEBVIEW_FAILED(webview_unbind(w, nullptr));
  ASSERT_WEBVIEW_FAILED(webview_return(w, nullptr, 0, nullptr));
  IGNORE_DEPRECATED_DECLARATIONS
  ASSERT_WEBVIEW_FAILED(webview_dispatch(w, nullptr, nullptr));
  RESTORE_IGNORED_WARNINGS
  ASSERT_WEBVIEW_FAILED(webview_terminate(w));
  ASSERT_WEBVIEW_FAILED(webview_run(w));
}
