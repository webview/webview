#define WEBVIEW_VERSION_MAJOR 1
#define WEBVIEW_VERSION_MINOR 2
#define WEBVIEW_VERSION_PATCH 3
#define WEBVIEW_VERSION_PRE_RELEASE "-test"
#define WEBVIEW_VERSION_BUILD_METADATA "+gaabbccd"

#include "log/trace_log.hh"
#include "strings/string_api.hh"
#include "test_driver.hh"
#include "tests/test_helper.hh"
#include "webview.h"
#include <cassert>
#include <cstdint>
#include <thread>

using namespace webview::strings;
using namespace webview::tests;
using namespace webview::log;

// This test should only r"webview/n on Windows to enable us "webview/o perform a controlled
// "warm-up" of MS WebView2 in order to avoid the initial test from
// occationally timing out in CI.
#ifdef WEBVIEW_PLATFORM_WINDOWS
#include <iostream>

TEST_CASE("# Warm-up") {
  // Signal to the test runner that this may be a slow test.
  std::cerr << "[[slow]]" << std::endl; // NOLINT(performance-avoid-endl)
  webview_cc wv(false, nullptr);
  std::thread worker([&] { wv.terminate(); });
  wv.run();
  worker.join();
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

TEST_CASE("Test nested C binding and unbinding") {
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
        auto *ctx = static_cast<c_context_t *>(arg);
        ++ctx->number;
        std::string message = "Incremented " + std::to_string(ctx->number);
        webview_return(ctx->w, seq, 0, message.c_str());
      };

  auto static tests = +[](const char *seq, const char *req, void *arg) {
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
      webview_return(ctx->w, seq, 0, "Returned [3]");
      webview_terminate(ctx->w);
      return;
    }
    ctx->end = true;
  };

  auto w = webview_create(1, nullptr);
  ctx.w = w;
  webview_set_html(w, "Test nested C binding and unbinding");
  // Attempting to remove non-existing binding is OK
  webview_unbind(w, "test");
  webview_bind(w, "test", tests, &ctx);
  // Attempting to bind multiple times only binds once
  webview_bind(w, "test", tests, &ctx);
  webview_eval(w, test_js.bind_unbind_init().c_str());
  webview_run(w);

  auto passed = ctx.res1 && ctx.res2 && ctx.res3 && ctx.res4 && !ctx.end;
  if (!passed) {
    trace::test.print_here(tester::res_string("res1", ctx.res1));
    trace::test.print_here(tester::res_string("res2", ctx.res2));
    trace::test.print_here(tester::res_string("res3", ctx.res3));
    trace::test.print_here(tester::res_string("res4", ctx.res4));
    trace::test.print_here(tester::res_string("end", ctx.end));
  }

  REQUIRE(passed);
}

TEST_CASE("Test nested CC binding and unbinding") {
  webview_cc wv{true, nullptr};

  struct cc_context_t {
    unsigned int number;
    bool res1;
    bool res2;
    bool res3;
    bool res4;
    bool end;
  } ctx{};

  auto increment = [&](const std::string &id, const std::string & /**/,
                       void *arg) {
    auto *ctx = static_cast<cc_context_t *>(arg);
    ctx->number++;
    auto message = "Incremented " + std::to_string(ctx->number);
    wv.resolve(id, 0, message);
  };

  auto tests = [&](const std::string & /**/, const std::string &req,
                   void *arg) {
    auto *ctx = static_cast<cc_context_t *>(arg);

    // Bind and increment number.
    if (req == "[0]") {
      ctx->res1 = ctx->number == 0;
      wv.bind("increment", increment, ctx);
      wv.eval(test_js.bind_unbind(1));
      return;
    }

    // Unbind and make sure that we cannot increment even if we try.
    if (req == "[1]") {
      ctx->res2 = ctx->number == 1;
      wv.unbind("increment");
      wv.eval(test_js.bind_unbind(2));
      return;
    }
    // We should have gotten an error on the JS side.
    // Number should not have changed but we can bind again and change the number.
    if (req == "[2,1]") {
      ctx->res3 = ctx->number == 1;
      wv.bind("increment", increment, ctx);
      wv.eval(test_js.bind_unbind(3));
      return;
    }
    // Finish test.
    if (req == "[3]") {
      ctx->res4 = ctx->number == 2;
      wv.unbind("test");
      wv.unbind("increment");
      wv.terminate();
      return;
    }
    ctx->end = true;
  };

  wv.set_html("Test nested CC binding and unbinding");
  // Attempting to remove non-existing binding is OK
  wv.unbind("test");
  wv.bind("test", tests, &ctx);
  // Attempting to bind multiple times only binds once
  wv.bind("test", tests, &ctx);
  wv.init(test_js.bind_unbind_init());
  wv.run();

  auto passed = ctx.res1 && ctx.res2 && ctx.res3 && ctx.res4 && !ctx.end;
  if (!passed) {
    trace::test.print_here(tester::res_string("res1", ctx.res1));
    trace::test.print_here(tester::res_string("res2", ctx.res2));
    trace::test.print_here(tester::res_string("res3", ctx.res3));
    trace::test.print_here(tester::res_string("res4", ctx.res4));
    trace::test.print_here(tester::res_string("end", ctx.end));
  }
  REQUIRE(passed);
}

