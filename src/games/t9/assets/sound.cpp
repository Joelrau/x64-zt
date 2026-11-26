#include <std_include.hpp>
#include "sound.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_value_array(auto& reader, SndValueArray& array)
		{
			reader.read_array(array.values, 3, array.count);
		}

		void read_aliases(auto& reader, SndAliasList& list)
		{
			const auto aliases = reader.read_array(list.aliases, 7, list.count);
			if (!aliases)
			{
				return;
			}

			for (auto i = 0; i < list.count; i++)
			{
				auto& alias = aliases[i];
				for (auto& asset : alias.assets)
				{
					reader.read_asset(ASSET_TYPE_SOUND_ASSET, asset);
				}

				if (const auto info = reader.read_array(alias.info, 7))
				{
					reader.read_asset(ASSET_TYPE_SOUND_ALIAS_MODIFIER, info->modifier);
				}

				reader.read_array(alias.extra, 0, 8);
			}
		}

		void read_groups(auto& reader, SndBank* asset)
		{
			const auto groups = reader.read_array(asset->groups, 7, asset->groupCount);
			if (!groups)
			{
				return;
			}

			for (auto i = 0u; i < asset->groupCount; i++)
			{
				const auto items = reader.read_array(groups[i].items, 7, groups[i].itemCount);
				if (!items)
				{
					continue;
				}

				for (auto j = 0u; j < groups[i].itemCount; j++)
				{
					reader.read_array(items[j].values, 7, items[j].valueCount);
				}
			}
		}
	}

	void sound_bank::read(auto& reader, SndBank* asset)
	{
		if (const auto lists = reader.read_array(asset->aliases, 7, asset->aliasCount))
		{
			for (auto i = 0u; i < asset->aliasCount; i++)
			{
				read_aliases(reader, lists[i]);
			}
		}

		reader.read_array(asset->aliasIndex, 1, asset->aliasCount);
		reader.read_asset_array(ASSET_TYPE_SOUND_DUCK, asset->ducks, asset->duckCount);
		reader.read_array(asset->entries, 7, asset->entryCount);
		read_groups(reader, asset);
		reader.read_asset(ASSET_TYPE_SOUND_ACOUSTICS, asset->acoustics);
	}

	void sound_asset::read(auto& reader, SndAsset* asset)
	{
		reader.read_array(asset->extra, 0, asset->extraSize);
		reader.read_array(asset->data, 3, asset->dataSize);
		reader.read_asset(ASSET_TYPE_STREAMKEY, asset->streamKey);
	}

	void sound_duck::read(auto&, SndDuck*)
	{
	}

	void sound_alias_modifier::read(auto& reader, SndAliasModifier* asset)
	{
		for (auto& curve : asset->curves)
		{
			read_value_array(reader, curve);
		}
	}

	void sound_acoustics::read(auto& reader, SndAcoustics* asset)
	{
		reader.read_string(asset->nameString);
		reader.read_asset(ASSET_TYPE_STREAMKEY, asset->streamKey);

		auto& data = asset->acousticsData;
		reader.read_array(data.data, 15, data.size);
		if (const auto probes = reader.read_array(data.probes, 15, data.probeCount))
		{
			for (auto i = 0; i < data.probeCount; i++)
			{
				reader.read_asset(ASSET_TYPE_STREAMKEY, probes[i].streamKey);
			}
		}
		reader.read_asset(ASSET_TYPE_STREAMKEY, data.streamKey);

		for (auto& values : asset->values0)
		{
			read_value_array(reader, values);
		}
		read_value_array(reader, asset->values1);
		for (auto& values : asset->values2)
		{
			read_value_array(reader, values);
		}
		read_value_array(reader, asset->values3);
		read_value_array(reader, asset->values4);
		reader.read_array(asset->records, 3, asset->recordCount);
	}

	void snd_driver_globals::read(auto& reader, SndDriverGlobals* asset)
	{
		reader.read_array(asset->records0, 7, asset->records0Count);
		if (const auto lists = reader.read_array(asset->lists, 7, asset->listCount))
		{
			for (auto i = 0u; i < asset->listCount; i++)
			{
				reader.read_array(lists[i].values, 3, lists[i].valueCount);
			}
		}

		reader.read_array(asset->records1, 7, asset->records1Count);
		reader.read_array(asset->records2, 7, asset->records2Count);
		reader.read_array(asset->records3, 7, asset->records3Count);
		reader.read_array(asset->records4, 7, asset->records4Count);
		reader.read_array(asset->records5, 7, asset->records5Count);
		reader.read_array(asset->records6, 7, asset->records6Count);
		reader.read_array(asset->records7, 7, asset->records7Count);
		reader.read_array(asset->records8, 7, asset->records8Count);
		reader.read_array(asset->records9, 7, asset->records9Count);
		reader.read_asset_array(ASSET_TYPE_SOUND_ALIAS_MODIFIER, asset->modifiers, asset->modifierCount);
	}
}

namespace zonetool::t9
{
	template void sound_bank::read(zone_reader&, SndBank*);
	template void sound_asset::read(zone_reader&, SndAsset*);
	template void sound_duck::read(zone_reader&, SndDuck*);
	template void sound_alias_modifier::read(zone_reader&, SndAliasModifier*);
	template void sound_acoustics::read(zone_reader&, SndAcoustics*);
	template void snd_driver_globals::read(zone_reader&, SndDriverGlobals*);
	template void sound_bank::read(zone_writer&, SndBank*);
	template void sound_asset::read(zone_writer&, SndAsset*);
	template void sound_duck::read(zone_writer&, SndDuck*);
	template void sound_alias_modifier::read(zone_writer&, SndAliasModifier*);
	template void sound_acoustics::read(zone_writer&, SndAcoustics*);
	template void snd_driver_globals::read(zone_writer&, SndDriverGlobals*);
}
