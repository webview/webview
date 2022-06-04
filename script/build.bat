rem Cross-compilation:
rem The official MinGW-w64 (tested 8.1.0) does not have multilib support so a 64-bit toolchain cannot build 32-bit binaries.
rem Multilib-enabled version tested: ray_linn/gcc-10.x-with-ada

@echo off
setlocal enableextensions
setlocal enabledelayedexpansion

set option_help=
set option_clean=false
set option_build=false
set option_build-examples=false
set option_build-tests=false
set option_test=false
set option_target-arch=
set option_reformat=false
set option_lint=false
set option_go-test=false
set option_webview2-version=1.0.1150.38

call :main %*
goto :eof

:main
    call :parse_options :on_option_parsed %* || goto :eof

    if "!option_help!" == "true" (
        call :print_help
        goto :eof
    )

    set script_dir=%~dp0
    set src_dir=%script_dir%..
    set examples_dir=%src_dir%\examples
    set build_dir=%src_dir%\build
    set build_deps_dir=%build_dir%\deps
    set webview2_dir=%build_deps_dir%\microsoft.web.webview2.!option_webview2-version!

    call :set_option_overrides || goto :eof
    call :print_current_options || goto :eof

    call :is_true_string "!option_reformat!"
    if "!__result__!" == "true" call :reformat || goto :eof

    if "!option_target-arch!" == "all" (
        set build_x64=true
        set build_x86=true
    ) else if "!option_target-arch!" == "x64" (
        set build_x64=true
    ) else if "!option_target-arch!" == "x86" (
        set build_x86=true
    ) else (
        echo Invalid target architecture.
        exit /b 1
    )

    call :is_true_string "!option_build!"
    if "!__result__!" == "true" (
        call :install_deps || goto :eof
        call :find_msvc || goto :eof
    )

    if "!build_x64!" == "true" call :build_super x64 || goto :eof
    if "!build_x86!" == "true" call :build_super x86 || goto :eof

    goto :eof

:print_help
    echo This is the build script for the webview library.
    echo.
    echo Usage:
    echo     program --help
    echo     program [--clean] [--build] [--build-examples] [--build-tests]
    echo             [--test] [--target-arch=ARCH] [--reformat] [--lint]
    echo             [--webview2-version=VERSION]
    echo.
    echo Options:
    echo     --help                      Display this help text.
    echo     --clean                     Clean the build directory.
    echo     --build                     Build library.
    echo     --build-examples            Build examples (implies --build).
    echo     --build-tests               Build tests (implies --build).
    echo     --test                      Run tests (implies --build-tests).
    echo     --target-arch=ARCH          Build for the target CPU architecture.
    echo                                 Choices: all, x86, x64
    echo     --reformat                  Reformat code (requires clang-format).
    echo     --lint                      Run lint checks (requires clang-tidy).
    echo     --go-test                   Run Go tests.
    echo     --webview2-version=VERSION  WebView2 version to use.
    goto :eof

rem Print option and their current values in a human-readable way.
:print_current_options
    echo Options:
    echo   Clean build: !option_clean!
    echo   Build library: !option_build!
    echo   Build examples: !option_build-examples!
    echo   Build tests: !option_build-tests!
    echo   Run tests: !option_test!
    echo   Target architecture: !option_target-arch!
    echo   Reformat code: !option_reformat!
    echo   Run lint checks: !option_lint!
    echo   WebView2 version: !option_webview2-version!
    echo   Run Go tests: !option_go-test!
    goto :eof

rem Stores the option as a variable.
:on_option_parsed name value
    set "option_%~1=%~2"
    goto :eof

rem Overrides options if needed. For example, options can be changed conditionally.
:set_option_overrides
    rem Use specific options in the CI environment.
    if defined CI (
        set option_build-examples=true
        set option_test=true
        set option_go-test=true
    )

    rem Running tests requires building tests.
    call :is_true_string "!option_test!"
    if "!__result__!" == "true" if not "!option_build-tests!" == "true" (
        set option_build-tests=true
    )

    rem Building examples requires building library.
    call :is_true_string "!option_build-examples!"
    if "!__result__!" == "true" if not "!option_build!" == "true" (
        set option_build=true
    )

    rem Building tests requires building library.
    call :is_true_string "!option_build-tests!"
    if "!__result__!" == "true" if not "!option_build!" == "true" (
        set option_build=true
    )

    rem Set the target architecture based on the machine's architecture.
    if "!option_target-arch!" == "" (
        if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
            set option_target-arch=x64
        ) else (
            set option_target-arch=x86
        )
    )
    goto :eof

rem Reformat code.
:reformat
    echo Reformatting code...
    echo Error: Not yet implemented. & exit /b 1
    goto :eof

rem Run lint checks.
:lint
    echo Running lint checks (%arch%)...
    echo Error: Not yet implemented. & exit /b 1
    goto :eof

