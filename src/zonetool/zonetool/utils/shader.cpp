#include <std_include.hpp>

#include "shader.hpp"
#include <zlib.h>
#include "dxbc_checksum.hpp"

#include <utils/bit_buffer.hpp>
#include <utils/io.hpp>

namespace shader
{
	namespace fxcd
	{
		// https://github.com/hellokenlee/fxcd

		// Create a temp file path with a specific extension
		inline std::wstring MakeTempPath(const wchar_t* ext) {
			wchar_t dir[MAX_PATH]{};
			GetTempPathW(MAX_PATH, dir);
			wchar_t tmp[MAX_PATH]{};
			GetTempFileNameW(dir, L"fxcd", 0, tmp);
			std::wstring path = tmp;
			// swap extension
			size_t dot = path.find_last_of(L'.');
			if (dot != std::wstring::npos) path.resize(dot);
			path += ext;
			return path;
		}

		// Run a command, capture combined stdout/stderr to `outLog`, return exit code (-1 on failure)
		inline int RunProcessCapture(std::wstring cmd, std::string& outLog) {
			SECURITY_ATTRIBUTES sa{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
			HANDLE r = nullptr, w = nullptr;
			if (!CreatePipe(&r, &w, &sa, 0)) return -1;
			SetHandleInformation(r, HANDLE_FLAG_INHERIT, 0);

			STARTUPINFOW si{};
			si.cb = sizeof(si);
			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdOutput = w;
			si.hStdError = w;
			si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

			PROCESS_INFORMATION pi{};
			std::vector<wchar_t> mutableCmd(cmd.begin(), cmd.end());
			mutableCmd.push_back(L'\0');

			BOOL ok = CreateProcessW(nullptr, mutableCmd.data(),
				nullptr, nullptr, TRUE,
				CREATE_NO_WINDOW, nullptr, nullptr,
				&si, &pi);

			CloseHandle(w);

			if (!ok) {
				CloseHandle(r);
				outLog = "CreateProcessW failed";
				return -1;
			}

			WaitForSingleObject(pi.hProcess, INFINITE);

			char buf[4096];
			DWORD got = 0;
			while (ReadFile(r, buf, sizeof(buf), &got, nullptr) && got) {
				outLog.append(buf, buf + got);
			}

			DWORD code = 0;
			GetExitCodeProcess(pi.hProcess, &code);

			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			CloseHandle(r);

			return static_cast<int>(code);
		}

		// --- Disassemble DXBC -> ASM file(s) with fxcd ---
		// disMode: L"--disassemble-ms" (MS) or L"--disassemble" / L"--disassemble-flugan" / L"--decompile"
		// extraFlags: e.g., L"-16" to patch CB offsets; L"-V" to validate
		// Returns true and fills asmOut if either <in>.msasm or <in>.asm was produced.
		bool DxbcToAsmWithFxcd(const void* dxbc, size_t size,
			std::wstring fxcdPath,
			std::wstring disMode,
			std::wstring extraFlags,
			std::string& asmOut,
			std::string* logOut)
		{
			if (!dxbc || size < 4) return false;

			const bool isMS = (disMode == L"--disassemble-ms");

			// write input
			std::wstring inPath = MakeTempPath(L".dxbc");
			{
				std::ofstream ofs(inPath, std::ios::binary);
				if (!ofs) return false;
				ofs.write(reinterpret_cast<const char*>(dxbc), static_cast<std::streamsize>(size));
			}
			std::wstring outPath = inPath;
			outPath.replace(outPath.size() - 5, 5, isMS ? L".msasm" : L".asm");

			// run fxcd
			std::wstring cmd = L"\"" + fxcdPath + L"\" " + disMode + L" ";
			if (!extraFlags.empty()) cmd += extraFlags + L" ";
			cmd += L"\"" + inPath + L"\"";

			std::string log;
			int code = RunProcessCapture(cmd, log);
			if (logOut) *logOut = log;

			bool ok = false;
			if (code == 0) {
				std::ifstream ifs(outPath, std::ios::binary);
				if (ifs) {
					asmOut.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
					ok = !asmOut.empty();
				}
			}

			// cleanup
			DeleteFileW(inPath.c_str());
			DeleteFileW(outPath.c_str());
			return ok;
		}

		// --- Assemble ASM -> DXBC using fxcd --assemble ---
		// fxcd writes "<asmPath>.shdr". We read that back.
		// If you want to preserve reflection/signatures, pass originalDxbc to use "--copy-reflection".
		bool AsmToDxbcWithFxcd(const std::string& asmText,
			std::wstring fxcdPath,
			std::wstring extraAssembleFlags,   // e.g., L"-V"
			const void* originalDxbc,
			size_t originalDxbcSize,
			std::vector<uint8_t>& outDxbc,
			std::string* logOut)
		{
			// write asm
			std::wstring asmPath = MakeTempPath(L".asm");
			{
				std::ofstream ofs(asmPath, std::ios::binary);
				if (!ofs) return false;
				ofs.write(asmText.data(), static_cast<std::streamsize>(asmText.size()));
			}

			// optional reflection
			std::wstring reflPath;
			if (originalDxbc && originalDxbcSize) {
				reflPath = MakeTempPath(L".dxbc");
				std::ofstream ofs(reflPath, std::ios::binary);
				if (!ofs) { DeleteFileW(asmPath.c_str()); return false; }
				ofs.write(reinterpret_cast<const char*>(originalDxbc), static_cast<std::streamsize>(originalDxbcSize));
			}

			// output is "<asm>.shdr"
			std::wstring outPath = asmPath;
			outPath.replace(outPath.size() - 4, 4, L".shdr");

			// run fxcd
			std::wstring cmd = L"\"" + fxcdPath + L"\" --assemble ";
			if (!extraAssembleFlags.empty()) cmd += extraAssembleFlags + L" ";
			if (!reflPath.empty())           cmd += L"--copy-reflection \"" + reflPath + L"\" ";
			cmd += L"\"" + asmPath + L"\"";

			std::string log;
			int code = RunProcessCapture(cmd, log);
			if (logOut) *logOut = log;

			bool ok = false;
			if (code == 0) {
				std::ifstream ifs(outPath, std::ios::binary);
				if (ifs) {
					outDxbc.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
					ok = !outDxbc.empty();
				}
			}

			// cleanup
			DeleteFileW(asmPath.c_str());
			if (!reflPath.empty()) DeleteFileW(reflPath.c_str());
			DeleteFileW(outPath.c_str());
			return ok;
		}
	}

