ifeq ($(OS),Windows_NT)
    REDIRECT = >NUL 2>&1
else
    REDIRECT = >/dev/null 2>&1
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
	ifeq ($(OS),Windows_NT)
		$(RM) nuget.exe
		$(RM) $(SRC_DIR)/Microsoft.Web.WebView2*
	endif

swig:
	@echo $(CURDIR)
	swig -c++ -javascript -napi -o ./$(SRC_DIR)/webview.napi_wrap.cxx ./$(SRC_DIR)/webview.napi.i	

configure:
	npx node-gyp configure -- -f gyp.generator.compile_commands_json.py $(REDIRECT)
	$(MV) $(BUILD_DIR)/Release/compile_commands.json ./compile_commands.json

	