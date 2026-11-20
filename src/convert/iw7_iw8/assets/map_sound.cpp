#include <std_include.hpp>

#include "../include.hpp"
#include "map_sound.hpp"
#include "sound.hpp"

#include <iw7/assets/sound_bank.hpp>
#include <iw7/assets/sound_globals.hpp>
#include <iw8/assets/sound.hpp>
#include <iw8/assets/streamkey.hpp>

namespace zonetool::iw7::converter::iw8::map_sound
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename T, typename S>
		T* convert_array(const S* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(T) == sizeof(S));

			if (!source || !count)
			{
				return nullptr;
			}

			auto* copy = allocator.allocate_array<T>(count);
			std::memcpy(copy, source, sizeof(S) * count);
			return copy;
		}

		std::vector<std::string> find_asset_bank_paths(SndBank* asset, const bool streamed)
		{
			std::vector<std::string> paths;
			const auto add_path = [&](const std::filesystem::path& path)
			{
				if (std::filesystem::exists(path) && !std::ranges::contains(paths, path.string()))
				{
					paths.emplace_back(path.string());
				}
			};

			const std::filesystem::path own = sound_asset_bank::create_path(asset, streamed);
			add_path(own);
			add_path(std::filesystem::path(own).replace_filename("patch_" + own.filename().string()));

			db_enum_assets(ASSET_TYPE_SOUND_BANK, [&](void* header)
			{
				if (const auto bank = static_cast<SndBank*>(header); bank->zone)
				{
					add_path(sound_asset_bank::create_path(bank, streamed));
				}
			});

			return paths;
		}

		const target::SndAssetBankEntry* add_asset(sound::asset_bank_builder& builder, const std::vector<std::string>& paths, const SndStringHash asset_id)
		{
			for (const auto& path : paths)
			{
				if (const auto entry = builder.add(path, asset_id))
				{
					return entry;
				}
			}

			return nullptr;
		}

		unsigned int map_channel(const std::vector<unsigned int>& channels, const unsigned int index)
		{
			return index < channels.size() ? channels[index] : 0;
		}

		int map_volmod(const std::vector<int>& volmods, const unsigned int index)
		{
			return index < volmods.size() ? volmods[index] : sound::convert_volmod(static_cast<int>(index));
		}

		bool has_duck(const SndBank* bank, const SndStringHash id)
		{
			return id && std::ranges::any_of(std::span(bank->ducks, bank->duckCount), [&](const SndDuck& duck) { return duck.id == id; });
		}

		bool is_exterior(const ZoneDef& zone, const SoundTable& table)
		{
			if (zone.unkIndex < 0 || static_cast<unsigned int>(zone.unkIndex) >= table.unkCount || !table.unk[zone.unkIndex].name)
			{
				return false;
			}

			return std::string_view(table.unk[zone.unkIndex].name).starts_with("ext");
		}

		target::ZoneDef convert_zone(const ZoneDef& zone, const SoundTable& table)
		{
			target::ZoneDef result{};
			result.id = zone.id;
			result.stateId = zone.stateId;
			result.reverbIndex = zone.reverbIndex;
			result.startOcclusionIndex = zone.startOcclusionIndex;
			result.numOcclusion = zone.numOcclusion;
			result.startFilterIndex = zone.startFilterIndex;
			result.numFilter = zone.numFilter;
			result.startMixIndex = zone.startMixIndex;
			result.numMix = zone.numMix;
			result.ambientStream = zone.ambientStream;
			result.startAdsrZoneIndex = zone.startAdsrZoneIndex;
			result.numAdsrZone = zone.numAdsrZone;
			result.startNPCAdsrZoneIndex = zone.startNPCAdsrZoneIndex;
			result.numNPCAdsrZone = zone.numNPCAdsrZone;
			result.ambientDefIndex = zone.ambientDefIndex;
			result.startFullOccIndex = zone.startFullOccIndex;
			result.numDisableFullOcc = zone.numDisableFullOcc;
			result.startPlayerBreathStateIndex = zone.startPlayerBreathStateIndex;
			result.numPlayerBreathStates = zone.numPlayerBreathStates;
			result.weapReflId = zone.weapReflId;
			result.exterior = is_exterior(zone, table);
			result.zoneName = zone.zoneName;
			result.stateName = zone.stateName;
			result.ambientName = zone.ambientName;
			result.mixName = zone.mixName;
			result.reverbName = zone.reverbName;
			result.filterName = zone.filterName;
			result.occlusionName = zone.occlusionName;
			result.plrAdsrName = zone.plrAdsrName;
			result.npcAdsrName = zone.npcAdsrName;
			result.weapReflName = zone.weapReflName;
			result.fullOccName = zone.fullOccName;
			result.playerBreathStateName = zone.playerBreathStateName;
			result.contextType1 = zone.contextType1;
			result.contextValue1 = zone.contextValue1;
			result.contextType2 = zone.contextType2;
			result.contextValue2 = zone.contextValue2;
			result.contextType3 = zone.contextType3;
			result.contextValue3 = zone.contextValue3;
			result.contextType4 = zone.contextType4;
			result.contextValue4 = zone.contextValue4;
			return result;
		}

		target::SoundTable convert_sound_table(const SoundTable& table, const std::vector<unsigned int>& channels,
			const std::vector<int>& volmods, utils::memory::allocator& allocator)
		{
			target::SoundTable result{};
			result.id = table.id;
			result.adsrSettings = convert_array<target::ADSRSetting>(table.adsrSettings, table.adsrCount, allocator);
			result.adsrCount = table.adsrCount;
			result.ambientDefs = convert_array<target::AmbientDef>(table.ambientDefs, table.ambientDefCount, allocator);
			result.ambientDefCount = table.ambientDefCount;
			result.ambientEvents = convert_array<target::AmbientEvent>(table.ambientEvents, table.ambientEventCount, allocator);
			result.ambientEventCount = table.ambientEventCount;
			result.ambientElements = convert_array<target::AmbientElement>(table.ambientElements, table.ambientElementsCount, allocator);
			result.ambientElementsCount = table.ambientElementsCount;
			result.filters = convert_array<target::FilterDef>(table.filters, table.filterCount, allocator);
			result.filterCount = table.filterCount;
			result.mixes = convert_array<target::MixDef>(table.mixes, table.mixCount, allocator);
			result.mixCount = table.mixCount;
			result.npcADSRZones = convert_array<target::AdsrZoneEntry>(table.npcADSRZones, table.npcADSRZoneCount, allocator);
			result.npcADSRZoneCount = table.npcADSRZoneCount;
			result.occlusionFilters = convert_array<target::OccludeDef>(table.occlusionFilters, table.occlusionFilterCount, allocator);
			result.occlusionFilterCount = table.occlusionFilterCount;
			result.playerADSRZones = convert_array<target::AdsrZoneEntry>(table.playerADSRZones, table.playerADSRZoneCount, allocator);
			result.playerADSRZoneCount = table.playerADSRZoneCount;
			result.reverbs = convert_array<target::ReverbDef>(table.reverbs, table.reverbCount, allocator);
			result.reverbCount = table.reverbCount;
			result.timeScaleSettings = convert_array<target::TimescaleEntry>(table.timeScaleSettings, table.timeScaleSettingCount, allocator);
			result.timeScaleSettingCount = table.timeScaleSettingCount;
			result.fullOcclusionDefs = convert_array<target::FullOcclusionDef>(table.fullOcclusionDefs, table.fullOcclusionDefCount, allocator);
			result.fullOcclusionDefCount = table.fullOcclusionDefCount;
			result.plrBreathStateDefs = convert_array<target::PlayerBreathStateDef>(table.plrBreathStateDefs, table.plrBreathStateDefCount, allocator);
			result.plrBreathStateDefCount = table.plrBreathStateDefCount;

			for (auto i = 0u; i < result.filterCount; i++)
			{
				result.filters[i].entChannelIdx = static_cast<char>(map_channel(channels, table.filters[i].entChannelIdx));
			}

			for (auto i = 0u; i < result.mixCount; i++)
			{
				result.mixes[i].volModIndex = static_cast<unsigned short>(map_volmod(volmods, table.mixes[i].volModIndex));
			}

			for (auto i = 0u; i < result.occlusionFilterCount; i++)
			{
				result.occlusionFilters[i].entChannelIdx = static_cast<unsigned short>(map_channel(channels, table.occlusionFilters[i].entChannelIdx));
			}

			for (auto i = 0u; i < result.timeScaleSettingCount; i++)
			{
				if (table.timeScaleSettings[i].entChannelIdx >= 0)
				{
					result.timeScaleSettings[i].entChannelIdx = static_cast<int>(map_channel(channels, table.timeScaleSettings[i].entChannelIdx));
				}
			}

			for (auto i = 0u; i < result.fullOcclusionDefCount; i++)
			{
				if (table.fullOcclusionDefs[i].entChannelIdx >= 0)
				{
					result.fullOcclusionDefs[i].entChannelIdx = static_cast<int>(map_channel(channels, table.fullOcclusionDefs[i].entChannelIdx));
				}
			}

			result.zones = allocator.allocate_array<target::ZoneDef>(table.zoneCount);
			result.zoneCount = table.zoneCount;
			for (auto i = 0u; i < table.zoneCount; i++)
			{
				result.zones[i] = convert_zone(table.zones[i], table);
			}

			return result;
		}

		target::SndSendEffectParams convert_send_effect(const SndSendEffectParams& effect)
		{
			target::SndSendEffectParams result{};
			std::memcpy(result.name, effect.name, sizeof(result.name));
			result.id = effect.id;
			result.smoothing = effect.smoothing;
			result.earlyTime = effect.earlyTime;
			result.lateTime = effect.lateTime;
			result.earlyGain = effect.earlyGain;
			result.lateGain = effect.lateGain;
			result.lateGainProx = effect.lateGainProx;
			result.returnGain = effect.returnGain;
			result.earlyLpf = effect.earlyLpf;
			result.lateLpf = effect.lateLpf;
			result.inputLpf = effect.inputLpf;
			result.dampLpf = effect.dampLpf;
			result.wallReflect = effect.wallReflect;
			result.dryGain = effect.dryGain;
			result.earlySize = effect.earlySize;
			result.lateSize = effect.lateSize;
			result.diffusion = effect.diffusion;
			result.rearLevel = effect.rearLevel;

			const auto disable_delay = [](float& level, float& lpf_cutoff)
			{
				level = 1.0f;
				lpf_cutoff = 24000.0f;
			};

			disable_delay(result.delay1Level, result.delay1LpfCutoff);
			disable_delay(result.delay2Level, result.delay2LpfCutoff);
			disable_delay(result.delay3Level, result.delay3LpfCutoff);
			disable_delay(result.delay4Level, result.delay4LpfCutoff);
			disable_delay(result.delay5Level, result.delay5LpfCutoff);
			return result;
		}

		float* convert_duck_values(const float* values, const std::vector<unsigned int>& channels, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate_array<float>(128);
			std::fill_n(result, 128, 1.0f);
			if (!values)
			{
				return result;
			}

			std::array<bool, 128> converted{};
			for (auto i = 0u; i < std::min<std::size_t>(channels.size(), 128); i++)
			{
				if (const auto channel = channels[i]; channel < converted.size() && !converted[channel])
				{
					result[channel] = values[i];
					converted[channel] = true;
				}
			}

			return result;
		}

		target::SndDuck convert_duck(const SndDuck& duck, const std::vector<unsigned int>& channels, utils::memory::allocator& allocator)
		{
			target::SndDuck result{};
			std::memcpy(result.name, duck.name, sizeof(result.name));
			result.id = duck.id;
			result.fadeIn = duck.fadeIn;
			result.fadeOut = duck.fadeOut;
			result.startDelay = duck.startDelay;
			result.minDistance = duck.minDistance;
			result.distance = duck.distance;
			result.length = duck.length;
			result.fadeInCurve = duck.fadeInCurve;
			result.fadeOutCurve = duck.fadeOutCurve;
			result.attenuation = convert_duck_values(duck.attenuation, channels, allocator);
			result.hpf = allocator.allocate_array<float>(128);
			result.lpf = convert_duck_values(duck.lpf, channels, allocator);
			result.focusAmount = allocator.allocate_array<float>(128);
			result.focusCone = allocator.allocate_array<unsigned int>(128);
			result.updateWhilePaused = duck.updateWhilePaused;
			result.trackAmplitude = duck.trackAmplitude;
			result.disableInSplitscreen = duck.disableInSplitscreen;
			result.aliasAttenuation = duck.aliasAttenuation;
			result.aliasLpf = duck.aliasLpf;
			result.duckAlias = duck.duckAlias;
			return result;
		}
	}

	void dump(SndBank* asset)
	{
		if (!asset->name || !asset->zone || !mark_dumped(ASSET_TYPE_SOUND_BANK, asset->name))
		{
			return;
		}

		std::vector<unsigned int> channels(std::size(ENTCHANNELS));
		for (auto i = 0u; i < channels.size(); i++)
		{
			channels[i] = sound::convert_channel(i);
		}

		std::vector<int> volmods(std::size(VOLMODS));
		for (auto i = 0u; i < volmods.size(); i++)
		{
			volmods[i] = sound::convert_volmod(static_cast<int>(i));
		}

		const auto globals = static_cast<SndGlobals*>(db_find_asset(ASSET_TYPE_SOUND_GLOBALS, "singleton"));
		if (!globals)
		{
			ZONETOOL_WARNING("Sound globals are not loaded, volumes of \"%s\" are not scaled", asset->name);
		}

		const auto loaded_paths = find_asset_bank_paths(asset, false);
		const auto streamed_paths = find_asset_bank_paths(asset, true);

		utils::memory::allocator allocator;
		sound::asset_bank_builder loaded;
		sound::asset_bank_builder streamed;
		std::vector<target::SndAliasList> lists;
		for (auto i = 0u; i < asset->aliasCount; i++)
		{
			const auto& list = asset->alias[i];

			target::SndAliasList result{};
			result.aliasName = list.aliasName;
			result.id = list.id;
			result.sequence = list.sequence;
			result.head = allocator.allocate_array<target::SndAlias>(list.count);
			for (auto j = 0; j < list.count; j++)
			{
				const auto& alias = list.head[j];
				const auto is_streamed = alias.flags.type == SAT_STREAMED || alias.flags.type == SAT_PRIMED;

				const target::SndAssetBankEntry* entry{};
				if (alias.assetFileName)
				{
					entry = is_streamed ? add_asset(streamed, streamed_paths, alias.assetId) : add_asset(loaded, loaded_paths, alias.assetId);
					if (!entry)
					{
						ZONETOOL_WARNING("Sound asset \"%s\" of alias \"%s\" is not in a sound asset bank", alias.assetFileName, list.aliasName);
						continue;
					}
				}

				auto& converted = result.head[result.count++];
				converted = sound::convert_alias(alias, entry, globals);
				converted.duck = has_duck(asset, alias.duck) ? alias.duck : 0;
			}

			if (result.count)
			{
				lists.emplace_back(result);
			}
		}

		if (asset->soundTable.unkCount)
		{
			ZONETOOL_WARNING("Sound bank \"%s\": %u weapon reflections are not converted", asset->name, asset->soundTable.unkCount);
		}

		if (asset->musicSetCount)
		{
			ZONETOOL_WARNING("Sound bank \"%s\": %u music sets are not converted", asset->name, asset->musicSetCount);
		}

		const auto data = loaded.build(asset->zone);

		const auto key = allocator.allocate<target::StreamKey>();
		key->name = allocator.duplicate_string(std::format("{}_all_loaded", asset->zone));
		key->data.residentData = const_cast<char*>(data.data());
		key->dataSize = static_cast<unsigned int>(data.size());
		key->behaviorIndex = 2;
		key->flags = 0x3;

		const auto bank = allocator.allocate<target::SndBankResident>();
		bank->bank.name = asset->name;
		bank->bank.zone = asset->zone;
		bank->bank.gameLanguage = asset->gameLanguage;
		bank->bank.soundLanguage = asset->soundLanguage;
		bank->bank.aliasCount = static_cast<unsigned int>(lists.size());
		bank->bank.alias = allocator.allocate_array<target::SndAliasList>(lists.size());
		std::ranges::copy(lists, bank->bank.alias);
		bank->bank.aliasIndex = sound::build_alias_index(bank->bank.alias, static_cast<unsigned short>(lists.size()), allocator);

		bank->bank.sendEffectCount = asset->sendEffectCount;
		bank->bank.sendEffects = allocator.allocate_array<target::SndSendEffectParams>(asset->sendEffectCount);
		for (auto i = 0u; i < asset->sendEffectCount; i++)
		{
			bank->bank.sendEffects[i] = convert_send_effect(asset->sendEffects[i]);
		}

		bank->bank.soundTable = convert_sound_table(asset->soundTable, channels, volmods, allocator);

		bank->bank.duckCount = asset->duckCount;
		bank->bank.ducks = allocator.allocate_array<target::SndDuck>(asset->duckCount);
		for (auto i = 0u; i < asset->duckCount; i++)
		{
			bank->bank.ducks[i] = convert_duck(asset->ducks[i], channels, allocator);
		}

		bank->bank.streamInfo.loadedStreamKey = key;
		bank->bank.streamInfo.alwaysLoaded = true;

		auto file = filesystem::file("soundbank\\"s + asset->zone + ".sabs");
		file.open("wb");
		file.write(streamed.build(asset->zone));
		file.close();

		target::stream_key::dump(key);
		target::sound_bank::dump(bank);
		add_csv_row("soundbank", bank->bank.name);

		ZONETOOL_INFO("Converted soundbank \"%s\": %zu aliases, %zu bytes of loaded audio", asset->name, lists.size(), data.size());
	}
}