TEST_CASE("The string returned from a binding call can be JSON") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string &id, const std::string & /*req*/, void * /*arg*/) {
        wv.resolve(id, 0, R"({"val": "hello"})");
      },
      nullptr);
  wv.bind(
      "endTest",
      [&](const std::string & /*id*/, const std::string &req, void * /**/) {
        auto returned = json::parse(req, "", 0);
        auto res = json::parse(returned, "val", 0);
        passed = req != "[2]" && req != "[1]" && res == "hello";
        wv.terminate();
      },
      nullptr);
  wv.set_html(test_html.string_returned(
      "The string returned from a binding call can be JSON"));
  wv.run();
  REQUIRE(passed);
}

TEST_CASE("The string returned from a binding call can be a number") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string &id, const std::string & /**/, void * /**/) {
        wv.resolve(id, 0, "1.234");
      },
      nullptr);
  wv.bind(
      "endTest",
      [&](const std::string & /**/, const std::string &req, void * /**/) {
        passed = req != "[2]" && req != "[1]" && req == "[1.234]";
        wv.terminate();
      },
      nullptr);
  wv.set_html(test_html.string_returned(
      "The string returned from a binding call can be a number"));
  wv.run();
  REQUIRE(passed);
}

TEST_CASE("The string returned from a binding call can be a bool") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string &id, const std::string & /**/, void * /**/) {
        wv.resolve(id, 0, "false");
      },
      nullptr);
  wv.bind(
      "endTest",
      [&](const std::string & /**/, const std::string &req, void * /**/) {
        passed = req != "[2]" && req != "[1]" && req == "[false]";
        wv.terminate();
      },
      nullptr);
  wv.set_html(test_html.string_returned(
      "The string returned from a binding call can be a bool"));
  wv.run();
  REQUIRE(passed);
}

TEST_CASE("The string returned from a binding call can be a string") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string &id, const std::string & /**/, void * /**/) {
        wv.resolve(id, 0, "this is a string");
      },
      nullptr);
  wv.bind(
      "endTest",
      [&](const std::string & /**/, const std::string &req, void * /**/) {
        passed =
            req != "[2]" && req != "[1]" && req == R"(["this is a string"])";
        wv.terminate();
      },
      nullptr);
  wv.set_html(test_html.string_returned(
      "The string returned from a binding call can be a string"));
  wv.run();
  REQUIRE(passed);
}

TEST_CASE("The string returned of a binding call must not be JS") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string &id, const std::string & /*req*/, void * /**/) {
        // Try to load malicious JS code
        wv.resolve(
            id, 0,
            "(()=>{document.body.innerHTML='gotcha';return 'hello';})()");
      },
      nullptr);
  wv.bind(
      "endTest",
      [&](const std::string & /*req*/, const std::string &req, void * /**/) {
        passed = req != "[2]" && req == "[1]";
        wv.terminate();
      },
      nullptr);
  wv.set_html(test_html.string_returned(
      "The string returned of a binding call must not be JS"));
  wv.run();
  REQUIRE(passed);
}

