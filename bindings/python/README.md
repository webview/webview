# webview

Python extension that provides API for the [webview] library.

## Getting started

Install the bindings:

```bash
pip install webview
```

Try the following example:

```python
import webview

w = webview.WebView(width=320, height=240, title="Hello", url="https://google.com", resizable=True, debug=False)
w.run()
```

You may use most of the webview APIs:

```python
# Change window title
w.set_title("New title")
# Make window fullscreen
w.set_fullscreen(True)
# Change initial window background color
w.set_color(255, 0, 0)
# Inject some JS
w.eval("alert('hello')")
# Inject some CSS
w.inject_css('* {background-color: yellow; }')
# Show native OS dialog
file_path = w.dialog(0, 0, "open file", "")
# Post funciton to the UI thread
w.dispatch(some_func)
w.dispatch(lambda: some_func())
# Control run loop
while w.loop(True):
  pass
```

Dispatch is currently only a stub and is implemented as direct function call.
Also, proper Python-to-JS object mapping is not implemented yet, but is highly

## Development

To build and install the library locally:

```bash
python setup.py sync install
```

To upload a new version:

```bash
python setup.py sync sdist
twine upload dist/webview-*.tar.gz
```

To build and install it locally:

```bash
python setup.py sync install
```

Please, ensure that all sources are formatted using `yapf`.


[webview]: https://github.com/zserge/webview
