#include <std_include.hpp>

#include "../include.hpp"
#include "sound.hpp"

#include <iw7/assets/sound_bank.hpp>
#include <iw7/assets/sound_globals.hpp>
#include <iw7/assets/weapon_sfx_package.hpp>
#include <iw7/zone/loader.hpp>
#include <iw8/assets/sound.hpp>
#include <iw8/xpak.hpp>

#include <io/game_files.hpp>

namespace zonetool::iw8::converter::iw7::sound
{
	namespace
	{
		namespace target = zonetool::iw7;

		std::filesystem::path iw7_game_folder;

		const std::pair<std::size_t, target::SndAliasLookup target::WeaponSFXPackageSounds::*> sound_slots[] =
		{
			{0, &target::WeaponSFXPackageSounds::pickupSound},
			{1, &target::WeaponSFXPackageSounds::pickupSoundPlayer},
			{2, &target::WeaponSFXPackageSounds::ammoPickupSound},
			{4, &target::WeaponSFXPackageSounds::projectileSound},
			{5, &target::WeaponSFXPackageSounds::pullbackSound},
			{6, &target::WeaponSFXPackageSounds::pullbackSoundPlayer},
			{7, &target::WeaponSFXPackageSounds::pullbackSoundQuick},
			{8, &target::WeaponSFXPackageSounds::pullbackSoundQuickPlayer},
			{11, &target::WeaponSFXPackageSounds::fireSound},
			{12, &target::WeaponSFXPackageSounds::fireSoundPlayer},
			{13, &target::WeaponSFXPackageSounds::fireSoundPlayerAkimbo},
			{14, &target::WeaponSFXPackageSounds::fireSoundPlayerAkimboRight},
			{30, &target::WeaponSFXPackageSounds::fireLastSoundPlayer},
			{31, &target::WeaponSFXPackageSounds::fireLastSoundPlayerAkimbo},
			{32, &target::WeaponSFXPackageSounds::fireLastSoundPlayerAkimboRight},
			{37, &target::WeaponSFXPackageSounds::emptyFireSound},
			{38, &target::WeaponSFXPackageSounds::emptyFireSoundPlayer},
			{61, &target::WeaponSFXPackageSounds::raiseSound},
			{62, &target::WeaponSFXPackageSounds::raiseSoundPlayer},
			{65, &target::WeaponSFXPackageSounds::putawaySound},
			{66, &target::WeaponSFXPackageSounds::putawaySoundPlayer},
			{68, &target::WeaponSFXPackageSounds::enterAdsSoundPlayer},
			{70, &target::WeaponSFXPackageSounds::leaveAdsSoundPlayer},
			{74, &target::WeaponSFXPackageSounds::projExplosionSound},
			{77, &target::WeaponSFXPackageSounds::projIgnitionSound},
			{78, &target::WeaponSFXPackageSounds::bounceSound},
		};

		const std::unordered_map<std::string, float>& iw7_volmods()
		{
			static const auto volmods = []
			{
				std::unordered_map<std::string, float> result;
				if (iw7_game_folder.empty())
				{
					ZONETOOL_WARNING("IW7 game folder is not set, volumes are not scaled to the IW7 volmods");
					return result;
				}

				auto previous_files = game_files::exchange(game_files::open_folder(iw7_game_folder / "zone"));
				const auto _ = gsl::finally([&previous_files] { game_files::set(std::move(previous_files)); });

				const auto path = game_files::find_zone("code_post_gfx");
				if (!path)
				{
					ZONETOOL_WARNING("IW7 zone \"code_post_gfx\" is missing, volumes are not scaled to the IW7 volmods");
					return result;
				}

				const auto database = db_create();
				db_scope scope(*database);
				const auto zone = target::load_zone(*path);
				for (const auto& asset : zone->assets)
				{
					if (asset.referenced || asset.type != target::ASSET_TYPE_SOUND_GLOBALS)
					{
						continue;
					}

					const auto globals = static_cast<const target::SndGlobals*>(asset.header);
					for (auto i = 0u; i < globals->volmodinfoCount; i++)
					{
						result.emplace(globals->volmodinfo[i].name, globals->volmodinfo[i].value);
					}
				}

				return result;
			}();

			return volmods;
		}