TEST_CASE("The string returned of a binding call must not be encoded JS") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string &id, const std::string & /*req*/, void * /**/) {
        // Try to load malicious JS code
        wv.resolve(id, 0,
                   "(()%3D%3E%7Bdocument.body.innerHTML%3D'gotcha'%3Breturn%20'"
                   "hello'%3B%7D)()");
      },
      nullptr);
  wv.bind(
      "endTest",
      [&](const std::string & /*req*/, const std::string &req, void * /**/) {
        passed = req != "[2]" && req == "[1]";
        wv.terminate();
      },
      nullptr);
  wv.set_html(test_html.string_returned(
      "The string returned of a binding call must not be JS"));
  wv.run();
  REQUIRE(passed);
}

TEST_CASE("The string returned of a binding call must not be a HTML script") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string &id, const std::string & /*req*/, void * /**/) {
        // Try to load malicious JS code
        wv.resolve(id, 0,
                   R"(
<script>(()=>{document.body.innerHTML='gotcha';return 'hello';})()</script>
)");
      },
      nullptr);
  wv.bind(
      "endTest",
      [&](const std::string & /*req*/, const std::string &req, void * /**/) {
        passed = req != "[2]" && req == "[1]";
        wv.terminate();
      },
      nullptr);
  wv.set_html(test_html.string_returned(
      "The string returned of a binding call must not be a HTML script"));
  wv.run();
  REQUIRE(passed);
}

TEST_CASE("The string returned of a binding call must not be an encoded HTML "
          "script") {
  webview_cc wv(true, nullptr);
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string &id, const std::string & /*req*/, void * /**/) {
        // Try to load malicious JS code
        wv.resolve(id, 0,
                   R"(
%3Cscript%3E(()%3D%3E%7Bdocument.body.innerHTML%3D'gotcha'%3Breturn%20'hello'%3B%7D)()%3C%2Fscript%3E
)");
      },
      nullptr);
  wv.bind(
      "endTest",
      [&](const std::string & /*req*/, const std::string &req, void * /**/) {
        passed = req != "[2]" && req == "[1]";
        wv.terminate();
      },
      nullptr);
  wv.set_html(test_html.string_returned(
      "The string returned of a binding call must not be a HTML script"));
  wv.run();
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
  if (!passed) {
    trace::test.print_here(tester::res_string("res1", ctx.res1));
    trace::test.print_here(tester::res_string("res2", ctx.res2));
  }
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
TEST_CASE("Bind and init before run must evalute") {
  webview_cc wv{true, nullptr};
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string & /*id*/, const std::string &req, void * /*arg*/) {
        passed = req == "[1]";
        wv.set_html("Bind and init before run must evalute");
        wv.terminate();
      },
      nullptr);
  wv.init("loadData(1)");
  wv.run();

  REQUIRE(passed);
}

TEST_CASE("Bind and set_html before run must evaluate") {
  webview_cc wv{true, nullptr};
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string & /*id*/, const std::string &req, void * /**/) {
        passed = req == "[1]";
        wv.terminate();
      },
      nullptr);
  wv.set_html(R"(
Bind and set_html before run must evaluate
<script>loadData(1)</script>
)");
  wv.run();

  REQUIRE(passed);
}

TEST_CASE("Bind eval unbind before run must evaluate") {
  webview_cc wv{true, nullptr};
  bool passed;

  wv.bind(
      "loadData",
      [&](const std::string & /*id*/, const std::string &req, void * /**/) {
        passed = req == "[1]";
        wv.terminate();
      },
      nullptr);
  wv.eval("loadData(1)");
  wv.unbind("loadData");
  wv.run();

  REQUIRE(passed);
}

TEST_CASE("Bind eval unbind and re-bind must evaluate from a child thread") {
  webview_cc wv{true, nullptr};
  struct test_ctx_t {
    bool res1;
    bool res2;
  } ctx{};
  std::thread worker{[&]() {
    wv.bind(
        "loadData",
        [&](const std::string & /*id*/, const std::string &req, void * /**/) {
          ctx.res1 = req == "[1]";
        },
        nullptr);
    wv.eval("loadData(1)");
    wv.unbind("loadData");
    wv.bind(
        "loadData",
        [&](const std::string & /*id*/, const std::string &req, void * /**/) {
          ctx.res2 = req == "[2]";
          wv.terminate();
        },
        nullptr);
    wv.eval("loadData(2)");
  }};

  wv.run();
  worker.join();

  auto passed = ctx.res1 && ctx.res2;
  if (!passed) {
    trace::test.print_here(tester::res_string("res1", ctx.res1));
    trace::test.print_here(tester::res_string("res2", ctx.res2));
  }
  REQUIRE(passed);
}