:build_super
    rem Keep vsdevcmd from polluting the current environment
    setlocal
    call :build %* || (endlocal & exit /b 1)
    endlocal
    goto :eof

rem All tasks related to building and testing are to be invoked here.
:build arch
    set arch=%~1
    set build_arch_dir=%build_dir%\%arch%

    call :is_true_string "!option_clean!"
    if "!__result__!" == "true" (
        echo Cleaning build directory ^(%arch%^)...
        rmdir /q /s "%build_arch_dir%" > nul
    )

    rem 4100: unreferenced formal parameter
    set warning_params=/W4 /wd4100
    set cl_params=/nologo /utf-8 %warning_params%
    set cc_params=/std:c11 %cl_params%
    set cxx_params=/std:c++17 /EHsc %cl_params%

    call :is_true_string "!option_lint!"
    if "!__result__!" == "true" call :lint || goto :eof

    call :is_true_string "!option_build!"
    if "!__result__!" == "true" (
        echo Setting up VS environment ^(%arch%^)...
        call "%vc_dir%\Common7\Tools\vsdevcmd.bat" -no_logo -arch=%arch% -host_arch=x64 || goto :eof
        call :copy_deps || goto :eof
        if not exist "%build_arch_dir%" mkdir "%build_arch_dir%" || goto :eof
        call :build_shared_library || goto :eof
    )

    call :is_true_string "!option_build-examples!"
    if "!__result__!" == "true" call :build_examples || goto :eof

    call :is_true_string "!option_build-tests!"
    if "!__result__!" == "true" call :build_tests || goto :eof

    call :is_true_string "!option_test!"
    if "!__result__!" == "true" call :run_tests || goto :eof

    call :is_true_string "!option_go-test!"
    if "!option_go-test!" == "true" call :go_run_tests || goto :eof

    goto :eof

rem Copy external dependencies into the build directory.
:copy_deps
    echo Copying dependencies (%arch%)...
    rem Copy only if needed
    robocopy "%webview2_dir%\build\native\%arch%" "%build_arch_dir%" "WebView2Loader.dll" > nul
    exit /b 0

rem Build the library.
:build_shared_library
    echo Building shared library (%arch%)...
    set output_file=%build_arch_dir%\webview.dll
    cl %cxx_params% ^
        /D "WEBVIEW_API=__declspec(dllexport)" ^
        /I "%webview2_dir%\build\native\include" ^
        "%webview2_dir%\build\native\%arch%\WebView2Loader.dll.lib" ^
        "/Fo%build_arch_dir%"\ ^
        "%src_dir%\webview.cc" /link /DLL "/OUT:%output_file%" || goto :eof
    goto :eof

rem Build examples.
:build_examples
    echo Building C++ example (%arch%)...
    set output_file=%build_arch_dir%\basic_cpp.exe
    cl %cxx_params% ^
        /I "%src_dir%" ^
        /I "%webview2_dir%\build\native\include" ^
        "%webview2_dir%\build\native\%arch%\WebView2Loader.dll.lib" ^
        "%build_arch_dir%\webview.lib" ^
        "/Fo%build_arch_dir%"\ ^
        "%examples_dir%\basic_cpp.cc" /link "/OUT:%output_file%" || goto :eof

    echo Building C example (%arch%)...
    set output_file=%build_arch_dir%\basic_c.exe
    cl %cc_params% ^
        /I "%src_dir%" ^
        "%webview2_dir%\build\native\%arch%\WebView2Loader.dll.lib" ^
        "%build_arch_dir%\webview.lib" ^
        "/Fo%build_arch_dir%"\ ^
        "%examples_dir%\basic_c.c" /link "/OUT:%output_file%" || goto :eof

    goto :eof

rem Build tests.
:build_tests
    echo Building tests (%arch%)...
    set output_file=%build_arch_dir%\webview_test.exe
    cl %cxx_params% ^
        /I "%src_dir%" ^
        /I "%webview2_dir%\build\native\include" ^
        "%webview2_dir%\build\native\%arch%\WebView2Loader.dll.lib" ^
        "%build_arch_dir%\webview.lib" ^
        "/Fo%build_arch_dir%"\ ^
        "%src_dir%\webview_test.cc" /link "/OUT:%output_file%" || goto :eof
    goto :eof

rem Run tests.
:run_tests
    echo Running tests (%arch%)...
    "%build_arch_dir%\webview_test.exe" || goto :eof
    goto :eof

rem Run Go tests.
:go_run_tests
    setlocal
    echo Running Go tests (%arch%)...
    if "%arch%" == "x64" (
        set GOARCH=amd64
    ) else if "%arch%" == "x86" (
        set GOARCH=386
    )
    set CGO_ENABLED=1
    set "PATH=%PATH%;%build_arch_dir%"
    go test || (endlocal & exit /b 1)
    endlocal
    goto :eof

