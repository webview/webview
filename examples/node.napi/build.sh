#! /bin/bash

rm -rf build
rm webview.napi_wrap.cxx
rm webview.napi.node
rm jscallback.node
rm JsCallback.js
rm compile_commands.json

swig -c++ -javascript -napi webview.napi.i
npm i
npx node-gyp configure -- -f gyp.generator.compile_commands_json.py >/dev/null 2>&1

mv build/Release/compile_commands.json compile_commands.json
mv build/Release/webview.napi.node ./webview.napi.node
mv build/Release/jscallback.node jscallback.node

bun build ./JsCallback.ts --outdir . --target "node" --external "*"
