#include <std_include.hpp>
#include "mapents.hpp"

namespace zonetool::iw3
{
	void map_ents::read(zone_reader32& reader, MapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		reader.pop_stream();
	}

	void map_ents::dump(MapEnts* asset)
	{
		filesystem::file file(asset->name.get() + ".ents"s);
		file.open("wb");

		if (file.get_fp() && asset->numEntityChars > 0)
		{
			file.write(asset->entityString.get(), asset->numEntityChars, 1);
		}

		file.close();
	}
}
