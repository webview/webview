ifeq ($(OS),Windows_NT)
    PATH_SEP = \\
else
    PATH_SEP = /
endif

RM := rm -rf
MKDIR := mkdir -p
MV := mv

# Directories
BUILD_DIR := build
SRC_DIR := src
DIST_DIR := dist

# Targets
.PHONY: swig clean configure build

all: swig configure

build:
	npm install
	npx tsc

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(DIST_DIR)
	$(RM) compile_commands.json
	$(RM) $(SRC_DIR)/webview.napi_wrap.cxx

swig:
	swig -c++ -javascript -napi -o $(subst /,$(PATH_SEP),$(SRC_DIR)/webview.napi_wrap.cxx) $(subst /,$(PATH_SEP),$(SRC_DIR)/webview.napi.i)	

configure:
	npx node-gyp configure -- -f gyp.generator.compile_commands_json.py >/dev/null 2>&1
	$(MV) $(BUILD_DIR)/Release/compile_commands.json ./compile_commands.json

	