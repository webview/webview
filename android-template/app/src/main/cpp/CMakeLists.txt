cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror")

add_library(webview-android-jni SHARED
        main.cpp
        webview.h)

# Include libraries needed for hello-jni lib
target_link_libraries(webview-android-jni
        android
        log)
