#include <std_include.hpp>
#include "rawfile.hpp"

#include <utils/compression.hpp>

namespace zonetool::iw5
{
	void rawfile::read(zone_reader32& reader, RawFile* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->buffer, 0, asset->compressedLen ? asset->compressedLen : asset->len + 1);
		reader.pop_stream();
	}

	std::string rawfile::text(RawFile* asset)
	{
		if (!asset->compressedLen)
		{
			return std::string(asset->buffer.get(), asset->len);
		}

		return utils::compression::zlib::decompress(std::string(asset->buffer.get(), asset->compressedLen));
	}

	void rawfile::dump(RawFile* asset)
	{
		filesystem::file file(asset->name.get());
		file.open("wb");

		if (file.get_fp() && asset->buffer)
		{
			file.write(text(asset));
		}

		file.close();
	}
}
