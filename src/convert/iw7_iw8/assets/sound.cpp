#include <std_include.hpp>

#include "../include.hpp"
#include "sound.hpp"

#include <iw7/assets/sound_bank.hpp>
#include <iw7/assets/sound_globals.hpp>
#include <iw8/assets/sound.hpp>
#include <iw8/assets/streamkey.hpp>

namespace zonetool::iw7::converter::iw8::sound
{
	namespace
	{
		namespace target = zonetool::iw8;

		const std::pair<SndAliasLookup WeaponSFXPackageSounds::*, std::size_t> sound_slots[] =
		{
			{&WeaponSFXPackageSounds::pickupSound, 0},
			{&WeaponSFXPackageSounds::pickupSoundPlayer, 1},
			{&WeaponSFXPackageSounds::ammoPickupSound, 2},
			{&WeaponSFXPackageSounds::projectileSound, 4},
			{&WeaponSFXPackageSounds::pullbackSound, 5},
			{&WeaponSFXPackageSounds::pullbackSoundPlayer, 6},
			{&WeaponSFXPackageSounds::pullbackSoundQuick, 7},
			{&WeaponSFXPackageSounds::pullbackSoundQuickPlayer, 8},
			{&WeaponSFXPackageSounds::fireSound, 11},
			{&WeaponSFXPackageSounds::fireSoundPlayer, 12},
			{&WeaponSFXPackageSounds::fireSoundPlayerAkimbo, 13},
			{&WeaponSFXPackageSounds::fireSoundPlayerAkimboRight, 14},
			{&WeaponSFXPackageSounds::fireLastSoundPlayer, 30},
			{&WeaponSFXPackageSounds::fireLastSoundPlayerAkimbo, 31},
			{&WeaponSFXPackageSounds::fireLastSoundPlayerAkimboRight, 32},
			{&WeaponSFXPackageSounds::emptyFireSound, 37},
			{&WeaponSFXPackageSounds::emptyFireSoundPlayer, 38},
			{&WeaponSFXPackageSounds::raiseSound, 61},
			{&WeaponSFXPackageSounds::raiseSoundPlayer, 62},
			{&WeaponSFXPackageSounds::putawaySound, 65},
			{&WeaponSFXPackageSounds::putawaySoundPlayer, 66},
			{&WeaponSFXPackageSounds::enterAdsSoundPlayer, 68},
			{&WeaponSFXPackageSounds::leaveAdsSoundPlayer, 70},
			{&WeaponSFXPackageSounds::projExplosionSound, 74},
			{&WeaponSFXPackageSounds::projIgnitionSound, 77},
			{&WeaponSFXPackageSounds::bounceSound, 78},
		};

		std::uint64_t align_up(const std::uint64_t value, const std::uint64_t alignment)
		{
			return (value + alignment - 1) & ~(alignment - 1);
		}

		const char* convert_reflection_class(const char* name)
		{
			if (!name)
			{
				return nullptr;
			}

			const std::string_view value = name;
			return value.starts_with("eng_") ? name + 4 : name;
		}

		target::SndAliasLoadType convert_load_type(const unsigned int type)
		{
			switch (type)
			{
			case SAT_HYBRID_PCM:
				return target::SAT_HYBRID_PCM;
			case SAT_STREAMED:
				return target::SAT_STREAMED;
			case SAT_PRIMED:
				return target::SAT_PRIMED;
			default:
				return target::SAT_LOADED;
			}
		}

		int convert_flags(const SndAliasFlags& flags)
		{
			target::SndAliasFlags result{};
			result.looping = flags.looping;
			result.reverb = flags.reverb;
			result.shape = flags.shape;
			result.type = convert_load_type(flags.type);
			result.channel = convert_channel(flags.channel);
			return std::bit_cast<int>(result);
		}

		short get_duration_dsec(const target::SndAssetBankEntry* entry)
		{
			if (!entry || entry->frameRate <= 0)
			{
				return 0;
			}

			const auto duration = (static_cast<std::uint64_t>(entry->frameCount) * 10 + entry->frameRate - 1) / entry->frameRate;
			return static_cast<short>(std::min<std::uint64_t>(duration, std::numeric_limits<short>::max()));
		}

		float volmod_value(const SndGlobals* globals, const int index)
		{
			if (!globals || index < 0 || static_cast<unsigned int>(index) >= globals->volmodinfoCount)
			{
				return 1.0f;
			}

			return globals->volmodinfo[index].value;
		}

