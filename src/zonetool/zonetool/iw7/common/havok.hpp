#pragma once

namespace zonetool::iw7
{
	namespace havok
	{
		namespace xml
		{

		}

		namespace binary
		{
			constexpr auto havok_file_ext = ".hkx";

			char* parse_havok_data(std::string path, unsigned int* size, zone_memory* mem);
			void dump_havok_data(std::string path, char* data, unsigned int size);
		}
	}
}