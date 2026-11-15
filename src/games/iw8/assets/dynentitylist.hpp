#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class dyn_entity_list
	{
	public:
		static void read(zone_reader& reader, DynEntityList* asset);
	};
}
