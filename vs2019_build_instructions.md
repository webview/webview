# Visual Studio 2019 Build Steps for C++

These build steps were taken primarily from Harikvpy's comment on an issue.

1. Create a new C++ `Windows Desktop Application` project

2. Clone the webview repo into your project, I cloned into the project folder and used $(ProjectDir)\webview as my path to the repo, but below I will use `<path to webview repo>` as a palceholder

3. In file explorer navigate to `<path to webview repo>`\script and run build.bat to generate the webview.lib library. It will end up in `<path to webview repo>`\build

4. In the main file for the project, you can delete all everything in the file until you end up with only this:

    ```c++
    int APIENTRY wWinMain(_In_ HINSTANCE    hInstance,
                          _In_opt_ HINSTANE hPrevInstance,
                          _In_ LPWSTR       lpCmdLine,
                          _In_ int          ncmdShow)
    {
        return 0;
    }
    ```

5. Now you can add your code like a normal project, here I'll use the front page example, adding the ifdefs to make it cross platform:

    ```c++
    #include "webview.h"
    #ifdef WIN32
    int APIENTRY wWinMain(_In_ HINSTANCE    hInstance,
                          _In_opt_ HINSTANE hPrevInstance,
                          _In_ LPWSTR       lpCmdLine,
                          _In_ int          ncmdShow)
    #else
    int main()
    #endif
    {
        webview::webview w(true, nullptr);
        w.set_title("Minimal example");
        w.set_size(480, 320, WEBVIEW_HINT_NONE | WEBVIEW_HINT_NO_CTX);
        w.navigate("https://en.m.wikipedia.org/wiki/Main_Page");
        w.run();
        return 0;
    }
    ```
4. In Visual Studio Right click the project and open the properties window

5. Under C++/C -> Preprocessor add the following definitions:

    _CRT_SECURE_NO_WARNINGS;_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS;WEBVIEW_API=__declspec(dllexport)

6. Just below, undefine the following:

    UNICODE;_UNICODE;

7. Under C++/C -> Language set the Conformance Mode to No (/permissive)

8. Under C++/C -> Language set the C++ standard to C++ 17 using the drop down

9. Under C++/C -> General, add the following include directories:

    `<path to webview repo>`;`<path to webview repo>`\script\microsoft.web.webview2.1.0.664.37\build\native\include;

10. Under Linker -> General -> Additional Library Directories, add these:

    `<path to webview repo>`\build;`<path to webview repo>`\script\microsoft.web.webview2.1.0.664.37\build\native\$(PlatformShortName)

11. Under Linker -> Input add these Additional Dependencies:
    webview.lib;WebView2Loader.dll.lib

12. Build to get the output folder and make sure everything compiles

13. Copy the webview.dll and WebView2Loader.dll to the output folder, and make sure to use the right dlls for the architecture you're targetting (x64/64 bit vs x86/32 bit), find them in the `<path to webview repo>`\dll\\(x86 | x64) folder(s)

14. Run the application



