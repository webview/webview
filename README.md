# webview

<a href="https://discord.gg/24KMecn" title="Join the chat at Discord"><img src="https://assets-global.website-files.com/6257adef93867e50d84d30e2/636e0b5061df29d55a92d945_full_logo_blurple_RGB.svg" alt="Discord" height="20" /></a>
[![Build Status](https://img.shields.io/github/actions/workflow/status/webview/webview/ci.yaml?branch=master)](https://github.com/webview/webview/actions)

A tiny cross-platform webview library for C/C++ to build modern cross-platform GUIs.

The goal of the project is to create a common HTML5 UI abstraction layer for the most widely used platforms.

It supports two-way JavaScript bindings (to call JavaScript from C/C++ and to call C/C++ from JavaScript).

> [!NOTE]
> Language binding for Go [has moved][webview_go]. Versions <= 0.1.1 are available in *this* repository.

## Platform Support

Platform | Technologies
-------- | ------------
Linux    | [GTK][gtk], [WebKitGTK][webkitgtk]
macOS    | Cocoa, [WebKit][webkit]
Windows  | [Windows API][win32-api], [WebView2][ms-webview2]

## Documentation

The most up-to-date documentation is right in the source code. Improving the documentation is a continuous effort and you are more than welcome to contribute.

## Prerequisites

Your compiler must support minimum C++11.

This project uses CMake and Ninja, and while recommended for your convenience, these tools aren't required for using the library.

### Linux and BSD

The [GTK][gtk] and [WebKitGTK][webkitgtk] libraries are required for development and distribution. You need to check your package repositories regarding which packages to install.

#### Packages

* Debian:
  * WebKitGTK 6.0, GTK 4:
    * Development: `apt install libgtk-4-dev libwebkitgtk-6.0-dev`
    * Production: `apt install libgtk-4-1 libwebkitgtk-6.0-4`
  * WebKitGTK 4.1, GTK 3, libsoup 3:
    * Development: `apt install libgtk-3-dev libwebkit2gtk-4.1-dev`
    * Production: `apt install libgtk-3-0 libwebkit2gtk-4.1-0`
  * WebKitGTK 4.0, GTK 3, libsoup 2:
    * Development: `apt install libgtk-3-dev libwebkit2gtk-4.0-dev`
    * Production: `apt install libgtk-3-0 libwebkit2gtk-4.0-37`
* Fedora:
  * WebKitGTK 6.0, GTK 4:
    * Development: `dnf install gtk4-devel webkitgtk6.0-devel`
    * Production: `dnf install gtk4 webkitgtk6.0`
  * WebKitGTK 4.1, GTK 3, libsoup 3:
    * Development: `dnf install gtk3-devel webkit2gtk4.1-devel`
    * Production: `dnf install gtk3 webkit2gtk4.1`
  * WebKitGTK 4.0, GTK 3, libsoup 2:
    * Development: `dnf install gtk3-devel webkit2gtk4.0-devel`
    * Production: `dnf install gtk3 webkit2gtk4.0`
* FreeBSD:
  * GTK 4: `pkg install webkit2-gtk4`
  * GTK 3: `pkg install webkit2-gtk3`

#### Library Dependencies

* Linux:
  * Use `pkg-config` with `--cflags` and `--libs` to get the compiler/linker options for one of these sets of modules:
    * `gtk4 webkitgtk-6.0`
    * `gtk+-3.0 webkit2gtk-4.1`
    * `gtk+-3.0 webkit2gtk-4.0`
  * Link libraries: `dl`
* macOS:
  * Link frameworks: `WebKit`
  * Link libraries: `dl`
* Windows:
  * [WebView2 from NuGet](https://www.nuget.org/packages/Microsoft.Web.WebView2).
  * Windows libraries: `advapi32 ole32 shell32 shlwapi user32 version`

#### BSD

* Execution on BSD-based systems may require adding the `wxallowed` option (see [mount(8)](https://man.openbsd.org/mount.8))  to your fstab to bypass [W^X](https://en.wikipedia.org/wiki/W%5EX "write xor execute") memory protection for your executable. Please see if it works without disabling this security feature first.

### Windows

Your compiler must support C++14 and we recommend to pair it with an up-to-date Windows 10 SDK.

For Visual C++ we recommend Visual Studio 2022 or later. There are some [requirements when using MinGW-w64](#mingw-w64-requirements).

Developers and end-users must have the [WebView2 runtime][ms-webview2-rt] installed on their system for any version of Windows before Windows 11.

## Getting Started

If you are a developer of this project then please go to the [development section](#development).

You will have a working app, but you are encouraged to explore the [available examples][examples].

Create the following files in a new directory:

`.gitignore`:
```
# Build artifacts
/build
```

### C++ Example

`CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.16)
project(example LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include(FetchContent)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 0.12.0)
FetchContent_MakeAvailable(webview)

add_executable(example WIN32)
target_sources(example PRIVATE main.cc)
target_link_libraries(example PRIVATE webview::core)
```

`main.cc`:
```cpp
#include "webview/webview.h"

#include <iostream>

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
  try {
    webview::webview w(false, nullptr);
    w.set_title("Basic Example");
    w.set_size(480, 320, WEBVIEW_HINT_NONE);
    w.set_html("Thanks for using webview!");
    w.run();
  } catch (const webview::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
```

### C Example

`CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.16)
project(example LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include(FetchContent)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 0.12.0)
FetchContent_MakeAvailable(webview)

add_executable(example WIN32)
target_sources(example PRIVATE main.c)
target_link_libraries(example PRIVATE webview::core_static)
```

`main.c`:
```cpp
#include "webview/webview.h"
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
  (void)hInst;
  (void)hPrevInst;
  (void)lpCmdLine;
  (void)nCmdShow;
#else
int main(void) {
#endif
  webview_t w = webview_create(0, NULL);
  webview_set_title(w, "Basic Example");
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
  webview_set_html(w, "Thanks for using webview!");
  webview_run(w);
  webview_destroy(w);
  return 0;
}
```

### Building the Example

Build the project:

```sh
cmake -G Ninja -B build -S . -D CMAKE_BUILD_TYPE=Release
cmake --build build
```

Find the executable in the `build/bin` directory.

### Building Amalgamated Library

An amalgamated library can be built when building the project using CMake, or the `amalgamate.py` script can be invoked directly.

The latter is described below.

```sh
python3 scripts/amalgamate/amalgamate.py --base core --search include --output webview_amalgamation.h src
```

See `python3 scripts/amalgamate/amalgamate.py --help` for script usage.

### Non-CMake Usage

Here's an example for invoking GCC/Clang-like compilers directly. Use the `main.cc` file from the previous example.

Place either the amalgamated `webview.h` header or all of the individual files into `libs/webview`, and `WebView2.h` from [MS WebView2][ms-webview2-sdk] into `libs`.

Build the project on your chosen platform.

<details>
  <summary>macOS</summary>
  <pre><code>c++ main.cc -O2 --std=c++11 -Ilibs -framework WebKit -ldl -o example</code></pre>
</details>

<details>
  <summary>Linux</summary>
  <pre><code>c++ main.cc -O2 --std=c++11 -Ilibs $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.1) -ldl -o example</code></pre>
</details>

<details>
  <summary>Windows</summary>
  <pre><code>c++ main.cc -O2 --std=c++14 -static -mwindows -Ilibs -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion -o example</code></pre>
</details>

## Customization

### CMake Targets

The following CMake targets are available:

Name                   | Description
----                   | -----------
`webview::core`        | Headers for C++.
`webview::core_shared` | Shared library for C.
`webview::core_static` | Static library for C.

Special targets for on-demand checks and related tasks:

Name                   | Description
----                   | -----------
`webview_format_check` | Check files with clang-format.
`webview_reformat`     | Reformat files with clang-format.

### CMake Options

The following boolean options can be used when building the webview project standalone or when building it as part of your project (e.g. with FetchContent).

Option                            | Description
------                            | -----------
`WEBVIEW_BUILD`                   | Enable building
`WEBVIEW_BUILD_AMALGAMATION`      | Build amalgamated library
`WEBVIEW_BUILD_DOCS`              | Build documentation
`WEBVIEW_BUILD_EXAMPLES`          | Build examples
`WEBVIEW_BUILD_SHARED_LIBRARY`    | Build shared libraries
`WEBVIEW_BUILD_STATIC_LIBRARY`    | Build static libraries
`WEBVIEW_BUILD_TESTS`             | Build tests
`WEBVIEW_ENABLE_CHECKS`           | Enable checks
`WEBVIEW_ENABLE_CLANG_FORMAT`     | Enable clang-format
`WEBVIEW_ENABLE_CLANG_TIDY`       | Enable clang-tidy
`WEBVIEW_ENABLE_PACKAGING`        | Enable packaging
`WEBVIEW_INSTALL_DOCS`            | Install documentation
`WEBVIEW_INSTALL_TARGETS`         | Install targets
`WEBVIEW_IS_CI`                   | Initialized by the `CI` environment variable
`WEBVIEW_PACKAGE_AMALGAMATION`    | Package amalgamated library
`WEBVIEW_PACKAGE_DOCS`            | Package documentation
`WEBVIEW_PACKAGE_HEADERS`         | Package headers
`WEBVIEW_PACKAGE_LIB`             | Package compiled libraries
`WEBVIEW_STRICT_CHECKS`           | Make checks strict
`WEBVIEW_STRICT_CLANG_FORMAT`     | Make clang-format check strict
`WEBVIEW_STRICT_CLANG_TIDY`       | Make clang-tidy check strict
`WEBVIEW_USE_COMPAT_MINGW`        | Use compatibility helper for MinGW
`WEBVIEW_USE_STATIC_MSVC_RUNTIME` | Use static runtime library (MSVC)

> [!NOTE]
> Checks are *enabled* by default, but aren't *enforced* by default for local development (controlled by the `WEBVIEW_IS_CI` option).

Non-boolean options:

Option                            | Description
------                            | -----------
`WEBVIEW_CLANG_FORMAT_EXE`        | Path of the `clang-format` executable.
`WEBVIEW_CLANG_TIDY_EXE`          | Path of the `clang-tidy` executable.

### Package Consumer Options

These options can be used when when using the webview CMake package.

#### Linux-specific Options

Option                          | Description
------                          | -----------
`WEBVIEW_WEBKITGTK_API`         | WebKitGTK API to interface with, e.g. `6.0`, `4.1` (recommended) or `4.0`. This will also automatically decide the GTK version. Uses the latest recommended API by default if available, or the latest known and available API. Note that there can be major differences between API versions that can affect feature availability. See webview API documentation for details on feature availability.

#### Windows-specific Options

Option                          | Description
------                          | -----------
`WEBVIEW_MSWEBVIEW2_VERSION`    | MS WebView2 version, e.g. `1.0.1150.38`.
`WEBVIEW_USE_BUILTIN_MSWEBVIEW2`| Use built-in MS WebView2.

### Compile-time Options

These options can be specified as preprocessor macros to modify the build, but are not needed when using CMake.

#### C API Linkage

Name                   | Description
----                   | -----------
`WEBVIEW_API`          | Controls C API linkage, symbol visibility and whether it's a shared library. By default this is `inline` for C++ and `extern` for C.
`WEBVIEW_BUILD_SHARED` | Modifies `WEBVIEW_API` for building a shared library.
`WEBVIEW_SHARED`       | Modifies `WEBVIEW_API` for using a shared library.
`WEBVIEW_STATIC`       | Modifies `WEBVIEW_API` for building or using a static library.

#### Backend Selection

Name                   | Description
----                   | -----------
`WEBVIEW_GTK`          | Compile the GTK/WebKitGTK backend.
`WEBVIEW_COCOA`        | Compile the Cocoa/WebKit backend.
`WEBVIEW_EDGE`         | Compile the Win32/WebView2 backend.

#### Windows-specific Options

Option                            | Description
------                            | -----------
`WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL` | Enables (`1`) or disables (`0`) the built-in implementation of the WebView2 loader. Enabling this avoids the need for `WebView2Loader.dll` but if the DLL is present then the DLL takes priority. This option is enabled by default.
`WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK`| Enables (`1`) or disables (`0`) explicit linking of `WebView2Loader.dll`. Enabling this avoids the need for import libraries (`*.lib`). This option is enabled by default if `WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL` is enabled.

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

[Customization options](#Customization) can be used to change how the library integrates the WebView2 loader.

## Thread Safety

Since library functions generally do not have thread safety guarantees, `webview_dispatch()` (C) / `webview::dispatch()` (C++) can be used to schedule code to execute on the main/GUI thread and thereby make that execution safe in multi-threaded applications.

`webview_return()` (C) / `webview::resolve()` (C++) uses `*dispatch()` internally and is therefore safe to call from another thread.

The main/GUI thread should be the thread that calls `webview_run()` (C) / `webview::run()` (C++).

## Development

This project uses the CMake build system.

### Development Dependencies

In addition to the dependencies mentioned earlier in this document for developing *with* the webview library, the following are used during development *of* the webview library.

* Amalgamation:
  * Python >= 3.9
* Checks:
  * `clang-format`
  * `clang-tidy`
* Documentation:
  * Doxygen
  * Graphvis

### Building

```sh
cmake -G "Ninja Multi-Config" -B build -S .
cmake --build build --config CONFIG
```

Replace `CONFIG` with one of `Debug`, `Release`, or `Profile`. Use `Profile` to enable code coverage (GCC/Clang).

Run tests:

```sh
ctest --test-dir build --build-config CONFIG
```

Generate test coverage report:

```sh
gcovr
```

Find the coverage report in `build/coverage`.

### Packaging

Run this after building the `Debug` and `Release` configs of the project:

```sh
cd build
cpack -G External -C "Debug;Release" --config CPackConfig.cmake
```

### Cross-compilation

See CMake toolchain files in the `cmake/toolchains` directory.

For example, this targets Windows x64 on Linux with POSIX threads:

```sh
cmake -G "Ninja Multi-Config" -B build -S . -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/x86_64-w64-mingw32.cmake -D WEBVIEW_TOOLCHAIN_EXECUTABLE_SUFFIX=-posix
cmake --build build --config CONFIG
```

## Limitations

### Browser Features

Since a browser engine is not a full web browser it may not support every feature you may expect from a browser. If you find that a feature does not work as expected then please consult with the browser engine's documentation and [open an issue][issues-new] if you think that the library should support it.

For example, the library does not attempt to support user interaction features like `alert()`, `confirm()` and `prompt()` and other non-essential features like `console.log()`.

## Bindings

Language    | Project
----------  | -------
Ada         | [thechampagne/webview-ada](https://github.com/thechampagne/webview-ada)
Bun         | [tr1ckydev/webview-bun](https://github.com/tr1ckydev/webview-bun)
C#          | [webview/webview_csharp](https://github.com/webview/webview_csharp)
C3          | [thechampagne/webview-c3](https://github.com/thechampagne/webview-c3)
Crystal     | [naqvis/webview](https://github.com/naqvis/webview)
D           | [thechampagne/webview-d](https://github.com/thechampagne/webview-d), [ronnie-w/webviewd](https://github.com/ronnie-w/webviewd)
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
Python      | [congzhangzh/webview_python](https://github.com/congzhangzh/webview_python),[zserge/webview-python](https://github.com/zserge/webview-python)
PHP         | [0hr/php-webview](https://github.com/0hr/php-webview)
Ruby        | [Maaarcocr/webview_ruby](https://github.com/Maaarcocr/webview_ruby)
Rust        | [Boscop/web-view](https://github.com/Boscop/web-view)
Swift       | [jakenvac/SwiftWebview](https://github.com/jakenvac/SwiftWebview)
V           | [malisipi/mui](https://github.com/malisipi/mui/tree/main/webview), [ttytm/webview](https://github.com/ttytm/webview)
Vala        | [taozuhong/webview-vala](https://github.com/taozuhong/webview-vala)
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

[examples]:          https://github.com/webview/webview/tree/master/examples
[gtk]:               https://gtk.org/
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
