#!/usr/bin/env bash

# Options with their default values.
option_help=
option_clean=false
option_build=false
option_build_examples=false
option_build_tests=false
option_test=false
option_target_arch=
option_reformat=false
option_lint=false
option_go_test=false

function main {
    if [[ "${#@}" == "0" ]]; then
        print_help
        return
    fi

    parse_options on_option_parsed $@ || return
    on_post_parse_options || return

    if [[ "${option_help}" == "true" ]]; then
        print_help
        return
    fi

    print_current_options || return

    local script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd) || return
    local src_dir=$(dirname "${script_dir}") || return
    local examples_dir=${src_dir}/examples
    local build_dir=${src_dir}/build

    if [[ "$(is_true_string "${option_reformat}")" == "true" ]]; then
        reformat || return
    fi

    if [[ "${option_target_arch}" == "all" ]]; then
        local build_x64=true
        local build_x86=true
    elif [[ "${option_target_arch}" == "x64" ]]; then
        local build_x64=true
    elif [[ "${option_target_arch}" == "x86" ]]; then
        local build_x86=true
    else
        echo "Error: Invalid target architecture." >&2
        return 1
    fi

    if [[ "${build_x64}" == "true" ]]; then
        build x64 || return
    fi

    if [[ "${build_x86}" == "true" ]]; then
        build x86 || return
    fi
}

function print_help {
    echo "This is the build script for the webview library."
    echo
    echo "Usage:"
    echo "    program --help"
    echo "    program [--clean] [--build] [--build-examples] [--build-tests]"
    echo "            [--test] [--target-arch=ARCH] [--reformat] [--lint]"
    echo
    echo "Options:"
    echo "    --help                      Display this help text."
    echo "    --clean                     Clean the build directory."
    echo "    --build                     Build library."
    echo "    --build-examples            Build examples (implies --build)."
    echo "    --build-tests               Build tests (implies --build)."
    echo "    --test                      Run tests (implies --build-tests)."
    echo "    --target-arch=ARCH          Build for the target CPU architecture."
    echo "                                Choices: all, x86, x64"
    echo "    --reformat                  Reformat code (requires clang-format)."
    echo "    --lint                      Run lint checks (requires clang-tidy)."
    echo "    --go-test                   Run Go tests."
    echo
    echo "Cross-compilation"
    echo "================="
    echo
    echo "Your toolchain must have multilib support. In addition you must install"
    echo "dependencies for your target architecture onto your system."
}

# Print option and their current values in a human-readable way.
function print_current_options {
    echo "Options:"
    echo "  Clean build: ${option_clean}"
    echo "  Build library: ${option_build}"
    echo "  Build examples: ${option_build_examples}"
    echo "  Build tests: ${option_build_tests}"
    echo "  Run tests: ${option_test}"
    echo "  Target architecture: ${option_target_arch}"
    echo "  Reformat code: ${option_reformat}"
    echo "  Run lint checks: ${option_lint}"
    echo "  Run Go tests: ${option_go_test}"
}

