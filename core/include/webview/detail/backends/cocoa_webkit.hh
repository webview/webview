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
#include "../platform/darwin/cocoa.hh"
#include "../platform/darwin/objc.hh"
#include "../platform/darwin/webkit.hh"
#include "../user_script.hh"

#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <string>

#include <CoreGraphics/CoreGraphics.h>
#include <objc/NSObjCRuntime.h>
#include <objc/objc-runtime.h>

namespace webview {
namespace detail {

using namespace objc::literals;

class user_script::impl {
public:
  impl(id script) : m_script{script} {
    objc::msg_send<void>(script, "retain"_sel);
  }

  ~impl() { objc::msg_send<void>(m_script, "release"_sel); }

  impl(const impl &) = delete;
  impl &operator=(const impl &) = delete;
  impl(impl &&) = delete;
  impl &operator=(impl &&) = delete;

  id get_native() const { return m_script; }

private:
  id m_script{};
};

class cocoa_wkwebview_engine : public engine_base {
public:
  cocoa_wkwebview_engine(bool debug, void *window)
      : m_app{get_shared_application()} {
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
        if (auto ui_delegate =
                objc::msg_send<id>(m_webview, "UIDelegate"_sel)) {
          objc::msg_send<void>(m_webview, "setUIDelegate:"_sel, nullptr);
          objc::msg_send<void>(ui_delegate, "release"_sel);
        }
        objc::msg_send<void>(m_webview, "release"_sel);
        m_webview = nullptr;
      }
      if (m_widget) {
        if (m_widget == cocoa::NSWindow_get_content_view(m_window)) {
          cocoa::NSWindow_set_content_view(m_window, nullptr);
        }
        objc::msg_send<void>(m_widget, "release"_sel);
        m_widget = nullptr;
      }
      if (owns_window()) {
        // Replace delegate to avoid callbacks and other bad things during
        // destruction.
        objc::msg_send<void>(m_window, "setDelegate:"_sel, nullptr);
        cocoa::NSWindow_close(m_window);
        on_window_destroyed(true);
      }
      m_window = nullptr;
    }
    if (m_window_delegate) {
      objc::msg_send<void>(m_window_delegate, "release"_sel);
      m_window_delegate = nullptr;
    }
    if (m_app_delegate) {
      objc::msg_send<void>(m_app, "setDelegate:"_sel, nullptr);
      // Make sure to release the delegate we created.
      objc::msg_send<void>(m_app_delegate, "release"_sel);
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
    objc::msg_send<void>(m_app, "run"_sel);
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
    cocoa::NSWindow_set_title(m_window, title);
    return {};
  }
  noresult set_size_impl(int width, int height, webview_hint_t hints) override {
    using namespace cocoa;
    objc::autoreleasepool arp;

    auto style = static_cast<NSWindowStyleMask>(
        NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable);
    if (hints != WEBVIEW_HINT_FIXED) {
      style =
          static_cast<NSWindowStyleMask>(style | NSWindowStyleMaskResizable);
    }
    NSWindow_set_style_mask(m_window, style);

    if (hints == WEBVIEW_HINT_MIN) {
      objc::msg_send<void>(m_window, "setContentMinSize:"_sel,
                           CGSizeMake(width, height));
    } else if (hints == WEBVIEW_HINT_MAX) {
      objc::msg_send<void>(m_window, "setContentMaxSize:"_sel,
                           CGSizeMake(width, height));
    } else {
      auto rect{NSWindow_get_frame(m_window)};
      NSWindow_set_frame(
          m_window, NSRectMake(rect.origin.x, rect.origin.y, width, height),
          true, false);
    }
    objc::msg_send<void>(m_window, "center"_sel);

    return window_show();
  }
  noresult navigate_impl(const std::string &url) override {
    objc::autoreleasepool arp;

    auto nsurl = objc::msg_send<id>(
        "NSURL"_cls, "URLWithString:"_sel,
        objc::msg_send<id>("NSString"_cls, "stringWithUTF8String:"_sel,
                           url.c_str()));

    objc::msg_send<void>(
        m_webview, "loadRequest:"_sel,
        objc::msg_send<id>("NSURLRequest"_cls, "requestWithURL:"_sel, nsurl));

    return {};
  }
  noresult set_html_impl(const std::string &html) override {
    objc::autoreleasepool arp;
    objc::msg_send<void>(m_webview, "loadHTMLString:baseURL:"_sel,
                         objc::msg_send<id>("NSString"_cls,
                                            "stringWithUTF8String:"_sel,
                                            html.c_str()),
                         nullptr);
    return {};
  }
  noresult eval_impl(const std::string &js) override {
    objc::autoreleasepool arp;
    // URI is null before content has begun loading.
    auto nsurl = objc::msg_send<id>(m_webview, "URL"_sel);
    if (!nsurl) {
      return {};
    }
    objc::msg_send<void>(m_webview, "evaluateJavaScript:completionHandler:"_sel,
                         objc::msg_send<id>("NSString"_cls,
                                            "stringWithUTF8String:"_sel,
                                            js.c_str()),
                         nullptr);
    return {};
  }

