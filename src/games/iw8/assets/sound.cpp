#include <std_include.hpp>
#include "sound.hpp"

namespace zonetool::iw8
{
	namespace
	{
		constexpr std::array sfx_package_strings
		{
			&WeaponSFXPackage::szAdsrBaseSetting, &WeaponSFXPackage::szWeapSndReflectionClass,
			&WeaponSFXPackage::szWeapSndReflFrontOverride, &WeaponSFXPackage::szWeapSndReflDistantOverride,
			&WeaponSFXPackage::szMountEnterExitString, &WeaponSFXPackage::szMountImpactString,
		};

		void dump_sound_bank(assetmanager::dumper& dump, const SndBankTransient* bank)
		{
			dump.dump_string(bank ? bank->bank.name : nullptr);
		}

		void parse_sound_bank(assetmanager::reader& read, zone_memory* mem, SndBankTransient*& bank)
		{
			const auto* const name = read.read_string();
			if (!name)
			{
				bank = nullptr;
				return;
			}

			bank = mem->allocate<SndBankTransient>();
			bank->bank.name = name;
		}
	}

	namespace
	{
		template <typename Walk, typename T>
		T* walk_named_array(Walk walk, T*& field, const std::size_t count, const std::uint32_t alignment)
		{
			const auto entries = walk.array(field, count, alignment);
			if (entries)
			{
				for (std::size_t i = 0; i < count; i++)
				{
					walk.string(entries[i].name);
				}
			}

			return entries;
		}

