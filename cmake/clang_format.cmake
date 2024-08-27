file(TO_CMAKE_PATH "${BINARY_DIR}" BINARY_DIR)
file(TO_CMAKE_PATH "${SOURCE_DIR}" SOURCE_DIR)

file(GLOB_RECURSE TEMP_FILES
    ${SOURCE_DIR}/*.h
    ${SOURCE_DIR}/*.hh
    ${SOURCE_DIR}/*.hpp
    ${SOURCE_DIR}/*.hxx
    ${SOURCE_DIR}/*.c
    ${SOURCE_DIR}/*.cc
    ${SOURCE_DIR}/*.cpp
    ${SOURCE_DIR}/*.cxx)

set(FILES)
foreach(FILE IN LISTS TEMP_FILES)
    file(TO_CMAKE_PATH "${FILE}" FILE)
    if(NOT FILE MATCHES "^${BINARY_DIR}(/|$)")
        list(APPEND FILES "${FILE}")
    endif()
endforeach()

if(CMD STREQUAL "check")
    set(ARGS "--dry-run")
elseif(CMD STREQUAL "reformat")
    set(ARGS "-i")
endif()

if(STRICT)
    list(APPEND ARGS "--Werror")
endif()

execute_process(COMMAND "${CLANG_FORMAT_EXE}" ${ARGS} ${FILES}
    RESULT_VARIABLE RESULT)

if(NOT RESULT EQUAL 0 AND (STRICT OR NOT RESULT EQUAL 1))
    message(FATAL_ERROR "clang-format check failed.")
endif()
