#include <std_include.hpp>
#include "xboneset.hpp"

namespace zonetool::s4
{
	void xbone_set::read(zone_reader& reader, XBoneSet* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.read_script_string_array(asset->bones, asset->boneCount);
		reader.pop_stream();
	}
}
