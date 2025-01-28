import { Worker, isMainThread, parentPort } from "node:worker_threads";
import lib, {
  JsCallback,
  webview_hint,
  webview_native_handle_kind,
} from "./JsCallback.js";

isMainThread ? runMainThread() : runWorkerThread(); // ********** The main and worker threads' code is combined into one file for convenience.

/** *************************************************************
 * Runs the worker thread, which is going to be blocked by Webview
 * **************************************************************/
function runWorkerThread() {
  const w = lib.webview_create(1, null);

  lib.webview_set_html(w, getHTML());

  parentPort.postMessage(w); // ******************************* Pass the Webview instance pointer to the main thread.

  lib.webview_run(w); // ************************************** worker thread is now blocked until Webview is terminated.
  lib.webview_destroy(w);
}

/** ***********************************************************
 * Runs the main thread, which is not blocked by Webview
 * ************************************************************/
function runMainThread() {
  const worker = new Worker("./example.js");

  worker.on("message", (w) => {
    infoLog(w);

    lib.webview_set_size(w, 1200, 1200, webview_hint.WEBVIEW_HINT_MIN);
    lib.webview_set_title(w, "Thread-safe Webview");

    // ******************************************************** Create a JS dispatch cb function
    const dispatchCbFn = (w, arg) => {
      const message = "Dispatch (main to worker thread) returns:";
      console.log(message, `${typeof arg}:`, { w, arg });
    };
    const dpCb = new JsCallback(w, "dispatch", dispatchCbFn);
    // ******************************************************** Create and bind a JS cb function
    const bindCbFn = (argIndexOrBoundFn, maybeBoundArg) => {
      const hasArgIndex = typeof maybeBoundArg !== "undefined";
      const infoMessage = hasArgIndex
        ? "The bound fn was called with an index `arg`, it will return to the window the backend arg at index:"
        : "The bound fn was called without an index `arg`, it will return to the window the backend bound `arg`:";
      console.info(infoMessage, argIndexOrBoundFn);
      if (!hasArgIndex) return argIndexOrBoundFn;
      const args = getArgs();
      const arg = args[argIndexOrBoundFn];
      return arg;
    };
    const bindCb = new JsCallback(w, "bind", bindCbFn);
    const boundArg = bindCb.arg("I am the bound arg");
    lib.webview_bind(w, "boundFn", bindCb.ptr, boundArg);

    setTimeout(() => {
      // ***************************************************** Call the bound JS cb function from the Webview window.
      lib.webview_eval(w, `boundFn().then(res => appendBoundRes(res));`);
      lib.webview_eval(w, `boundFn(0).then(res => appendBoundRes(res));`);
      lib.webview_eval(w, `boundFn(1).then(res => appendBoundRes(res));`);
      lib.webview_eval(w, `boundFn(2).then(res => appendBoundRes(res));`);
      lib.webview_eval(w, `boundFn(3).then(res => appendBoundRes(res));`);

      // ****************************************************** Dispatch the cb function to the worker thread.
      const args = getArgs();
      lib.webview_dispatch(w, dpCb.ptr, dpCb.arg());
      lib.webview_dispatch(w, dpCb.ptr, dpCb.arg(args[0]));
      lib.webview_dispatch(w, dpCb.ptr, dpCb.arg(args[1]));
      lib.webview_dispatch(w, dpCb.ptr, dpCb.arg(args[2]));
      lib.webview_dispatch(w, dpCb.ptr, dpCb.arg(args[3]));

      dpCb.close(); //**************************************** We are done with the dispatch cb function, so free the memory.
    }, 1000);

    setTimeout(() => {
      bindCb.close(); // ************************************* We are done with the bind cb function, so free the memory
      lib.webview_terminate(w); // *************************** Close the Webview window.
    }, 7000);
  });
}

function getHTML() {
  return `
  <html>
    <body>
      <p>
        <h1>Hello Webview</h1>
        Multi-thread for Node JS.</br>This example will close itself in a few seconds.
      </p>
      <div id="bound">
        <h3>Returned values from the bound cb function:</h3>
      </div>
    </body>
    <script>
      window.appendBoundRes = function(res){
        res = typeof res !== "string" ? JSON.stringify(res, null, 4) : res;
        const p = "<p>" + res + "</p>";
        document.getElementById("bound").innerHTML += p;
      }
    </script>
  </html>
  `;
}
function getArgs() {
  return [
    { val: "I am an `Object` arg" },
    ["I am an `Object` array arg"],
    123.321,
    "etc...",
  ];
}
function infoLog(webview_t) {
  console.info("Webview version:", lib.webview_version());
  const ui_window = lib.webview_get_window(webview_t);
  const kind_ui_widget = lib.webview_get_native_handle(
    webview_t,
    webview_native_handle_kind.WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET
  );
  const kind_browser_controller = lib.webview_get_native_handle(
    webview_t,
    webview_native_handle_kind.WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER
  );
  const kind_ui_window = lib.webview_get_native_handle(
    webview_t,
    webview_native_handle_kind.WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW
  );
  console.info("Webview handle kinds:", {
    webview_t,
    ui_window,
    kind_ui_window,
    kind_browser_controller,
    kind_ui_widget,
  });
}
