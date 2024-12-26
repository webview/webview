# Webview Swig build for Node NAPI

```js
swig -c++ -javascript -napi webview_napi.i // Builds the swig c++ wrapper
npm i // Installs Node NAPI dependencies and the webview library using node-gyp
```

# Webview Swig usage for Node NAPI

```js
const lib = require("./webview.napi.node");
import { JSCallback } from "swigUtils.js";

const w = lib.webview_create(1, null);
lib.webview_set_html("<html>Hello Webview</html")

```
