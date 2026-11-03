#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace sound
		{
			std::optional<zonetool::h1::SoundDspBus> get_dsp_bus(const char* channel);
			std::size_t get_dsp_bus_channel_count(zonetool::h1::SoundDspBus bus);
			std::optional<zonetool::h1::SoundVolMod> get_vol_mod(const char* vol_mod);

			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, utils::memory::allocator& allocator);
			void dump(snd_alias_list_t* asset);
		}
	}
}