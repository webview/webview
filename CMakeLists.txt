cmake_minimum_required(VERSION 3.16)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/internal.cmake")
webview_extract_version()

project(
    webview
    VERSION "${WEBVIEW_VERSION_NUMBER}"
    DESCRIPTION "A tiny cross-platform webview library for C/C++ to build modern cross-platform GUIs."
    HOMEPAGE_URL https://github.com/webview/webview)

webview_init()

if(WEBVIEW_BUILD)
    add_subdirectory(compatibility)

    if(WEBVIEW_BUILD_TESTS)
        include(CTest)
        add_subdirectory(test_driver)
    endif()

    add_subdirectory(core)

    if(WEBVIEW_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()

    if(WEBVIEW_BUILD_DOCS)
        add_subdirectory(docs)
    endif()

    if(WEBVIEW_INSTALL_TARGETS)
        webview_install_targets()
    endif()

    if(WEBVIEW_ENABLE_PACKAGING)
        add_subdirectory(packaging)
    endif()
endif()