		template <typename T, std::size_t Size>
		int find_name(const std::string_view name, const T (&table)[Size])
		{
			const auto found = std::ranges::find(table, name);
			return found == std::end(table) ? -1 : static_cast<int>(found - std::begin(table));
		}

		template <typename Count, typename Entry>
		void add_names(name_table& table, const Count count, const Entry* entries)
		{
			for (Count i = 0; i < count; i++)
			{
				table.emplace(entries[i].id, entries[i].name);
			}
		}

		std::string resolve_name(const name_table& table, const unsigned int id)
		{
			if (id == snd_hash_name("default"))
			{
				return "default";
			}

			const auto found = table.find(id);
			return found == table.end() ? std::string{} : found->second;
		}

		unsigned char convert_table_index(const name_table& names, const unsigned int id, const std::string* table, const std::size_t count,
			const char* kind, const char* alias_name, const unsigned char fallback)
		{
			const auto name = resolve_name(names, id);
			if (!name.empty())
			{
				for (std::size_t i = 0; i < count; i++)
				{
					if (table[i] == name)
					{
						return static_cast<unsigned char>(i);
					}
				}
			}

			if (id != snd_hash_name("default"))
			{
				ZONETOOL_WARNING("%s \"%s\" of alias \"%s\" does not exist in IW7, using the fallback", kind,
					name.empty() ? std::format("{:#x}", id).data() : name.data(), alias_name);
			}

			return fallback;
		}

		int convert_volmod(const int index, const char* alias_name)
		{
			const auto name = index >= 0 && index < static_cast<int>(std::size(volmod_names)) ? volmod_names[index] : std::string_view("default");
			if (const auto found = find_name(name, target::VOLMODS); found >= 0)
			{
				return found;
			}

			ZONETOOL_WARNING("volmod \"%.*s\" of alias \"%s\" does not exist in IW7, using \"default\"", static_cast<int>(name.size()), name.data(), alias_name);
			return std::max(find_name("default", target::VOLMODS), 0);
		}

		unsigned int convert_channel(const unsigned int index)
		{
			const std::string name = index < std::size(entchannel_names) ? std::string(entchannel_names[index]) : "";
			if (const auto found = find_name(name, target::ENTCHANNELS); found >= 0)
			{
				return static_cast<unsigned int>(found);
			}

			std::vector<std::string> candidates;
			for (auto start = name.find('_'); start != std::string::npos; start = name.find('_', start + 1))
			{
				const auto end = name.find('_', start + 1);
				candidates.emplace_back(name.substr(0, start) + (end == std::string::npos ? "" : name.substr(end)));
			}

			for (auto end = name.rfind('_'); end != std::string::npos && end != 0; end = name.rfind('_', end - 1))
			{
				candidates.emplace_back(name.substr(0, end));
			}

			for (const auto& candidate : candidates)
			{
				if (const auto found = find_name(candidate, target::ENTCHANNELS); found >= 0)
				{
					ZONETOOL_WARNING("entchannel \"%s\" does not exist in IW7, using \"%s\"", name.data(), candidate.data());
					return static_cast<unsigned int>(found);
				}
			}

			ZONETOOL_WARNING("entchannel \"%s\" does not exist in IW7", name.data());
			return 0;
		}

		unsigned int convert_alias_type(const unsigned int type, const char* alias_name)
		{
			switch (type)
			{
			case SAT_HYBRID_PCM:
				ZONETOOL_WARNING("Sound alias \"%s\" is hybrid PCM, the PCM part is dropped", alias_name);
				return target::SAT_LOADED;
			case SAT_STREAMED:
				return target::SAT_STREAMED;
			case SAT_PRIMED:
				return target::SAT_PRIMED;
			default:
				return target::SAT_LOADED;
			}
		}

