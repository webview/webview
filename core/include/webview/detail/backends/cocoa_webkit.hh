/*
 * MIT License
 *
 * Copyright (c) 2017 Serge Zaitsev
 * Copyright (c) 2022 Steffen André Langnes
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

#ifndef WEBVIEW_BACKENDS_COCOA_WEBKIT_HH
#define WEBVIEW_BACKENDS_COCOA_WEBKIT_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)

//
// ====================================================================
//
// This implementation uses Cocoa WKWebView backend on macOS. It is
// written using ObjC runtime and uses WKWebView class as a browser runtime.
// You should pass "-framework Webkit" flag to the compiler.
//
// ====================================================================
//

#include "../../types.hh"
#include "../engine_base.hh"
#include "../platform/darwin/cocoa/cocoa.hh"
#include "../platform/darwin/objc/objc.hh"
#include "../platform/darwin/webkit/webkit.hh"
#include "../user_script.hh"

#include <atomic>
#include <cstring>
#include <functional>
#include <list>
#include <memory>
#include <string>

#include <objc/objc-runtime.h>

namespace webview {
namespace detail {

class user_script::impl {
public:
  impl(id script) : m_script{objc::retain(script)} {}

  ~impl() { objc::release(m_script); }

  impl(const impl &) = delete;
  impl &operator=(const impl &) = delete;
  impl(impl &&) = delete;
  impl &operator=(impl &&) = delete;

  id get_native() const { return m_script; }

private:
  id m_script{};
};

// Encapsulate backend in its own namespace to avoid polluting the parent
// namespace when pulling in commonly-used symbols from other namespaces.
// Since those commmon symbols are used a lot, this reduces the overall
// noise in the code.
namespace cocoa_webkit {

using namespace cocoa;
using namespace webkit;

class cocoa_wkwebview_engine : public engine_base {
public:
  cocoa_wkwebview_engine(bool debug, void *window,
                         const std::string &scheme = {},
                         const std::string &scheme_root = {})
      : engine_base{!window}, m_app{NSApplication_get_sharedApplication()} {
    m_scheme = scheme;
    m_scheme_root = scheme_root;
    window_init(window);
    window_settings(debug);
    dispatch_size_default();
  }

  cocoa_wkwebview_engine(const cocoa_wkwebview_engine &) = delete;
  cocoa_wkwebview_engine &operator=(const cocoa_wkwebview_engine &) = delete;
  cocoa_wkwebview_engine(cocoa_wkwebview_engine &&) = delete;
  cocoa_wkwebview_engine &operator=(cocoa_wkwebview_engine &&) = delete;

  virtual ~cocoa_wkwebview_engine() {
    objc::autoreleasepool arp;
    if (m_window) {
      if (m_webview) {
        if (auto ui_delegate{WKWebView_get_UIDelegate(m_webview)}) {
          WKWebView_set_UIDelegate(m_webview, nullptr);
          objc::release(ui_delegate);
        }
        objc::release(m_webview);
        m_webview = nullptr;
      }
      if (m_widget) {
        if (m_widget == NSWindow_get_contentView(m_window)) {
          NSWindow_set_contentView(m_window, nullptr);
        }
        objc::release(m_widget);
        m_widget = nullptr;
      }
      if (owns_window()) {
        // Replace delegate to avoid callbacks and other bad things during
        // destruction.
        NSWindow_set_delegate(m_window, nullptr);
        NSWindow_close(m_window);
        on_window_destroyed(true);
      }
      m_window = nullptr;
    }
    if (m_window_delegate) {
      objc::release(m_window_delegate);
      m_window_delegate = nullptr;
    }
    if (m_app_delegate) {
      NSApplication_set_delegate(m_app, nullptr);
      // Make sure to release the delegate we created.
      objc::release(m_app_delegate);
      m_app_delegate = nullptr;
    }
    if (owns_window()) {
      // Needed for the window to close immediately.
      deplete_run_loop_event_queue();
    }
    // TODO: Figure out why m_manager is still alive after the autoreleasepool
    // has been drained.
  }

protected:
  result<void *> window_impl() override {
    if (m_window) {
      return m_window;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }

  result<void *> widget_impl() override {
    if (m_widget) {
      return m_widget;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }

  result<void *> browser_controller_impl() override {
    if (m_webview) {
      return m_webview;
    }
    return error_info{WEBVIEW_ERROR_INVALID_STATE};
  }

  noresult terminate_impl() override {
    stop_run_loop();
    return {};
  }

  noresult run_impl() override {
    NSApplication_run(m_app);
    return {};
  }

  noresult dispatch_impl(std::function<void()> f) override {
    dispatch_async_f(dispatch_get_main_queue(), new dispatch_fn_t(f),
                     (dispatch_function_t)([](void *arg) {
                       auto f = static_cast<dispatch_fn_t *>(arg);
                       (*f)();
                       delete f;
                     }));
    return {};
  }

  noresult set_title_impl(const std::string &title) override {
    NSWindow_set_title(m_window, title);
    return {};
  }
  noresult set_size_impl(int width, int height, webview_hint_t hints) override {
    objc::autoreleasepool arp;

    auto style = static_cast<NSWindowStyleMask>(
        NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable);
    if (hints != WEBVIEW_HINT_FIXED) {
      style =
          static_cast<NSWindowStyleMask>(style | NSWindowStyleMaskResizable);
    }
    NSWindow_set_styleMask(m_window, style);

    if (hints == WEBVIEW_HINT_MIN) {
      NSWindow_set_contentMinSize(m_window, NSSizeMake(width, height));
    } else if (hints == WEBVIEW_HINT_MAX) {
      NSWindow_set_contentMaxSize(m_window, NSSizeMake(width, height));
    } else {
      auto rect{NSWindow_get_frame(m_window)};
      NSWindow_setFrame(m_window,
                        NSRectMake(rect.origin.x, rect.origin.y, width, height),
                        true, false);
    }
    NSWindow_center(m_window);

    return window_show();
  }
  noresult navigate_impl(const std::string &url) override {
    objc::autoreleasepool arp;

    WKWebView_loadRequest(
        m_webview, NSURLRequest_requestWithURL(NSURL_URLWithString(url)));

    return {};
  }
  noresult set_html_impl(const std::string &html) override {
    objc::autoreleasepool arp;
    WKWebView_loadHTMLString(m_webview, NSString_stringWithUTF8String(html),
                             nullptr);
    return {};
  }
  noresult eval_impl(const std::string &js) override {
    objc::autoreleasepool arp;
    // URI is null before content has begun loading.
    auto nsurl{WKWebView_get_URL(m_webview)};
    if (!nsurl) {
      return {};
    }
    WKWebView_evaluateJavaScript(m_webview, NSString_stringWithUTF8String(js),
                                 nullptr);
    return {};
  }

  user_script add_user_script_impl(const std::string &js) override {
    objc::autoreleasepool arp;
    auto wk_script{WKUserScript_withSource(
        NSString_stringWithUTF8String(js),
        WKUserScriptInjectionTimeAtDocumentStart, true)};
    // Script is retained when added.
    WKUserContentController_addUserScript(m_manager, wk_script);
    user_script script{
        js, user_script::impl_ptr{new user_script::impl{wk_script},
                                  [](user_script::impl *p) { delete p; }}};
    return script;
  }

  void remove_all_user_scripts_impl(
      const std::list<user_script> & /*scripts*/) override {
    objc::autoreleasepool arp;
    // Removing scripts decreases the retain count of each script.
    WKUserContentController_removeAllUserScripts(m_manager);
  }

  bool are_user_scripts_equal_impl(const user_script &first,
                                   const user_script &second) override {
    auto *wk_first = first.get_impl().get_native();
    auto *wk_second = second.get_impl().get_native();
    return wk_first == wk_second;
  }

