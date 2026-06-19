# Windows Window Types Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add backward-compatible popup, transparent, and control creation modes, with Windows-first transparent rendering and native borderless window interaction.

**Architecture:** Add an extensible options structure to the C API and a matching C++ value type, then normalize old and new calls into one backend constructor. Keep window-mode policy and interaction protocol in small testable headers; let the Win32 backend own only HWND/WebView2 operations. Existing popup/control behavior remains the compatibility baseline, while transparent mode adds alpha background and reserved HTML-to-native window actions.

**Tech Stack:** C++11/C API, Win32, COM, Microsoft WebView2, CMake, webview test driver.

---

## File Structure

- Modify `core/include/webview/types.h`: public C enum and extensible creation options.
- Modify `core/include/webview/types.hh`: public C++ `window_options` type.
- Create `core/include/webview/detail/window_options.hh`: compatibility mapping and validation shared by C API/backends.
- Modify `core/include/webview/api.h`: declare and document `webview_create_with_options`.
- Modify `core/include/webview/c_api_impl.hh`: route old/new C entry points through normalized options.
- Modify `core/include/webview/detail/engine_base.hh`: retain normalized type, parent/owner, and ownership semantics.
- Modify all three backend headers: delegate old constructors to the options constructor; reject transparent mode outside Windows.
- Create `core/include/webview/detail/window_interaction.hh`: parse reserved actions and classify resize edges without HWND dependencies.
- Modify `core/include/webview/detail/backends/win32_edge.hh`: HWND styles, owner/parent behavior, WebView2 alpha, and native actions.
- Modify `core/tests/src/unit_tests.cc`: pure options and interaction tests.
- Modify `core/tests/src/functional_tests.cc`: Windows HWND/WebView2 integration tests.
- Create `examples/transparent.cc` and modify `examples/CMakeLists.txt`: manual Windows example.
- Modify `README.md`: public usage and platform support.

### Task 1: Public Window Options and Compatibility Mapping

**Files:**
- Modify: `core/include/webview/types.h`
- Modify: `core/include/webview/types.hh`
- Create: `core/include/webview/detail/window_options.hh`
- Test: `core/tests/src/unit_tests.cc`

- [ ] **Step 1: Write failing tests for old-call mapping and options validation**

Append tests that express the normalized model before creating the helper:

```cpp
TEST_CASE("Window options preserve legacy creation semantics") {
  using namespace webview::detail;
  auto popup = legacy_window_options(nullptr);
  REQUIRE(popup.type == WEBVIEW_WINDOW_TYPE_POPUP);
  REQUIRE(popup.window == nullptr);

  auto parent = reinterpret_cast<void *>(static_cast<uintptr_t>(1));
  auto control = legacy_window_options(parent);
  REQUIRE(control.type == WEBVIEW_WINDOW_TYPE_CONTROL);
  REQUIRE(control.window == parent);
}

TEST_CASE("C creation options are validated before backend creation") {
  using namespace webview::detail;
  webview_create_options_t options{};
  REQUIRE(!validate_create_options(nullptr).ok());
  REQUIRE(!validate_create_options(&options).ok());

  options.size = sizeof(options);
  options.window_type = WEBVIEW_WINDOW_TYPE_POPUP;
  REQUIRE(validate_create_options(&options).ok());

  options.window_type = static_cast<webview_window_type_t>(99);
  REQUIRE(!validate_create_options(&options).ok());
}
```

- [ ] **Step 2: Run the unit target and verify RED**

Run:

```powershell
cmake -S . -B build -DWEBVIEW_BUILD_TESTS=ON -DWEBVIEW_BUILD_EXAMPLES=ON
cmake --build build --config Debug --target webview_core_unit_tests
```

Expected: compilation fails because `webview_create_options_t`,
`legacy_window_options`, and `validate_create_options` do not exist.

- [ ] **Step 3: Add the public enum, C structure, C++ type, and helper**

Add to `types.h` (including `<stddef.h>` for `size_t`):