		std::optional<float> find_iw8_volmod(const SndGlobals* globals, const int index)
		{
			if (!globals || index < 0 || static_cast<unsigned int>(index) >= globals->volmodinfoCount)
			{
				return {};
			}

			return globals->volmodinfo[index].value;
		}

		target::SndAlias convert_alias(const SndAlias& alias, const converter_state& state)
		{
			const auto* name = alias.aliasName;
			const auto flags = std::bit_cast<SndAliasFlags>(alias.flags);

			const auto volmod = convert_volmod(alias.volModIndex, name);
			const auto source_volmod = find_iw8_volmod(state.globals, alias.volModIndex).value_or(1.0f);
			const auto target_volmod = [&]
			{
				const auto& values = iw7_volmods();
				const auto found = values.find(target::VOLMODS[volmod]);
				return found == values.end() ? 1.0f : found->second;
			}();
			const auto volume_scale = source_volmod / target_volmod;

			const auto default_index = [](const auto& table)
			{
				return static_cast<unsigned char>(std::max(find_name("default", table), 0));
			};

			target::SndAlias result{};
			result.aliasName = alias.aliasName;
			result.subtitle = alias.subtitle;
			result.secondaryAliasName = alias.secondaryAliasName;
			result.assetFileName = alias.assetFileName;
			result.contextType = 0;
			result.contextValue = 0;
			result.duck = alias.duck;
			result.volMin = alias.volMin * volume_scale;
			result.volMax = alias.volMax * volume_scale;
			result.volModIndex = volmod;
			result.pitchMin = alias.pitchMin;
			result.pitchMax = alias.pitchMax;
			result.donutFadeEnd = alias.donutFadeEnd;
			result.distMin = alias.distMin;
			result.distMax = alias.distMax;
			result.velocityMin = alias.velocityMin;
			result.flags.looping = flags.looping;
			result.flags.reverb = flags.reverb;
			result.flags.shape = flags.shape;
			result.flags.type = convert_alias_type(flags.type, name);
			result.flags.channel = convert_channel(flags.channel);
			result.masterPriority = alias.masterPriority;
			result.masterPercentage = alias.masterPercentage;
			result.slavePercentage = alias.masterSlavePercentage;
			result.probability = alias.probability;
			result.lfePercentage = alias.lfePercentage;
			result.centerPercentage = alias.centerPercentage;
			result.startDelay = alias.startDelay;
			result.volumeFalloffCurveIndex = convert_table_index(state.names.vf_curves, alias.volumeFalloffCurve, target::vf_curves_s,
				std::size(target::vf_curves_s), "volume falloff curve", name, default_index(target::vf_curves_s));
			result.lpfCurveIndex = convert_table_index(state.names.lpf_curves, alias.lpfCurve, target::lpf_curves_s,
				std::size(target::lpf_curves_s), "lpf curve", name, default_index(target::lpf_curves_s));
			result.hpfCurveIndex = convert_table_index(state.names.hpf_curves, alias.hpfCurve, target::hpf_curves_s,
				std::size(target::hpf_curves_s), "hpf curve", name, default_index(target::hpf_curves_s));
			result.reverbSendCurveIndex = convert_table_index(state.names.rvb_curves, alias.reverbSendCurve, target::rvb_curves_s,
				std::size(target::rvb_curves_s), "reverb send curve", name, default_index(target::rvb_curves_s));
			result.envelopMin = alias.envelopMin;
			result.envelopMax = alias.envelopMax;
			result.envelopPercentage = alias.envelopPercentage;
			result.speakerMapIndex = convert_table_index(state.names.speaker_maps, alias.speakerMap, target::speaker_maps_s,
				std::size(target::speaker_maps_s), "speaker map", name, default_index(target::speaker_maps_s));
			result.reverbMultiplier = alias.reverbMultiplier;
			result.farReverbMultiplier = alias.farReverbMultiplier;
			result.occlusionShapeIndex = convert_table_index(state.names.occlusion_shapes, alias.occlusionShape, target::occlusion_shapes_s,
				std::size(target::occlusion_shapes_s), "occlusion shape", name, default_index(target::occlusion_shapes_s));
			result.dopplerPresetIndex = convert_table_index(state.names.doppler_presets, alias.dopplerPreset, target::doppler_presets_s,
				std::size(target::doppler_presets_s), "doppler preset", name, 0xFF);
			result.smartPanDistance2d = alias.smartPanDistance2d;
			result.smartPanDistance3d = alias.smartPanDistance3d;
			result.smartPanAttenuation2d = alias.smartPanAttenuation2d;
			result.stereoSpreadMinDist = alias.stereoSpreadMinDist;
			result.stereoSpreadMaxDist = alias.stereoSpreadMaxDist;
			result.stereoSpreadMidPoint = alias.stereoSpreadMidPoint;
			result.stereoSpreadMaxAngle = alias.stereoSpreadMaxAngle;

			if (result.volMin > 1.0f || result.volMax > 1.0f)
			{
				ZONETOOL_WARNING("Sound alias \"%s\" volume %.3f-%.3f is above 1.0 after the IW7 volmod correction", name, result.volMin, result.volMax);
			}

			return result;
		}

