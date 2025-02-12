include(${PROJECT_SOURCE_DIR}/test_driver/cmake/discovery.cmake)

if(MSVC)
    add_compile_options(/utf-8)
    target_include_directories(main PRIVATE "${MSPT_INCLUDE_DIR}")
else()
    find_package(Threads REQUIRED)
endif()


add_executable(webview_core_tests)
target_sources(webview_core_tests PRIVATE src/tests.cc)
target_link_libraries(webview_core_tests PRIVATE webview::core webview_test_driver pthread Threads::Threads)
webview_discover_tests(webview_core_tests)