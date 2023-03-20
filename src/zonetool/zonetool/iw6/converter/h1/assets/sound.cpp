#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
#include "zonetool/iw6/converter/h1/include.hpp"
#include "sound.hpp"

#include "zonetool/h1/assets/sound.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace sound
		{
			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::snd_alias_list_t>();

				REINTERPRET_CAST_SAFE(name);



				return new_asset;
			}

			void dump(snd_alias_list_t* asset, ZoneMemory* mem)
			{
				//auto* converted_asset = convert(asset, mem);
				//zonetool::h1::ISound::dump(converted_asset);
			}
		}
	}
}