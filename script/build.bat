@echo off
setlocal enableextensions enabledelayedexpansion

rem Options with their default values.
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
    call :on_post_parse_options || goto :eof

    if "!option_help!" == "true" (
        call :print_help
        goto :eof
    )

    call :print_current_options || goto :eof

    set script_dir=%~dp0
    set src_dir=!script_dir!..
    set examples_dir=!src_dir!\examples
    set build_dir=!src_dir!\build
    set build_deps_dir=!build_dir!\deps
    set webview2_dir=!build_deps_dir!\microsoft.web.webview2.!option_webview2-version!

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
        echo Error: Invalid target architecture.>&2
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
    echo.
    echo Cross-compilation with Go
    echo =========================
    echo.
    echo Unless your MinGW-w64 toolchain has multilib support then you need to
    echo install both the 64-bit- and 32-bit toolchains. MinGW-w64 is expected
    echo to be found in one of the following locations:
    echo  - !SystemDrive!\mingw64 or !SystemDrive!\mingw32
    echo  - Chocolatey
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

rem Overrides options after being parsed.
:on_post_parse_options
    rem Running tests requires building tests.
    call :is_true_string "!option_test!"
    if "!__result__!" == "true" (
        call :is_true_string "!option_build-tests!"
        if not "!__result__!" == "true" set option_build-tests=true
    )

    rem Building examples requires building library.
    call :is_true_string "!option_build-examples!"
    if "!__result__!" == "true" (
        call :is_true_string "!option_build!"
        if not "!__result__!" == "true" set option_build=true
    )

    rem Building tests requires building library.
    call :is_true_string "!option_build-tests!"
    if "!__result__!" == "true" (
        call :is_true_string "!option_build!"
        if not "!__result__!" == "true" set option_build=true
    )

    rem Set the target architecture based on the machine's architecture.
    if "!option_target-arch!" == "" (
        call :get_host_arch || goto :eof
        set option_target-arch=!__result__!
    )
    goto :eof

rem Reformat code.
:reformat
    setlocal
    set file_params=
    echo Reformatting code...
    for %%f in ("!src_dir!\*.c" "!src_dir!\*.cc" "!src_dir!\*.h" "!examples_dir!\*.c" "!examples_dir!\*.cc" "!examples_dir!\*.h") do (
        set file_params=!file_params! "%%f"
    )
    clang-format -i !file_params! || (endlocal & exit /b 1)
    endlocal
    goto :eof

rem Run lint checks.
:lint
    echo Running lint checks (!arch!)...
    rem These parameters should should roughly match the parameters passed to cl.exe
    for %%f in ("!src_dir!\*.c" "!src_dir!\*.cc" "!examples_dir!\*.c" "!examples_dir!\*.cc") do (
        setlocal
        set ext=%%~xf
        set ext=!ext:~1!
        if "!ext!" == "c" (
            set std=c++11
        ) else if "!ext!" == "cc" (
            set std=c++17
        ) else (
            echo Error: Unknown file extension: !ext!>&2
            endlocal & cmd /c exit 1 & goto :lint_loop_end
        )
        clang-tidy "--config-file=!src_dir!\.clang-tidy" ^
            "--warnings-as-errors=*" ^
            "%%~f" -- ^
            "--std=!std!" -DWEBVIEW_EDGE ^
            "-I!src_dir!" ^
            "-I!webview2_dir!\build\native\include" || (endlocal & cmd /c exit 1 & goto :lint_loop_end)
        endlocal
    )
