#include <std_include.hpp>
#include "tables.hpp"
#include "scriptbundle.hpp"

namespace zonetool::t9
{
	void footstep_table::read(auto& reader, FootstepTable* asset)
	{
		for (auto& entry : asset->entries)
		{
			reader.read_asset_array(ASSET_TYPE_SURFACESOUNDDEF, entry.surfaceSoundDefs, entry.surfaceSoundDefCount);
			reader.read_array(entry.hashes, 7, entry.hashCount);
			reader.read_asset(ASSET_TYPE_SOUND_ALIAS_MODIFIER, entry.modifier);
		}
	}

	void string_table::read(auto& reader, StringTable* asset)
	{
		const auto cell_count = asset->columnCount * asset->rowCount;
		if (const auto values = reader.read_array(asset->values, 7, cell_count))
		{
			for (auto i = 0; i < cell_count; i++)
			{
				if (values[i].type < 2 || values[i].type > 8)
				{
					reader.read_string(values[i].string);
				}
			}
		}

		reader.read_array(asset->cellIndices, 3, cell_count);
		reader.read_array(asset->cellOrder, 1, cell_count);
		reader.read_array(asset->hashes, 7, asset->hashCount);
	}

	void structured_table::read(auto& reader, StructuredTable* asset)
	{
		const auto cell_count = asset->entryCount * asset->propertyCount;
		if (const auto cells = reader.read_array(asset->cells, 7, cell_count))
		{
			for (auto i = 0; i < cell_count; i++)
			{
				if (cells[i].type != 2)
				{
					reader.read_string(cells[i].string);
				}
			}
		}

		reader.read_array(asset->cellIndices, 3, cell_count);

		if (const auto properties = reader.read_array(asset->properties, 7, asset->propertyCount))
		{
			for (auto i = 0; i < asset->propertyCount; i++)
			{
				reader.read_string(properties[i].name);
			}
		}

		reader.read_array(asset->propertyIndices, 3, asset->propertyCount);
	}

	void script_bundle_list::read(auto& reader, ScriptBundleList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_SCRIPTBUNDLE, asset->bundles, asset->bundleCount);
	}

	void map_table_entry::read(auto& reader, MapTableEntry* asset)
	{
		const auto read_strings = [&](char**& field, const std::uint32_t count)
		{
			if (const auto strings = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_string(strings[i]);
				}
			}
		};

		reader.read_string(asset->name);
		reader.read_string(asset->string0);
		reader.read_string(asset->string1);

		for (auto& image : asset->previewImages)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}

		if (const auto sets = reader.read_array(asset->compassSets, 7, asset->compassSetCount))
		{
			for (auto i = 0u; i < asset->compassSetCount; i++)
			{
				for (auto& image : sets[i].images)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, image);
				}
			}
		}

		reader.read_string(asset->string2);
		reader.read_string(asset->string3);
		reader.read_asset(ASSET_TYPE_OBJECTIVELIST, asset->objectiveList);
		reader.read_string(asset->string4);
		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLELIST, asset->scriptBundleList);
		reader.read_string(asset->string5);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		read_strings(asset->strings0, asset->stringCount0);
		read_strings(asset->strings1, asset->stringCount0);
		read_strings(asset->strings2, asset->stringCount1);
		read_strings(asset->strings3, asset->stringCount2);
		read_strings(asset->strings4, asset->stringCount2);
		reader.read_asset(ASSET_TYPE_PLAYERGLOBALTUNABLES, asset->playerGlobalTunables);

		for (auto& tunables : asset->overheadCameraTunables)
		{
			reader.read_asset(ASSET_TYPE_OVERHEADCAMERATUNABLES, tunables);
		}

		for (auto& tunables : asset->compassTunables)
		{
			reader.read_asset(ASSET_TYPE_COMPASSTUNABLES, tunables);
		}

		read_script_bundle_object(reader, asset->bundle);
	}

	void map_table::read(auto& reader, MapTable* asset)
	{
		reader.read_asset_array(ASSET_TYPE_MAPTABLEENTRY, asset->entries, asset->entryCount);
		reader.read_array(asset->entryIndices, 3, asset->entryCount);
		reader.read_array(asset->hashes0, 7, asset->hashCount0);
		reader.read_array(asset->hashes1, 7, asset->hashCount1);
	}

	void map_table_list::read(auto& reader, MapTableList* asset)
	{
		for (auto& span : asset->spans)
		{
			reader.read_array(span.hashes, 7, span.count);
		}
	}

	void bitfield::read(auto& reader, BitField* asset)
	{
		reader.read_array(asset->bits, 7, asset->rowCount * asset->columnCount);
	}

	void leaderboard_def::read(auto& reader, LeaderboardDef* asset)
	{
		if (const auto columns = reader.read_array(asset->columns, 7, asset->columnCount))
		{
			for (auto i = 0; i < asset->columnCount; i++)
			{
				reader.read_string(columns[i].name);
			}
		}
	}

	void cg_media_table::read(auto& reader, CgMediaTable* asset)
	{
		for (auto& tables : asset->footstepTables)
		{
			for (auto& table : tables)
			{
				reader.read_asset(ASSET_TYPE_FOOTSTEPTABLE, table);
			}
		}

		for (auto& sound : asset->surfaceSoundDefs)
		{
			reader.read_asset(ASSET_TYPE_SURFACESOUNDDEF, sound);
		}

		for (auto& material : asset->materials0)
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, material);
		}

		for (auto& image : asset->images)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}

		for (auto& material : asset->materials1)
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, material);
		}

		reader.read_asset(ASSET_TYPE_SURFACEFXTABLE, asset->surfaceFxTable);
		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->physPreset);
		reader.read_asset(ASSET_TYPE_BEAM, asset->beam);

		for (auto& effect : asset->effects)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}
	}

	void bullet_penetration::read(auto&, BulletPenetration*)
	{
	}

	void loc_dmg_table::read(auto&, LocDmgTable*)
	{
	}

	namespace
	{
		void read_playlist_settings(auto& reader, PlaylistSetting*& field, const std::int32_t count)
		{
			const auto settings = reader.read_array(field, 7, count);
			for (auto i = 0; settings && i < count; i++)
			{
				reader.read_string(settings[i].name);

				const auto strings = reader.read_array(settings[i].strings, 7, settings[i].stringCount);
				for (auto j = 0; strings && j < settings[i].stringCount; j++)
				{
					reader.read_string(strings[j].string);
				}

				reader.read_array(settings[i].values, 7, settings[i].valueCount);
			}
		}

		void read_playlist_setting_group(auto& reader, PlaylistSettingGroup*& field)
		{
			if (const auto group = reader.read_single(field, 7))
			{
				read_playlist_settings(reader, group->settings, group->settingCount);
			}
		}

		void read_playlist(auto& reader, Playlist& playlist)
		{
			reader.read_string(playlist.string0);
			for (auto& image : playlist.images)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, image);
			}

			reader.read_string(playlist.string1);

			const auto strings = reader.read_array(playlist.strings, 7, playlist.stringCount);
			for (auto i = 0; strings && i < playlist.stringCount; i++)
			{
				reader.read_string(strings[i].string);
			}

			read_playlist_settings(reader, playlist.settings, playlist.settingCount);

			const auto variants = reader.read_array(playlist.variants, 7, playlist.variantCount);
			for (auto i = 0; variants && i < playlist.variantCount; i++)
			{
				reader.read_single(variants[i].info, 7);
				read_playlist_setting_group(reader, variants[i].settingGroup);
			}

			read_script_bundle_object(reader, playlist.bundle);
		}
	}

	void playlists::read(auto& reader, Playlists* asset)
	{
		const auto infos = reader.read_array(asset->infos, 7, asset->infoCount);
		for (auto i = 0; infos && i < asset->infoCount; i++)
		{
			reader.read_single(infos[i], 7);
		}

		const auto setting_groups = reader.read_array(asset->settingGroups, 7, asset->settingGroupCount);
		for (auto i = 0; setting_groups && i < asset->settingGroupCount; i++)
		{
			read_playlist_setting_group(reader, setting_groups[i]);
		}

		const auto sets = reader.read_array(asset->sets, 7, asset->setCount);
		for (auto i = 0; sets && i < asset->setCount; i++)
		{
			const auto list = reader.read_array(sets[i].playlists, 7, sets[i].playlistCount);
			for (auto j = 0; list && j < sets[i].playlistCount; j++)
			{
				read_playlist(reader, list[j]);
			}
		}
	}

	void playlist_schedule::read(auto& reader, PlaylistSchedule* asset)
	{
		const auto entries = reader.read_array(asset->entries, 7, asset->entryCount);
		for (auto i = 0; entries && i < asset->entryCount; i++)
		{
			reader.read_array(entries[i].values, 3, entries[i].valueCount);
		}
	}

	void arena_seasons::read(auto& reader, ArenaSeasons* asset)
	{
		const auto seasons = reader.read_array(asset->seasons, 7, asset->seasonCount);
		for (auto i = 0; seasons && i < asset->seasonCount; i++)
		{
			reader.read_single(seasons[i].seasonName, 0);
			reader.read_array(seasons[i].values, 3, seasons[i].valueCount);
		}

		reader.read_array(asset->ranges, 7, asset->rangeCount);
	}

	void scenario::read(auto& reader, Scenario* asset)
	{
		reader.read_array(asset->items, 7, asset->itemCount);

		const auto groups = reader.read_array(asset->groups, 7, asset->groupCount);
		for (auto i = 0ull; groups && i < asset->groupCount; i++)
		{
			reader.read_array(groups[i].values, 7, groups[i].valueCount);
		}
	}

	void dlog_event::read(auto& reader, DLogEvent* asset)
	{
		reader.read_string(asset->eventName);
		reader.read_string(asset->description);
		reader.read_asset(ASSET_TYPE_DDL, asset->ddl);

		const auto field_lists = reader.read_array(asset->fieldLists, 7, asset->fieldListCount);
		for (auto i = 0; field_lists && i < asset->fieldListCount; i++)
		{
			if (const auto list = reader.read_single(field_lists[i].list, 7))
			{
				reader.read_string(list->name);
				reader.read_array(list->fields, 7, list->fieldCount);
			}
		}

		reader.read_array(asset->fields, 7, asset->fieldCount);

		const auto int_lists = reader.read_array(asset->intLists, 7, asset->intListCount);
		for (auto i = 0; int_lists && i < asset->intListCount; i++)
		{
			if (const auto list = reader.read_single(int_lists[i].list, 7))
			{
				reader.read_array(list->values, 3, list->valueCount);
			}
		}
	}
}

