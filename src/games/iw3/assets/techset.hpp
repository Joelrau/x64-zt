#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class tech_set
	{
	public:
		static void read(zone_reader32& reader, MaterialTechniqueSet* asset);
	};
}
