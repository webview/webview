#!/usr/bin/env sh

script_dir=$(cd "$(dirname "${0}")" && pwd) || exit 1
project_dir=$(dirname "${script_dir}") || exit 1
build_dir=${project_dir}/build

if [ -z "${CLANG_TIDY_EXE}" ]; then
    CLANG_TIDY_EXE=clang-tidy
fi

processed_count=0
while read f; do
    if echo "${f}" | grep -qE '\.c$'; then
        language_arg="--extra-arg-before=--language=c"
    elif echo "${f}" | grep -qE '\.c(c|pp|xx)$'; then
        language_arg="--extra-arg-before=--language=c++"
    fi
    "${CLANG_TIDY_EXE}" "${language_arg}" \
        -p "${build_dir}" \
        "--warnings-as-errors=*" \
        "${f}" || exit 1
    processed_count=$((processed_count+1))
done <<EOF
$(find "${project_dir}" \( -iname "*.c" -or -iname "*.cc" -or -iname "*.cpp" -or -iname "*.cxx" \) -not -iwholename "${build_dir}/*")
EOF

if [ "${processed_count}" -lt 1 ]; then
    echo "Error: No files processed." >&2
    exit 1
fi
