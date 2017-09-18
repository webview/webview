cmake_minimum_required(VERSION 2.8)

project(timer)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/webview)
add_executable(timer main.cc)
target_link_libraries(timer PUBLIC webview pthread)
