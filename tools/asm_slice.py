import argparse
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
DUMPS = {
    "h1": ROOT / "reference/h1/mp_1.15/ship_asm.txt",
    "h1_1.4": ROOT / "reference/h1/mp_1.4/h1_asm.txt",
    "iw7": ROOT / "reference/iw7/ship/ship_asm.txt",
}


def find_functions(dump, pattern):
    regex = re.compile(pattern)
    with open(dump, encoding="utf-8", errors="replace") as file:
        lines = []
        name = None
        for line in file:
            if line.startswith("; FUNC "):
                if name is not None:
                    yield name, lines
                parts = line.split(maxsplit=4)
                name = parts[4].strip() if regex.search(parts[4]) else None
                lines = [line]
            elif name is not None:
                lines.append(line)
        if name is not None:
            yield name, lines


def main():
    parser = argparse.ArgumentParser(description="Print functions from an IDA asm dump.")
    parser.add_argument("pattern", help="regular expression for the function name")
    parser.add_argument("-g", "--game", default="h1", choices=DUMPS.keys())
    parser.add_argument("-l", "--list", action="store_true", help="print names only")
    parser.add_argument("-m", "--max", type=int, default=20, help="maximum function count")
    args = parser.parse_args()

    count = 0
    for name, lines in find_functions(DUMPS[args.game], args.pattern):
        sys.stdout.write(name + "\n" if args.list else "".join(lines) + "\n")
        count += 1
        if count >= args.max:
            break


if __name__ == "__main__":
    main()
