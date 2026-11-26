#include <std_include.hpp>
#include "locdmgtable.hpp"

namespace zonetool::s4
{
	void loc_dmg_table::read(zone_reader& reader, LocDmgTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
