include(${PROJECT_SOURCE_DIR}/test_driver/cmake/discovery.cmake)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(webview_core_functional_tests WIN32)
target_include_directories(webview_core_functional_tests PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include ${PROJECT_SOURCE_DIR}/test_driver/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_compile_definitions(webview_core_functional_tests PUBLIC TEST_FUNCTIONAL)
target_sources(webview_core_functional_tests PRIVATE src/main.cc src/functional_tests.cc)
target_link_libraries(webview_core_functional_tests PUBLIC webview::core webview_test_driver)
webview_discover_tests(webview_core_functional_tests
    TIMEOUT 60
    TIMEOUT_AFTER_MATCH 300 "[[slow]]")

add_executable(webview_core_unit_tests)
target_include_directories(webview_core_unit_tests PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include ${PROJECT_SOURCE_DIR}/test_driver/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")


target_sources(webview_core_unit_tests PRIVATE src/main.cc src/unit_tests.cc)
target_link_libraries(webview_core_unit_tests PUBLIC webview::core webview_test_driver)
webview_discover_tests(webview_core_unit_tests
    TIMEOUT 10)
