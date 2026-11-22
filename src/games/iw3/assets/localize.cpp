#include <std_include.hpp>
#include "localize.hpp"

namespace zonetool::iw3
{
	void localize::read(zone_reader32& reader, LocalizeEntry* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->value);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void localize::dump(LocalizeEntry* asset)
	{
		filesystem::file file("localizedstrings\\"s + asset->name.get());
		file.open("wb");

		if (file.get_fp())
		{
			file.write(asset->value.get(), std::strlen(asset->value.get()), 1);
		}

		file.close();
	}
}