```c
typedef enum {
  WEBVIEW_WINDOW_TYPE_POPUP = 0,
  WEBVIEW_WINDOW_TYPE_TRANSPARENT = 1,
  WEBVIEW_WINDOW_TYPE_CONTROL = 2
} webview_window_type_t;

typedef struct {
  size_t size;
  int debug;
  webview_window_type_t window_type;
  void *window;
} webview_create_options_t;
```

Add to `types.hh`:

```cpp
struct window_options {
  webview_window_type_t type{WEBVIEW_WINDOW_TYPE_POPUP};
  void *window{};
};
```

Create `window_options.hh` with these complete operations:

```cpp
inline window_options legacy_window_options(void *window) {
  return {window ? WEBVIEW_WINDOW_TYPE_CONTROL : WEBVIEW_WINDOW_TYPE_POPUP,
          window};
}

inline noresult validate_create_options(
    const webview_create_options_t *options) {
  if (!options || options->size < sizeof(webview_create_options_t)) {
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT,
                      "Invalid webview creation options"};
  }
  switch (options->window_type) {
  case WEBVIEW_WINDOW_TYPE_POPUP:
  case WEBVIEW_WINDOW_TYPE_TRANSPARENT:
    return {};
  case WEBVIEW_WINDOW_TYPE_CONTROL:
    if (options->window) {
      return {};
    }
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT,
                      "Control mode requires a parent window"};
  default:
    return error_info{WEBVIEW_ERROR_INVALID_ARGUMENT,
                      "Unknown webview window type"};
  }
}

inline window_options to_window_options(
    const webview_create_options_t &options) {
  return {options.window_type, options.window};
}
```

- [ ] **Step 4: Rebuild and verify GREEN**

Run:

```powershell
cmake --build build --config Debug --target webview_core_unit_tests
ctest --test-dir build -C Debug -R webview_core_unit_tests --output-on-failure
```

Expected: unit target builds and all discovered unit tests pass.

- [ ] **Step 5: Commit the public model**

```powershell
git add core/include/webview/types.h core/include/webview/types.hh core/include/webview/detail/window_options.hh core/tests/src/unit_tests.cc
git commit -m "feat: add window creation option types"
```

### Task 2: New C/C++ Creation Path and Backend Plumbing

**Files:**
- Modify: `core/include/webview/api.h`
- Modify: `core/include/webview/c_api_impl.hh`
- Modify: `core/include/webview/detail/engine_base.hh`
- Modify: `core/include/webview/detail/backends/win32_edge.hh`
- Modify: `core/include/webview/detail/backends/gtk_webkitgtk.hh`
- Modify: `core/include/webview/detail/backends/cocoa_webkit.hh`
- Test: `core/tests/src/functional_tests.cc`

- [ ] **Step 1: Write failing C API compatibility tests**

Add tests that create through the new API and preserve the old one:

```cpp
TEST_CASE("Create popup through options C API") {
  webview_create_options_t options{sizeof(options), 0,
                                   WEBVIEW_WINDOW_TYPE_POPUP, nullptr};
  auto w = webview_create_with_options(&options);
  REQUIRE(w != nullptr);
  webview_dispatch(w, cb_terminate, nullptr);
  webview_run(w);
  webview_destroy(w);
}

TEST_CASE("Reject invalid options through C API") {
  REQUIRE(webview_create_with_options(nullptr) == nullptr);
  webview_create_options_t options{};
  REQUIRE(webview_create_with_options(&options) == nullptr);
}
```

- [ ] **Step 2: Build and verify RED**

```powershell
cmake --build build --config Debug --target webview_core_functional_tests
```

Expected: compilation fails because `webview_create_with_options` is undeclared.

- [ ] **Step 3: Implement the new entry point and normalized constructors**

Declare in `api.h`:

```c
WEBVIEW_API webview_t webview_create_with_options(
    const webview_create_options_t *options);
```

Implement in `c_api_impl.hh`, and make the old function delegate:

