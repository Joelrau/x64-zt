#include <std_include.hpp>
#include "player_anim_script.hpp"

namespace zonetool::iw7
{
	namespace
	{
		constexpr auto script_anim_count = 121;
		constexpr auto script_event_count = 55;

		void read_script_entries(zone_reader& reader, PlayerAnimScriptEntry*& field, const std::size_t count)
		{
			if (const auto entries = reader.read_array(field, 7, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					reader.read_array(entries[i].items, 3, entries[i].itemCount);
					reader.read_array(entries[i].transitions, 3, entries[i].transitionCount);
				}
			}
		}

		void read_script_items(zone_reader& reader, PlayerAnimScript* asset)
		{
			if (const auto items = reader.read_array(asset->scriptItems, 7, asset->scriptItemCount))
			{
				for (auto i = 0u; i < asset->scriptItemCount; i++)
				{
					reader.read_array(items[i].conditions, 3, items[i].conditionCount);
					reader.read_array(items[i].commands, 3, items[i].commandCount);
				}
			}
		}
	}

	void player_anim_script::read(zone_reader& reader, PlayerAnimScript* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->scriptName);
		if (const auto animations = reader.read_array(asset->animations, 3, asset->animationCount))
		{
			for (auto i = 0u; i < asset->animationCount; i++)
			{
				reader.read_script_string(animations[i].entryName);
				reader.read_script_string(animations[i].animName);
				reader.read_script_string(animations[i].shadowAnimName);
			}
		}
		read_script_entries(reader, asset->scriptAnims, script_anim_count);
		read_script_entries(reader, asset->scriptEvents, script_event_count);
		read_script_items(reader, asset);
		reader.read_array(asset->scriptTransitions, 3, asset->scriptTransitionCount);
		reader.read_array(asset->scriptIdleTurns, 3, asset->scriptIdleTurnCount);
		if (const auto twitches = reader.read_array(asset->scriptIdleTwitches, 7, asset->scriptIdleTwitchCount))
		{
			for (auto i = 0u; i < asset->scriptIdleTwitchCount; i++)
			{
				reader.read_array(twitches[i].twitches, 1, twitches[i].twitchCount);
			}
		}
		reader.read_array(asset->scriptAimSets, 3, asset->scriptAimSetCount);
		reader.read_array(asset->scriptLeanSets, 3, asset->scriptLeanSetCount);
		reader.read_array(asset->unk, 3, asset->unkCount);
		reader.read_array(asset->torsoAnimPackMap, 1, asset->animationCount);
		reader.read_array(asset->torsoAnimUnpackMap, 1, asset->torsoAnimCount);
		reader.read_array(asset->legsAnimPackMap, 1, asset->animationCount);
		reader.read_array(asset->legsAnimUnpackMap, 1, asset->legsAnimCount);
		reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->xAnims, asset->xAnimCount);
		reader.pop_stream();
	}
}
