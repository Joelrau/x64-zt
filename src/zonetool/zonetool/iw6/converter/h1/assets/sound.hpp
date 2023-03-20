#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace sound
		{
			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, ZoneMemory* mem);
			void dump(snd_alias_list_t* asset, ZoneMemory* mem);
		}
	}
}