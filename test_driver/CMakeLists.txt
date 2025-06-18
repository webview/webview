add_library(webview_test_driver STATIC)

target_include_directories(webview_test_driver
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_sources(webview_test_driver PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/test_driver.cc)