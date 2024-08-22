find_package(Threads REQUIRED)

add_executable(webview_example_bind_cc WIN32)
target_sources(webview_example_bind_cc PRIVATE src/main.cc)
target_link_libraries(webview_example_bind_cc PRIVATE webview::headers Threads::Threads)