  user_script add_user_script_impl(const std::string &js) override {
    objc::autoreleasepool arp;
    auto wk_script = objc::msg_send<id>(
        objc::msg_send<id>("WKUserScript"_cls, "alloc"_sel),
        "initWithSource:injectionTime:forMainFrameOnly:"_sel,
        objc::msg_send<id>("NSString"_cls, "stringWithUTF8String:"_sel,
                           js.c_str()),
        WKUserScriptInjectionTimeAtDocumentStart, YES);
    // Script is retained when added.
    objc::msg_send<void>(m_manager, "addUserScript:"_sel, wk_script);
    user_script script{
        js, user_script::impl_ptr{new user_script::impl{wk_script},
                                  [](user_script::impl *p) { delete p; }}};
    objc::msg_send<void>(wk_script, "release"_sel);
    return script;
  }

  void remove_all_user_scripts_impl(
      const std::list<user_script> & /*scripts*/) override {
    objc::autoreleasepool arp;
    // Removing scripts decreases the retain count of each script.
    objc::msg_send<id>(m_manager, "removeAllUserScripts"_sel);
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
      cls = objc_allocateClassPair((Class) "NSResponder"_cls, class_name, 0);
      class_addProtocol(cls, objc_getProtocol("NSTouchBarProvider"));
      class_addMethod(cls,
                      "applicationShouldTerminateAfterLastWindowClosed:"_sel,
                      (IMP)(+[](id, SEL, id) -> BOOL { return NO; }), "c@:@");
      class_addMethod(cls, "applicationDidFinishLaunching:"_sel,
                      (IMP)(+[](id self, SEL, id notification) {
                        auto app =
                            objc::msg_send<id>(notification, "object"_sel);
                        auto w = get_associated_webview(self);
                        w->on_application_did_finish_launching(self, app);
                      }),
                      "v@:@");
      objc_registerClassPair(cls);
    }
    return objc::msg_send<id>((id)cls, "new"_sel);
  }
  id create_script_message_handler() {
    objc::autoreleasepool arp;
    constexpr auto class_name = "WebviewWKScriptMessageHandler";
    // Avoid crash due to registering same class twice
    auto cls = objc_lookUpClass(class_name);
    if (!cls) {
      cls = objc_allocateClassPair((Class) "NSResponder"_cls, class_name, 0);
      class_addProtocol(cls, objc_getProtocol("WKScriptMessageHandler"));
      class_addMethod(
          cls, "userContentController:didReceiveScriptMessage:"_sel,
          (IMP)(+[](id self, SEL, id, id msg) {
            auto w = get_associated_webview(self);
            w->on_message(objc::msg_send<const char *>(
                objc::msg_send<id>(msg, "body"_sel), "UTF8String"_sel));
          }),
          "v@:@@");
      objc_registerClassPair(cls);
    }
    auto instance = objc::msg_send<id>((id)cls, "new"_sel);
    objc_setAssociatedObject(instance, "webview", (id)this,
                             OBJC_ASSOCIATION_ASSIGN);
    return instance;
  }
  static id create_webkit_ui_delegate() {
    objc::autoreleasepool arp;
    constexpr auto class_name = "WebviewWKUIDelegate";
    // Avoid crash due to registering same class twice
    auto cls = objc_lookUpClass(class_name);
    if (!cls) {
      cls = objc_allocateClassPair((Class) "NSObject"_cls, class_name, 0);
      class_addProtocol(cls, objc_getProtocol("WKUIDelegate"));
      class_addMethod(
          cls,
          "webView:runOpenPanelWithParameters:initiatedByFrame:completionHandler:"_sel,
          (IMP)(+[](id, SEL, id, id parameters, id, id completion_handler) {
            auto allows_multiple_selection =
                objc::msg_send<BOOL>(parameters, "allowsMultipleSelection"_sel);
            auto allows_directories =
                objc::msg_send<BOOL>(parameters, "allowsDirectories"_sel);

            // Show a panel for selecting files.
            auto panel = objc::msg_send<id>("NSOpenPanel"_cls, "openPanel"_sel);
            objc::msg_send<void>(panel, "setCanChooseFiles:"_sel, YES);
            objc::msg_send<void>(panel, "setCanChooseDirectories:"_sel,
                                 allows_directories);
            objc::msg_send<void>(panel, "setAllowsMultipleSelection:"_sel,
                                 allows_multiple_selection);
            auto modal_response =
                objc::msg_send<NSModalResponse>(panel, "runModal"_sel);

            // Get the URLs for the selected files. If the modal was canceled
            // then we pass null to the completion handler to signify
            // cancellation.
            id urls = modal_response == NSModalResponseOK
                          ? objc::msg_send<id>(panel, "URLs"_sel)
                          : nullptr;

            // Invoke the completion handler block.
            auto sig = objc::msg_send<id>(
                "NSMethodSignature"_cls, "signatureWithObjCTypes:"_sel, "v@?@");
            auto invocation = objc::msg_send<id>(
                "NSInvocation"_cls, "invocationWithMethodSignature:"_sel, sig);
            objc::msg_send<void>(invocation, "setTarget:"_sel,
                                 completion_handler);
            objc::msg_send<void>(invocation, "setArgument:atIndex:"_sel, &urls,
                                 1);
            objc::msg_send<void>(invocation, "invoke"_sel);
          }),
          "v@:@@@@");
      objc_registerClassPair(cls);
    }
    return objc::msg_send<id>((id)cls, "new"_sel);
  }
  static id create_window_delegate() {
    objc::autoreleasepool arp;
    constexpr auto class_name = "WebviewNSWindowDelegate";
    // Avoid crash due to registering same class twice
    auto cls = objc_lookUpClass(class_name);
    if (!cls) {
      cls = objc_allocateClassPair((Class) "NSObject"_cls, class_name, 0);
      class_addProtocol(cls, objc_getProtocol("NSWindowDelegate"));
      class_addMethod(cls, "windowWillClose:"_sel,
                      (IMP)(+[](id self, SEL, id notification) {
                        auto window =
                            objc::msg_send<id>(notification, "object"_sel);
                        auto w = get_associated_webview(self);
                        w->on_window_will_close(self, window);
                      }),
                      "v@:@");
      objc_registerClassPair(cls);
    }
    return objc::msg_send<id>((id)cls, "new"_sel);
  }
  static id get_shared_application() {
    return objc::msg_send<id>("NSApplication"_cls, "sharedApplication"_sel);
  }
  static cocoa_wkwebview_engine *get_associated_webview(id object) {
    auto w =
        (cocoa_wkwebview_engine *)objc_getAssociatedObject(object, "webview");
    assert(w);
    return w;
  }
  static id get_main_bundle() noexcept {
    return objc::msg_send<id>("NSBundle"_cls, "mainBundle"_sel);
  }
  static bool is_app_bundled() noexcept {
    auto bundle = get_main_bundle();
    if (!bundle) {
      return false;
    }
    auto bundle_path = objc::msg_send<id>(bundle, "bundlePath"_sel);
    auto bundled =
        objc::msg_send<BOOL>(bundle_path, "hasSuffix:"_sel, ".app"_str);
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
      objc::msg_send<void>(app, "setActivationPolicy:"_sel,
                           NSApplicationActivationPolicyRegular);
      // Activate the app regardless of other active apps.
      // This can be obtrusive so we only do it when necessary.
      objc::msg_send<void>(app, "activateIgnoringOtherApps:"_sel, YES);
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
  void window_settings(bool debug) {
    objc::autoreleasepool arp;

    auto config = objc::autoreleased(
        objc::msg_send<id>("WKWebViewConfiguration"_cls, "new"_sel));

    m_manager = objc::msg_send<id>(config, "userContentController"_sel);
    m_webview = objc::msg_send<id>("WKWebView"_cls, "alloc"_sel);

    auto preferences = objc::msg_send<id>(config, "preferences"_sel);
    auto yes_value =
        objc::msg_send<id>("NSNumber"_cls, "numberWithBool:"_sel, YES);

    if (debug) {
      // Equivalent Obj-C:
      // [[config preferences] setValue:@YES forKey:@"developerExtrasEnabled"];
      objc::msg_send<id>(preferences, "setValue:forKey:"_sel, yes_value,
                         "developerExtrasEnabled"_str);
    }

    // Equivalent Obj-C:
    // [[config preferences] setValue:@YES forKey:@"fullScreenEnabled"];
    objc::msg_send<id>(preferences, "setValue:forKey:"_sel, yes_value,
                       "fullScreenEnabled"_str);

#if defined(__has_builtin)
#if __has_builtin(__builtin_available)
    if (__builtin_available(macOS 10.13, *)) {
      // Equivalent Obj-C:
      // [[config preferences] setValue:@YES forKey:@"javaScriptCanAccessClipboard"];
      objc::msg_send<id>(preferences, "setValue:forKey:"_sel, yes_value,
                         "javaScriptCanAccessClipboard"_str);

      // Equivalent Obj-C:
      // [[config preferences] setValue:@YES forKey:@"DOMPasteAllowed"];
      objc::msg_send<id>(preferences, "setValue:forKey:"_sel, yes_value,
                         "DOMPasteAllowed"_str);
    }
#else
#error __builtin_available not supported by compiler
#endif
#else
#error __has_builtin not supported by compiler
#endif

    auto ui_delegate = create_webkit_ui_delegate();
    objc::msg_send<void>(m_webview, "initWithFrame:configuration:"_sel,
                         CGRectMake(0, 0, 0, 0), config);
    // Autoresizing mask is needed to prevent the Web Inspector pane from
    // pushing the main web view out of bounds
    auto autoresizing_mask = NSViewWidthSizable | NSViewMaxXMargin |
                             NSViewHeightSizable | NSViewMaxYMargin;
    objc::msg_send<void>(m_webview, "setAutoresizingMask:"_sel,
                         autoresizing_mask);
    objc_setAssociatedObject(ui_delegate, "webview", (id)this,
                             OBJC_ASSOCIATION_ASSIGN);
    objc::msg_send<void>(m_webview, "setUIDelegate:"_sel, ui_delegate);

    if (debug) {
      // Explicitly make WKWebView inspectable via Safari on OS versions that
      // disable the feature by default (macOS 13.3 and later) and support
      // enabling it. According to Apple, the behavior on older OS versions is
      // for content to always be inspectable in "debug builds".
      // Testing shows that this is true for macOS 12.6 but somehow not 10.15.
      // https://webkit.org/blog/13936/enabling-the-inspection-of-web-content-in-apps/
#if defined(__has_builtin)
#if __has_builtin(__builtin_available)
      if (__builtin_available(macOS 13.3, iOS 16.4, tvOS 16.4, *)) {
        objc::msg_send<void>(
            m_webview, "setInspectable:"_sel,
            objc::msg_send<id>("NSNumber"_cls, "numberWithBool:"_sel, YES));
      }
#else
#error __builtin_available not supported by compiler
#endif
#else
#error __has_builtin not supported by compiler
#endif
    }

    auto script_message_handler =
        objc::autoreleased(create_script_message_handler());
    objc::msg_send<void>(m_manager, "addScriptMessageHandler:name:"_sel,
                         script_message_handler, "__webview__"_str);

    add_init_script("function(message) {\n\
  return window.webkit.messageHandlers.__webview__.postMessage(message);\n\
}");
    set_up_widget();
    cocoa::NSWindow_set_content_view(m_window, m_widget);
    if (owns_window()) {
      cocoa::NSWindow_make_key_and_order_front(m_window);
    }
  }
  void set_up_widget() {
    objc::autoreleasepool arp;
    // Create a new view that can contain both the web view and the Web Inspector pane
    m_widget = objc::msg_send<id>("NSView"_cls, "alloc"_sel);
    objc::msg_send<void>(m_widget, "initWithFrame:"_sel,
                         CGRectMake(0, 0, 0, 0));
    // Autoresizing is needed because the Web Inspector pane is a sibling of the web view
    objc::msg_send<void>(m_widget, "setAutoresizesSubviews:"_sel, YES);
    objc::msg_send<void>(m_widget, "addSubview:"_sel, m_webview);
    objc::msg_send<void>(m_webview, "setFrame:"_sel,
                         objc::msg_send_stret<CGRect>(m_widget, "bounds"_sel));
  }
  void stop_run_loop() {
    objc::autoreleasepool arp;
    // Request the run loop to stop. This doesn't immediately stop the loop.
    objc::msg_send<void>(m_app, "stop:"_sel, nullptr);
    // The run loop will stop after processing an NSEvent.
    // Event type: NSEventTypeApplicationDefined (macOS 10.12+),
    //             NSApplicationDefined (macOS 10.0–10.12)
    int type = 15;
    auto event = objc::msg_send<id>(
        "NSEvent"_cls,
        "otherEventWithType:location:modifierFlags:timestamp:windowNumber:context:subtype:data1:data2:"_sel,
        type, CGPointMake(0, 0), 0, 0, 0, nullptr, 0, 0, 0);
    objc::msg_send<void>(m_app, "postEvent:atStart:"_sel, event, YES);
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

    set_owns_window(!window);
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
    objc_setAssociatedObject(m_app_delegate, "webview", (id)this,
                             OBJC_ASSOCIATION_ASSIGN);
    objc::msg_send<void>(m_app, "setDelegate:"_sel, m_app_delegate);

    // Start the main run loop so that the app delegate gets the
    // NSApplicationDidFinishLaunchingNotification notification after the run
    // loop has started in order to perform further initialization.
    // We need to return from this constructor so this run loop is only
    // temporary.
    objc::msg_send<void>(m_app, "run"_sel);
  }
  void window_init_proceed() {
    using namespace cocoa;
    objc::autoreleasepool arp;

    m_window = NSWindow_new(NSRectMake(0, 0, 0, 0), NSWindowStyleMaskTitled,
                            NSBackingStoreBuffered, false);
    m_window_delegate = create_window_delegate();
    objc_setAssociatedObject(m_window_delegate, "webview", (id)this,
                             OBJC_ASSOCIATION_ASSIGN);
    objc::msg_send<void>(m_window, "setDelegate:"_sel, m_window_delegate);
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
    auto mask = NSUIntegerMax; // NSEventMaskAny
    // NSDefaultRunLoopMode
    auto mode = objc::msg_send<id>("NSString"_cls, "stringWithUTF8String:"_sel,
                                   "kCFRunLoopDefaultMode");
    while (fn()) {
      objc::autoreleasepool arp2;
      auto event = objc::msg_send<id>(
          m_app, "nextEventMatchingMask:untilDate:inMode:dequeue:"_sel, mask,
          nullptr, mode, YES);
      if (event) {
        objc::msg_send<void>(m_app, "sendEvent:"_sel, event);
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
};

} // namespace detail

using browser_engine = detail::cocoa_wkwebview_engine;

} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_BACKENDS_COCOA_WEBKIT_HH
