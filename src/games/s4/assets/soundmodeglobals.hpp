#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class sound_mode_globals
	{
	public:
		static void read(zone_reader& reader, SndModeGlobals* asset);
	};
}
