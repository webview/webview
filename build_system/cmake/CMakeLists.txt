cmake_minimum_required(VERSION 3.16)

# Use the latest CMake policies the project has been validated against.
set(WEBVIEW_MAX_POLICY_VERSION 3.26)
if(CMAKE_VERSION VERSION_GREATER WEBVIEW_MAX_POLICY_VERSION)
    cmake_policy(VERSION "${WEBVIEW_MAX_POLICY_VERSION}")
else()
    cmake_policy(VERSION "${CMAKE_VERSION}")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/internal.cmake")
webview_extract_version()

project(
    webview
    VERSION "${WEBVIEW_VERSION_NUMBER}"
    DESCRIPTION "A tiny cross-platform webview library for C/C++ to build modern cross-platform GUIs."
    HOMEPAGE_URL https://github.com/webview/webview)

webview_init()

# Core header library
add_library(webview_core_headers INTERFACE)
add_library(webview::core_headers ALIAS webview_core_headers)
target_include_directories(
    webview_core_headers
    INTERFACE
        "$<BUILD_INTERFACE:${INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(webview_core_headers INTERFACE ${WEBVIEW_DEPENDENCIES})
set_target_properties(webview_core_headers PROPERTIES
    EXPORT_NAME core_headers)

# Core shared library
if(WEBVIEW_BUILD_SHARED_LIBRARY)
    add_library(webview_core_shared SHARED "${SRC_DIR}/core/webview.cc")
    add_library(webview::core_shared ALIAS webview_core_shared)
    target_link_libraries(webview_core_shared PUBLIC webview_core_headers)
    set_target_properties(webview_core_shared PROPERTIES
        OUTPUT_NAME webview
        VERSION "${WEBVIEW_VERSION_NUMBER}"
        SOVERSION "${WEBVIEW_VERSION_COMPATIBILITY}"
        EXPORT_NAME core_shared)
    target_compile_definitions(webview_core_shared
        INTERFACE WEBVIEW_SHARED
        PRIVATE WEBVIEW_BUILD_SHARED)
endif()

# Core static library
if(WEBVIEW_BUILD_STATIC_LIBRARY)
    # Change .lib file name for MSVC because otherwise it would be the same for shared and static
    if(MSVC)
        set(STATIC_LIBRARY_OUTPUT_NAME webview_static)
    else()
        set(STATIC_LIBRARY_OUTPUT_NAME webview)
    endif()

    add_library(webview_core_static STATIC "${SRC_DIR}/core/webview.cc")
    add_library(webview::core_static ALIAS webview_core_static)
    target_link_libraries(webview_core_static PUBLIC webview_core_headers)
    set_target_properties(webview_core_static PROPERTIES
        OUTPUT_NAME "${STATIC_LIBRARY_OUTPUT_NAME}"
        POSITION_INDEPENDENT_CODE ON
        EXPORT_NAME core_static)
    target_compile_definitions(webview_core_static PUBLIC WEBVIEW_STATIC)
endif()

if(WEBVIEW_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

if(WEBVIEW_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(WEBVIEW_INSTALL_TARGETS)
    webview_install()
endif()

if(WEBVIEW_BUILD_PACKAGE)
    add_subdirectory(packaging)
endif()
