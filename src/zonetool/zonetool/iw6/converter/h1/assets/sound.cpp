#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "sound.hpp"

#include "zonetool/h1/assets/sound.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace sound
		{
			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::snd_alias_list_t>();

				REINTERPRET_CAST_SAFE(name);



				return new_asset;
			}

			void dump(snd_alias_list_t* asset)
			{
				//auto* converted_asset = convert(asset, allocator);
				//zonetool::h1::sound::dump(converted_asset);
			}
		}
	}
}