/*

	Havok data is entirely different in IW8 compared to IW7. A few assets use Havok's technology as a pre-calculated blob,
	meaning that without access to the source tools or SDK, we cannot produce a 1:1 accurate blob.

	The next solution is to convert legacy COD physics/collision data on the fly using Havok's very nice SDK that IW8 will let us use.
	This is not ideal either, as the conversion process will probably never be perfect and may produce different results than the original data, 
	but it is better than nothing.

	This entire component is pretty much useless right now and will be worked on later. It simply just dumps the pre-calculated blobs
	from IW8 assets so it can be seen in the raw makeup.

*/

#include "std_include.hpp"
#include "zonetool/utils/utils.hpp"
#include "havok.hpp"

namespace zonetool::iw8
{
	namespace havok
	{
		namespace binary
		{
			char* parse_havok_data(std::string path, unsigned int* size, zone_memory* mem)
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
				auto bytes = file.read_bytes(file.size());
				file.close();

				/*
				if (!validate_header(reinterpret_cast<hkxHeaderData*>(bytes.data())))
				{
					*size = 0;
					return nullptr;
				}
				*/

				auto* data = mem->allocate<char>(*size);
				memcpy(data, bytes.data(), *size);

				return data;
			}

			void dump_havok_data(std::string path, char* data, unsigned int size)
			{
				if (!path.ends_with(havok_file_ext))
				{
					path.append(binary::havok_file_ext);
				}

				if (!data || !size)
				{
					return;
				}

				// TODO: IW8 is completely different, but it is still havok
				/*
				if (!validate_header(reinterpret_cast<binary::hkxHeaderData*>(data)))
				{
					__debugbreak();
				}
				*/

				auto file = filesystem::file(path);
				file.open("wb");
				file.write(data, size, 1);
				file.close();
			}
		}
	}
}