		const SndGlobals* find_globals()
		{
			const SndGlobals* globals{};
			db_enum_assets(ASSET_TYPE_SOUND_GLOBALS, [&](void* header)
			{
				if (!globals)
				{
					globals = static_cast<const SndGlobals*>(header);
				}
			});

			return globals;
		}

		void index_bank(converter_state& state, SndBank* bank)
		{
			for (auto i = 0u; i < bank->aliasCount; i++)
			{
				if (bank->alias[i].aliasName)
				{
					state.aliases.try_emplace(bank->alias[i].aliasName, alias_source{bank, &bank->alias[i]});
				}
			}

			auto& names = state.names;
			add_names(names.vf_curves, bank->vfCurveCount, bank->vfCurves);
			add_names(names.lpf_curves, bank->lpfCurveCount, bank->lpfCurves);
			add_names(names.hpf_curves, bank->hpfCurveCount, bank->hpfCurves);
			add_names(names.rvb_curves, bank->rvbCurveCount, bank->rvbCurves);
			add_names(names.speaker_maps, bank->speakerMapCount, bank->speakerMaps);
			add_names(names.occlusion_shapes, bank->occlusionShapeCount, bank->occlusionShapes);
			add_names(names.doppler_presets, bank->dopplerPresetCount, bank->dopplerPresets);
			for (auto i = 0u; i < bank->duckCount; i++)
			{
				names.ducks.try_emplace(bank->ducks[i].id, &bank->ducks[i]);
			}
		}

		converter_state build_state()
		{
			converter_state state{};
			state.globals = find_globals();
			db_enum_assets(ASSET_TYPE_SOUND_BANK, [&](void* header)
			{
				index_bank(state, &static_cast<SndBankResident*>(header)->bank);
			});
			db_enum_assets(ASSET_TYPE_SOUND_BANK_TRANSIENT, [&](void* header)
			{
				index_bank(state, &static_cast<SndBankTransient*>(header)->bank);
			});

			return state;
		}

		std::optional<sab_source> open_sab(std::function<bool(std::uint64_t, std::size_t, void*)> read, const char* label)
		{
			SndAssetBankHeader header{};
			if (!read(0, sizeof(header), &header) || std::memcmp(&header.magic, "2UX#", 4))
			{
				ZONETOOL_WARNING("Sound asset bank \"%s\" has no valid header", label);
				return {};
			}

			sab_source source{};
			source.entries.resize(header.entryCount);
			if (!header.entryCount || !read(header.entryOffset, source.entries.size() * sizeof(SndAssetBankEntry), source.entries.data()))
			{
				ZONETOOL_WARNING("Sound asset bank \"%s\" has no entries (count %u)", label, header.entryCount);
				return {};
			}

			source.read = std::move(read);
			return source;
		}

		std::optional<std::string> find_streamed_file(const SndBank* bank)
		{
			const auto zone = bank->zone ? std::string(bank->zone) : std::string{};
			const auto bank_name = std::string(bank->name);
			const auto stem = bank_name.substr(0, bank_name.rfind('.'));
			const auto files = game_files::get().list(".sabs");
			for (const auto* wanted : {&zone, &stem})
			{
				for (const auto& file : files)
				{
					if (!wanted->empty() && std::filesystem::path(file).filename() == *wanted + ".sabs")
					{
						return file;
					}
				}
			}

			return {};
		}

		bank_audio& open_audio(converter_state& state, SndBank* bank)
		{
			auto& audio = state.audio[bank];
			if (audio.opened)
			{
				return audio;
			}

			audio.opened = true;

			if (const auto* key = bank->streamInfo.loadedStreamKey; key && key->dataSize)
			{
				if (key->flags & STREAM_KEY_RESIDENT)
				{
					audio.loaded_data.assign(key->data.residentData, key->dataSize);
				}
				else if (const auto data = xpak::read(key->xpakInfo.key, key->dataSize))
				{
					audio.loaded_data = *data;
				}
				else
				{
					ZONETOOL_WARNING("Stream key \"%s\" of bank \"%s\" has no data", key->name, bank->name);
				}

				if (!audio.loaded_data.empty())
				{
					audio.loaded = open_sab([&audio](const std::uint64_t offset, const std::size_t size, void* buffer)
					{
						if (offset + size > audio.loaded_data.size())
						{
							return false;
						}

						std::memcpy(buffer, audio.loaded_data.data() + offset, size);
						return true;
					}, bank->name);
				}
			}

			if (const auto file = find_streamed_file(bank))
			{
				ZONETOOL_INFO("Bank \"%s\" streams from \"%s\"", bank->name, file->data());
				audio.streamed = open_sab([name = *file](const std::uint64_t offset, const std::size_t size, void* buffer)
				{
					return game_files::get().read(name, offset, size, buffer);
				}, file->data());
			}

			return audio;
		}

		bool export_audio(converter_state& state, const alias_source& source, const SndAlias& alias)
		{
			auto& audio = open_audio(state, source.bank);
			const auto flags = std::bit_cast<SndAliasFlags>(alias.flags);
			const auto wants_stream = flags.type == SAT_STREAMED || flags.type == SAT_PRIMED;

			const auto find_entry = [&](std::optional<sab_source>& bank) -> const SndAssetBankEntry*
			{
				if (!bank)
				{
					return nullptr;
				}

				const auto found = std::ranges::find(bank->entries, alias.assetId, &SndAssetBankEntry::id);
				return found == bank->entries.end() ? nullptr : &*found;
			};

			auto* bank = &audio.loaded;
			const auto* entry = wants_stream ? find_entry(audio.streamed) : nullptr;
			if (entry)
			{
				bank = &audio.streamed;
			}
			else
			{
				entry = find_entry(audio.loaded);
				if (entry && wants_stream)
				{
					ZONETOOL_WARNING("Sound asset \"%s\" is streamed, only the loaded part is converted", alias.assetFileName);
				}
			}

			if (!entry)
			{
				ZONETOOL_WARNING("Sound asset \"%s\" of alias \"%s\" is not in the bank \"%s\"", alias.assetFileName, alias.aliasName, source.bank->name);
				return false;
			}

			if (entry->format != target::SND_ASSET_FORMAT_FLAC)
			{
				ZONETOOL_WARNING("Sound asset \"%s\" has format %d, only flac is converted", alias.assetFileName, static_cast<int>(entry->format));
				return false;
			}

			if (entry->hybridPcmSize)
			{
				ZONETOOL_WARNING("Sound asset \"%s\" has %u bytes of hybrid PCM, they are dropped", alias.assetFileName, entry->hybridPcmSize);
			}

			const auto size = static_cast<std::size_t>(entry->size) + entry->seekTableSize;
			std::vector<std::uint8_t> data(size);
			if (!(*bank)->read(entry->offset, size, data.data()))
			{
				ZONETOOL_WARNING("Sound asset \"%s\" could not be read", alias.assetFileName);
				return false;
			}

			static_assert(sizeof(target::SndAssetBankEntry) == sizeof(SndAssetBankEntry));
			auto target_entry = std::bit_cast<target::SndAssetBankEntry>(*entry);
			target::sound_asset_bank::flac::dump(target_entry, alias.assetFileName, data.data(), data.size());
			return true;
		}

