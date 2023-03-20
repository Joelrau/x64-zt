#include <std_include.hpp>

#include "shader.hpp"

#include <zlib.h>

namespace shader
{
	namespace
	{
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

	unsigned int calc_crc32(unsigned char* program, unsigned int program_size)
	{
		if (program == nullptr)
		{
			return 0;
		}

		return get_crc32(program, program_size);
	}
}