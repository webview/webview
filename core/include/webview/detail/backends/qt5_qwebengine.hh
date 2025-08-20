/*
 * MIT License
 *
 * Copyright (c) 2025 Serhii Plyhun
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

 #ifndef WEBVIEW_BACKENDS_QT_QWEBENGINE_HH
 #define WEBVIEW_BACKENDS_QT_QWEBENGINE_HH
 
 #if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
 
 #include "../../macros.h"
 
 #if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_QT)
 
 //
 // ====================================================================
 //
 // This implementation uses QWebEngine backend. It requires Qt5 and
 // QWebEngine libraries.
 //
 // ====================================================================
 //
 
 #include "../../errors.hh"
 #include "../../types.hh"
 #include "../engine_base.hh"
 #include "../platform/linux/qt/compat.hh"
 #include "../user_script.hh"
 
 #include <functional>
 #include <list>
 #include <memory>
 #include <string>
 
 #include <QMainWindow>
 #include <QWebEngineScriptCollection>
 #include <QWebEngineView>
 #include <QWebEngineScript>
 #include <QTimer>
 #include <QString>
 #include <QUrl>
 #include <QCoreApplication>
 #include <QApplication>
 #include <QObject>
 #include <QWebEngineSettings>
 #include <QWebChannel>
 #include <QFile>
 #include <QIODevice>
 #include <QList>
 
 #include <fcntl.h>
 #include <sys/stat.h>
 
 namespace webview {
 namespace detail {
 
 class user_script::impl {
 public:
   impl(QWebEngineScript *script) : m_script{script} {}
 
   impl(const impl &) = delete;
   impl &operator=(const impl &) = delete;
   impl(impl &&) = delete;
   impl &operator=(impl &&) = delete;
 
   QWebEngineScript *get_native() const { return m_script; }
 
 private:
   QWebEngineScript *m_script{};
 };
 
 class qt_web_engine : public QObject, public engine_base {
   Q_OBJECT
 public slots:
    void post(const QString &event, const QString &data) {
        m_callback(data.toStdString());
    }
 public:
   qt_web_engine(bool debug, void *window) : engine_base{!window} {
     window_init(window);
     window_settings(debug);
     dispatch_size_default();
   }
 
   qt_web_engine(const qt_web_engine &) = delete;
   qt_web_engine &operator=(const qt_web_engine &) = delete;
   qt_web_engine(qt_web_engine &&) = delete;
   qt_web_engine &operator=(qt_web_engine &&) = delete;
 
   virtual ~qt_web_engine() {
     if (m_window) {
       if (owns_window()) {
         m_window->close();
         on_window_destroyed(true);
       } else {
         qt_compat::window_remove_child(m_window, m_webview);
       }
     }
   }
 
 protected:
   result<void *> window_impl() override {
     if (m_window) {
       return m_window;
     }
     return error_info{WEBVIEW_ERROR_INVALID_STATE};
   }
 
   result<void *> widget_impl() override {
     if (m_webview) {
       return m_webview;
     }
     return error_info{WEBVIEW_ERROR_INVALID_STATE};
   }
 
   result<void *> browser_controller_impl() override {
     if (m_webview) {
       return m_webview;
     }
     return error_info{WEBVIEW_ERROR_INVALID_STATE};
   }
 
   noresult run_impl() override {
     if (owns_window()) {
       m_app->exec();
     } else {
       m_stop_run_loop = false;
       while (!m_stop_run_loop) {
         QCoreApplication::processEvents();
       }
     }
     return {};
   }
 
   noresult terminate_impl() override {
     return dispatch_impl([&] { m_stop_run_loop = true; });
   }
 
   noresult dispatch_impl(std::function<void()> f) override {
     auto* func = new std::function<void()>(f);
     QTimer* timer = new QTimer();
     timer->moveToThread(m_app->thread());
     timer->setSingleShot(true);
     QObject::connect(timer, &QTimer::timeout, [=]() {
        (*func)();
        delete func;
        timer->deleteLater();
     });
     QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
     return {};
   }
 
   noresult set_title_impl(const std::string &title) override {
     m_window->setWindowTitle(QString::fromStdString(title));
     return {};
   }
 
   noresult set_size_impl(int width, int height, webview_hint_t hints) override {
     if (hints == WEBVIEW_HINT_NONE || hints == WEBVIEW_HINT_FIXED) {
       qt_compat::window_set_size(m_window, width, height);
     } else if (hints == WEBVIEW_HINT_MIN) {
       m_window->setMinimumSize(width, height);
     } else if (hints == WEBVIEW_HINT_MAX) {
       qt_compat::window_set_max_size(m_window, width, height);
     } else {
       return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT, "Invalid hint"};
     }
     return window_show();
   }
 
   noresult navigate_impl(const std::string &url) override {
     m_webview->page()->load(QUrl::fromUserInput(QString::fromStdString(url)));
     return {};
   }
 
   noresult set_html_impl(const std::string &html) override {
     m_webview->page()->setHtml(QString::fromStdString(html));
     return {};
   }
 
   noresult eval_impl(const std::string &js) override {
     qDebug() << js.c_str();
     if (!m_webview->page()) {
       return {};
     }
     m_webview->page()->runJavaScript(QString::fromStdString(js), [this](const QVariant &v) {
        on_message(v.toString().toStdString());
     });
     return {};
   }
 
   user_script add_user_script_impl(const std::string &js) override {
     QString script_name = QString::fromStdString("user_script_");
     QList<QWebEngineScript> scripts = m_webview->page()->scripts().findScripts(script_name);
     QString scriptSource;
     if (!scripts.isEmpty()) {
       scriptSource = scripts.first().sourceCode() + QString::fromStdString(js) + QString::fromStdString(";");
       m_webview->page()->scripts().remove(scripts.first());
     } else {
       scriptSource = QString::fromStdString(js) + QString::fromStdString(";");
     }
     QWebEngineScript *wk_script = new QWebEngineScript();
     wk_script->setName(script_name);
     wk_script->setSourceCode(scriptSource);
     wk_script->setWorldId(QWebEngineScript::MainWorld);
     wk_script->setInjectionPoint(QWebEngineScript::DocumentCreation);
     wk_script->setRunsOnSubFrames(true);
     m_webview->page()->scripts().insert(QWebEngineScript(*wk_script));
     user_script script{
         js, user_script::impl_ptr{new user_script::impl{wk_script},
                                   [](user_script::impl *p) { delete p; }}};
     return script;
   }
 
   void remove_all_user_scripts_impl(
       const std::list<user_script> & /*scripts*/) override {
     m_webview->page()->scripts().clear();
   }
 
   bool are_user_scripts_equal_impl(const user_script &first,
                                    const user_script &second) override {
     auto *wk_first = first.get_impl().get_native();
     auto *wk_second = second.get_impl().get_native();
     return wk_first == wk_second;
   }
 
 private:
   QString qwebchannel_js() {
     QFile apiFile(":/qtwebchannel/qwebchannel.js");
     if (!apiFile.open(QIODevice::ReadOnly))
         qDebug() << "Couldn't load Qt's QWebChannel API!";
     QString apiScript = QString::fromLatin1(apiFile.readAll());
     apiFile.close();
     return apiScript;
   }

   void window_init(void *window) {
     m_window = static_cast<QMainWindow *>(window);
     if (owns_window()) {
       m_argc = 0;
       m_app = new QApplication(m_argc, NULL);
       m_window = qt_compat::window_new();
       on_window_created();
       auto on_window_destroy = +[](void* arg) {
         auto *w = static_cast<qt_web_engine *>(arg);
         w->m_window = nullptr;
         w->on_window_destroyed();
       };
       QObject::connect(m_window, &QObject::destroyed, on_window_destroy);
     }
     // Initialize webview widget
     m_webview = new QWebEngineView();
     m_callback = std::bind(&qt_web_engine::on_message, this, std::placeholders::_1);
     m_webchannel = new QWebChannel(m_webview->page());
     m_webchannel->registerObject("qtwebview", this);
     m_webview->page()->setWebChannel(m_webchannel);
     add_user_script(qwebchannel_js().toStdString() + R"DELIM(
(function() {
window.webChannel = new QWebChannel(qt.webChannelTransport, function(channel)
{
    window.qtwebview = channel.objects.qtwebview;
});
})()
     )DELIM");
     add_init_script(R"DELIM(
function(message) {
    return window.qtwebview.post("JavaScript", message);
}
     )DELIM");
   }
 
   void window_settings(bool debug) {
     m_webview->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
   }
 
   noresult window_show() {
     if (m_is_window_shown) {
       return {};
     }
     qt_compat::window_set_child(m_window, m_webview);
     qt_compat::widget_set_visible(m_webview, true);
 
     if (owns_window()) {
       m_window->show();
       m_webview->setFocus();
       qt_compat::widget_set_visible(m_webview, true);
     }
     m_is_window_shown = true;
     return {};
   }
 
   void run_event_loop_while(std::function<bool()> fn) override {
     while (fn()) {
       QCoreApplication::processEvents();
     }
   }
 
   int m_argc;
   std::function<void(const std::string)> m_callback;
   QWebChannel *m_webchannel{};
   QApplication *m_app{};
   QMainWindow *m_window{};
   QWebEngineView *m_webview{};
   bool m_stop_run_loop{};
   bool m_is_window_shown{};
 };

 } // namespace detail
 
 using browser_engine = detail::qt_web_engine;
 
 } // namespace webview
 
 #endif // defined(WEBVIEW_PLATFORM_LINUX) && defined(WEB)
 #endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
 #endif // WEBVIEW_BACKENDS_QT_QWEBENGINE_HH
 