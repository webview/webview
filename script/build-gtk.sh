#!/bin/sh

set -e

DIR="$(cd "$(dirname "$0")/../" && pwd)"

FLAGS="-DWEBVIEW_GTK -std=c++11 -Wall -Wextra -pedantic $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)"

if command -v clang-format >/dev/null 2>&1 ; then
	echo "Formatting..."
	clang-format -i \
		"$DIR/webview.h" \
		"$DIR/webview_test.cc" \
		"$DIR/main.cc"
else
	echo "SKIP: Formatting (clang-format not installed)"
fi

if command -v clang-tidy >/dev/null 2>&1 ; then
	echo "Linting..."
	clang-tidy "$DIR/main.cc" -- $FLAGS
	clang-tidy "$DIR/webview_test.cc" -- $FLAGS
else
	echo "SKIP: Linting (clang-tidy not installed)"
fi

echo "Building example"
c++ main.cc $FLAGS -o webview

echo "Building test app"
c++ webview_test.cc $FLAGS -o webview_test

echo "Running tests"
./webview_test
