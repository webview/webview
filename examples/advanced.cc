#include "webview/webview.h"

#include <iostream>
#include <string>

constexpr const auto html =
    R"html(
<html>
<body>
<style>
  .message { color: red; margin-top: 8px; }
</style>

  <h2>Download</h2>
  <button id="download">Download a document</button>

  <h2>Navigation Error</h2>
  <button onclick="window.location.href='https://github.com/navigation_error';">
    navigation_error_callback
  </button>

  <h2>Decide Policy Navigation</h2>
  <a href="https://google.com">
    <button>Blocked</button>
  </a>
  <a href="https://webkit.org" target="_blank" rel="noopener">
    <button>New Window</button>
  </a>

  <h2>Message</h2>
  <div id="message" class="message"></div>
</body>
</html>

<script>
  function showMessage(msg) {
    document.getElementById('message').textContent = msg;
  }
</script>

<script type="module">
  const getElements = ids => Object.assign({}, ...ids.map(
    id => ({ [id]: document.getElementById(id) })));
  const ui = getElements([
    "download",
  ]);
  ui.download.addEventListener("click", async () => {
    const blob = new Blob(['Hello world'], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'hello.txt';
    a.click();
    URL.revokeObjectURL(url);
  });
</script>
)html";

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
  try {

    webview::webview w(true, nullptr);
    w.set_title("Advanced Example");
    w.set_size(480, 640, WEBVIEW_HINT_NONE);

    // Handle a navigation error
    w.set_navigation_error_callback(
        [&](int httpStatusCode, void * /*arg*/) -> bool {
          std::cout << "Navigation error: " << httpStatusCode << std::endl;
          if (httpStatusCode == 404) {
            // Show a message for 5 seconds.
            const auto msg =
                R"(<div id="err"><b>Navigation error occured</b></div>)"
                R"(<script type="module">setTimeout(() => )"
                R"(document.getElementById('err').remove(), 3000);</script>)";
            w.set_html(html + std::string(msg));
            return true;
          }
          // Further handling of Webengine.
          return false;
        },
        nullptr);

    // Handle Navigation
    w.set_decide_policy_navigation_callback(
        [&](const char *url, bool /*triggeredByReload*/,
            void * /*arg*/) -> bool {
          // Don't be evil
          const bool allow =
              std::string(url).find("google.com") == std::string::npos;
          const auto msg =
              std::string(allow ? "Allow" : "Decline") + ": " + url;
          w.eval("showMessage('" + msg + "');");
          return allow;
        },
        nullptr);

    // webview_set_cookie() does not work with loaded html, a server connection is required

    w.set_html(html);
    w.run();
  } catch (const webview::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
