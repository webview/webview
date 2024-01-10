@echo off
setlocal enabledelayedexpansion
goto :main

:realpath
    setlocal
    set "out_var=%~1"
    set "in_path=%~dpf2"
    endlocal & set "%out_var%=%in_path%"
    goto :eof

:dirname
    setlocal
    set "out_var=%~1"
    set "in_path=%~dp2"
    if "%in_path:~-1%" == "\" (
        set "in_path=%in_path:~0,-1%"
    )
    endlocal & set "%out_var%=%in_path%"
    goto :eof

:get_host_arch
    setlocal
    set out_var=%~1
    if defined PROCESSOR_ARCHITEW6432 (
        set "host_arch=%PROCESSOR_ARCHITEW6432%"
    ) else (
        set "host_arch=%PROCESSOR_ARCHITECTURE%"
    )
    if "%host_arch%" == "AMD64" (
        set result=x64
    ) else if "%host_arch%" == "x86" (
        set result=x86
    ) else (
        echo ERROR: Unsupported host machine architecture.
        endlocal
        exit /b 1
    )
    endlocal & set %out_var%=%result%
    goto :eof

:fetch_mswebview2
    set nuget_exe=%tools_dir%\nuget\nuget.exe
    if not exist "%nuget_exe%" (
        call :dirname nuget_dir "%nuget_exe%" || exit /b 1
        if not exist "!nuget_dir!" mkdir "!nuget_dir!" || exit /b 1
        echo Fetching NuGet...
        curl -sSLo "%nuget_exe%" https://dist.nuget.org/win-x86-commandline/latest/nuget.exe || exit /b 1
    )
    set mswebview2_dir=%libs_dir%\Microsoft.Web.WebView2.%mswebview2_version%
    if not exist "%mswebview2_dir%" (
        mkdir "%mswebview2_dir%" || exit /b 1
        echo Fetching mswebview2 %mswebview2_version%...
        "%nuget_exe%" install Microsoft.Web.Webview2 -Verbosity quiet -Version "%mswebview2_version%" -OutputDirectory "%libs_dir%" || exit /b 1
    )
    goto :eof

:is_ci
    if "%CI%" == "" exit /b 1
    goto :eof

:find_msvc
    setlocal
    set out_var=%~1
    rem Find vswhere.exe
    set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "%vswhere%" set "vswhere=!ProgramFiles!\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "%vswhere%" (
        echo ERROR: Failed to find vswhere.exe>&2
        endlocal & exit /b 1
    )
    rem Find VC tools
    for /f "usebackq tokens=*" %%i in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set vc_dir=%%i
    )
    if not exist "%vc_dir%\Common7\Tools\vsdevcmd.bat" (
        echo ERROR: Failed to find MSVC.>&2
        endlocal & exit /b 1
    )
    endlocal & set "%out_var%=%vc_dir%"
    goto :eof

:activate_msvc
    where cl.exe > nul 2>&1 && goto :eof || cmd /c exit 0
    call :find_msvc vc_dir || goto :eof
    call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -no_logo -arch=%~1 || goto :eof
    goto :eof

:task_clean
    if exist "%build_dir%" rmdir /q /s "%build_dir%" || exit /b 1
    goto :eof

:task_format
    where clang-format > nul 2>&1
    if not "%errorlevel%" == "0" (
        setlocal
        set message=Formatting ^(clang-format not installed^)
        call :is_ci && (
            echo FAIL: !message!
            endlocal
            exit /b 1
        )
        echo SKIP: !message!
        endlocal
        exit /b 0
    )
    echo Formatting...
    clang-format -i ^
        "%project_dir%\webview.h" ^
        "%project_dir%\webview_test.cc" ^
        "%project_dir%\examples/basic.c" ^
        "%project_dir%\examples/bind.c" ^
        "%project_dir%\examples/basic.cc" ^
        "%project_dir%\examples/bind.cc" ^
        || exit /b 1
    goto :eof

:task_deps
    call :fetch_mswebview2 || exit /b 1
    goto :eof

:task_check
    echo SKIP: Linting ^(not implemented^)
    goto :eof