rem Install dependencies.
:install_deps
    if not exist "%build_deps_dir%" mkdir "%build_deps_dir%" || goto :eof
    rem If you update the nuget package, change its version here
    echo Using Nuget Package microsoft.web.webview2.!option_webview2-version!.
    if not exist "%webview2_dir%" (
        curl -sSLO --output-dir "%build_deps_dir%" ^
            https://dist.nuget.org/win-x86-commandline/latest/nuget.exe || goto :eof
        "%build_deps_dir%\nuget.exe" install Microsoft.Web.Webview2 ^
            -Version "!option_webview2-version!" -OutputDirectory "%build_deps_dir%" || goto :eof
        echo Nuget package installed.
    )
    goto :eof

rem Find the latest installed MSVC (VC++).
:find_msvc
    echo Looking for vswhere.exe...
    set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "%vswhere%" set "vswhere=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "%vswhere%" (
        echo Error: Failed to find vswhere.exe
        exit /b 1
    )
    echo Found %vswhere%.

    echo Looking for VC...
    for /f "usebackq tokens=*" %%i in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set vc_dir=%%i
    )
    if not exist "%vc_dir%\Common7\Tools\vsdevcmd.bat" (
        echo Error: Failed to find VC tools.
        exit /b 1
    )
    echo Found %vc_dir%.
    goto :eof

rem Parses a command line option prefixed with "--".
rem Example input: --verbose "--message=hello world"
rem If the value is omitted then it will be "true".
:parse_options callback args...
    set callback=%~1
    shift
    set awaiting_value_for_option=
    for %%a in (%*) do (
        set "option=%%~a"
        if "!option:~0,2!" == "--" (
            rem If we are waiting for a value while parsing a new option then the previous option is a switch.
            if not "!awaiting_value_for_option!" == "" (
                call %callback% "!awaiting_value_for_option!" "true"
                set awaiting_value_for_option=
            )
            rem Attempts to parse the option name and value if present ("foo=bar").
            call :parse_option "!option:~2!"
            if defined option_name (
                if defined option_value (
                    call %callback% "!option_name!" "!option_value!"
                    set option_name=
                    set option_value=
                ) else (
                    rem Maybe the next parameter is the value.
                    set "awaiting_value_for_option=!option_name!"
                    set option_name=
                )
            )
        ) else if not "!awaiting_value_for_option!" == "" (
            rem If we are waiting for a value while parsing the next parameter that has no prefix then it is the value.
            rem This handles input like "--foo bar" and "--foo=bar" without quotes ("=" is treated like space).
            call %callback% "!awaiting_value_for_option!" "!option!"
            set awaiting_value_for_option=
        )
    )
    rem If we are waiting for a value while there are no more parameters to parse then the option is a switch.
    if not "!awaiting_value_for_option!" == "" (
        call %callback% "!awaiting_value_for_option!" "true"
        set awaiting_value_for_option=
    )
    goto :eof

rem Parses a key/value pair separated by "=", e.g. "foo=bar".
:parse_option option
    if "%~1" == "" (
        rem Ignore empty input
        goto :eof
    )
    set max_length=32767
    set "s=%~1"
    set /a i=0
    set option_name=
    set option_value=

:parse_option_loop
    rem Make sure there is no way to loop forever
    if !i! geq !max_length! goto :parse_option_loop_end
    set "c=!s:~%i%,1!"
    rem Reached end of input?
    if "!c!" == "" (
        if "!option_name!" == "" (
            rem Found name but no key/value separator
            set "option_name=!s:~0,%i%!"
            rem set option_value=true
        ) else if "!option_value!" == "" (
            rem Found empty value
            set option_value=
        )
        goto :parse_option_loop_end
    ) else if "!option_name!" == "" (
        if "!c!" == "=" (
            rem Found name
            set "option_name=!s:~0,%i%!"
        )
    ) else if "!option_value!" == "" (
        rem Found value
        set "option_value=!s:~%i%!"
    )
    set /a "i+=1"
    goto :parse_option_loop

:parse_option_loop_end
    goto :eof

rem Checks whether the given string is equivalent to "true"
rem Returns "true" if true; otherwise "false".
:is_true_string value
    set __result__=false
    if "%~1" == "1" set __result__=true
    if "%~1" == "true" set __result__=true
    if "%~1" == "yes" set __result__=true

rem Get the host machine/CPU architecture.
:get_host_arch
    setlocal
    if not "%PROCESSOR_ARCHITEW6432%" == "" (
        set "host_arch=%PROCESSOR_ARCHITEW6432%"
    ) else (
        set "host_arch=%PROCESSOR_ARCHITECTURE%"
    )
    if "%host_arch%" == "AMD64" (
        set __result__=x64
    ) else if "%host_arch%" == "x86" (
        set __result__=x86
    ) else (
        echo Unsupported host machine architecture.
        exit /b 1
    )
    endlocal & set "__result__=!__result__!"
    goto :eof
