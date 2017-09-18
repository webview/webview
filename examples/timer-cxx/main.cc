#include <chrono>
#include <mutex>
#include <sstream>
#include <thread>

#include <cstdio>
#include <cstring>

#include <webview.h>

class Timer {
public:
  int get() {
    this->mutex.lock();
    int n = this->ticks;
    this->mutex.unlock();
    return n;
  }
  void set(int ticks) {
    this->mutex.lock();
    this->ticks = ticks;
    this->mutex.unlock();
  }
  void incr(int n = 1) {
    this->mutex.lock();
    this->ticks = this->ticks + n;
    this->mutex.unlock();
  }
  void start(struct webview *w) {
    this->thread = std::thread(&Timer::run, this, w);
    this->thread.detach();
  }
  void render(struct webview *w) {
    auto n = this->get();
    std::ostringstream jscode;
    jscode << "updateTicks(" << n << ")";
    webview_eval(w, jscode.str().c_str());
  }

private:
  void run(struct webview *w) {
    for (;;) {
      std::this_thread::sleep_for(std::chrono::microseconds(100000));
      this->incr();
      webview_dispatch(w,
                       [](struct webview *w, void *arg) {
                         Timer *timer = static_cast<Timer *>(arg);
                         timer->render(w);
                       },
                       this);
    }
  }
  std::thread thread;
  std::mutex mutex;
  int ticks = 0;
  struct webview *w;
};

class TempFile {
public:
  TempFile(std::string contents) {
    this->path = std::tmpnam(nullptr);
    FILE *f = fopen(this->path.c_str(), "wb");
    fwrite(contents.c_str(), contents.length(), 1, f);
    fclose(f);
  }
  ~TempFile() { unlink(this->path.c_str()); }
  std::string url() { return "file://" + path; }

private:
  std::string path;
};

static const char *html = R"html(
<html>
  <body>
    <p id='ticks'></p>
    <button onclick='external.invoke_("reset")'>reset</button>
    <button onclick='external.invoke_("exit")'>exit</button>
    <script>
      function updateTicks(n) { 
        document.getElementById('ticks').innerHTML = 'ticks '+n;
      }
    </script>
  </body>
</html>
)html";

static void timer_cb(struct webview *w, const char *arg) {
  Timer *timer = static_cast<Timer *>(w->userdata);
  if (strcmp(arg, "reset") == 0) {
    timer->set(0);
    timer->render(w);
  } else if (strcmp(arg, "exit") == 0) {
    webview_terminate(w);
  }
}

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  Timer timer;
  TempFile tmp(html);
  struct webview webview = {};

  std::string url = tmp.url();

  webview.title = "Timer";
  webview.url = url.c_str();
  webview.width = 400;
  webview.height = 300;
  webview.resizable = 0;
  webview.external_invoke_cb = timer_cb;
  webview.userdata = &timer;

  webview_init(&webview);
  timer.start(&webview);
  while (webview_loop(&webview, 1) == 0)
    ;
  webview_exit(&webview);
  return 0;
}
