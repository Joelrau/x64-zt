#include <std_include.hpp>

#include "shader.hpp"
#include <zlib.h>
#include "dxbc_checksum.hpp"

#include <utils/io.hpp>

// https://github.com/jingoro2112/dna/blob/master/dnausb/win32/ddk/api/d3d11TokenizedProgramFormat.hpp

namespace shader
{
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

	namespace asm_
	{
		std::array<const char*, 107> opcode_names =
		{
			"add",
			"and",
			"break",
			"breakc",
			"call",
			"callc",
			"case",
			"continue",
			"continuec",
			"cut",
			"default",
			"deriv_rtx",
			"deriv_rty",
			"discard",
			"div",
			"dp2",
			"dp3",
			"dp4",
			"else",
			"emit",
			"emitThenCut",
			"endif",
			"endloop",
			"endswitch",
			"eq",
			"exp",
			"frc",
			"ftoi",
			"ftou",
			"ge",
			"iadd",
			"if",
			"ieq",
			"ige",
			"ilt",
			"imad",
			"imax",
			"imin",
			"imul",
			"ine",
			"ineg",
			"ishl",
			"ishr",
			"itof",
			"label",
			"ld",
			"ldms",
			"log",
			"loop",
			"lt",
			"mad",
			"min",
			"max",
			"custom_data",
			"mov",
			"movc",
			"mul",
			"ne",
			"nop",
			"not",
			"or",
			"resinfo",
			"ret",
			"retc",
			"round_ne",
			"round_ni",
			"round_pi",
			"round_z",
			"rsq",
			"sample",
			"sample_c",
			"sample_c_lz",
			"sample_l",
			"sample_d",
			"sample_b",
			"sqrt",
			"switch",
			"sincos",
			"udiv",
			"ult",
			"uge",
			"umul",
			"umad",
			"umax",
			"umin",
			"ushr",
			"utof",
			"xor",
			"dcl_resource",
			"dcl_constantbuffer",
			"dcl_sampler",
			"dcl_indexrange",
			"dcl_outputtopology",
			"dcl_inputprimitive",
			"dcl_maxout",
			"dcl_input",
			"dcl_input_sgv",
			"dcl_input_siv",
			"dcl_input_ps",
			"dcl_input_ps_sgv",
			"dcl_input_ps_siv",
			"dcl_output",
			"dcl_output_sgv",
			"dcl_output_siv",
			"dcl_temps",
			"dcl_indexableTemp",
			"dcl_globalFlags",
		};

