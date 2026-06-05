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
        target_compile_features(MSWebView2::headers INTERFACE cxx_std_11)
    endif()
endif()
