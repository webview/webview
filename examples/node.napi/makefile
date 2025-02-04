BUILD_DIR := build
SRC_DIR := src
DIST_DIR := dist

ifeq ($(OS),Windows_NT)
    REDIRECT = >NUL 2>&1
	RM_DIR = del /s /f /q
	RM = del /f /q
	COPY_COMP_COMDS = copy "$(BUILD_DIR)\Release\compile_commands.json" ".\compile_commands.json"
else
    REDIRECT = >/dev/null 2>&1
	RM_DIR = rm -rf
	RM = rm -f
	COPY_COMP_COMDS = cp $(BUILD_DIR)/Release/compile_commands.json compile_commands.json
endif

.PHONY: swig clean configure build tsc nuget

all: swig tsc nuget configure

build:
	npx node-gyp build

clean:
	$(RM_DIR) $(BUILD_DIR)
	$(RM_DIR) $(DIST_DIR)
	$(RM) compile_commands.json
	$(RM) nuget.exe
	cd $(SRC_DIR) && $(RM_DIR) Microsoft.Web.WebView2*
	cd $(SRC_DIR) && $(RM) webview.napi_wrap.cxx

swig:
	swig -c++ -javascript -napi -o ./$(SRC_DIR)/webview.napi_wrap.cxx ./$(SRC_DIR)/webview.napi.i	

tsc:
	npx tsc

nuget:
	node ./dist/windows.preinstall.js

configure:
	npx node-gyp configure -- -f gyp.generator.compile_commands_json.py $(REDIRECT)
	$(COPY_COMP_COMDS)
	npx node-gyp clean
	npx node-gyp configure
	
	

	