	namespace
	{
		unsigned char* parse_shader_chunk(unsigned char* program, unsigned int program_size, unsigned int* chunk_size)
		{
			const auto header = reinterpret_cast<dx11_shader_header*>(program);

			auto shader_chunk_offset = 0;
			for (auto i = 0u; i < header->chunk_count; i++)
			{
				const auto offset = *reinterpret_cast<int*>(
					reinterpret_cast<size_t>(program) + sizeof(dx11_shader_header) + sizeof(int) * i);

				const auto type = *reinterpret_cast<int*>(reinterpret_cast<size_t>(program) + offset);
				if (type == 'RDHS' || type == 'XEHS')
				{
					shader_chunk_offset = offset;
				}

				if (i >= header->chunk_count - 1)
				{
					*chunk_size = header->program_size - shader_chunk_offset;
				}
				else
				{
					const auto next_offset = *reinterpret_cast<int*>(
						reinterpret_cast<size_t>(program) + sizeof(dx11_shader_header) + sizeof(int) * (i + 1));
					*chunk_size = next_offset - shader_chunk_offset;
				}
			}

			return reinterpret_cast<unsigned char*>(reinterpret_cast<size_t>(program) + shader_chunk_offset);
		}

		unsigned long get_crc32(void* data_address, std::uint32_t data_len)
		{
			unsigned long crc = crc32(0L, Z_NULL, 0);
			crc = crc32(crc, reinterpret_cast<const unsigned char*>(data_address), data_len);
			return crc;
		}

		std::string get_crc32_str(void* data_address, std::uint32_t data_len)
		{
			return std::to_string(get_crc32(data_address, data_len));
		}
	}

