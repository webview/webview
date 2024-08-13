add_library(webview_test_driver STATIC ${WEBVIEW_ROOT_DIR}/test_driver/src/testing.cpp)
target_include_directories(webview_test_driver PUBLIC ${WEBVIEW_ROOT_DIR}/test_driver/include)
