# Windows Window Types Design

## Goal

Extend webview with explicit popup, transparent, and control creation modes on
Windows while preserving the behavior and ABI of the existing creation API.
The transparent mode must render HTML with an alpha-transparent WebView2
background and provide native window movement and resizing for a borderless
window.

Linux and macOS support for transparent windows is deferred. The public API is
designed so those backends can implement the same mode later.

## Public API

Add the following C declarations:

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

WEBVIEW_API webview_t webview_create_with_options(
    const webview_create_options_t *options);
```

The `size` member provides room for ABI-compatible extension. Callers set it to
`sizeof(webview_create_options_t)`.

Add a C++ `webview::window_options` value type and an options-based backend
constructor. C and C++ expose equivalent behavior.

The existing API remains source- and behavior-compatible:

- `webview_create(debug, NULL)` maps to `WEBVIEW_WINDOW_TYPE_POPUP`.
- `webview_create(debug, window)` maps to `WEBVIEW_WINDOW_TYPE_CONTROL`.
- Existing constructors retain the same mapping.

## Window Modes

### Popup

Popup creates and owns a standard top-level window. It preserves the existing
`WS_OVERLAPPEDWINDOW` implementation. An optional `window` in the new options
structure is treated as an owner HWND, not as an embedding parent.

### Control

Control embeds the WebView2 widget in a caller-owned parent HWND using
`WS_CHILD`. The parent HWND, application lifetime, and message loop remain the
caller's responsibility. A valid parent HWND is required.

### Transparent

Transparent creates and owns a borderless top-level window. An optional
`window` is treated as an owner HWND. The host uses these capabilities:

```text
WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX |
WS_MAXIMIZEBOX | WS_SYSMENU
```

The visible non-client frame is removed through non-client handling while the
window retains native resize, minimize, maximize, restore, and snap behavior.
The implementation is DPI-aware.

After WebView2 controller creation, the backend obtains
`ICoreWebView2Controller2` and sets `DefaultBackgroundColor` to
`COREWEBVIEW2_COLOR{0, 0, 0, 0}`. If the required controller interface or
transparent background operation is unavailable, creation fails rather than
showing an opaque fallback.

The library does not inject visual CSS. Authors opt into page transparency in
their own HTML, for example:

```css
html, body {
  background: transparent;
}
```

Transparent pixels remain part of the window's input area. Cross-process mouse
click-through is not part of this implementation.

## HTML Window Interaction

Transparent windows support declarative drag regions:

```html
<header data-webview-drag>
  Draggable title area
  <button data-webview-no-drag>Close</button>
</header>
```

An internal document-start script observes pointer input without changing page
layout or styles. It sends reserved internal messages to the Windows backend.
Reserved messages are handled before normal user bindings and are not exposed
as application binding calls.

Behavior:

- Primary-button presses inside `data-webview-drag` begin native window move.
- A nearest `data-webview-no-drag` ancestor suppresses dragging.
- Standard interactive elements such as buttons, links, inputs, selects, text
  areas, and editable content suppress dragging unless explicitly overridden by
  a future API.
- Double-clicking a drag region toggles maximize and restore.
- Pointer presses within a DPI-scaled edge/corner threshold begin the matching
  native resize operation.
- Native movement and resizing use Win32 non-client operations so Windows keeps
  control of snap, multi-monitor transitions, and DPI changes.

The internal message parser and edge-direction calculation are isolated from
the WebView2 callback so they can be unit tested without moving real windows.

## Validation and Errors

Creation fails for these inputs:

- A null options pointer.
- An options `size` smaller than the documented structure size.
- An unknown window type.
- Control mode without a valid parent HWND.
- Popup or transparent mode with a non-null invalid owner HWND.
- Transparent mode when the required WebView2 controller capability fails.

On non-Windows backends, popup and control keep their existing behavior.
Transparent mode fails explicitly and is not silently converted to popup.

## Code Organization

- `core/include/webview/types.h`: C enum and options structure.
- `core/include/webview/api.h`: C creation function and documentation.
- `core/include/webview/types.hh`: C++ options value type.
- `core/include/webview/c_api_impl.hh`: C validation, compatibility mapping,
  and backend construction.
- `core/include/webview/detail/engine_base.hh`: shared window mode and ownership
  state.
- `core/include/webview/detail/backends/win32_edge.hh`: Win32 creation paths,
  WebView2 alpha setup, and native interaction handling.
- GTK and Cocoa backend headers: options constructor plumbing and explicit
  transparent-mode rejection.
- `examples/transparent.cc`: Windows transparent-window example.
- README and API comments: usage and platform limitations.

No unrelated backend refactoring is included.

## Test Strategy

Implementation follows red-green-refactor cycles.

1. Add failing unit tests for options validation, enum validation, compatibility
   mapping, edge/corner classification, and reserved interaction messages.
2. Add the minimum shared API and model code needed to pass those tests.
3. Add failing Windows functional tests for popup/control/transparent styles,
   parent-versus-owner relationships, ownership, and destruction behavior.
4. Implement the three Win32 creation paths and make those tests pass.
5. Add a failing Windows test that reads the WebView2 controller background
   color and expects alpha zero, then implement transparent controller setup.
6. Add the interaction script and native move/resize dispatch after its parser
   and classification tests are in place.
7. Run all unit and functional tests, build the examples, and regenerate/build
   the amalgamated header.
8. Manually verify the transparent example for alpha rendering, controls,
   dragging, double-click maximize/restore, all resize edges/corners, snapping,
   and DPI changes.

## Acceptance Criteria

- Existing C and C++ creation calls behave exactly as before.
- The new API creates all three Windows modes with correct owner/parent and
  lifetime semantics.
- Transparent HTML renders without an opaque WebView2 background.
- Declared drag regions move the native window without stealing input from
  excluded or standard interactive elements.
- The borderless window resizes from every edge and corner and retains Windows
  snap/maximize behavior.
- Unsupported transparent mode fails clearly on non-Windows backends.
- Tests, examples, and amalgamation builds pass.
