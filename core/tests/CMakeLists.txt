include(${PROJECT_SOURCE_DIR}/test_driver/cmake/discovery.cmake)

add_executable(webview_core_tests)
target_sources(webview_core_tests PRIVATE src/tests.cc)

if(MSVC)
    add_compile_options(/utf-8)
    if(NOT EXISTS ${MSPT_INCLUDE_DIR}/pthread.h)
        message(WARNING "Microsoft POSIX Thread Library (MSPT) not found. Make sure it's installed.")
    else()
        target_include_directories(webview_core_tests PRIVATE "${MSPT_INCLUDE_DIR}")
        target_link_libraries(webview_core_tests PRIVATE webview::core webview_test_driver pthread)
    endif()
else()
    find_package(Threads REQUIRED)
    target_link_libraries(webview_core_tests PRIVATE webview::core webview_test_driver Threads::Threads)
endif()

webview_discover_tests(webview_core_tests)