		SndAliasList* find_alias_list(const std::string& name, SndBank*& owner)
		{
			SndAliasList* result{};
			db_enum_assets(ASSET_TYPE_SOUND_BANK, [&](void* header)
			{
				const auto bank = static_cast<SndBank*>(header);
				for (auto i = 0u; !result && i < bank->aliasCount; i++)
				{
					if (bank->alias[i].aliasName && name == bank->alias[i].aliasName)
					{
						result = &bank->alias[i];
						owner = bank;
					}
				}
			});

			return result;
		}

		utils::memory::allocator weapon_allocator;
		asset_bank_builder weapon_audio;
		std::vector<target::SndAliasList> weapon_lists;

		bool has_weapon_alias(const char* name)
		{
			return name && std::ranges::any_of(weapon_lists, [&](const auto& list) { return !std::strcmp(list.aliasName, name); });
		}
	}

	int convert_volmod(const int index)
	{
		const std::string name = index >= 0 && index < static_cast<int>(std::size(VOLMODS)) ? VOLMODS[index] : "default";
		if (const auto found = std::ranges::find(target::volmod_names, name); found != std::end(target::volmod_names))
		{
			return static_cast<int>(found - std::begin(target::volmod_names));
		}

		ZONETOOL_WARNING("volmod \"%s\" does not exist in IW8", std::string(name).data());
		return static_cast<int>(std::ranges::find(target::volmod_names, "default") - std::begin(target::volmod_names));
	}

	unsigned int convert_channel(const unsigned int index)
	{
		const std::string name = index < std::size(ENTCHANNELS) ? ENTCHANNELS[index] : "";
		const auto find_channel = [](const std::string_view search)
		{
			return std::ranges::find(target::entchannel_names, search);
		};

		if (const auto found = find_channel(name); found != std::end(target::entchannel_names))
		{
			return static_cast<unsigned int>(found - std::begin(target::entchannel_names));
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
			if (const auto found = find_channel(candidate); found != std::end(target::entchannel_names))
			{
				ZONETOOL_WARNING("entchannel \"%s\" does not exist in IW8, using \"%s\"", name.data(), candidate.data());
				return static_cast<unsigned int>(found - std::begin(target::entchannel_names));
			}
		}

		ZONETOOL_WARNING("entchannel \"%s\" does not exist in IW8", name.data());
		return 0;
	}

	bool is_looping(const std::string& alias_name)
	{
		SndBank* bank{};
		const auto* list = find_alias_list(alias_name, bank);
		return list && list->count && list->head[0].flags.looping;
	}

	target::SndAlias convert_alias(const SndAlias& alias, const target::SndAssetBankEntry* entry, const SndGlobals* globals)
	{
		const auto default_hash = snd_hash_name("default");

		target::SndAlias result{};
		result.aliasName = alias.aliasName;
		result.subtitle = alias.subtitle;
		result.secondaryAliasName = alias.secondaryAliasName;
		result.assetFileName = alias.assetFileName;
		result.id = alias.id;
		result.secondaryId = alias.secondaryId;
		result.assetId = alias.assetId;
		result.contextType = alias.contextType;
		result.contextValue = alias.contextValue;
		result.sequence = alias.sequence;
		result.volMin = alias.volMin * volmod_value(globals, alias.volModIndex); // iw8 base volmods are 1.0
		result.volMax = alias.volMax * volmod_value(globals, alias.volModIndex);
		result.volModIndex = convert_volmod(alias.volModIndex);
		result.pitchMin = alias.pitchMin;
		result.pitchMax = alias.pitchMax;
		result.donutFadeEnd = alias.donutFadeEnd;
		result.distMin = alias.distMin;
		result.distMax = alias.distMax;
		result.velocityMin = alias.velocityMin;
		result.flags = convert_flags(alias.flags);
		result.probability = alias.probability;
		result.lfePercentage = alias.lfePercentage;
		result.centerPercentage = alias.centerPercentage;
		result.startDelay = alias.startDelay;
		result.startDelayMax = alias.startDelay;
		result.volumeFalloffCurve = default_hash; // TODO: map IW7 curve names
		result.lpfCurve = default_hash;
		result.hpfCurve = default_hash;
		result.reverbSendCurve = default_hash;
		result.envelopMin = alias.envelopMin;
		result.envelopMax = alias.envelopMax;
		result.envelopPercentage = alias.envelopPercentage;
		result.speakerMap = default_hash;
		result.reverbMultiplier = alias.reverbMultiplier;
		result.farReverbMultiplier = alias.farReverbMultiplier;
		result.occlusionShape = default_hash;
		result.dopplerPreset = default_hash;
		result.smartPanDistance2d = alias.smartPanDistance2d;
		result.smartPanDistance3d = alias.smartPanDistance3d;
		result.smartPanAttenuation2d = alias.smartPanAttenuation2d;
		result.stereoSpreadMinDist = alias.stereoSpreadMinDist;
		result.stereoSpreadMaxDist = alias.stereoSpreadMaxDist;
		result.stereoSpreadMidPoint = alias.stereoSpreadMidPoint;
		result.stereoSpreadMaxAngle = static_cast<char>(alias.stereoSpreadMaxAngle);
		result.masterPriority = static_cast<char>(alias.masterPriority);
		result.masterPercentage = alias.masterPercentage;
		result.masterSlavePercentage = alias.slavePercentage;
		result.masterThreshold = 0.25118863f;
		result.maxDuration_dsec = get_duration_dsec(entry);
		return result;
	}

	target::SndIndexEntry* build_alias_index(const target::SndAliasList* lists, const unsigned short count, utils::memory::allocator& allocator)
	{
		const auto empty = std::numeric_limits<unsigned short>::max();
		const auto index = allocator.allocate_array<target::SndIndexEntry>(count);
		std::memset(index, 0xFF, sizeof(target::SndIndexEntry) * count);

		std::vector<bool> placed(count);
		for (unsigned short i = 0; i < count; i++)
		{
			const auto slot = lists[i].id % count;
			if (index[slot].value == empty)
			{
				index[slot].value = i;
				placed[i] = true;
			}
		}

		for (unsigned short i = 0; i < count; i++)
		{
			if (placed[i])
			{
				continue;
			}

			auto last = static_cast<unsigned short>(lists[i].id % count);
			while (index[last].next != empty)
			{
				last = index[last].next;
			}

			auto free_slot = empty;
			for (unsigned short offset = 1; offset < count && free_slot == empty; offset++)
			{
				if (const auto after = static_cast<unsigned short>((last + offset) % count); index[after].value == empty)
				{
					free_slot = after;
				}
				else if (const auto before = static_cast<unsigned short>((last + count - offset) % count); index[before].value == empty)
				{
					free_slot = before;
				}
			}

			if (free_slot == empty)
			{
				ZONETOOL_FATAL("Unable to allocate sound bank alias index list");
			}

			index[last].next = free_slot;
			index[free_slot].value = i;
			placed[i] = true;
		}

		return index;
	}

	asset_bank_builder::source_bank* asset_bank_builder::open(const std::string& path)
	{
		if (const auto found = this->sources_.find(path); found != this->sources_.end())
		{
			return found->second.get();
		}

		auto& source = this->sources_[path];
		source = std::make_unique<source_bank>();
		source->file.open(path, std::ios::binary);

		SndAssetBankHeader header{};
		if (!source->file.read(reinterpret_cast<char*>(&header), sizeof(header)) || std::memcmp(&header.magic, "2UX#", 4))
		{
			ZONETOOL_WARNING("Sound asset bank \"%s\" could not be read", path.data());
			return source.get();
		}

		const auto read_table = [&](auto& table, const std::uint64_t offset)
		{
			table.resize(header.entryCount);
			if (offset)
			{
				source->file.seekg(offset);
				source->file.read(reinterpret_cast<char*>(table.data()), table.size() * sizeof(table[0]));
			}
		};

		read_table(source->entries, header.entryOffset);
		read_table(source->checksums, header.checksumOffset);
		read_table(source->source_checksums, header.SourceChecksumOffset);
		read_table(source->names, header.AssetNameOffset);
		return source.get();
	}

	const target::SndAssetBankEntry* asset_bank_builder::add(SndBank* bank, const SndStringHash asset_id)
	{
		return this->add(sound_asset_bank::create_path(bank, false), asset_id);
	}