	std::vector<size_t> get_dest_reference_offsets(unsigned char* program, unsigned int program_size)
	{
		std::vector<size_t> offsets;

		unsigned int chunk_size{};
		auto chunk = parse_shader_chunk(program, program_size, &chunk_size);
		const auto offset = chunk - program;

		utils::bit_buffer_le bit_buffer({reinterpret_cast<char*>(chunk), chunk_size});

		if (chunk_size == 0)
		{
			utils::io::write_file("shader.cso", std::string{(char*)program, program_size}, false);
		}

		[[maybe_unused]] const auto type = bit_buffer.read_bytes(4); // "SHEX"
		assert(type == 'XEHS' || type == 'RDHS');
		[[maybe_unused]] const auto len = bit_buffer.read_bytes(4);
		[[maybe_unused]] const auto minor_version = bit_buffer.read_bits(4);
		[[maybe_unused]] const auto major_version = bit_buffer.read_bits(4);
		assert(major_version == 5);
		[[maybe_unused]] const auto program_type = bit_buffer.read_bytes(1);
		assert(program_type >= 0 && program_type <= 5);

		bit_buffer.read_bytes(2);
		[[maybe_unused]] const auto num_dwords = bit_buffer.read_bytes(4);

		while (bit_buffer.total() < len * 8)
		{
			[[maybe_unused]] const auto opcode = bit_buffer.read_bits(10);
			bit_buffer.read_bits(14);
			[[maybe_unused]] const auto opcode_len = bit_buffer.read_bits(6);
			auto extended = bit_buffer.read_bits(1);
			bit_buffer.read_bits(1);

			while (extended)
			{
				bit_buffer.read_bits(31);
				extended = bit_buffer.read_bits(1);
			}

			std::function<void()> read_operand;

			read_operand = [&]()
			{
				const auto num_components_type = bit_buffer.read_bits(1);
				bit_buffer.read_bits(11);
				const auto operand_type = bit_buffer.read_bits(7);
				bit_buffer.read_bits(1);
				[[maybe_unused]] const auto dimension = bit_buffer.read_bits(1);

				auto num_components = 0;
				switch (num_components_type)
				{
				case 0:
					num_components = 0;
					break;
				case 1:
					num_components = 1;
					break;
				case 2:
					num_components = 4;
					break;
				case 3:
					throw std::runtime_error("D3D10_SB_OPERAND_N_COMPONENT not supported");
					break;
				}

				for (auto i = 0; i < 3; i++)
				{
					bit_buffer.read_bits(1);
					bit_buffer.read_bits(2);
				}

				bit_buffer.read_bits(1);

				const auto extended_operand = bit_buffer.read_bits(1);
				if (extended_operand)
				{
					bit_buffer.read_bytes(4);
				}

				/*uint64_t values[3]{};
				for (auto i = 0; i < dimension; i++)
				{
					switch (index_representations[i])
					{
					case 0:
						values[i] = bit_buffer.read_bytes(4);
						break;
					case 1:
						values[i] = bit_buffer.read_uint64();
					case 2:
						read_operand();
						break;
					case 3:
						values[i] = bit_buffer.read_bytes(4);
						read_operand();
						break;
					case 4:
						values[i] = bit_buffer.read_uint64();
						read_operand();
						break;
					default:
						throw std::runtime_error("Invalid representation");
					}
					printf("%i ", values[i]);
				}*/

				if (operand_type == 8)
				{
					const auto cb_index = bit_buffer.read_bytes(4);
					const auto dest_offset = bit_buffer.total() / 8;
					const auto dest = bit_buffer.read_bytes(4);

					if (cb_index <= 4 && dest < 0x7F) // workaround
					{
						offsets.push_back(dest_offset + offset);
					}
				}

				switch (operand_type)
				{
				case (4):
					for (auto i = 0; i < num_components; i++)
					{
						bit_buffer.read_bytes(4);
					}
					break;
				case (5):
					for (auto i = 0; i < num_components; i++)
					{
						bit_buffer.read_bytes(4);
						bit_buffer.read_bytes(4);
					}
					break;
				}
			};

			// parse operands
			if (opcode_len > 0)
			{
				const auto bits_to_read = (4 * 8) * (opcode_len - 1);
				const auto end = bit_buffer.total() + bits_to_read;
				auto total = bit_buffer.total();

				if (opcode == 120)
				{
					bit_buffer.read_bytes(4);
				}

				while (bit_buffer.total() < end)
				{
					read_operand();
					total = bit_buffer.total();
				}

				bit_buffer.set_bit(end);
			}
		}

		return offsets;
	}

	shader_checksum generate_checksum(unsigned char* program, unsigned int program_size)
	{
		shader_checksum checksum{};
		dxbc::CalculateDXBCChecksum(program, program_size, reinterpret_cast<DWORD*>(&checksum));
		return checksum;
	}

	unsigned int calc_crc32(unsigned char* program, unsigned int program_size)
	{
		if (program == nullptr)
		{
			return 0;
		}

		return get_crc32(program, program_size);
	}
}
