#!/bin/sh

set -e

DIR="$(cd "$(dirname "$0")/../" && pwd)"
INCLUDE_DIR=$DIR
SOURCE_DIR=$DIR
BUILD_DIR=$DIR/build
BUILD_INT_DIR=$BUILD_DIR/intermediate
BUILD_BIN_DIR=$BUILD_DIR/bin
STATIC_BUILD_INT_DIR=$BUILD_DIR/intermediate/static
SHARED_BUILD_INT_DIR=$BUILD_DIR/intermediate/shared
BUILD_LIB_DIR=$BUILD_DIR/lib
STATIC_BUILD_LIB_DIR=$BUILD_LIB_DIR/static
SHARED_BUILD_LIB_DIR=$BUILD_LIB_DIR/shared
TEST_SOURCE_DIR=$DIR/test
TEST_INT_DIR=$BUILD_INT_DIR/test
TEST_BIN_DIR=$BUILD_BIN_DIR/test
EXAMPLES_SOURCE_DIR=$DIR/examples
EXAMPLES_INT_DIR=$BUILD_INT_DIR/examples
EXAMPLES_BIN_DIR=$BUILD_BIN_DIR/examples

CFLAGS="-I$INCLUDE_DIR"
CXXFLAGS="-I$INCLUDE_DIR"

if [ "$(uname)" = "Darwin" ]; then
	CXXFLAGS="$CXXFLAGS -DWEBVIEW_COCOA -std=c++11 -Wall -Wextra -pedantic -framework WebKit"
else
	CXXFLAGS="$CXXFLAGS -DWEBVIEW_GTK -std=c++11 -Wall -Wextra -pedantic $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)"
fi

if command -v clang-format >/dev/null 2>&1 ; then
	echo "Formatting..."
	# TODO: Clean up examples and remove them from the exclusion list
	find . -type f -not -name "$EXAMPLES_SOURCE_DIR" -regextype posix-extended -iregex '.*\.(h|cc?)$' -exec clang-format -i {} +
else
	echo "SKIP: Formatting (clang-format not installed)"
fi

if command -v clang-tidy >/dev/null 2>&1 ; then
	echo "Linting..."
	# TODO: Clean up examples and remove them from the exclusion list
	find . -type f -not -name "$EXAMPLES_SOURCE_DIR" -regextype posix-extended -iregex '.*\.cc$' -exec clang-tidy {} -- $CXXFLAGS \;
else
	echo "SKIP: Linting (clang-tidy not installed)"
fi

mkdir --parents "$TEST_INT_DIR" "$TEST_BIN_DIR" \
	"$STATIC_BUILD_INT_DIR" "$STATIC_BUILD_LIB_DIR" \
	"$SHARED_BUILD_INT_DIR" "$SHARED_BUILD_LIB_DIR" \
	"$EXAMPLES_INT_DIR" "$EXAMPLES_BIN_DIR"

echo "Building static library"
c++ -c "$SOURCE_DIR/webview.cc" -DWEBVIEW_BUILDING -DWEBVIEW_STATIC $CXXFLAGS -o "$STATIC_BUILD_INT_DIR/webview.o"
ar rcs "$STATIC_BUILD_LIB_DIR/libwebview.a" "$STATIC_BUILD_INT_DIR/webview.o"

echo "Building shared library"
c++ -c "$SOURCE_DIR/webview.cc" -DWEBVIEW_BUILDING -DWEBVIEW_SHARED -fPIC -fvisibility=hidden -fvisibility-inlines-hidden $CXXFLAGS -o "$SHARED_BUILD_INT_DIR/webview.o"
c++ -shared "$SHARED_BUILD_INT_DIR/webview.o" -o "$SHARED_BUILD_LIB_DIR/libwebview.so"

echo "Building C++ example using header-only library"
c++ "$EXAMPLES_SOURCE_DIR/main.cc" $CXXFLAGS -o "$EXAMPLES_BIN_DIR/cpp_example_header"

echo "Building C++ example using static library"
c++ "$EXAMPLES_SOURCE_DIR/main.cc" "-L$STATIC_BUILD_LIB_DIR" -lwebview -DWEBVIEW_STATIC $CXXFLAGS -o "$EXAMPLES_BIN_DIR/cpp_example_static"

echo "Building C example using shared library"
cc -c "$EXAMPLES_SOURCE_DIR/main.c" -DWEBVIEW_SHARED $CFLAGS -o "$EXAMPLES_INT_DIR/c_example.o"
c++ "$EXAMPLES_INT_DIR/c_example.o" "-L$SHARED_BUILD_LIB_DIR" -lwebview $CXXFLAGS -o "$EXAMPLES_BIN_DIR/c_example"

echo "Building test app"
c++ "$TEST_SOURCE_DIR/webview_test.cc" $CXXFLAGS -o "$TEST_BIN_DIR/webview_test_header"
c++ "$TEST_SOURCE_DIR/webview_test.cc" "-L$STATIC_BUILD_LIB_DIR" -lwebview -DWEBVIEW_STATIC $CXXFLAGS -o "$TEST_BIN_DIR/webview_test_static"

find "$TEST_BIN_DIR" -type f -not -name "webview_shared_library_test" | while read f; do
	echo "Running test app: $(basename "$f")"
	"$f"
done

test_shared_library() {
	echo "Building shared library test"
	c++ "$TEST_SOURCE_DIR/webview_shared_library_test.cc" "-L$SHARED_BUILD_LIB_DIR" -lwebview $CXXFLAGS -o "$TEST_BIN_DIR/webview_shared_library_test"
	echo "Running shared library tests"
	set +e
	local result
	"$TEST_BIN_DIR/webview_shared_library_test" > /dev/null 2>&1
	if [ $? != 0 ]; then result=OK; else result=FAILED; fi
	echo "Should not be able to run shared library test without shared library: $result"
	if [ "$result" != "OK" ]; then return 1; fi
	LD_LIBRARY_PATH="$SHARED_BUILD_LIB_DIR" "$TEST_BIN_DIR/webview_shared_library_test" 2>&1 | grep --silent OK
	if [ $? = 0 ]; then result=OK; else result=FAILED; fi
	echo "Should be able to run shared library test using LD_LIBRARY_PATH: $result"
	if [ "$result" != "OK" ]; then return 1; fi
}

test_shared_library

if command -v go >/dev/null 2>&1 ; then
	echo "Running Go tests"
	CGO_ENABLED=1 go test
else
	echo "SKIP: Go tests"
fi
