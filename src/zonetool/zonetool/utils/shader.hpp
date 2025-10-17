#pragma once

namespace shader
{
	namespace fxcd
	{
		bool DxbcToAsmWithFxcd(const void* dxbc, size_t size,
			std::wstring fxcdPath,
			std::wstring disassemblerMode,
			std::wstring extraFlags,
			std::string& asmOut,
			std::string* logOut = nullptr);

		bool AsmToDxbcWithFxcd(const std::string& asmText,
			std::wstring fxcdPath,
			std::wstring extraAssembleFlags,
			const void* originalDxbc,
			std::size_t originalDxbcSize,
			std::vector<uint8_t>& outDxbc,
			std::string* logOut = nullptr);
	}

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
