/**
 * A simplified implementation to demonstrate issues on Windows.
 *
 * - The `JsCallback` implementation has been removed from here as well as the node-gyp build.
 * - I have tried compilation both with the built-in MS WebView2 and Nuget MS WebView2.
 *
 * The result is the same - the MsWebview2 window opens, but is not responding to calls from JS...
 */

const { Worker, isMainThread, parentPort } = require("node:worker_threads");
const { libWv: wv, webview_hint } = require("./dist/webview.napi.js");

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

  worker.on("message", (w) => {
    wv.webview_set_size(w, 1200, 1200, webview_hint.WEBVIEW_HINT_MIN);
    wv.webview_set_title(w, "Thread-safe Webview");

    setTimeout(() => {
      wv.webview_eval(w, "console.log('evaluated');"); // This does not respond on Windows
      console.log("evaluated");
    }, 5000);

    setTimeout(() => {
      wv.webview_terminate(w); // This does not respond on Windows
    }, 8000);
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
