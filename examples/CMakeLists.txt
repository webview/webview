find_package(Threads REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    configure_file(shared/windows/version.rc.in shared/windows/version.rc @ONLY)
    set(SHARED_SOURCES
        shared/windows/resources.rc
        ${CMAKE_CURRENT_BINARY_DIR}/shared/windows/version.rc)
endif()

add_executable(webview_example_basic_c WIN32)
target_sources(webview_example_basic_c PRIVATE basic.c ${SHARED_SOURCES})
target_link_libraries(webview_example_basic_c PRIVATE webview::static)

add_executable(webview_example_basic_cc WIN32)
target_sources(webview_example_basic_cc PRIVATE basic.cc ${SHARED_SOURCES})
target_link_libraries(webview_example_basic_cc PRIVATE webview::headers)

add_executable(webview_example_bind_c WIN32)
target_sources(webview_example_bind_c PRIVATE bind.c ${SHARED_SOURCES})
target_link_libraries(webview_example_bind_c PRIVATE webview::static Threads::Threads)

if(MSVC)
    # Disable some warnings when using MSVC
    target_compile_definitions(webview_example_bind_c PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_executable(webview_example_bind_cc WIN32)
target_sources(webview_example_bind_cc PRIVATE bind.cc ${SHARED_SOURCES})
target_link_libraries(webview_example_bind_cc PRIVATE webview::headers Threads::Threads)
