#!/bin/bash

unix_realpath_wrapper() {
    if [[ "${host_os}" == "macos" ]]; then
        readlink -f "${1}" || return 1
    else
        realpath "${1}" || return 1
    fi
}

windows_fetch_mswebview2() {
    if [[ "${host_os}" == "windows" ]]; then
        local nuget_exe=${tools_dir}/nuget/nuget.exe
        if [[ ! -f "${nuget_exe}" ]]; then
            mkdir -p "$(dirname "${nuget_exe}")" || return 1
            echo "Fetching NuGet..."
            curl -sSLo "${nuget_exe}" https://dist.nuget.org/win-x86-commandline/latest/nuget.exe || return 1
        fi
    fi
    local mswebview2_dir=${libs_dir}/Microsoft.Web.WebView2.${mswebview2_version}
    if [[ ! -d "${mswebview2_dir}" ]]; then
        mkdir -p "${mswebview2_dir}" || return 1
        echo "Fetching mswebview2 ${mswebview2_version}..."
        if [[ "${host_os}" == "windows" ]]; then
            "${nuget_exe}" install Microsoft.Web.Webview2 -Version "${mswebview2_version}" -OutputDirectory "${libs_dir}" || return 1
        else
            local mswebview2_zip=${mswebview2_dir}.zip
            if [[ ! -f "${mswebview2_zip}" ]]; then
                curl -sSLo "${mswebview2_zip}" "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${mswebview2_version}" || return 1
                unzip -q "${mswebview2_zip}" -d "${mswebview2_dir}" || return 1
            fi
        fi
    fi
}

go_setup_env() {
    # Argument quoting works for Go 1.18 and later but as of 2022-06-26 GitHub Actions has Go 1.17.11.
    # See https://go-review.googlesource.com/c/go/+/334732/
    # TODO: Use proper quoting when GHA has Go 1.18 or later.
    export "CGO_CXXFLAGS=-I${libs_dir}/Microsoft.Web.WebView2.${mswebview2_version}/build/native/include"
    export CGO_ENABLED=1
}

task_clean() {
    if [[ -d "${build_dir}" ]]; then
        rm -rd "${build_dir}" || return 1
    fi
}

task_format() {
    if command -v clang-format >/dev/null 2>&1 ; then
        echo "Formatting..."
        clang-format -i \
                "${project_dir}/webview.h" \
                "${project_dir}/webview_test.cc" \
                "${project_dir}/examples/"*.c \
                "${project_dir}/examples/"*.cc || return 1
    else
        echo "SKIP: Formatting (clang-format not installed)"
    fi
}

task_deps() {
    if [[ "${target_os}" == "windows" ]]; then
        windows_fetch_mswebview2 || return 1
    fi
}

task_check() {
    if command -v clang-tidy >/dev/null 2>&1 ; then
        echo "Linting..."
        clang-tidy "${project_dir}/examples/basic.cc" -- "${cxx_compile_flags[@]}" "${cxx_link_flags[@]}" || return 1
        clang-tidy "${project_dir}/examples/bind.cc" -- "${cxx_compile_flags[@]}" "${cxx_link_flags[@]}" || return 1
        clang-tidy "${project_dir}/webview_test.cc" -- "${cxx_compile_flags[@]}" "${cxx_link_flags[@]}" || return 1
    else
        echo "SKIP: Linting (clang-tidy not installed)"
    fi
}

task_build() {
    mkdir -p build/examples/c build/examples/cc build/examples/go || true

    echo "Building C++ examples"
    "${cxx_compiler}" "${project_dir}/examples/basic.cc" "${cxx_compile_flags[@]}" "${cxx_link_flags[@]}" -o "${build_dir}/examples/cc/basic${exe_suffix}" || exit 1
    "${cxx_compiler}" "${project_dir}/examples/bind.cc" "${cxx_compile_flags[@]}" "${cxx_link_flags[@]}" -o "${build_dir}/examples/cc/bind${exe_suffix}" || exit 1

    echo "Building C examples"
    "${cxx_compiler}" -c "${cxx_compile_flags[@]}" "${project_dir}/webview.cc" -o "${build_dir}/webview.o" || exit 1
    "${c_compiler}" -c "${c_compile_flags[@]}" "${project_dir}/examples/basic.c" -o "${build_dir}/examples/c/basic.o" || exit 1
    "${c_compiler}" -c "${c_compile_flags[@]}" "${project_dir}/examples/bind.c" -o "${build_dir}/examples/c/bind.o" || exit 1
    "${cxx_compiler}" "${cxx_compile_flags[@]}" "${build_dir}/examples/c/basic.o" "${build_dir}/webview.o" "${cxx_link_flags[@]}" -o "${build_dir}/examples/c/basic${exe_suffix}" || exit 1
    "${cxx_compiler}" "${cxx_compile_flags[@]}" "${build_dir}/examples/c/bind.o" "${build_dir}/webview.o" "${cxx_link_flags[@]}" -o "${build_dir}/examples/c/bind${exe_suffix}" || exit 1

    echo "Building test app"
    "${cxx_compiler}" "${cxx_compile_flags[@]}" "${project_dir}/webview_test.cc" "${cxx_link_flags[@]}" -o "${build_dir}/webview_test${exe_suffix}" || exit 1
}

