#!/bin/sh

set -e

DIR="$(cd "$(dirname "$0")/../" && pwd)"
FLAGS="-Wall -Wextra -pedantic -I$DIR"
CFLAGS="-std=c99 $FLAGS"

if [ "$(uname)" = "Darwin" ]; then
	CXXFLAGS="-DWEBVIEW_COCOA -std=c++11 $FLAGS -framework WebKit"
else
	CXXFLAGS="-DWEBVIEW_GTK -std=c++11 $FLAGS $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)"
fi

if command -v clang-format >/dev/null 2>&1 ; then
	echo "Formatting..."
	clang-format -i \
		"$DIR/webview.h" \
		"$DIR/webview_test.cc" \
		"$DIR/examples/"*.c \
		"$DIR/examples/"*.cc
else "$DIR/examples/basic.cc" -- $CXXFLAGS
	clang-tidy "$DIR/examples/bind.cc" -- $CXXFLAGS
	clang-tidy "$DIR/webview_test.cc" -- $CXXFLAGS
else
	echo "SKIP: Linting (clang-tidy not installed)"
fi

mkdir -p build/examples/c build/examples/cc build/examples/go || true

echo "Building C++ examples"
c++ examples/basic.cc $CXXFLAGS -o build/examples/cc/basic
c++ examples/bind.cc $CXXFLAGS -o build/examples/cc/bind

echo "Building C examples"
c++ -c 
