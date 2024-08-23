#include "webview/test_driver.hh"

#define WEBVIEW_VERSION_MAJOR 1
#define WEBVIEW_VERSION_MINOR 2
#define WEBVIEW_VERSION_PATCH 3
#define WEBVIEW_VERSION_PRE_RELEASE "-test"
#define WEBVIEW_VERSION_BUILD_METADATA "+gaabbccd"

#include "webview/webview.h"

#include <cassert>
#include <cstdint>

TEST_CASE("Start app loop and terminate it") {
  webview::webview w(false, nullptr);
  w.dispatch([&]() { w.terminate(); });
  w.run();
}

static void cb_assert_arg(webview_t w, void *arg) {
  REQUIRE(w != nullptr);
  REQUIRE(memcmp(arg, "arg", 3) == 0);
}

static void cb_terminate(webview_t w, void *arg) {
  REQUIRE(arg == nullptr);
  webview_terminate(w);
}

TEST_CASE("Use C API to create a window, run app and terminate it") {
  webview_t w;
  w = webview_create(false, nullptr);
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
  webview_set_title(w, "Test");
  webview_set_html(w, "set_html ok");
  webview_navigate(w, "data:text/plain,navigate%20ok");
  webview_dispatch(w, cb_assert_arg, (void *)"arg");
  webview_dispatch(w, cb_terminate, nullptr);
  webview_run(w);
  webview_destroy(w);
}

TEST_CASE("Use C API to test binding and unbinding") {
  struct context_t {
    webview_t w;
    unsigned int number;
  } context{};
  auto test = +[](const char *seq, const char *req, void *arg) {
    auto increment = +[](const char *seq, const char * /*req*/, void *arg) {
      auto *context = static_cast<context_t *>(arg);
      ++context->number;
      webview_return(context->w, seq, 0, "");
    };
    auto context = static_cast<context_t *>(arg);
    std::string req_(req);
    // Bind and increment number.
    if (req_ == "[0]") {
      REQUIRE(context->number == 0);
      webview_bind(context->w, "increment", increment, context);
      webview_eval(context->w,
                   "try{window.increment().then(r => window.test(1))"
                   ".catch(() => window.test(1,1))}"
                   "catch{window.test(1,1)}");
      webview_return(context->w, seq, 0, "");
      return;
    }
    // Unbind and make sure that we cannot increment even if we try.
    if (req_ == "[1]") {
      REQUIRE(context->number == 1);
      webview_unbind(context->w, "increment");
      webview_eval(context->w,
                   "try{window.increment().then(r => window.test(2))"
                   ".catch(() => window.test(2,1))}"
                   "catch{window.test(2,1)}");
      webview_return(context->w, seq, 0, "");
      return;
    }
    // Number should not have changed but we can bind again and change the number.
    if (req_ == "[2,1]") {
      REQUIRE(context->number == 1);
      webview_bind(context->w, "increment", increment, context);
      webview_eval(context->w,
                   "try{window.increment().then(r => window.test(3))"
                   ".catch(() => window.test(3,1))}"
                   "catch{window.test(3,1)}");
      webview_return(context->w, seq, 0, "");
      return;
    }
    // Finish test.
    if (req_ == "[3]") {
      REQUIRE(context->number == 2);
      webview_terminate(context->w);
      return;
    }
    REQUIRE(!"Should not reach here");
  };
  auto html = "<script>\n"
              "  window.test(0);\n"
              "</script>";
  auto w = webview_create(1, nullptr);
  context.w = w;
  // Attempting to remove non-existing binding is OK
  webview_unbind(w, "test");
  webview_bind(w, "test", test, &context);
  // Attempting to bind multiple times only binds once
  webview_bind(w, "test", test, &context);
  webview_set_html(w, html);
  webview_run(w);
}