private:
  id create_app_delegate() {
    objc::autoreleasepool arp;
    constexpr auto class_name = "WebviewAppDelegate";
    // Avoid crash due to registering same class twice
    auto cls = objc_lookUpClass(class_name);
    if (!cls) {
      // Note: Avoid registering the class name "AppDelegate" as it is the
      // default name in projects created with Xcode, and using the same name
      // causes objc_registerClassPair to crash.
      cls =
          objc_allocateClassPair(objc::get_class("NSResponder"), class_name, 0);
      class_addProtocol(cls, objc_getProtocol("NSTouchBarProvider"));
      class_addMethod(
          cls,
          objc::selector("applicationShouldTerminateAfterLastWindowClosed:"),
          (IMP)(+[](id, SEL, id) -> BOOL { return NO; }), "c@:@");
      class_addMethod(cls, objc::selector("applicationDidFinishLaunching:"),
                      (IMP)(+[](id self, SEL, id notification) {
                        auto app{NSNotification_get_object(notification)};
                        auto w = get_associated_webview(self);
                        w->on_application_did_finish_launching(self, app);
                      }),
                      "v@:@");
      objc_registerClassPair(cls);
    }
    return objc::Class_new(cls);
  }
  id create_script_message_handler() {
    objc::autoreleasepool arp;
    constexpr auto class_name = "WebviewWKScriptMessageHandler";
    // Avoid crash due to registering same class twice
    auto cls = objc_lookUpClass(class_name);
    if (!cls) {
      cls =
          objc_allocateClassPair(objc::get_class("NSResponder"), class_name, 0);
      class_addProtocol(cls, objc_getProtocol("WKScriptMessageHandler"));
      class_addMethod(
          cls, objc::selector("userContentController:didReceiveScriptMessage:"),
          (IMP)(+[](id self, SEL, id, id msg) {
            auto w = get_associated_webview(self);
            w->on_message(
                NSString_get_UTF8String(WKScriptMessage_get_body(msg)));
          }),
          "v@:@@");
      objc_registerClassPair(cls);
    }
    auto instance{objc::Class_new(cls)};
    set_associated_webview(instance, this);
    return instance;
  }
  static id create_webkit_ui_delegate() {
    objc::autoreleasepool arp;
    constexpr auto class_name = "WebviewWKUIDelegate";
    // Avoid crash due to registering same class twice
    auto cls = objc_lookUpClass(class_name);
    if (!cls) {
      cls = objc_allocateClassPair(objc::get_class("NSObject"), class_name, 0);
      class_addProtocol(cls, objc_getProtocol("WKUIDelegate"));
      class_addMethod(
          cls,
          objc::selector("webView:runOpenPanelWithParameters:initiatedByFrame:"
                         "completionHandler:"),
          (IMP)(+[](id, SEL, id, id parameters, id, id completion_handler) {
            auto allows_multiple_selection{
                WKOpenPanelParameters_get_allowsMultipleSelection(parameters)};
            auto allows_directories{
                WKOpenPanelParameters_get_allowsDirectories(parameters)};

            // Show a panel for selecting files.
            auto panel{NSOpenPanel_openPanel()};
            NSOpenPanel_set_canChooseFiles(panel, true);
            NSOpenPanel_set_canChooseDirectories(panel, allows_directories);
            NSOpenPanel_set_allowsMultipleSelection(panel,
                                                    allows_multiple_selection);
            auto modal_response{NSSavePanel_runModal(panel)};

            // Get the URLs for the selected files. If the modal was canceled
            // then we pass null to the completion handler to signify
            // cancellation.
            id urls{modal_response == NSModalResponseOK
                        ? NSOpenPanel_get_URLs(panel)
                        : nullptr};

            // Invoke the completion handler block.
            auto sig{NSMethodSignature_signatureWithObjCTypes("v@?@")};
            auto invocation{NSInvocation_invocationWithMethodSignature(sig)};
            NSInvocation_set_target(invocation, completion_handler);
            NSInvocation_setArgument(invocation, &urls, 1);
            NSInvocation_invoke(invocation);
          }),
          "v@:@@@@");
      objc_registerClassPair(cls);
    }
    return objc::Class_new(cls);
  }
  static id create_window_delegate() {
    objc::autoreleasepool arp;
    constexpr auto class_name = "WebviewNSWindowDelegate";
    // Avoid crash due to registering same class twice
    auto cls = objc_lookUpClass(class_name);
    if (!cls) {
      cls = objc_allocateClassPair(objc::get_class("NSObject"), class_name, 0);
      class_addProtocol(cls, objc_getProtocol("NSWindowDelegate"));
      class_addMethod(cls, objc::selector("windowWillClose:"),
                      (IMP)(+[](id self, SEL, id notification) {
                        auto window{NSNotification_get_object(notification)};
                        auto w = get_associated_webview(self);
                        w->on_window_will_close(self, window);
                      }),
                      "v@:@");
      objc_registerClassPair(cls);
    }
    return objc::Class_new(cls);
  }
  static cocoa_wkwebview_engine *get_associated_webview(id object) {
    objc::autoreleasepool arp;
    if (id assoc_obj{objc_getAssociatedObject(object, "webview")}) {
      cocoa_wkwebview_engine *w{};
      NSValue_getValue(assoc_obj, &w, sizeof(w));
      return w;
    }
    return nullptr;
  }
  static void set_associated_webview(id object, cocoa_wkwebview_engine *w) {
    objc::autoreleasepool arp;
    objc_setAssociatedObject(object, "webview", NSValue_valueWithPointer(w),
                             OBJC_ASSOCIATION_RETAIN);
  }
  static bool is_app_bundled() noexcept {
    auto bundle = NSBundle_get_mainBundle();
    if (!bundle) {
      return false;
    }
    auto bundle_path = NSBundle_get_bundlePath(bundle);
    auto bundled =
        NSString_hasSuffix(bundle_path, NSString_stringWithUTF8String(".app"));
    return !!bundled;
  }
  void on_application_did_finish_launching(id /*delegate*/, id app) {
    // See comments related to application lifecycle in create_app_delegate().
    if (owns_window()) {
      // Stop the main run loop so that we can return
      // from the constructor.
      stop_run_loop();
    }

    // Activate the app if it is not bundled.
    // Bundled apps launched from Finder are activated automatically but
    // otherwise not. Activating the app even when it has been launched from
    // Finder does not seem to be harmful but calling this function is rarely
    // needed as proper activation is normally taken care of for us.
    // Bundled apps have a default activation policy of
    // NSApplicationActivationPolicyRegular while non-bundled apps have a
    // default activation policy of NSApplicationActivationPolicyProhibited.
    if (!is_app_bundled()) {
      // "setActivationPolicy:" must be invoked before
      // "activateIgnoringOtherApps:" for activation to work.
      NSApplication_setActivationPolicy(app,
                                        NSApplicationActivationPolicyRegular);
      // Activate the app regardless of other active apps.
      // This can be obtrusive so we only do it when necessary.
      NSApplication_activateIgnoringOtherApps(app, true);
    }

    window_init_proceed();
  }
  void on_window_will_close(id /*delegate*/, id /*window*/) {
    // Widget destroyed along with window.
    m_widget = nullptr;
    m_webview = nullptr;
    m_window = nullptr;
    dispatch([this] { on_window_destroyed(); });
  }
  // ---- custom URL scheme handler (serves files from a root directory) ----

  static void set_scheme_root(id handler, const std::string &root) {
    objc_setAssociatedObject(handler, "webview_scheme_root",
                             NSString_stringWithUTF8String(root.c_str()),
                             OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  }

  static std::string get_scheme_root(id handler) {
    if (auto root{static_cast<id>(
            objc_getAssociatedObject(handler, "webview_scheme_root"))}) {
      return NSString_get_UTF8String(root);
    }
    return {};
  }

  id create_scheme_handler() {
    objc::autoreleasepool arp;
    constexpr auto class_name = "WebviewWKURLSchemeHandler";
    // Avoid crash due to registering same class twice; the IMPs are static
    // and read their per-instance state from an associated object, so one
    // class is shared by every engine instance.
    auto cls = objc_lookUpClass(class_name);
    if (!cls) {
      cls = objc_allocateClassPair(objc::get_class("NSObject"), class_name, 0);
      class_addProtocol(cls, objc_getProtocol("WKURLSchemeHandler"));
      class_addMethod(cls, objc::selector("webView:startURLSchemeTask:"),
                      (IMP)(&scheme_start), "v@:@@");
      class_addMethod(cls, objc::selector("webView:stopURLSchemeTask:"),
                      (IMP)(&scheme_stop), "v@:@@");
      objc_registerClassPair(cls);
    }
    return objc::Class_new(cls);
  }

  void install_scheme_handler(id config) {
    m_scheme_handler = create_scheme_handler();
    set_scheme_root(m_scheme_handler, m_scheme_root);
    objc::msg_send<void>(
        config, objc::selector("setURLSchemeHandler:forURLScheme:"),
        m_scheme_handler, NSString_stringWithUTF8String(m_scheme.c_str()));
  }

  static void scheme_start(id self, SEL cmd, id webview, id task) noexcept;
  static void scheme_stop(id self, SEL cmd, id webview, id task) noexcept;

  void window_settings(bool debug) {
    objc::autoreleasepool arp;

    auto config{objc::autorelease(WKWebViewConfiguration_new())};

    // A custom URL scheme handler must be registered on the configuration
    // BEFORE the WKWebView is created; late registration on an existing
    // web view is not possible. That is why the scheme is a creation-time
    // parameter (webview_create_from_config) rather than a setter.
    if (!m_scheme.empty() && !m_scheme_root.empty()) {
      install_scheme_handler(config);
    }

    m_manager = WKWebViewConfiguration_get_userContentController(config);

    auto preferences = WKWebViewConfiguration_get_preferences(config);
    auto yes_value = NSNumber_numberWithBool(true);

    if (debug) {
      NSObject_setValue_forKey(
          preferences, yes_value,
          NSString_stringWithUTF8String("developerExtrasEnabled"));
    }

    NSObject_setValue_forKey(
        preferences, yes_value,
        NSString_stringWithUTF8String("fullScreenEnabled"));

#if defined(__has_builtin)
#if __has_builtin(__builtin_available)
    if (__builtin_available(macOS 10.13, *)) {
      NSObject_setValue_forKey(
          preferences, yes_value,
          NSString_stringWithUTF8String("javaScriptCanAccessClipboard"));
      NSObject_setValue_forKey(
          preferences, yes_value,
          NSString_stringWithUTF8String("DOMPasteAllowed"));
    }
#else
#error __builtin_available not supported by compiler
#endif
#else
#error __has_builtin not supported by compiler
#endif

    auto ui_delegate = create_webkit_ui_delegate();
    m_webview =
        objc::retain(WKWebView_withFrame(CGRectMake(0, 0, 0, 0), config));
    // Autoresizing mask is needed to prevent the Web Inspector pane from
    // pushing the main web view out of bounds
    auto autoresizing_mask{static_cast<NSAutoresizingMaskOptions>(
        NSViewWidthSizable | NSViewMaxXMargin | NSViewHeightSizable |
        NSViewMaxYMargin)};
    NSView_set_autoresizingMask(m_webview, autoresizing_mask);
    set_associated_webview(ui_delegate, this);
    WKWebView_set_UIDelegate(m_webview, ui_delegate);

    if (debug) {
      // Explicitly make WKWebView inspectable via Safari on OS versions that
      // disable the feature by default (macOS 13.3 and later) and support
      // enabling it. According to Apple, the behavior on older OS versions is
      // for content to always be inspectable in "debug builds".
      // Testing shows that this is true for macOS 12.6 but somehow not 10.15.
      // https://webkit.org/blog/13936/enabling-the-inspection-of-web-content-in-apps/
      WKWebView_set_inspectable(m_webview, true);
    }

    auto script_message_handler =
        objc::autorelease(create_script_message_handler());
    WKUserContentController_addScriptMessageHandler(
        m_manager, script_message_handler,
        NSString_stringWithUTF8String("__webview__"));

    add_init_script("function(message) {\n\
  return window.webkit.messageHandlers.__webview__.postMessage(message);\n\
}");
    set_up_widget();
    NSWindow_set_contentView(m_window, m_widget);
    if (owns_window()) {
      NSWindow_makeKeyAndOrderFront(m_window);
    }
  }
  void set_up_widget() {
    objc::autoreleasepool arp;
    // Create a new view that can contain both the web view and the Web Inspector pane
    m_widget = objc::retain(NSView_withFrame(NSRectMake(0, 0, 0, 0)));
    // Autoresizing is needed because the Web Inspector pane is a sibling of the web view
    NSView_set_autoresizesSubviews(m_widget, true);
    NSView_addSubview(m_widget, m_webview);
    NSView_set_frame(m_webview, NSView_get_bounds(m_widget));
  }
  void stop_run_loop() {
    objc::autoreleasepool arp;
    // Request the run loop to stop. This doesn't immediately stop the loop.
    NSApplication_stop(m_app);
    // The run loop will stop after processing an NSEvent.
    auto event{NSEvent_otherEventWithType(
        NSEventTypeApplicationDefined, NSPointMake(0, 0),
        NSEventModifierFlags{}, 0, 0, nullptr, 0, 0, 0)};
    NSApplication_postEvent(m_app, event, true);
  }
  static bool get_and_set_is_first_instance() noexcept {
    static std::atomic_bool first{true};
    bool temp = first;
    if (temp) {
      first = false;
    }
    return temp;
  }
  void window_init(void *window) {
    objc::autoreleasepool arp;

    m_window = static_cast<id>(window);
    if (!owns_window()) {
      return;
    }

    // Skip application setup if this isn't the first instance of this class
    // because the launch event is only sent once.
    if (!get_and_set_is_first_instance()) {
      window_init_proceed();
      return;
    }

    m_app_delegate = create_app_delegate();
    set_associated_webview(m_app_delegate, this);
    NSApplication_set_delegate(m_app, m_app_delegate);

    // Start the main run loop so that the app delegate gets the
    // NSApplicationDidFinishLaunchingNotification notification after the run
    // loop has started in order to perform further initialization.
    // We need to return from this constructor so this run loop is only
    // temporary.
    NSApplication_run(m_app);
  }
  void window_init_proceed() {
    objc::autoreleasepool arp;

    m_window = objc::retain(NSWindow_withContentRect(
        NSRectMake(0, 0, 0, 0), NSWindowStyleMaskTitled, NSBackingStoreBuffered,
        false));
    m_window_delegate = create_window_delegate();
    set_associated_webview(m_window_delegate, this);
    NSWindow_set_delegate(m_window, m_window_delegate);
    on_window_created();
  }

  noresult window_show() {
    objc::autoreleasepool arp;
    if (m_is_window_shown) {
      return {};
    }
    m_is_window_shown = true;
    return {};
  }

  void run_event_loop_while(std::function<bool()> fn) override {
    objc::autoreleasepool arp;
    while (fn()) {
      objc::autoreleasepool arp2;
      if (auto event{NSApplication_nextEventMatchingMask(
              m_app, NSEventMaskAny, nullptr,
              NSRunLoopMode::NSDefaultRunLoopMode(), true)}) {
        NSApplication_sendEvent(m_app, event);
      }
    }
  }

  id m_app{};
  id m_app_delegate{};
  id m_window_delegate{};
  id m_window{};
  id m_widget{};
  id m_webview{};
  id m_manager{};
  bool m_is_window_shown{};
  std::string m_scheme{};
  std::string m_scheme_root{};
  id m_scheme_handler{};
};

