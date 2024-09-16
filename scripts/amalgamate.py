from argparse import ArgumentParser
from dataclasses import dataclass
import os
import re


@dataclass
class ProcessorContext:
    include_pattern = re.compile(r'#include "([^"]+)"\n')
    block_comment_pattern = re.compile(r"^/\*.*?\*/\n", re.DOTALL)
    visited_files = set()
    ordered_files = []
    chunks = []
    visited_copyright_notices = set()


def process_file(context: ProcessorContext, input: os.PathLike):
    input = os.path.realpath(input)

    if input in context.visited_files:
        return
    context.visited_files.add(input)

    if not os.path.exists(input):
        raise Exception("Input file not found: {}".format(input))

    print("Processing file: {}".format(input))

    with open(input, encoding="utf-8") as f:
        content = f.read()

        content = context.block_comment_pattern.sub(
            lambda m: replace_copyright(context, m), content
        )

        end = 0
        for m in context.include_pattern.finditer(content):
            context.chunks.append(content[end : m.start(0)])
            end = m.end(0)
            include_file = os.path.join(os.path.dirname(input), m[1])

            if include_file in context.visited_files:
                continue

            if not os.path.exists(include_file):
                print("File not found: {}".format(include_file))
                context.chunks.append(m[0])
                continue

            context.chunks.append("// File: {}\n".format(m[1]))
            process_file(context, include_file)
        context.chunks.append(content[end:])


def replace_copyright(context: ProcessorContext, match: re.Match[str]):
    if not "copyright".casefold() in match[0].casefold():
        return match[0]
    hashed = hash(match[0])
    if hashed in context.visited_copyright_notices:
        return ""
    context.visited_copyright_notices.add(hashed)
    return match[0]


def main(options):
    context = ProcessorContext()
    for input in options.input:
        process_file(context, input)
    with open(os.path.realpath(options.output), mode="w", encoding="utf-8") as f:
        for chunk in context.chunks:
            f.write(chunk)


def parse_args():
    parser = ArgumentParser(
        prog="amalgamate",
        description="Combines a C or C++ source/header hierarchy into one file",
    )
    parser.add_argument("input", nargs="+", help="Input file")
    parser.add_argument("output", help="Output file")
    return parser.parse_args()


if __name__ == "__main__":
    main(parse_args())