```cpp
WEBVIEW_API webview_t webview_create_with_options(
    const webview_create_options_t *options) {
  using namespace webview::detail;
  webview::webview *w{};
  auto err = api_filter(
      [&]() -> webview::result<webview::webview *> {
        validate_create_options(options).ensure_ok();
        return new webview::webview{static_cast<bool>(options->debug),
                                    to_window_options(*options)};
      },
      [&](webview::webview *value) { w = value; });
  return err == WEBVIEW_ERROR_OK ? w : nullptr;
}

WEBVIEW_API webview_t webview_create(int debug, void *window) {
  auto legacy = webview::detail::legacy_window_options(window);
  webview_create_options_t options{sizeof(options), debug, legacy.type,
                                   legacy.window};
  return webview_create_with_options(&options);
}
```

Change `engine_base` to receive `window_options`, expose `window_type()` and
`native_window()`, and set ownership from `type != CONTROL`. Add options
constructors to each backend; keep the old constructor as a delegating
constructor using `legacy_window_options(window)`. GTK/Cocoa throw
`WEBVIEW_ERROR_INVALID_ARGUMENT` for transparent mode until implemented.

- [ ] **Step 4: Run compatibility and full tests**

```powershell
cmake --build build --config Debug --target webview_core_functional_tests
ctest --test-dir build -C Debug --output-on-failure
```

Expected: old and new creation tests pass; existing tests remain green.

- [ ] **Step 5: Commit creation plumbing**

```powershell
git add core/include/webview/api.h core/include/webview/c_api_impl.hh core/include/webview/detail/engine_base.hh core/include/webview/detail/backends core/tests/src/functional_tests.cc
git commit -m "feat: add options-based webview creation"
```

### Task 3: Win32 Popup, Owner, Control, and Transparent HWND Modes

**Files:**
- Modify: `core/include/webview/detail/backends/win32_edge.hh`
- Test: `core/tests/src/functional_tests.cc`

- [ ] **Step 1: Write failing Win32 HWND relationship/style tests**

Under `#ifdef WEBVIEW_PLATFORM_WINDOWS`, add a helper-created owner HWND and
assert the three contracts:

```cpp
TEST_CASE("Windows window modes use correct HWND relationships and styles") {
  auto module = GetModuleHandleW(nullptr);
  auto owner = CreateWindowExW(0, L"STATIC", L"owner", WS_OVERLAPPEDWINDOW,
                               0, 0, 100, 100, nullptr, nullptr, module,
                               nullptr);
  REQUIRE(owner != nullptr);

  {
    webview::webview popup(false, {WEBVIEW_WINDOW_TYPE_POPUP, owner});
    auto popup_hwnd = static_cast<HWND>(popup.window().value());
    REQUIRE(GetWindow(popup_hwnd, GW_OWNER) == owner);
    REQUIRE((GetWindowLongPtrW(popup_hwnd, GWL_STYLE) & WS_CHILD) == 0);
  }
  {
    webview::webview control(false, {WEBVIEW_WINDOW_TYPE_CONTROL, owner});
    auto control_widget = static_cast<HWND>(control.widget().value());
    REQUIRE(GetParent(control_widget) == owner);
  }
  {
    webview::webview transparent(false,
                                 {WEBVIEW_WINDOW_TYPE_TRANSPARENT, owner});
    auto transparent_hwnd = static_cast<HWND>(transparent.window().value());
    auto style = GetWindowLongPtrW(transparent_hwnd, GWL_STYLE);
    REQUIRE(GetWindow(transparent_hwnd, GW_OWNER) == owner);
    REQUIRE((style & WS_POPUP) != 0);
    REQUIRE((style & WS_THICKFRAME) != 0);
  }

  DestroyWindow(owner);
}
```

- [ ] **Step 2: Run the Windows functional target and verify RED**

```powershell
cmake --build build --config Debug --target webview_core_functional_tests
ctest --test-dir build -C Debug -R "Windows window modes" --output-on-failure
```

Expected: relationship/style assertions fail because options are not yet used
to choose HWND creation behavior.

- [ ] **Step 3: Split Win32 window initialization by mode**

Implement `window_init(const window_options &options)` with these branches:

