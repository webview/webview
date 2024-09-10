cmake_minimum_required(VERSION 3.16)
project(example LANGUAGES C CXX)

# Workaround for missing "EventToken.h" (used by MS WebView2) when targetting Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("#include <eventtoken.h>" HAVE_EVENTTOKEN_H)
    if(NOT HAVE_EVENTTOKEN_H)
        include_directories(../../compatibility/mingw/include)
    endif()
endif()

find_package(webview REQUIRED CONFIG)

add_executable(example WIN32 ../../examples/cc/basic/src/main.cc)
target_link_libraries(example PRIVATE webview::core)
