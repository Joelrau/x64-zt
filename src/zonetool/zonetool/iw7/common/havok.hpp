#pragma once

namespace zonetool::iw7
{
	namespace havok
	{
		constexpr auto havok_file_ext = ".hkx";

		static char* parse_havok_data(std::string path, unsigned int* size, zone_memory* mem)
		{
			if (!path.ends_with(havok_file_ext))
			{
				path.append(havok_file_ext);
			}

			auto file = filesystem::file(path);
			if (!file.exists())
			{
				*size = 0;
				return nullptr;
			}
			file.open("rb");
			*size = static_cast<unsigned int>(file.size());
			auto bytes = file.read_bytes(*size);
			auto* data = mem->allocate<char>(*size);
			memcpy(data, bytes.data(), *size);
			file.close();
			return data;
		}

		static void dump_havok_data(std::string path, char* data, unsigned int size)
		{
			if (!path.ends_with(havok_file_ext))
			{
				path.append(havok_file_ext);
			}

			auto file = filesystem::file(path);
			file.open("wb");
			file.write(data, size, 1);
			file.close();
		}
	}
}