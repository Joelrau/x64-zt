#pragma once

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
		unsigned int cheksum[4];
	};

	std::vector<size_t> get_dest_reference_offsets(unsigned char* program, unsigned int program_size);
	shader_checksum generate_checksum(unsigned char* program, unsigned int program_size);

	unsigned int calc_crc32(unsigned char* program, unsigned int program_size);
}
