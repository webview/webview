@echo off

echo Prepare directories...
set script_dir=%~dp0
set src_dir=%script_dir%..
set build_dir=%script_dir%..\build
set libs_dir=%src_dir%\libs
mkdir "%build_dir%"

echo Webview directory: %src_dir%
echo Build directory: %build_dir%

echo Looking for vswhere.exe...
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" set "vswhere=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" (
	echo ERROR: Failed to find vswhere.exe
	exit /b 1
)
echo Found %vswhere%

echo Looking for VC...
for /f "usebackq tokens=*" %%i in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set vc_dir=%%i
)
if not exist "%vc_dir%\Common7\Tools\vsdevcmd.bat" (
	echo ERROR: Failed to find VC tools x86/x64
	exit /b 1
)
echo Found %vc_dir%

:: 4100: unreferenced formal parameter
set warning_params=/W4 /wd4100

:: build dlls if not found
if not exist "%src_dir%\dll\x64\webview.dll" (
	mkdir "%src_dir%\dll\x86"
	mkdir "%src_dir%\dll\x64"

	call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x86 -host_arch=x64

	echo "Building webview.dll (x86)"
	cl %warning_params% ^
		/D "WEBVIEW_API=__declspec(dllexport)" ^
		/std:c++17 /EHsc "/Fo%build_dir%"\ ^
		"%src_dir%\webview.cc" /link /DLL "/OUT:%src_dir%\dll\x86\webview.dll" || exit \b

	call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x64 -host_arch=x64
	echo "Building webview.dll (x64)"
	cl %warning_params% ^
		/D "WEBVIEW_API=__declspec(dllexport)" ^
		/std:c++17 /EHsc "/Fo%build_dir%"\ ^
		"%src_dir%\webview.cc" /link /DLL "/OUT:%src_dir%\dll\x64\webview.dll" || exit \b
)
if not exist "%build_dir%\webview.dll" (
	copy "%src_dir%\dll\x64\webview.dll" %build_dir%
)

call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x64 -host_arch=x64

echo Building C++ examples (x64)
mkdir build\examples\cpp
cl %warning_params% ^
	/I "%src_dir%" ^
	"%src_dir%\dll\x64\webview.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%\examples\cpp"\ ^
	"%src_dir%\examples\basic.cc" /link "/OUT:%build_dir%\examples\cpp\basic.exe" || exit \b
cl %warning_params% ^
	/I "%src_dir%" ^
	"%src_dir%\dll\x64\webview.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%\examples\cpp"\ ^
	"%src_dir%\examples\bind.cc" /link "/OUT:%build_dir%\examples\cpp\bind.exe" || exit \b

echo Building C examples (x64)
mkdir build\examples\c
cl %warning_params% ^
	/I "%src_dir%" ^
	"%src_dir%\dll\x64\webview.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%\examples\c"\ ^
	"%src_dir%\examples\basic.c" /link "/OUT:%build_dir%\examples\c\basic.exe" || exit \b
cl %warning_params% ^
	/I "%src_dir%" ^
	"%src_dir%\dll\x64\webview.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%\examples\c"\ ^
	"%src_dir%\examples\bind.c" /link "/OUT:%build_dir%\examples\c\bind.exe" || exit \b

echo Building webview_test.exe (x64)
cl %warning_params% ^
	/utf-8 ^
	/I "%src_dir%" ^
	/std:c++17 /EHsc "/Fo%build_dir%"\ ^
	"%src_dir%\webview_test.cc" /link "/OUT:%build_dir%\webview_test.exe" || exit \b

echo Building Go examples
mkdir build\examples\go
go build -ldflags="-H windowsgui" -o build\examples\go\basic.exe examples\basic.go || exit /b
go build -ldflags="-H windowsgui" -o build\examples\go\bind.exe examples\bind.go || exit /b

echo Running tests
"%build_dir%\webview_test.exe" || exit \b

echo Running Go tests
cd /D %src_dir%
set CGO_ENABLED=1
go test || exit \b
