@echo off
setlocal enableextensions enabledelayedexpansion

rem Options with their default values.
set option_help=
set option_clean=false
set option_build=false
set option_build_examples=false
set option_build_tests=false
set option_test=false
set option_target_arch=
set option_reformat=false
set option_lint=false
set option_go_build_examples=false
set option_go_test=false
set option_webview2_version=1.0.1150.38
set option_toolchain=msvc
set option_fetch_deps=false
set option_cc=gcc
set option_cxx=g++

call :main %*
goto :eof

:main
    if "%~1" == "" (
        call :print_help
        goto :eof
    )

    call :parse_options :on_option_parsed %* || goto :eof
    call :on_post_parse_options || goto :eof

    call :is_true_string "!option_help!"
    if "!__result__!" == "true" (
        call :print_help
        goto :eof
    )

    call :print_current_options || goto :eof

    set script_dir=%~dp0
    set src_dir=!script_dir!..
    set examples_dir=!src_dir!\examples
    set build_dir=!src_dir!\build
    set build_deps_dir=!build_dir!\deps
    set webview2_dir=!build_deps_dir!\microsoft.web.webview2.!option_webview2_version!

    call :is_true_string "!option_reformat!"
    if "!__result__!" == "true" call :reformat || goto :eof

    call :is_true_string "!option_fetch_deps!"
    if "!__result__!" == "true" (
        call :fetch_deps || goto :eof
    )

    if "!option_target_arch!" == "all" (
        set build_x64=true
        set build_x86=true
    ) else if "!option_target_arch!" == "x64" (
        set build_x64=true
    ) else if "!option_target_arch!" == "x86" (
        set build_x86=true
    ) else (
        echo Error: Invalid target architecture.>&2
        exit /b 1
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
    echo             [--test] [--target-arch=ARCH] [--reformat] [--lint[=lax]]
    echo             [--cc=PATH] [--cxx=PATH] [--go-build-examples] [--go-test]
    echo             [--fetch-deps] [--toolchain=TOOLCHAIN] [--webview2-version=VERSION]
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
    echo     --lint=lax                  Run lint checks in lax mode.
    echo     --cc                        C compiler binary, e.g. cc, gcc or clang.
    echo                                 Can be set by the CC environment variable.
    echo                                 Currently only supported with MinGW.
    echo     --cxx                       C++ compiler binary, e.g. c++, g++ or clang++.
    echo                                 Currently only supported with MinGW.
    echo                                 Can be set by the CXX environment variable.
    echo     --go-build-examples         Build Go examples.
    echo     --go-test                   Run Go tests (implies --fetch-deps).
    echo     --fetch-deps                Fetch library dependencies.
    echo                                 Implied when building and linting.
    echo     --toolchain=TOOLCHAIN       C/C++ toolchain.
    echo                                 Choices: msvc, mingw.
    echo     --webview2-version=VERSION  WebView2 version to use.
    echo.
    echo Compilation with MinGW-w64
    echo ==========================
    echo.
    echo Unless your MinGW-w64 toolchain has multilib support then you need to
    echo install both the 64-bit- and 32-bit toolchains for cross-compilation.
    echo MinGW-w64 is expected to be found in one of the following locations:
    echo  - !SystemDrive!\mingw64 or !SystemDrive!\mingw32
    echo  - !SystemDrive!\msys64\mingw64 or !SystemDrive!\msys64\mingw32
    echo  - Chocolatey
    goto :eof

rem Print option and their current values in a human-readable way.
:print_current_options
    echo Options:
    echo   Clean build: !option_clean!
    echo   Build library: !option_build!
    echo   Build examples: !option_build_examples!
    echo   Build tests: !option_build_tests!
    echo   Run tests: !option_test!
    echo   Target architecture: !option_target_arch!
    echo   Reformat code: !option_reformat!
    echo   Run lint checks: !option_lint!
    echo   C compiler: !option_cc!
    echo   C++ compiler: !option_cxx!
    echo   Build Go examples: !option_go_build_examples!
    echo   Run Go tests: !option_go_test!
    echo   Fetch dependencies: !option_fetch_deps!
    echo   Toolchain: !option_toolchain!
    echo   WebView2 version: !option_webview2_version!
    goto :eof

rem Stores the option as a variable.
:on_option_parsed name value
    call :sanetize_option_name "%~1" || goto :eof
    set "option_!__result__!=%~2"
    set "_option_set_explicitly_!__result__!=true"
    goto :eof

rem Overrides and validates options after being parsed.
rem Make sure to allow the user to override options that are being set here.
:on_post_parse_options
    rem Running tests requires building tests.
    call :is_true_string "!option_test!" || goto :eof
    if "!__result__!" == "true" (
        call :is_option_set_explicitly build_tests || goto :eof
        if not "!__result__!" == "true" (
            set option_build_tests=true
        )
    )

    rem Running Go tests requires fetching dependencies.
    call :is_true_string "!option_go_test!" || goto :eof
    if "!__result__!" == "true" (
        call :is_option_set_explicitly fetch_deps || goto :eof
        if not "!__result__!" == "true" (
            set option_fetch_deps=true
        )
    )

    rem Building examples requires building library.
    call :is_true_string "!option_build_examples!" || goto :eof
    if "!__result__!" == "true" (
        call :is_option_set_explicitly build || goto :eof
        if not "!__result__!" == "true" (
            set option_build=true
        )
    )

    rem Building tests requires building library.
    call :is_true_string "!option_build_tests!" || goto :eof
    if "!__result__!" == "true" (
        call :is_option_set_explicitly build || goto :eof
        if not "!__result__!" == "true" (
            set option_build=true
        )
    )

    rem Building the requires fetching dependencies.
    call :is_true_string "!option_build!" || goto :eof
    if "!__result__!" == "true" (
        call :is_option_set_explicitly fetch_deps || goto :eof
        if not "!__result__!" == "true" (
            set option_fetch_deps=true
        )
    )

    rem Lint check with clang-tidy requires fetching dependencies.
    call :is_false_string "!option_lint!" || goto :eof
    if not "!__result__!" == "true" (
        call :is_true_string "!option_lint!" || goto :eof
        if not "!__result__!" == "true" if not "!option_lint!" == "lax" (
            echo Error: Invalid lint option: !option_lint!>&2
            exit /b 1
        )
        call :is_option_set_explicitly fetch_deps || goto :eof
        if not "!__result__!" == "true" (
            set option_fetch_deps=true
        )
    )

    rem Set the target architecture based on the machine's architecture.
    if "!option_target_arch!" == "" (
        call :is_option_set_explicitly target_arch || goto :eof
        if not "!__result__!" == "true" (
            call :get_host_arch || goto :eof
            set option_target_arch=!__result__!
        )
    )

    rem Set the C compiler binary based on the environment.
    call :is_option_set_explicitly cc || goto :eof
    if not "!__result__!" == "true" if not "!CC!" == "" (
        set option_cc=!CC!
    )

    rem Set the C++ compiler binary based on the environment.
    call :is_option_set_explicitly cxx || goto :eof
    if not "!__result__!" == "true" if not "!CXX!" == "" (
        set option_cxx=!CXX!
    )

    rem Validate toolchain.
    if not "!option_toolchain!" == "msvc" if not "!option_toolchain!" == "mingw" (
        echo Error: Invalid toolchain: !option_toolchain!>&2
        exit /b 1
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
    set strict_params=--warnings-as-errors=*
    if "!option_lint!" == "lax" set strict_params=
    rem Specify target architecture only if it differs from the host architecture.
    set arch_param=
    call :get_host_arch || goto :eof
    if not "!__result__!" == "!arch!" (
        if "!arch!" == "x64" (
            set arch_param=-m64
        ) else if "!arch!" == "x86" (
            set arch_param=-m32
        )
    )
    rem These parameters should should roughly match the parameters passed to cl.exe
    for %%f in ("!src_dir!\*.c" "!src_dir!\*.cc" "!examples_dir!\*.c" "!examples_dir!\*.cc") do (
        echo Checking %%~f...
        setlocal
        set ext=%%~xf
        set ext=!ext:~1!
        if "!ext!" == "c" (
            set std=c11
        ) else if "!ext!" == "cc" (
            set std=c++17
        ) else (
            echo Error: Unknown file extension: !ext!>&2
            endlocal & cmd /c exit 1 & goto :lint_loop_end
        )
        clang-tidy !strict_params! ^
            "%%~f" -- ^
            "--std=!std!" !arch_param! -DWEBVIEW_EDGE ^
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

    if "!option_toolchain!" == "msvc" (
        rem 4100: unreferenced formal parameter
        set warning_params=/W4 /wd4100
        set common_params=/nologo /utf-8 !warning_params! ^
            "/I!src_dir!"
        set link_params="!webview2_dir!\build\native\!arch!\WebView2Loader.dll.lib"
        set cc_params=/std:c11
        set cxx_params=/std:c++17 /EHsc /DWEBVIEW_EDGE ^
            "/I!webview2_dir!\build\native\include"
    ) else if "!option_toolchain!" == "mingw" (
        set warning_params=-Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-unused-parameter -Wno-cast-function-type
        set common_params= !warning_params! ^
            "-I!src_dir!" "-I!webview2_dir!\build\native\include"
        set link_params="-L!webview2_dir!\build\native\!arch!" -lWebView2Loader.dll -lole32 -lshell32 -lshlwapi -luser32
        set cc_params=--std=c11
        set cxx_params=--std=c++17 -DWEBVIEW_EDGE

        rem Specify target architecture only if it differs from the host architecture.
        call :get_host_arch || goto :eof
        if not "!__result__!" == "!arch!" (
            if "!arch!" == "x64" (
                set common_params=!common_params! -m64
            ) else if "!arch!" == "x86" (
                set common_params=!common_params! -m32
            )
        )
    )

    rem We should copy runtime dependencies to the build directory if we are building or running tests.
    set copy_deps_to_build_dir=false
    call :is_true_string "!option_build!"
    if "!__result__!" == "true" set copy_deps_to_build_dir=true
    call :is_true_string "!option_test!"
    if "!__result__!" == "true" set copy_deps_to_build_dir=true
    call :is_true_string "!option_go_build_examples!"
    if "!__result__!" == "true" set copy_deps_to_build_dir=true
    call :is_true_string "!option_go_test!"
    if "!__result__!" == "true" set copy_deps_to_build_dir=true

    call :is_false_string "!option_lint!"
    if not "!__result__!" == "true" call :lint || goto :eof

    call :is_true_string "!option_build!"
    if "!__result__!" == "true" (
        if not exist "!build_arch_dir!" mkdir "!build_arch_dir!" || goto :eof
        call :build_library || goto :eof
    )

    call :is_true_string "!option_build_examples!"
    if "!__result__!" == "true" call :build_examples || goto :eof

    call :is_true_string "!option_go_build_examples!"
    if "!__result__!" == "true" call :go_build_examples || goto :eof

    call :is_true_string "!option_build_tests!"
    if "!__result__!" == "true" call :build_tests || goto :eof

    call :is_true_string "!copy_deps_to_build_dir!"
    if "!__result__!" == "true" call :copy_deps || goto :eof

    call :is_true_string "!option_test!"
    if "!__result__!" == "true" call :run_tests || goto :eof

    call :is_true_string "!option_go_test!"
    if "!__result__!" == "true" call :go_run_tests || goto :eof

    goto :eof

rem Copy external dependencies into the build directory.
:copy_deps
    echo Copying dependencies (!arch!)...
    rem Copy only if needed
    call :is_true_string "!option_build_examples!"
    if "!__result__!" == "true" (
        robocopy "!build_arch_dir!" "!build_arch_dir!\examples\c" "*.dll" > nul
        robocopy "!webview2_dir!\build\native\!arch!" "!build_arch_dir!\examples\c" "*.dll" > nul
        robocopy "!webview2_dir!\build\native\!arch!" "!build_arch_dir!\examples\cc" "*.dll" > nul
    )
    call :is_true_string "!option_go_build_examples!"
    if "!__result__!" == "true" (
        robocopy "!webview2_dir!\build\native\!arch!" "!build_arch_dir!\examples\go" "*.dll" > nul
    )
    robocopy "!webview2_dir!\build\native\!arch!" "!build_arch_dir!" "*.dll" > nul
    exit /b 0

rem Build the library.
:build_library
    call :activate_toolchain "!option_toolchain!" "!arch!" || goto :eof
    call :compile shared_library "!src_dir!\webview.cc" "shared library" "!build_arch_dir!" || goto :eof
    goto :eof

rem Build examples.
:build_examples
    call :activate_toolchain "!option_toolchain!" "!arch!" || goto :eof
    setlocal
    if "!option_toolchain!" == "msvc" (
        set link_params=!link_params! "!build_arch_dir!\webview.lib"
    ) else if "!option_toolchain!" == "mingw" (
        set link_params=!link_params! "!build_arch_dir!\webview.dll"
    )
    for %%f in ("!examples_dir!\*.c") do (
        call :compile exe "%%f" "C example" "!build_arch_dir!/examples/c" || goto :build_examples_loop_end
    )
    endlocal
    for %%f in ("!examples_dir!\*.cc") do (
        call :compile exe "%%f" "C++ example" "!build_arch_dir!/examples/cc" || goto :build_examples_loop_end
    )
:build_examples_loop_end
    endlocal
    goto :eof

rem Build tests.
:build_tests
    call :activate_toolchain "!option_toolchain!" "!arch!" || goto :eof
    setlocal
    if "!option_toolchain!" == "mingw" (
        rem Linking pthread fixes error when using Clang
        set link_params=!link_params! -pthread
    )
    for %%f in ("!src_dir!\*_test.cc") do (
        call :compile exe "%%f" "test" "!build_arch_dir!" || goto :build_tests_loop_end
    )
:build_tests_loop_end
    endlocal
    goto :eof

rem Run tests.
:run_tests
    if not exist "!build_arch_dir!" (
        echo Error: Please build the tests.>&2
        exit /b 1
    )
    setlocal
    set failed=false
    rem Continue even when tests fail.
    for %%f in ("!build_arch_dir!\*_test.exe") do (
        echo Running test %%~nxf ^(!arch!^)...
        cmd /c "%%~f" || set failed=true
    )
    if "!failed!" == "true" (
        echo Error: One or more test executables failed.>&2
        endlocal & exit /b 1
    )
    endlocal
    goto :eof

rem Build Go examples.
:go_build_examples
    call :activate_toolchain mingw "!arch!" || goto :eof
    setlocal
    if "!arch!" == "x64" (
        set GOARCH=amd64
    ) else if "!arch!" == "x86" (
        set GOARCH=386
    )
    rem Argument quoting only works for Go 1.18 and later.
    rem We therefore conditionally quote arguments in CGO_CXXFLAGS and CGO_LDFLAGS
    rem based on the version of Go.
    call :get_go_version go_version || (endlocal & exit /b 1)
    call :parse_version "!go_version!" go_version_major go_version_minor || (endlocal & exit /b 1)
    set use_quoted_args=false
    if "!go_version_major!" gtr "1" set use_quoted_args=true
    if "!go_version_major!" == "1" if "!go_version_minor!" geq "18" set use_quoted_args=true
    rem Linking pthread fixes error when using Clang
    if "!use_quoted_args!" == "true" (
        set CGO_CXXFLAGS="-I!webview2_dir!\build\native\include"
        set CGO_LDFLAGS="-L!webview2_dir!\build\native\!arch!" -pthread
    ) else (
        set "CGO_CXXFLAGS=-I!webview2_dir!\build\native\include"
        set "CGO_LDFLAGS=-L!webview2_dir!\build\native\!arch!" -pthread
    )
    set "CC=!option_cc!"
    set "CXX=!option_cxx!"
    set CGO_ENABLED=1
    for %%f in ("!examples_dir!\*.go") do (
        echo Building Go example %%~nf ^(!arch!^)...
        go build -o "!build_arch_dir!/examples/go/%%~nf.exe" "%%~f" || goto :go_build_examples_loop_end
    )
:go_build_examples_loop_end
    endlocal
    goto :eof

rem Run Go tests.
:go_run_tests
    call :activate_toolchain mingw "!arch!" || goto :eof
    setlocal
    echo Running Go tests (!arch!)...
    if "!arch!" == "x64" (
        set GOARCH=amd64
    ) else if "!arch!" == "x86" (
        set GOARCH=386
    )
    rem Argument quoting only works for Go 1.18 and later.
    rem We therefore conditionally quote arguments in CGO_CXXFLAGS and CGO_LDFLAGS
    rem based on the version of Go.
    call :get_go_version go_version || (endlocal & exit /b 1)
    call :parse_version "!go_version!" go_version_major go_version_minor || (endlocal & exit /b 1)
    set use_quoted_args=false
    if "!go_version_major!" gtr "1" set use_quoted_args=true
    if "!go_version_major!" == "1" if "!go_version_minor!" geq "18" set use_quoted_args=true
    rem Linking pthread fixes error when using Clang
    if "!use_quoted_args!" == "true" (
        set CGO_CXXFLAGS="-I!webview2_dir!\build\native\include"
        set CGO_LDFLAGS="-L!webview2_dir!\build\native\!arch!" -pthread
    ) else (
        set "CGO_CXXFLAGS=-I!webview2_dir!\build\native\include"
        set "CGO_LDFLAGS=-L!webview2_dir!\build\native\!arch! -pthread"
    )
    set "CC=!option_cc!"
    set "CXX=!option_cxx!"
    set CGO_ENABLED=1
    set "PATH=!PATH!;!build_arch_dir!"
    go test || (endlocal & exit /b 1)
    endlocal
    goto :eof

rem Compile a C/C++ file into an executable or shared library.
:compile type file description
    if not "%~1" == "exe" if not "%~1" == "shared_library" (
        echo Error: Invalid type : %~1>&2
        exit /b 1
    )
    setlocal
    set type=%~1
    set file=%~2
    set name=%~n2
    set ext=%~x2
    set ext=!ext:~1!
    set description=%~3
    set output_dir=%~4
    set message=Building !description! !name! (!arch!)...
    echo !message!
    if not exist "!output_dir!" mkdir "!output_dir!" || (endlocal & exit /b 1)
    call :compile_!type!_!option_toolchain!_!ext! || (endlocal & exit /b 1)
    endlocal
    goto :eof

:compile_shared_library_msvc_c
:compile_shared_library_mingw_c
    echo Error: Invalid combination (!type!, !option_toolchain!, !ext!).>&2
    exit /b 1

:compile_shared_library_msvc_cc
    cl !common_params! !cxx_params! !link_params! ^
        "/DWEBVIEW_API=__declspec(dllexport)" ^
        "!file!" ^
        "/Fo:!output_dir!\\" ^
        /link /DLL "/OUT:!output_dir!/!name!.dll"
    goto :eof

:compile_shared_library_mingw_cc
    "!option_cxx!" -fPIC --shared "!file!" !common_params! !cxx_params! !link_params! ^
        -o "!output_dir!/!name!.dll"
    goto :eof

:compile_exe_msvc_c
    cl !common_params! !cc_params! !link_params! "!file!" ^
         "/Fo:!output_dir!\\" ^
        /link "/OUT:!output_dir!/!name!.exe"
    goto :eof

:compile_exe_msvc_cc
    cl !common_params! !cxx_params! !link_params! "!file!" ^
         "/Fo:!output_dir!\\" ^
        /link "/OUT:!output_dir!/!name!.exe"
    goto :eof

:compile_exe_mingw_c
    "!option_cc!" "!file!" !common_params! !cc_params! !link_params! ^
        "-L!build_arch_dir!" -lwebview ^
        -o "!output_dir!/!name!.exe"
    goto :eof

:compile_exe_mingw_cc
    "!option_cxx!" "!file!" !common_params! !cxx_params! !link_params! ^
        "-L!build_arch_dir!" -lwebview ^
        -o "!output_dir!/!name!.exe"
    goto :eof

rem Fetch dependencies.
:fetch_deps
    if not exist "!webview2_dir!" (
        mkdir "!webview2_dir!" || goto :eof
        echo Fetching Nuget package Microsoft.Web.WebView2 version !option_webview2_version!...
        curl -sSL "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/!option_webview2_version!" ^
            | tar -xf - -C "!webview2_dir!" || goto :eof
    )
    goto :eof

rem Attempt to find and activate the specified toolchain.
:activate_toolchain toolchain arch
    if "%~1" == "msvc" (
        call :activate_msvc "%~2" && goto :eof
    ) else if "%~1" == "mingw" (
        call :activate_mingw "%~2" && goto :eof
    ) else (
        echo Error: Invalid toolchain: %~1>&2
        exit /b 1
    )
    echo Error: Unable to activate toolchain "%~1".>&2
    exit /b 1

rem Find and activate MinGW-w64.
:activate_mingw arch
    setlocal
    set arch=%~1
    set mingw_path=
    for %%p in ("!SystemDrive!" "!ProgramData!\chocolatey\lib\mingw\tools\install" "!SystemDrive!\msys64") do (
        call :find_mingw_path "!arch!" "%%~p"
        if not "!__result__!" == "" (
            set mingw_path=!__result__!
            goto :activate_mingw_loop_end
        )
    )
:activate_mingw_loop_end
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
    set test_sub_path=bin\g++.exe
    if "!arch!" == "x64" (
        if exist "!mingw64_path!\!test_sub_path!" (
            set mingw_path=!mingw64_path!
        )
    ) else if "!arch!" == "x86" (
        if exist "!mingw32_path!\!test_sub_path!" (
            set mingw_path=!mingw32_path!
        ) else if exist "!mingw64_path!\!test_sub_path!" (
            set mingw_path=!mingw64_path!
        )
    )
    endlocal & set __result__=%mingw_path%
    goto :eof

rem Set up a MSVC (VC++) environment if needed.
:activate_msvc arch
    where cl.exe > nul 2>&1 && goto :eof || cmd /c exit /b 0
    call :find_msvc || goto :eof
    echo Setting up VS environment ^(%~1^)...
    call :get_host_arch || goto :eof
    call "!vc_dir!\Common7\Tools\vsdevcmd.bat" -no_logo -arch=%~1 -host_arch=!__result__! || goto :eof
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

rem Sanetizes the given option name by converting "-" to "_".
rem Letter casing is untouched because variable names are case-insensitive.
:sanetize_option_name name
    set __result__=%~1
    set __result__=!__result__:-=_!
    goto :eof

rem Checks whether the given option was set explicitly.
rem Returns "true" if true; otherwise "false".
:is_option_set_explicitly option_name
    call :sanetize_option_name "%~1" || goto :eof
    if "!_option_set_explicitly_%__result__%!" == "true" (
        set __result__=true
    ) else (
        set __result__=false
    )
    goto :eof

rem Checks whether the given string is equivalent to "true"
rem Returns "true" if true; otherwise "false".
:is_true_string value
    set __result__=false
    if "%~1" == "1" set __result__=true
    if "%~1" == "true" set __result__=true
    if "%~1" == "yes" set __result__=true
    goto :eof

rem Checks whether the given string is equivalent to "false"
rem Returns "true" if true; otherwise "false".
:is_false_string value
    set __result__=false
    if "%~1" == "" set __result__=true
    if "%~1" == "0" set __result__=true
    if "%~1" == "false" set __result__=true
    if "%~1" == "no" set __result__=true
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

rem Extract the MAJOR.MINOR.PATCH version number of Go from the output of the "go version" command.
:get_go_version version
    setlocal
    for /f "usebackq tokens=1-4" %%a in (`go version`) do (
        rem Example output: go version go1.19.3 windows/amd64
        if "%%a" == "go" if "%%b" == "version" (
            set version=%%c
            set version=!version:go=!
        )
    )
    if "!version!" == "" (endlocal & goto :eof)
    endlocal & set "%~1=%version%"
    goto :eof

rem Parses a MAJOR.MINOR.PATCH version number and returns each element in the specified variables.
:parse_version version major minor patch
    setlocal
    if "%~1" == "" (endlocal & exit /b 1)
    for /f "tokens=1-3 delims=." %%a in ("%~1") do (
        set major=%%a
        set minor=%%b
        set patch=%%c
    )
    if "!major!" == "" (endlocal & exit /b 1)
    if "!minor!" == "" (endlocal & exit /b 1)
    if "!patch!" == "" (endlocal & exit /b 1)
    endlocal & (
        if not "%~2" == ""  set "%~2=%major%"
        if not "%~3" == ""  set "%~3=%minor%"
        if not "%~4" == ""  set "%~4=%patch%"
    )
    goto :eof

