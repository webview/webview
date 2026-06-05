# Core header library
add_library(webview_core_headers INTERFACE)
add_library(webview::core ALIAS webview_core_headers)
target_include_directories(
    webview_core_headers
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(webview_core_headers INTERFACE ${WEBVIEW_DEPENDENCIES})
# Note that we also use CMAKE_CXX_STANDARD which can override this
target_compile_features(webview_core_headers INTERFACE cxx_std_11)
set_target_properties(webview_core_headers PROPERTIES
    EXPORT_NAME core)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND WEBVIEW_USE_COMPAT_MINGW)
    target_link_libraries(webview_core_headers INTERFACE webview::compat_mingw)
endif()

# Core shared library
if(WEBVIEW_BUILD_SHARED_LIBRARY)
    add_library(webview_core_shared SHARED)
    add_library(webview::core_shared ALIAS webview_core_shared)
    target_sources(webview_core_shared PRIVATE src/webview.cc)
    target_link_libraries(webview_core_shared PUBLIC webview_core_headers)
    set_target_properties(webview_core_shared PROPERTIES
        OUTPUT_NAME webview
        VERSION "${WEBVIEW_VERSION_NUMBER}"
        SOVERSION "${WEBVIEW_VERSION_COMPATIBILITY}"
        EXPORT_NAME core_shared)
    target_compile_definitions(webview_core_shared
        INTERFACE WEBVIEW_SHARED
        PRIVATE WEBVIEW_BUILD_SHARED)
endif()

# Core static library
if(WEBVIEW_BUILD_STATIC_LIBRARY)
    # Change .lib file name for MSVC because otherwise it would be the same for shared and static
    if(MSVC)
        set(STATIC_LIBRARY_OUTPUT_NAME webview_static)
    else()
        set(STATIC_LIBRARY_OUTPUT_NAME webview)
    endif()

    add_library(webview_core_static STATIC)
    add_library(webview::core_static ALIAS webview_core_static)
    target_sources(webview_core_static PRIVATE src/webview.cc)
    target_link_libraries(webview_core_static PUBLIC webview_core_headers)
    set_target_properties(webview_core_static PROPERTIES
        OUTPUT_NAME "${STATIC_LIBRARY_OUTPUT_NAME}"
        POSITION_INDEPENDENT_CODE ON
        EXPORT_NAME core_static)
    target_compile_definitions(webview_core_static PUBLIC WEBVIEW_STATIC)
endif()

if(WEBVIEW_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(WEBVIEW_BUILD_AMALGAMATION)
    webview_find_python3(${WEBVIEW_IS_CI})
    if(Python3_FOUND)
        webview_find_clang_format(${WEBVIEW_IS_CI})
        if(WEBVIEW_CLANG_FORMAT_EXE)
            file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS include/**)
            file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/**)
            set(AMALGAMATION_STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/amalgamation/webview.h.stamp")

            add_custom_command(
                OUTPUT "${AMALGAMATION_STAMP_FILE}"
                COMMAND "${CMAKE_COMMAND}" -E touch "${AMALGAMATION_STAMP_FILE}"
                COMMAND ${Python3_EXECUTABLE}
                    "${PROJECT_SOURCE_DIR}/scripts/amalgamate/amalgamate.py"
                    --clang-format-exe "${WEBVIEW_CLANG_FORMAT_EXE}"
                    --base "${CMAKE_CURRENT_SOURCE_DIR}"
                    --search include
                    --output "${CMAKE_CURRENT_BINARY_DIR}/amalgamation/webview.h"
                    ${SOURCE_FILES}
                DEPENDS ${HEADER_FILES} ${SOURCE_FILES}
                COMMENT "Building amalgamation..."
                VERBATIM)

            add_custom_target(webview_amalgamate ALL
                DEPENDS "${AMALGAMATION_STAMP_FILE}")

            install(FILES "${CMAKE_CURRENT_BINARY_DIR}/amalgamation/webview.h"
                DESTINATION .
                COMPONENT webview_amalgamation)
        else()
            message(WARNING "Skipping amalgamation as clang-format was not found")
        endif()
    else()
        message(WARNING "Skipping amalgamation as Python 3 was not found")
    endif()
endif()
