#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class loaded_sound
	{
	public:
		static void read(zone_reader32& reader, LoadedSound* asset);
	};
}
