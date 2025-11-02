#pragma once

#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>
#include <utils/bit_buffer.hpp>
#include "../dxbc_checksum.hpp"

#include "components/definitions.hpp"
#include "components/reader.hpp"
#include "components/writer.hpp"
#include "components/disassembler.hpp"
#include "components/tokens.hpp"

namespace shader
{
	struct dx11_shader_header
	{
		unsigned char dxbc[4]; // "DXBC"
		unsigned int checksum[4];
		unsigned int unk_int; // always 1, sanity check?
		unsigned int program_size;
		unsigned int chunk_count;
		// unsigned int chunk_offsets[chunk_count];
		// unsigned char chunks[chunk_count][?];
	};

	struct shader_checksum
	{
		unsigned int checksum[4];
	};

	shader_checksum generate_checksum(unsigned char* program, unsigned int program_size);
	unsigned int calc_crc32(unsigned char* program, unsigned int program_size);

	namespace asm_
	{
		instruction_t read_instruction(utils::bit_buffer_le& input_buffer);
		void write_instruction(utils::bit_buffer_le& output_buffer, const instruction_t& instruction);
		void print_instruction(const instruction_t& instruction);
	}

	using instruction_cb = std::function<bool(utils::bit_buffer_le&, asm_::instruction_t)>;

	std::string patch_shader(unsigned char* program, unsigned int program_size, const instruction_cb& callback);
}
