import argparse
import collections
import lzma
import multiprocessing
import pathlib
import sys

from asm_slice import DUMPS, ROOT, unpack_asm


def is_address(text):
	return len(text) >= 6 and all(c in b"0123456789abcdef" for c in text)


def split_comment(line):
	parts = line.split(None, 2)
	if len(parts) < 2 or not is_address(parts[0]):
		return None, -1
	return parts[1], line.rfind(b"; ")


def find_auto_comments(path):
	comments = collections.Counter()
	mnemonics = collections.Counter()
	with open(path, "rb") as file:
		for raw in file:
			line = raw.rstrip(b"\r\n")
			mnemonic, index = split_comment(line)
			if mnemonic is None:
				continue
			mnemonics[mnemonic] += 1
			if index >= 0:
				comments[(mnemonic, line[index + 2:])] += 1
	return {pair for pair, count in comments.items() if count >= 50 and count >= mnemonics[pair[0]] * 0.3 and is_description(pair[1])}


def is_description(comment):
	return len(comment) >= 3 and comment[:1].isupper()


def pack(dump):
	folder = ROOT / DUMPS[dump]
	asm_path = folder / "ship_asm.txt"
	packed_path = folder / "ship_asm.txt.xz"
	temp_path = folder / "ship_asm.txt.xz.tmp"
	auto_comments = find_auto_comments(asm_path)
	with open(asm_path, "rb") as source, lzma.open(temp_path, "wb", preset=6) as target:
		for raw in source:
			line = raw.rstrip(b"\r\n")
			ending = raw[len(line):]
			mnemonic, index = split_comment(line)
			if index >= 0 and (mnemonic, line[index + 2:]) in auto_comments:
				line = line[:index].rstrip()
			target.write(line + ending)
	temp_path.replace(packed_path)
	return f"{dump}: {asm_path.stat().st_size / 1e6:.1f} MB to {packed_path.stat().st_size / 1e6:.1f} MB, {len(auto_comments)} auto comments removed"


def main():
	parser = argparse.ArgumentParser(description="Pack ship_asm.txt to ship_asm.txt.xz for git, or unpack it for asm_slice.py and rg.")
	parser.add_argument("action", choices=["pack", "unpack"], help="pack removes the IDA instruction descriptions and keeps the line count, so func_index.txt stays valid")
	parser.add_argument("dumps", nargs="*", help=f"one or more of {', '.join(DUMPS)}. Default: all dumps")
	args = parser.parse_args()
	for dump in args.dumps:
		if dump not in DUMPS:
			parser.error(f"unknown dump {dump}")
	dumps = args.dumps or [dump for dump in DUMPS if (ROOT / DUMPS[dump]).exists()]

	if args.action == "unpack":
		for dump in dumps:
			unpack_asm(ROOT / DUMPS[dump])
		return

	with multiprocessing.Pool(len(dumps)) as pool:
		for result in pool.imap_unordered(pack, dumps):
			print(result, flush=True)


if __name__ == "__main__":
	sys.stdout.reconfigure(errors="replace")
	main()
