cmake_minimum_required(VERSION 3.15)
project (webview)

include_directories(${CMAKE_SOURCE_DIR})

#//////////////////////////
# sources 
#//////////////////////////

set(src ${src})
set(src ${src} ${CMAKE_SOURCE_DIR}/webview.h)

#//////////////////////////
# executable 
#//////////////////////////

add_executable(bind WIN32 ${CMAKE_SOURCE_DIR}/examples/bind.cc ${src})

#//////////////////////////
# link with libraries
#//////////////////////////

if (MSVC)
  include_directories("J:\\build\\external\\libs\\Microsoft.Web.WebView2.1.0.1150.38\\build\\native\\include")
endif()

set(lib_dep ${lib_dep})
if (MSVC)
  set(lib_dep ${lib_dep} "J:\\build\\external\\libs\\Microsoft.Web.WebView2.1.0.1150.38\\build\\native\\x64\\WebView2LoaderStatic.lib")
endif()

target_link_libraries (bind ${lib_dep})
