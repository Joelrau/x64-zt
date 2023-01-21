#include "std_include.hpp"
#include "vehicledef.hpp"

namespace zonetool::h2
{

#define VEHICLE_READ_FIELD(__type__, __field__) \
	if (!data[#__field__].is_null()) *(__type__*)(&vehicle->__field__) = (__type__)data[#__field__].get<__type__>()

#define VEHICLE_READ_FIELD_ARR(__type__, __field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			*(__type__*)(&vehicle->__field__[idx##__field__]) = (__type__)data[#__field__][idx##__field__].get<__type__>(); \
		} \
	}

#define VEHICLE_READ_STRING(__field__) \
	if (!data[#__field__].is_null()) vehicle->__field__ = mem->StrDup(data[#__field__].get<std::string>())

#define VEHICLE_READ_ASSET(__type__, __datafield__, __field__) \
	if (!data[#__field__].is_null() && data[#__field__].is_string()) \
	{ \
		auto asset##__field__ = data[#__field__].get<std::string>(); \
		if (asset##__field__.empty()) \
		{ \
			vehicle->__field__ = nullptr; \
		} \
		else \
		{ \
			vehicle->__field__ = DB_FindXAssetHeader(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
		} \
	} \
	else \
	{ \
		vehicle->__field__ = nullptr; \
	}

#define VEHICLE_READ_ASSET_ARR(__type__, __datafield__, __field__, __struct__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto asset##__field__ = data[#__field__][idx##__field__].get<std::string>(); \
			if (asset##__field__.empty()) \
			{ \
				vehicle->__field__[idx##__field__] = nullptr; \
			} \
			else \
			{ \
				vehicle->__field__[idx##__field__] = DB_FindXAssetHeader(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
			} \
		} \
	} \

#define VEHICLE_READ_VALUE_ARR(__type__, __field__, __count__) \
	if (!data[#__field__].is_null()) \
	{ \
		for (auto i = 0; i < __count__; i++) \
		{ \
				vehicle->__field__[i] = data[#__field__][i].get<__type__>(); \
		} \
	} \

	namespace
	{
		constexpr const char* vehicle_fire_type_names[VEH_TURRET_FIRE_TYPE_COUNT] =
		{
			"singleFire",
			"dualFire",
			"altFire",
		};

		constexpr const char* vehicle_axle_type_names[VEH_AXLE_COUNT] =
		{
			"front",
			"rear",
			"all",
		};

		constexpr const char* vehicle_type_names[VEH_TYPE_COUNT] =
		{
			"wheels4",
			"tank",
			"plane",
			"boat",
			"artillery",
			"helicopter",
			"snowmobile",
			"submarine",
			"ugv",
		};

		void parse_veh_phys_def(VehiclePhysDef* vehicle, json& data, ZoneMemory* mem)
		{
			VEHICLE_READ_FIELD(int, physicsEnabled);
			VEHICLE_READ_STRING(physPresetName);
			VEHICLE_READ_ASSET(ASSET_TYPE_PHYSPRESET, physPreset, physPreset);
			VEHICLE_READ_STRING(unkString01);
			//VEHICLE_READ_THING(VehicleUnknown, vehicleUnknown01);
			VEHICLE_READ_STRING(accelGraphName);
			VEHICLE_READ_FIELD(VehicleAxleType, steeringAxle);
			VEHICLE_READ_FIELD(VehicleAxleType, powerAxle);
			VEHICLE_READ_FIELD(VehicleAxleType, brakingAxle);
			VEHICLE_READ_VALUE_ARR(float, floatValues01, 53);
		}
	}

	void IVehicleDef::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* IVehicleDef::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	VehicleDef* IVehicleDef::parse(const std::string& name, ZoneMemory* mem)
	{
		const auto path = "vehicles\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vehicle \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		json data = json::parse(bytes);

		auto* vehicle = mem->Alloc<VehicleDef>();

		// base asset
		auto base = data["baseAsset"].get<std::string>();
		VehicleDef* baseAsset = nullptr;
		if (!base.empty())
		{
			baseAsset = DB_FindXAssetHeader(ASSET_TYPE_VEHICLE, base.data(), 0).vehicle;
			if (baseAsset == nullptr || DB_IsXAssetDefault(ASSET_TYPE_VEHICLE, base.data()))
			{
				ZONETOOL_WARNING("Could not load base asset \"%s\" into memory...", base.data());
			}
			else
			{
				memcpy(vehicle, baseAsset, sizeof(VehicleDef));
			}
		}
		else
		{
			ZONETOOL_WARNING("No base asset is defined for vehicle \"%s\", stuff might go wrong!", name.data());
		}

		VEHICLE_READ_STRING(internalName);
		VEHICLE_READ_FIELD(VehicleType, vehicleType);
		VEHICLE_READ_STRING(useHintString);
		VEHICLE_READ_FIELD(int, health);
		VEHICLE_READ_VALUE_ARR(float, floatValues01, 37);
		VEHICLE_READ_VALUE_ARR(int, intValues01, 8);

		parse_veh_phys_def(&vehicle->vehPhysDef, data["vehPhysDef"], mem);

		VEHICLE_READ_VALUE_ARR(float, floatValues02, 12);

		VEHICLE_READ_STRING(unkString01);

		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, effect01);
		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, effect02);
		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, effect03);
		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, effect04);
		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, effect05);

		VEHICLE_READ_VALUE_ARR(float, floatValues03, 7);
		VEHICLE_READ_VALUE_ARR(int, intValues02, 1);
		VEHICLE_READ_VALUE_ARR(float, floatValues04, 32);

		VEHICLE_READ_FIELD(int, vehHelicopterSoftCollisions);
		VEHICLE_READ_FIELD(int, vehHelicopterUseGroundFX);

		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, vehHelicopterGroundFx);
		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, vehHelicopterGroundWaterFx);

		VEHICLE_READ_VALUE_ARR(float, floatValues05, 78);
		VEHICLE_READ_STRING(unkString02);
		VEHICLE_READ_VALUE_ARR(float, floatValues06, 82);
		VEHICLE_READ_STRING(turretWeaponName);
		VEHICLE_READ_ASSET(ASSET_TYPE_WEAPON, weapon, turretWeapon);

		VEHICLE_READ_FIELD(float, turretHorizSpanLeft);
		VEHICLE_READ_FIELD(float, turretHorizSpanRight);
		VEHICLE_READ_FIELD(float, turretVertSpanUp);
		VEHICLE_READ_FIELD(float, turretVertSpanDown);
		VEHICLE_READ_FIELD(float, turretHorizResistLeft);
		VEHICLE_READ_FIELD(float, turretHorizResistRight);
		VEHICLE_READ_FIELD(float, turretVertResistUp);
		VEHICLE_READ_FIELD(float, turretVertResistDown);
		VEHICLE_READ_FIELD(float, turretRotRate);
		VEHICLE_READ_FIELD(VehicleTurretFireType, turretFireType);

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, turretSpinSnd);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, turretStopSnd);

		VEHICLE_READ_FIELD(int, trophyEnabled);
		VEHICLE_READ_FIELD(float, trophyRadius);
		VEHICLE_READ_FIELD(float, trophyInactiveRadius);
		VEHICLE_READ_FIELD(int, trophyAmmoCount);
		VEHICLE_READ_FIELD(float, trophyReloadTime);

		for (auto i = 0; i < 4; i++)
		{
			if (!data["trophyTags"][i].empty())
			{
				this->add_script_string(&vehicle->trophyTags[i], mem->StrDup(data["trophyTags"][i].get<std::string>()));
			}
		}

		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, trophyExplodeFx);
		VEHICLE_READ_ASSET(ASSET_TYPE_FX, fx, trophyFlashFx);

		VEHICLE_READ_ASSET(ASSET_TYPE_MATERIAL, material, compassFriendlyIcon);
		VEHICLE_READ_ASSET(ASSET_TYPE_MATERIAL, material, compassEnemyIcon);
		VEHICLE_READ_ASSET(ASSET_TYPE_MATERIAL, material, compassFriendlyAltIcon);
		VEHICLE_READ_ASSET(ASSET_TYPE_MATERIAL, material, compassEnemyAltIcon);

		VEHICLE_READ_FIELD(int, compassIconWidth);
		VEHICLE_READ_FIELD(int, compassIconHeight);

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, idleLowSnd);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, idleHighSnd);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineLowSnd);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineHighSnd);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, sound01);

		VEHICLE_READ_FIELD(float, engineSndSpeed);
		if (!data["audioOriginTag"].empty())
		{
			this->add_script_string(&vehicle->audioOriginTag, mem->StrDup(data["audioOriginTag"].get<std::string>()));
		}

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, idleLowSndAlt);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, idleHighSndAlt);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineLowSndAlt);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineHighSndAlt);

		VEHICLE_READ_FIELD(float, engineSndSpeedAlt);
		if (!data["audioOriginTagAlt"].empty())
		{
			this->add_script_string(&vehicle->audioOriginTagAlt, mem->StrDup(data["audioOriginTagAlt"].get<std::string>()));
		}

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, turretSpinSndAlt);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, turretStopSndAlt);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineStartUpSnd);
		VEHICLE_READ_FIELD(int, engineStartUpLength);

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineShutdownSnd);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineIdleSnd);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineSustainSnd);
		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineRampUpSnd);
		VEHICLE_READ_FIELD(int, engineRampUpLength);

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, engineRampDownSnd);
		VEHICLE_READ_FIELD(int, engineRampDownLength);

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, suspensionSoftSnd);
		VEHICLE_READ_FIELD(float, suspensionSoftCompression);

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, suspensionHardSnd);
		VEHICLE_READ_FIELD(float, suspensionHardCompression);

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, collisionSnd);
		VEHICLE_READ_FIELD(float, collisionBlendSpeed);

		VEHICLE_READ_ASSET(ASSET_TYPE_SOUND, sound, speedSnd);
		VEHICLE_READ_FIELD(float, speedSndBlendSpeed);

		VEHICLE_READ_STRING(surfaceSndPrefix);

		VEHICLE_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, surfaceSnds, snd_alias_list_t, 53);

		VEHICLE_READ_FIELD(float, surfaceSndBlendSpeed);
		VEHICLE_READ_FIELD(float, slideVolume);
		VEHICLE_READ_FIELD(float, slideBlendSpeed);
		VEHICLE_READ_FIELD(float, inAirPitch);

		VEHICLE_READ_STRING(soundTriggerOverrideZone);

		VEHICLE_READ_FIELD(bool, soundTriggerOverrideReverb);
		VEHICLE_READ_FIELD(bool, soundTriggerOverrideMix);
		VEHICLE_READ_FIELD(bool, soundTriggerOverrideFilter);
		VEHICLE_READ_FIELD(bool, soundTriggerOverrideOcclusion);
		VEHICLE_READ_FIELD(bool, soundTriggerOverrideAmbient);
		VEHICLE_READ_FIELD(bool, soundTriggerOverrideAmbientEvents);
		VEHICLE_READ_FIELD(bool, soundTriggerOverrideADSR);

		return vehicle;
	}

	void IVehicleDef::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->internalName = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Vehicle file \"%s\" not found.", name.data());
		}
	}

	void IVehicleDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
		auto* vehicle = this->asset_;

		for (auto i = 0; i < 4; i++)
		{
			vehicle->trophyTags[i] = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&vehicle->trophyTags[i])));
		}

		vehicle->audioOriginTag = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&vehicle->audioOriginTag)));

		vehicle->audioOriginTagAlt = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&vehicle->audioOriginTagAlt)));
	}

	void IVehicleDef::load_depending(IZone* zone)
	{
		auto vehicle = this->asset_;
#define VEHICLE_SUBASSET_DEPENDING(__field__,__type__/*,__struct__*/) \
		if (vehicle->__field__) \
		{ \
			zone->add_asset_of_type(__type__, vehicle->__field__->name); \
		}

		VEHICLE_SUBASSET_DEPENDING(vehPhysDef.physPreset, ASSET_TYPE_PHYSPRESET);

		VEHICLE_SUBASSET_DEPENDING(effect01, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(effect02, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(effect03, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(effect04, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(effect05, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(vehHelicopterGroundFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(vehHelicopterGroundWaterFx, ASSET_TYPE_FX);

		VEHICLE_SUBASSET_DEPENDING(turretWeapon, ASSET_TYPE_WEAPON);

		VEHICLE_SUBASSET_DEPENDING(turretSpinSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(turretStopSnd, ASSET_TYPE_SOUND);

		VEHICLE_SUBASSET_DEPENDING(trophyExplodeFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(trophyFlashFx, ASSET_TYPE_FX);

		VEHICLE_SUBASSET_DEPENDING(compassFriendlyIcon, ASSET_TYPE_MATERIAL);
		VEHICLE_SUBASSET_DEPENDING(compassEnemyIcon, ASSET_TYPE_MATERIAL);
		VEHICLE_SUBASSET_DEPENDING(compassFriendlyAltIcon, ASSET_TYPE_MATERIAL);
		VEHICLE_SUBASSET_DEPENDING(compassEnemyAltIcon, ASSET_TYPE_MATERIAL);

		VEHICLE_SUBASSET_DEPENDING(idleLowSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(idleHighSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineLowSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineHighSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(sound01, ASSET_TYPE_SOUND);

		VEHICLE_SUBASSET_DEPENDING(idleLowSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(idleHighSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineLowSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineHighSndAlt, ASSET_TYPE_SOUND);

		VEHICLE_SUBASSET_DEPENDING(turretSpinSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(turretStopSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineStartUpSnd, ASSET_TYPE_SOUND);

		VEHICLE_SUBASSET_DEPENDING(engineShutdownSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineIdleSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineSustainSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineRampUpSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineRampDownSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(suspensionSoftSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(suspensionHardSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(collisionSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(speedSnd, ASSET_TYPE_SOUND);

		for (auto i = 0; i < 53; i++)
		{
			VEHICLE_SUBASSET_DEPENDING(surfaceSnds[i], ASSET_TYPE_SOUND);
		}
	}

	std::string IVehicleDef::name()
	{
		return this->name_;
	}

	std::int32_t IVehicleDef::type()
	{
		return ASSET_TYPE_VEHICLE;
	}

	void IVehicleDef::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

#define VEHICLE_STRING(__field__) \
		if (data->__field__) \
		{ \
			dest->__field__ = buf->write_str(data->__field__); \
		}

#define VEHICLE_SUBASSET(__field__,__type__,__struct__) \
		if (data->__field__) \
		{ \
			dest->__field__ = reinterpret_cast<__struct__*>(zone->get_asset_pointer(__type__, data->__field__->name)); \
		}

#define VEHICLE_SOUND_CUSTOM(__field__) \
		if (data->__field__) \
		{ \
			auto ptr = 0xFDFDFDFFFFFFFFFF; \
			buf->align(7); \
			buf->write(&ptr); \
			buf->write_str(data->__field__->name); \
			ZoneBuffer::clear_pointer(&dest->__field__); \
		}

		buf->push_stream(3);

		dest->internalName = buf->write_str(this->name());

		VEHICLE_STRING(useHintString);

		VEHICLE_STRING(vehPhysDef.physPresetName);
		VEHICLE_SUBASSET(vehPhysDef.physPreset, ASSET_TYPE_PHYSPRESET, PhysPreset);
		VEHICLE_STRING(vehPhysDef.unkString01);
		VEHICLE_STRING(vehPhysDef.accelGraphName);

		VEHICLE_STRING(unkString01);

		VEHICLE_SUBASSET(effect01, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(effect02, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(effect03, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(effect04, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(effect05, ASSET_TYPE_FX, FxEffectDef);

		VEHICLE_SUBASSET(vehHelicopterGroundFx, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(vehHelicopterGroundWaterFx, ASSET_TYPE_FX, FxEffectDef);

		VEHICLE_STRING(unkString02);
		VEHICLE_STRING(turretWeaponName);

		VEHICLE_SUBASSET(turretWeapon, ASSET_TYPE_WEAPON, WeaponDef);

		VEHICLE_SOUND_CUSTOM(turretSpinSnd);
		VEHICLE_SOUND_CUSTOM(turretStopSnd);

		VEHICLE_SUBASSET(trophyExplodeFx, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(trophyFlashFx, ASSET_TYPE_FX, FxEffectDef);

		VEHICLE_SUBASSET(compassFriendlyIcon, ASSET_TYPE_MATERIAL, Material);
		VEHICLE_SUBASSET(compassEnemyIcon, ASSET_TYPE_MATERIAL, Material);
		VEHICLE_SUBASSET(compassFriendlyAltIcon, ASSET_TYPE_MATERIAL, Material);
		VEHICLE_SUBASSET(compassEnemyAltIcon, ASSET_TYPE_MATERIAL, Material);

		VEHICLE_SOUND_CUSTOM(idleLowSnd);
		VEHICLE_SOUND_CUSTOM(idleHighSnd);
		VEHICLE_SOUND_CUSTOM(engineLowSnd);
		VEHICLE_SOUND_CUSTOM(engineHighSnd);
		VEHICLE_SOUND_CUSTOM(sound01);
		VEHICLE_SOUND_CUSTOM(idleLowSndAlt);
		VEHICLE_SOUND_CUSTOM(idleHighSndAlt);
		VEHICLE_SOUND_CUSTOM(engineLowSndAlt);
		VEHICLE_SOUND_CUSTOM(engineHighSndAlt);
		VEHICLE_SOUND_CUSTOM(turretSpinSndAlt);
		VEHICLE_SOUND_CUSTOM(turretStopSndAlt);
		VEHICLE_SOUND_CUSTOM(engineStartUpSnd);
		VEHICLE_SOUND_CUSTOM(engineShutdownSnd);
		VEHICLE_SOUND_CUSTOM(engineIdleSnd);
		VEHICLE_SOUND_CUSTOM(engineSustainSnd);
		VEHICLE_SOUND_CUSTOM(engineRampUpSnd);
		VEHICLE_SOUND_CUSTOM(engineRampDownSnd);
		VEHICLE_SOUND_CUSTOM(suspensionSoftSnd);
		VEHICLE_SOUND_CUSTOM(suspensionHardSnd);
		VEHICLE_SOUND_CUSTOM(collisionSnd);
		VEHICLE_SOUND_CUSTOM(speedSnd);

		VEHICLE_STRING(surfaceSndPrefix);

		for (auto i = 0; i < 53; i++)
		{
			VEHICLE_SOUND_CUSTOM(surfaceSnds[i]);
		}

		VEHICLE_STRING(soundTriggerOverrideZone);

		buf->pop_stream();
	}

#define VEHICLE_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__ \

#define VEHICLE_DUMP_SUB_FIELD(__field__, __subfield__) \
	data[#__field__][#__subfield__] = asset->__field__.__subfield__ \

#define VEHICLE_DUMP_SUB_FIELD_STRING(__field__, __subfield__) \
	if (asset->__field__.__subfield__ != nullptr) \
	{ \
		data[#__field__][#__subfield__] = asset->__field__.__subfield__; \
	} \
	else \
	{ \
		data[#__field__][#__subfield__] = nullptr; \
	} \

#define VEHICLE_DUMP_STRING(__name__) \
	if (asset->__name__ != nullptr) \
	{ \
		data[#__name__] = asset->__name__; \
	} \
	else \
	{ \
		data[#__name__] = nullptr; \
	} \

#define VEHICLE_DUMP_VALUE_ARRAY(__field__, __count__) \
	for (auto i = 0; i < __count__; i++) \
	{ \
		data[#__field__][i] = asset->__field__[i]; \
	} \

#define VEHICLE_DUMP_SUB_VALUE_ARRAY(__field__, __subfield__, __count__) \
	for (auto i = 0; i < __count__; i++) \
	{ \
		data[#__field__][#__subfield__][i] = asset->__field__.__subfield__[i]; \
	} \

#define VEHICLE_DUMP_SUB_ASSET(__field__, __subfield__) \
	if (asset->__field__.__subfield__) \
	{ \
		data[#__field__][#__subfield__] = asset->__field__.__subfield__->name; \
	} \
	else \
	{ \
		data[#__field__][#__subfield__] = ""; \
	}

#define VEHICLE_DUMP_AXLE_TYPE(__name__) \
	data["vehPhysDef"][#__name__] = vehicle_axle_type_names[asset->vehPhysDef.__name__]; \

#define VEHICLE_DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	} \

	void IVehicleDef::dump(VehicleDef* asset)
	{
		const auto path = "vehicles\\"s + asset->internalName + ".json"s;

		ordered_json data;

		data["baseAsset"] = asset->internalName;

		VEHICLE_DUMP_STRING(internalName);
		VEHICLE_DUMP_FIELD(vehicleType);
		VEHICLE_DUMP_STRING(useHintString);
		VEHICLE_DUMP_FIELD(health);

		VEHICLE_DUMP_VALUE_ARRAY(floatValues01, 37);
		VEHICLE_DUMP_VALUE_ARRAY(intValues01, 8);

		VEHICLE_DUMP_SUB_FIELD(vehPhysDef, physicsEnabled);
		VEHICLE_DUMP_SUB_FIELD_STRING(vehPhysDef, physPresetName);
		VEHICLE_DUMP_SUB_ASSET(vehPhysDef, physPreset);
		VEHICLE_DUMP_SUB_FIELD_STRING(vehPhysDef, unkString01);
		//VEHICLE_DUMP_THING(vehPhysDef, vehicleUnknown01);
		VEHICLE_DUMP_SUB_FIELD_STRING(vehPhysDef, accelGraphName);
		VEHICLE_DUMP_SUB_FIELD(vehPhysDef, steeringAxle);
		VEHICLE_DUMP_SUB_FIELD(vehPhysDef, powerAxle);
		VEHICLE_DUMP_SUB_FIELD(vehPhysDef, brakingAxle);
		VEHICLE_DUMP_SUB_VALUE_ARRAY(vehPhysDef, floatValues01, 53);

		VEHICLE_DUMP_VALUE_ARRAY(floatValues02, 12);
		VEHICLE_DUMP_STRING(unkString01);
		VEHICLE_DUMP_ASSET(effect01);
		VEHICLE_DUMP_ASSET(effect02);
		VEHICLE_DUMP_ASSET(effect03);
		VEHICLE_DUMP_ASSET(effect04);
		VEHICLE_DUMP_ASSET(effect05);
		VEHICLE_DUMP_VALUE_ARRAY(floatValues03, 7);
		VEHICLE_DUMP_VALUE_ARRAY(intValues02, 1);
		VEHICLE_DUMP_VALUE_ARRAY(floatValues04, 32);
		VEHICLE_DUMP_FIELD(vehHelicopterSoftCollisions);
		VEHICLE_DUMP_FIELD(vehHelicopterUseGroundFX);
		VEHICLE_DUMP_ASSET(vehHelicopterGroundFx);
		VEHICLE_DUMP_ASSET(vehHelicopterGroundWaterFx);
		VEHICLE_DUMP_VALUE_ARRAY(floatValues05, 78);
		VEHICLE_DUMP_STRING(unkString02);
		VEHICLE_DUMP_VALUE_ARRAY(floatValues06, 82);
		VEHICLE_DUMP_STRING(turretWeaponName);
		VEHICLE_DUMP_ASSET(turretWeapon);
		VEHICLE_DUMP_FIELD(turretHorizSpanLeft);
		VEHICLE_DUMP_FIELD(turretHorizSpanRight);
		VEHICLE_DUMP_FIELD(turretVertSpanUp);
		VEHICLE_DUMP_FIELD(turretVertSpanDown);
		VEHICLE_DUMP_FIELD(turretHorizResistLeft);
		VEHICLE_DUMP_FIELD(turretHorizResistRight);
		VEHICLE_DUMP_FIELD(turretVertResistUp);
		VEHICLE_DUMP_FIELD(turretVertResistDown);
		VEHICLE_DUMP_FIELD(turretRotRate);
		VEHICLE_DUMP_FIELD(turretFireType);
		VEHICLE_DUMP_ASSET(turretSpinSnd);
		VEHICLE_DUMP_FIELD(trophyEnabled);
		VEHICLE_DUMP_FIELD(trophyEnabled);
		VEHICLE_DUMP_FIELD(trophyRadius);
		VEHICLE_DUMP_FIELD(trophyInactiveRadius);
		VEHICLE_DUMP_FIELD(trophyAmmoCount);
		VEHICLE_DUMP_FIELD(trophyReloadTime);

		for (auto i = 0; i < 4; i++)
		{
			if (asset->trophyTags[i])
			{
				data["trophyTags"][i] = SL_ConvertToString(asset->trophyTags[i]);
			}
			else
			{
				data["trophyTags"][i] = "";
			}
		}

		VEHICLE_DUMP_ASSET(trophyExplodeFx);
		VEHICLE_DUMP_ASSET(trophyFlashFx);

		VEHICLE_DUMP_ASSET(compassFriendlyIcon);
		VEHICLE_DUMP_ASSET(compassEnemyIcon);
		VEHICLE_DUMP_ASSET(compassFriendlyAltIcon);
		VEHICLE_DUMP_ASSET(compassEnemyAltIcon);

		VEHICLE_DUMP_FIELD(compassIconWidth);
		VEHICLE_DUMP_FIELD(compassIconHeight);
		VEHICLE_DUMP_ASSET(idleLowSnd);
		VEHICLE_DUMP_ASSET(idleHighSnd);
		VEHICLE_DUMP_ASSET(engineLowSnd);
		VEHICLE_DUMP_ASSET(engineHighSnd);
		VEHICLE_DUMP_ASSET(sound01);
		VEHICLE_DUMP_FIELD(engineSndSpeed);

		if (asset->audioOriginTag)
		{
			data["audioOriginTag"] = SL_ConvertToString(asset->audioOriginTag);
		}
		else
		{
			data["audioOriginTag"] = "";
		}

		VEHICLE_DUMP_ASSET(idleLowSndAlt);
		VEHICLE_DUMP_ASSET(idleHighSndAlt);
		VEHICLE_DUMP_ASSET(engineLowSndAlt);
		VEHICLE_DUMP_ASSET(engineHighSndAlt);
		VEHICLE_DUMP_FIELD(engineSndSpeedAlt);

		if (asset->audioOriginTagAlt)
		{
			data["audioOriginTagAlt"] = SL_ConvertToString(asset->audioOriginTagAlt);
		}
		else
		{
			data["audioOriginTagAlt"] = "";
		}

		VEHICLE_DUMP_ASSET(turretSpinSndAlt);
		VEHICLE_DUMP_ASSET(turretStopSndAlt);
		VEHICLE_DUMP_ASSET(engineStartUpSnd);
		VEHICLE_DUMP_FIELD(engineStartUpLength);
		VEHICLE_DUMP_ASSET(engineShutdownSnd);
		VEHICLE_DUMP_ASSET(engineIdleSnd);
		VEHICLE_DUMP_ASSET(engineSustainSnd);
		VEHICLE_DUMP_ASSET(engineRampUpSnd);
		VEHICLE_DUMP_FIELD(engineRampUpLength);
		VEHICLE_DUMP_ASSET(engineRampDownSnd);
		VEHICLE_DUMP_FIELD(engineRampDownLength);
		VEHICLE_DUMP_ASSET(suspensionSoftSnd);
		VEHICLE_DUMP_FIELD(suspensionSoftCompression);
		VEHICLE_DUMP_ASSET(suspensionHardSnd);
		VEHICLE_DUMP_FIELD(suspensionHardCompression);
		VEHICLE_DUMP_ASSET(collisionSnd);
		VEHICLE_DUMP_FIELD(collisionBlendSpeed);
		VEHICLE_DUMP_ASSET(speedSnd);
		VEHICLE_DUMP_FIELD(speedSndBlendSpeed);
		VEHICLE_DUMP_STRING(surfaceSndPrefix);

		for (auto i = 0; i < 53; i++)
		{
			if (asset->surfaceSnds[i])
			{
				data["surfaceSnds"][i] = asset->surfaceSnds[i]->name;
			}
			else
			{
				data["surfaceSnds"][i] = "";
			}
		}

		VEHICLE_DUMP_FIELD(surfaceSndBlendSpeed);
		VEHICLE_DUMP_FIELD(slideVolume);
		VEHICLE_DUMP_FIELD(slideBlendSpeed);
		VEHICLE_DUMP_FIELD(inAirPitch);
		VEHICLE_DUMP_STRING(soundTriggerOverrideZone);
		VEHICLE_DUMP_FIELD(soundTriggerOverrideReverb);
		VEHICLE_DUMP_FIELD(soundTriggerOverrideMix);
		VEHICLE_DUMP_FIELD(soundTriggerOverrideFilter);
		VEHICLE_DUMP_FIELD(soundTriggerOverrideOcclusion);
		VEHICLE_DUMP_FIELD(soundTriggerOverrideAmbient);
		VEHICLE_DUMP_FIELD(soundTriggerOverrideAmbientEvents);
		VEHICLE_DUMP_FIELD(soundTriggerOverrideADSR);

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}