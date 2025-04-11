cmake_minimum_required(VERSION 3.20)

project(webview LANGUAGES CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/webview.cmake")

add_library(webview 
        src/engine_base.cpp
        src/backends/win32_edge.cpp
        src/user_script.cpp
        )
add_library(alx-home::webview ALIAS webview)

set_target_properties(webview PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(webview PROPERTIES CXX_STANDARD 26)
set_target_properties(webview PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON)
set_target_properties(webview PROPERTIES CMAKE_CXX_EXTENSIONS ON)

target_include_directories(webview INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(webview PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/webview")

target_link_libraries(webview PUBLIC ${WEBVIEW_DEPENDENCIES})
target_link_libraries(webview PRIVATE package Dwmapi alx-home::json alx-home::promise)

set(COMPILE_OPTIONS
    -std=c++2c
    "$<$<CONFIG:DEBUG>:-DDEBUG>"
    -Wall -Wextra -Wpedantic -Wcast-align -Waddress-of-packed-member
    -Wno-language-extension-token
    -ftemplate-backtrace-limit=0
    "$<$<CONFIG:Release>:-O3>"
    "$<$<CONFIG:Debug>:-O0>"
)

# set(SANITIZE "address")

if(DEFINED SANITIZE)
    list(APPEND COMPILE_OPTIONS
        -fsanitize=${SANITIZE}
    )
endif(DEFINED SANITIZE)

# if(DEFINED ADDRESS_SANITIZER)
#     list(APPEND COMPILE_OPTIONS 
#         "-DADDRESS_SANITIZER"
#         -fsanitize-recover=address
#     )
# endif(DEFINED ADDRESS_SANITIZER)

if(MSVC)
    list(TRANSFORM COMPILE_OPTIONS PREPEND "-clang:")
    target_compile_options(webview PUBLIC /W4 ${COMPILE_OPTIONS})
else()
    target_compile_options(webview PUBLIC 
        -export-dynamic
        -ggdb3 -pg -g
        ${COMPILE_OPTIONS}
        -D_GNU_SOURCE
        -Wno-psabi
    )
endif()


# set(MAJOR_VERSION 1)
# set(MINOR_VERSION 0)
# set(REVISION 0)

# set(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${REVISION})

# set_target_properties(vfrnav PROPERTIES VERSION ${MAJOR_VERSION})
# set_target_properties(vfrnav PROPERTIES SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION})