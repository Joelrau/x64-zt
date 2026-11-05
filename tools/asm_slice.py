import argparse
import bisect
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent / "re_reference"
DUMPS = {
	"h1": "h1/mp_1.15",
	"h1_1.4": "h1/mp_1.4",
	"h2": "h2/ship",
	"iw6": "iw6/ship",
	"iw7": "iw7/ship",
	"s1": "s1/ship",
	"s2": "s2/ship",
}
FUNC_LINE = re.compile(r"^(\d+):; FUNC ([0-9a-fA-F]+) ([0-9a-fA-F]+) (.*)$")
IMAGE_BASE = 0x140000000


class function_index:
	def __init__(self, folder):
		self.starts = []
		self.entries = []
		with open(folder / "func_index.txt", encoding="utf-8", errors="replace") as file:
			for line in file:
				match = FUNC_LINE.match(line.strip())
				if match:
					start = int(match.group(2), 16)
					self.starts.append(start)
					self.entries.append((start, int(match.group(3), 16), int(match.group(1)), match.group(4)))
		self.rebased = bool(self.starts) and self.starts[0] < IMAGE_BASE

	def normalize(self, address):
		if self.rebased and address >= IMAGE_BASE:
			return address - IMAGE_BASE
		if not self.rebased and address < IMAGE_BASE:
			return address + IMAGE_BASE
		return address

	def find_address(self, address):
		address = self.normalize(address)
		index = bisect.bisect_right(self.starts, address) - 1
		if index >= 0:
			entry = self.entries[index]
			if entry[0] <= address <= entry[1]:
				return entry
		return None

	def find_name(self, pattern):
		regex = re.compile(pattern)
		return [entry for entry in self.entries if regex.search(entry[3])]

	def next_line(self, entry):
		index = self.entries.index(entry)
		return self.entries[index + 1][2] if index + 1 < len(self.entries) else None


def read_lines(asm_path, first, last):
	result = []
	with open(asm_path, encoding="utf-8", errors="replace") as file:
		for number, line in enumerate(file, 1):
			if number > last:
				break
			if number >= first:
				result.append(line.rstrip("\n"))
	return result


def parse_address(text):
	try:
		return int(text, 16)
	except ValueError:
		return None


def print_function(index, asm_path, entry, around, max_lines):
	start, end, line, name = entry
	last = (index.next_line(entry) or line + max_lines) - 1
	lines = read_lines(asm_path, line, last)
	if around is not None:
		key = format(index.normalize(around), "x")
		hit = next((i for i, text in enumerate(lines) if text.startswith(key)), None)
		if hit is not None:
			lines = lines[max(0, hit - max_lines // 2):hit + max_lines // 2]
	print(f"; {name} {start:x}..{end:x} (ship_asm.txt line {line})")
	print("\n".join(lines[:max_lines]))
	if len(lines) > max_lines:
		print(f"; ... {len(lines) - max_lines} more lines, raise --max-lines")


def print_callers(asm_path, index, target):
	regex = re.compile(r"\bcall\s+(cs:)?" + re.escape(target) + r"\b")
	seen = set()
	with open(asm_path, encoding="utf-8", errors="replace") as file:
		for text in file:
			if regex.search(text):
				address = parse_address(text.split(maxsplit=1)[0])
				entry = index.find_address(address) if address is not None else None
				caller = entry[3] if entry else "?"
				if (caller, address) not in seen:
					seen.add((caller, address))
					print(f"{address:x} in {caller}")


def main():
	parser = argparse.ArgumentParser(description="Print functions from a re_reference IDA dump without reading the whole file.")
	parser.add_argument("dump", choices=DUMPS.keys(), help="h1 = mp_1.15 (RVAs), h1_1.4 = the build that h1-mod runs (full addresses)")
	parser.add_argument("targets", nargs="+", help="hex addresses (RVA or 0x140...) or a name regex")
	parser.add_argument("-l", "--list", action="store_true", help="print the containing function of each target only (use for call stacks)")
	parser.add_argument("-a", "--around", action="store_true", help="print only --max-lines around the address instead of the whole function")
	parser.add_argument("-c", "--callers", action="store_true", help="list every call site of the named function")
	parser.add_argument("-m", "--max-lines", type=int, default=400)
	args = parser.parse_args()

	folder = ROOT / DUMPS[args.dump]
	asm_path = folder / "ship_asm.txt"
	index = function_index(folder)

	for target in args.targets:
		address = parse_address(target.removeprefix("0x").removeprefix("sub_"))
		if address is not None:
			entry = index.find_address(address)
			if entry is None:
				print(f"{target}: no function")
				continue
			entries = [entry]
		else:
			entries = index.find_name(target)
			if not entries:
				print(f"{target}: no function name matches")
				continue

		for entry in entries[:20]:
			if args.list:
				offset = f"+0x{index.normalize(address) - entry[0]:x}" if address is not None else ""
				print(f"{target}: {entry[3]}{offset} ({entry[0]:x}..{entry[1]:x}, line {entry[2]})")
			elif args.callers:
				print_callers(asm_path, index, entry[3])
			else:
				print_function(index, asm_path, entry, address if args.around else None, args.max_lines)


if __name__ == "__main__":
	sys.stdout.reconfigure(errors="replace")
	main()
