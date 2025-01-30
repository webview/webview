"use strict";
const libCb = require("bindings")("./jscallback.node");
const { libWv: wv } = require("./webview.napi.js");
const cbMap = new Map();
class JsCallback extends libCb.JsCallback {
    /**
     * Instance values and methods for working with Webview callback functions.
     * @param w - The Webview instance pointer
     * @param cbKind - "bind" or "dispatch"
     * @param callback - The user defined JS callback function
     */
    constructor(w, cbKind, callback) {
        if (cbKind === "bind")
            callback = wrapBindCb(w, callback);
        if (cbKind === "dispatch")
            callback = wrapDispatchCb(callback);
        super(callback);
        /** Argument counter for the instance */
        this.argCount = 0;
        /**
         * Initiates a user provided argument in the native callback instance.
         *  The `JsCallback` instance UID piggy-backs on the Webview user `arg` parameter ID.
         *  @returns `Object` The `arg` ID and the instance UID piggy-backed.
         */
        this.arg = (arg) => {
            if (!super.setArg)
                return isClosing(this.uid);
            const argId = this.argCount;
            this.argCount++;
            super.setArg({ arg }, argId);
            return { cbUid: this.uid, argId };
        };
        this.uid = super.cbUid;
        if (cbKind === "bind")
            this.ptr = super.bindPtr;
        if (cbKind === "dispatch")
            this.ptr = super.dispatchPtr;
        cbMap.set(this.uid, this);
    }
    /**
     * Releases the memory associated with this `JsCallback` instance.
     * This method must explicitly be called by the user to avoid memory leaks.
     * */
    close(isShutdown) {
        if (this.ptr === null)
            return;
        this.ptr = null;
        isShutdown ? super.destroy(true) : super.destroy();
        cbMap.delete(this.uid);
    }
    /**
     * Destroys active cb instances then ends the process.
     * @param code
     */
    static destroy(code) {
        cbMap.forEach((self) => {
            self.close(true);
        });
        process.exit(code);
    }
}
/** Wraps the user cb in the expected Webview `dispatch` cb shape. */
function wrapDispatchCb(callback) {
    return function (w, arg) {
        callback(w, arg);
    };
}
/** Wraps the user cb in the expected Webview `bind` cb shape.  */
function wrapBindCb(w, callback) {
    return function (id, req, arg) {
        try {
            const args = JSON.parse(req);
            let result = callback(...args, arg); // The bound user provided `arg` is added to the back of the arguments.
            const isPromise = result instanceof Promise;
            if (!isPromise)
                return processResult(result);
            result = result
                .then((result) => processResult)
                .catch((err) => {
                throw err;
            });
            function processResult(result) {
                result = typeof result !== "undefined" ? JSON.stringify(result) : "";
                wv.webview_return(w, id, 0, result);
            }
        }
        catch (err) {
            err = typeof err !== "undefined" ? JSON.stringify(err) : "";
            wv.webview_return(w, id, 1, err);
        }
    };
}
/** The native cb instance is being destroyed, so return a `null` argId */
function isClosing(cbUid) {
    const message = `A cb function was called after it's native instance (cbUid: ${cbUid}) was closed`;
    console.warn(message);
    return { cbUid, argId: null };
}
module.exports = {
    JsCallback,
};
