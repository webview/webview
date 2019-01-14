WEBVIEW_gtk_FLAGS = -DWEBVIEW_GTK -std=c++14 -Wall -Wextra -pedantic $(shell pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0)
WEBVIEW_cocoa_FLAGS = -DWEBVIEW_COCOA -framework WebKit
WEBVIEW_mshtml_FLAGS := -DWEBVIEW_MSHTML -std=c++14 -luser32 -lole32 -loleaut32 -lcomctl32 -luuid -static
WEBVIEW_edge_FLAGS := -DWEBVIEW_EDGE

ifndef WEBVIEW_$(WEBVIEW)_FLAGS
$(error "Unknown WEBVIEW value, use WEBVIEW=gtk|cocoa|mshtml|edge")
endif

all:
	@echo "make WEBVIEW=... test - build and run tests"
	@echo "make WEBVIEW=... lint - run clang-tidy checkers"
	@echo "make WEBVIEW=... fmt	- run clang-format for all sources"

fmt: webview.h
	clang-format -i $^

lint:
	clang-tidy webview_test.cc -- $(WEBVIEW_$(WEBVIEW)_FLAGS)

test:
	$(CXX) webview_test.cc $(WEBVIEW_$(WEBVIEW)_FLAGS) -o webview_test
	./webview_test
	rm webview_test