inline void cocoa_wkwebview_engine::scheme_stop(id, SEL, id, id) noexcept {
  // Content is delivered in a single shot from scheme_start; there is no
  // streaming state to cancel.
}

inline void cocoa_wkwebview_engine::scheme_start(id self, SEL, id,
                                                 id task) noexcept {
  objc::autoreleasepool arp;
  try {
    auto request{objc::msg_send<id>(task, objc::selector("request"))};
    auto url{objc::msg_send<id>(request, objc::selector("URL"))};

    // Map scheme://<host>/<path> onto <root>/<path>. Percent-escapes in
    // the path are decoded first so that e.g. %20 works as expected.
    auto url_str{objc::msg_send<id>(url, objc::selector("absoluteString"))};
    auto url_text = std::string{NSString_get_UTF8String(url_str)};
    auto pos = url_text.find("://");
    auto path =
        pos == std::string::npos ? std::string{} : url_text.substr(pos + 3);
    auto slash = path.find('/');
    path = slash == std::string::npos ? std::string{} : path.substr(slash + 1);
    std::string decoded;
    for (std::size_t i = 0; i < path.size(); i++) {
      if (path[i] == '%' && i + 2 < path.size()) {
        auto hex = [](char c) -> int {
          if (c >= '0' && c <= '9') {
            return c - '0';
          }
          if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
          }
          if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
          }
          return -1;
        };
        auto hi = hex(path[i + 1]);
        auto lo = hex(path[i + 2]);
        if (hi >= 0 && lo >= 0) {
          decoded += static_cast<char>((hi << 4) | lo);
          i += 2;
          continue;
        }
      }
      decoded += path[i];
    }

    // Confine requests to the root directory: reject any ".." component.
    auto is_dotdot = [](const std::string &s, std::size_t i) {
      return s.compare(i, 2, "..") == 0 &&
             (i + 2 == s.size() || s[i + 2] == '/');
    };
    auto forbidden = decoded.empty() || decoded == "..";
    for (std::size_t i = 0; !forbidden && i < decoded.size(); i++) {
      if ((i == 0 || decoded[i - 1] == '/') && is_dotdot(decoded, i)) {
        forbidden = true;
      }
    }

    auto full =
        forbidden ? std::string{} : get_scheme_root(self) + "/" + decoded;
    auto data{full.empty() ? id{}
                           : objc::msg_send<id>(
                                 objc::get_class("NSData"),
                                 objc::selector("dataWithContentsOfFile:"),
                                 NSString_stringWithUTF8String(full.c_str()))};

    // Minimal MIME detection so WKWebView parses HTML/CSS/JS correctly.
    auto mime = std::string{"application/octet-stream"};
    auto ends_with = [&](const char *suffix) {
      auto len = std::strlen(suffix);
      return decoded.size() >= len &&
             decoded.compare(decoded.size() - len, len, suffix) == 0;
    };
    if (ends_with(".html") || ends_with(".htm")) {
      mime = "text/html";
    } else if (ends_with(".js")) {
      mime = "application/javascript";
    } else if (ends_with(".css")) {
      mime = "text/css";
    } else if (ends_with(".json")) {
      mime = "application/json";
    } else if (ends_with(".svg")) {
      mime = "image/svg+xml";
    } else if (ends_with(".png")) {
      mime = "image/png";
    } else if (ends_with(".jpg") || ends_with(".jpeg")) {
      mime = "image/jpeg";
    } else if (ends_with(".gif")) {
      mime = "image/gif";
    } else if (ends_with(".webp")) {
      mime = "image/webp";
    } else if (ends_with(".ico")) {
      mime = "image/x-icon";
    } else if (ends_with(".txt")) {
      mime = "text/plain";
    }

    auto headers{objc::msg_send<id>(objc::get_class("NSMutableDictionary"),
                                    objc::selector("dictionary"))};
    objc::msg_send<void>(headers, objc::selector("setObject:forKey:"),
                         NSString_stringWithUTF8String(mime.c_str()),
                         NSString_stringWithUTF8String("Content-Type"));
    auto response{objc::msg_send<id>(
        objc::msg_send<id>(objc::get_class("NSHTTPURLResponse"),
                           objc::selector("alloc")),
        objc::selector("initWithURL:statusCode:HTTPVersion:headerFields:"), url,
        static_cast<long>(data ? 200 : 404),
        NSString_stringWithUTF8String("HTTP/1.1"), headers)};
    objc::msg_send<void>(task, objc::selector("didReceiveResponse:"), response);
    if (data) {
      objc::msg_send<void>(task, objc::selector("didReceiveData:"), data);
    }
    objc::msg_send<void>(task, objc::selector("didFinish"));
  } catch (...) {
    // Never let an exception cross into Objective-C.
  }
}

} // namespace cocoa_webkit
} // namespace detail

using browser_engine = detail::cocoa_webkit::cocoa_wkwebview_engine;

} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_BACKENDS_COCOA_WEBKIT_HH
