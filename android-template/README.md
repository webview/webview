# Native Activity Webview Template

This sample uses JNI to instantiate an Android Webview from C code.

## Requisites

- [Android Studio](http://developer.android.com/sdk/index.html)
- [Android NDK](https://developer.android.com/ndk/)
- [CMake plugin](http://tools.android.com/tech-docs/external-c-builds)

## Instructions

1. Place `webview.h` inside `app/src/main/cpp` directory.
2. Place custom code (eg. `main.cpp`) in the same folder.
3. Open project with Android Studio.
4. Set NDK in `File/Project Structure...`.
5. Sync Project with Gradle Files.
6. Connect a device with ADB.
7. Compile and run.

## Implementation

Code written by the user should contain at least the following lines:

```cpp
#include "webview.h"

ifdef ANDROID
int android_main(void *android_app) {
  webview::webview w(true, android_app);
#else
int main() {
  webview::webview w(true, nullptr);
#endif
  /* user code goes here! */
  return 0;
}
```

Android entrypoint `android_main` is expected by the library.
