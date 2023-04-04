#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace sound
		{
			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, zone_memory* mem);
			void dump(snd_alias_list_t* asset, zone_memory* mem);
		}
	}
}