		const char* convert_reflection_class(const char* name, utils::memory::allocator& allocator)
		{
			if (!name || !*name)
			{
				return nullptr;
			}

			return allocator.duplicate_string(std::format("eng_{}", name));
		}

		void write_bank_json(const target::SndBank& bank)
		{
			auto file = filesystem::file("soundbank\\"s + bank.name + ".json"s);
			file.open("wb");

			ordered_json data;
			data["name"] = bank.name;
			data["zone"] = bank.zone;
			data["gameLanguage"] = bank.gameLanguage;
			data["soundLanguage"] = bank.soundLanguage;
			data["aliases"][0] = target::aliases::get_path(const_cast<target::SndBank*>(&bank));

			file.write(data.dump(4));
			file.close();
		}
	}

	void set_iw7_folder(const std::filesystem::path& folder)
	{
		iw7_game_folder = folder;
	}

	void dump(WeaponCompleteDef* asset, const std::vector<std::string>& notetrack_aliases)
	{
		auto* package = asset->weapDef->sfxPackage;
		if (package && package->name)
		{
			if (const auto loaded = static_cast<WeaponSFXPackage*>(db_find_asset(ASSET_TYPE_SFX_PACKAGE, package->name)))
			{
				package = loaded;
			}
		}

		if (!package || !package->name || !mark_dumped(ASSET_TYPE_SFX_PACKAGE, package->name))
		{
			return;
		}

		auto state = build_state();
		if (!state.globals)
		{
			ZONETOOL_WARNING("IW8 sound globals are not loaded, volmods are assumed to be 1.0");
		}

		std::vector<std::string> names;
		const auto add_name = [&](const char* name)
		{
			if (name && *name && !std::ranges::contains(names, name))
			{
				names.emplace_back(name);
			}
		};

		std::size_t mapped_slots = 0;
		std::size_t unmapped_slots = 0;
		if (package->sounds)
		{
			for (std::size_t i = 0; i < std::size(package->sounds->sounds); i++)
			{
				const auto* name = package->sounds->sounds[i].name;
				if (!name || !*name)
				{
					continue;
				}

				if (std::ranges::any_of(sound_slots, [&](const auto& slot) { return slot.first == i; }))
				{
					mapped_slots++;
					add_name(name);
				}
				else
				{
					unmapped_slots++;
					ZONETOOL_WARNING("Slot %zu \"%s\" of sfxpkg \"%s\" has no IW7 slot", i, name, package->name);
				}
			}
		}

		for (const auto& name : notetrack_aliases)
		{
			add_name(name.data());
		}

		utils::memory::allocator allocator;
		std::vector<target::SndAliasList> lists;
		std::vector<target::SndDuck> ducks;
		std::unordered_map<std::string, bool> exported_files;
		std::size_t alias_count = 0;
		std::size_t file_count = 0;
		for (std::size_t i = 0; i < names.size(); i++)
		{
			const auto name = names[i];
			const auto found = state.aliases.find(name);
			if (found == state.aliases.end())
			{
				ZONETOOL_WARNING("Sound alias \"%s\" is not loaded", name.data());
				continue;
			}

			const auto& source = found->second;
			target::SndAliasList result{};
			result.aliasName = source.list->aliasName;
			result.head = allocator.allocate_array<target::SndAlias>(source.list->count);
			for (auto j = 0; j < source.list->count; j++)
			{
				const auto& alias = source.list->head[j];
				if (alias.assetFileName)
				{
					auto [exported, inserted] = exported_files.try_emplace(alias.assetFileName, false);
					if (inserted)
					{
						exported->second = export_audio(state, source, alias);
						file_count += exported->second;
					}

					if (!exported->second)
					{
						continue;
					}
				}

				result.head[result.count++] = convert_alias(alias, state);
				alias_count++;
				add_name(alias.secondaryAliasName);

				if (alias.duck && std::ranges::none_of(ducks, [&](const auto& duck) { return duck.id == alias.duck; }))
				{
					if (const auto duck = state.names.ducks.find(alias.duck); duck != state.names.ducks.end())
					{
						target::SndDuck entry{};
						std::memcpy(entry.name, duck->second->name, sizeof(entry.name));
						entry.id = duck->second->id;
						entry.fadeIn = duck->second->fadeIn;
						entry.fadeOut = duck->second->fadeOut;
						entry.startDelay = duck->second->startDelay;
						entry.minDistance = duck->second->minDistance;
						entry.distance = duck->second->distance;
						entry.length = duck->second->length;
						entry.fadeInCurve = duck->second->fadeInCurve;
						entry.fadeOutCurve = duck->second->fadeOutCurve;
						entry.attenuation = duck->second->attenuation;
						entry.lpf = duck->second->lpf;
						entry.updateWhilePaused = duck->second->updateWhilePaused;
						entry.trackAmplitude = duck->second->trackAmplitude;
						entry.disableInSplitscreen = duck->second->disableInSplitscreen;
						entry.aliasAttenuation = duck->second->aliasAttenuation;
						entry.aliasLpf = duck->second->aliasLpf;
						entry.duckAlias = duck->second->duckAlias;
						ducks.emplace_back(entry);
					}
				}
			}

			if (result.count)
			{
				lists.emplace_back(result);
			}
		}

		const auto bank = allocator.allocate<target::SndBankResident>();
		bank->bank.name = allocator.duplicate_string(std::format("{}.all", package->name));
		bank->bank.zone = package->name;
		bank->bank.gameLanguage = "all";
		bank->bank.soundLanguage = "all";
		bank->bank.aliasCount = static_cast<unsigned int>(lists.size());
		bank->bank.alias = allocator.allocate_array<target::SndAliasList>(lists.size());
		std::ranges::copy(lists, bank->bank.alias);
		bank->bank.duckCount = static_cast<unsigned int>(ducks.size());
		bank->bank.ducks = allocator.allocate_array<target::SndDuck>(ducks.size());
		std::ranges::copy(ducks, bank->bank.ducks);

		const auto converted = [&](const char* name)
		{
			return name && std::ranges::any_of(lists, [&](const auto& list) { return !std::strcmp(list.aliasName, name); });
		};

		const auto result = allocator.allocate<target::WeaponSFXPackage>();
		result->name = package->name;
		result->sounds = allocator.allocate<target::WeaponSFXPackageSounds>();
		result->sounds->name = package->sounds && package->sounds->name ? package->sounds->name : package->name;
		if (package->sounds)
		{
			for (const auto& [slot, member] : sound_slots)
			{
				if (converted(package->sounds->sounds[slot].name))
				{
					result->sounds->*member = package->sounds->sounds[slot].name;
				}
			}
		}

		result->rattleSoundType = static_cast<char>(package->rattleSoundType);
		result->szWeapSndReflectionClass = convert_reflection_class(package->szWeapSndReflectionClass, allocator);
		result->weapSndFireVolumeShot1 = package->weapSndFireVolumeShot1;
		result->weapSndFireVolumeShot2 = package->weapSndFireVolumeShot2;
		result->weapSndFireVolumeShot3 = package->weapSndFireVolumeShot3;
		result->weapSndProneFireLFEVolume = package->weapSndProneFireLFEVolume;

		target::aliases::dump(&bank->bank);
		target::ducks::dump(&bank->bank);
		write_bank_json(bank->bank);
		target::weapon_sfx_package::dump(result);
		add_csv_row("soundbank", bank->bank.name);
		add_csv_row("sfxpkg", result->name);

		ZONETOOL_INFO("Converted sfxpkg \"%s\": %zu of 96 slots mapped (%zu without an IW7 slot), %zu alias lists, %zu aliases, %zu audio files", package->name,
			mapped_slots, unmapped_slots, lists.size(), alias_count, file_count);
	}
}
