set(WEBVIEW_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
include("${WEBVIEW_CMAKE_DIR}/webview.cmake")

macro(webview_init)
    include(CheckCXXSourceCompiles)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    list(APPEND CMAKE_MODULE_PATH "${WEBVIEW_CMAKE_DIR}/modules")

    if(WEBVIEW_IS_TOP_LEVEL_BUILD)
        # CMAKE_OSX_* should be set prior to the first project() or enable_language()
        # Target macOS version
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9 CACHE STRING "")
    endif()

    enable_language(C CXX)

    webview_options()
    webview_internal_options()

    set(WEBVIEW_ROOT_DIR "${WEBVIEW_CMAKE_DIR}/../..")
    set(WEBVIEW_EXAMPLE_DIR "${WEBVIEW_ROOT_DIR}/examples")
    set(WEBVIEW_INCLUDE_DIR "${WEBVIEW_ROOT_DIR}/include")
    set(WEBVIEW_SRC_DIR "${WEBVIEW_ROOT_DIR}/src")
    set(WEBVIEW_TEST_DIR "${WEBVIEW_ROOT_DIR}/tests")

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
        set(CMAKE_C_STANDARD 99 CACHE STRING "")
        set(CMAKE_C_STANDARD_REQUIRED YES)
        set(CMAKE_C_EXTENSIONS NO)
        set(CMAKE_CXX_STANDARD 11 CACHE STRING "")
        set(CMAKE_CXX_STANDARD_REQUIRED YES)
        set(CMAKE_CXX_EXTENSIONS NO)

        # Enable compile warnings
        if(MSVC)
            add_compile_options(/W4)
        else()
            add_compile_options(-Wall -Wextra -Wpedantic)
        endif()

        # Workaround for missing "EventToken.h" (used by MS WebView2)
        if(CMAKE_SYSTEM_NAME STREQUAL Windows AND NOT MSVC)
            check_cxx_source_compiles("#include <EventToken.h>" HAVE_EVENTTOKEN_H)
            if(NOT HAVE_EVENTTOKEN_H)
                configure_file("${WEBVIEW_CMAKE_DIR}/compatibility/mingw/EventToken.h" "${CMAKE_CURRENT_BINARY_DIR}/generated/include/EventToken.h" COPYONLY)
                include_directories("${CMAKE_CURRENT_BINARY_DIR}/generated/include")
            endif()
        endif()

        # Set default build type for single-config generators
        get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
        if(NOT IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
            set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
        endif()

        # Settings used when creating an official build
        if(WEBVIEW_IS_OFFICIAL_BUILD)
            if(MSVC AND NOT CMAKE_MSVC_RUNTIME_LIBRARY)
                # Use static MSVC runtime library
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            endif()
        endif()

        webview_set_install_rpath()
    endif()

    webview_find_dependencies()
endmacro()

macro(webview_extract_version)
    file(READ "${WEBVIEW_CMAKE_DIR}/../../include/webview/webview.h" WEBVIEW_H_CONTENT)

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

macro(webview_install)
    # Install headers
    install(DIRECTORY "${WEBVIEW_INCLUDE_DIR}/webview"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT webview_development)
    install(FILES "${WEBVIEW_INCLUDE_DIR}/webview.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT webview_development)

    # Install modules
    install(DIRECTORY "${WEBVIEW_CMAKE_DIR}/modules"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/webview"
        COMPONENT webview_development)

    # Install targets
    install(TARGETS webview_core_headers EXPORT webview_targets)

    if(WEBVIEW_BUILD_SHARED_LIBRARY)
        list(APPEND WEBVIEW_INSTALL_TARGET_NAMES webview_core_shared)
    endif()
    if(WEBVIEW_BUILD_STATIC_LIBRARY)
        list(APPEND WEBVIEW_INSTALL_TARGET_NAMES webview_core_static)
    endif()

    install(TARGETS ${WEBVIEW_INSTALL_TARGET_NAMES}
        EXPORT webview_targets
        RUNTIME
            DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT webview_runtime
        LIBRARY
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT webview_runtime
            NAMELINK_COMPONENT webview_development
        ARCHIVE
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT webview_development)

    install(EXPORT webview_targets
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/webview"
        NAMESPACE webview::
        FILE webview-targets.cmake
        COMPONENT webview_development)

    export(EXPORT webview_targets FILE "${CMAKE_CURRENT_BINARY_DIR}/webview-targets.cmake")

    # Install package config
    configure_package_config_file(
        webview-config.cmake.in
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
            webview.cmake
            "${CMAKE_CURRENT_BINARY_DIR}/webview-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/webview-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/webview"
        COMPONENT webview_development)
endmacro()

macro(webview_internal_options)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(WEBVIEW_IS_TOP_LEVEL_BUILD TRUE)
    endif()

    option(WEBVIEW_BUILD_TESTS "" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_BUILD_EXAMPLES "" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_INSTALL_TARGETS "" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_BUILD_PACKAGE "" ${WEBVIEW_IS_TOP_LEVEL_BUILD})
    option(WEBVIEW_BUILD_SHARED_LIBRARY "" ON)
    option(WEBVIEW_BUILD_STATIC_LIBRARY "" ON)
    option(WEBVIEW_IS_OFFICIAL_BUILD "" OFF)
endmacro()

macro(webview_set_install_rpath)
    if(CMAKE_SYSTEM_NAME STREQUAL Darwin OR CMAKE_SYSTEM_NAME STREQUAL Linux)
        # RPATH/RUNPATH
        if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
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
