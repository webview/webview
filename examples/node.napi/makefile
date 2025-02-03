BUILD_DIR := build
SRC_DIR := src
DIST_DIR := dist

ifeq ($(OS),Windows_NT)
	RM_DIR = del /s /f /q
	RM = del /f /q
	COPY_COMP_COMDS = copy "$(BUILD_DIR)\Release\Debug\compile_commands.json" ".\compile_commands.json"
else
	RM_DIR = rm -rf
	RM = rm -f
	COPY_COMP_COMDS = cp $(BUILD_DIR)/Release/Debug/compile_commands.json compile_commands.json
endif

.PHONY: swig clean configure build

all: swig configure

build:
	npx tsc
	npx node-gyp build

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
	npx node-gyp configure
	$(COPY_COMP_COMDS)
	
	

	