:lint_loop_end
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
    set build_arch_dir=!build_dir!\!arch!

    call :is_true_string "!option_clean!"
    if "!__result__!" == "true" (
        echo Cleaning build directory ^(!arch!^)...
        if exist "!build_arch_dir!" rmdir /q /s "!build_arch_dir!" > nul || goto :eof
    )

    rem 4100: unreferenced formal parameter
    set warning_params=/W4 /wd4100
    set cl_params=/nologo /utf-8 !warning_params! "/Fo!build_arch_dir!"\ "/I!src_dir!" ^
        "!webview2_dir!\build\native\!arch!\WebView2Loader.dll.lib"
    set cc_params=/std:c11 !cl_params!
    set cxx_params=/std:c++17 /EHsc !cl_params! /DWEBVIEW_EDGE ^
        "/I!webview2_dir!\build\native\include"

    call :is_true_string "!option_lint!"
    if "!__result__!" == "true" call :lint || goto :eof

    call :is_true_string "!option_build!"
    if "!__result__!" == "true" (
        echo Setting up VS environment ^(!arch!^)...
        call "!vc_dir!\Common7\Tools\vsdevcmd.bat" -no_logo -arch=!arch! -host_arch=x64 || goto :eof
        call :copy_deps || goto :eof
        if not exist "!build_arch_dir!" mkdir "!build_arch_dir!" || goto :eof
        call :build_shared_library || goto :eof
    )

    call :is_true_string "!option_build-examples!"
    if "!__result__!" == "true" call :build_examples || goto :eof

    call :is_true_string "!option_build-tests!"
    if "!__result__!" == "true" call :build_tests || goto :eof

    call :is_true_string "!option_test!"
    if "!__result__!" == "true" call :run_tests || goto :eof

    call :is_true_string "!option_go-test!"
    if "!__result__!" == "true" call :go_run_tests || goto :eof

    goto :eof

rem Copy external dependencies into the build directory.
:copy_deps
    echo Copying dependencies (!arch!)...
    rem Copy only if needed
    robocopy "!webview2_dir!\build\native\!arch!" "!build_arch_dir!" "WebView2Loader.dll" > nul
    exit /b 0

rem Build the library.
:build_shared_library
    echo Building shared library (!arch!)...
    cl !cxx_params! ^
        "/DWEBVIEW_API=__declspec(dllexport)" ^
        "!src_dir!\webview.cc" ^
        /link /DLL "/OUT:!build_arch_dir!\webview.dll" || goto :eof
    goto :eof

rem Build examples.
:build_examples
    for %%f in ("!examples_dir!\*.c" "!examples_dir!\*.cc") do (
        call :compile_exe "%%f" "example" || goto :build_examples_loop_end
    )
:build_examples_loop_end
    goto :eof

rem Build tests.
:build_tests
    for %%f in ("!src_dir!\*_test.c" "!src_dir!\*_test.cc") do (
        call :compile_exe "%%f" "test" || goto :build_tests_loop_end
    )
:build_tests_loop_end
    goto :eof

rem Run tests.
:run_tests
    setlocal
    set failed=false
    rem Continue even when tests fail.
    for %%f in ("!build_arch_dir!\*_test.exe") do (
        echo Running test %%~nxf ^(!arch!^)...
        cmd /c "%%~f" || set failed=true
    )
    if "!failed!" == "true" (endlocal & exit /b 1)
    endlocal
    goto :eof

rem Run Go tests.
:go_run_tests
    call :find_mingw "!arch!" || goto :eof
    setlocal
    echo Running Go tests (!arch!)...
    if "!arch!" == "x64" (
        set GOARCH=amd64
    ) else if "!arch!" == "x86" (
        set GOARCH=386
    )
    set CGO_ENABLED=1
    set "PATH=!PATH!;!build_arch_dir!"
    go test || (endlocal & exit /b 1)
    endlocal
    goto :eof

rem Compile a C/C++ file into an executable.
:compile_exe file description
    setlocal
    set file=%~1
    set name=%~n1
    set ext=%~x1
    set ext=!ext:~1!
    set description=%~2
    set output_path_excl_ext=!build_arch_dir!\!name!
    set message=Building !description! !name! (!arch!)...
    echo !message!
    if "!ext!" == "c" (
        cl !cc_params! "!build_arch_dir!\webview.lib" "!file!" ^
            /link "/OUT:!build_arch_dir!\!name!.exe" || (endlocal & goto :eof)
    ) else if "!ext!" == "cc" (
        cl !cxx_params! "!build_arch_dir!\webview.lib" "!file!" ^
            /link "/OUT:!output_path_excl_ext!.exe" || (endlocal & goto :eof)
    ) else (
        echo Error: Unknown file extension: !ext!>&2
        endlocal & exit /b 1
    )
    endlocal
    goto :eof

