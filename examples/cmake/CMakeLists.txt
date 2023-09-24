cmake_minimum_required(VERSION 3.12)
project(example LANGUAGES CXX)

find_package(webview REQUIRED)

# Require language standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_executable(example WIN32 ../basic.cc)
target_link_libraries(example PRIVATE webview::core_headers)
