find_package(Threads REQUIRED)

add_executable(webview_example_bind_c WIN32)
target_sources(webview_example_bind_c PRIVATE src/main.c)
target_link_libraries(webview_example_bind_c PRIVATE webview::static Threads::Threads)

if(MSVC)
    # Disable some warnings when using MSVC
    target_compile_definitions(webview_example_bind_c PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
