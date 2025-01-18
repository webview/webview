if(DEFINED MSWebView2_ROOT)
    find_path(MSWebView2_INCLUDE_DIR WebView2.h
        PATHS
            "${MSWebView2_ROOT}/build/native"
            "${MSWebView2_ROOT}"
        PATH_SUFFIXES include
        NO_CMAKE_FIND_ROOT_PATH)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MSWebView2 REQUIRED_VARS MSWebView2_INCLUDE_DIR)

if(MSWebView2_FOUND)
    if(NOT TARGET MSWebView2::headers)
        add_library(MSWebView2::headers INTERFACE IMPORTED)
        set_target_properties(MSWebView2::headers PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${MSWebView2_INCLUDE_DIR}")
        target_compile_features(MSWebView2::headers INTERFACE cxx_std_14)

        if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
            set(MSWebView2_LIB ${MSWebView2_INCLUDE_DIR}/../x64/WebView2LoaderStatic.lib)
        else()
            set(MSWebView2_LIB ${MSWebView2_INCLUDE_DIR}/../x86/WebView2LoaderStatic.lib)
        endif()
        target_link_libraries(MSWebView2::headers INTERFACE ${MSWebView2_LIB})
    endif()
endif()
