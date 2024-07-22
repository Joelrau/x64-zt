#include "std_include.hpp"
#include "weapon_sfx_package.hpp"

namespace zonetool::iw7
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

	void parse_sounds(WeaponSFXPackageSounds* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_STRING(pickupSound);
		PARSE_STRING(pickupSoundPlayer);
		PARSE_STRING(ammoPickupSound);
		PARSE_STRING(ammoPickupSoundPlayer);
		PARSE_STRING(projectileSound);
		PARSE_STRING(pullbackSound);
		PARSE_STRING(pullbackSoundPlayer);
		PARSE_STRING(pullbackSoundQuick);
		PARSE_STRING(pullbackSoundQuickPlayer);
		PARSE_STRING(fireSound);
		PARSE_STRING(fireSoundPlayer);
		PARSE_STRING(fireSoundPlayerAkimbo);
		PARSE_STRING(fireSoundPlayerAkimboRight);
		PARSE_STRING(fireStartSound);
		PARSE_STRING(fireStartSoundPlayer);
		PARSE_STRING(fireLoopSound);
		PARSE_STRING(fireLoopSoundPlayer);
		PARSE_STRING(fireStopSound);
		PARSE_STRING(fireStopSoundPlayer);
		PARSE_STRING(fireLastSound);
		PARSE_STRING(fireLastSoundPlayer);
		PARSE_STRING(fireLastSoundPlayerAkimbo);
		PARSE_STRING(fireLastSoundPlayerAkimboRight);
		PARSE_STRING(regenerateSound);
		PARSE_STRING(regenerateSoundPlayer);
		PARSE_STRING(emptyFireSound);
		PARSE_STRING(emptyFireSoundPlayer);
		PARSE_STRING(emptyRegenFireSound);
		PARSE_STRING(emptyRegenFireSoundPlayer);
		PARSE_STRING(rechamberSound);
		PARSE_STRING(rechamberSoundPlayer);
		PARSE_STRING(reloadSound);
		PARSE_STRING(reloadSoundPlayer);
		PARSE_STRING(reloadEmptySound);
		PARSE_STRING(reloadEmptySoundPlayer);
		PARSE_STRING(reloadStartSound);
		PARSE_STRING(reloadStartSoundPlayer);
		PARSE_STRING(reloadEndSound);
		PARSE_STRING(reloadEndSoundPlayer);
		PARSE_STRING(detonateSound);
		PARSE_STRING(detonateSoundPlayer);
		PARSE_STRING(nightVisionWearSound);
		PARSE_STRING(nightVisionWearSoundPlayer);
		PARSE_STRING(nightVisionRemoveSound);
		PARSE_STRING(nightVisionRemoveSoundPlayer);
		PARSE_STRING(altSwitchSound);
		PARSE_STRING(altSwitchSoundPlayer);
		PARSE_STRING(raiseSound);
		PARSE_STRING(raiseSoundPlayer);
		PARSE_STRING(firstRaiseSound);
		PARSE_STRING(firstRaiseSoundPlayer);
		PARSE_STRING(putawaySound);
		PARSE_STRING(putawaySoundPlayer);
		PARSE_STRING(enterAdsSoundPlayer);
		PARSE_STRING(leaveAdsSoundPlayer);
		PARSE_STRING(bulletRicochetImpact);
		PARSE_STRING(projExplosionSound);
		PARSE_STRING(projDudSound);
		PARSE_STRING(projIgnitionSound);
		PARSE_STRING(bounceSound);
		PARSE_STRING(rollingSound);
	}

	WeaponSFXPackage* weapon_sfx_package::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "sfxpkg\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing sfxpkg \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<WeaponSFXPackage>();
		asset->name = mem->duplicate_string(name);

		PARSE_FIELD(rattleSoundType);
		PARSE_STRING(szAdsrBaseSetting);
		PARSE_STRING(szWeapSndReflectionClass);
		PARSE_FIELD(weapSndFireVolumeShot1);
		PARSE_FIELD(weapSndFireVolumeShot2);
		PARSE_FIELD(weapSndFireVolumeShot3);
		PARSE_FIELD(weapSndProneFireLFEVolume);

		if (!data["sounds"].is_null())
		{
			asset->sounds = mem->allocate<WeaponSFXPackageSounds>();
			parse_sounds(asset->sounds, data["sounds"], mem);
		}
		else
		{
			asset->sounds = nullptr;
		}

		return asset;
	}

	void weapon_sfx_package::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).weaponSFXPackage;
		}
	}

	void weapon_sfx_package::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void weapon_sfx_package::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;
		if (asset->sounds)
		{
#define LOAD_DEPENDING_SOUND(__field__) \
			if(asset->sounds->__field__) \
			{ \
			}

			LOAD_DEPENDING_SOUND(pickupSound);
			LOAD_DEPENDING_SOUND(pickupSoundPlayer);
			LOAD_DEPENDING_SOUND(ammoPickupSound);
			LOAD_DEPENDING_SOUND(ammoPickupSoundPlayer);
			LOAD_DEPENDING_SOUND(projectileSound);
			LOAD_DEPENDING_SOUND(pullbackSound);
			LOAD_DEPENDING_SOUND(pullbackSoundPlayer);
			LOAD_DEPENDING_SOUND(pullbackSoundQuick);
			LOAD_DEPENDING_SOUND(pullbackSoundQuickPlayer);
			LOAD_DEPENDING_SOUND(fireSound);
			LOAD_DEPENDING_SOUND(fireSoundPlayer);
			LOAD_DEPENDING_SOUND(fireSoundPlayerAkimbo);
			LOAD_DEPENDING_SOUND(fireSoundPlayerAkimboRight);
			LOAD_DEPENDING_SOUND(fireStartSound);
			LOAD_DEPENDING_SOUND(fireStartSoundPlayer);
			LOAD_DEPENDING_SOUND(fireLoopSound);
			LOAD_DEPENDING_SOUND(fireLoopSoundPlayer);
			LOAD_DEPENDING_SOUND(fireStopSound);
			LOAD_DEPENDING_SOUND(fireStopSoundPlayer);
			LOAD_DEPENDING_SOUND(fireLastSound);
			LOAD_DEPENDING_SOUND(fireLastSoundPlayer);
			LOAD_DEPENDING_SOUND(fireLastSoundPlayerAkimbo);
			LOAD_DEPENDING_SOUND(fireLastSoundPlayerAkimboRight);
			LOAD_DEPENDING_SOUND(regenerateSound);
			LOAD_DEPENDING_SOUND(regenerateSoundPlayer);
			LOAD_DEPENDING_SOUND(emptyFireSound);
			LOAD_DEPENDING_SOUND(emptyFireSoundPlayer);
			LOAD_DEPENDING_SOUND(emptyRegenFireSound);
			LOAD_DEPENDING_SOUND(emptyRegenFireSoundPlayer);
			LOAD_DEPENDING_SOUND(rechamberSound);
			LOAD_DEPENDING_SOUND(rechamberSoundPlayer);
			LOAD_DEPENDING_SOUND(reloadSound);
			LOAD_DEPENDING_SOUND(reloadSoundPlayer);
			LOAD_DEPENDING_SOUND(reloadEmptySound);
			LOAD_DEPENDING_SOUND(reloadEmptySoundPlayer);
			LOAD_DEPENDING_SOUND(reloadStartSound);
			LOAD_DEPENDING_SOUND(reloadStartSoundPlayer);
			LOAD_DEPENDING_SOUND(reloadEndSound);
			LOAD_DEPENDING_SOUND(reloadEndSoundPlayer);
			LOAD_DEPENDING_SOUND(detonateSound);
			LOAD_DEPENDING_SOUND(detonateSoundPlayer);
			LOAD_DEPENDING_SOUND(nightVisionWearSound);
			LOAD_DEPENDING_SOUND(nightVisionWearSoundPlayer);
			LOAD_DEPENDING_SOUND(nightVisionRemoveSound);
			LOAD_DEPENDING_SOUND(nightVisionRemoveSoundPlayer);
			LOAD_DEPENDING_SOUND(altSwitchSound);
			LOAD_DEPENDING_SOUND(altSwitchSoundPlayer);
			LOAD_DEPENDING_SOUND(raiseSound);
			LOAD_DEPENDING_SOUND(raiseSoundPlayer);
			LOAD_DEPENDING_SOUND(firstRaiseSound);
			LOAD_DEPENDING_SOUND(firstRaiseSoundPlayer);
			LOAD_DEPENDING_SOUND(putawaySound);
			LOAD_DEPENDING_SOUND(putawaySoundPlayer);
			LOAD_DEPENDING_SOUND(enterAdsSoundPlayer);
			LOAD_DEPENDING_SOUND(leaveAdsSoundPlayer);
			LOAD_DEPENDING_SOUND(bulletRicochetImpact);
			LOAD_DEPENDING_SOUND(projExplosionSound);
			LOAD_DEPENDING_SOUND(projDudSound);
			LOAD_DEPENDING_SOUND(projIgnitionSound);
			LOAD_DEPENDING_SOUND(bounceSound);
			LOAD_DEPENDING_SOUND(rollingSound);
			
#undef LOAD_DEPENDING_SOUND
		}
	}

	std::string weapon_sfx_package::name()
	{
		return this->name_;
	}

	std::int32_t weapon_sfx_package::type()
	{
		return ASSET_TYPE_SFX_PACKAGE;
	}

	void weapon_sfx_package::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->sounds)
		{
			buf->align(7);
			dest->sounds = buf->write(data->sounds);

#define WRITE_STRING(__field__) \
			if(data->sounds->__field__) \
			{ \
				dest->sounds->__field__ = buf->write_str(data->sounds->__field__); \
			}

			WRITE_STRING(name);
			WRITE_STRING(pickupSound);
			WRITE_STRING(pickupSoundPlayer);
			WRITE_STRING(ammoPickupSound);
			WRITE_STRING(ammoPickupSoundPlayer);
			WRITE_STRING(projectileSound);
			WRITE_STRING(pullbackSound);
			WRITE_STRING(pullbackSoundPlayer);
			WRITE_STRING(pullbackSoundQuick);
			WRITE_STRING(pullbackSoundQuickPlayer);
			WRITE_STRING(fireSound);
			WRITE_STRING(fireSoundPlayer);
			WRITE_STRING(fireSoundPlayerAkimbo);
			WRITE_STRING(fireSoundPlayerAkimboRight);
			WRITE_STRING(fireStartSound);
			WRITE_STRING(fireStartSoundPlayer);
			WRITE_STRING(fireLoopSound);
			WRITE_STRING(fireLoopSoundPlayer);
			WRITE_STRING(fireStopSound);
			WRITE_STRING(fireStopSoundPlayer);
			WRITE_STRING(fireLastSound);
			WRITE_STRING(fireLastSoundPlayer);
			WRITE_STRING(fireLastSoundPlayerAkimbo);
			WRITE_STRING(fireLastSoundPlayerAkimboRight);
			WRITE_STRING(regenerateSound);
			WRITE_STRING(regenerateSoundPlayer);
			WRITE_STRING(emptyFireSound);
			WRITE_STRING(emptyFireSoundPlayer);
			WRITE_STRING(emptyRegenFireSound);
			WRITE_STRING(emptyRegenFireSoundPlayer);
			WRITE_STRING(rechamberSound);
			WRITE_STRING(rechamberSoundPlayer);
			WRITE_STRING(reloadSound);
			WRITE_STRING(reloadSoundPlayer);
			WRITE_STRING(reloadEmptySound);
			WRITE_STRING(reloadEmptySoundPlayer);
			WRITE_STRING(reloadStartSound);
			WRITE_STRING(reloadStartSoundPlayer);
			WRITE_STRING(reloadEndSound);
			WRITE_STRING(reloadEndSoundPlayer);
			WRITE_STRING(detonateSound);
			WRITE_STRING(detonateSoundPlayer);
			WRITE_STRING(nightVisionWearSound);
			WRITE_STRING(nightVisionWearSoundPlayer);
			WRITE_STRING(nightVisionRemoveSound);
			WRITE_STRING(nightVisionRemoveSoundPlayer);
			WRITE_STRING(altSwitchSound);
			WRITE_STRING(altSwitchSoundPlayer);
			WRITE_STRING(raiseSound);
			WRITE_STRING(raiseSoundPlayer);
			WRITE_STRING(firstRaiseSound);
			WRITE_STRING(firstRaiseSoundPlayer);
			WRITE_STRING(putawaySound);
			WRITE_STRING(putawaySoundPlayer);
			WRITE_STRING(enterAdsSoundPlayer);
			WRITE_STRING(leaveAdsSoundPlayer);
			WRITE_STRING(bulletRicochetImpact);
			WRITE_STRING(projExplosionSound);
			WRITE_STRING(projDudSound);
			WRITE_STRING(projIgnitionSound);
			WRITE_STRING(bounceSound);
			WRITE_STRING(rollingSound);

#undef WRITE_STRING

			buf->clear_pointer(&dest->sounds);
		}

		if (data->szAdsrBaseSetting)
		{
			dest->szAdsrBaseSetting = buf->write_str(data->szAdsrBaseSetting);
		}

		if (data->szWeapSndReflectionClass)
		{
			dest->szWeapSndReflectionClass = buf->write_str(data->szWeapSndReflectionClass);
		}

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

	void dump_sounds(WeaponSFXPackageSounds* asset, ordered_json& data)
	{
		DUMP_STRING(name);
		DUMP_STRING(pickupSound);
		DUMP_STRING(pickupSoundPlayer);
		DUMP_STRING(ammoPickupSound);
		DUMP_STRING(ammoPickupSoundPlayer);
		DUMP_STRING(projectileSound);
		DUMP_STRING(pullbackSound);
		DUMP_STRING(pullbackSoundPlayer);
		DUMP_STRING(pullbackSoundQuick);
		DUMP_STRING(pullbackSoundQuickPlayer);
		DUMP_STRING(fireSound);
		DUMP_STRING(fireSoundPlayer);
		DUMP_STRING(fireSoundPlayerAkimbo);
		DUMP_STRING(fireSoundPlayerAkimboRight);
		DUMP_STRING(fireStartSound);
		DUMP_STRING(fireStartSoundPlayer);
		DUMP_STRING(fireLoopSound);
		DUMP_STRING(fireLoopSoundPlayer);
		DUMP_STRING(fireStopSound);
		DUMP_STRING(fireStopSoundPlayer);
		DUMP_STRING(fireLastSound);
		DUMP_STRING(fireLastSoundPlayer);
		DUMP_STRING(fireLastSoundPlayerAkimbo);
		DUMP_STRING(fireLastSoundPlayerAkimboRight);
		DUMP_STRING(regenerateSound);
		DUMP_STRING(regenerateSoundPlayer);
		DUMP_STRING(emptyFireSound);
		DUMP_STRING(emptyFireSoundPlayer);
		DUMP_STRING(emptyRegenFireSound);
		DUMP_STRING(emptyRegenFireSoundPlayer);
		DUMP_STRING(rechamberSound);
		DUMP_STRING(rechamberSoundPlayer);
		DUMP_STRING(reloadSound);
		DUMP_STRING(reloadSoundPlayer);
		DUMP_STRING(reloadEmptySound);
		DUMP_STRING(reloadEmptySoundPlayer);
		DUMP_STRING(reloadStartSound);
		DUMP_STRING(reloadStartSoundPlayer);
		DUMP_STRING(reloadEndSound);
		DUMP_STRING(reloadEndSoundPlayer);
		DUMP_STRING(detonateSound);
		DUMP_STRING(detonateSoundPlayer);
		DUMP_STRING(nightVisionWearSound);
		DUMP_STRING(nightVisionWearSoundPlayer);
		DUMP_STRING(nightVisionRemoveSound);
		DUMP_STRING(nightVisionRemoveSoundPlayer);
		DUMP_STRING(altSwitchSound);
		DUMP_STRING(altSwitchSoundPlayer);
		DUMP_STRING(raiseSound);
		DUMP_STRING(raiseSoundPlayer);
		DUMP_STRING(firstRaiseSound);
		DUMP_STRING(firstRaiseSoundPlayer);
		DUMP_STRING(putawaySound);
		DUMP_STRING(putawaySoundPlayer);
		DUMP_STRING(enterAdsSoundPlayer);
		DUMP_STRING(leaveAdsSoundPlayer);
		DUMP_STRING(bulletRicochetImpact);
		DUMP_STRING(projExplosionSound);
		DUMP_STRING(projDudSound);
		DUMP_STRING(projIgnitionSound);
		DUMP_STRING(bounceSound);
		DUMP_STRING(rollingSound);
	}

	void weapon_sfx_package::dump(WeaponSFXPackage* asset)
	{
		const auto path = "sfxpkg\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_FIELD(rattleSoundType);
		DUMP_STRING(szAdsrBaseSetting);
		DUMP_STRING(szWeapSndReflectionClass);
		DUMP_FIELD(weapSndFireVolumeShot1);
		DUMP_FIELD(weapSndFireVolumeShot2);
		DUMP_FIELD(weapSndFireVolumeShot3);
		DUMP_FIELD(weapSndProneFireLFEVolume);

		if (asset->sounds)
		{
			dump_sounds(asset->sounds, data["sounds"]);
		}
		else
		{
			data["sounds"] = nullptr;
		}

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}