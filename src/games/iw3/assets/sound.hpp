#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class sound
	{
	public:
		static void read(zone_reader32& reader, snd_alias_list_t* asset);
	};
}
