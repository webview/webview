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

 #ifndef WEBVIEW_PLATFORM_LINUX_QT_COMPAT_HH
 #define WEBVIEW_PLATFORM_LINUX_QT_COMPAT_HH
 
 #if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
 
 #include "../../../../macros.h"
 
 #if defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_QT)
 
 #include <QObject>
 #include <QWidget>
 #include <QMainWindow>
 #include <QWebEnginePage>
 #include <QApplication>
 
 namespace webview {
 namespace detail {
 
 /**
  * GTK compatibility helper class.
  */
 class qt_compat {
 public:
   static QMainWindow *window_new() {
      return new QMainWindow();
   }
 
   static void window_set_child(QMainWindow *window, QWidget *widget) {
      window->setCentralWidget(widget);
   }
 
   static void window_remove_child(QMainWindow *window, QWidget *widget) {
      if (widget == window->centralWidget()) {
        window->takeCentralWidget();
      }
   }
 
   static void widget_set_visible(QWidget *widget, bool visible) {
      widget->setVisible(visible);
   }
 
   static void window_set_size(QMainWindow *window, int width, int height) {
      window->setFixedSize(width, height);
   }
 
   static void window_set_max_size(QMainWindow *window, int width, int height) {
      window->setMaximumSize(width, height);
   }
 };
 
 } // namespace detail
 } // namespace webview
 
 #endif // defined(WEBVIEW_PLATFORM_LINUX) && defined(WEBVIEW_QT)
 #endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
 #endif // WEBVIEW_PLATFORM_LINUX_QT_COMPAT_HH
 