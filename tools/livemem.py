import argparse
import ctypes
import ctypes.wintypes as wintypes
import struct
import sys

import numpy

PROCESS_QUERY_INFORMATION = 0x0400
PROCESS_VM_READ = 0x0010
MEM_COMMIT = 0x1000
PAGE_GUARD = 0x100
PAGE_READABLE = 0x02 | 0x04 | 0x08 | 0x20 | 0x40 | 0x80
CHUNK_SIZE = 64 << 20

# H1 offsets from src/games/h1/structs.hpp
H1_MATERIAL_STATE_BITS_ENTRY = 0x30
H1_MATERIAL_STATE_BITS_COUNT = 0x122
H1_MATERIAL_TECHNIQUE_SET = 0x128
H1_MATERIAL_STATE_BITS_TABLE = 0x140
H1_TECHSET_TECHNIQUES = 0x10
H1_TECHNIQUE_PASS_COUNT = 0x0A
H1_TECHNIQUE_COUNT = 240
H1_STATE_BITS_SIZE = 0x28

kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)


class memory_basic_information(ctypes.Structure):
	_fields_ = [
		("base_address", ctypes.c_uint64),
		("allocation_base", ctypes.c_uint64),
		("allocation_protect", wintypes.DWORD),
		("partition_id", wintypes.DWORD),
		("region_size", ctypes.c_uint64),
		("state", wintypes.DWORD),
		("protect", wintypes.DWORD),
		("type", wintypes.DWORD),
		("padding", wintypes.DWORD),
	]


kernel32.OpenProcess.restype = wintypes.HANDLE
kernel32.VirtualQueryEx.argtypes = [wintypes.HANDLE, ctypes.c_uint64, ctypes.POINTER(memory_basic_information), ctypes.c_size_t]
kernel32.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_uint64, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]


class process:
	def __init__(self, pid):
		self.handle = kernel32.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, False, pid)
		if not self.handle:
			raise SystemExit(f"cannot open process {pid} (error {ctypes.get_last_error()})")

	def read(self, address, size):
		buffer = ctypes.create_string_buffer(size)
		count = ctypes.c_size_t()
		if not kernel32.ReadProcessMemory(self.handle, address, buffer, size, ctypes.byref(count)):
			return None
		return buffer.raw[:count.value]

	def read_u64(self, address):
		data = self.read(address, 8)
		return struct.unpack("<Q", data)[0] if data else 0

	def read_string(self, address):
		data = self.read(address, 256) if address else None
		return data.split(b"\0")[0].decode("latin1") if data else None

	def regions(self):
		address = 0
		info = memory_basic_information()
		while kernel32.VirtualQueryEx(self.handle, address, ctypes.byref(info), ctypes.sizeof(info)):
			if info.state == MEM_COMMIT and info.protect & PAGE_READABLE and not info.protect & PAGE_GUARD:
				yield info.base_address, info.region_size
			address = info.base_address + info.region_size


def find_references(proc, target, stride, count):
	lowest = target - stride * count
	for base, size in proc.regions():
		for offset in range(0, size, CHUNK_SIZE):
			data = proc.read(base + offset, min(size - offset, CHUNK_SIZE))
			if not data:
				continue
			values = numpy.frombuffer(data[:len(data) // 8 * 8], "<u8")
			for index in numpy.nonzero((values <= target) & (values >= lowest))[0]:
				value = int(values[index])
				if (target - value) % stride == 0:
					yield base + offset + int(index) * 8, (target - value) // stride


def print_h1_material(proc, material):
	data = proc.read(material, H1_MATERIAL_STATE_BITS_TABLE + 8)
	if not data:
		print(f"{material:x}: not readable")
		return
	entries = data[H1_MATERIAL_STATE_BITS_ENTRY:H1_MATERIAL_STATE_BITS_ENTRY + H1_TECHNIQUE_COUNT]
	state_bits_count = data[H1_MATERIAL_STATE_BITS_COUNT]
	techset = proc.read_u64(material + H1_MATERIAL_TECHNIQUE_SET)
	print(f"material {proc.read_string(proc.read_u64(material))}")
	print(f"techset {proc.read_string(proc.read_u64(techset))}, stateBitsCount {state_bits_count}, stateBitsTable {proc.read_u64(material + H1_MATERIAL_STATE_BITS_TABLE):x}")
	for index in range(H1_TECHNIQUE_COUNT):
		technique = proc.read_u64(techset + H1_TECHSET_TECHNIQUES + index * 8)
		if not technique:
			continue
		pass_count = struct.unpack("<H", proc.read(technique + H1_TECHNIQUE_PASS_COUNT, 2))[0]
		entry = entries[index]
		bad = entry == 0xFF or entry + pass_count > state_bits_count
		print(f"{'BAD ' if bad else '    '}{index:3} entry {entry:3} passes {pass_count} {proc.read_string(proc.read_u64(technique))}")


def main():
	parser = argparse.ArgumentParser(description="Read the memory of a live process, for example a game that shows its crash dialog.")
	parser.add_argument("pid", type=int)
	commands = parser.add_subparsers(dest="command", required=True)
	read = commands.add_parser("read", help="hex dump of <address>")
	read.add_argument("address")
	read.add_argument("size", nargs="?", default="40")
	refs = commands.add_parser("refs", help="find qwords that point to <address> - k * stride (an array base that reaches <address>)")
	refs.add_argument("address")
	refs.add_argument("--stride", default="8")
	refs.add_argument("--count", type=int, default=300)
	refs.add_argument("--field", default="0", help="offset of the pointer field in its owner struct; prints the owner name")
	h1_material = commands.add_parser("h1-material", help="print an H1 Material and the techniques that have no valid state bits")
	h1_material.add_argument("address")
	args = parser.parse_args()

	proc = process(args.pid)
	if args.command == "read":
		data = proc.read(int(args.address, 16), int(args.size, 16))
		print(data.hex(" ", 8) if data else "not readable")
	elif args.command == "refs":
		field = int(args.field, 16)
		for address, index in find_references(proc, int(args.address, 16), int(args.stride, 16), args.count):
			owner = address - field
			print(f"{address:x} index {index} owner {owner:x} name {proc.read_string(proc.read_u64(owner))!r:.80}")
	elif args.command == "h1-material":
		print_h1_material(proc, int(args.address, 16))


if __name__ == "__main__":
	sys.stdout.reconfigure(encoding="utf-8", errors="replace")
	main()
