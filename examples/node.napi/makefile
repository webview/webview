ifeq ($(OS),Windows_NT)
    REDIRECT = >NUL 2>&1
	RMD = rd /s /q
	RM = del /f /q
else
    REDIRECT = >/dev/null 2>&1
	RMD = rm -rf
	RM = rm
endif

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
	ifeq ($(OS),Windows_NT)
		if exists $(SRC_DIR)/Microsoft.Web.WebView2* $(RMD) $(SRC_DIR)/Microsoft.Web.WebView2*
		if exists nuget.exe $(RM) nuget.exe
		if exists $(BUILD_DIR) $(RMD) $(BUILD_DIR)
		if exists $(DIST_DIR) $(RMD) $(DIST_DIR)
		if exists compile_commands.json $(RM) compile_commands.json
		if exists $(SRC_DIR)/webview.napi_wrap.cxx $(RM) $(SRC_DIR)/webview.napi_wrap.cxx
	else
		$(RMD) $(BUILD_DIR)
		$(RMD) $(DIST_DIR)
		$(RM) compile_commands.json
		$(RM) $(SRC_DIR)/webview.napi_wrap.cxx
	endif
	

swig:
	@echo $(CURDIR)
	swig -c++ -javascript -napi -o ./$(SRC_DIR)/webview.napi_wrap.cxx ./$(SRC_DIR)/webview.napi.i	

configure:
	npx node-gyp configure -- -f gyp.generator.compile_commands_json.py $(REDIRECT)
	mv $(BUILD_DIR)/Release/compile_commands.json ./compile_commands.json

	