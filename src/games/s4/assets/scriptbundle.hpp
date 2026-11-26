#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class script_bundle
	{
	public:
		static void read(zone_reader& reader, ScriptBundle* asset);
	};
}
