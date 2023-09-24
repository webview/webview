find_package(Threads REQUIRED)

add_executable(webview_example_basic_c WIN32 "${EXAMPLE_DIR}/basic.c")
add_executable(webview_example_basic_cc WIN32 "${EXAMPLE_DIR}/basic.cc")
add_executable(webview_example_bind_c WIN32 "${EXAMPLE_DIR}/bind.c")
add_executable(webview_example_bind_cc WIN32 "${EXAMPLE_DIR}/bind.cc")

target_link_libraries(webview_example_basic_c PRIVATE webview_core_static)
target_link_libraries(webview_example_basic_cc PRIVATE webview_core_headers)
target_link_libraries(webview_example_bind_c PRIVATE webview_core_static Threads::Threads)
target_link_libraries(webview_example_bind_cc PRIVATE webview_core_headers Threads::Threads)

set_target_properties(webview_example_basic_c PROPERTIES OUTPUT_NAME basic_c)
set_target_properties(webview_example_basic_cc PROPERTIES OUTPUT_NAME basic_cc)
set_target_properties(webview_example_bind_c PROPERTIES OUTPUT_NAME bind_c)
set_target_properties(webview_example_bind_cc PROPERTIES OUTPUT_NAME bind_cc)

if(MSVC)
    # Disable some warnings when using MSVC
    target_compile_definitions(webview_example_bind_c PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
