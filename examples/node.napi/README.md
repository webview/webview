# Node N-API multi-thread Webview implementation

This is a minimal (non-trivial) example implementation for running Webview on Node JS in a multi-thread context, thus enabling the use of `webview_dispatch` and `webview_bind` callback functions in javascript.

### It:
- Wraps the Webview SWIG interface enabling the passing of pointers between threads over IPC, and passing native cb function pointers into Webview.
- Compiles the wrapped SWIG interface to a Node native module.
- Provides a Node native module for creating and managing thread-safe cb functions.

See `example.js` in this folder for usage.

## Running the example
- Clone the Webview repository.
- Install the [prerequisites](https://github.com/webview/webview/tree/master?tab=readme-ov-file#prerequisites) for your OS.

- Note: for linux, use GTK 4.

```Bash
cd webview/examples/node.napi
npm i
node example.js
```

## Development
You will need Swig version >= 4.3 for building the `-javascript -napi` Webview wrapper. Earlier Swig versions do not support [N-Api](https://github.com/nodejs/node-addon-api/tree/main).

```bash
make clean
make
make build
```