		std::unordered_set<std::uint32_t> allowed_opcodes =
		{
			D3D10_SB_OPCODE_ADD,
			D3D10_SB_OPCODE_AND,
			D3D10_SB_OPCODE_BREAK,
			D3D10_SB_OPCODE_BREAKC,
			D3D10_SB_OPCODE_CALL,
			D3D10_SB_OPCODE_CALLC,
			D3D10_SB_OPCODE_CASE,
			D3D10_SB_OPCODE_CONTINUE,
			D3D10_SB_OPCODE_CONTINUEC,
			D3D10_SB_OPCODE_CUT,
			D3D10_SB_OPCODE_DEFAULT,
			D3D10_SB_OPCODE_DERIV_RTX,
			D3D10_SB_OPCODE_DERIV_RTY,
			D3D10_SB_OPCODE_DISCARD,
			D3D10_SB_OPCODE_DIV,
			D3D10_SB_OPCODE_DP2,
			D3D10_SB_OPCODE_DP3,
			D3D10_SB_OPCODE_DP4,
			D3D10_SB_OPCODE_ELSE,
			D3D10_SB_OPCODE_EMIT,
			D3D10_SB_OPCODE_EMITTHENCUT,
			D3D10_SB_OPCODE_ENDIF,
			D3D10_SB_OPCODE_ENDLOOP,
			D3D10_SB_OPCODE_ENDSWITCH,
			D3D10_SB_OPCODE_EQ,
			D3D10_SB_OPCODE_EXP,
			D3D10_SB_OPCODE_FRC,
			D3D10_SB_OPCODE_FTOI,
			D3D10_SB_OPCODE_FTOU,
			D3D10_SB_OPCODE_GE,
			D3D10_SB_OPCODE_IADD,
			D3D10_SB_OPCODE_IF,
			D3D10_SB_OPCODE_IEQ,
			D3D10_SB_OPCODE_IGE,
			D3D10_SB_OPCODE_ILT,
			D3D10_SB_OPCODE_IMAD,
			D3D10_SB_OPCODE_IMAX,
			D3D10_SB_OPCODE_IMIN,
			D3D10_SB_OPCODE_IMUL,
			D3D10_SB_OPCODE_INE,
			D3D10_SB_OPCODE_INEG,
			D3D10_SB_OPCODE_ISHL,
			D3D10_SB_OPCODE_ISHR,
			D3D10_SB_OPCODE_ITOF,
			D3D10_SB_OPCODE_LABEL,
			D3D10_SB_OPCODE_LD,
			D3D10_SB_OPCODE_LD_MS,
			D3D10_SB_OPCODE_LOG,
			D3D10_SB_OPCODE_LOOP,
			D3D10_SB_OPCODE_LT,
			D3D10_SB_OPCODE_MAD,
			D3D10_SB_OPCODE_MIN,
			D3D10_SB_OPCODE_MAX,
			D3D10_SB_OPCODE_CUSTOMDATA,
			D3D10_SB_OPCODE_MOV,
			D3D10_SB_OPCODE_MOVC,
			D3D10_SB_OPCODE_MUL,
			D3D10_SB_OPCODE_NE,
			D3D10_SB_OPCODE_NOP,
			D3D10_SB_OPCODE_NOT,
			D3D10_SB_OPCODE_OR,
			D3D10_SB_OPCODE_RESINFO,
			D3D10_SB_OPCODE_RET,
			D3D10_SB_OPCODE_RETC,
			D3D10_SB_OPCODE_ROUND_NE,
			D3D10_SB_OPCODE_ROUND_NI,
			D3D10_SB_OPCODE_ROUND_PI,
			D3D10_SB_OPCODE_ROUND_Z,
			D3D10_SB_OPCODE_RSQ,
			//D3D10_SB_OPCODE_SAMPLE,
			//D3D10_SB_OPCODE_SAMPLE_C,
			//D3D10_SB_OPCODE_SAMPLE_C_LZ,
			//D3D10_SB_OPCODE_SAMPLE_L,
			//D3D10_SB_OPCODE_SAMPLE_D,
			//D3D10_SB_OPCODE_SAMPLE_B,
			D3D10_SB_OPCODE_SQRT,
			D3D10_SB_OPCODE_SWITCH,
			D3D10_SB_OPCODE_SINCOS,
			D3D10_SB_OPCODE_UDIV,
			D3D10_SB_OPCODE_ULT,
			D3D10_SB_OPCODE_UGE,
			D3D10_SB_OPCODE_UMUL,
			D3D10_SB_OPCODE_UMAD,
			D3D10_SB_OPCODE_UMAX,
			D3D10_SB_OPCODE_UMIN,
			D3D10_SB_OPCODE_USHR,
			D3D10_SB_OPCODE_UTOF,
			D3D10_SB_OPCODE_XOR,

			D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER,
		};

		namespace reader
		{
			operand_extended_t read_extended_operand(utils::bit_buffer_le& bit_buffer)
			{
				operand_extended_t operand{};

				operand.type = bit_buffer.read_bits(6);
				if (operand.type == D3D10_SB_EXTENDED_OPERAND_MODIFIER)
				{
					operand.modifier = bit_buffer.read_bits(8);
					bit_buffer.read_bits(17);
				}
				else
				{
					bit_buffer.read_bits(25);
				}

				operand.extended = bit_buffer.read_bits(1);

				return operand;
			}

