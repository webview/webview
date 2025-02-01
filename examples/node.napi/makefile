ifeq ($(OS),Windows_NT)
    REDIRECT = >NUL 2>&1
	RMD = rm -rf
else
    REDIRECT = >/dev/null 2>&1
	RMD = rd /s /q
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
	rm compile_commands.json
	rm $(SRC_DIR)/webview.napi_wrap.cxx
	ifeq ($(OS),Windows_NT)
		$(RMD) $(SRC_DIR)/Microsoft.Web.WebView2*
		rm nuget.exe
		
	endif

swig:
	@echo $(CURDIR)
	swig -c++ -javascript -napi -o ./$(SRC_DIR)/webview.napi_wrap.cxx ./$(SRC_DIR)/webview.napi.i	

configure:
	npx node-gyp configure -- -f gyp.generator.compile_commands_json.py $(REDIRECT)
	mv $(BUILD_DIR)/Release/compile_commands.json ./compile_commands.json

	