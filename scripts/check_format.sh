#!/usr/bin/env sh

script_dir=$(cd "$(dirname "${0}")" && pwd) || exit 1
project_dir=$(dirname "${script_dir}") || exit 1
build_dir=${project_dir}/build

if [ -z "${CLANG_FORMAT_EXE}" ]; then
    CLANG_FORMAT_EXE=clang-format
fi

processed_count=0
while read f; do
    "${CLANG_FORMAT_EXE}" --dry-run --Werror "${f}" || exit 1
    processed_count=$((processed_count+1))
done <<EOF
$(find "${project_dir}" \( -iname "*.h" -or -iname "*.hpp" -or -iname "*.c" -or -iname "*.cpp" \) -not -iwholename "${build_dir}/*")
EOF

if [ "${processed_count}" -lt 1 ]; then
    echo "Error: No files processed." >&2
    exit 1
fi
