set(WEBVIEW_CURRENT_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
include("${WEBVIEW_CURRENT_CMAKE_DIR}/webview.cmake")

# Needed when we need the project directory before calling project()
set(WEBVIEW_ROOT_DIR "${WEBVIEW_CURRENT_CMAKE_DIR}/..")

macro(webview_init)
    include(CheckCXXSourceCompiles)
    include(CMakeDependentOption)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)

    list(APPEND CMAKE_MODULE_PATH "${WEBVIEW_CURRENT_CMAKE_DIR}/modules")

    enable_language(C CXX)

    webview_options()
    webview_internal_options()

    # Version 0.x of the library can't guarantee backward compatibility.
    if(WEBVIEW_VERSION_NUMBER VERSION_LESS 1.0)
        set(WEBVIEW_VERSION_COMPATIBILITY "${WEBVIEW_VERSION_MAJOR}.${WEBVIEW_VERSION_MINOR}")
    else()
        set(WEBVIEW_VERSION_COMPATIBILITY "${WEBVIEW_VERSION_NUMBER}")
    endif()

    # Hide symbols by default
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

    # Use debug postfix to separate debug/release binaries for the library
    set(CMAKE_DEBUG_POSTFIX d)

    if(WEBVIEW_IS_TOP_LEVEL_BUILD)
        # Add custom build types
        if(CMAKE_CONFIGURATION_TYPES)
            list(APPEND CMAKE_CONFIGURATION_TYPES Profile)
            list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
            set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE)
        endif()

        # Use custom compiler/linker flags for the "Profile" build type
        if((CMAKE_C_COMPILER_ID MATCHES "((^GNU)|Clang)$") AND (CMAKE_CXX_COMPILER_ID MATCHES "((^GNU)|Clang)$"))
            set(CMAKE_C_FLAGS_PROFILE "-g -O0 -fprofile-arcs -ftest-coverage")
            set(CMAKE_CXX_FLAGS_PROFILE "-g -O0 -fprofile-arcs -ftest-coverage")
            set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-fprofile-arcs")
            set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "-fprofile-arcs")
        elseif(MSVC)
            # MSVC isn't supported but warnings are emitted if these variables are undefined
            set(CMAKE_C_FLAGS_PROFILE "/Od")
            set(CMAKE_CXX_FLAGS_PROFILE "/Od")
            set(CMAKE_EXE_LINKER_FLAGS_PROFILE "")
            set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "")
        endif()

        # Default language standards
        # All of CMAKE_<LANG>_STANDARD, CMAKE_<LANG>_REQUIRED and CMAKE_<LANG>_EXTENSIONS must be set
        # Note that we also use the cxx_std_11 compile feature for consumers
        set(CMAKE_C_STANDARD 99 CACHE STRING "")
        set(CMAKE_C_STANDARD_REQUIRED YES)
        set(CMAKE_C_EXTENSIONS NO)
        set(CMAKE_CXX_STANDARD 11 CACHE STRING "")
        set(CMAKE_CXX_STANDARD_REQUIRED YES)
        set(CMAKE_CXX_EXTENSIONS NO)

        # Enable output of compile commands
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

        # Enable compile warnings
        if(MSVC)
            add_compile_options(/W4)
        else()
            add_compile_options(-Wall -Wextra -Wpedantic)
        endif()

        # Set default build type for single-config generators
        get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
        if(NOT IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
            set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
        endif()

        if(WEBVIEW_USE_STATIC_MSVC_RUNTIME AND MSVC AND NOT CMAKE_MSVC_RUNTIME_LIBRARY)
            # Use static MSVC runtime library
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()

        webview_set_install_rpath()

        if(WEBVIEW_ENABLE_CHECKS AND WEBVIEW_ENABLE_CLANG_FORMAT)
            # Allow skipping clang-format outside of CI environment

            webview_find_clang_format(${WEBVIEW_IS_CI})

            if(WEBVIEW_CLANG_FORMAT_FOUND)
                add_custom_target(webview_format_check ALL
                    COMMAND ${CMAKE_COMMAND}
                        -D CMD=check
                        -D "BINARY_DIR=${PROJECT_BINARY_DIR}"
                        -D "SOURCE_DIR=${PROJECT_SOURCE_DIR}"
                        -D "CLANG_FORMAT_EXE=${WEBVIEW_CLANG_FORMAT_EXE}"
                        -D "STRICT=$<AND:$<BOOL:${WEBVIEW_STRICT_CHECKS}>,$<BOOL:${WEBVIEW_STRICT_CLANG_FORMAT}>>"
                        -P "${WEBVIEW_CURRENT_CMAKE_DIR}/clang_format.cmake"
                    COMMENT "Checking files with clang-format..."
                    VERBATIM)
                add_custom_target(webview_reformat
                    COMMAND ${CMAKE_COMMAND}
                        -D CMD=reformat
                        -D "BINARY_DIR=${PROJECT_BINARY_DIR}"
                        -D "SOURCE_DIR=${PROJECT_SOURCE_DIR}"
                        -D "CLANG_FORMAT_EXE=${WEBVIEW_CLANG_FORMAT_EXE}"
                        -P "${WEBVIEW_CURRENT_CMAKE_DIR}/clang_format.cmake"
                    COMMENT "Reformatting files with clang-format..."
                    VERBATIM)
            else()
                message(WARNING "Skipping clang-format checks as clang-format was not found")
            endif()
        endif()

        if(WEBVIEW_ENABLE_CHECKS AND WEBVIEW_ENABLE_CLANG_TIDY)
            if((CMAKE_C_COMPILER_ID MATCHES "Clang$") AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang$"))
                # Allow skipping clang-tidy outside of CI environment

                webview_find_clang_tidy(${WEBVIEW_IS_CI})

                if(WEBVIEW_CLANG_TIDY_FOUND)
                    set(WEBVIEW_CLANG_TIDY_ARGS)
                    if(WEBVIEW_STRICT_CHECKS AND WEBVIEW_STRICT_CLANG_TIDY)
                        list(APPEND WEBVIEW_CLANG_TIDY_ARGS "--warnings-as-errors=*")
                    endif()

                    set(CMAKE_C_CLANG_TIDY "${WEBVIEW_CLANG_TIDY_EXE}" ${WEBVIEW_CLANG_TIDY_ARGS})
                    set(CMAKE_CXX_CLANG_TIDY "${WEBVIEW_CLANG_TIDY_EXE}" ${WEBVIEW_CLANG_TIDY_ARGS})
                else()
                    message(WARNING "Skipping clang-tidy checks as clang-tidy was not found: ${WEBVIEW_CLANG_TIDY_EXE_HINT}")
                endif()
            else()
                # Skip check when clang isn't used with clang-tidy to avoid errors due to unsupported compiler flags
                # such as -fno-keep-inline-dllexport (tested GCC 14, Clang-Tidy 18)
                message(WARNING "Skipping clang-tidy checks with non-clang compiler.")
            endif()
        endif()
    endif()

    if(WEBVIEW_BUILD)
        webview_find_dependencies()
    endif()
endmacro()

function(webview_find_clang_format REQUIRED)
    if(WEBVIEW_CLANG_FORMAT_EXE)
        set(WEBVIEW_CLANG_FORMAT_FOUND TRUE PARENT_SCOPE)
        return()
    endif()
    set(CLANG_FORMAT_EXE_HINT "clang-format")
    set(FIND_ARGS WEBVIEW_CLANG_FORMAT_EXE "${CLANG_FORMAT_EXE_HINT}")
    if(REQUIRED)
        list(APPEND FIND_ARGS REQUIRED)
    endif()
    find_program(${FIND_ARGS})
    if(WEBVIEW_CLANG_FORMAT_EXE)
        set(WEBVIEW_CLANG_FORMAT_FOUND TRUE PARENT_SCOPE)
    else()
        set(WEBVIEW_CLANG_FORMAT_FOUND FALSE PARENT_SCOPE)
    endif()
endfunction()

function(webview_find_clang_tidy REQUIRED)
    if(WEBVIEW_CLANG_TIDY_EXE)
        set(WEBVIEW_CLANG_TIDY_FOUND TRUE PARENT_SCOPE)
        return()
    endif()
    # Using WEBVIEW_TOOLCHAIN_EXECUTABLE_SUFFIX here because we pair clang-tidy with the clang compiler
    set(WEBVIEW_CLANG_TIDY_EXE_HINT "clang-tidy${WEBVIEW_TOOLCHAIN_EXECUTABLE_SUFFIX}")
    set(FIND_ARGS WEBVIEW_CLANG_TIDY_EXE "${WEBVIEW_CLANG_TIDY_EXE_HINT}")
    if(REQUIRED)
        list(APPEND FIND_ARGS REQUIRED)
    endif()
    find_program(${FIND_ARGS})
    if(WEBVIEW_CLANG_TIDY_EXE)
        set(WEBVIEW_CLANG_TIDY_FOUND TRUE PARENT_SCOPE)
    else()
        set(WEBVIEW_CLANG_TIDY_FOUND FALSE PARENT_SCOPE)
    endif()
endfunction()

function(webview_find_doxygen REQUIRED)
    if(REQUIRED)
        list(APPEND FIND_ARGS REQUIRED)
    endif()
    list(APPEND FIND_ARGS COMPONENTS dot)
    find_package(Doxygen ${FIND_ARGS})
    set(Doxygen_FOUND "${Doxygen_FOUND}" PARENT_SCOPE)
    set(Doxygen_FOUND_EXECUTABLE "${Doxygen_FOUND_EXECUTABLE}" PARENT_SCOPE)
endfunction()

function(webview_find_python3 REQUIRED)
    if(REQUIRED)
        list(APPEND FIND_ARGS REQUIRED)
    endif()
    find_package(Python3 ${FIND_ARGS})
    set(Python3_FOUND "${Python3_FOUND}" PARENT_SCOPE)
    set(Python3_EXECUTABLE "${Python3_EXECUTABLE}" PARENT_SCOPE)
endfunction()

macro(webview_extract_version)
    file(READ "${WEBVIEW_ROOT_DIR}/core/include/webview/version.h" WEBVIEW_H_CONTENT)

    if(NOT DEFINED WEBVIEW_VERSION_MAJOR)
        string(REGEX MATCH "#define WEBVIEW_VERSION_MAJOR ([0-9]+)" WEBVIEW_VERSION_MAJOR_MATCH "${WEBVIEW_H_CONTENT}")
        set(WEBVIEW_VERSION_MAJOR "${CMAKE_MATCH_1}")
    endif()

    if(NOT DEFINED WEBVIEW_VERSION_MINOR)
        string(REGEX MATCH "#define WEBVIEW_VERSION_MINOR ([0-9]+)" WEBVIEW_VERSION_MINOR_MATCH "${WEBVIEW_H_CONTENT}")
        set(WEBVIEW_VERSION_MINOR "${CMAKE_MATCH_1}")
    endif()

    if(NOT DEFINED WEBVIEW_VERSION_PATCH)
        string(REGEX MATCH "#define WEBVIEW_VERSION_PATCH ([0-9]+)" WEBVIEW_VERSION_PATCH_MATCH "${WEBVIEW_H_CONTENT}")
        set(WEBVIEW_VERSION_PATCH "${CMAKE_MATCH_1}")
    endif()

    if(NOT DEFINED WEBVIEW_VERSION_PRE_RELEASE)
        string(REGEX MATCH "#define WEBVIEW_VERSION_PRE_RELEASE \"([^\"]*)\"" WEBVIEW_VERSION_PRE_RELEASE_MATCH "${WEBVIEW_H_CONTENT}")
        set(WEBVIEW_VERSION_PRE_RELEASE "${CMAKE_MATCH_1}")
    endif()

    if(NOT DEFINED WEBVIEW_VERSION_BUILD_METADATA)
        string(REGEX MATCH "#define WEBVIEW_VERSION_BUILD_METADATA \"([^\"]*)\"" WEBVIEW_VERSION_BUILD_METADATA_MATCH "${WEBVIEW_H_CONTENT}")
        set(WEBVIEW_VERSION_BUILD_METADATA "${CMAKE_MATCH_1}")
    endif()

    set(WEBVIEW_VERSION_NUMBER "${WEBVIEW_VERSION_MAJOR}.${WEBVIEW_VERSION_MINOR}.${WEBVIEW_VERSION_PATCH}")
    set(WEBVIEW_VERSION "${WEBVIEW_VERSION_NUMBER}${WEBVIEW_VERSION_PRE_RELEASE}${WEBVIEW_VERSION_BUILD_METADATA}")
endmacro()

macro(webview_install_targets)
    # Install headers
    install(DIRECTORY "${WEBVIEW_ROOT_DIR}/core/include/webview"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT webview_headers)

    # Install modules
    install(DIRECTORY "${WEBVIEW_CURRENT_CMAKE_DIR}/modules"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/webview"
        COMPONENT webview_cmake)

    # Install targets
    list(APPEND WEBVIEW_INSTALL_TARGET_NAMES webview_core_headers)

    if(WEBVIEW_BUILD_SHARED_LIBRARY)
        list(APPEND WEBVIEW_INSTALL_TARGET_NAMES webview_core_shared)
    endif()

    if(WEBVIEW_BUILD_STATIC_LIBRARY)
        list(APPEND WEBVIEW_INSTALL_TARGET_NAMES webview_core_static)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND WEBVIEW_USE_COMPAT_MINGW)
        list(APPEND WEBVIEW_INSTALL_TARGET_NAMES webview_compat_mingw)
    endif()

    install(TARGETS ${WEBVIEW_INSTALL_TARGET_NAMES}
        COMPONENT webview_libraries_runtime_release
        CONFIGURATIONS Release
        RUNTIME
            DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            NAMELINK_COMPONENT webview_trash
        ARCHIVE
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT webview_trash)

    install(TARGETS ${WEBVIEW_INSTALL_TARGET_NAMES}
        EXPORT webview_targets
        COMPONENT webview_libraries
        RUNTIME
            DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE
            DESTINATION "${CMAKE_INSTALL_LIBDIR}")

    install(EXPORT webview_targets
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/webview"
        NAMESPACE webview::
        FILE webview-targets.cmake
        COMPONENT webview_cmake)

    export(EXPORT webview_targets FILE "${CMAKE_CURRENT_BINARY_DIR}/webview-targets.cmake")

    # Install package config
    configure_package_config_file(
        "${WEBVIEW_CURRENT_CMAKE_DIR}/webview-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/webview-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/webview"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/webview-config-version.cmake"
        VERSION "${WEBVIEW_VERSION_COMPATIBILITY}"
        COMPATIBILITY SameMinorVersion)

    install(
        FILES
            "${WEBVIEW_CURRENT_CMAKE_DIR}/webview.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/webview-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/webview-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/webview"
        COMPONENT webview_cmake)
endmacro()

macro(webview_internal_options)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(WEBVIEW_IS_TOP_LEVEL_BUILD TRUE)
    endif()

    if(NOT DEFINED WEBVIEW_IS_CI)
        set(WEBVIEW_IS_CI FALSE)
        if("$ENV{CI}" MATCHES "^(1|true|TRUE)$")
            set(WEBVIEW_IS_CI TRUE)
        endif()
    endif()

    option(WEBVIEW_BUILD "Enable building" ON)
    cmake_dependent_option(WEBVIEW_BUILD_AMALGAMATION "Build amalgamated library" ON "WEBVIEW_BUILD;WEBVIEW_IS_TOP_LEVEL_BUILD" OFF)
    option(WEBVIEW_BUILD_DOCS "Build documentation" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_BUILD_TESTS "Build tests" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_BUILD_EXAMPLES "Build examples" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_INSTALL_DOCS "Install documentation" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_INSTALL_TARGETS "Install targets" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_BUILD_SHARED_LIBRARY "Build shared libraries" ON)
    option(WEBVIEW_BUILD_STATIC_LIBRARY "Build static libraries" ON)
    option(WEBVIEW_USE_COMPAT_MINGW "Use compatibility helper for MinGW" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_USE_STATIC_MSVC_RUNTIME "Use static runtime library (MSVC)" OFF)
    option(WEBVIEW_ENABLE_CHECKS "Enable checks" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_ENABLE_CLANG_FORMAT "Enable clang-format" ${WEBVIEW_ENABLE_CHECKS})
    option(WEBVIEW_ENABLE_CLANG_TIDY "Enable clang-tidy" ${WEBVIEW_ENABLE_CHECKS})
    option(WEBVIEW_ENABLE_PACKAGING "Enable packaging" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_STRICT_CHECKS "Make checks strict" ${WEBVIEW_IS_CI})
    cmake_dependent_option(WEBVIEW_PACKAGE_AMALGAMATION "Package amalgamated library" ON WEBVIEW_ENABLE_PACKAGING OFF)
    cmake_dependent_option(WEBVIEW_PACKAGE_DOCS "Package documentation" ON WEBVIEW_ENABLE_PACKAGING OFF)
    cmake_dependent_option(WEBVIEW_PACKAGE_HEADERS "Package headers" ON WEBVIEW_ENABLE_PACKAGING OFF)
    cmake_dependent_option(WEBVIEW_PACKAGE_LIB "Package compiled libraries" ON WEBVIEW_ENABLE_PACKAGING OFF)
    option(WEBVIEW_STRICT_CLANG_FORMAT "Make clang-format check strict" ${WEBVIEW_STRICT_CHECKS})
    option(WEBVIEW_STRICT_CLANG_TIDY "Make clang-tidy check strict" ${WEBVIEW_STRICT_CHECKS})
endmacro()

macro(webview_set_install_rpath)
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # RPATH/RUNPATH
        if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
            set(RPATH_BASE @loader_path)
        else()
            set(RPATH_BASE $ORIGIN)
        endif()

        file(RELATIVE_PATH RPATH_SUBDIR
            "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
        set(CMAKE_INSTALL_RPATH "${RPATH_BASE}" "${RPATH_BASE}/${RPATH_SUBDIR}")
    endif()
endmacro()