```cpp
switch (options.type) {
case WEBVIEW_WINDOW_TYPE_POPUP:
  create_owned_window(WS_OVERLAPPEDWINDOW, static_cast<HWND>(options.window));
  break;
case WEBVIEW_WINDOW_TYPE_TRANSPARENT:
  create_owned_window(WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX |
                          WS_MAXIMIZEBOX | WS_SYSMENU,
                      static_cast<HWND>(options.window));
  break;
case WEBVIEW_WINDOW_TYPE_CONTROL:
  if (!IsWindow(static_cast<HWND>(options.window))) {
    throw exception{WEBVIEW_ERROR_INVALID_ARGUMENT,
                    "Control mode requires a valid parent HWND"};
  }
  m_window = static_cast<HWND>(options.window);
  break;
}
```

`create_owned_window` passes the owner HWND as `hWndParent` to
`CreateWindowW`. Validate non-null owner handles with `IsWindow`. Add
transparent-only `WM_NCCALCSIZE` handling that returns `0` for `wParam == TRUE`
and uses `DefWindowProcW` while maximized so Windows does not cover the work
area. Preserve existing DPI, close, min/max-size, activation, and destruction
handlers.

- [ ] **Step 4: Re-run functional tests and verify GREEN**

```powershell
cmake --build build --config Debug --target webview_core_functional_tests
ctest --test-dir build -C Debug -R "Windows window modes" --output-on-failure
```

Expected: all three relationship/style tests pass.

- [ ] **Step 5: Commit HWND mode behavior**

```powershell
git add core/include/webview/detail/backends/win32_edge.hh core/tests/src/functional_tests.cc
git commit -m "feat: implement Win32 window creation modes"
```

### Task 4: Transparent WebView2 Background

**Files:**
- Modify: `core/include/webview/detail/backends/win32_edge.hh`
- Test: `core/tests/src/functional_tests.cc`

- [ ] **Step 1: Write a failing alpha-background integration test**

```cpp
TEST_CASE("Transparent mode configures an alpha-zero WebView2 background") {
  webview::webview w(false, {WEBVIEW_WINDOW_TYPE_TRANSPARENT, nullptr});
  auto controller = static_cast<ICoreWebView2Controller *>(
      w.browser_controller().value());
  ICoreWebView2Controller2 *controller2{};
  REQUIRE(SUCCEEDED(controller->QueryInterface(IID_PPV_ARGS(&controller2))));
  COREWEBVIEW2_COLOR color{};
  REQUIRE(SUCCEEDED(controller2->get_DefaultBackgroundColor(&color)));
  REQUIRE(color.A == 0);
  controller2->Release();
}
```

- [ ] **Step 2: Run the test and verify RED**

```powershell
cmake --build build --config Debug --target webview_core_functional_tests
ctest --test-dir build -C Debug -R "alpha-zero" --output-on-failure
```

Expected: test fails because the default background alpha is opaque.

- [ ] **Step 3: Set alpha zero during transparent controller setup**

Immediately after controller creation and before showing the widget:

```cpp
if (window_type() == WEBVIEW_WINDOW_TYPE_TRANSPARENT) {
  ICoreWebView2Controller2 *controller2{};
  auto hr = m_controller->QueryInterface(IID_PPV_ARGS(&controller2));
  if (FAILED(hr) || !controller2) {
    return error_info{WEBVIEW_ERROR_INVALID_STATE,
                      "WebView2 transparent background is unavailable"};
  }
  COREWEBVIEW2_COLOR color{0, 0, 0, 0};
  hr = controller2->put_DefaultBackgroundColor(color);
  controller2->Release();
  if (FAILED(hr)) {
    return error_info{WEBVIEW_ERROR_INVALID_STATE,
                      "Failed to set transparent WebView2 background"};
  }
}
```

- [ ] **Step 4: Re-run alpha and full functional tests**

```powershell
cmake --build build --config Debug --target webview_core_functional_tests
ctest --test-dir build -C Debug --output-on-failure
```

Expected: alpha test and all existing functional tests pass.

- [ ] **Step 5: Commit transparent rendering**

```powershell
git add core/include/webview/detail/backends/win32_edge.hh core/tests/src/functional_tests.cc
git commit -m "feat: render transparent WebView2 backgrounds"
```

### Task 5: Testable HTML Window Interaction Protocol

