#include <std_include.hpp>
#include "localize.hpp"

namespace zonetool::iw8
{
	void localize::read(zone_reader& reader, LocalizeEntry* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->value);
		reader.pop_stream();
	}
}
