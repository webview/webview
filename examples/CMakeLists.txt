find_package(Threads REQUIRED)

set(COPYRIGHT "Copyright [year], Your Name")
set(VENDOR "Your Name")
set(DESCRIPTION "Example app made with webview")
set(SHARED_SOURCES)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include(resources/macos/resources.cmake)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include(resources/windows/resources.cmake)
endif()

add_executable(webview_example_basic_c MACOSX_BUNDLE WIN32)
target_sources(webview_example_basic_c PRIVATE basic.c ${SHARED_SOURCES})
target_link_libraries(webview_example_basic_c PRIVATE webview::core_static)

add_executable(webview_example_basic_cc MACOSX_BUNDLE WIN32)
target_sources(webview_example_basic_cc PRIVATE basic.cc ${SHARED_SOURCES})
target_link_libraries(webview_example_basic_cc PRIVATE webview::core)

add_executable(webview_example_bind_c MACOSX_BUNDLE WIN32)
target_sources(webview_example_bind_c PRIVATE bind.c ${SHARED_SOURCES})
target_link_libraries(webview_example_bind_c PRIVATE webview::core_static Threads::Threads)

if(MSVC)
    # Disable some warnings when using MSVC
    target_compile_definitions(webview_example_bind_c PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_executable(webview_example_bind_cc MACOSX_BUNDLE WIN32)
target_sources(webview_example_bind_cc PRIVATE bind.cc ${SHARED_SOURCES})
target_link_libraries(webview_example_bind_cc PRIVATE webview::core Threads::Threads)
