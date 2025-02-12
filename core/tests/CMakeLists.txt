include(${PROJECT_SOURCE_DIR}/test_driver/cmake/discovery.cmake)

add_executable(webview_core_tests)
find_package(Threads REQUIRED)

if(MSVC)
    add_compile_options(/utf-8 /MT)
endif()

target_sources(webview_core_tests PRIVATE src/tests.cc)
target_link_libraries(webview_core_tests PRIVATE webview::core webview_test_driver Threads::Threads)
webview_discover_tests(webview_core_tests)