rem Find MinGW-w64.
:find_mingw arch
    setlocal
    set arch=%~1
    set mingw_path=
    for %%p in ("!SystemDrive!" "!ProgramData!\chocolatey\lib\mingw\tools\install") do (
        call :find_mingw_path "!arch!" "%%~p"
        if not "!__result__!" == "" (
            set mingw_path=!__result__!
            goto :find_mingw_loop_end
        )
    )
:find_mingw_loop_end
    if "!mingw_path!" == "" (
        rem Hope for the best outcome if it's invocable.
        where gcc > nul 2>&1 && (
            endlocal
            goto :eof
        )
        echo Error: Unable to find MinGW-w64.>&2
        endlocal
        exit /b 1
    )
    echo MinGW found: !mingw_path!
    set PATH=!mingw_path!\bin;!PATH!
    endlocal & set PATH=%PATH%
    goto :eof

rem Find MinGW-w64 under a specific path.
:find_mingw_path arch path_hint
    setlocal
    set arch=%~1
    set path_hint=%~2
    set mingw64_path=!path_hint!\mingw64
    set mingw32_path=!path_hint!\mingw32
    set mingw_path=
    if "!arch!" == "x64" (
        if exist "!mingw64_path!" (
            set mingw_path=!mingw64_path!
        )
    ) else if "!arch!" == "x86" (
        if exist "!mingw32_path!" (
            set mingw_path=!mingw32_path!
        ) else if exist "!mingw64_path!" (
            set mingw_path=!mingw64_path!
        )
    )
    endlocal & set __result__=%mingw_path%
    goto :eof

rem Install dependencies.
:install_deps
    if not exist "!build_deps_dir!" mkdir "!build_deps_dir!" || goto :eof
    rem If you update the nuget package, change its version here
    echo Using Nuget Package microsoft.web.webview2.!option_webview2-version!.
    if not exist "!webview2_dir!" (
        curl -sSLO --output-dir "!build_deps_dir!" ^
            https://dist.nuget.org/win-x86-commandline/latest/nuget.exe || goto :eof
        "!build_deps_dir!\nuget.exe" install Microsoft.Web.Webview2 ^
            -Version "!option_webview2-version!" -OutputDirectory "!build_deps_dir!" || goto :eof
        echo Nuget package installed.
    )
    goto :eof

rem Find the latest installed MSVC (VC++).
:find_msvc
    echo Looking for vswhere.exe...
    set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "!vswhere!" set "vswhere=!ProgramFiles!\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "!vswhere!" (
        echo Error: Failed to find vswhere.exe>&2
        exit /b 1
    )
    echo Found !vswhere!.

    echo Looking for VC...
    for /f "usebackq tokens=*" %%i in (`"!vswhere!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set vc_dir=%%i
    )
    if not exist "!vc_dir!\Common7\Tools\vsdevcmd.bat" (
        echo Error: Failed to find VC tools.>&2
        exit /b 1
    )
    echo Found !vc_dir!.
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
                call !callback! "!awaiting_value_for_option!" "true"
                set awaiting_value_for_option=
            )
            rem Attempts to parse the option name and value if present ("foo=bar").
            call :parse_option "!option:~2!"
            if defined option_name (
                if defined option_value (
                    call !callback! "!option_name!" "!option_value!"
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
            call !callback! "!awaiting_value_for_option!" "!option!"
            set awaiting_value_for_option=
        )
    )
    rem If we are waiting for a value while there are no more parameters to parse then the option is a switch.
    if not "!awaiting_value_for_option!" == "" (
        call !callback! "!awaiting_value_for_option!" "true"
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
    goto :eof

rem Get the host machine/CPU architecture.
:get_host_arch
    setlocal
    if defined PROCESSOR_ARCHITEW6432 (
        set "host_arch=!PROCESSOR_ARCHITEW6432!"
    ) else (
        set "host_arch=!PROCESSOR_ARCHITECTURE!"
    )
    if "!host_arch!" == "AMD64" (
        set __result__=x64
    ) else if "!host_arch!" == "x86" (
        set __result__=x86
    ) else (
        echo Error: Unsupported host machine architecture.>&2
        endlocal
        exit /b 1
    )
    endlocal & set __result__=%__result__%
    goto :eof
