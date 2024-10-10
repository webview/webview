# Extracts the library's version number and prints it to stdout.

include("${CMAKE_CURRENT_LIST_DIR}/internal.cmake")
webview_extract_version()
# Need this workaround because message() prints to stderr
execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${WEBVIEW_VERSION}")