namespace zonetool::t9
{
	template void footstep_table::read(zone_reader&, FootstepTable*);
	template void string_table::read(zone_reader&, StringTable*);
	template void structured_table::read(zone_reader&, StructuredTable*);
	template void script_bundle_list::read(zone_reader&, ScriptBundleList*);
	template void map_table_entry::read(zone_reader&, MapTableEntry*);
	template void map_table::read(zone_reader&, MapTable*);
	template void map_table_list::read(zone_reader&, MapTableList*);
	template void bitfield::read(zone_reader&, BitField*);
	template void leaderboard_def::read(zone_reader&, LeaderboardDef*);
	template void cg_media_table::read(zone_reader&, CgMediaTable*);
	template void bullet_penetration::read(zone_reader&, BulletPenetration*);
	template void loc_dmg_table::read(zone_reader&, LocDmgTable*);
	template void playlists::read(zone_reader&, Playlists*);
	template void playlist_schedule::read(zone_reader&, PlaylistSchedule*);
	template void arena_seasons::read(zone_reader&, ArenaSeasons*);
	template void scenario::read(zone_reader&, Scenario*);
	template void dlog_event::read(zone_reader&, DLogEvent*);
	template void footstep_table::read(zone_writer&, FootstepTable*);
	template void string_table::read(zone_writer&, StringTable*);
	template void structured_table::read(zone_writer&, StructuredTable*);
	template void script_bundle_list::read(zone_writer&, ScriptBundleList*);
	template void map_table_entry::read(zone_writer&, MapTableEntry*);
	template void map_table::read(zone_writer&, MapTable*);
	template void map_table_list::read(zone_writer&, MapTableList*);
	template void bitfield::read(zone_writer&, BitField*);
	template void leaderboard_def::read(zone_writer&, LeaderboardDef*);
	template void cg_media_table::read(zone_writer&, CgMediaTable*);
	template void bullet_penetration::read(zone_writer&, BulletPenetration*);
	template void loc_dmg_table::read(zone_writer&, LocDmgTable*);
	template void playlists::read(zone_writer&, Playlists*);
	template void playlist_schedule::read(zone_writer&, PlaylistSchedule*);
	template void arena_seasons::read(zone_writer&, ArenaSeasons*);
	template void scenario::read(zone_writer&, Scenario*);
	template void dlog_event::read(zone_writer&, DLogEvent*);
}