			operand_t read_operand(utils::memory::allocator& allocator, utils::bit_buffer_le& bit_buffer)
			{
				operand_t operand{};

				operand.components.type = bit_buffer.read_bits(2);

				switch (operand.components.type)
				{
				case D3D10_SB_OPERAND_4_COMPONENT:
				{
					operand.components.selection_mode = bit_buffer.read_bits(2);
					if (operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE)
					{
						operand.components.mask = bit_buffer.read_bits(4);
						bit_buffer.read_bits(4);
					}
					else if (operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE)
					{
						operand.components.names[0] = bit_buffer.read_bits(2);
						operand.components.names[1] = bit_buffer.read_bits(2);
						operand.components.names[2] = bit_buffer.read_bits(2);
						operand.components.names[3] = bit_buffer.read_bits(2);
					}
					else if (operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE)
					{
						operand.components.names[0] = bit_buffer.read_bits(2);
						bit_buffer.read_bits(6);
					}

					break;
				}
				case D3D10_SB_OPERAND_0_COMPONENT:
				case D3D10_SB_OPERAND_1_COMPONENT:
				default:
					bit_buffer.read_bits(10);
					break;
				}

				operand.type = bit_buffer.read_bits(8);
				operand.dimension = bit_buffer.read_bits(2);

				operand.indices[0].representation = bit_buffer.read_bits(3);
				operand.indices[1].representation = bit_buffer.read_bits(3);
				operand.indices[2].representation = bit_buffer.read_bits(3);

				operand.extended = bit_buffer.read_bits(1);
				auto extended = operand.extended;
				while (extended)
				{
					const auto operand_extended = read_extended_operand(bit_buffer);
					operand.extensions.emplace_back(operand_extended);
					extended = operand_extended.extended;
				}

				const auto read_index_rep = [&](const std::uint32_t index)
				{
					switch (operand.indices[index].representation)
					{
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
						operand.indices[index].values[0].u32 = bit_buffer.read_bytes(4);
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
						operand.indices[index].values[0].u64.fields.low = bit_buffer.read_bytes(4);
						operand.indices[index].values[0].u64.fields.high = bit_buffer.read_bytes(4);
						break;
					case D3D10_SB_OPERAND_INDEX_RELATIVE:
						operand.extra_operand = allocator.allocate<operand_t>();
						*operand.extra_operand = read_operand(allocator, bit_buffer);
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
						operand.indices[index].values[0].u32 = bit_buffer.read_bytes(4);
						operand.extra_operand = allocator.allocate<operand_t>();
						*operand.extra_operand = read_operand(allocator, bit_buffer);
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:
						operand.indices[index].values[0].u64.fields.low = bit_buffer.read_bytes(4);
						operand.indices[index].values[0].u64.fields.high = bit_buffer.read_bytes(4);
						operand.extra_operand = allocator.allocate<operand_t>();
						*operand.extra_operand = read_operand(allocator, bit_buffer);
						break;
					}
				};

				if (operand.dimension >= D3D10_SB_OPERAND_INDEX_1D)
				{
					read_index_rep(0);
				}

				if (operand.dimension >= D3D10_SB_OPERAND_INDEX_2D)
				{
					read_index_rep(1);
				}

				if (operand.dimension == D3D10_SB_OPERAND_INDEX_3D)
				{
					read_index_rep(2);
				}

				if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
				{
					if (operand.components.type == D3D10_SB_OPERAND_4_COMPONENT)
					{
						operand.indices[0].values[0].u32 = bit_buffer.read_bytes(4);
						operand.indices[0].values[1].u32 = bit_buffer.read_bytes(4);
						operand.indices[0].values[2].u32 = bit_buffer.read_bytes(4);
						operand.indices[0].values[3].u32 = bit_buffer.read_bytes(4);
					}
					else
					{
						operand.indices[0].values[0].u32 = bit_buffer.read_bytes(4);
					}
				}

				if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE64)
				{
					operand.indices[0].values[0].u64.fields.low = bit_buffer.read_bytes(4);
					operand.indices[0].values[0].u64.fields.high = bit_buffer.read_bytes(4);
				}

				return operand;
			}

