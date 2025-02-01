ifeq ($(OS),Windows_NT)
    RDR = >NUL 2>&1
	RDRE = 2>null
	RMD = rd /s /q
	RM = del /f /q
else
    RDR = >/dev/null 2>&1
	RDRE = 2>/dev/null
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
		$(RMD) $(SRC_DIR)/Microsoft.Web.WebView2* $(RDRE)
		$(RM) nuget.exe $(RDRE)
	endif
	$(RMD) $(BUILD_DIR) $(RDRE)
	$(RMD) $(DIST_DIR) $(RDRE)
	$(RM) compile_commands.json $(RDRE)
	$(RM) $(SRC_DIR)/webview.napi_wrap.cxx $(RDRE)	

swig:
	@echo $(CURDIR)
	swig -c++ -javascript -napi -o ./$(SRC_DIR)/webview.napi_wrap.cxx ./$(SRC_DIR)/webview.napi.i	

configure:
	npx node-gyp configure -- -f gyp.generator.compile_commands_json.py $(RDR)
	mv $(BUILD_DIR)/Release/compile_commands.json ./compile_commands.json

	