task_test() {
    echo "Running tests..."
    "${build_dir}/webview_test${exe_suffix}" || return 1
}

task_go_build() {
    if command -v go >/dev/null 2>&1 ; then
        go_setup_env || return 1
        echo "Building Go examples..."
        (cd "${project_dir}" && (
            go build -ldflags "-H windowsgui" -o "build/examples/go/basic${exe_suffix}" examples/basic.go || exit 1
            go build -ldflags "-H windowsgui" -o "build/examples/go/bind${exe_suffix}" examples/bind.go || exit 1
        )) || return 1
    else
        echo "SKIP: Go build (go not installed)"
    fi
}

task_go_test() {
    if command -v go >/dev/null 2>&1 ; then
        go_setup_env || return 1
        echo "Running Go tests..."
        CGO_ENABLED=1 go test
    else
        echo "SKIP: Go tests (go not installed)"
    fi
}

run_task() {
    local name=${1/:/_}
    shift
    eval "task_${name}" "${@}" || return 1
}

# Host operating system
if [[ -z "${HOST_OS}" ]]; then
    if [[ "${OSTYPE}" == "msys" || "${OSTYPE}" == "cygwin" ]]; then
        host_os=windows
    elif [[ "$(uname)" == "Darwin" ]]; then
        host_os=macos
    else
        host_os=linux
    fi
else
    host_os=${HOST_OS}
fi

# Target operating system for cross-compilation
if [[ -z "${TARGET_OS}" ]]; then
    # Target OS is by default the same as the host OS
    target_os=${host_os}
else
    target_os=${TARGET_OS}
fi

# Versions of dependencies
mswebview2_version=1.0.1150.38

# Default C standard
c_std=c99
# Default C++ standard
cxx_std=c++11
# Default C compiler
c_compiler=cc
# Default C++ compiler
cxx_compiler=c++

# C compiler override
if [[ ! -z "${CC}" ]]; then
    c_compiler=${CC}
fi

# C++ compiler override
if [[ ! -z "${CXX}" ]]; then
    cxx_compiler=${CXX}
fi

project_dir=$(dirname "$(dirname "$(unix_realpath_wrapper "${BASH_SOURCE[0]}")")") || exit 1
build_dir=${project_dir}/build
external_dir=${build_dir}/external
libs_dir=${external_dir}/libs
tools_dir=${external_dir}/tools
warning_flags=(-Wall -Wextra -pedantic)
common_compile_flags=("${warning_flags[@]}" "-I${project_dir}")
common_link_flags=("${warning_flags[@]}")
c_compile_flags=("${common_compile_flags[@]}")
c_link_flags=("${common_link_flags[@]}")
cxx_compile_flags=("${common_compile_flags[@]}")
cxx_link_flags=("${common_link_flags[@]}")
exe_suffix=

if [[ "${target_os}" == "windows" ]]; then
    cxx_std=c++17
fi

c_compile_flags+=("-std=${c_std}")
cxx_compile_flags+=("-std=${cxx_std}")

if [[ "${target_os}" == "linux" ]]; then
    pkgconfig_libs=(gtk+-3.0 webkit2gtk-4.0)
    cxx_compile_flags+=($(pkg-config --cflags "${pkgconfig_libs[@]}")) || exit 1
    cxx_link_flags+=($(pkg-config --libs "${pkgconfig_libs[@]}")) || exit 1
elif [[ "${target_os}" == "macos" ]]; then
    cxx_link_flags+=(-framework WebKit)
    macos_target_version=10.9
    c_compile_flags+=("-mmacosx-version-min=${macos_target_version}")
    cxx_compile_flags+=("-mmacosx-version-min=${macos_target_version}")
elif [[ "${target_os}" == "windows" ]]; then
    exe_suffix=.exe
    cxx_compile_flags+=("-I${libs_dir}/Microsoft.Web.WebView2.${mswebview2_version}/build/native/include")
    cxx_link_flags+=(-mwindows -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion)
fi

# Default tasks
tasks=(clean format deps check build test go:build go:test)

# Task override from command line
if [[ ${#@} -gt 0 ]]; then
    tasks=("${@}")
fi

echo "-- C compiler: ${c_compiler}"
echo "-- C compiler flags: ${c_compile_flags[@]}"
echo "-- C linker flags: ${c_link_flags[@]}"
echo "-- C++ compiler: ${cxx_compiler}"
echo "-- C++ compiler flags: ${cxx_compile_flags[@]}"
echo "-- C++ linker flags: ${cxx_link_flags[@]}"

for task in "${tasks[@]}"; do
    run_task "${task}" || exit 1
done