			opcode_extended_t read_opcode_extended(utils::bit_buffer_le& input_buffer)
			{
				opcode_extended_t opcode{};

				opcode.type = input_buffer.read_bits(6);
				switch (opcode.type)
				{
				case D3D10_SB_EXTENDED_OPCODE_SAMPLE_CONTROLS:
					input_buffer.read_bits(3);
					opcode.values[0] = input_buffer.read_bits(4);
					opcode.values[1] = input_buffer.read_bits(4);
					opcode.values[2] = input_buffer.read_bits(4);
					input_buffer.read_bits(10);
					break;
				case D3D11_SB_EXTENDED_OPCODE_RESOURCE_DIM:
					opcode.values[0] = input_buffer.read_bits(5);
					input_buffer.read_bits(20);
					break;
				case D3D11_SB_EXTENDED_OPCODE_RESOURCE_RETURN_TYPE:
					opcode.values[0] = input_buffer.read_bits(4);
					opcode.values[1] = input_buffer.read_bits(4);
					opcode.values[2] = input_buffer.read_bits(4);
					opcode.values[3] = input_buffer.read_bits(4);
					input_buffer.read_bits(9);
					break;
				default:
					input_buffer.read_bits(25);
					break;
				}

				opcode.extended = input_buffer.read_bits(1);

				return opcode;
			}

			opcode_t read_opcode(utils::bit_buffer_le& input_buffer)
			{
				opcode_t opcode{};

				opcode.type = input_buffer.read_bits(11);
				opcode.controls = input_buffer.read_bits(13);
				opcode.length = input_buffer.read_bits(7);
				opcode.extended = input_buffer.read_bits(1);

				auto extended = opcode.extended;
				while (extended)
				{
					const auto opcode_extended = read_opcode_extended(input_buffer);
					opcode.extensions.emplace_back(opcode_extended);
					extended = opcode_extended.extended;
				}

				return opcode;
			}

			instruction_t read_instruction(utils::memory::allocator& allocator, utils::bit_buffer_le& input_buffer)
			{
				instruction_t instruction{};
				instruction.opcode = read_opcode(input_buffer);

				const auto end = input_buffer.total() + (instruction.opcode.length - 1) * 8 * 4;
				while (input_buffer.total() < end)
				{
					const auto operand = read_operand(allocator, input_buffer);
					instruction.operands.emplace_back(operand);
				}

				return instruction;
			}
		}

		namespace writer
		{
			void write_operand(utils::bit_buffer_le& output_buffer, const operand_t& operand)
			{
				output_buffer.write_bits(2, operand.components.type);
				output_buffer.write_bits(2, operand.components.selection_mode);

				switch (operand.components.selection_mode)
				{
				case D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE:
					output_buffer.write_bits(4, operand.components.mask);
					output_buffer.write_bits(4, 0);
					break;
				case D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE:
					output_buffer.write_bits(2, operand.components.names[0]);
					output_buffer.write_bits(2, operand.components.names[1]);
					output_buffer.write_bits(2, operand.components.names[2]);
					output_buffer.write_bits(2, operand.components.names[3]);
					break;
				case D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE:
					output_buffer.write_bits(2, operand.components.names[0]);
					output_buffer.write_bits(6, 0);
					break;
				}

				output_buffer.write_bits(8, operand.type);
				output_buffer.write_bits(2, operand.dimension);

				output_buffer.write_bits(3, operand.indices[0].representation);
				output_buffer.write_bits(3, operand.indices[1].representation);
				output_buffer.write_bits(3, operand.indices[2].representation);

				output_buffer.write_bits(1, operand.extended);

				for (const auto& operand_extended : operand.extensions)
				{
					output_buffer.write_bits(6, operand_extended.type);
					output_buffer.write_bits(8, operand_extended.modifier);
					output_buffer.write_bits(17, 0);
					output_buffer.write_bits(1, operand_extended.extended);
				}

				const auto write_index = [&](const std::uint32_t index)
				{
					switch (operand.indices[index].representation)
					{
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
						output_buffer.write_bytes(4, operand.indices[index].values[0].u32);
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
						output_buffer.write_bytes(8, operand.indices[index].values[0].u64.value);
						break;
					case D3D10_SB_OPERAND_INDEX_RELATIVE:
						write_operand(output_buffer, *operand.extra_operand);
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
						write_operand(output_buffer, *operand.extra_operand);
						output_buffer.write_bytes(4, operand.indices[index].values[0].u32);
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:
						write_operand(output_buffer, *operand.extra_operand);
						output_buffer.write_bytes(8, operand.indices[index].values[0].u64.value);
						break;
					}
				};

				if (operand.dimension >= D3D10_SB_OPERAND_INDEX_1D)
				{
					write_index(0);
				}

				if (operand.dimension >= D3D10_SB_OPERAND_INDEX_2D)
				{
					write_index(1);
				}

				if (operand.dimension == D3D10_SB_OPERAND_INDEX_3D)
				{
					write_index(2);
				}

				if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
				{
					if (operand.components.type == D3D10_SB_OPERAND_4_COMPONENT)
					{
						output_buffer.write_bytes(4, operand.indices[0].values[0].u32);
						output_buffer.write_bytes(4, operand.indices[0].values[1].u32);
						output_buffer.write_bytes(4, operand.indices[0].values[2].u32);
						output_buffer.write_bytes(4, operand.indices[0].values[3].u32);
					}
					else
					{
						output_buffer.write_bytes(4, operand.indices[0].values[0].u32);
					}
				}

				if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE64)
				{
					output_buffer.write_bytes(8, operand.indices[0].values[0].u64.value);
				}
			}

