ifeq ($(OS),Windows_NT)
    REDIRECT = >NUL 2>&1
	RM_DIR = del /s /f /q
	RM = del /f /q
	CP = copy
else
    REDIRECT = >/dev/null 2>&1
	RM_DIR = rm -rf
	RM = rm -f
	CP = cp
endif

# Directories
BUILD_DIR := build
SRC_DIR := src
DIST_DIR := dist

# Targets
.PHONY: swig clean configure build

all: swig configure

build:
	npx tsc
	npm install

clean:
	$(RM_DIR) $(BUILD_DIR)
	$(RM_DIR) $(DIST_DIR)
	$(RM) compile_commands.json
	$(RM) nuget.exe
	cd $(SRC_DIR)
	$(RM_DIR) Microsoft.Web.WebView2*
	$(RM) webview.napi_wrap.cxx
	cd ../

swig:
	swig -c++ -javascript -napi -o ./$(SRC_DIR)/webview.napi_wrap.cxx ./$(SRC_DIR)/webview.napi.i	

configure:
	npx node-gyp configure -- -f gyp.generator.compile_commands_json.py
	$(CP) $(BUILD_DIR)/Release/compile_commands.json ./compile_commands.json

	