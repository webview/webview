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
        if [[ "${PATCH_MSWEBVIEW2}" == 1 ]]; then
            echo "Patching mswebview2 ${mswebview2_version}..."
            sed -i 's/#include "EventToken.h"/\/\/#include "EventToken.h"/' ${mswebview2_dir}/build/native/include/WebView2.h || return 1
        fi
    fi
}

go_setup_env() {
    local cgo_cxxflags=()
    if [[ "${target_os}" == "windows" ]]; then
        cgo_cxxflags+=("'-I${libs_dir}/Microsoft.Web.WebView2.${mswebview2_version}/build/native/include'")
        header_path=${project_dir}/webview_mingw_support.h
        # This must somehow be a Windows-style path (forward slashes are OK)
        header_path=$(cygpath --mixed "${header_path}")
        cgo_cxxflags+=("'--include=${header_path}'")
    fi
    export CGO_CXXFLAGS="${cgo_cxxflags[@]}"
    export CGO_ENABLED=1
}

is_ci() {
    if [[ -z "${CI+x}" ]]; then
        return 1
    fi
    return 0
}

task_clean() {
    if [[ -d "${build_dir}" ]]; then
        rm -rd "${build_dir}" || return 1
    fi
}

task_format() {
    if ! command -v clang-format >/dev/null 2>&1 ; then
        local message="Formatting (clang-format not installed)"
        # Allow skipping this task on macOS because GHA doesn't have clang-format installed.
        if is_ci && [[ "${host_os}" != "macos" ]]; then
            echo "FAIL: ${message}"
            return 1
        fi
        echo "SKIP: ${message}"
        return 0
    fi
    echo "Formatting..."
    clang-format -i \
        "${project_dir}/webview.h" \
        "${project_dir}/webview_test.cc" \
        "${project_dir}/examples/"*.c \
        "${project_dir}/examples/"*.cc || return 1
}

task_deps() {
    if [[ "${target_os}" == "windows" ]]; then
        windows_fetch_mswebview2 || return 1
    fi
}

task_check() {
    if ! command -v clang-tidy >/dev/null 2>&1 ; then
        local message="Linting (clang-tidy not installed)"
        # Allow skipping this task on macOS because GHA doesn't have clang-tidy installed.
        if is_ci && [[ "${host_os}" != "macos" ]]; then
            echo "FAIL: ${message}"
            return 1
        fi
        echo "SKIP: ${message}"
        return 0
    fi
    echo "Linting..."
    clang-tidy "${project_dir}/examples/basic.cc" -- "${cxx_compile_flags[@]}" "${cxx_link_flags[@]}" || return 1
    clang-tidy "${project_dir}/examples/bind.cc" -- "${cxx_compile_flags[@]}" "${cxx_link_flags[@]}" || return 1
    clang-tidy "${project_dir}/webview_test.cc" -- "${cxx_compile_flags[@]}" "${cxx_link_flags[@]}" || return 1
}

task_build() {
    mkdir -p build/examples/c build/examples/cc build/examples/go || true

    echo "Building C++ examples"
    "${cxx_compiler}" "${cxx_compile_flags[@]}" "${project_dir}/examples/basic.cc" "${cxx_link_flags[@]}" -o "${build_dir}/examples/cc/basic${exe_suffix}" || exit 1
    "${cxx_compiler}" "${cxx_compile_flags[@]}" "${project_dir}/examples/bind.cc" "${cxx_link_flags[@]}" -o "${build_dir}/examples/cc/bind${exe_suffix}" || exit 1

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

invoke_go_build() {
    local output=${1}
    local input=${2}
    local ldflags=()
    if [[ ! -z "${3}" ]]; then
        ldflags=("${3}")
    fi
    (cd "${project_dir}" && go build "${ldflags[@]}" -o "${output}" "${input}") || return 1
}

task_go_build() {
    if ! command -v go >/dev/null 2>&1 ; then
        local message="Go build (go not installed)"
        if is_ci; then
            echo "FAIL: ${message}"
            return 1
        fi
        echo "SKIP: ${message}"
        return 0
    fi
    go_setup_env || return 1
    echo "Building Go examples..."
    local go_ldflags=()
    if [[ "${target_os}" == "windows" ]]; then
        go_ldflags=(-H windowsgui)
    fi
    if [[ "${#go_ldflags}" -gt 0 ]]; then
        go_ldflags="-ldflags=${go_ldflags[@]}"
    fi
    invoke_go_build "build/examples/go/basic${exe_suffix}" examples/basic.go "${go_ldflags}" || return 1
    invoke_go_build "build/examples/go/bind${exe_suffix}" examples/bind.go "${go_ldflags}" || return 1
}

task_go_test() {
    if ! command -v go >/dev/null 2>&1 ; then
        local message="Go tests (go not installed)"
        if is_ci; then
            echo "FAIL: ${message}"
            return 1
        fi
        echo "SKIP: ${message}"
        return 0
    fi
    go_setup_env || return 1
    echo "Running Go tests..."
    CGO_ENABLED=1 go test
}

task_info() {
    echo "-- Target OS: ${target_os}"
    echo "-- C compiler: ${c_compiler}"
    echo "-- C compiler flags: ${c_compile_flags[@]}"
    echo "-- C linker flags: ${c_link_flags[@]}"
    echo "-- C++ compiler: ${cxx_compiler}"
    echo "-- C++ compiler flags: ${cxx_compile_flags[@]}"
    echo "-- C++ linker flags: ${cxx_link_flags[@]}"
}

run_task() {
    local name=${1/:/_}
    shift
    eval "task_${name}" "${@}" || return 1
}

# Host operating system
if [[ -z "${HOST_OS+x}" ]]; then
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
if [[ -z "${TARGET_OS+x}" ]]; then
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
if [[ ! -z "${CC+x}" ]]; then
    c_compiler=${CC}
fi

# C++ compiler override
if [[ ! -z "${CXX+x}" ]]; then
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
    cxx_compile_flags+=("--include=${project_dir}/webview_mingw_support.h")
    cxx_link_flags+=(-mwindows -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion)
fi

# Default tasks
tasks=(info clean format deps check build test go:build go:test)

# Task override from command line
if [[ ${#@} -gt 0 ]]; then
    tasks=("${@}")
fi

for task in "${tasks[@]}"; do
    run_task "${task}" || exit 1
done