			void write_opcode_extended(utils::bit_buffer_le& output_buffer, const opcode_extended_t& opcode)
			{
				output_buffer.write_bits(6, opcode.type);

				switch (opcode.type)
				{
				case D3D10_SB_EXTENDED_OPCODE_SAMPLE_CONTROLS:
					output_buffer.write_bits(3, 0);
					output_buffer.write_bits(4, opcode.values[0]);
					output_buffer.write_bits(4, opcode.values[0]);
					output_buffer.write_bits(4, opcode.values[0]);
					output_buffer.write_bits(10, 0);
					break;
				case D3D11_SB_EXTENDED_OPCODE_RESOURCE_DIM:
					output_buffer.write_bits(5, opcode.values[0]);
					output_buffer.write_bits(20, 0);
					break;
				case D3D11_SB_EXTENDED_OPCODE_RESOURCE_RETURN_TYPE:
					output_buffer.write_bits(4, opcode.values[0]);
					output_buffer.write_bits(4, opcode.values[1]);
					output_buffer.write_bits(4, opcode.values[2]);
					output_buffer.write_bits(4, opcode.values[3]);
					output_buffer.write_bits(9, 0);
					break;
				default:
					output_buffer.write_bits(25, 0);
					break;
				}

				output_buffer.write_bits(1, opcode.extended);
			}

			void write_opcode(utils::bit_buffer_le& output_buffer, const opcode_t& opcode)
			{
				output_buffer.write_bits(10, opcode.type);
				output_buffer.write_bits(14, opcode.controls);
				output_buffer.write_bits(6, opcode.length);
				output_buffer.write_bits(1, opcode.extended);
				output_buffer.write_bits(1, 0);

				for (const auto& opcode_extended : opcode.extensions)
				{
					write_opcode_extended(output_buffer, opcode_extended);
				}
			}

			std::uint32_t get_operand_length(const operand_t& operand)
			{
				std::uint32_t length = 1;

				length += static_cast<std::uint32_t>(operand.extensions.size());

				const auto write_index = [&](const std::uint32_t index)
				{
					switch (operand.indices[index].representation)
					{
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
						length += 1;
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
						length += 2;
						break;
					case D3D10_SB_OPERAND_INDEX_RELATIVE:
						length += get_operand_length(*operand.extra_operand);
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
						length += get_operand_length(*operand.extra_operand);
						length += 1;
						break;
					case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:
						length += get_operand_length(*operand.extra_operand);
						length += 2;
						break;
					}
				};

				if (operand.dimension >= D3D10_SB_OPERAND_INDEX_1D)
				{
					write_index(0);
				}

				if (operand.dimension >= D3D10_SB_OPERAND_INDEX_2D)
				{
					write_index(1);
				}

				if (operand.dimension == D3D10_SB_OPERAND_INDEX_3D)
				{
					write_index(2);
				}

				if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
				{
					if (operand.components.type == D3D10_SB_OPERAND_4_COMPONENT)
					{
						length += 4;
					}
					else
					{
						length += 1;
					}
				}

				if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE64)
				{
					length += 2;
				}

