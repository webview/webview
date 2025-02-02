const { Worker, isMainThread, parentPort } = require("node:worker_threads");
const { JsCallback } = require("./dist/JsCallback.js");
const {
  libWv: wv,
  webview_hint,
  webview_native_handle_kind,
} = require("./dist/webview.napi.js");

isMainThread ? runMainThread() : runWorkerThread(); // ********** The main and worker threads' code is combined into one file for convenience.

/** *************************************************************
 * Runs the worker thread, which is going to be blocked by Webview
 * *********************************************************** */
function runWorkerThread() {
  const w = wv.webview_create(1, null);
  wv.webview_set_html(w, getHTML());

  parentPort.postMessage(w); // ******************************* Pass the Webview instance pointer to the main thread.

  wv.webview_run(w); // *************************************** The worker thread is now blocked until Webview is terminated.
  wv.webview_destroy(w);
}

/** ***********************************************************
 * Runs the main thread, which is not blocked by Webview
 * ********************************************************* */
function runMainThread() {
  const worker = new Worker("./example.js");
  worker.on("exit", (code) => {
    JsCallback.destroy(code); // ***************************** Free all cb memory if the window is closed by the user.
  });
  worker.on("error", (message) => {
    JsCallback.destroy(1);
    throw message;
  });
  worker.on("message", (w) => {
    infoLog(w);

    wv.webview_set_size(w, 1200, 1200, webview_hint.WEBVIEW_HINT_MIN);
    wv.webview_set_title(w, "Thread-safe Webview");

    // ******************************************************** Create a dispatch JS cb function
    const dispatchCbFn = (w, arg) => {
      const message = "Dispatch (main to worker thread) returns:";
      console.log(message, `${typeof arg}:`, { w, arg });
    };
    const dpCb = new JsCallback(w, "dispatch", dispatchCbFn);

    // ******************************************************** Create and bind a JS cb function
    const bindCbFn = (argIndexOrBoundFn, maybeBoundArg) => {
      const i = typeof maybeBoundArg !== "undefined" ? argIndexOrBoundFn : null;
      const infoMessage =
        i !== null
          ? "The bound fn was called with a parameter value, it will return to the window an indexed arg value:"
          : "The bound fn was called without a parameter value, it will return to the window it's bound `arg`:";
      console.info(infoMessage, argIndexOrBoundFn);
      return i === null ? argIndexOrBoundFn : getUserArgs()[i];
    };
    const bindCb = new JsCallback(w, "bind", bindCbFn);
    const boundArg = bindCb.arg("I am the bound `arg`");
    wv.webview_bind(w, "boundFn", bindCb.ptr, boundArg);

    setTimeout(() => {
      // ***************************************************** Call the bound JS cb function from the Webview window.
      wv.webview_eval(w, `boundFn().then(res => appendBoundRes(res));`);
      wv.webview_eval(w, `boundFn(0).then(res => appendBoundRes(res));`);
      wv.webview_eval(w, `boundFn(1).then(res => appendBoundRes(res));`);
      wv.webview_eval(w, `boundFn(2).then(res => appendBoundRes(res));`);
      wv.webview_eval(w, `boundFn(3).then(res => appendBoundRes(res));`);

      // ****************************************************** Dispatch the cb function to the worker thread.
      const args = getUserArgs();
      wv.webview_dispatch(w, dpCb.ptr, dpCb.arg());
      wv.webview_dispatch(w, dpCb.ptr, dpCb.arg(args[0]));
      wv.webview_dispatch(w, dpCb.ptr, dpCb.arg(args[1]));
      wv.webview_dispatch(w, dpCb.ptr, dpCb.arg(args[2]));
      wv.webview_dispatch(w, dpCb.ptr, dpCb.arg(args[3]));
      //dpCb.close(); //**************************************** We are done with the dispatch cb function, so free the memory.
    }, 1000);

    setTimeout(() => {
      wv.webview_unbind(w, "boundFn");
      //bindCb.close(); // ************************************* We are done with the bind cb function, so free the memory
      wv.webview_terminate(w); // **************************** Close the Webview window.
    }, 7000);
  });
}

/** ***********************************************************
 * Utilities for the example
 * ********************************************************* */
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
function getUserArgs() {
  return [
    { val: "I am an `Object` arg" },
    ["I am an `Object` array arg"],
    123.321,
    "etc...",
  ];
}
function infoLog(webview_t) {
  console.info("Webview version:", wv.webview_version());
  const ui_window = wv.webview_get_window(webview_t);
  const kind_ui_widget = wv.webview_get_native_handle(
    webview_t,
    webview_native_handle_kind.WEBVIEW_NATIVE_HANDLE_KIND_UI_WIDGET
  );
  const kind_browser_controller = wv.webview_get_native_handle(
    webview_t,
    webview_native_handle_kind.WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER
  );
  const kind_ui_window = wv.webview_get_native_handle(
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
