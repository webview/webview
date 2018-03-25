cmake_minimum_required(VERSION 2.8)

project(minimal)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/webview)
add_executable(minimal WIN32 MACOSX_BUNDLE main.c)
if (APPLE)
	target_compile_options(minimal PRIVATE "-ObjC")
endif()
target_link_libraries(minimal PUBLIC webview)