	const target::SndAssetBankEntry* asset_bank_builder::add(const std::string& path, const SndStringHash asset_id)
	{
		if (const auto found = std::ranges::find(this->entries_, asset_id, &target::SndAssetBankEntry::id); found != this->entries_.end())
		{
			return &*found;
		}

		const auto source = this->open(path);
		const auto found = std::ranges::find(source->entries, asset_id, &SndAssetBankEntry::id);
		if (found == source->entries.end())
		{
			return nullptr;
		}

		const auto index = found - source->entries.begin();
		const auto size = static_cast<std::size_t>(found->size) + found->seekTableSize + found->hybridPcmSize;

		static_assert(sizeof(target::SndAssetBankEntry) == sizeof(SndAssetBankEntry));
		auto& entry = this->entries_.emplace_back(std::bit_cast<target::SndAssetBankEntry>(*found));
		entry.offset = 0x1000 + this->data_.size();

		const auto position = this->data_.size();
		this->data_.resize(position + size);
		source->file.clear();
		source->file.seekg(found->offset);
		source->file.read(this->data_.data() + position, size);

		this->checksums_.emplace_back(source->checksums[index]);
		this->source_checksums_.emplace_back(source->source_checksums[index]);
		this->names_.emplace_back(source->names[index]);
		return &entry;
	}

	std::string asset_bank_builder::build(const std::string& zone_name) const
	{
		target::SndAssetBankHeader header{};
		std::memcpy(&header.magic, "2UX#", 4);
		header.version = 10;
		header.entrySize = sizeof(target::SndAssetBankEntry);
		header.checksumSize = 16;
		header.dependencySize = 64;
		header.entryCount = static_cast<unsigned int>(this->entries_.size());
		header.dependencyCount = 8;
		header.buildVersion = 16;
		std::memset(header.checksumChecksum, 0xCC, sizeof(header.checksumChecksum));
		zone_name.copy(header.zoneName, sizeof(header.zoneName) - 1);
		std::memcpy(header.platform, "pc_dx12", 7);
		std::memcpy(header.language, "all", 3);
		header.convertedAssetVersion = 10;
		header.assetSectionSize = static_cast<unsigned int>(this->data_.size());

		std::string result(0x1000, '\0');
		result.append(this->data_);

		const auto append_section = [&](const auto& table)
		{
			result.resize(align_up(result.size(), 0x1000));
			const auto offset = result.size();
			result.append(reinterpret_cast<const char*>(table.data()), table.size() * sizeof(table[0]));
			return offset;
		};

		header.entryOffset = append_section(this->entries_);
		header.checksumOffset = append_section(this->checksums_);
		header.SourceChecksumOffset = append_section(this->source_checksums_);
		header.AssetNameOffset = append_section(this->names_);
		header.fileSize = result.size();

		std::memcpy(result.data(), &header, sizeof(header));
		return result;
	}

	void dump(WeaponCompleteDef* asset)
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

		std::vector<std::string> names;
		const auto add_name = [&](const char* name)
		{
			if (name && *name && !std::ranges::contains(names, name))
			{
				names.emplace_back(name);
			}
		};

		if (package->sounds)
		{
			for (const auto& [source, slot] : sound_slots)
			{
				add_name(package->sounds->*source);
			}
		}

		for (auto i = 0u; i < asset->numNotetrackSoundMappings; i++)
		{
			add_name(SL_ConvertToString(asset->notetrackSoundMapValues[i]));
		}

		const auto globals = static_cast<SndGlobals*>(db_find_asset(ASSET_TYPE_SOUND_GLOBALS, "singleton"));
		if (!globals)
		{
			ZONETOOL_WARNING("Sound globals are not loaded, volumes of \"%s\" are not scaled", package->name);
		}

