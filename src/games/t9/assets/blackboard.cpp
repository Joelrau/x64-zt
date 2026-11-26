#include <std_include.hpp>
#include "blackboard.hpp"

namespace zonetool::t9
{
	namespace
	{
		std::size_t get_value_size(const std::int32_t type)
		{
			switch (type)
			{
			case 0:
			case 1:
			case 3:
				return 4;
			case 2:
				return 1;
			default:
				return 0;
			}
		}
	}

	void blackboard::read(auto& reader, Blackboard* asset)
	{
		const auto entries = reader.read_array(asset->entries, 7, asset->entryCount);
		for (auto i = 0; entries && i < asset->entryCount; i++)
		{
			reader.read_array(entries[i].value, 3, get_value_size(entries[i].type));
			reader.read_array(entries[i].values, 7, entries[i].valueCount);
		}
	}
}

namespace zonetool::t9
{
	template void blackboard::read(zone_reader&, Blackboard*);
	template void blackboard::read(zone_writer&, Blackboard*);
}
