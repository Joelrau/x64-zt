#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace sound
		{
			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, utils::memory::allocator& allocator);
			void dump(snd_alias_list_t* asset);
		}
	}
}