**Files:**
- Create: `core/include/webview/detail/window_interaction.hh`
- Modify: `core/include/webview/detail/backends/win32_edge.hh`
- Test: `core/tests/src/unit_tests.cc`

- [ ] **Step 1: Write failing parser tests**

```cpp
TEST_CASE("Reserved window interaction messages parse exact actions") {
  using namespace webview::detail;
  REQUIRE(parse_window_action("__webview_window__:move") ==
          window_action::move);
  REQUIRE(parse_window_action("__webview_window__:toggle-maximize") ==
          window_action::toggle_maximize);
  REQUIRE(parse_window_action("__webview_window__:resize:nw") ==
          window_action::resize_nw);
  REQUIRE(parse_window_action("user-message") == window_action::none);
#ifdef WEBVIEW_PLATFORM_WINDOWS
  REQUIRE(window_action_hit_test(window_action::resize_n) == HTTOP);
  REQUIRE(window_action_hit_test(window_action::resize_ne) == HTTOPRIGHT);
  REQUIRE(window_action_hit_test(window_action::resize_e) == HTRIGHT);
  REQUIRE(window_action_hit_test(window_action::resize_se) == HTBOTTOMRIGHT);
  REQUIRE(window_action_hit_test(window_action::resize_s) == HTBOTTOM);
  REQUIRE(window_action_hit_test(window_action::resize_sw) == HTBOTTOMLEFT);
  REQUIRE(window_action_hit_test(window_action::resize_w) == HTLEFT);
  REQUIRE(window_action_hit_test(window_action::resize_nw) == HTTOPLEFT);
#endif
}
```

- [ ] **Step 2: Run unit tests and verify RED**

```powershell
cmake --build build --config Debug --target webview_core_unit_tests
```

Expected: compilation fails because `window_action` and
`parse_window_action` do not exist.

- [ ] **Step 3: Implement the exact action enum/parser**

Create `window_interaction.hh` with `none`, `move`, `toggle_maximize`, and all
eight resize directions. Parse only exact strings prefixed by
`__webview_window__:`; use a static pair table and return `none` for every
unknown string.

The complete native mapping is:

```cpp
inline int window_action_hit_test(window_action action) {
  switch (action) {
  case window_action::move: return HTCAPTION;
  case window_action::resize_n: return HTTOP;
  case window_action::resize_ne: return HTTOPRIGHT;
  case window_action::resize_e: return HTRIGHT;
  case window_action::resize_se: return HTBOTTOMRIGHT;
  case window_action::resize_s: return HTBOTTOM;
  case window_action::resize_sw: return HTBOTTOMLEFT;
  case window_action::resize_w: return HTLEFT;
  case window_action::resize_nw: return HTTOPLEFT;
  default: return HTNOWHERE;
  }
}
```

Guard the Win32 mapping with `WEBVIEW_PLATFORM_WINDOWS`; keep parsing portable.

- [ ] **Step 4: Verify parser tests pass**

```powershell
cmake --build build --config Debug --target webview_core_unit_tests
ctest --test-dir build -C Debug -R webview_core_unit_tests --output-on-failure
```

Expected: all parser and existing unit tests pass.

- [ ] **Step 5: Add the document-start interaction script and native dispatch**

For transparent mode only, add an internal script that:

```js
(() => {
  const send = action =>
    window.chrome.webview.postMessage('__webview_window__:' + action);
  const interactive =
    'a,button,input,select,textarea,[contenteditable="true"],'
    + '[data-webview-no-drag]';
  const edge = (event) => {
    const t = 8;
    const n = event.clientY < t, s = event.clientY >= innerHeight - t;
    const w = event.clientX < t, e = event.clientX >= innerWidth - t;
    return n ? (w ? 'nw' : e ? 'ne' : 'n')
      : s ? (w ? 'sw' : e ? 'se' : 's')
      : w ? 'w' : e ? 'e' : '';
  };
  addEventListener('pointerdown', event => {
    if (event.button !== 0) return;
    const direction = edge(event);
    if (direction) { event.preventDefault(); send('resize:' + direction); return; }
    const drag = event.target.closest('[data-webview-drag]');
    if (drag && !event.target.closest(interactive)) {
      event.preventDefault(); send('move');
    }
  }, true);
  addEventListener('dblclick', event => {
    if (event.target.closest('[data-webview-drag]') &&
        !event.target.closest(interactive)) send('toggle-maximize');
  }, true);
})();
```

