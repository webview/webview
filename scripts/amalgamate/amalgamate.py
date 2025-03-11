from argparse import ArgumentParser
from dataclasses import dataclass, field
import graphlib
import os
import pathlib
import re
import shutil
import subprocess
from tempfile import TemporaryDirectory
from typing import List, MutableMapping, MutableSet, Sequence


@dataclass
class Include:
    path: os.PathLike
    chunks: List[str] = field(default_factory=list)

    def __eq__(self, other):
        if isinstance(other, "Include"):
            return self.path == other.path
        return self.path == other

    def __lt__(self, other):
        if isinstance(other, "Include"):
            return self.path < other.path
        return self.path < other


@dataclass
class ProcessorContext:
    base_dir: os.PathLike
    include_pattern = re.compile(
        r'^\s*#include "([^"]+)"\s*(?://\s*(.+))?', re.M)
    block_comment_pattern = re.compile(r"^/\*.*?\*/\n", re.DOTALL)
    visited_files: MutableSet[os.PathLike] = field(default_factory=set)
    visited_copyright_notices: MutableSet[str] = field(default_factory=set)
    ordered_copyright_notices: List[str] = field(default_factory=list)
    graph: MutableMapping[str, MutableSet[str]] = field(default_factory=dict)
    includes: MutableMapping[str, Include] = field(default_factory=dict)


def process_file(context: ProcessorContext, input: os.PathLike, search_dirs: Sequence[os.PathLike] = []):
    input = os.path.realpath(
        input if os.path.isabs(input) else os.path.join(
            context.base_dir, input)
    )

    search_dirs = [os.path.realpath(
        search_dir if os.path.isabs(search_dir) else os.path.join(
            context.base_dir, search_dir)) for search_dir in search_dirs]

    if input in context.visited_files:
        return
    context.visited_files.add(input)

    if not os.path.exists(input):
        raise Exception("Input file not found: {}".format(input))

    if os.path.isdir(input):
        print("Processing directory: {}".format(input))
        for entry in os.scandir(input):
            process_file(context, entry.path, search_dirs=search_dirs)
        return

    print("Processing file: {}".format(input))

    input_include_files = context.graph.setdefault(input, set())

    with open(input, encoding="utf-8") as f:
        content = f.read()

        content = context.block_comment_pattern.sub(
            lambda m: replace_copyright(context, m), content
        ).strip()

        input_parent_dir = os.path.dirname(input)
        input_include = context.includes.setdefault(input, Include(input))

        end = 0
        for m in context.include_pattern.finditer(content):
            input_include.chunks.append(content[end: m.start(0)])
            end = m.end(0)

            comment_instruction = m[2]
            if comment_instruction is not None:
                skip_include = comment_instruction == "amalgamate(skip)"

            include_file_in_parent_dir = os.path.realpath(
                os.path.join(input_parent_dir, m[1]))
            if include_file_in_parent_dir in context.visited_files:
                input_include_files.add(include_file_in_parent_dir)
                continue
            if os.path.exists(include_file_in_parent_dir):
                input_include_files.add(include_file_in_parent_dir)
                process_file(context, include_file_in_parent_dir,
                             search_dirs=search_dirs)
                continue

            include_file_in_search_dir_found = False
            for search_dir in search_dirs:
                include_file_in_search_dir = os.path.realpath(
                    os.path.join(search_dir, m[1]))
                if include_file_in_search_dir in context.visited_files:
                    input_include_files.add(include_file_in_search_dir)
                    include_file_in_search_dir_found = True
                    break
                if os.path.exists(include_file_in_search_dir):
                    input_include_files.add(include_file_in_search_dir)
                    process_file(context, include_file_in_search_dir,
                                 search_dirs=search_dirs)
                    include_file_in_search_dir_found = True
                    break
            if include_file_in_search_dir_found:
                continue

            if skip_include:
                print("Skipped: {}".format(m[1]))
                input_include.chunks.append(m[0])
                continue

            raise Exception("Not found: {}".format(m[1]))
        input_include.chunks.append(content[end:])


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
    base_dir: os.PathLike, inputs: Sequence[os.PathLike], output: os.PathLike,
    search_dirs: Sequence[os.PathLike] = []
):
    context = ProcessorContext(base_dir=base_dir)
    for input in inputs:
        process_file(context, input, search_dirs=search_dirs)
    output = os.path.realpath(output)
    print("Sorting...")
    graph = dict(
        sorted(
            tuple((k, sorted(v)) for k, v in context.graph.items()), key=lambda i: i[0]
        )
    )
    sorter = graphlib.TopologicalSorter(graph)
    ordered_includes = tuple(
        map(lambda x: context.includes[x], sorter.static_order()))
    os.makedirs(os.path.dirname(output), exist_ok=True)
    with open(output, mode="w", encoding="utf-8") as f:
        if len(context.ordered_copyright_notices) > 0:
            print("Embedding copyright notices...")
            for notice in context.ordered_copyright_notices:
                f.write(notice)
                f.write("\n")
        for include in ordered_includes:
            if len(include.chunks) > 0:
                posix_include_relative_path = pathlib.Path(
                    os.path.relpath(include.path, context.base_dir)
                ).as_posix()
                print("Embedding file: {}".format(include.path))
                f.write("// file begin: {}\n".format(posix_include_relative_path))
                for chunk in include.chunks:
                    f.write(chunk)
                f.write("\n// file end: {}\n".format(posix_include_relative_path))


def reformat_file(file: os.PathLike, clang_format_exe: os.PathLike):
    file = os.path.realpath(file)
    resolved_clang_format_exe = shutil.which(clang_format_exe)
    if resolved_clang_format_exe is None:
        raise Exception("clang-format not found: {}".format(clang_format_exe))
    print("Reformatting file: {}".format(file))
    subprocess.check_call((resolved_clang_format_exe, "-i", file))


def main(options):
    base_dir = os.getcwd() if options.base is None else options.base
    output_path = os.path.realpath(options.output)
    # Use a suffix that clang-format recognizes as C++
    with TemporaryDirectory() as temp_dir:
        try:
            amalgamated_path = os.path.join(temp_dir, "amalgamated.hh")
            amalgamate(base_dir, options.input, amalgamated_path,
                       search_dirs=options.search)
            reformat_file(amalgamated_path,
                          clang_format_exe=options.clang_format_exe)
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            shutil.move(amalgamated_path, output_path)
            print("Saved output to file: {}".format(output_path))
        finally:
            shutil.rmtree(temp_dir)


def parse_args():
    parser = ArgumentParser(
        prog="amalgamate",
        description="Combines a C or C++ source/header hierarchy into one file",
    )
    parser.add_argument("--base", help="Base directory")
    parser.add_argument(
        "--clang-format-exe", help="clang-format executable", default="clang-format"
    )
    parser.add_argument("--search", nargs="+", help="Header search directory")
    parser.add_argument("--output", help="Output file")
    parser.add_argument("input", nargs="+", help="Input file")
    return parser.parse_args()


if __name__ == "__main__":
    main(parse_args())
