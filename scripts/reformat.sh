#!/usr/bin/env sh

script_dir=$(cd "$(dirname "${0}")" && pwd) || exit 1
project_dir=$(dirname "${script_dir}") || exit 1
build_dir=${project_dir}/build

if [ -z "${CLANG_FORMAT_EXE}" ]; then
    CLANG_FORMAT_EXE=clang-format
fi

while read f; do
    "${CLANG_FORMAT_EXE}" -i "${f}" || exit 1
done <<EOF
$(find "${project_dir}" \( -iname "*.h" -or -iname "*.hh" -or -iname "*.c" -or -iname "*.cc" \) -not -iwholename "${build_dir}/*")
EOF

"${script_dir}/check_format.sh" || exit 1
