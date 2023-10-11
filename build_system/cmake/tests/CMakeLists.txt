find_package(Threads REQUIRED)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# Add core tests
add_executable(webview_core_test "${WEBVIEW_TEST_DIR}/core_test.cc")
target_link_libraries(webview_core_test PRIVATE webview_core_headers Threads::Threads)
add_test(NAME core COMMAND webview_core_test)
# Make sure the test timeout for CTest is longer than any test timeout in the test executable
set_tests_properties(core PROPERTIES TIMEOUT 61)
list(APPEND TEST_TARGETS webview_core_test)

# Add target for building all tests
add_custom_target(tests DEPENDS ${TEST_TARGETS})
