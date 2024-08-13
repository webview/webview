cmake_minimum_required(VERSION 3.12)
project(example LANGUAGES CXX)

find_package(webview REQUIRED)

# Require language standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Workaround for missing "EventToken.h" (used by MS WebView2)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR AND CMAKE_SYSTEM_NAME STREQUAL "Windows" AND NOT MSVC)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("#include <EventToken.h>" HAVE_EVENTTOKEN_H)
    if(NOT HAVE_EVENTTOKEN_H)
        configure_file("${CMAKE_CURRENT_LIST_DIR}/../../build_system/cmake/compatibility/mingw/EventToken.h" "${CMAKE_CURRENT_BINARY_DIR}/generated/include/EventToken.h" COPYONLY)
        include_directories("${CMAKE_CURRENT_BINARY_DIR}/generated/include")
    endif()
endif()

add_executable(example WIN32 ../basic.cc)
target_link_libraries(example PRIVATE webview::core_headers)
