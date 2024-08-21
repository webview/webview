find_package(Doxygen REQUIRED dot)

set(WEBVIEW_DOXYFILE_HAVE_DOT YES)
set(WEBVIEW_DOXYFILE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output")
set(WEBVIEW_DOXYFILE_CLANG_DATABASE_PATH "${PROJECT_BINARY_DIR}")

get_target_property(CORE_SOURCE_DIR webview::core_headers SOURCE_DIR)
set(WEBVIEW_DOXYFILE_INPUT "${CORE_SOURCE_DIR}/include")

configure_file(Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

add_custom_target(webview_api_docs
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    VERBATIM)

if(WEBVIEW_INSTALL_DOCS)
    install(DIRECTORY "${WEBVIEW_DOXYFILE_OUTPUT_DIRECTORY}/html"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        COMPONENT webview_development)
    install(DIRECTORY "${WEBVIEW_DOXYFILE_OUTPUT_DIRECTORY}/html"
        DESTINATION .
        COMPONENT webview_development_api_docs)
endif()
