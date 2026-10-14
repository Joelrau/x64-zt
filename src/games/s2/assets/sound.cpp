#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_sound_file(zone_reader& reader, SoundFile* file)
		{
			switch (file->type)
			{
			case SAT_LOADED:
				reader.read_asset(ASSET_TYPE_LOADED_SOUND, file->u.loadSnd);
				break;
			case SAT_STREAMED:
				loaded_sound::read_stream_info(reader, file->u.streamSnd.info);
				break;
			case SAT_PRIMED:
				reader.read_asset(ASSET_TYPE_LOADED_SOUND, file->u.primedSnd.loadedPart);
				loaded_sound::read_stream_info(reader, file->u.primedSnd.streamedPart);
				break;
			default:
				break;
			}
		}

		void read_alias(zone_reader& reader, snd_alias_t* alias)
		{
			reader.read_string(alias->aliasName);
			reader.read_string(alias->subtitle);
			reader.read_string(alias->secondaryAliasName);
			reader.read_string(alias->chainAliasName);
			reader.read_string(alias->mixerGroup);
			reader.read_string(alias->squelchName);

			if (const auto file = reader.read_single(alias->soundFile, 7))
			{
				read_sound_file(reader, file);
			}

			reader.read_asset(ASSET_TYPE_SOUND_CONTEXT, alias->sndContext);
			reader.read_asset(ASSET_TYPE_SOUND_CURVE, alias->volumeFalloffCurve);
			reader.read_asset(ASSET_TYPE_DIST_CURVE, alias->distCurve);
			reader.read_asset(ASSET_TYPE_REVERB_CURVE, alias->reverbSendCurve);

			if (const auto duck = reader.read_single(alias->duck, 7))
			{
				reader.read_string(duck->name);
			}

			for (auto& modifier : alias->parameterModifiers)
			{
				reader.read_asset(ASSET_TYPE_ALIAS_PARAMETER_MODIFIER, modifier);
			}

			reader.read_asset(ASSET_TYPE_ALIAS_COMBAT_CONE, alias->combatCone);
		}
	}

	void sound::read(zone_reader& reader, snd_alias_list_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto aliases = reader.read_array(asset->head, 7, asset->count))
		{
			for (auto i = 0; i < asset->count; i++)
			{
				read_alias(reader, &aliases[i]);
			}
		}

		reader.read_array(asset->sortedIndices, 0, asset->sortedIndexCount);
		reader.pop_stream();
	}
}
