#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class script_debug_data
	{
	public:
		static void read(zone_reader& reader, ScriptDebugData* asset);
	};
}
