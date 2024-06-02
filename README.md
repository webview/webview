# webview

<a href="https://discord.gg/24KMecn" title="Join the chat at Discord"><img src="https://assets-global.website-files.com/6257adef93867e50d84d30e2/636e0b5061df29d55a92d945_full_logo_blurple_RGB.svg" alt="Discord" height="20" /></a>
[![Build Status](https://img.shields.io/github/actions/workflow/status/webview/webview/ci.yaml?branch=master)](https://github.com/webview/webview)

A tiny cross-platform webview library for C/C++ to build modern cross-platform GUIs.

The goal of the project is to create a common HTML5 UI abstraction layer for the most widely used platforms.

It supports two-way JavaScript bindings (to call JavaScript from C/C++ and to call C/C++ from JavaScript).

> [!NOTE]
> Language binding for Go [has moved][webview_go]. Versions <= 0.1.1 are available in *this* repository.

## Platform Support

Platform | Technologies
-------- | ------------
Linux    | [GTK 3][gtk], [WebKitGTK][webkitgtk]
macOS    | Cocoa, [WebKit][webkit]
Windows  | [Windows API][win32-api], [WebView2][ms-webview2]

## Documentation

The most up-to-date documentation is right in the source code. Improving the documentation is a continuous effort and you are more than welcome to contribute.

## Prerequisites

Your compiler must support minimum C++11 except for platforms that require a more modern version.

### Linux and BSD

The [GTK][gtk] and [WebKit2GTK][webkitgtk] libraries are required for development and distribution. You need to check your package repositories regarding how to install those those.

Debian-based systems:

* Packages:
  * Development: `apt install libgtk-3-dev libwebkit2gtk-4.0-dev`
  * Production: `apt install libgtk-3-0 libwebkit2gtk-4.0-37`

Fedora-based systems:

* Packages:
  * Development: `dnf install gtk3-devel webkit2gtk4.0-devel`
  * Production: `dnf install gtk3 webkit2gtk4.0`

BSD-based systems:

* FreeBSD packages: `pkg install webkit2-gtk3`
* Execution on BSD-based systems may require adding the `wxallowed` option (see [mount(8)](https://man.openbsd.org/mount.8))  to your fstab to bypass [W^X](https://en.wikipedia.org/wiki/W%5EX "write xor execute") memory protection for your executable. Please see if it works without disabling this security feature first.

### Windows

Your compiler must support C++14 and we recommend to pair it with an up-to-date Windows 10 SDK.

For Visual C++ we recommend Visual Studio 2022 or later. We have a [separate section for MinGW-w64](#mingw-w64-requirements).

Developers and end-users must have the [WebView2 runtime][ms-webview2-rt] installed on their system for any version of Windows before Windows 11.

## Getting Started

If you are a developer of this project then please go to the [development section](#development).

Instructions here are written for GCC when compiling C/C++ code using Unix-style command lines, and assumes that multiple commands are executed in the same shell session. Command lines for Windows use syntax specific to the Command shell but you can use any shell such as PowerShell as long as you adapt the commands accordingly. See the [MinGW-w64 requirements](#mingw-w64-requirements) when building on Windows.

You will have a working app but you are encouraged to explore the [available examples][examples] and try the ones that go beyond the mere basics.

Start with creating a new directory structure for your project:

```sh
mkdir my-project && cd my-project
mkdir build libs "libs/webview"
```

### Windows Preparation

The [WebView2 SDK][ms-webview2-sdk] is required when compiling programs:

```bat
mkdir libs\webview2
curl -sSL "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2" | tar -xf - -C libs\webview2
```

If you wish to use the official WebView2 loader (`WebView2Loader.dll`) then grab a copy of the DLL (replace `x64` with your target architecture):

```bat
copy /Y libs\webview2\build\native\x64\WebView2Loader.dll build
```

> **Note:** See the [WebView2 loader section](#ms-webview2-loader) for more options.

### C/C++ Preparation

Fetch the webview library:

```sh
curl -sSLo "libs/webview/webview.h" "https://raw.githubusercontent.com/webview/webview/master/webview.h"
curl -sSLo "libs/webview/webview.cc" "https://raw.githubusercontent.com/webview/webview/master/webview.cc"
```

### Getting Started with C++

Save the basic C++ example into your project directory:

```sh
curl -sSLo basic.cc "https://raw.githubusercontent.com/webview/webview/master/examples/basic.cc"
```

Build and run the example:

```sh
# Linux
g++ basic.cc -std=c++11 -Ilibs/webview $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0) -o build/basic && ./build/basic
# macOS
g++ basic.cc -std=c++11 -Ilibs/webview -framework WebKit -o build/basic && ./build/basic
# Windows/MinGW
g++ basic.cc -std=c++14 -mwindows -Ilibs/webview -Ilibs/webview2/build/native/include -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion -o build/basic.exe && "build/basic.exe"
```

#### Bonus for Visual C++

Build a C++ example:

```bat
cl basic.cc /std:c++14 /EHsc /Fobuild\ ^
    /I libs\webview ^
    /I libs\webview2\build\native\include ^
    /link /OUT:build\basic.exe
```

### Getting Started with C

Save the basic C example into your project directory:

```sh
curl -sSLo basic.c "https://raw.githubusercontent.com/webview/webview/master/examples/basic.c"
```

Build the library and example, then run it:

```sh
# Linux
g++ -c libs/webview/webview.cc -std=c++11 -DWEBVIEW_STATIC $(pkg-config --cflags gtk+-3.0 webkit2gtk-4.0) -o build/webview.o
gcc -c basic.c -std=c99 -Ilibs/webview -o build/basic.o
g++ build/basic.o build/webview.o $(pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -o build/basic && build/basic
# macOS
g++ -c libs/webview/webview.cc -std=c++11 -DWEBVIEW_STATIC -o build/webview.o
gcc -c basic.c -std=c99 -Ilibs/webview -o build/basic.o
g++ build/basic.o build/webview.o -framework WebKit -o build/basic && build/basic
# Windows/MinGW
g++ -c libs/webview/webview.cc -std=c++14 -DWEBVIEW_STATIC -Ilibs/webview2/build/native/include -o build/webview.o
gcc -c basic.c -std=c99 -Ilibs/webview -o build/basic.o
g++ build/basic.o build/webview.o -mwindows -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion -o build/basic.exe && "build/basic.exe"
```

#### Bonus for Visual C++

Build a shared library:

```bat
cl libs\webview\webview.cc /std:c++14 /EHsc /Fobuild\ ^
    /D WEBVIEW_BUILD_SHARED ^
    /I libs\webview ^
    /I libs\webview2\build\native\include ^
    /link /DLL /OUT:build\webview.dll
```

Build a C example using the shared library:

```bat
cl basic.c build\webview.lib /EHsc /Fobuild\ ^
    /D WEBVIEW_SHARED ^
    /I libs\webview ^
    /link /OUT:build\basic.exe
```

### More Examples

The examples shown here are mere pieces of a bigger picture so we encourage you to try [other examples][examples] and explore on your own—you can follow the same procedure. Please [get in touch][issues-new] if you find any issues.

### Compile-time Options

#### C API Linkage

Name                   | Description
----                   | -----------
`WEBVIEW_API`          | Controls C API linkage, symbol visibility and whether it's a shared library. By default this is `inline` for C++ and `extern` for C.
`WEBVIEW_BUILD_SHARED` | Modifies `WEBVIEW_API` for building a shared library.
`WEBVIEW_SHARED`       | Modifies `WEBVIEW_API` for using a shared library.
`WEBVIEW_STATIC`       | Modifies `WEBVIEW_API` for building or using a static library.

## App Distribution

Distribution of your app is outside the scope of this library but we can give some pointers for you to explore.

### macOS Application Bundle

On macOS you would typically create a bundle for your app with an icon and proper metadata.

A minimalistic bundle typically has the following directory structure:

```
example.app                 bundle
└── Contents
    ├── Info.plist          information property list
    ├── MacOS
    |   └── example         executable
    └── Resources
        └── example.icns    icon
```

Read more about the [structure of bundles][macos-app-bundle] at the Apple Developer site.

> Tip: The `png2icns` tool can create icns files from PNG files. See the `icnsutils` package for Debian-based systems.

### Windows Apps

You would typically create a resource script file (`*.rc`) with information about the app as well as an icon. Since you should have MinGW-w64 readily available then you can compile the file using `windres` and link it into your program. If you instead use Visual C++ then look into the [Windows Resource Compiler][win32-rc].

The directory structure could look like this:

```
my-project/
├── icons/
|   ├── application.ico
|   └── window.ico
├── basic.cc
└── resources.rc
```

`resources.rc`:
```
100 ICON "icons\\application.ico"
32512 ICON "icons\\window.ico"
```

> **Note:** The ID of the icon resource to be used for the window must be `32512` (`IDI_APPLICATION`).

Compile:
```sh
windres -o build/resources.o resources.rc
g++ basic.cc build/resources.o [...]
```

Remember to bundle the DLLs you have not linked statically, e.g. those from MinGW-w64 and optionally `WebView2Loader.dll`.

## MinGW-w64 Requirements

In order to build this library using MinGW-w64 on Windows then it must support C++14 and have an up-to-date Windows SDK.

Distributions that are known to be compatible:

* [LLVM MinGW](https://github.com/mstorsjo/llvm-mingw)
* [MSYS2](https://www.msys2.org/)
* [WinLibs](https://winlibs.com/)

## MS WebView2 Loader

Linking the WebView2 loader part of the Microsoft WebView2 SDK is not a hard requirement when using our webview library, and neither is distributing `WebView2Loader.dll` with your app.

If, however, `WebView2Loader.dll` is loadable at runtime, e.g. from the executable's directory, then it will be used; otherwise our minimalistic implementation will be used instead.

Should you wish to use the official loader then remember to distribute it along with your app unless you link it statically. Linking it statically is possible with Visual C++ but not MinGW-w64.

Here are some of the noteworthy ways our implementation of the loader differs from the official implementation:

* Does not support configuring WebView2 using environment variables such as `WEBVIEW2_BROWSER_EXECUTABLE_FOLDER`.
* Microsoft Edge Insider (preview) channels are not supported.

The following compile-time options can be used to change how the library integrates the WebView2 loader:

* `WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL=<1|0>` - Enables or disables the built-in implementation of the WebView2 loader. Enabling this avoids the need for `WebView2Loader.dll` but if the DLL is present then the DLL takes priority. This option is enabled by default.
* `WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK=<1|0>` - Enables or disables explicit linking of `WebView2Loader.dll`. Enabling this avoids the need for import libraries (`*.lib`). This option is enabled by default if `WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL` is enabled.

## Development

To build the library, examples and run tests, use one of the builds scripts in the `script` directory:

* `build.sh`:
  * On Unix-based systems.
  * On Windows in a Unix-like environment such as MSYS2.

* `build.bat`:
  * On Windows when building with Visual C++.

You can specify individual tasks on the command line for these scripts:

Task       | Description
---------- | ---------------------------------------
`info`     | Displays information.
`clean`    | Cleans the build directory.
`format`   | Reformats code.
`deps`     | Fetches dependencies.
`check`    | Runs checks.
`build`    | Builds the library, examples and tests.
`test`     | Runs tests.

Additionally, the scripts accept the following environment variables.

Both scripts:

Variable     | Description
------------ | ---------------------------------------------------------
`CI`         | Changes behavior in CI environments (more strict).
`TARGET_ARCH`| Target architecture for cross-compilation (`x64`, `x86`).
`BUILD_DIR`  | Overrides the path of the build directory.

Only `build.sh`:

Variable     | Description
------------ | --------------------------------------------------------------
`HOST_OS`    | Host operating system (`linux`, `macos`, `windows`).
`TARGET_OS`  | Target operating system for cross-compilation (see `HOST_OS`).
`CC`         | C compiler executable.
`CXX`        | C++ compiler executable.
`LIB_PREFIX` | Library name prefix.
`PKGCONFIG`  | Alternative `pkgconfig` executable.

### Cross-compilation

See the CI configuration for examples.

## Limitations

### Browser Features

Since a browser engine is not a full web browser it may not support every feature you may expect from a browser. If you find that a feature does not work as expected then please consult with the browser engine's documentation and [open an issue][issues-new] if you think that the library should support it.

For example, the library does not attempt to support user interaction features like `alert()`, `confirm()` and `prompt()` and other non-essential features like `console.log()`.

## Bindings

Language    | Project
----------  | -------
Bun         | [tr1ckydev/webview-bun](https://github.com/tr1ckydev/webview-bun)
C#          | [webview/webview_csharp](https://github.com/webview/webview_csharp)
C3          | [thechampagne/webview-c3](https://github.com/thechampagne/webview-c3)
Crystal     | [naqvis/webview](https://github.com/naqvis/webview)
D           | [thechampagne/webview-d](https://github.com/thechampagne/webview-d)
Deno        | [webview/webview_deno](https://github.com/webview/webview_deno)
Go          | [webview/webview_go][webview_go]
Harbour     | [EricLendvai/Harbour_WebView](https://github.com/EricLendvai/Harbour_WebView)
Haskell     | [lettier/webviewhs](https://github.com/lettier/webviewhs)
Janet       | [janet-lang/webview](https://github.com/janet-lang/webview)
Java        | [webview/webview_java](https://github.com/webview/webview_java)
Kotlin      | [Winterreisender/webviewko](https://github.com/Winterreisender/webviewko)
Nim         | [oskca/webview](https://github.com/oskca/webview), [neroist/webview](https://github.com/neroist/webview)
Node.js     | [Winterreisender/webview-nodejs](https://github.com/Winterreisender/webview-nodejs)
Odin        | [thechampagne/webview-odin](https://github.com/thechampagne/webview-odin)
Pascal      | [PierceNg/fpwebview](http://github.com/PierceNg/fpwebview)
Python      | [zserge/webview-python](https://github.com/zserge/webview-python)
PHP         | [0hr/php-webview](https://github.com/0hr/php-webview)
Ruby        | [Maaarcocr/webview_ruby](https://github.com/Maaarcocr/webview_ruby)
Rust        | [Boscop/web-view](https://github.com/Boscop/web-view)
Swift       | [jakenvac/SwiftWebview](https://github.com/jakenvac/SwiftWebview)
V           | [malisipi/mui](https://github.com/malisipi/mui/tree/main/webview), [ttytm/webview](https://github.com/ttytm/webview)
Zig         | [thechampagne/webview-zig](https://github.com/thechampagne/webview-zig)

If you wish to add bindings to the list, feel free to submit a pull request or [open an issue][issues-new].

## Generating Bindings

You can generate bindings for the library by yourself using the included SWIG interface (`webview.i`).

Here are some examples to get you started. Unix-style command lines are used for conciseness.

```sh
mkdir -p build/bindings/{python,csharp,java,ruby}
swig -c++ -python -outdir build/bindings/python -o build/bindings/python/python_wrap.cpp webview.i
swig -c++ -csharp -outdir build/bindings/csharp -o build/bindings/csharp/csharp_wrap.cpp webview.i
swig -c++ -java -outdir build/bindings/java -o build/bindings/java/java_wrap.cpp webview.i
swig -c++ -ruby -outdir build/bindings/ruby -o build/bindings/ruby/ruby_wrap.cpp webview.i
```

## License

Code is distributed under MIT license, feel free to use it in your proprietary projects as well.

[macos-app-bundle]:  https://developer.apple.com/library/archive/documentation/CoreFoundation/Conceptual/CFBundles/BundleTypes/BundleTypes.html
[examples]:          https://github.com/webview/webview/tree/master/examples
[gtk]:               https://docs.gtk.org/gtk3/
[issues]:            https://github.com/webview/docs/issues
[issues-new]:        https://github.com/webview/webview/issues/new
[webkit]:            https://webkit.org/
[webkitgtk]:         https://webkitgtk.org/
[webview]:           https://github.com/webview/webview
[webview_go]:        https://github.com/webview/webview_go
[webview.dev]:       https://webview.dev
[ms-webview2]:       https://developer.microsoft.com/en-us/microsoft-edge/webview2/
[ms-webview2-sdk]:   https://www.nuget.org/packages/Microsoft.Web.WebView2
[ms-webview2-rt]:    https://developer.microsoft.com/en-us/microsoft-edge/webview2/
[win32-api]:         https://docs.microsoft.com/en-us/windows/win32/apiindex/windows-api-list
[win32-rc]:          https://docs.microsoft.com/en-us/windows/win32/menurc/resource-compiler
