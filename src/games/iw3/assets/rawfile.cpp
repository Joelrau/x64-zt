#include <std_include.hpp>
#include "rawfile.hpp"

namespace zonetool::iw3
{
	void rawfile::read(zone_reader32& reader, RawFile* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->buffer, 0, asset->len + 1);
		reader.pop_stream();
	}

	void rawfile::dump(RawFile* asset)
	{
		filesystem::file file(asset->name.get());
		file.open("wb");

		if (file.get_fp() && asset->len > 0)
		{
			file.write(asset->buffer.get(), asset->len, 1);
		}

		file.close();
	}
}
