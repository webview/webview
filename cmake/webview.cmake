include(FetchContent)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WEBVIEW_MSWEBVIEW2_VERSION "1.0.1587.40" CACHE STRING "MS WebView2 version")
    option(WEBVIEW_USE_BUILTIN_MSWEBVIEW2 "Use built-in MS WebView2" ON)
endif()

macro(webview_find_dependencies)
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        list(APPEND WEBVIEW_DEPENDENCIES "-framework WebKit" dl)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        find_package(PkgConfig REQUIRED)

        # List of preferred WebkitGTK modules (from most to least preferred)
        set(WEBVIEW_WEBKITGTK_PREFERRED_API_LIST webkit2gtk-4.1)
        # List of known WebkitGTK modules (from higher to lower version)
        set(WEBVIEW_WEBKITGTK_KNOWN_API_LIST webkitgtk-6.0 webkit2gtk-4.1 webkit2gtk-4.0)

        # Try to find specific WebKitGTK API
        if(NOT "${WEBVIEW_WEBKITGTK_API}" STREQUAL "")
            if(WEBVIEW_WEBKITGTK_API VERSION_EQUAL 6.0)
                pkg_check_modules(WEBVIEW_WEBKITGTK REQUIRED IMPORTED_TARGET webkitgtk-6.0)
            elseif(WEBVIEW_WEBKITGTK_API VERSION_EQUAL 4.1)
                pkg_check_modules(WEBVIEW_WEBKITGTK REQUIRED IMPORTED_TARGET webkit2gtk-4.1)
            elseif(WEBVIEW_WEBKITGTK_API VERSION_EQUAL 4.0)
                pkg_check_modules(WEBVIEW_WEBKITGTK REQUIRED IMPORTED_TARGET webkit2gtk-4.0)
            else()
                message(FATAL_ERROR "Unsupported WebKitGTK API: ${WEBVIEW_WEBKITGTK_API}")
            endif()
        endif()

        if("${WEBVIEW_WEBKITGTK_MODULE_NAME}" STREQUAL "")
            # Try to find a preferred WebKitGTK API
            pkg_search_module(WEBVIEW_WEBKITGTK IMPORTED_TARGET ${WEBVIEW_WEBKITGTK_PREFERRED_API_LIST})
            if (NOT WEBVIEW_WEBKITGTK_FOUND)
              message(STATUS "Trying to find any WebKitGTK API")
              pkg_search_module(WEBVIEW_WEBKITGTK REQUIRED IMPORTED_TARGET ${WEBVIEW_WEBKITGTK_KNOWN_API_LIST})
            endif()
        else()
            pkg_check_modules(WEBVIEW_WEBKITGTK REQUIRED IMPORTED_TARGET "${WEBVIEW_WEBKITGTK_MODULE_NAME}")
        endif()

        if("${WEBVIEW_WEBKITGTK_MODULE_NAME}" STREQUAL "webkitgtk-6.0")
            set(WEBVIEW_WEBKITGTK_API 6.0)
        elseif("${WEBVIEW_WEBKITGTK_MODULE_NAME}" STREQUAL "webkit2gtk-4.1")
            set(WEBVIEW_WEBKITGTK_API 4.1)
        elseif("${WEBVIEW_WEBKITGTK_MODULE_NAME}" STREQUAL "webkit2gtk-4.0")
            set(WEBVIEW_WEBKITGTK_API 4.0)
        else()
            message(FATAL_ERROR "Couldn't find any supported WebKitGTK API")
        endif()

        # Find matching GTK module
        if("${WEBVIEW_WEBKITGTK_API}" VERSION_GREATER_EQUAL 6.0)
            pkg_check_modules(WEBVIEW_GTK REQUIRED IMPORTED_TARGET gtk4)
        elseif("${WEBVIEW_WEBKITGTK_API}" VERSION_LESS 5.0)
            pkg_check_modules(WEBVIEW_GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
        endif()

        list(APPEND WEBVIEW_DEPENDENCIES PkgConfig::WEBVIEW_WEBKITGTK PkgConfig::WEBVIEW_GTK dl)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        if(WEBVIEW_USE_BUILTIN_MSWEBVIEW2)
            find_package(MSWebView2 QUIET)
            if(NOT MSWebView2_FOUND)
                webview_fetch_mswebview2(${WEBVIEW_MSWEBVIEW2_VERSION})
            endif()
            find_package(MSWebView2 REQUIRED)
            if(MSWebView2_FOUND)
                list(APPEND WEBVIEW_DEPENDENCIES MSWebView2::headers)
            endif()
        endif()
        list(APPEND WEBVIEW_DEPENDENCIES advapi32 ole32 shell32 shlwapi user32 version)
    endif()
endmacro()

function(webview_fetch_mswebview2 VERSION)
    cmake_policy(PUSH)
    # Avoid warning related to FetchContent and DOWNLOAD_EXTRACT_TIMESTAMP
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    FetchContent_Declare(microsoft_web_webview2
        URL "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${VERSION}"
        CONFIGURE_COMMAND "")
    FetchContent_MakeAvailable(microsoft_web_webview2)
    set(MSWebView2_ROOT "${microsoft_web_webview2_SOURCE_DIR}")
    set(MSWebView2_ROOT "${MSWebView2_ROOT}" PARENT_SCOPE)
    cmake_policy(POP)
endfunction()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
webview_find_dependencies()
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/alx-home/json
    GIT_TAG 0.12.0)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    promise
    GIT_REPOSITORY https://github.com/alx-home/promise
    GIT_TAG 0.12.0)
FetchContent_MakeAvailable(promise)
