#include "webview/webview.h"
#include <sstream>

const
int width = 500;
const
int height = 500;

int main() 
{
    webview::webview w(true, nullptr);
    w.set_size(width, height, WEBVIEW_HINT_FIXED);
    w.set_title("Snake");
    w.no_ctx();
    w.center();

    std::ostringstream settings;
    settings << "webview=true;" << "width=" << width << ";height=" << height;

    w.init(settings.str());

    w.bind("_exit", [&w](auto) {
        w.terminate();
        return "";
    });


    std::string html = {
    #include "snake.html"
    };
    auto content = "data:text/html," + html;

    auto escaped = webview::escape_js(content);

    w.navigate(escaped);
    w.run();
    return 0;
}
