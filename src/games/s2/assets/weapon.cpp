#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		constexpr std::size_t xanim_count = 188;
	}

	void weapon_def::read(zone_reader& reader, WeaponCompleteDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);

		const field_reader w(reader, asset);
		w.strings({0, 8});
		w.asset_array(ASSET_TYPE_XMODEL, 16, 2);
		w.asset_array(ASSET_TYPE_XMODEL, 24, 2);
		w.asset(ASSET_TYPE_XMODEL, 32);
		w.string(40);
		w.asset_array(ASSET_TYPE_XANIM, 48, xanim_count);
		w.asset_array(ASSET_TYPE_XANIM, 56, xanim_count);
		w.script_strings(64, 32);
		w.asset_array(ASSET_TYPE_ATTACHMENT, 72, w.at<std::uint8_t>(1232));
		w.asset_array(ASSET_TYPE_XANIM, 88, xanim_count);
		w.entries(96, 32, w.at<std::uint8_t>(1233), [](const field_reader& entry, std::size_t)
		{
			entry.assets(ASSET_TYPE_XANIM, 8, 2);
		});
		w.entries(104, 24, w.at<std::uint8_t>(1234), [](const field_reader& entry, std::size_t)
		{
			entry.sounds(8, 2);
		});
		w.entries(112, 24, w.at<std::uint8_t>(1235), [](const field_reader& entry, std::size_t)
		{
			entry.assets(ASSET_TYPE_FX, 8, 2);
		});
		w.bytes(120, 3, 16 * w.at<std::uint8_t>(1236));
		w.entries(128, 24, w.at<std::uint8_t>(1237), [](const field_reader& entry, std::size_t)
		{
			entry.script_strings(8, 36);
			entry.script_strings(16, 36);
		});
		w.script_strings(136, 36);
		w.script_strings(144, 36);
		w.script_strings(152, 16);
		w.script_strings(160, 16);
		w.script_strings(168, 16);
		w.asset_array(ASSET_TYPE_FX, 176, 16);
		w.script_strings(184, 16);
		w.string(192);
		w.assets(ASSET_TYPE_FX, 200, 13);
		w.sounds(304, 65);
		w.sound_array(824, 62);
		w.sound_array(832, 62);
		w.assets(ASSET_TYPE_FX, 840, 5);
		w.assets(ASSET_TYPE_MATERIAL, 880, 3);
		w.asset_array(ASSET_TYPE_XMODEL, 904, 2);
		w.asset_array(ASSET_TYPE_XMODEL, 912, 2);
		w.assets(ASSET_TYPE_XMODEL, 920, 2);
		w.assets(ASSET_TYPE_MATERIAL, 936, 4);
		w.strings({968, 976, 984});
		w.asset(ASSET_TYPE_PHYSCOLLMAP, 992);
		w.asset(ASSET_TYPE_PHYSPRESET, 1000);
		w.strings({1008, 1016});
		w.bytes(1024, 3, 0x58);
		w.strings({1032, 1040, 1048});
		w.assets(ASSET_TYPE_TRACER, 1056, 3);
		w.asset(ASSET_TYPE_LASER, 1080);
		w.assets(ASSET_TYPE_BEAM, 1088, 2);
		w.sound(1104);
		w.asset(ASSET_TYPE_FX, 1112);
		w.string(1120);
		w.sound(1128);
		w.sounds(1136, 4);
		w.sounds(1168, 4);
		w.sounds(1200, 2);
		w.asset(ASSET_TYPE_XMODEL, 1216);
		w.entries(1224, 0x48, 1, [](const field_reader& entry, std::size_t)
		{
			entry.sounds(16, 4);
			entry.assets(ASSET_TYPE_SOUND, 48, 2);
		});
		w.assets(ASSET_TYPE_MATERIAL, 2336, 4);
		w.assets(ASSET_TYPE_MATERIAL, 2680, 3);
		w.string(2848);
		w.asset(ASSET_TYPE_XMODEL, 2856);
		w.assets(ASSET_TYPE_FX, 2864, 3);
		w.sounds(2888, 3);
		w.bytes(2944, 3, 0xF8);
		w.bytes(2952, 3, 0xF8);
		w.assets(ASSET_TYPE_FX, 2960, 2);
		w.asset(ASSET_TYPE_FX, 3032);
		w.sound(3040);
		w.string(3400);
		w.bytes(3416, 3, 8 * w.at<std::uint16_t>(3448));
		w.bytes(3432, 3, 8 * w.at<std::uint16_t>(3448));
		w.string(3408);
		w.bytes(3424, 3, 8 * w.at<std::uint16_t>(3450));
		w.bytes(3440, 3, 8 * w.at<std::uint16_t>(3450));
		w.string(3560);
		w.script_string(3820);
		w.script_string(3944);
		w.script_string(3948);

		reader.pop_stream();
	}
}
