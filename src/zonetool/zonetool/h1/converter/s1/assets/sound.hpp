#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace sound
		{
			zonetool::s1::snd_alias_list_t* convert(snd_alias_list_t* asset, utils::memory::allocator& allocator);
			void dump(snd_alias_list_t* asset);
		}
	}
}