# Stores the option as a variable.
function on_option_parsed {
    local name=$(echo "${1}" | tr "[:upper:]" "[:lower:]") || return
    name=${name//-/_}
    eval "option_${name}=${2}" || return
}

# Overrides options after being parsed.
function on_post_parse_options {
    # Running tests requires building tests.
    if [[ "$(is_true_string "${option_test}")" == "true" && "${option_build_tests}" != "true" ]]; then
        option_build_tests=true
    fi

    # Building examples requires building library.
    if [[ "$(is_true_string "${option_build_examples}")" == "true" && "${option_build}" != "true" ]]; then
        option_build=true
    fi

    # Building tests requires building library.
    if [[ "$(is_true_string "${option_build_tests}")" == "true" && "${option_build}" != "true" ]]; then
        option_build=true
    fi

    # Set the target architecture based on the machine's architecture.
    if [[ -z "${option_target_arch}" ]]; then
        option_target_arch=$(get_host_arch)
    fi
}

# Reformat code.
function reformat {
    echo "Reformatting code..."
    while read f; do
        clang-format -i "${f}" || return
    done <<< "$(find "${src_dir}" -type f \
        -path "${src_dir}/*.c" \
        -or -path "${src_dir}/*.cc" \
        -or -path "${src_dir}/*.h" \
        -or -path "${examples_dir}/*.c" \
        -or -path "${examples_dir}/*.cc" \
        -or -path "${examples_dir}/*.h")"
}

# Run lint checks.
function lint {
    echo "Running lint checks (${arch})..."
    while read f; do
        local ext=${f##*.}
        if [[ "${ext}" == "c" ]]; then
            local tidy_params=${cc_params}
        elif [[ "${ext}" == "cc" ]]; then
            local tidy_params=${cxx_params}
        else
            echo "Error: Unknown file extension: ${ext}" >&2
            return 1
        fi
        clang-tidy "--config-file=${src_dir}/.clang-tidy" \
            "--warnings-as-errors=*" \
            "${f}" -- "-I${src_dir}" ${tidy_params} || return
    done <<< "$(find "${src_dir}" -type f \
        -path "${src_dir}/*.c" \
        -or -path "${src_dir}/*.cc" \
        -or -path "${examples_dir}/*.c" \
        -or -path "${examples_dir}/*.cc")"
}

# All tasks related to building and testing are to be invoked here.
function build {
    local arch=${1}
    local build_arch_dir=${build_dir}/${arch}

    if [[ "$(is_true_string "${option_clean}")" == "true" ]]; then
        echo "Cleaning build directory (${arch})..."
        if [[ -d "${build_arch_dir}" ]]; then
            rm -rf "${build_arch_dir}" || return
        fi
    fi

    local warning_params="-Wall -Wextra -pedantic"
    local common_params=
    local link_params=
    local cc_params="--std=c99 ${warning_params}"
    local cxx_params="--std=c++11 ${warning_params}"
    if [[ "$(uname)" = "Darwin" ]]; then
        link_params="${link_params} -framework WebKit"
        cxx_params="${cxx_params} -DWEBVIEW_COCOA"
    else
        local pkgconfig_libs="gtk+-3.0 webkit2gtk-4.0"
        local pkgconfig_ldflags=$(pkg-config --libs "${pkgconfig_libs}") || return
        local pkgconfig_cflags=$(pkg-config --cflags "${pkgconfig_libs}") || return
        link_params="${link_params} ${pkgconfig_ldflags}"
        cxx_params="${cxx_params} ${pkgconfig_cflags} -DWEBVIEW_GTK"
    fi

    # Specify target architecture only if it differs from the host architecture.
    if [[ "$(get_host_arch)" != "${arch}" ]]; then
        if [[ "${arch}" == "x64" ]]; then
            common_params="${common_params} -m64"
        elif [[ "${arch}" == "x86" ]]; then
            common_params="${common_params} -m32"
        fi
    fi

    if [[ "$(is_true_string "${option_lint}")" == "true" ]]; then
        lint || return
    fi

    if [[ "$(is_true_string "${option_build}")" == "true" ]]; then
        if [[ ! -d "${build_arch_dir}" ]]; then
            mkdir -p "${build_arch_dir}" || return
        fi
        build_library || return
    fi

    if [[ "$(is_true_string "${option_build_examples}")" == "true" ]]; then
        build_examples || return
    fi

    if [[ "$(is_true_string "${option_build_tests}")" == "true" ]]; then
        build_tests || return
    fi

    if [[ "$(is_true_string "${option_test}")" == "true" ]]; then
        run_tests || return
    fi

    if [[ "$(is_true_string "${option_go_test}")" == "true" ]]; then
        go_run_tests || return
    fi
}

# Build the library.
function build_library {
    echo "Building library (${arch})..."
    c++ -c "-I${src_dir}" "${src_dir}/webview.cc" \
        ${common_params} ${cxx_params} -o "${build_arch_dir}/webview.o" || return
}

# Build examples.
function build_examples {
    while read file; do
        compile_exe "${file}" "example" || return
    done <<< "$(find "${src_dir}" -type f \
        -path "${examples_dir}/*.c" \
        -or -path "${examples_dir}/*.cc")"
}

# Build tests.
function build_tests {
    while read file; do
        compile_exe "${file}" "test" || return
    done <<< "$(find "${src_dir}" -type f \
        -path "${src_dir}/*_test.c" \
        -or -path "${src_dir}/*_test.cc")"
}

# Run tests.
function run_tests {
    local failed=false
    # Continue even when tests fail.
    while read file; do
        local name=$(basename "-s.${file##*.}" "${file}")
        echo "Running test ${name} (${arch})..."
        "${file}" || failed=true
    done <<< "$(find "${build_arch_dir}" -type f -name "*_test")"
    if [[ "${failed}" == "true" ]]; then
        return 1
    fi
    return 0
}

# Run Go tests.
function go_run_tests {
    echo "Running Go tests (${arch})..."
    if [[ "${arch}" == "x64" ]]; then
        local GOARCH=amd64
    elif [[ "${arch}" == "x86" ]]; then
        local GOARCH=386
    fi
    GOARCH=${GOARCH} CGO_ENABLED=1 go test || return
}

# Compile a C/C++ file into an executable.
function compile_exe {
    local file=${1}
    local description=${2}
    local name=$(basename "-s.${file##*.}" "${file}")
    local output_path_excl_ext=${build_arch_dir}/${name}
    local message="Building ${description} ${name} (${arch})..."
    echo "${message}"
    if [[ "${file##*.}" == "c" ]]; then
        cc -c "-I${src_dir}" "${file}" \
            ${common_params} \
            -o "${output_path_excl_ext}.o"|| return
        c++ "-I${src_dir}" "${output_path_excl_ext}.o" \
            "${build_arch_dir}/webview.o" \
            ${common_params} ${link_params} ${cxx_params} \
            -o "${output_path_excl_ext}"|| return
    elif [[ "${file##*.}" == "cc" ]]; then
        c++ "-I${src_dir}" "${file}" \
            ${common_params} ${link_params} ${cxx_params} \
            -o "${output_path_excl_ext}" || return
    fi
}

# Parses a command line option prefixed with "--".
# Example input: --verbose "--message=hello world"
# If the value is omitted then it will be "true".
function parse_options {
    local callback=${1}
    local awaiting_value_for_option=
    for option in $@; do
        if [[ "${option}" =~ ^--[a-z0-9] ]]; then
            # If we are waiting for a value while parsing a new option then the previous option is a switch.
            if [[ ! -z "${awaiting_value_for_option}" ]]; then
                "${callback}" "${awaiting_value_for_option}" "true"
                awaiting_value_for_option=
            fi
            # Attempts to parse the option name and value if present ("foo=bar").
            local option=${option:2}
            local option_length=${#option}
            local option_name=${option%%=*}
            local option_name_length=${#option_name}
            if [[ "${option_length}" -gt "${option_name_length}" ]]; then
                local option_value=${option:((${option_name_length} + 1))}
                "${callback}" "${option_name}" "${option_value}"
            else
                # Maybe the next parameter is the value.
                awaiting_value_for_option=${option_name}
            fi
        elif [[ ! -z "${awaiting_value_for_option}" ]]; then
            # If we are waiting for a value while parsing the next parameter that has no prefix then it is the value.
            # This handles input like "--foo bar" and "--foo=bar" without quotes ("=" is treated like space).
            "${callback}" "${awaiting_value_for_option}" "${option}"
            awaiting_value_for_option=
        fi
        shift
    done
    # If we are waiting for a value while there are no more parameters to parse then the option is a switch.
    if [[ ! -z "${awaiting_value_for_option}" ]]; then
        "${callback}" "${awaiting_value_for_option}" "true"
        awaiting_value_for_option=
    fi
}

# Checks whether the given string is equivalent to "true"
# Returns "true" if true; otherwise "false".
function is_true_string {
	if [[ "${1}" =~ 1|true|yes ]]; then
		echo "true"
        return 0
	fi
	echo "false"
    return 0
}

# Get the host machine/CPU architecture.
function get_host_arch {
    local host_arch=$(uname -m)
    if [[ "${host_arch}" == "x86_64" ]]; then
        echo "x64"
    elif [[ "${host_arch}" == "i386" ]]; then
        echo "x86"
    else
        echo "Error: Unsupported host machine architecture." >&2
        return 1
    fi
    return 0
}

(main $@) || exit 1
