#include "webview.h"

#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
  webview::webview w(false, nullptr);
  w.init("window.external.invoke('init')");
  w.init("window.onload = () => window.external.invoke('window.onload')");
  w.init("document.addEventListener('DOMContentLoaded', () => window.external.invoke('domready'))");
  w.init("document.onreadystatechange = () => window.external.invoke('state change ' + document.readyState)");
  w.set_title("Example");
  w.set_size(480, 320, true);
  w.navigate("data:text/html,<html><body>hello</body></html>");
  w.run();
  return 0;
}
