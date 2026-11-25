#include <std_include.hpp>
#include "sound.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_sound_file(zone_reader32& reader, SoundFile* file)
		{
			if (file->type == SAT_LOADED)
			{
				reader.read_asset(ASSET_TYPE_LOADED_SOUND, file->u.loadSnd);
			}
			else
			{
				reader.read_string(file->u.streamSnd.dir);
				reader.read_string(file->u.streamSnd.name);
			}
		}

		void read_alias(zone_reader32& reader, snd_alias_t* alias)
		{
			reader.read_string(alias->aliasName);
			reader.read_string(alias->subtitle);
			reader.read_string(alias->secondaryAliasName);
			reader.read_string(alias->chainAliasName);
			reader.read_string(alias->mixerGroup);

			if (const auto file = reader.read_single(alias->soundFile, 3))
			{
				read_sound_file(reader, file);
			}

			reader.read_asset(ASSET_TYPE_SOUND_CURVE, alias->volumeFalloffCurve);

			if (const auto speaker_map = reader.read_single(alias->speakerMap, 3))
			{
				reader.read_string(speaker_map->name);
			}
		}
	}

	void sound::read(zone_reader32& reader, snd_alias_list_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->aliasName);

		if (const auto aliases = reader.read_array(asset->head, 3, asset->count))
		{
			for (auto i = 0; i < asset->count; i++)
			{
				read_alias(reader, &aliases[i]);
			}
		}

		reader.pop_stream();
	}

	void sound::read_custom(zone_reader32& reader, SndAliasCustom& sound)
	{
		if (const auto name = reader.read_single(sound.name, 3))
		{
			reader.read_string(name->soundName);
		}
	}
}
