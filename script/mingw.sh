#!/bin/sh

set -e

DIR_ROOT="$(cd "$(dirname "$0")/../" && pwd)"

cd $DIR_ROOT
if [ ! -d "${DIR_ROOT}/build" ]; then
    mkdir "${DIR_ROOT}/build"
fi

# If you update the nuget package, change its version here
nuget_version="1.0.1774.30"
echo "${DIR_ROOT}/build/Microsoft.Web.WebView2.${nuget_version}"
echo -n "Check and install Nuget Package Microsoft.Web.WebView2.${nuget_version}......"
if [ ! -d "${DIR_ROOT}/build/Microsoft.Web.WebView2.${nuget_version}" ]; then
	curl -sSLO https://dist.nuget.org/win-x86-commandline/latest/nuget.exe --output-dir "${DIR_ROOT}/build"
	${DIR_ROOT}/build/nuget.exe install Microsoft.Web.Webview2 -Version ${nuget_version} -OutputDirectory "${DIR_ROOT}/build" -Verbosity quiet
	echo "[Installed]"
else
  echo "[Installed]"
fi

echo -n "Build webview object......"
g++ -c webview.cc -std=c++17 -Ibuild/Microsoft.Web.WebView2.${nuget_version}/build/native/include -o build/webview.o
echo "[done]"

echo -n "Build webview library......"
g++ -shared -D "WEBVIEW_API=__declspec(dllexport)" build/webview.o -mwindows -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion -o build/webview.dll
echo "[done]"

echo -n "Build webview example(link with dll)......"
gcc -std=c99 -I. -L./build -mwindows -lwebview -o build/basic_dll.exe examples/basic.c
echo "[done]"

echo -n "Build webview example(stand-alone)......"
gcc -c examples/basic.c -std=c99 -I. -o build/basic.o
g++ build/basic.o build/webview.o -mwindows -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion -o build/basic_std.exe
echo "[done]"
