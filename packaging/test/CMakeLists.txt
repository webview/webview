cmake_minimum_required(VERSION 3.16)
project(example LANGUAGES C CXX)

find_package(webview REQUIRED CONFIG)

add_executable(example WIN32 ../../examples/cc/basic/src/main.cc)
target_link_libraries(example PRIVATE webview::core_headers)
