#include <std_include.hpp>
#include "sound.hpp"

namespace zonetool::iw8
{
	void read_alias_list(zone_reader& reader, SndAliasList& list)
	{
		reader.read_string(list.aliasName);
		if (const auto aliases = reader.read_array(list.head, 7, list.count))
		{
			for (auto i = 0; i < list.count; i++)
			{
				reader.read_string(aliases[i].aliasName);
				reader.read_string(aliases[i].subtitle);
				reader.read_string(aliases[i].secondaryAliasName);
				reader.read_string(aliases[i].assetFileName);
			}
		}
	}

	namespace
	{
		template <typename T>
		T* read_named_array(zone_reader& reader, T*& field, const std::size_t alignment, const std::size_t count)
		{
			const auto entries = reader.read_array(field, alignment, count);
			if (entries)
			{
				for (std::size_t i = 0; i < count; i++)
				{
					reader.read_string(entries[i].name);
				}
			}

			return entries;
		}

		void read_adsr_zones(zone_reader& reader, AdsrZoneEntry*& field, const std::size_t count)
		{
			if (const auto zones = read_named_array(reader, field, 7, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					reader.read_string(zones[i].weaponName);
				}
			}
		}

		void read_zone_def(zone_reader& reader, ZoneDef& zone)
		{
			reader.read_string(zone.ambientStream);
			reader.read_string(zone.zoneName);
			reader.read_string(zone.stateName);
			reader.read_string(zone.ambientName);
			reader.read_string(zone.mixName);
			reader.read_string(zone.reverbName);
			reader.read_string(zone.filterName);
			reader.read_string(zone.occlusionName);
			reader.read_string(zone.plrAdsrName);
			reader.read_string(zone.npcAdsrName);
			reader.read_string(zone.weapReflName);
			reader.read_string(zone.fullOccName);
			reader.read_string(zone.playerBreathStateName);
		}

		void read_sound_table(zone_reader& reader, SoundTable& table)
		{
			read_named_array(reader, table.adsrSettings, 7, table.adsrCount);
			read_named_array(reader, table.ambientDefs, 7, table.ambientDefCount);
			reader.read_array(table.ambientEvents, 3, table.ambientEventCount);

			if (const auto elements = reader.read_array(table.ambientElements, 7, table.ambientElementsCount))
			{
				for (auto i = 0u; i < table.ambientElementsCount; i++)
				{
					reader.read_string(elements[i].aliasName);
				}
			}

			reader.read_array(table.filters, 3, table.filterCount);
			reader.read_array(table.mixes, 3, table.mixCount);
			read_adsr_zones(reader, table.npcADSRZones, table.npcADSRZoneCount);
			reader.read_array(table.occlusionFilters, 3, table.occlusionFilterCount);
			read_adsr_zones(reader, table.playerADSRZones, table.playerADSRZoneCount);
			reader.read_array(table.reverbs, 3, table.reverbCount);
			reader.read_array(table.timeScaleSettings, 3, table.timeScaleSettingCount);

			if (const auto zones = reader.read_array(table.zones, 7, table.zoneCount))
			{
				for (auto i = 0u; i < table.zoneCount; i++)
				{
					read_zone_def(reader, zones[i]);
				}
			}

			reader.read_array(table.fullOcclusionDefs, 3, table.fullOcclusionDefCount);

			if (const auto states = reader.read_array(table.plrBreathStateDefs, 7, table.plrBreathStateDefCount))
			{
				for (auto i = 0u; i < table.plrBreathStateDefCount; i++)
				{
					reader.read_string(states[i].inhaleAlias);
					reader.read_string(states[i].exhaleAlias);
				}
			}
		}

		void read_aliases(zone_reader& reader, SndBank& bank)
		{
			if (const auto lists = reader.read_array(bank.alias, 7, bank.aliasCount))
			{
				for (auto i = 0u; i < bank.aliasCount; i++)
				{
					read_alias_list(reader, lists[i]);
				}
			}
		}

		void read_ducks(zone_reader& reader, SndBank& bank)
		{
			if (const auto ducks = reader.read_array(bank.ducks, 7, bank.duckCount))
			{
				for (auto i = 0u; i < bank.duckCount; i++)
				{
					auto& duck = ducks[i];
					reader.read_array(duck.attenuation, 31, 128);
					reader.read_array(duck.hpf, 31, 128);
					reader.read_array(duck.lpf, 31, 128);
					reader.read_array(duck.focusAmount, 31, 128);
					reader.read_array(duck.focusCone, 3, 128);
				}
			}
		}

		void read_music_sets(zone_reader& reader, SndBank& bank)
		{
			if (const auto sets = reader.read_array(bank.musicSets, 7, bank.musicSetCount))
			{
				for (auto i = 0u; i < bank.musicSetCount; i++)
				{
					if (const auto states = reader.read_array(sets[i].states, 7, sets[i].stateCount))
					{
						for (auto j = 0u; j < sets[i].stateCount; j++)
						{
							reader.read_array(states[j].loops, 3, states[j].loopCount);
						}
					}
				}
			}
		}

		void read_weapon_reflections(zone_reader& reader, SndBank& bank)
		{
			if (const auto reflections = read_named_array(reader, bank.weapReflections, 7, bank.weapReflectionCount))
			{
				for (auto i = 0u; i < bank.weapReflectionCount; i++)
				{
					reader.read_string(reflections[i].weaponClass);
					reader.read_string(reflections[i].type);
				}
			}
		}

		void read_bank(zone_reader& reader, SndBank& bank)
		{
			reader.read_string(bank.name);
			reader.read_string(bank.zone);
			reader.read_string(bank.gameLanguage);
			reader.read_string(bank.soundLanguage);
			read_aliases(reader, bank);
			reader.read_array(bank.aliasIndex, 1, bank.aliasCount);
			reader.read_array(bank.sendEffects, 3, bank.sendEffectCount);
			read_sound_table(reader, bank.soundTable);
			read_ducks(reader, bank);
			read_music_sets(reader, bank);
			read_weapon_reflections(reader, bank);
			reader.read_array(bank.dopplerPresets, 3, bank.dopplerPresetCount);
			reader.read_array(bank.occlusionShapes, 3, bank.occlusionShapeCount);
			reader.read_array(bank.focusCones, 3, bank.focusConeCount);
			reader.read_array(bank.vfCurves, 3, bank.vfCurveCount);
			reader.read_array(bank.lpfCurves, 3, bank.lpfCurveCount);
			reader.read_array(bank.hpfCurves, 3, bank.hpfCurveCount);
			reader.read_array(bank.rvbCurves, 3, bank.rvbCurveCount);
			reader.read_array(bank.speakerMaps, 3, bank.speakerMapCount);
			reader.read_array(bank.futzs, 3, bank.futzCount);
			reader.read_asset(ASSET_TYPE_STREAM_KEY, bank.streamInfo.loadedStreamKey);
		}
	}

	void sound_globals::read(zone_reader& reader, SndGlobals* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->volmodinfo, 3, asset->volmodinfoCount);
		reader.read_array(asset->entchannelInfo, 3, asset->entchannelCount);

		if (const auto contexts = reader.read_array(asset->contexts, 7, asset->contextCount))
		{
			for (auto i = 0u; i < asset->contextCount; i++)
			{
				reader.read_string(contexts[i].type);
			}
		}

		if (const auto values = reader.read_array(asset->contextValues, 7, asset->contextValueCount))
		{
			for (auto i = 0u; i < asset->contextValueCount; i++)
			{
				reader.read_string(values[i].value);
			}
		}

		reader.read_array(asset->masters, 3, asset->masterCount);
		reader.read_array(asset->whizbys, 3, asset->whizbyCount);
		reader.read_array(asset->perkinfo, 3, asset->perkinfoCount);
		reader.pop_stream();
	}

	void sound_bank::read(zone_reader& reader, SndBankResident* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		read_bank(reader, asset->bank);
		reader.pop_stream();
	}

	void sound_bank_transient::read(zone_reader& reader, SndBankTransient* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		read_bank(reader, asset->bank);
		reader.pop_stream();
	}

	void sfx_package::read(zone_reader& reader, WeaponSFXPackage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto sounds = reader.read_single(asset->sounds, 7))
		{
			reader.read_string(sounds->name);
			for (auto& sound : sounds->sounds)
			{
				reader.read_string(sound.name);
			}
		}

		reader.read_asset(ASSET_TYPE_SFX_PACKAGE, asset->transientFallbackPackage);
		reader.read_asset(ASSET_TYPE_SOUND_BANK_TRANSIENT, asset->detailSoundBankNPC);
		reader.read_asset(ASSET_TYPE_SOUND_BANK_TRANSIENT, asset->detailSoundBankPlayer);
		reader.read_string(asset->szAdsrBaseSetting);
		reader.read_string(asset->szWeapSndReflectionClass);
		reader.read_string(asset->szWeapSndReflFrontOverride);
		reader.read_string(asset->szWeapSndReflDistantOverride);
		reader.read_string(asset->szMountEnterExitString);
		reader.read_string(asset->szMountImpactString);
		reader.pop_stream();
	}
}
