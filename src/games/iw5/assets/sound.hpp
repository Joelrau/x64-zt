#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class sound
	{
	public:
		static void read(zone_reader32& reader, snd_alias_list_t* asset);
		static void read_custom(zone_reader32& reader, SndAliasCustom& sound);
	};
}
