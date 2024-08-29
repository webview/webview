configure_file("${CMAKE_CURRENT_LIST_DIR}/version.rc.in" "resources/windows/version.rc" @ONLY)
list(APPEND SHARED_SOURCES
    "resources/windows/resources.rc"
    "${CMAKE_CURRENT_BINARY_DIR}/resources/windows/version.rc")
