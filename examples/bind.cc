#include "webview/webview.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

constexpr const auto html =
    R"html(
<div>
  <button id="increment">+</button>
  <button id="decrement">−</button>
  <span>Counter: <span id="counterResult">0</span></span>
</div>
<hr />
<div>
  <button id="compute">Compute</button>
  <span>Result: <span id="computeResult">(not started)</span></span>
</div>
<script type="module">
  const getElements = ids => Object.assign({}, ...ids.map(
    id => ({ [id]: document.getElementById(id) })));
  const ui = getElements([
    "increment", "decrement", "counterResult", "compute",
    "computeResult"
  ]);
  ui.increment.addEventListener("click", async () => {
    ui.counterResult.textContent = await window.count(1);
  });
  ui.decrement.addEventListener("click", async () => {
    ui.counterResult.textContent = await window.count(-1);
  });
  ui.compute.addEventListener("click", async () => {
    ui.compute.disabled = true;
    ui.computeResult.textContent = "(pending)";
    ui.computeResult.textContent = await window.compute(6, 7);
    ui.compute.disabled = false;
  });
</script>)html";

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
  try {
    long count = 0;

    webview::webview w(true, nullptr);
    w.set_title("Bind Example");
    w.set_size(480, 320, WEBVIEW_HINT_NONE);

    // A binding that counts up or down and immediately returns the new value.
    w.bind("count", [&](const std::string &req) -> std::string {
      // Imagine that req is properly parsed or use your own JSON parser.
      auto direction = std::stol(req.substr(1, req.size() - 1));
      return std::to_string(count += direction);
    });

    // A binding that creates a new thread and returns the result at a later time.
    w.bind(
        "compute",
        [&](const std::string &id, const std::string &req, void * /*arg*/) {
          // Create a thread and forget about it for the sake of simplicity.
          std::thread([&, id, req] {
            // Simulate load.
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // Imagine that req is properly parsed or use your own JSON parser.
            const auto *result = "42";
            w.resolve(id, 0, result);
          }).detach();
        },
        nullptr);

    w.set_html(html);
    w.run();
  } catch (const webview::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