		auto converted_count = 0u;
		for (std::size_t i = 0; i < names.size(); i++)
		{
			const auto name = names[i];
			if (has_weapon_alias(name.data()))
			{
				continue;
			}

			SndBank* bank{};
			const auto* list = find_alias_list(name, bank);
			if (!list)
			{
				ZONETOOL_WARNING("Sound alias \"%s\" is not loaded", name.data());
				continue;
			}

			target::SndAliasList result{};
			result.aliasName = list->aliasName;
			result.id = list->id;
			result.sequence = list->sequence;
			result.head = weapon_allocator.allocate_array<target::SndAlias>(list->count);
			for (auto j = 0; j < list->count; j++)
			{
				const auto& alias = list->head[j];
				if (alias.flags.type == SAT_STREAMED || alias.flags.type == SAT_PRIMED)
				{
					ZONETOOL_WARNING("Sound alias \"%s\" is streamed and is not converted", name.data());
					continue;
				}

				const auto* entry = alias.assetFileName ? weapon_audio.add(bank, alias.assetId) : nullptr;
				if (alias.assetFileName && !entry)
				{
					ZONETOOL_WARNING("Sound asset \"%s\" of alias \"%s\" is not in the loaded bank", alias.assetFileName, name.data());
					continue;
				}

				result.head[result.count++] = convert_alias(alias, entry, globals);
				add_name(alias.secondaryAliasName);
			}

			if (result.count)
			{
				weapon_lists.emplace_back(result);
				converted_count++;
			}
		}

		utils::memory::allocator allocator;
		const auto result = allocator.allocate<target::WeaponSFXPackage>();
		result->name = package->name;
		result->sounds = allocator.allocate<target::WeaponSFXPackageSounds>();
		result->sounds->name = package->sounds && package->sounds->name ? package->sounds->name : package->name;
		if (package->sounds)
		{
			for (const auto& [source, slot] : sound_slots)
			{
				if (has_weapon_alias(package->sounds->*source))
				{
					result->sounds->sounds[slot].name = package->sounds->*source;
				}
			}
		}

		result->rattleSoundType = static_cast<unsigned char>(package->rattleSoundType);
		result->szWeapSndReflectionClass = convert_reflection_class(package->szWeapSndReflectionClass);
		result->weapSndFireVolumeShot1 = package->weapSndFireVolumeShot1;
		result->weapSndFireVolumeShot2 = package->weapSndFireVolumeShot2;
		result->weapSndFireVolumeShot3 = package->weapSndFireVolumeShot3;
		result->weapSndProneFireLFEVolume = package->weapSndProneFireLFEVolume;
		result->weapSndMediumRangeStart = 1000.0f;
		result->weapSndFarRangeStart = 2000.0f;
		result->weapSndDistantRangeStart = 3000.0f;
		result->sfxImpactType = 1;
		result->sfxMeleeImpactType = target::MELEE_IMPACT_TYPE_GUN_SMALL;

		target::sfx_package::dump(result);
		add_csv_row("sfxpkg", result->name);

		ZONETOOL_INFO("Converted sfxpkg \"%s\": %u new aliases", package->name, converted_count);
	}

	void dump_weapon_bank(const std::string& zone_name)
	{
		if (weapon_lists.empty())
		{
			return;
		}

		const auto bank_zone = zone_name + "_weapons";
		const auto data = weapon_audio.build(bank_zone);

		utils::memory::allocator allocator;
		const auto key = allocator.allocate<target::StreamKey>();
		key->name = allocator.duplicate_string(std::format("{}_all_loaded", bank_zone));
		key->data.residentData = const_cast<char*>(data.data());
		key->dataSize = static_cast<unsigned int>(data.size());
		key->behaviorIndex = 2;
		key->flags = 0x3;

		const auto bank = allocator.allocate<target::SndBankResident>();
		bank->bank.name = allocator.duplicate_string(std::format("{}.all", bank_zone));
		bank->bank.zone = bank_zone.data();
		bank->bank.gameLanguage = "all";
		bank->bank.soundLanguage = "all";
		bank->bank.aliasCount = static_cast<unsigned int>(weapon_lists.size());
		bank->bank.alias = weapon_lists.data();
		bank->bank.aliasIndex = build_alias_index(bank->bank.alias, static_cast<unsigned short>(weapon_lists.size()), allocator);
		bank->bank.streamInfo.loadedStreamKey = key;
		bank->bank.streamInfo.alwaysLoaded = true;

		auto streamed = filesystem::file("soundbank\\"s + bank_zone + ".sabs");
		streamed.open("wb");
		streamed.write(asset_bank_builder{}.build(bank_zone));
		streamed.close();

		target::stream_key::dump(key);
		target::sound_bank::dump(bank);
		add_csv_row("soundbank", bank->bank.name);

		ZONETOOL_INFO("Converted weapon soundbank \"%s\": %zu aliases, %zu bytes of audio", bank->bank.name, weapon_lists.size(), data.size());

		weapon_lists.clear();
		weapon_audio = {};
	}
}
