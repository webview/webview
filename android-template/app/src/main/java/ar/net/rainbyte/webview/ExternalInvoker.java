package ar.net.rainbyte.webview;

import android.webkit.JavascriptInterface;

public class ExternalInvoker {
    private NativeFunction mCallback;

    public ExternalInvoker(long funPtr) {
        mCallback = new NativeFunction(funPtr);
    }

    @JavascriptInterface
    public void invoke(String msg) {
        mCallback.run(msg);
    }
}
