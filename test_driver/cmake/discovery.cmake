set(WEBVIEW_TEST_DISCOVERY_DIR ${CMAKE_CURRENT_LIST_DIR})

function(webview_discover_tests TARGET)
    cmake_parse_arguments(
        "TEST"
        ""
        "WORKING_DIRECTORY"
        ""
        ${ARGN}
    )

    SET(TEST_DRIVER_CONFIG_FILE_BASE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_test_driver")
    if(IS_MULTI_CONFIG)
        set(TEST_DRIVER_CONFIG_FILE "${TEST_DRIVER_CONFIG_FILE_BASE}-$<CONFIG>.cmake")
    else()
        set(TEST_DRIVER_CONFIG_FILE "${TEST_DRIVER_CONFIG_FILE_BASE}.cmake")
    endif()

    set(TEST_INCLUDE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_test_include.cmake")
    set(TEST_DRIVER_INCLUDE_FILE "${WEBVIEW_TEST_DISCOVERY_DIR}/discovery.cmake")

    add_custom_command(
        TARGET "${TARGET}" POST_BUILD
        BYPRODUCTS "${TEST_INCLUDE_FILE}" "${TEST_DRIVER_CONFIG_FILE}"
        COMMAND "${CMAKE_COMMAND}"
            -D "IS_MULTI_CONFIG=${IS_MULTI_CONFIG}"
            -D "BUILD_CONFIG=$<CONFIG>"
            -D "TARGET=${TARGET}"
            -D "TEST_DRIVER_CONFIG_FILE_BASE=${TEST_DRIVER_CONFIG_FILE_BASE}"
            -D "TEST_DRIVER=$<TARGET_FILE:${TARGET}>"
            -D "TEST_INCLUDE_FILE=${TEST_INCLUDE_FILE}"
            -D "TEST_DRIVER_CONFIG_FILE=${TEST_DRIVER_CONFIG_FILE}"
            -D "TEST_DRIVER_INCLUDE_FILE=${TEST_DRIVER_INCLUDE_FILE}"
            -P "${WEBVIEW_TEST_DISCOVERY_DIR}/generate_includes.cmake"
        WORKING_DIRECTORY "${TEST_WORKING_DIRECTORY}"
        VERBATIM
    )

    set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${TEST_INCLUDE_FILE}")
endfunction()

function(webview_discover_tests_impl TEST_DRIVER)
    execute_process(
        COMMAND "${TEST_DRIVER}" --list
        OUTPUT_VARIABLE TEST_DRIVER_OUTPUT
        RESULT_VARIABLE TEST_DRIVER_RESULT
    )
    string(STRIP "${TEST_DRIVER_OUTPUT}" TEST_DRIVER_OUTPUT)
    string(REGEX REPLACE "(;)" "\\\\\\1" TEST_LIST "${TEST_DRIVER_OUTPUT}")
    string(REGEX REPLACE "[\r\n]+" ";" TEST_LIST "${TEST_LIST}")
    foreach(TEST_LIST_ITEM IN LISTS TEST_LIST)
        add_test(
            "${TEST_LIST_ITEM}"
            "${TEST_DRIVER}" "${TEST_LIST_ITEM}"
        )
    endforeach()
endfunction()
