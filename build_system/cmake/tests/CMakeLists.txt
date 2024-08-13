include(${WEBVIEW_CMAKE_DIR}/test_driver/modules/discovery.cmake)

find_package(Threads REQUIRED)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# Add core tests
add_executable(webview_core_tests "${WEBVIEW_TEST_DIR}/core_tests.cc")
target_link_libraries(webview_core_tests PRIVATE webview_core_headers webview_test_driver Threads::Threads)
list(APPEND TEST_TARGETS webview_core_tests)
webview_discover_tests(webview_core_tests)

# Add target for building all tests
add_custom_target(tests DEPENDS ${TEST_TARGETS})
