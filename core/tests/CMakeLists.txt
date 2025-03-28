include(${PROJECT_SOURCE_DIR}/test_driver/cmake/discovery.cmake)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(webview_core_functional_tests)
target_sources(webview_core_functional_tests PRIVATE src/functional_tests.cc)
target_link_libraries(webview_core_functional_tests PRIVATE webview::core webview_test_driver)
webview_discover_tests(webview_core_functional_tests
    TIMEOUT 60
    TIMEOUT_AFTER_MATCH 300 "[[slow]]")

add_executable(webview_core_unit_tests)
target_sources(webview_core_unit_tests PRIVATE src/unit_tests.cc)
target_link_libraries(webview_core_unit_tests PRIVATE webview::core webview_test_driver)
webview_discover_tests(webview_core_unit_tests
    TIMEOUT 10)
