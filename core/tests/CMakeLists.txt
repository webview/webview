include(${PROJECT_SOURCE_DIR}/test_driver/cmake/discovery.cmake)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(webview_core_tests)
target_sources(webview_core_tests PRIVATE src/tests.cc)
target_link_libraries(webview_core_tests PRIVATE webview::core webview_test_driver)
webview_discover_tests(webview_core_tests)
