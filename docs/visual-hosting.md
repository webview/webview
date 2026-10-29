# Visual hosting on Windows (WebView2 composition controller)

**Status:** implemented and running in a real application. Mouse, wheel, drag
and cursor are forwarded; touch/pen (`SendPointerInput`) is not yet. This
document is the rationale and the measured evidence behind the design.

## The problem

You cannot show native content — a video frame, a GPU surface, an OpenGL view —
either *above* or *below* a WebView2 created the way `webview` creates it today.
Not with the right z-order, not with a transparent background, not by painting
the host window. All four are closed, and the failures are silent: the OS
reports your window as visible, at the correct rectangle, and you see nothing.

This is the *airspace* problem. It is not specific to this library. Microsoft
hit it with their own WPF control and reached the same conclusion documented
below:

> the WebView2 control will always be the top-most control in the WPF app, and
> any WPF element in the same location will end up below the WebView2 control.
> Solving this issue necessitates moving away from the HwndHost and windowed
> hosting model, and instead use visuals to host the WebView2.

`webview` currently calls `ICoreWebView2Environment::CreateCoreWebView2Controller`
(`core/include/webview/detail/backends/win32_edge.hh`), which is *windowed*
hosting. This document proposes adding *visual* hosting via
`ICoreWebView2Environment3::CreateCoreWebView2CompositionController`, so an
embedder can place its own DirectComposition visuals above or below the web
content.

## Evidence

Measured against a real application (a video editor that needs GPU-composited
video underneath its HTML UI), Windows 11, WebView2 Evergreen. Each experiment
was run to a conclusion before the next was attempted.

### 1. Child HWND above the webview — never visible

A `WS_CHILD` window created as a sibling of the webview widget, `SetWindowPos`
to `HWND_TOP`, sized to the target rectangle.

Enumerating the window tree confirms it is created, visible and correctly
placed — its rectangle matches, to the pixel, what the page reported for the
element it was tracking:

```
hwnd=... class='ThothiumPreviewSurface' visible=True rect=328,269 106x188
hwnd=... class='webview_widget'          visible=True rect=112,135 1082x710
             └ Chrome_WidgetWin_0/1, Chrome_RenderWidgetHostHWND, Intermediate D3D Window
```

It is never seen. Chromium composites through DirectComposition, and that
composition covers sibling child windows regardless of HWND z-order.

**This is the failure to be most careful about**: every observable signal says
success. `IsWindowVisible` is true, the rect is right, `WM_PAINT` runs.

### 2. Top-most owned popup — visible

The same window as `WS_POPUP` with `WS_EX_TOPMOST`, owned by the host window,
positioned in screen coordinates.

**Visible.** This is the control that proves the painting code is correct and
experiment 1 failed by *occlusion*, not by a drawing bug. Worth running before
concluding anything, because it separates two failures that look identical.

Not a solution: a top-most popup floats over menus and modal dialogs, does not
clip to the host window, and fights z-order with other applications.

### 3. Transparent background, native content beneath — no compositing

`ICoreWebView2Controller2::put_DefaultBackgroundColor` with alpha 0 **succeeds**,
and the page genuinely becomes transparent — page-painted backgrounds disappear
and the bare host window shows through.

What appears through the hole is the host window's *unpainted client area*, not
the sibling child window beneath the webview. The documentation says:

> when the DefaultBackgroundColor is transparent, WebView will render hosting app
> content as the background

That is true of visual hosting. With a windowed controller, a sibling child
window underneath is not "hosting app content" and does not appear.

### 4. Painting the host window itself — no compositing

Two attempts:

- **`GetDC(host)` + `FillRect`** — draws nothing. A DC obtained this way has
  child windows clipped out of its visible region, and the webview covers the
  entire client area. The fill is discarded before it reaches a pixel.
- **Subclassing the host window** (`SetWindowLongPtrW`/`GWLP_WNDPROC`) and
  filling during its own `WM_ERASEBKGND`, which is not subject to that clipping.
  The subclass installs and runs. Still nothing appears.

**Conclusion: with a windowed controller, transparent WebView2 does not
composite over host-window content.** Both directions — above and below — are
closed, and no amount of z-order or painting fixes it.

## The design

Add an opt-in visual-hosting path alongside the existing windowed one:

1. Query the environment for `ICoreWebView2Environment3` and call
   `CreateCoreWebView2CompositionController` instead of
   `CreateCoreWebView2Controller`, with a matching completion handler.
2. Create a DirectComposition device and a target bound to the host window,
   then a visual tree. Set the controller's `RootVisualTarget` to the visual the
   web content should render into.
3. The embedder gets access to the tree, so it can insert its own visuals above
   or below the web content, with real alpha.

Windowed hosting stays the default. Visual hosting is opt-in, because it
carries a real cost:

### Pointer input must be forwarded by hand

This is the substantive work, not the rendering. A visual-hosted WebView2
receives no *pointer* input from the system. The host must forward it via
`ICoreWebView2CompositionController`:

- `SendMouseInput` for every move, button, double-click and wheel event,
  including the `mouse_data` for wheel deltas and X-buttons;
- pointer input for touch and pen, via `SendPointerInput`;
- cursor updates — the controller exposes a `Cursor` property and a
  `CursorChanged` event; without handling it the cursor never changes over web
  content.

Getting any of these subtly wrong produces bugs that are hard to attribute: drag
selection that stops at the window edge, or hover states that stick. Anyone
enabling visual hosting is taking that on, which is why it is not the default.

**Keyboard input needs nothing.** There is no `SendKeyboardInput` anywhere in
the WebView2 API — the composition controller exposes only `SendMouseInput` and
`SendPointerInput`. Keyboard follows window focus and reaches the web content
through the normal mechanism. Worth stating plainly, because "visual hosting
means forwarding *all* input by hand" is the natural assumption, and acting on
it would mean building something the platform already handles.

## Why this belongs upstream

Every embedder that needs native content composited with web content hits this,
and the only way through is inside the backend's controller creation — an
embedder cannot reach it from outside. Microsoft's own guidance names visual
hosting as the answer. Adding it here means each project does not have to fork
to discover the four dead ends above.

## Licence

`webview` is MIT. This work is offered under the same terms.
