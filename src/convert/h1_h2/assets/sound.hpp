#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace sound
		{
			zonetool::h2::snd_alias_list_t* convert(zonetool::h1::snd_alias_list_t* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::snd_alias_list_t* asset);
		}
	}
}