TEST_CASE("Test synchronous binding and unbinding") {
  auto make_call_js = [](unsigned int result) {
    std::string js;
    js += "try{window.increment().then(r => window.test(";
    js += std::to_string(result);
    js += "))";
    js += ".catch(() => window.test(";
    js += std::to_string(result);
    js += ",1))}catch{window.test(";
    js += std::to_string(result);
    js += ",1)}";
    return js;
  };
  unsigned int number = 0;
  webview::webview w(false, nullptr);
  auto test = [&](const std::string &req) -> std::string {
    auto increment = [&](const std::string & /*req*/) -> std::string {
      ++number;
      return "";
    };
    // Bind and increment number.
    if (req == "[0]") {
      REQUIRE(number == 0);
      w.bind("increment", increment);
      w.eval(make_call_js(1));
      return "";
    }
    // Unbind and make sure that we cannot increment even if we try.
    if (req == "[1]") {
      REQUIRE(number == 1);
      w.unbind("increment");
      w.eval(make_call_js(2));
      return "";
    }
    // We should have gotten an error on the JS side.
    // Number should not have changed but we can bind again and change the number.
    if (req == "[2,1]") {
      REQUIRE(number == 1);
      w.bind("increment", increment);
      w.eval(make_call_js(3));
      return "";
    }
    // Finish test.
    if (req == "[3]") {
      REQUIRE(number == 2);
      w.terminate();
      return "";
    }
    REQUIRE(!"Should not reach here");
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

TEST_CASE("The string returned from a binding call must be JSON") {
  constexpr auto html =
      R"html(<script>
  try {
    window.loadData()
      .then(() => window.endTest(0))
      .catch(() => window.endTest(1));
  } catch {
    window.endTest(2);
  }
</script>)html";

  webview::webview w(true, nullptr);

  w.bind("loadData", [](const std::string & /*req*/) -> std::string {
    return "\"hello\"";
  });

  w.bind("endTest", [&](const std::string &req) -> std::string {
    REQUIRE(req != "[2]");
    REQUIRE(req != "[1]");
    REQUIRE(req == "[0]");
    w.terminate();
    return "";
  });

  w.set_html(html);
  w.run();
}

TEST_CASE("The string returned of a binding call must not be JS") {
  constexpr const auto html =
      R"html(<script>
  try {
    window.loadData()
      .then(() => window.endTest(0))
      .catch(() => window.endTest(1));
  } catch {
    window.endTest(2);
  }
</script>)html";

  webview::webview w(true, nullptr);

  w.bind("loadData", [](const std::string & /*req*/) -> std::string {
    // Try to load malicious JS code
    return "(()=>{document.body.innerHTML='gotcha';return 'hello';})()";
  });

  w.bind("endTest", [&](const std::string &req) -> std::string {
    REQUIRE(req != "[0]");
    REQUIRE(req != "[2]");
    REQUIRE(req == "[1]");
    w.terminate();
    return "";
  });

  w.set_html(html);
  w.run();
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

struct test_webview : webview::browser_engine {
  using cb_t = std::function<void(test_webview *, int, const std::string &)>;
  test_webview(cb_t cb) : webview::browser_engine(true, nullptr), m_cb(cb) {}
  void on_message(const std::string &msg) override { m_cb(this, i++, msg); }
  int i = 0;
  cb_t m_cb;
};

TEST_CASE("Ensure that JS code can call native code and vice versa") {
  test_webview browser([](test_webview *w, int i, const std::string &msg) {
    switch (i) {
    case 0:
      REQUIRE(msg == "loaded");
      w->eval("window.__webview__.post('exiting ' + window.x)");
      break;
    case 1:
      REQUIRE(msg == "exiting 42");
      w->terminate();
      break;
    default:
      REQUIRE(0);
    }
  });
  browser.init(R"(
    window.x = 42;
    window.onload = () => {
      window.__webview__.post('loaded');
    };
  )");
  browser.navigate("data:text/html,%3Chtml%3Ehello%3C%2Fhtml%3E");
  browser.run();
}

TEST_CASE("Ensure that JSON parsing works") {
  auto J = webview::detail::json_parse;
  // Valid input with expected output
  REQUIRE(J(R"({"foo":"bar"})", "foo", -1) == "bar");
  REQUIRE(J(R"({"foo":""})", "foo", -1).empty());
  REQUIRE(J(R"({"foo":{}")", "foo", -1) == "{}");
  REQUIRE(J(R"({"foo": {"bar": 1}})", "foo", -1) == R"({"bar": 1})");
  REQUIRE(J(R"(["foo", "bar", "baz"])", "", 0) == "foo");
  REQUIRE(J(R"(["foo", "bar", "baz"])", "", 2) == "baz");
  // Valid UTF-8 with expected output
  REQUIRE(J(R"({"フー":"バー"})", "フー", -1) == "バー");
  REQUIRE(J(R"(["フー", "バー", "バズ"])", "", 2) == "バズ");
  // Invalid input with valid output - should probably fail
  REQUIRE(J(R"({"foo":"bar")", "foo", -1) == "bar");
  // Valid input with other invalid parameters - should fail
  REQUIRE(J(R"([])", "", 0).empty());
  REQUIRE(J(R"({})", "foo", -1).empty());
  REQUIRE(J(R"(["foo", "bar", "baz"])", "", -1).empty());
  REQUIRE(J(R"(["foo"])", "", 1234).empty());
  REQUIRE(J(R"(["foo"])", "", -1234).empty());
  // Invalid input - should fail
  REQUIRE(J("", "", 0).empty());
  REQUIRE(J("", "foo", -1).empty());
  REQUIRE(J(R"({"foo":")", "foo", -1).empty());
  REQUIRE(J(R"({"foo":{)", "foo", -1).empty());
  REQUIRE(J(R"({"foo":{")", "foo", -1).empty());
  REQUIRE(J(R"(}")", "foo", -1).empty());
  REQUIRE(J(R"({}}")", "foo", -1).empty());
  REQUIRE(J(R"("foo)", "foo", -1).empty());
  REQUIRE(J(R"(foo)", "foo", -1).empty());
  REQUIRE(J(R"({{[[""foo""]]}})", "", 1234).empty());
  REQUIRE(J("bad", "", 0).empty());
  REQUIRE(J("bad", "foo", -1).empty());
}

TEST_CASE("Ensure that JSON escaping works") {
  using webview::detail::json_escape;

  // Simple case without need for escaping. Quotes added by default.
  REQUIRE(json_escape("hello") == "\"hello\"");
  // Simple case without need for escaping. Quotes explicitly not added.
  REQUIRE(json_escape("hello", false) == "hello");
  // Empty input should return empty output.
  REQUIRE(json_escape("", false).empty());
  // '"' and '\' should be escaped.
  REQUIRE(json_escape("\"", false) == "\\\"");
  REQUIRE(json_escape("\\", false) == "\\\\");
  // Commonly-used characters that should be escaped.
  REQUIRE(json_escape("\b\f\n\r\t", false) == "\\b\\f\\n\\r\\t");
  // ASCII control characters should be escaped.
  REQUIRE(json_escape(std::string{"\0\x1f", 2}, false) == "\\u0000\\u001f");
  // ASCII printable characters (even DEL) shouldn't be escaped.
  REQUIRE(json_escape("\x20\x7e\x7f", false) == "\x20\x7e\x7f");
  // Valid UTF-8.
  REQUIRE(json_escape("\u2328", false) == "\u2328");
  REQUIRE(json_escape("フーバー", false) == "フーバー");
  // Replacement character for invalid characters.
  REQUIRE(json_escape("�", false) == "�");
  // Invalid characters should be replaced with '�' but we just leave them as-is.
  REQUIRE(json_escape("\x80\x9f\xa0\xff", false) == "\x80\x9f\xa0\xff");
  // JS code should not be executed (eval).
  auto expected_gotcha = R"js(alert(\"gotcha\"))js";
  REQUIRE(json_escape(R"(alert("gotcha"))", false) == expected_gotcha);
}

TEST_CASE("optional class") {
  using namespace webview::detail;

  REQUIRE(!optional<int>{}.has_value());
  REQUIRE(optional<int>{1}.has_value());
  REQUIRE(optional<int>{1}.get() == 1);

  try {
    optional<int>{}.get();
    REQUIRE(!!"Expected exception");
  } catch (const bad_access &) {
    // Do nothing
  }

  REQUIRE(!optional<int>{optional<int>{}}.has_value());
  REQUIRE(optional<int>{optional<int>{1}}.has_value());
  REQUIRE(optional<int>{optional<int>{1}}.get() == 1);
}

TEST_CASE("result class") {
  using namespace webview::detail;
  using namespace webview;

  REQUIRE(result<int>{}.has_value());
  REQUIRE(result<int>{}.value() == 0);
  REQUIRE(result<int>{1}.has_value());
  REQUIRE(result<int>{1}.value() == 1);
  REQUIRE(!result<int>{}.has_error());
  REQUIRE(!result<int>{1}.has_error());
  REQUIRE(result<int>{}.ok());
  REQUIRE(result<int>{1}.ok());
  REQUIRE(!result<int>{error_info{}}.ok());
  REQUIRE(!result<int>{error_info{}}.has_value());
  REQUIRE(result<int>{error_info{}}.has_error());

  auto result_with_error = result<int>{
      error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, "invalid argument"}};
  REQUIRE(result_with_error.error().code() == WEBVIEW_ERROR_INVALID_ARGUMENT);
  REQUIRE(result_with_error.error().message() == "invalid argument");

  try {
    result<int>{}.error();
    REQUIRE(!!"Expected exception");
  } catch (const bad_access &) {
    // Do nothing
  }
}

TEST_CASE("noresult class") {
  using namespace webview::detail;
  using namespace webview;

  REQUIRE(!noresult{}.has_error());
  REQUIRE(noresult{}.ok());
  REQUIRE(!noresult{error_info{}}.ok());
  REQUIRE(noresult{error_info{}}.has_error());

  auto result_with_error =
      noresult{error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, "invalid argument"}};
  REQUIRE(result_with_error.error().code() == WEBVIEW_ERROR_INVALID_ARGUMENT);
  REQUIRE(result_with_error.error().message() == "invalid argument");

  try {
    noresult{}.error();
    REQUIRE(!!"Expected exception");
  } catch (const bad_access &) {
    // Do nothing
  }
}

#define ASSERT_WEBVIEW_FAILED(expr) REQUIRE(WEBVIEW_FAILED(expr))

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
  ASSERT_WEBVIEW_FAILED(webview_dispatch(w, nullptr, nullptr));
  ASSERT_WEBVIEW_FAILED(webview_terminate(w));
  ASSERT_WEBVIEW_FAILED(webview_run(w));
  ASSERT_WEBVIEW_FAILED(webview_destroy(w));
}

#if _WIN32
TEST_CASE("Ensure that version number parsing works on Windows") {
  using namespace webview::detail;
  auto v = parse_version("");
  REQUIRE(v.size() == 4);
  REQUIRE(v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0);
  v = parse_version("1");
  REQUIRE(v[0] == 1 && v[1] == 0 && v[2] == 0 && v[3] == 0);
  v = parse_version("0.2");
  REQUIRE(v[0] == 0 && v[1] == 2 && v[2] == 0 && v[3] == 0);
  v = parse_version("0.0.3");
  REQUIRE(v[0] == 0 && v[1] == 0 && v[2] == 3 && v[3] == 0);
  v = parse_version("0.0.0.4");
  REQUIRE(v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 4);
  v = parse_version("1.2.3.4.5");
  REQUIRE(v.size() == 4);
  REQUIRE(v[0] == 1 && v[1] == 2 && v[2] == 3 && v[3] == 4);
  v = parse_version("1.2.3.4.5.6");
  REQUIRE(v[0] == 1 && v[1] == 2 && v[2] == 3 && v[3] == 4);
  v = parse_version("11.22.33.44");
  REQUIRE(v[0] == 11 && v[1] == 22 && v[2] == 33 && v[3] == 44);
  v = parse_version("0.0.0.0");
  REQUIRE(v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0);
  v = parse_version("-1.-2.-3.-4");
  REQUIRE(v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0);
  v = parse_version("a.b.c.d");
  REQUIRE(v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0);
  v = parse_version("...");
  REQUIRE(v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0);
}

TEST_CASE("Ensure that narrow/wide string conversion works on Windows") {
  using namespace webview::detail;
  REQUIRE(widen_string("").empty());
  REQUIRE(narrow_string(L"").empty());
  REQUIRE(widen_string("foo") == L"foo");
  REQUIRE(narrow_string(L"foo") == "foo");
  REQUIRE(widen_string("フー") == L"フー");
  REQUIRE(narrow_string(L"フー") == "フー");
  REQUIRE(widen_string("æøå") == L"æøå");
  REQUIRE(narrow_string(L"æøå") == "æøå");
  // Unicode number for the smiley face below: U+1F600
  REQUIRE(widen_string("😀") == L"😀");
  REQUIRE(narrow_string(L"😀") == "😀");
  // Ensure that elements of wide string are correct
  {
    auto s = widen_string("😀");
    REQUIRE(s.size() == 2);
    REQUIRE(static_cast<std::uint16_t>(s[0]) == 0xD83D);
    REQUIRE(static_cast<std::uint16_t>(s[1]) == 0xDE00);
  }
  // Ensure that elements of narrow string are correct
  {
    auto s = narrow_string(L"😀");
    REQUIRE(s.size() == 4);
    REQUIRE(static_cast<std::uint8_t>(s[0]) == 0xf0);
    REQUIRE(static_cast<std::uint8_t>(s[1]) == 0x9f);
    REQUIRE(static_cast<std::uint8_t>(s[2]) == 0x98);
    REQUIRE(static_cast<std::uint8_t>(s[3]) == 0x80);
  }
  // Null-characters must also be converted
  REQUIRE(widen_string(std::string(2, '\0')) == std::wstring(2, L'\0'));
  REQUIRE(narrow_string(std::wstring(2, L'\0')) == std::string(2, '\0'));
}
#endif
