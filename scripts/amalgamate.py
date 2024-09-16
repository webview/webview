from argparse import ArgumentParser
from dataclasses import dataclass
import os
import re
import shutil
import subprocess
from typing import Sequence


@dataclass
class ProcessorContext:
    base_dir: os.PathLike
    include_pattern = re.compile(r'#include "([^"]+)"\n')
    block_comment_pattern = re.compile(r"^/\*.*?\*/\n", re.DOTALL)
    visited_files = set()
    chunks = []
    visited_copyright_notices = set()
    ordered_copyright_notices = []


def process_file(
    context: ProcessorContext, input: os.PathLike, base_dir: os.PathLike = None
):
    base_dir = os.path.realpath(context.base_dir if base_dir is None else base_dir)
    input = os.path.realpath(
        input if os.path.isabs(input) else os.path.join(base_dir, input)
    )

    if input in context.visited_files:
        return
    context.visited_files.add(input)

    if not os.path.exists(input):
        raise Exception("Input file not found: {}".format(input))

    print("Processing file: {}".format(input))

    input_relative_root = os.path.relpath(input, context.base_dir)
    context.chunks.append("// file begin: {}\n".format(input_relative_root))
    with open(input, encoding="utf-8") as f:
        content = f.read()

        content = context.block_comment_pattern.sub(
            lambda m: replace_copyright(context, m), content
        ).strip()

        base_dir = os.path.dirname(input)

        end = 0
        for m in context.include_pattern.finditer(content):
            context.chunks.append(content[end : m.start(0)])
            end = m.end(0)
            include_file = os.path.join(base_dir, m[1])

            if include_file in context.visited_files:
                continue

            if not os.path.exists(include_file):
                print("File not found: {}".format(include_file))
                context.chunks.append(m[0])
                continue

            process_file(context, include_file, base_dir=base_dir)
        context.chunks.append(content[end:])
    context.chunks.append("\n// file end: {}\n".format(input_relative_root))


def replace_copyright(context: ProcessorContext, match: re.Match[str]):
    if "copyright".casefold() not in match[0].casefold():
        return match[0]
    hashed = hash(match[0])
    if hashed in context.visited_copyright_notices:
        return ""
    context.visited_copyright_notices.add(hashed)
    context.ordered_copyright_notices.append(match[0].strip())
    return ""


def amalgamate(
    base_dir: os.PathLike, inputs: Sequence[os.PathLike], output: os.PathLike
):
    context = ProcessorContext(base_dir=base_dir)
    for input in inputs:
        process_file(context, input)
    output = os.path.realpath(output)
    os.makedirs(os.path.dirname(output), exist_ok=True)
    with open(output, mode="w", encoding="utf-8") as f:
        for notice in context.ordered_copyright_notices:
            f.write(notice)
            f.write("\n")
        for chunk in context.chunks:
            f.write(chunk)


def reformat_file(file: os.PathLike, clang_format_exe: os.PathLike = "clang-format"):
    clang_format_exe = shutil.which(clang_format_exe)
    if clang_format_exe is None:
        raise Exception("clang-format not found: {}".format(clang_format_exe))
    print("Reformatting file: {}".format(file))
    subprocess.check_call((clang_format_exe, "-i", file))


def main(options):
    amalgamate(options.base, options.input, options.output)
    reformat_file(options.output, clang_format_exe=options.clang_format_exe)


def parse_args():
    parser = ArgumentParser(
        prog="amalgamate",
        description="Combines a C or C++ source/header hierarchy into one file",
    )
    parser.add_argument("--base", help="Base directory")
    parser.add_argument("--clang-format-exe", help="clang-format executable")
    parser.add_argument("--output", help="Output file")
    parser.add_argument("input", nargs="+", help="Input file")
    return parser.parse_args()


if __name__ == "__main__":
    main(parse_args())