:task_build
    call :activate_msvc "%target_arch%" || goto :eof

    if not exist "%build_dir%\library" mkdir "%build_dir%\library"

    echo Building shared library...
    "%cxx_compiler%" %cxx_compile_flags% /DWEBVIEW_BUILD_SHARED "%project_dir%\webview.cc" "/Fo%build_dir%\library"\ %cxx_link_flags% /link /DLL "/out:%build_dir%\library\webview%shared_lib_suffix%" || exit /b 1

    if not exist "%build_dir%\examples\c" mkdir "%build_dir%\examples\c"
    if not exist "%build_dir%\examples\cc" mkdir "%build_dir%\examples\cc"

    echo Building C++ examples...
    "%cxx_compiler%" %cxx_compile_flags% "%project_dir%\examples\basic.cc" "/Fo%build_dir%\examples\cc"\ %cxx_link_flags% /link "/out:%build_dir%\examples\cc\basic%exe_suffix%" || exit /b 1
    "%cxx_compiler%" %cxx_compile_flags% "%project_dir%\examples\bind.cc" "/Fo%build_dir%\examples\cc"\ %cxx_link_flags% /link "/out:%build_dir%\examples\cc\bind%exe_suffix%" || exit /b 1

    echo Building C examples...
    "%cxx_compiler%" /c %cxx_compile_flags% /DWEBVIEW_STATIC "%project_dir%\webview.cc" "/Fo%build_dir%\library\webview.obj" %cxx_link_flags% || exit /b 1
    "%c_compiler%" %c_compile_flags% "%project_dir%\examples\basic.c" "%build_dir%\library\webview.obj" "/Fo%build_dir%\examples\c"\ %c_link_flags% /link "/out:%build_dir%\examples\c\basic%exe_suffix%" || exit /b 1
    "%c_compiler%" %c_compile_flags% "%project_dir%\examples\bind.c" "%build_dir%\library\webview.obj" "/Fo%build_dir%\examples\c"\ %c_link_flags% /link "/out:%build_dir%\examples\c\bind%exe_suffix%" || exit /b 1

    echo Building test app...
    "%cxx_compiler%" %cxx_compile_flags% "%project_dir%\webview_test.cc" "/Fo%build_dir%"\ %cxx_link_flags% /link "/out:%build_dir%\webview_test%exe_suffix%" || exit /b 1
    goto :eof

:task_test
    echo Running tests...
    "%build_dir%\webview_test%exe_suffix%" || exit /b 1
    goto :eof

:task_info
    echo -- Target architecture: %target_arch%
    echo -- Build directory: %build_dir%
    echo -- C standard: %c_std%
    echo -- C compiler: %c_compiler%
    echo -- C compiler flags: %c_compile_flags%
    echo -- C linker flags: %c_link_flags%
    echo -- C++ standard: %cxx_std%
    echo -- C++ compiler: %cxx_compiler%
    echo -- C++ compiler flags: %cxx_compile_flags%
    echo -- C++ linker flags: %cxx_link_flags%
    goto :eof

:run_task
    setlocal
    set name=%~1
    set name=%name::=_%
    call :task_%name% || exit /b 1
    goto :eof

:main
rem Versions of dependencies
set mswebview2_version=1.0.1150.38

rem Target architecture for cross-compilation
call :get_host_arch host_arch || exit /b
if not defined TARGET_ARCH (
    rem Target architecture is by default the same as the host architecture
    set target_arch=%host_arch%
)

rem Default C standard unless overridden
if not defined C_STD set c_std=c11
rem Default C++ standard unless overridden
if not defined CXX_STD set cxx_std=c++14
rem Default C compiler
set c_compiler=cl
rem Default C++ compiler
set cxx_compiler=cl

call :dirname project_dir "%~dpf0" || exit /b
call :dirname project_dir "%project_dir%" || exit /b

rem Default build directory unless overridden
if defined BUILD_DIR (
    call :realpath build_dir "%BUILD_DIR%" || exit /b
) else (
    set build_dir=%project_dir%\build
)

set external_dir=%build_dir%\external
set libs_dir=%external_dir%\libs
set tools_dir=%external_dir%\tools
set warning_flags=/W4
set common_compile_flags=%warning_flags% /utf-8 /I "%project_dir%"
set common_link_flags=%warning_flags%
set c_compile_flags=%common_compile_flags%
set c_link_flags=%common_link_flags%
set cxx_compile_flags=%common_compile_flags% /EHsc
set cxx_link_flags=%common_link_flags%
set exe_suffix=.exe
set shared_lib_suffix=.dll

set c_compile_flags=%c_compile_flags% "/std:%c_std%"
set cxx_compile_flags=%cxx_compile_flags% "/std:%cxx_std%"

set cxx_compile_flags=%cxx_compile_flags% /I "%libs_dir%\Microsoft.Web.WebView2.%mswebview2_version%\build\native\include"

rem Default tasks
set tasks=info clean format deps check build test

rem Task override from command line
if not "%~1" == "" (
    set tasks=%*
)

for %%t in (%tasks%) do (
    call :run_task "%%t" || exit /b 1
)
