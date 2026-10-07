#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace sound
		{
			zonetool::h1::snd_alias_list_t* convert(zonetool::h2::snd_alias_list_t* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::snd_alias_list_t* asset);
		}
	}
}