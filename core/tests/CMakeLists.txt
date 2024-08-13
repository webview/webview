include(${PROJECT_SOURCE_DIR}/test_driver/cmake/discovery.cmake)

find_package(Threads REQUIRED)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(webview_core_tests)
target_sources(webview_core_tests PRIVATE src/tests.cc)
target_link_libraries(webview_core_tests PRIVATE webview::core_headers webview_test_driver Threads::Threads)
list(APPEND TEST_TARGETS webview_core_tests)
webview_discover_tests(webview_core_tests)

# Add target for building all tests
add_custom_target(tests DEPENDS ${TEST_TARGETS})
