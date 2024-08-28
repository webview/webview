# Compatibility target for MinGW that can be used to work around missing
# "EventToken.h" header (used by MS WebView2) when targetting Windows.

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_library(webview_compat_mingw INTERFACE)
    add_library(webview::compat_mingw ALIAS webview_compat_mingw)
    set_target_properties(webview_compat_mingw PROPERTIES
        EXPORT_NAME compat_mingw)
    target_include_directories(
        webview_compat_mingw
        INTERFACE
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
endif()
