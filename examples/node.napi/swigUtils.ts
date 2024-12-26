type cTypes = "void*" | "char*";
type jsPointer = number | bigint;

//@ts-ignore
declare module "./webview.napi.node" {
  export class JSCallback {
    constructor(callback: (ptr: bigint) => void);
    getPointer(): bigint;
    free(): void;
  }
}

declare const someOtherNativeLib: {
  functionWithCallbackArg: (
    arg1: string,
    arg2: string,
    callbackPointer: jsPointer
  ) => {
    // does something with callbackFunctionPointer
  };
};

//@ts-expect-error
const libwebview = require("./webview.napi.node");

enum webview_hint {
  WEBVIEW_HINT_NONE = libwebview.WEBVIEW_HINT_NONE,
  WEBVIEW_HINT_MIN = libwebview.WEBVIEW_HINT_MIN,
  WEBVIEW_HINT_MAX = libwebview.WEBVIEW_HINT_MAX,
  WEBVIEW_HINT_FIXED = libwebview.WEBVIEW_HINT_FIXED,
}
/** Create a derived class in Javascript */
class JSCallback extends libwebview.JSCallback {
  /** A thread-safe pointer to the `callback` constructor parameter */
  ptr: jsPointer | null;
  constructor(
    callback: (id: jsPointer) => void
    //cStruct: { returns: "void"; args: cTypes[] } // definitions for the native c/c++ structure of the `callback` function parameter
  ) {
    super(callback);
    this.ptr = super.getPointer();
  }

  /** free the callback from memory */
  close() {
    this.ptr = null;
    super.free(); // frees the memory in the native scope
  }
}

/** Example usage of the derived class in Javascript */
/*
function main() {
  const callback = new JSCallback((id: jsPointer) => {
    console.log(`I was called-back with pointer value ${id}`);
  });
  const callbackPointer = callback.ptr;
  console.log({ callbackPointer });

  setTimeout(() => {
    callback.close(); // functionWithCallbackArg is now done with `callbackPointer`, so free the memory
  }, 1000);
}

main();
*/

export { JSCallback, webview_hint };
export default libwebview;
