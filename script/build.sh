#!/usr/bin/env bash
set -e

declare -A options
options["help"]=
options["clean"]=false
options["build"]=false
options["build-examples"]=false
options["build-tests"]=false
options["test"]=false
options["target-arch"]=
options["reformat"]=false
options["lint"]=false
options["go-test"]=false

function main {
    parse_options on_option_parsed $@

    if [[ "${options["help"]}" == "true" ]]; then
        print_help
        return 0
    fi

    local script_dir=$(dirname "$(realpath "${BASH_SOURCE[0]}")")
    local src_dir=$(dirname "${script_dir}")
    local examples_dir=${src_dir}/examples
    local build_dir=${src_dir}/build

    set_option_overrides
    print_current_options

    if [[ "$(is_true_string "${options[reformat]}")" == "true" ]]; then
        reformat
    fi

    if [[ "${options[target-arch]}" == "all" ]]; then
        local build_x64=true
        local build_x86=true
    elif [[ "${options[target-arch]}" == "x64" ]]; then
        local build_x64=true
    elif [[ "${options[target-arch]}" == "x86" ]]; then
        local build_x86=true
    else
        echo "Invalid target architecture."
        return 1
    fi

    if [[ "${build_x64}" == "true" ]]; then
        build x64
    fi

    if [[ "${build_x86}" == "true" ]]; then
        build x86
    fi

    if [[ "${options[go-test]}" == "true" ]]; then
        go_run_tests
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
}

# Print option and their current values in a human-readable way.
function print_current_options {
    echo "Options:"
    echo "  Clean build: ${options[clean]}"
    echo "  Build library: ${options[build]}"
    echo "  Build examples: ${options[build-examples]}"
    echo "  Build tests: ${options[build-tests]}"
    echo "  Run tests: ${options[test]}"
    echo "  Target architecture: ${options[target-arch]}"
    echo "  Reformat code: ${options[reformat]}"
    echo "  Run lint checks: ${options[lint]}"
    echo "  Run Go tests: ${options[go-test]}"
}

# Stores the option as a variable.
function on_option_parsed {
    options[$1]=$2
}

# Overrides options if needed. For example, options can be changed conditionally.
function set_option_overrides {
    # Use specific options in the CI environment.
    if [[ ! -z "${CI}" ]]; then
        options["build-examples"]=true
        options["test"]=true
        options["target-arch"]=all
        options["go-test"]=true
    fi

    # Running tests requires building tests.
    if [[ "$(is_true_string "${options[test]}")" == "true" && "${options[build-tests]}" != "true" ]]; then
        options["build-tests"]=true
    fi

    # Building examples requires building library.
    if [[ "$(is_true_string "${options[build-examples]}")" == "true" && "${options[build]}" != "true" ]]; then
        options["build"]=true
    fi

    # Building tests requires building library.
    if [[ "$(is_true_string "${options[build-tests]}")" == "true" && "${options[build]}" != "true" ]]; then
        options["build"]=true
    fi

    # Set the target architecture based on the machine's architecture.
    if [[ -z "${options[target-arch]}" ]]; then
        if [[ "$(uname -m)" == "x86_64" ]]; then
            options["target-arch"]=x64
        else
            options["target-arch"]=x86
        fi
    fi
}

# Reformat code.
function reformat {
    echo "Reformatting code..."
    clang-format -i \
        "${src_dir}/webview.h" \
        "${src_dir}/webview_test.cc" \
        "${examples_dir}/basic_cpp.cc"
}

# Run lint checks.
function lint {
    echo "Running lint checks (${arch})..."
    clang-tidy "${examples_dir}/basic_cpp.cc" -- "-I${src_dir}" ${link_params} ${cxx_params}
    clang-tidy "${src_dir}/webview_test.cc" -- "-I${src_dir}" ${link_params} ${cxx_params}
}

# All tasks related to building and testing are to be invoked here.
function build {
    local arch=${1}
    local build_arch_dir=${build_dir}/${arch}

    if [[ "$(is_true_string "${options[clean]}")" == "true" ]]; then
        echo "Cleaning build directory (${arch})..."
        rm -rf "${build_arch_dir}"
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
        local pkgconfig_libs=(gtk+-3.0 webkit2gtk-4.0)
        local pkgconfig_ldflags=$(pkg-config --libs "${pkgconfig_libs[@]}")
        local pkgconfig_cflags=$(pkg-config --cflags "${pkgconfig_libs[@]}")
        link_params="${link_params} ${pkgconfig_ldflags}"
        cxx_params="${cxx_params} ${pkgconfig_cflags} -DWEBVIEW_GTK"
    fi
    if [[ "${arch}" == "x64" ]]; then
        common_params="${common_params} -m64"
    else
        common_params="${common_params} -m32"
    fi

    if [[ "$(is_true_string "${options[lint]}")" == "true" ]]; then
        lint
    fi

    if [[ "$(is_true_string "${options[build]}")" == "true" ]]; then
        if [[ ! -d "${build_arch_dir}" ]]; then
            mkdir -p "${build_arch_dir}"
        fi
        build_library
    fi

    if [[ "$(is_true_string "${options[build-examples]}")" == "true" ]]; then
        build_examples
    fi

    if [[ "$(is_true_string "${options[build-tests]}")" == "true" ]]; then
        build_tests
    fi

    if [[ "$(is_true_string "${options[test]}")" == "true" ]]; then
        run_tests
    fi
}

# Build the library.
function build_library {
    echo "Building library (${arch})..."
    local output_file=${build_arch_dir}/webview
    c++ -c "-I${src_dir}" "${src_dir}/webview.cc" ${common_params} ${cxx_params} -o "${output_file}.o"
}

# Build examples.
function build_examples {
    local output_file=

    echo "Building C++ example (${arch})..."
    output_file=${build_arch_dir}/basic_cpp
    c++ "-I${src_dir}" "${examples_dir}/basic_cpp.cc" ${common_params} ${link_params} ${cxx_params} -o "${output_file}"

    echo "Building C example (${arch})..."
    output_file=${build_arch_dir}/basic_c
    cc -c "-I${src_dir}" "${examples_dir}/basic_c.c" ${common_params} -o "${output_file}.o"
    c++ "-I${src_dir}" "${output_file}.o" "${build_arch_dir}/webview.o" ${common_params} ${link_params} ${cxx_params} -o "${output_file}"
}

# Build tests.
function build_tests {
    echo "Building tests (${arch})..."
    local output_file=${build_arch_dir}/webview_test
    c++ "-I${src_dir}" "${src_dir}/webview_test.cc" ${common_params} ${link_params} ${cxx_params} -o "${output_file}"
}

# Run tests.
function run_tests {
    echo "Running tests (${arch})..."
    "${build_arch_dir}/webview_test"
}

# Run Go tests.
function go_run_tests {
    echo "Running Go tests..."
    CGO_ENABLED=1 go test
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

(main $@) || exit 1
