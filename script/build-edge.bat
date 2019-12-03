@echo off
setlocal enabledelayedexpansion

@pushd %~dp0
@set script_dir=%CD%
@popd

set PROGRAMFILES32=%PROGRAMFILES(x86)%
if not exist "%PROGRAMFILES(x86)%" set PROGRAMFILES32=%PROGRAMFILES%
set VSWHERE=%PROGRAMFILES32%\Microsoft Visual Studio\Installer\vswhere.exe
if not exist "%VSWHERE%" (
	echo VS2017/VS2019 installation directory does not exist, or the vswhere.exe tool is missing.
  pause
	exit
)

set msbuild=
for /f "usebackq tokens=*" %%i in (`"%vswhere%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  set msbuild="%%i"
)
if not exist %msbuild% (
  echo Could not find MSBuild.exe. Please install the VS2017 BuildTools component or a workload that includes it.
  pause
  exit
)

%msbuild% %script_dir%/msvc/webview-dll/webview-dll.sln
pause