		template <typename Walk>
		void walk_adsr_zones(Walk walk, AdsrZoneEntry*& field, const std::size_t count)
		{
			if (const auto zones = walk_named_array(walk, field, count, 7))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					walk.string(zones[i].weaponName);
				}
			}
		}

		template <typename Walk>
		void walk_zone_def(Walk walk, ZoneDef& zone)
		{
			walk.string(zone.ambientStream);
			walk.string(zone.zoneName);
			walk.string(zone.stateName);
			walk.string(zone.ambientName);
			walk.string(zone.mixName);
			walk.string(zone.reverbName);
			walk.string(zone.filterName);
			walk.string(zone.occlusionName);
			walk.string(zone.plrAdsrName);
			walk.string(zone.npcAdsrName);
			walk.string(zone.weapReflName);
			walk.string(zone.fullOccName);
			walk.string(zone.playerBreathStateName);
		}

		template <typename Walk>
		void walk_sound_table(Walk walk, SoundTable& table)
		{
			walk_named_array(walk, table.adsrSettings, table.adsrCount, 7);
			walk_named_array(walk, table.ambientDefs, table.ambientDefCount, 7);
			walk.array(table.ambientEvents, table.ambientEventCount, 3);

			if (const auto elements = walk.array(table.ambientElements, table.ambientElementsCount, 7))
			{
				for (auto i = 0u; i < table.ambientElementsCount; i++)
				{
					walk.string(elements[i].aliasName);
				}
			}

			walk.array(table.filters, table.filterCount, 3);
			walk.array(table.mixes, table.mixCount, 3);
			walk_adsr_zones(walk, table.npcADSRZones, table.npcADSRZoneCount);
			walk.array(table.occlusionFilters, table.occlusionFilterCount, 3);
			walk_adsr_zones(walk, table.playerADSRZones, table.playerADSRZoneCount);
			walk.array(table.reverbs, table.reverbCount, 3);
			walk.array(table.timeScaleSettings, table.timeScaleSettingCount, 3);

			if (const auto zones = walk.array(table.zones, table.zoneCount, 7))
			{
				for (auto i = 0u; i < table.zoneCount; i++)
				{
					walk_zone_def(walk, zones[i]);
				}
			}

			walk.array(table.fullOcclusionDefs, table.fullOcclusionDefCount, 3);

			if (const auto states = walk.array(table.plrBreathStateDefs, table.plrBreathStateDefCount, 7))
			{
				for (auto i = 0u; i < table.plrBreathStateDefCount; i++)
				{
					walk.string(states[i].inhaleAlias);
					walk.string(states[i].exhaleAlias);
				}
			}
		}

		template <typename Walk>
		void walk_bank(Walk walk, SndBank& bank)
		{
			walk.string(bank.zone);
			walk.string(bank.gameLanguage);
			walk.string(bank.soundLanguage);

			if (const auto lists = walk.array(bank.alias, bank.aliasCount, 7))
			{
				for (auto i = 0u; i < bank.aliasCount; i++)
				{
					walk_alias_list(walk, lists[i]);
				}
			}

			walk.array(bank.aliasIndex, bank.aliasCount, 1);
			walk.array(bank.sendEffects, bank.sendEffectCount, 3);
			walk_sound_table(walk, bank.soundTable);

			if (const auto ducks = walk.array(bank.ducks, bank.duckCount, 7))
			{
				for (auto i = 0u; i < bank.duckCount; i++)
				{
					walk.array(ducks[i].attenuation, 128, 31);
					walk.array(ducks[i].hpf, 128, 31);
					walk.array(ducks[i].lpf, 128, 31);
					walk.array(ducks[i].focusAmount, 128, 31);
					walk.array(ducks[i].focusCone, 128, 3);
				}
			}

			if (const auto sets = walk.array(bank.musicSets, bank.musicSetCount, 7))
			{
				for (auto i = 0u; i < bank.musicSetCount; i++)
				{
					if (const auto states = walk.array(sets[i].states, sets[i].stateCount, 7))
					{
						for (auto j = 0u; j < sets[i].stateCount; j++)
						{
							walk.array(states[j].loops, states[j].loopCount, 3);
						}
					}
				}
			}

			if (const auto reflections = walk_named_array(walk, bank.weapReflections, bank.weapReflectionCount, 7))
			{
				for (auto i = 0u; i < bank.weapReflectionCount; i++)
				{
					walk.string(reflections[i].weaponClass);
					walk.string(reflections[i].type);
				}
			}

			walk.array(bank.dopplerPresets, bank.dopplerPresetCount, 3);
			walk.array(bank.occlusionShapes, bank.occlusionShapeCount, 3);
			walk.array(bank.focusCones, bank.focusConeCount, 3);
			walk.array(bank.vfCurves, bank.vfCurveCount, 3);
			walk.array(bank.lpfCurves, bank.lpfCurveCount, 3);
			walk.array(bank.hpfCurves, bank.hpfCurveCount, 3);
			walk.array(bank.rvbCurves, bank.rvbCurveCount, 3);
			walk.array(bank.speakerMaps, bank.speakerMapCount, 3);
			walk.array(bank.futzs, bank.futzCount, 3);
			walk.asset(ASSET_TYPE_STREAM_KEY, bank.streamInfo.loadedStreamKey);
		}

		template <typename Walk>
		void walk_globals(Walk walk, SndGlobals& globals)
		{
			walk.array(globals.volmodinfo, globals.volmodinfoCount, 3);
			walk.array(globals.entchannelInfo, globals.entchannelCount, 3);

			if (const auto contexts = walk.array(globals.contexts, globals.contextCount, 7))
			{
				for (auto i = 0u; i < globals.contextCount; i++)
				{
					walk.string(contexts[i].type);
				}
			}

			if (const auto values = walk.array(globals.contextValues, globals.contextValueCount, 7))
			{
				for (auto i = 0u; i < globals.contextValueCount; i++)
				{
					walk.string(values[i].value);
				}
			}

			walk.array(globals.masters, globals.masterCount, 3);
			walk.array(globals.whizbys, globals.whizbyCount, 3);
			walk.array(globals.perkinfo, globals.perkinfoCount, 3);
		}
	}

	void sound_globals::read(zone_reader& reader, SndGlobals* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_globals(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void sound_globals::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_globals(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void sound_globals::dump(SndGlobals* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("soundglobals\\"s + asset->name + ".soundglobalsb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_globals(dump_walk{dump}, *asset);
		dump.close();
	}

	SndGlobals* sound_globals::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("soundglobals\\"s + name + ".soundglobalsb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing soundglobals \"%s\"...", name.data());

		auto* asset = read.read_single<SndGlobals>();
		asset->name = read.read_string();
		walk_globals(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void sound_globals::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<SndGlobals>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string sound_globals::name()
	{
		return this->name_;
	}

	std::int32_t sound_globals::type()
	{
		return ASSET_TYPE_SOUND_GLOBALS;
	}

	void sound_bank::read(zone_reader& reader, SndBankResident* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->bank.name);
		walk_bank(read_walk{reader}, asset->bank);
		reader.pop_stream();
	}

	void sound_bank::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->bank.name = buf->write_str(this->name());
		walk_bank(write_walk{zone, buf}, dest->bank);
		buf->pop_stream();
	}

	void sound_bank::dump(SndBankResident* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("soundbank\\"s + asset->bank.name + ".soundbankb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->bank.name);
		walk_bank(dump_walk{dump}, asset->bank);
		dump.close();
	}

	SndBankResident* sound_bank::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("soundbank\\"s + name + ".soundbankb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing soundbank \"%s\"...", name.data());

		auto* asset = read.read_single<SndBankResident>();
		asset->bank.name = read.read_string();
		walk_bank(parse_walk{read}, asset->bank);
		read.close();
		return asset;
	}

	void sound_bank::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<SndBankResident>();
			this->asset_->bank.name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void sound_bank::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset(zone, ASSET_TYPE_STREAM_KEY, this->asset_->bank.streamInfo.loadedStreamKey);
	}

	std::string sound_bank::name()
	{
		return this->name_;
	}

	std::int32_t sound_bank::type()
	{
		return ASSET_TYPE_SOUND_BANK;
	}

	void sound_bank_transient::read(zone_reader& reader, SndBankTransient* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->bank.name);
		walk_bank(read_walk{reader}, asset->bank);
		reader.pop_stream();
	}

	void sound_bank_transient::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->bank.name = buf->write_str(this->name());
		walk_bank(write_walk{zone, buf}, dest->bank);
		buf->pop_stream();
	}

	void sound_bank_transient::dump(SndBankTransient* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("soundbanktransient\\"s + asset->bank.name + ".soundbanktransientb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->bank.name);
		walk_bank(dump_walk{dump}, asset->bank);
		dump.close();
	}

	SndBankTransient* sound_bank_transient::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("soundbanktransient\\"s + name + ".soundbanktransientb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing soundbanktransient \"%s\"...", name.data());

		auto* asset = read.read_single<SndBankTransient>();
		asset->bank.name = read.read_string();
		walk_bank(parse_walk{read}, asset->bank);
		read.close();
		return asset;
	}

	void sound_bank_transient::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<SndBankTransient>();
			this->asset_->bank.name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void sound_bank_transient::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset(zone, ASSET_TYPE_STREAM_KEY, this->asset_->bank.streamInfo.loadedStreamKey);
	}

	std::string sound_bank_transient::name()
	{
		return this->name_;
	}

	std::int32_t sound_bank_transient::type()
	{
		return ASSET_TYPE_SOUND_BANK_TRANSIENT;
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
		for (const auto string : sfx_package_strings)
		{
			reader.read_string(asset->*string);
		}
		reader.pop_stream();
	}

	void sfx_package::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (const auto sounds = write_array(buf, dest->sounds, 1, 7))
		{
			write_string(buf, sounds->name);
			for (auto& sound : sounds->sounds)
			{
				write_string(buf, sound.name);
			}
		}

		write_asset(zone, ASSET_TYPE_SFX_PACKAGE, dest->transientFallbackPackage);
		write_asset(zone, ASSET_TYPE_SOUND_BANK_TRANSIENT, dest->detailSoundBankNPC);
		write_asset(zone, ASSET_TYPE_SOUND_BANK_TRANSIENT, dest->detailSoundBankPlayer);
		for (const auto string : sfx_package_strings)
		{
			write_string(buf, dest->*string);
		}
		buf->pop_stream();
	}

	void sfx_package::dump(WeaponSFXPackage* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("sfxpkg\\"s + asset->name + ".sfxpkgb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_single(asset->sounds);
		if (asset->sounds)
		{
			dump.dump_string(asset->sounds->name);
			for (const auto& sound : asset->sounds->sounds)
			{
				dump.dump_string(sound.name);
			}
		}

		dump.dump_asset(asset->transientFallbackPackage);
		dump_sound_bank(dump, asset->detailSoundBankNPC);
		dump_sound_bank(dump, asset->detailSoundBankPlayer);
		for (const auto string : sfx_package_strings)
		{
			dump.dump_string(asset->*string);
		}
		dump.close();
	}

	WeaponSFXPackage* sfx_package::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("sfxpkg\\"s + name + ".sfxpkgb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing sfxpkg \"%s\"...", name.data());

		auto* asset = read.read_single<WeaponSFXPackage>();
		asset->name = read.read_string();
		if (const auto sounds = parse_single(read, asset->sounds))
		{
			sounds->name = read.read_string();
			for (auto& sound : sounds->sounds)
			{
				sound.name = read.read_string();
			}
		}

		parse_asset(read, asset->transientFallbackPackage);
		parse_sound_bank(read, mem, asset->detailSoundBankNPC);
		parse_sound_bank(read, mem, asset->detailSoundBankPlayer);
		for (const auto string : sfx_package_strings)
		{
			asset->*string = read.read_string();
		}
		read.close();
		return asset;
	}

	void sfx_package::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponSFXPackage>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void sfx_package::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset(zone, ASSET_TYPE_SFX_PACKAGE, this->asset_->transientFallbackPackage);
		add_asset(zone, ASSET_TYPE_SOUND_BANK_TRANSIENT, this->asset_->detailSoundBankNPC);
		add_asset(zone, ASSET_TYPE_SOUND_BANK_TRANSIENT, this->asset_->detailSoundBankPlayer);
	}

	std::string sfx_package::name()
	{
		return this->name_;
	}

	std::int32_t sfx_package::type()
	{
		return ASSET_TYPE_SFX_PACKAGE;
	}
}