				return length;
			}
		}

		namespace disassembler
		{
			void print_operand(const operand_t& op, bool last)
			{
				if (op.extended)
				{
					for (const auto& extension : op.extensions)
					{
						switch (extension.modifier)
						{
						case D3D10_SB_OPERAND_MODIFIER_NEG:
							printf("-");
							break;
						case D3D10_SB_OPERAND_MODIFIER_ABS:
							printf("abs(");
							break;
						case D3D10_SB_OPERAND_MODIFIER_ABSNEG:
							printf("-abs(");
							break;
						}
					}
				}

				if (op.type == D3D10_SB_OPERAND_TYPE_INPUT)
				{
					printf("v%i", op.indices[0].values[0].u32);
				}

				if (op.type == D3D10_SB_OPERAND_TYPE_OUTPUT)
				{
					printf("o%i", op.indices[0].values[0].u32);
				}

				if (op.type == D3D10_SB_OPERAND_TYPE_TEMP)
				{
					printf("r%i", op.indices[0].values[0].u32);
				}

				if (op.type == D3D10_SB_OPERAND_TYPE_SAMPLER)
				{
					printf("s%i", op.indices[0].values[0].u32);
				}

				if (op.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER)
				{
					printf("cb%i[", op.indices[0].values[0].u32);
					if (op.extra_operand != nullptr)
					{
						print_operand(*op.extra_operand, true);
						printf(" ");
					}
					printf("%i]", op.indices[1].values[0].u32);
				}

				if (op.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE_CONSTANT_BUFFER)
				{
					printf("icb[");
					print_operand(*op.extra_operand, true);
					printf("]");
				}

				if (op.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
				{
					if (op.components.type == D3D10_SB_OPERAND_4_COMPONENT)
					{
						printf("l(%f, %f, %f, %f)",
							op.indices[0].values[0].f32,
							op.indices[0].values[1].f32,
							op.indices[0].values[2].f32,
							op.indices[0].values[3].f32
						);
					}
					else
					{
						printf("l(%f)", op.indices[0].values[0].f32);
					}

				}

				if (op.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE64)
				{
					printf("l(%lli)", op.indices[0].values[0].u64.value);
				}

				const auto print_component = [&](const std::uint32_t component)
				{
					switch (component)
					{
					case D3D10_SB_4_COMPONENT_X:
						printf("x");
						break;
					case D3D10_SB_4_COMPONENT_Y:
						printf("y");
						break;
					case D3D10_SB_4_COMPONENT_Z:
						printf("z");
						break;
					case D3D10_SB_4_COMPONENT_W:
						printf("w");
						break;
					}
				};

				switch (op.components.selection_mode)
				{
				case D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE:
				{
					const auto mask = (op.components.mask << D3D10_SB_OPERAND_4_COMPONENT_MASK_SHIFT);
					if ((mask & D3D10_SB_OPERAND_4_COMPONENT_MASK_MASK) != 0)
					{
						printf(".");
					}

					if (mask & D3D10_SB_OPERAND_4_COMPONENT_MASK_X)
					{
						printf("x");
					}

					if (mask & D3D10_SB_OPERAND_4_COMPONENT_MASK_Y)
					{
						printf("y");
					}

					if (mask & D3D10_SB_OPERAND_4_COMPONENT_MASK_Z)
					{
						printf("z");
					}

					if (mask & D3D10_SB_OPERAND_4_COMPONENT_MASK_W)
					{
						printf("w");
					}

					break;
				}
				case D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE:
				{
					printf(".");
					print_component(op.components.names[0]);
					print_component(op.components.names[1]);
					print_component(op.components.names[2]);
					print_component(op.components.names[3]);
					break;
				}
				case D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE:
				{
					printf(".");
					print_component(op.components.names[0]);
					break;
				}
				}

				if (op.extended)
				{
					for (const auto& extension : op.extensions)
					{
						switch (extension.modifier)
						{
						case D3D10_SB_OPERAND_MODIFIER_ABS:
						case D3D10_SB_OPERAND_MODIFIER_ABSNEG:
							printf(")");
							break;
						}
					}
				}

				if (!last)
				{
					printf(", ");
				}
			}

			void print_operands(const std::vector<operand_t>& operands)
			{
				for (auto i = 0; i < operands.size(); i++)
				{
					print_operand(operands[i], i == operands.size() - 1);
				}
			}

			void print_instruction(const instruction_t& instruction)
			{
				if (instruction.opcode.type >= opcode_names.size())
				{
					return;
				}

				const auto name = opcode_names[instruction.opcode.type];

				if (instruction.opcode.controls & 0x8)
				{
					printf("%s_sat ", name);
				}
				else
				{
					printf("%s ", name);
				}

				print_operands(instruction.operands);

				printf("\n");
			}
		}

		namespace tokens
		{
			opcode_t create_opcode(const std::uint32_t type, const std::uint32_t controls)
			{
				opcode_t opcode{};

				opcode.type = type;
				opcode.controls = controls;
				opcode.length = 1;

				return opcode;
			}

			operand_t create_literal_operand(const float x, const float y, const float z, const float w)
			{
				operand_t operand{};

				operand.type = D3D10_SB_OPERAND_TYPE_IMMEDIATE32;

				operand.dimension = D3D10_SB_OPERAND_INDEX_0D;
				operand.extended = 0;

				operand.components.type = D3D10_SB_OPERAND_4_COMPONENT;
				operand.components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE;
				operand.components.mask = 0;

				operand.indices[0].values[0].f32 = x;
				operand.indices[0].values[1].f32 = y;
				operand.indices[0].values[2].f32 = z;
				operand.indices[0].values[3].f32 = w;

				return operand;
			}

			operand_t create_register_operand(const std::uint32_t type, const std::string& component_names, const std::vector<std::uint32_t>& indices)
			{
				if (indices.size() > 3 || component_names.size() > 4)
				{
					throw std::runtime_error("create_register_operand: invalid args");
				}

				operand_t operand{};

				operand.type = type;

				operand.dimension = static_cast<std::uint32_t>(indices.size());
				operand.extended = 0;

				operand.components.type = D3D10_SB_OPERAND_4_COMPONENT;
				operand.components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE;
				operand.components.mask = 0;

				static std::unordered_map<char, std::uint32_t> component_name_map =
				{
					{'x', D3D10_SB_4_COMPONENT_X},
					{'y', D3D10_SB_4_COMPONENT_Y},
					{'z', D3D10_SB_4_COMPONENT_Z},
					{'w', D3D10_SB_4_COMPONENT_W},
					{'r', D3D10_SB_4_COMPONENT_R},
					{'g', D3D10_SB_4_COMPONENT_G},
					{'b', D3D10_SB_4_COMPONENT_B},
					{'a', D3D10_SB_4_COMPONENT_A},
				};

				for (auto i = 0u; i < component_names.size(); i++)
				{
					const auto iter = component_name_map.find(component_names[i]);
					if (iter == component_name_map.end())
					{
						continue;
					}

					operand.components.names[i] = iter->second;
				}

				for (auto i = 0u; i < indices.size(); i++)
				{
					operand.indices[i].values[0].u32 = indices[i];
				}

				return operand;
			}

			std::vector<std::uint32_t> find_operands(const instruction_t& instruction, const std::uint32_t beg,
				const std::function<bool(const operand_t&)>& cb)
			{
				std::vector<std::uint32_t> result;

				if (beg >= instruction.operands.size())
				{
					return result;
				}

				for (auto i = beg; i < instruction.operands.size(); i++)
				{
					if (cb(instruction.operands[i]))
					{
						result.emplace_back(i);
					}
				}

				return result;
			}
		}

		void process_instruction(utils::memory::allocator& allocator, utils::bit_buffer_le& input_buffer, utils::bit_buffer_le& output_buffer,
			const instruction_cb& callback)
		{
			const auto begin = input_buffer.total();

			const auto opcode = static_cast<std::uint32_t>(input_buffer.read_bits(11));
			if (opcode == D3D10_SB_OPCODE_CUSTOMDATA)
			{
				input_buffer.set_bit(begin);

				const auto desc = input_buffer.read_bytes(4);
				const auto count = input_buffer.read_bytes(4);

				output_buffer.write_bytes(4, desc);
				output_buffer.write_bytes(4, count);

				for (auto i = 0u; i < count - 2; i++)
				{
					output_buffer.write_bytes(4, input_buffer.read_bytes(4));
				}
			}
			else if (allowed_opcodes.contains(opcode))
			{
				input_buffer.set_bit(begin);

				const auto instruction = reader::read_instruction(allocator, input_buffer);
				const auto skip = callback(output_buffer, instruction);

				input_buffer.set_bit(begin);
				for (auto i = 0u; i < instruction.opcode.length; i++)
				{
					const auto value = input_buffer.read_bytes(4);
					if (!skip)
					{
						output_buffer.write_bytes(4, value);
					}
				}
			}
			else
			{
				input_buffer.read_bits(13);
				const auto length = input_buffer.read_bits(7);
				input_buffer.read_bits(1);

				input_buffer.set_bit(begin);
				for (auto i = 0u; i < length; i++)
				{
					output_buffer.write_bytes(4, input_buffer.read_bytes(4));
				}
			}
		}
	}

	std::string patch_shader(unsigned char* program, unsigned int program_size, const asm_::instruction_cb& callback)
	{
		std::string output_program;

		if (program_size == 0)
		{
			return output_program;
		}

		unsigned int chunk_size{};
		auto chunk = parse_shader_chunk(program, program_size, &chunk_size);
		const auto offset = chunk - program;

		output_program.append(reinterpret_cast<char*>(program), offset);

		utils::bit_buffer_le input_buffer({reinterpret_cast<char*>(chunk), chunk_size});
		utils::bit_buffer_le output_buffer;

		const auto type = input_buffer.read_bytes(4); // "SHEX"
		const auto len = input_buffer.read_bytes(4);
		const auto minor_version = input_buffer.read_bits(4);
		const auto major_version = input_buffer.read_bits(4);
		const auto program_type = input_buffer.read_bytes(1);
		const auto unk = input_buffer.read_bytes(2);
		const auto num_dwords = input_buffer.read_bytes(4);

		output_buffer.write_bytes(4, type);
		output_buffer.write_bytes(4, len);
		output_buffer.write_bits(4, minor_version);
		output_buffer.write_bits(4, major_version);
		output_buffer.write_bytes(1, program_type);
		output_buffer.write_bytes(2, unk);
		output_buffer.write_bytes(4, num_dwords);

		utils::memory::allocator allocator;

		while (input_buffer.total() < chunk_size * 8)
		{
			asm_::process_instruction(allocator, input_buffer, output_buffer, callback);
		}

		const auto total_len = output_buffer.total() / 8u;
		const auto new_len = total_len - 8u;
		const auto new_num_dwords = (new_len / 4u);

		output_buffer.set_byte(4);
		output_buffer.write_bytes(4, new_len);
		output_buffer.set_byte(12);
		output_buffer.write_bytes(4, new_num_dwords);

		const auto buffer = output_buffer.get_buffer().data();
		output_program.append(buffer, total_len);

		const auto header = reinterpret_cast<dx11_shader_header*>(output_program.data());

		header->program_size = static_cast<std::uint32_t>(output_program.size());

		dxbc::CalculateDXBCChecksum(reinterpret_cast<unsigned char*>(output_program.data()),
			static_cast<std::uint32_t>(output_program.size()), reinterpret_cast<DWORD*>(&header->checksum));

		return output_program;
	}
}
