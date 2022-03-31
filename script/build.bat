@echo off

echo Prepare directories...
set script_dir=%~dp0
set src_dir=%script_dir%..
set build_dir=%script_dir%..\build
mkdir "%build_dir%"

echo Webview directory: %src_dir%
echo Build directory: %build_dir%

:: If you update the nuget package, change its version here
set nuget_version=microsoft.web.webview2.1.0.1150.38
echo Using Nuget Package %nuget_version%
if not exist "%script_dir%\%nuget_version%" (
	echo ERROR: Nuget package not found
	exit /b 1
)

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

call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x64 -host_arch=x64

if not exist "%build_dir%\WebView2Loader.dll" (
	copy "%script_dir%\%nuget_version%\build\native\x64\WebView2Loader.dll" "%build_dir%"
)

echo Building webview.exe (x64)
cl /I "%script_dir%\%nuget_version%\build\native\include" ^
	"%script_dir%\%nuget_version%\build\native\x64\WebView2Loader.dll.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%"\ ^
	"%src_dir%\main.cc" /link "/OUT:%build_dir%\webview.exe" || exit \b

echo Building webview_test.exe (x64)
cl /I "%script_dir%\%nuget_version%\build\native\include" ^
	"%script_dir%\%nuget_version%\build\native\x64\WebView2Loader.dll.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%"\ ^
	"%src_dir%\webview_test.cc" /link "/OUT:%build_dir%\webview_test.exe" || exit \b

echo Running Go tests
cd /D %src_dir%
set CGO_ENABLED=1
set "PATH=%PATH%;%src_dir%\dll\x64;%src_dir%\dll\x86"
go test || exit \b
