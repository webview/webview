cmake_minimum_required(VERSION 3.16)
project(example LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include(FetchContent)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG master)
FetchContent_MakeAvailable(webview)

add_executable(basic_cc WIN32 basic.cc)
target_link_libraries(basic_cc PRIVATE webview::core_headers)

add_executable(basic_c_shared WIN32 basic.c)
target_link_libraries(basic_c_shared PRIVATE webview::core_shared)

add_executable(basic_c_static WIN32 basic.c)
target_link_libraries(basic_c_static PRIVATE webview::core_static)
