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
	curl -sSLO https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
	nuget.exe install Microsoft.Web.Webview2 -Version 1.0.1150.38 -OutputDirectory .
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

mkdir "%src_dir%\dll\x86"
mkdir "%src_dir%\dll\x64"
copy  "%script_dir%\%nuget_version%\build\native\x64\WebView2Loader.dll" "%src_dir%\dll\x64"
copy  "%script_dir%\%nuget_version%\build\native\x86\WebView2Loader.dll" "%src_dir%\dll\x86"

call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x86 -host_arch=x64

echo Building webview.dll (x86)
cl /D "WEBVIEW_API=__declspec(dllexport)" ^
	/I "%script_dir%\%nuget_version%\build\native\include" ^
	"%script_dir%\%nuget_version%\build\native\x86\WebView2Loader.dll.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%"\ ^
	"%src_dir%\webview.cc" /link /DLL "/OUT:%src_dir%\dll\x86\webview.dll" || exit \b

call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -arch=x64 -host_arch=x64
echo Building webview.dll (x64)
cl /D "WEBVIEW_API=__declspec(dllexport)" ^
	/I "%script_dir%\%nuget_version%\build\native\include" ^
	"%script_dir%\%nuget_version%\build\native\x64\WebView2Loader.dll.lib" ^
	/std:c++17 /EHsc "/Fo%build_dir%"\ ^
	"%src_dir%\webview.cc" /link /DLL "/OUT:%src_dir%\dll\x64\webview.dll" || exit \b
