ifeq ($(OS),Windows_NT)
    RDR = >NUL 2>&1
	RMD = del /s /f /q
	RM = $(RMD)
else
    RDR = >/dev/null 2>&1
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
	$(RMD) $(BUILD_DIR)
	$(RMD) $(DIST_DIR)
	$(RMD) $(SRC_DIR)/Microsoft.Web.WebView2*
	$(RM) compile_commands.json
	$(RM) $(SRC_DIR)/webview.napi_wrap.cxx
	
	$(RM) nuget.exe

swig:
	@echo $(CURDIR)
	swig -c++ -javascript -napi -o ./$(SRC_DIR)/webview.napi_wrap.cxx ./$(SRC_DIR)/webview.napi.i	

configure:
	npx node-gyp configure -- -f gyp.generator.compile_commands_json.py $(RDR)
	mv $(BUILD_DIR)/Release/compile_commands.json ./compile_commands.json

	