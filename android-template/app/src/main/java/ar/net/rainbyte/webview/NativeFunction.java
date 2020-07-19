package ar.net.rainbyte.webview;

public class NativeFunction {
    static {
        System.loadLibrary("webview-android-jni");
    }
    public native void runFunction(long funPtr, Object ... args);

    long mFunPtr;

    public NativeFunction(long funPtr) {
        mFunPtr = funPtr;
    }

    public void run(Object ... args) {
        runFunction(mFunPtr, args);
    }
}
