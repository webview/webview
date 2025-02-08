# For a use case of cross-compiling from linux to Win32 with IDE clangd intellisense compatibility
# Download or compile llvm-mingw: https://github.com/mstorsjo/llvm-mingw/releases
# Extract or compile to /usr/local/lib/llvm-mingw
# Add /usr/local/lib/llvm-mingw/bin to $PATH
# Use the compile flag `-D CMAKE_EXPORT_COMPILE_COMMANDS=1` to generate "compile_commands.json"

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(WEBVIEW_USE_COMPAT_MINGW true)
set(CMAKE_SYSROOT "/usr/local/lib/llvm-mingw")
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT}/x86_64-w64-mingw32)
set(CMAKE_C_FLAGS "--target=x86_64-w64-windows-gnu -fuse-ld=lld")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_C_COMPILER x86_64-w64-mingw32-clang)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-clang++)
set(CMAKE_RANLIB x86_64-w64-mingw32-ranlib)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)


set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)