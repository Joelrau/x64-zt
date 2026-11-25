#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class loaded_sound
	{
	public:
		static void read(zone_reader32& reader, LoadedSound* asset);
	};
}
