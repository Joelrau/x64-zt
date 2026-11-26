#include <std_include.hpp>
#include "localize.hpp"

namespace zonetool::t9
{
	void localize::read(auto& reader, LocalizeEntry* asset)
	{
		reader.read_string(asset->value);
	}
}

namespace zonetool::t9
{
	template void localize::read(zone_reader&, LocalizeEntry*);
	template void localize::read(zone_writer&, LocalizeEntry*);
}
