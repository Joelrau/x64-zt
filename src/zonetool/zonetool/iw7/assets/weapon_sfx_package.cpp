#include "std_include.hpp"
#include "weapon_sfx_package.hpp"

namespace zonetool::iw7
{
	WeaponSFXPackage* weapon_sfx_package::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "sfxpkg\\"s + name + ".json"s;
		filesystem::file file(path);
		file.open("rb");

		if (file.get_fp())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing sfxpkg \"%s\"...", name.data());

		const auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		auto data = json::parse(bytes);

		auto asset = mem->allocate<WeaponSFXPackage>();

		asset->name = mem->duplicate_string(data["name"].get<std::string>());

		//

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

		//

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

		DUMP_STRING(name);
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