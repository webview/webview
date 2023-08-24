#include "webview.h"

#include <list>
#include <memory>
#include <string>

constexpr const auto primary_html =
    R"html(<button id="new-window">New window</button>
<script>
  const newWindowButton = document.querySelector("#new-window");
  document.addEventListener("DOMContentLoaded", () => {
    newWindowButton.addEventListener("click", () => {
      window.newWindow();
    });
  });
</script>)html";

constexpr const auto secondary_html = R"html(<div>Secondary window</div>)html";

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
  std::list<webview::webview> instances;

  auto &primary = *instances.emplace(instances.end(), false, nullptr);
  primary.set_title("Multiple Windows Example");
  primary.set_size(480, 320, WEBVIEW_HINT_NONE);

  primary.bind("newWindow", [&](const std::string & /*req*/) -> std::string {
#ifdef WEBVIEW_GTK
    auto *native_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#else
#error Not implemented yet
#endif
    auto &secondary = *instances.emplace(instances.end(), false, native_window);
    secondary.set_title("Secondary Window");
    secondary.set_size(480, 320, WEBVIEW_HINT_NONE);
    secondary.set_html(secondary_html);
    return "";
  });

  primary.set_html(primary_html);
  primary.run();

  return 0;
}
