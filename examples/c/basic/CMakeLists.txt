add_executable(webview_example_basic_c WIN32)
target_sources(webview_example_basic_c PRIVATE src/main.c)
target_link_libraries(webview_example_basic_c PRIVATE webview::core_static)

if(MSVC)
    # Disable some warnings when using MSVC
    target_compile_definitions(webview_example_basic_c PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