Route the WebView2 callback through `on_web_message`. If the parser returns an
action in transparent mode, consume it. For move/resize call `ReleaseCapture()`
then `SendMessageW(m_window, WM_NCLBUTTONDOWN, hit_test, 0)`. For toggle, call
`ShowWindow(m_window, IsZoomed(m_window) ? SW_RESTORE : SW_MAXIMIZE)`. Forward
all non-reserved messages unchanged to `engine_base::on_message`.

- [ ] **Step 6: Run unit and functional regression tests**

```powershell
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Expected: all tests pass and ordinary JS binding messages still work.

- [ ] **Step 7: Commit interaction behavior**

```powershell
git add core/include/webview/detail/window_interaction.hh core/include/webview/detail/backends/win32_edge.hh core/tests/src/unit_tests.cc
git commit -m "feat: add transparent window drag and resize actions"
```

### Task 6: Transparent Example and User Documentation

**Files:**
- Create: `examples/transparent.cc`
- Modify: `examples/CMakeLists.txt`
- Modify: `README.md`

- [ ] **Step 1: Add a Windows-only transparent example target**

Create `transparent.cc` using the options constructor, transparent body CSS,
a `data-webview-drag` header, and `data-webview-no-drag` buttons. The close
button binds to a native `close` callback that calls `terminate()`.

Add the target only under `CMAKE_SYSTEM_NAME STREQUAL "Windows"`:

```cmake
add_executable(webview_example_transparent WIN32)
target_sources(webview_example_transparent PRIVATE transparent.cc ${SHARED_SOURCES})
target_link_libraries(webview_example_transparent PRIVATE webview::core)
```

- [ ] **Step 2: Document C and C++ creation calls**

Add a README section containing these exact minimal calls:

```c
webview_create_options_t options = {
    sizeof(options), 0, WEBVIEW_WINDOW_TYPE_TRANSPARENT, NULL};
webview_t w = webview_create_with_options(&options);
```

```cpp
webview::webview w(false,
    {WEBVIEW_WINDOW_TYPE_TRANSPARENT, nullptr});
```

Document popup owner versus control parent semantics, Windows-only transparent
support, required transparent page CSS, drag/no-drag attributes, 8 CSS-pixel
resize threshold, and the fact that transparent pixels do not click through.

- [ ] **Step 3: Build the example**

```powershell
cmake --build build --config Debug --target webview_example_transparent
```

Expected: the example links successfully.

- [ ] **Step 4: Commit docs and example**

```powershell
git add examples/transparent.cc examples/CMakeLists.txt README.md
git commit -m "docs: add transparent window example"
```

### Task 7: Release-Surface and Final Verification

**Files:**
- Generated by build: `build/amalgamation/webview.h`
- Verify: all modified source files

- [ ] **Step 1: Run formatting checks/fixes used by the repository**

```powershell
cmake --build build --config Debug --target webview_amalgamate
git diff --check
```

Expected: amalgamation target succeeds and `git diff --check` reports no
whitespace errors.

- [ ] **Step 2: Build all configured targets**

```powershell
cmake --build build --config Debug
```

Expected: core libraries, tests, examples, and amalgamation build successfully.

- [ ] **Step 3: Run the complete automated test suite**

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

Expected: 100% tests passed, 0 failed.

- [ ] **Step 4: Manually verify transparent interaction**

Run:

```powershell
.\build\bin\Debug\webview_example_transparent.exe
```

Verify: page background is alpha-transparent; HTML controls remain clickable;
the declared header drags; controls inside it do not drag; double-click toggles
maximize/restore; every edge/corner resizes; Windows snap works; moving between
different-DPI monitors keeps the resize threshold usable.

- [ ] **Step 5: Inspect final scope**

```powershell
git status --short
git diff HEAD~4 --stat
git log -5 --oneline
```

Expected: only the planned API, backend, tests, example, README, design, and plan
files are present; no build artifacts are tracked.
