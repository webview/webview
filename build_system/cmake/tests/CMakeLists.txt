find_package(Threads REQUIRED)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(webview_core_test "${TEST_DIR}/core_test.cc")
target_link_libraries(webview_core_test PRIVATE webview_core_headers Threads::Threads)

include(CTest)
add_test(NAME core COMMAND webview_core_test)
