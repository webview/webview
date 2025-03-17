# Node N-API multi-thread Webview implementation

This is a minimal but non-trivial example implementation for running Webview on Node JS in a multi-thread context, thus enabling the use of `webview_dispatch` and `webview_bind` callback functions in javascript.

**See `example.js` in this folder for usage.**

### It:
- Wraps the Webview SWIG interface enabling the passing of pointers between threads over IPC, and passing native cb function pointers into Webview.
- Compiles the wrapped SWIG interface to a Node native module.
- Provides a Node native module for creating and managing thread-safe cb functions.



## Running the example

- Install the [prerequisites](https://github.com/webview/webview/tree/master?tab=readme-ov-file#prerequisites) for your OS.
- Note for linux: use GTK 4.
- Clone the Webview repository.


```Bash
cd webview/examples/node.napi
git checkout 9a358d9453d288d2560612a6fef2cf2efe67757f # master head at time of writing

npm i
node example.js
```

## Development
- You will need [Swig](https://swig.org/) version >= 4.3 for building the `-javascript -napi` Webview wrapper.<br>Earlier Swig versions do not support [N-Api](https://github.com/nodejs/node-addon-api/tree/main) (node-addon-api).
- C++17 is used for compatibility with N-Api.
- See 'binding.gyp' for the compiler commands.

The cross-platform makefile is for convenience:
```bash
make clean
make
make build
```
