#!/bin/sh

set -e

DIR="$(cd "$(dirname "$0")/../" && pwd)"

if [ "$(uname)" = "Darwin" ]; then
	FLAGS="-DWEBVIEW_COCOA -std=c++11 -Wall -Wextra -pedantic -framework WebKit"
else
	FLAGS="-DWEBVIEW_GTK -std=c++11 -Wall -Wextra -pedantic $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)"
fi

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

if command -v go >/dev/null 2>&1 ; then
	echo "Running Go tests"
	CGO_ENABLED=1 go test
else
	echo "SKIP: Go tests"
fi
