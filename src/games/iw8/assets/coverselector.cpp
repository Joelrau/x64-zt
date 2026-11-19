#include <std_include.hpp>
#include "coverselector.hpp"

namespace zonetool::iw8
{
	void cover_selector::read(zone_reader& reader, CoverSelector* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_script_string(asset->scrName);
		reader.pop_stream();
	}
}
