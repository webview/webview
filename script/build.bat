@echo off

echo Prepare directories...
set script_dir=%~dp0
set src_dir=%script_dir%..
set build_dir=%script_dir%..\build
mkdir "%build_dir%"

echo Webview directory: %src_dir%
echo Build directory: %build_dir%

:: If you update the nuget package, change its version here
set nuget_version=1.0.1150.38
echo Using Nuget Package microsoft.web.webview2.%nuget_version%
if not exist "%script_dir%\microsoft.web.webview2.%nuget_version%" (
	curl -sSLO https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
	nuget.exe install Microsoft.Web.Webview2 -Version %nuget_version% -OutputDirectory %script_dir%
	echo Nuget package installed
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

:: 4100: unreferenced formal parameter
set warning_params=/W4 /wd4100

:: build dlls if not found
if not exist "%src_dir%\dll\x64\webview.dll" (
	mkdir "%src_dir%\dll\x86"
	mkdir "%src_dir%\dll\x64"
	copy  "%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\x64\WebView2Loader.dll" "%src_dir%\dll\x64"
	copy  "%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\x86\WebView2Loader.dll" "%src_dir%\dll\x86"

	call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x86 -host_arch=x64

	echo "Building webview.dll (x86)"
	cl %warning_params% ^
		/D "WEBVIEW_API=__declspec(dllexport)" ^
		/I "%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\include" ^
		"%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\x86\WebView2Loader.dll.lib" ^
		/std:c++17 /EHsc "/Fo%build_dir%"\ ^
		"%src_dir%\webview.cc" /link /DLL "/OUT:%src_dir%\dll\x86\webview.dll" || exit \b

	call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x64 -host_arch=x64
	echo "Building webview.dll (x64)"
	cl %warning_params% ^
		/D "WEBVIEW_API=__declspec(dllexport)" ^
		/I "%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\include" ^
		"%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\x64\WebView2Loader.dll.lib" ^
		/std:c++17 /EHsc "/Fo%build_dir%"\ ^
		"%src_dir%\webview.cc" /link /DLL "/OUT:%src_dir%\dll\x64\webview.dll" || exit \b
)
if not exist "%build_dir%\webview.dll" (
	copy "%src_dir%\dll\x64\webview.dll" %build_dir%
)
if not exist "%build_dir%\WebView2Loader.dll" (
	copy "%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\x64\WebView2Loader.dll" "%build_dir%"
)

call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x64 -host_arch=x64

echo Building webview.exe (x64)
cl %warning_params% ^
	/I "%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\include" ^
	"%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\x64\WebView2Loader.dll.lib" ^
	"%src_dir%\dll\x64\webview.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%"\ ^
	"%src_dir%\main.cc" /link "/OUT:%build_dir%\webview.exe" || exit \b

echo Building webview_test.exe (x64)
cl %warning_params% ^
	/I "%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\include" ^
	"%script_dir%\microsoft.web.webview2.%nuget_version%\build\native\x64\WebView2Loader.dll.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%"\ ^
	"%src_dir%\webview_test.cc" /link "/OUT:%build_dir%\webview_test.exe" || exit \b

echo Running Go tests
cd /D %src_dir%
set CGO_ENABLED=1
set "PATH=%PATH%;%src_dir%\dll\x64;%src_dir%\dll\x86"
go test || exit \b
