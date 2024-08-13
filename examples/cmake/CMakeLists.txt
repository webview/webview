cmake_minimum_required(VERSION 3.16)
project(example LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG master)
FetchContent_MakeAvailable(webview)
find_package(webview REQUIRED)

add_executable(example WIN32 ../cc/basic/main.cc)
target_link_libraries(example PRIVATE webview::core_headers)
