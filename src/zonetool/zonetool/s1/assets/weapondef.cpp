#include "std_include.hpp"
#include "weapondef.hpp"

namespace zonetool::s1
{
	constexpr const char* anim_names[NUM_WEAP_ANIMS] =
	{
		"ROOT",
		"IDLE",
		"EMPTY_IDLE",
		"FIRE",
		"HOLD_FIRE",
		"LASTSHOT",
		"RECHAMBER",
		"GRENADE_PRIME",
		"GRENADE_PRIME_READY_TO_THROW",
		"MELEE_SWIPE",
		"MELEE_HIT",
		"MELEE_FATAL",
		"MELEE_MISS",
		"MELEE_VICTIM_CROUCHING_HIT",
		"MELEE_VICTIM_CROUCHING_FATAL",
		"MELEE_VICTIM_CROUCHING_MISS",
		"MELEE_ALT_STANDING",
		"MELEE_ALT_CROUCHING",
		"MELEE_ALT_PRONE",
		"MELEE_ALT_JUMPING",
		"MELEE_ALT_STANDING_VICTIM_CROUCHING",
		"MELEE_ALT_CROUCHING_VICTIM_CROUCHING",
		"MELEE_ALT_PRONE_VICTIM_CROUCHING",
		"MELEE_ALT_JUMPING_VICTIM_CROUCHING",
		"RELOAD",
		"RELOAD_EMPTY",
		"RELOAD_START",
		"RELOAD_END",
		"FAST_RELOAD",
		"FAST_RELOAD_EMPTY",
		"FAST_RELOAD_START",
		"FAST_RELOAD_END",
		"DUALMAG_RELOAD",
		"DUALMAG_RELOAD_EMPTY",
		"SPEED_RELOAD",
		"RELOAD_FROM_ALT",
		"RAISE",
		"FIRST_RAISE",
		"BREACH_RAISE",
		"DROP",
		"ALT_RAISE",
		"ALT_DROP",
		"ALT_OVERRIDE",
		"QUICK_RAISE",
		"QUICK_DROP",
		"EMPTY_RAISE",
		"EMPTY_DROP",
		"HYBRID_SIGHT_ON",
		"HYBRID_SIGHT_OFF",
		"SPRINT_IN",
		"SPRINT_IN_FROM_SLIDE",
		"SPRINT_IN_CANCEL",
		"SPRINT_LOOP",
		"SPRINT_OUT",
		"SPRINTANDFIRE_IN",
		"SPRINTANDFIRE_LOOP",
		"SPRINTANDFIRE_OUT",
		"STUNNED_START",
		"STUNNED_LOOP",
		"STUNNED_END",
		"THROWBACK",
		"DETONATE",
		"NIGHTVISION_WEAR",
		"NIGHTVISION_REMOVE",
		"ADS_FIRE",
		"ADS_LASTSHOT",
		"ADS_RECHAMBER",
		"BLAST_FRONT",
		"BLAST_RIGHT",
		"BLAST_BACK",
		"BLAST_LEFT",
		"SLIDE_IN",
		"SLIDE_LOOP",
		"SLIDE_OUT_TO_SPRINT",
		"SLIDE_OUT",
		"SLIDE_AND_FIRE_IN",
		"SLIDE_AND_FIRE_LOOP",
		"SLIDE_AND_FIRE_OUT",
		"HIGH_JUMP_IN",
		"HIGH_JUMP_DROP_IN",
		"HIGH_JUMP_DROP_LOOP",
		"HIGH_JUMP_DROP_LAND",
		"DODGE_GROUND_BACK",
		"DODGE_GROUND_LEFT",
		"DODGE_GROUND_RIGHT",
		"DODGE_AIR_FORWARD",
		"DODGE_AIR_BACK",
		"DODGE_AIR_LEFT",
		"DODGE_AIR_RIGHT",
		"LAND_DIP",
		"RECOIL_SETTLE",
		"SWIM_LOOP",
		"MANTLE_UP_64",
		"MANTLE_UP_56",
		"MANTLE_UP_48",
		"MANTLE_UP_40",
		"MANTLE_UP_32",
		"MANTLE_UP_24",
		"MANTLE_UP_16",
		"MANTLE_OVER_64",
		"MANTLE_OVER_56",
		"MANTLE_OVER_48",
		"MANTLE_OVER_40",
		"MANTLE_OVER_32",
		"MANTLE_OVER_24",
		"MANTLE_OVER_16",
		"GOLIATH_ENTRY",
		"OFFHAND_SWITCH",
		"HEAT_COOLDOWN_IN",
		"HEAT_COOLDOWN_OUT",
		"OVERHEAT_OUT",
		"SCRIPTED",
		"RELOAD_MULTIPLE_1",
		"RELOAD_MULTIPLE_2",
		"RELOAD_MULTIPLE_3",
		"RELOAD_MULTIPLE_4",
		"RELOAD_MULTIPLE_5",
		"RELOAD_MULTIPLE_6",
		"RELOAD_MULTIPLE_7",
		"RELOAD_MULTIPLE_8",
		"RELOAD_MULTIPLE_FAST_1",
		"RELOAD_MULTIPLE_FAST_2",
		"RELOAD_MULTIPLE_FAST_3",
		"RELOAD_MULTIPLE_FAST_4",
		"RELOAD_MULTIPLE_FAST_5",
		"RELOAD_MULTIPLE_FAST_6",
		"RELOAD_MULTIPLE_FAST_7",
		"RELOAD_MULTIPLE_FAST_8",
		"ADS_UP",
		"ADS_DOWN",
		"RECOIL",
		"WEAP_ALT_ANIM_ADJUST",
		"ADDITIVE_ADS_ROOT",
		"ADDITIVE_ADS_UP",
		"ADDITIVE_HYBRID_SIGHT_UP_ROOT",
		"ADDITIVE_HYBRID_SIGHT_UP",
		"ADDITIVE_DRAG_LEFT_ROOT",
		"ADDITIVE_DRAG_LEFT",
		"ADDITIVE_DRAG_RIGHT_ROOT",
		"ADDITIVE_DRAG_RIGHT",
		"ADDITIVE_DRAG_UP_ROOT",
		"ADDITIVE_DRAG_UP",
		"ADDITIVE_DRAG_DOWN_ROOT",
		"ADDITIVE_DRAG_DOWN",
		"ADDITIVE_SWIM_FORWARD_ROOT",
		"ADDITIVE_SWIM_FORWARD",
		"ADDITIVE_SWIM_BACKWARD_ROOT",
		"ADDITIVE_SWIM_BACKWARD",
		"ADDITIVE_JUMP_ROOT",
		"ADDITIVE_JUMP",
		"ADDITIVE_JUMP_BOOST",
		"ADDITIVE_JUMP_LAND_ROOT",
		"ADDITIVE_JUMP_LAND",
		"ADDITIVE_JUMP_LAND_HEAVY",
		"ADDITIVE_WALK_ROOT",
		"ADDITIVE_WALK",
		"ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT",
		"ADDITIVE_HEAT_COOLDOWN_LOOP",
		"ADDITIVE_OVERHEAT_IN_ROOT",
		"ADDITIVE_OVERHEAT_IN",
		"ADDITIVE_OVERHEAT_LOOP_ROOT",
		"ADDITIVE_OVERHEAT_LOOP",
		"ADDITIVE_CRAWL_IN_ROOT",
		"ADDITIVE_CRAWL_IN",
		"ADDITIVE_CRAWL_LOOP_ROOT",
		"ADDITIVE_CRAWL_LOOP",
		"ADDITIVE_CRAWL_LOOP_LEFT_ROOT",
		"ADDITIVE_CRAWL_LOOP_LEFT",
		"ADDITIVE_CRAWL_LOOP_RIGHT_ROOT",
		"ADDITIVE_CRAWL_LOOP_RIGHT",
		"ADDITIVE_PRONE_DROP_ROOT",
		"ADDITIVE_PRONE_DROP",
		"ADDITIVE_EMPTY_ROOT",
		"ADDITIVE_EMPTY",
		"ADDITIVE_SHOT_CHARGE_IN_ROOT",
		"ADDITIVE_SHOT_CHARGE_IN",
		"ADDITIVE_SHOT_CHARGE_LOOP_ROOT",
		"ADDITIVE_SHOT_CHARGE_LOOP",
		"ADDITIVE_SCRIPTED_ROOT",
		"ADDITIVE_SCRIPTED"
	};

	const char* get_anim_name_from_index(weapAnimFiles_t index)
	{
		return anim_names[index];
	}

	const char* get_anim_name_from_index(int index)
	{
		return anim_names[index];
	}

	weapAnimFiles_t get_anim_index_from_name(const char* name)
	{
		for (unsigned int i = 0; i < NUM_WEAP_ANIMS; i++)
		{
			const char* anim_name = anim_names[i];
			if (!_stricmp(anim_name, name))
			{
				return static_cast<weapAnimFiles_t>(i);
			}
		}

		ZONETOOL_FATAL("Invalid animation index returned for name: \"%s\"", name);
		//return WEAP_ANIM_INVALID;
	}

	void weapon_def::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* weapon_def::get_script_string(scr_string_t* ptr)
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

#define WEAPON_READ_FIELD(__type__, __field__) \
	if (!data[#__field__].is_null()) *(__type__*)(&weapon->__field__) = (__type__)data[#__field__].get<__type__>()

#define WEAPON_READ_FIELD_ARR(__type__, __field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		if constexpr (std::is_pointer<decltype(weapon->__field__)>::value) \
		{ \
			*reinterpret_cast<__type__**>(reinterpret_cast<size_t>(&weapon->__field__)) = mem->allocate<__type__>(__size__); \
		} \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			*(__type__*)(&weapon->__field__[idx##__field__]) = (__type__)data[#__field__][idx##__field__].get<__type__>(); \
		} \
	}

#define WEAPON_READ_STRING(__field__) \
	if (!data[#__field__].is_null()) weapon->__field__ = mem->duplicate_string(data[#__field__].get<std::string>())

#define WEAPON_READ_ASSET(__type__, __datafield__, __field__) \
	if (!data[#__field__].is_null() && data[#__field__].is_string()) \
	{ \
		auto asset##__field__ = data[#__field__].get<std::string>(); \
		if (asset##__field__.empty()) \
		{ \
			weapon->__field__ = nullptr; \
		} \
		else \
		{ \
			weapon->__field__ = db_find_x_asset_header(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
		} \
	} \
	else \
	{ \
		weapon->__field__ = nullptr; \
	}

#define WEAPON_READ_ASSET_ARR(__type__, __datafield__, __field__, __struct__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		weapon->__field__ = mem->allocate<__struct__*>(__size__); \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto asset##__field__ = data[#__field__][idx##__field__].get<std::string>(); \
			if (asset##__field__.empty()) \
			{ \
				weapon->__field__[idx##__field__] = nullptr; \
			} \
			else \
			{ \
				weapon->__field__[idx##__field__] = db_find_x_asset_header(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
			} \
		} \
	} \
	else \
	{ \
		weapon->__field__ = nullptr; \
	}

#define WEAPON_READ_ANIM_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		weapon->__field__ = mem->allocate<XAnimParts*>(__size__); \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto asset##__field__ = data[#__field__][get_anim_name_from_index(idx##__field__)].get<std::string>(); \
			if (asset##__field__.empty()) \
			{ \
				weapon->__field__[idx##__field__] = nullptr; \
			} \
			else \
			{ \
				weapon->__field__[idx##__field__] = db_find_x_asset_header(XAssetType::ASSET_TYPE_XANIMPARTS, asset##__field__.data(), 1).parts; \
			} \
		} \
	} \
	else \
	{ \
		weapon->__field__ = nullptr; \
	}

#define WEAPON_READ_SOUND(__field__) \
	if (!data["sounds"][#__field__].is_null() && data["sounds"][#__field__].is_string()) \
	{ \
		auto asset##__field__ = data["sounds"][#__field__].get<std::string>(); \
		if (asset##__field__.empty()) \
		{ \
			weapon->__field__ = nullptr; \
		} \
		else \
		{ \
			weapon->__field__ = db_find_x_asset_header(XAssetType::ASSET_TYPE_SOUND, asset##__field__.data(), 1).sound; \
		} \
	} \
	else \
	{ \
		weapon->__field__ = nullptr; \
	}

	void parse_overlay(ADSOverlay* weapon, json& data)
	{
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, shader);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, shaderLowRes);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, shaderEMP);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, shaderEMPLowRes);
		WEAPON_READ_FIELD(int, reticle);
		WEAPON_READ_FIELD(float, width);
		WEAPON_READ_FIELD(float, height);
		WEAPON_READ_FIELD(float, widthSplitscreen);
		WEAPON_READ_FIELD(float, heightSplitscreen);
	}

	void parse_turret_hydraulic_settings(TurretHydraulicSettings* settings, json& data, zone_memory* mem)
	{
		settings->minVelocity = data["minVelocity"].is_number() ? data["minVelocity"].get<float>() : 0.0f;
		settings->maxVelocity = data["maxVelocity"].is_number() ? data["maxVelocity"].get<float>() : 0.0f;

		if (!data["verticalSound"].is_null())
		{
			auto string = data["verticalSound"].get<std::string>();
			if (!string.empty())
			{
				settings->verticalSound = mem->allocate<snd_alias_list_t>();
				settings->verticalSound->name = mem->duplicate_string(string);
			}
		}

		if (!data["verticalStopSound"].is_null())
		{
			auto string = data["verticalStopSound"].get<std::string>();
			if (!string.empty())
			{
				settings->verticalStopSound = mem->allocate<snd_alias_list_t>();
				settings->verticalStopSound->name = mem->duplicate_string(string);
			}
		}

		if (!data["horizontalSound"].is_null())
		{
			auto string = data["horizontalSound"].get<std::string>();
			if (!string.empty())
			{
				settings->horizontalSound = mem->allocate<snd_alias_list_t>();
				settings->horizontalSound->name = mem->duplicate_string(string);
			}
		}

		if (!data["horizontalStopSound"].is_null())
		{
			auto string = data["horizontalStopSound"].get<std::string>();
			if (!string.empty())
			{
				settings->horizontalStopSound = mem->allocate<snd_alias_list_t>();
				settings->horizontalStopSound->name = mem->duplicate_string(string);
			}
		}
	}

	void parse_accuracy_graph(WeaponDef* def, json& data, zone_memory* mem)
	{
		for (auto i = 0; i < 2; i++)
		{
			def->accuracyGraphName[i] = data["accuracyGraphName"][i].is_string()
				? mem->duplicate_string(data["accuracyGraphName"][i].get<std::string>())
				: nullptr;

			if (data["accuracyGraphKnots"][i].is_array())
			{
				const auto count = data["accuracyGraphKnots"][i].size();
				def->accuracyGraphKnots[i] = mem->allocate<vec2_t>(count);
				def->accuracyGraphKnotCount[i] = static_cast<short>(count);

				for (auto o = 0; o < count; o++)
				{
					def->accuracyGraphKnots[i][o][0] = data["accuracyGraphKnots"][i][o][0].get<float>();
					def->accuracyGraphKnots[i][o][1] = data["accuracyGraphKnots"][i][o][1].get<float>();
				}
			}

			if (data["originalAccuracyGraphKnots"][i].is_array())
			{
				const auto count = data["originalAccuracyGraphKnots"][i].size();
				def->originalAccuracyGraphKnots[i] = mem->allocate<vec2_t>(count);
				def->accuracyGraphKnotCount[i] = static_cast<short>(count);

				for (auto o = 0; o < count; o++)
				{
					def->originalAccuracyGraphKnots[i][o][0] = data["originalAccuracyGraphKnots"][i][o][0].get<float>();
					def->originalAccuracyGraphKnots[i][o][1] = data["originalAccuracyGraphKnots"][i][o][1].get<float>();
				}
			}
		}
	}
	
	void parse_statetimers(StateTimers* weapon, json& data)
	{
		WEAPON_READ_FIELD(int, fireDelay);
		WEAPON_READ_FIELD(int, meleeDelay);
		WEAPON_READ_FIELD(int, meleeChargeDelay);
		WEAPON_READ_FIELD(int, detonateDelay);
		WEAPON_READ_FIELD(int, fireTime);
		WEAPON_READ_FIELD(int, rechamberTime);
		WEAPON_READ_FIELD(int, rechamberTimeOneHanded);
		WEAPON_READ_FIELD(int, rechamberBoltTime);
		WEAPON_READ_FIELD(int, holdFireTime);
		WEAPON_READ_FIELD(int, grenadePrimeReadyToThrowTime);
		WEAPON_READ_FIELD(int, detonateTime);
		WEAPON_READ_FIELD(int, meleeTime);
		WEAPON_READ_FIELD(int, meleeChargeTime);
		WEAPON_READ_FIELD(int, reloadTime);
		WEAPON_READ_FIELD(int, reloadShowRocketTime);
		WEAPON_READ_FIELD(int, reloadEmptyTime);
		WEAPON_READ_FIELD(int, reloadAddTime);
		WEAPON_READ_FIELD(int, reloadEmptyAddTime);
		WEAPON_READ_FIELD(int, reloadStartTime);
		WEAPON_READ_FIELD(int, reloadStartAddTime);
		WEAPON_READ_FIELD(int, reloadEndTime);
		WEAPON_READ_FIELD(int, reloadTimeDualWield);
		WEAPON_READ_FIELD(int, reloadAddTimeDualWield);
		WEAPON_READ_FIELD(int, reloadEmptyDualMag);
		WEAPON_READ_FIELD(int, reloadEmptyAddTimeDualMag);
		//WEAPON_READ_FIELD(int, u25);
		//WEAPON_READ_FIELD(int, u26);
		WEAPON_READ_FIELD(int, dropTime);
		WEAPON_READ_FIELD(int, raiseTime);
		WEAPON_READ_FIELD(int, altDropTime);
		WEAPON_READ_FIELD(int, altRaiseTime);
		WEAPON_READ_FIELD(int, quickDropTime);
		WEAPON_READ_FIELD(int, quickRaiseTime);
		WEAPON_READ_FIELD(int, firstRaiseTime);
		WEAPON_READ_FIELD(int, breachRaiseTime);
		WEAPON_READ_FIELD(int, emptyRaiseTime);
		WEAPON_READ_FIELD(int, emptyDropTime);
		WEAPON_READ_FIELD(int, sprintInTime);
		WEAPON_READ_FIELD(int, sprintLoopTime);
		WEAPON_READ_FIELD(int, sprintOutTime);
		WEAPON_READ_FIELD(int, stunnedTimeBegin);
		WEAPON_READ_FIELD(int, stunnedTimeLoop);
		WEAPON_READ_FIELD(int, stunnedTimeEnd);
		WEAPON_READ_FIELD(int, nightVisionWearTime);
		WEAPON_READ_FIELD(int, nightVisionWearTimeFadeOutEnd);
		WEAPON_READ_FIELD(int, nightVisionWearTimePowerUp);
		WEAPON_READ_FIELD(int, nightVisionRemoveTime);
		WEAPON_READ_FIELD(int, nightVisionRemoveTimePowerDown);
		WEAPON_READ_FIELD(int, nightVisionRemoveTimeFadeInStart);
		WEAPON_READ_FIELD(int, aiFuseTime);
		WEAPON_READ_FIELD(int, fuseTime);
		WEAPON_READ_FIELD(int, missileTime);
		WEAPON_READ_FIELD(int, primeTime);
		WEAPON_READ_FIELD(bool, bHoldFullPrime);
		WEAPON_READ_FIELD(int, blastFrontTime);
		WEAPON_READ_FIELD(int, blastRightTime);
		WEAPON_READ_FIELD(int, blastBackTime);
		WEAPON_READ_FIELD(int, blastLeftTime);
		//WEAPON_READ_FIELD(int, u58);
		//WEAPON_READ_FIELD(int, u59);
		//WEAPON_READ_FIELD(int, u60);
		//WEAPON_READ_FIELD(int, u61);
		//WEAPON_READ_FIELD(int, u62);
		//WEAPON_READ_FIELD(int, u63);
		//WEAPON_READ_FIELD(int, u64);
		//WEAPON_READ_FIELD(int, u65);
		//WEAPON_READ_FIELD(int, u66);
		//WEAPON_READ_FIELD(int, u67);
		//WEAPON_READ_FIELD(int, u68);
		WEAPON_READ_FIELD(int, offhandSwitchTime);
		//WEAPON_READ_FIELD(int, u70);
		//WEAPON_READ_FIELD(int, u71);
		//WEAPON_READ_FIELD(int, u72);
		//WEAPON_READ_FIELD(int, u73);
		//WEAPON_READ_FIELD(int, u74);
	}

	WeaponDef* weapon_def::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "weapons\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing weapon \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		json data = json::parse(bytes);

		auto* weapon = mem->allocate<WeaponDef>();

		// base asset
		auto base = data["baseAsset"].get<std::string>();
		WeaponDef* baseAsset = nullptr;
		if (!base.empty())
		{
			baseAsset = db_find_x_asset_header(ASSET_TYPE_WEAPON, base.data(), 0).weapon;
			if (baseAsset == nullptr || DB_IsXAssetDefault(ASSET_TYPE_WEAPON, base.data()))
			{
				ZONETOOL_FATAL("Could not load base asset \"%s\" into memory...", base.data());
			}
			memcpy(weapon, baseAsset, sizeof(WeaponDef));
		}
		else
		{
			ZONETOOL_WARNING("No base asset is defined for weapon \"%s\", stuff might go wrong!", name.data());
		}

		WEAPON_READ_STRING(szInternalName);
		WEAPON_READ_STRING(szDisplayName);
		WEAPON_READ_STRING(szOverlayName);

		WEAPON_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, gunModel, XModel, 2);
		WEAPON_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, worldModel, XModel, 2);

		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, camoWorldModelMaterialOverride);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, camoViewModelMaterialOverride);

		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, handModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, unknownModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, worldClipModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, rocketModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, knifeModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, worldKnifeModel);

		WEAPON_READ_ANIM_ARR(szXAnimsRightHanded, 180);
		WEAPON_READ_ANIM_ARR(szXAnimsLeftHanded, 180);
		WEAPON_READ_ANIM_ARR(szXAnims, 180);

		weapon->hideTags = mem->allocate<scr_string_t>(32);
		for (auto i = 0; i < 32; i++)
		{
			this->add_script_string(&weapon->hideTags[i], mem->duplicate_string(data["hideTags"][i].get<std::string>()));
		}

		weapon->numWeaponAttachments = static_cast<unsigned char>(data["attachments"].size());
		if (weapon->numWeaponAttachments)
		{
			weapon->attachments = mem->allocate<WeaponAttachment*>(weapon->numWeaponAttachments);
			for (auto i = 0; i < weapon->numWeaponAttachments; i++)
			{
				if (!data["attachments"][i].is_null())
				{
					weapon->attachments[i] = mem->allocate<WeaponAttachment>();
					weapon->attachments[i]->name = mem->duplicate_string(data["attachments"][i].get<std::string>());
				}
			}
		}

		weapon->numAnimOverrides = static_cast<unsigned char>(data["animOverrides"].size());
		if (weapon->numAnimOverrides)
		{
			weapon->animOverrides = mem->allocate<AnimOverrideEntry>(weapon->numAnimOverrides);
			for (auto i = 0u; i < weapon->numAnimOverrides; i++)
			{
				auto altmodeAnim = data["animOverrides"][i]["altmodeAnim"].get<std::string>();
				if (!altmodeAnim.empty())
				{
					weapon->animOverrides[i].altmodeAnim = db_find_x_asset_header(ASSET_TYPE_XANIMPARTS, altmodeAnim.data(), 1).parts;
				}
				auto overrideAnim = data["animOverrides"][i]["overrideAnim"].get<std::string>();
				if (!overrideAnim.empty())
				{
					weapon->animOverrides[i].overrideAnim = db_find_x_asset_header(ASSET_TYPE_XANIMPARTS, overrideAnim.data(), 1).parts;
				}
				weapon->animOverrides[i].attachment1 = data["animOverrides"][i]["attachment1"].get<unsigned short>();
				weapon->animOverrides[i].attachment2 = data["animOverrides"][i]["attachment2"].get<unsigned short>();
				weapon->animOverrides[i].altTime = data["animOverrides"][i]["altTime"].get<int>();
				weapon->animOverrides[i].animTime = data["animOverrides"][i]["animTime"].get<int>();
				//weapon->animOverrides[i].animTreeType = data["animOverrides"][i]["animTreeType"].get<unsigned int>();
			}
		}

		weapon->numSoundOverrides = static_cast<unsigned char>(data["soundOverrides"].size());
		if (weapon->numSoundOverrides)
		{
			weapon->soundOverrides = mem->allocate<SoundOverrideEntry>(weapon->numSoundOverrides);
			for (auto i = 0u; i < weapon->numSoundOverrides; i++)
			{
				auto altmodeSound = data["soundOverrides"][i]["altmodeSound"].get<std::string>();
				if (!altmodeSound.empty())
				{
					weapon->soundOverrides[i].altmodeSound = db_find_x_asset_header(ASSET_TYPE_SOUND, altmodeSound.data(), 1).sound;
				}
				auto overrideSound = data["soundOverrides"][i]["overrideSound"].get<std::string>();
				if (!overrideSound.empty())
				{
					weapon->soundOverrides[i].overrideSound = db_find_x_asset_header(ASSET_TYPE_SOUND, overrideSound.data(), 1).sound;
				}
				weapon->soundOverrides[i].attachment1 = data["soundOverrides"][i]["attachment1"].get<unsigned short>();
				weapon->soundOverrides[i].attachment2 = data["soundOverrides"][i]["attachment2"].get<unsigned short>();
				//weapon->soundOverrides[i].soundType = data["soundOverrides"][i]["soundType"].get<unsigned int>();
			}
		}

		weapon->numFXOverrides = static_cast<unsigned char>(data["fxOverrides"].size());
		if (weapon->numFXOverrides)
		{
			weapon->fxOverrides = mem->allocate<FXOverrideEntry>(weapon->numFXOverrides);
			for (auto i = 0u; i < weapon->numFXOverrides; i++)
			{
				auto altmodeFX = data["fxOverrides"][i]["altmodeFX"].get<std::string>();
				if (!altmodeFX.empty())
				{
					weapon->fxOverrides[i].altmodeFX = db_find_x_asset_header(ASSET_TYPE_FX, altmodeFX.data(), 1).fx;
				}
				auto overrideFX = data["fxOverrides"][i]["overrideFX"].get<std::string>();
				if (!overrideFX.empty())
				{
					weapon->fxOverrides[i].overrideFX = db_find_x_asset_header(ASSET_TYPE_SOUND, overrideFX.data(), 1).fx;
				}
				weapon->fxOverrides[i].attachment1 = data["fxOverrides"][i]["attachment1"].get<unsigned short>();
				weapon->fxOverrides[i].attachment2 = data["fxOverrides"][i]["attachment2"].get<unsigned short>();
				//weapon->fxOverrides[i].fxType = data["fxOverrides"][i]["fxType"].get<unsigned int>();
			}
		}

		weapon->numReloadStateTimerOverrides = static_cast<unsigned char>(data["reloadOverrides"].size());
		if (weapon->numReloadStateTimerOverrides)
		{
			weapon->reloadOverrides = mem->allocate<ReloadStateTimerEntry>(weapon->numReloadStateTimerOverrides);
			for (auto i = 0u; i < weapon->numReloadStateTimerOverrides; i++)
			{
				weapon->reloadOverrides[i].attachment = data["reloadOverrides"][i]["attachment"].get<int>();
				weapon->reloadOverrides[i].reloadAddTime = data["reloadOverrides"][i]["reloadAddTime"].get<int>();
				weapon->reloadOverrides[i].reloadEmptyAddTime = data["reloadOverrides"][i]["reloadEmptyAddTime"].get<int>();
				weapon->reloadOverrides[i].reloadStartAddTime = data["reloadOverrides"][i]["reloadStartAddTime"].get<int>();
			}
		}

		weapon->numNotetrackOverrides = static_cast<unsigned char>(data["notetrackOverrides"].size());
		if (weapon->numNotetrackOverrides)
		{
			weapon->notetrackOverrides = mem->allocate<NoteTrackToSoundEntry>(weapon->numNotetrackOverrides);
			for (auto i = 0u; i < weapon->numNotetrackOverrides; i++)
			{
				weapon->notetrackOverrides[i].attachment = data["notetrackOverrides"][i]["attachment"].get<int>();
				for (auto j = 0u; j < 36; j++)
				{
					auto notetrackSoundMapKey = data["notetrackOverrides"][i]["notetrackSoundMapKeys"][j].get<std::string>();
					if (!notetrackSoundMapKey.empty())
					{
						this->add_script_string(&weapon->notetrackOverrides[i].notetrackSoundMapKeys[j], mem->duplicate_string(notetrackSoundMapKey));
					}
					auto notetrackSoundMapValue = data["notetrackOverrides"][i]["notetrackSoundMapValues"][j].get<std::string>();
					if (!notetrackSoundMapValue.empty())
					{
						this->add_script_string(&weapon->notetrackOverrides[i].notetrackSoundMapValues[j], mem->duplicate_string(notetrackSoundMapValue));
					}
				}
			}
		}

		weapon->notetrackSoundMapKeys = mem->allocate<scr_string_t>(36);
		weapon->notetrackSoundMapValues = mem->allocate<scr_string_t>(36);
		for (auto i = 0; i < 36; i++)
		{
			auto notetrack = data["notetrackSoundMapKeys"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackSoundMapKeys[i], mem->duplicate_string(notetrack));
		}
		for (auto i = 0; i < 36; i++)
		{
			auto notetrack = data["notetrackSoundMapValues"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackSoundMapValues[i], mem->duplicate_string(notetrack));
		}

		weapon->notetrackRumbleMapKeys = mem->allocate<scr_string_t>(16);
		weapon->notetrackRumbleMapValues = mem->allocate<scr_string_t>(16);
		for (auto i = 0; i < 16; i++)
		{
			auto notetrack = data["notetrackRumbleMapKeys"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackRumbleMapKeys[i], mem->duplicate_string(notetrack));
		}
		for (auto i = 0; i < 16; i++)
		{
			auto notetrack = data["notetrackRumbleMapValues"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackRumbleMapValues[i], mem->duplicate_string(notetrack));
		}

		weapon->notetrackFXMapKeys = mem->allocate<scr_string_t>(16);
		weapon->notetrackFXMapTagValues = mem->allocate<scr_string_t>(16);
		for (auto i = 0; i < 16; i++)
		{
			auto notetrack = data["notetrackFXMapKeys"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackFXMapKeys[i], mem->duplicate_string(notetrack));
		}
		for (auto i = 0; i < 16; i++)
		{
			auto notetrack = data["notetrackFXMapTagValues"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackFXMapTagValues[i], mem->duplicate_string(notetrack));
		}

		weapon->notetrackFXMapValues = mem->allocate<FxEffectDef*>(16);
		for (auto i = 0; i < 16; i++)
		{
			auto notetrack = data["notetrackFXMapValues"][i].get<std::string>();
			weapon->notetrackFXMapValues[i] = db_find_x_asset_header(ASSET_TYPE_FX, notetrack.data(), 1).fx;
		}

		WEAPON_READ_STRING(szModeName);
		WEAPON_READ_STRING(szAltWeaponName);
		WEAPON_READ_STRING(szAmmoName);
		WEAPON_READ_STRING(szClipName);
		WEAPON_READ_STRING(szSharedAmmoCapName);

		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewFlashEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewBodyFlashEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, worldFlashEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewFlashADSEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewBodyFlashADSEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, effect06);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, effect07);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, effect08);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, effect09);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, effect10);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, effect11);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, effect12);

		WEAPON_READ_SOUND(pickupSound);
		WEAPON_READ_SOUND(pickupSoundPlayer);
		WEAPON_READ_SOUND(ammoPickupSound);
		WEAPON_READ_SOUND(ammoPickupSoundPlayer);
		WEAPON_READ_SOUND(projectileSound);
		WEAPON_READ_SOUND(pullbackSound);
		WEAPON_READ_SOUND(pullbackSoundPlayer);
		WEAPON_READ_SOUND(pullbackSoundQuick);
		WEAPON_READ_SOUND(pullbackSoundQuickPlayer);
		WEAPON_READ_SOUND(fireSound);
		WEAPON_READ_SOUND(fireSoundPlayer);
		WEAPON_READ_SOUND(fireSoundPlayerAkimbo);
		WEAPON_READ_SOUND(sound13);
		WEAPON_READ_SOUND(sound14);
		WEAPON_READ_SOUND(sound15);
		WEAPON_READ_SOUND(sound16);
		WEAPON_READ_SOUND(fireLoopSound);
		WEAPON_READ_SOUND(fireLoopSoundPlayer);
		WEAPON_READ_SOUND(sound19);
		WEAPON_READ_SOUND(sound20);
		WEAPON_READ_SOUND(sound21);
		WEAPON_READ_SOUND(sound22);
		WEAPON_READ_SOUND(fireLoopEndPointSound);
		WEAPON_READ_SOUND(fireLoopEndPointSoundPlayer);
		WEAPON_READ_SOUND(fireStopSound);
		WEAPON_READ_SOUND(fireStopSoundPlayer);
		WEAPON_READ_SOUND(sound27);
		WEAPON_READ_SOUND(sound28);
		WEAPON_READ_SOUND(sound29);
		WEAPON_READ_SOUND(sound30);
		WEAPON_READ_SOUND(fireLastShotSound);
		WEAPON_READ_SOUND(fireLastShotSoundPlayer);
		WEAPON_READ_SOUND(fireFirstSound);
		WEAPON_READ_SOUND(fireFirstSound);
		WEAPON_READ_SOUND(fireFirstSoundPlayer);
		WEAPON_READ_SOUND(fireLastSound);
		WEAPON_READ_SOUND(fireLastSoundPlayer);
		WEAPON_READ_SOUND(emptyFireSound);
		WEAPON_READ_SOUND(emptyFireSoundPlayer);
		WEAPON_READ_SOUND(sound39);
		WEAPON_READ_SOUND(meleeSwipeSound);
		WEAPON_READ_SOUND(meleeSwipeSoundPlayer);
		WEAPON_READ_SOUND(meleeHitSound);
		WEAPON_READ_SOUND(meleeHitSoundPlayer);
		WEAPON_READ_SOUND(meleeMissSound);
		WEAPON_READ_SOUND(meleeMissSoundPlayer);
		WEAPON_READ_SOUND(rechamberSound);
		WEAPON_READ_SOUND(rechamberSoundPlayer);
		WEAPON_READ_SOUND(reloadSound);
		WEAPON_READ_SOUND(reloadSoundPlayer);
		WEAPON_READ_SOUND(reloadEmptySound);
		WEAPON_READ_SOUND(reloadEmptySoundPlayer);
		WEAPON_READ_SOUND(reloadStartSound);
		WEAPON_READ_SOUND(reloadStartSoundPlayer);
		WEAPON_READ_SOUND(reloadEndSound);
		WEAPON_READ_SOUND(reloadEndSoundPlayer);
		WEAPON_READ_SOUND(detonateSound);
		WEAPON_READ_SOUND(detonateSoundPlayer);
		WEAPON_READ_SOUND(nightVisionWearSound);
		WEAPON_READ_SOUND(nightVisionWearSoundPlayer);
		WEAPON_READ_SOUND(nightVisionRemoveSound);
		WEAPON_READ_SOUND(nightVisionRemoveSoundPlayer);
		WEAPON_READ_SOUND(raiseSound);
		WEAPON_READ_SOUND(raiseSoundPlayer);
		WEAPON_READ_SOUND(sound64);
		WEAPON_READ_SOUND(sound65);
		WEAPON_READ_SOUND(sound66);
		WEAPON_READ_SOUND(sound67);
		WEAPON_READ_SOUND(putawaySound);
		WEAPON_READ_SOUND(putawaySoundPlayer);
		WEAPON_READ_SOUND(sound70);
		WEAPON_READ_SOUND(sound71);
		WEAPON_READ_SOUND(adsEnterSoundPlayer);
		WEAPON_READ_SOUND(adsLeaveSoundPlayer);
		WEAPON_READ_SOUND(adsCrosshairEnemySound);

		WEAPON_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, bounceSound, snd_alias_list_t, 53);
		WEAPON_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, rollingSound, snd_alias_list_t, 53);

		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewShellEjectEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, worldShellEjectEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewLastShotEjectEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, worldLastShotEjectEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewMagEjectEffect);

		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, reticleCenter);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, reticleSide);

		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, hudIcon);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, pickupIcon);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, unknownIcon2);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, unknownIcon3);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, unknownIcon4);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, ammoCounterIcon);

		WEAPON_READ_ASSET(ASSET_TYPE_PHYSCOLLMAP, physCollmap, physCollmap);
		WEAPON_READ_ASSET(ASSET_TYPE_PHYSPRESET, physPreset, physPreset);

		WEAPON_READ_STRING(szUseHintString);
		WEAPON_READ_STRING(dropHintString);

		WEAPON_READ_FIELD_ARR(float, locationDamageMultipliers, 22);

		WEAPON_READ_STRING(fireRumble);
		WEAPON_READ_STRING(fireMedRumble);
		WEAPON_READ_STRING(fireHighRumble);
		WEAPON_READ_STRING(meleeImpactRumble);

		WEAPON_READ_ASSET(ASSET_TYPE_TRACER, tracerDef, tracer1);
		WEAPON_READ_ASSET(ASSET_TYPE_TRACER, tracerDef, tracer2);

		WEAPON_READ_ASSET(ASSET_TYPE_LASER, laser, laser);

		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, turretOverheatSound);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, turretOverheatEffect);
		WEAPON_READ_STRING(turretBarrelSpinRumble);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, turretBarrelSpinMaxSnd);
		for (auto i = 0; i < 4; i++)
		{
			auto sound = data["turretBarrelSpinUpSnd"][i].get<std::string>();
			if (!sound.empty())
			{
				weapon->turretBarrelSpinUpSnd[i] = db_find_x_asset_header(ASSET_TYPE_SOUND, sound.data(), 1).sound;
			}
		}
		for (auto i = 0; i < 4; i++)
		{
			auto sound = data["turretBarrelSpinDownSnd"][i].get<std::string>();
			if (!sound.empty())
			{
				weapon->turretBarrelSpinDownSnd[i] = db_find_x_asset_header(ASSET_TYPE_SOUND, sound.data(), 1).sound;
			}
		}
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, missileConeSoundAlias);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, missileConeSoundAliasAtBase);

		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, stowOffsetModel);

		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, killIcon);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, dpadIcon);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, hudProximityWarningIcon);

		WEAPON_READ_STRING(projectileName);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, projectileModel);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projExplosionEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projDudEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, projExplosionSound);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, projDudSound);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projTrailEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projBeaconEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projIgnitionEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, projIgnitionSound);

		WEAPON_READ_STRING(szScript);

		if (!data["turretHydraulicSettings"].is_null())
		{
			weapon->turretHydraulicSettings = mem->allocate<TurretHydraulicSettings>();
			parse_turret_hydraulic_settings(weapon->turretHydraulicSettings, data["turretHydraulicSettings"], mem);
		}

		parse_statetimers(&weapon->stateTimers, data["stateTimers"]);
		parse_statetimers(&weapon->akimboStateTimers, data["stateTimersAkimbo"]);

		parse_overlay(&weapon->overlay, data["overlay"]);

		parse_accuracy_graph(weapon, data["accuracy_graph"], mem);

		// parse stowtag
		if (!data["stowTag"].is_null())
		{
			auto stowTag = data["stowTag"].get<std::string>();
			this->add_script_string(&weapon->stowTag, mem->duplicate_string(stowTag));
		}

		WEAPON_READ_FIELD(int, altWeapon);
		WEAPON_READ_FIELD(playerAnimType_t, playerAnimType);
		WEAPON_READ_FIELD(weapType_t, weapType);
		WEAPON_READ_FIELD(weapClass_t, weapClass);
		WEAPON_READ_FIELD(PenetrateType, penetrateType);
		WEAPON_READ_FIELD(float, penetrateDepth);
		WEAPON_READ_FIELD(ImpactType, impactType);
		WEAPON_READ_FIELD(weapInventoryType_t, inventoryType);
		WEAPON_READ_FIELD(weapFireType_t, fireType);
		WEAPON_READ_FIELD(weapFireBarrels_t, fireBarrels);
		WEAPON_READ_FIELD(weapAdsFireMode_t, adsFireMode);
		WEAPON_READ_FIELD(float, burstFireCooldown);
		WEAPON_READ_FIELD(weapGreebleType_t, greebleType);
		WEAPON_READ_FIELD(weapAutoReloadType_t, autoReloadType);
		WEAPON_READ_FIELD(WeaponSlotRestriction, slotRestriction);
		WEAPON_READ_FIELD(OffhandClass, offhandClass);
		WEAPON_READ_FIELD(weapStance_t, stance);
		WEAPON_READ_FIELD(int, reticleCenterSize);
		WEAPON_READ_FIELD(int, reticleSideSize);
		WEAPON_READ_FIELD(int, reticleMinOfs);
		WEAPON_READ_FIELD(activeReticleType_t, activeReticleType);
		WEAPON_READ_FIELD_ARR(float, standMove, 3);
		WEAPON_READ_FIELD_ARR(float, standRot, 3);
		WEAPON_READ_FIELD_ARR(float, strafeMove, 3);
		WEAPON_READ_FIELD_ARR(float, strafeRot, 3);
		WEAPON_READ_FIELD_ARR(float, duckedOfs, 3);
		WEAPON_READ_FIELD_ARR(float, duckedMove, 3);
		WEAPON_READ_FIELD_ARR(float, duckedRot, 3);
		WEAPON_READ_FIELD_ARR(float, proneOfs, 3);
		WEAPON_READ_FIELD_ARR(float, proneMove, 3);
		WEAPON_READ_FIELD_ARR(float, proneRot, 3);
		WEAPON_READ_FIELD(float, posMoveRate);
		WEAPON_READ_FIELD(float, posProneMoveRate);
		WEAPON_READ_FIELD(float, standMoveMinSpeed);
		WEAPON_READ_FIELD(float, duckedMoveMinSpeed);
		WEAPON_READ_FIELD(float, proneMoveMinSpeed);
		WEAPON_READ_FIELD(float, posRotRate);
		WEAPON_READ_FIELD(float, posProneRotRate);
		WEAPON_READ_FIELD(weaponIconRatioType_t, hudIconRatio);
		WEAPON_READ_FIELD(weaponIconRatioType_t, pickupIconRatio);
		WEAPON_READ_FIELD(weaponIconRatioType_t, ammoCounterIconRatio);
		WEAPON_READ_FIELD(int, ammoCounterClip);
		WEAPON_READ_FIELD(int, startAmmo);
		WEAPON_READ_FIELD(int, ammoIndex);
		WEAPON_READ_FIELD(int, ammoIndexUnknown);
		WEAPON_READ_FIELD(int, clipIndex);
		WEAPON_READ_FIELD(int, maxAmmo);
		WEAPON_READ_FIELD(int, minAmmoReq);
		WEAPON_READ_FIELD(int, clipSize);
		WEAPON_READ_FIELD(int, shotCount);
		WEAPON_READ_FIELD(int, sharedAmmoCapIndex);
		WEAPON_READ_FIELD(int, sharedAmmoCap);
		WEAPON_READ_FIELD(int, damage);
		WEAPON_READ_FIELD(int, playerDamage);
		WEAPON_READ_FIELD(int, meleeDamage);
		WEAPON_READ_FIELD(int, damageType);
		WEAPON_READ_FIELD(float, autoAimRange);
		WEAPON_READ_FIELD(float, aimAssistRange);
		WEAPON_READ_FIELD(float, aimAssistRangeAds);
		WEAPON_READ_FIELD(float, aimPadding);
		WEAPON_READ_FIELD(float, enemyCrosshairRange);
		WEAPON_READ_FIELD(float, moveSpeedScale);
		WEAPON_READ_FIELD(float, adsMoveSpeedScale);
		WEAPON_READ_FIELD(float, sprintDurationScale);
		WEAPON_READ_FIELD(float, adsZoomFov);
		WEAPON_READ_FIELD(float, adsZoomInFrac);
		WEAPON_READ_FIELD(float, adsZoomOutFrac);
		WEAPON_READ_FIELD(float, adsSceneBlur);
		WEAPON_READ_FIELD(float, adsBobFactor);
		WEAPON_READ_FIELD(float, adsViewBobMult);
		WEAPON_READ_FIELD(float, hipSpreadStandMin);
		WEAPON_READ_FIELD(float, hipSpreadDuckedMin);
		WEAPON_READ_FIELD(float, hipSpreadProneMin);
		WEAPON_READ_FIELD(float, hipSpreadStandMax);
		//WEAPON_READ_FIELD(float, xU_009);
		//WEAPON_READ_FIELD(float, xU_010);
		WEAPON_READ_FIELD(float, hipSpreadDuckedMax);
		WEAPON_READ_FIELD(float, hipSpreadProneMax);
		WEAPON_READ_FIELD(float, hipSpreadDecayRate);
		WEAPON_READ_FIELD(float, hipSpreadFireAdd);
		WEAPON_READ_FIELD(float, hipSpreadTurnAdd);
		WEAPON_READ_FIELD(float, hipSpreadMoveAdd);
		WEAPON_READ_FIELD(float, hipSpreadDuckedDecay);
		WEAPON_READ_FIELD(float, hipSpreadProneDecay);
		WEAPON_READ_FIELD(float, hipReticleSidePos);
		WEAPON_READ_FIELD(float, adsIdleAmount);
		WEAPON_READ_FIELD(float, hipIdleAmount);
		WEAPON_READ_FIELD(float, adsIdleSpeed);
		WEAPON_READ_FIELD(float, hipIdleSpeed);
		WEAPON_READ_FIELD(float, idleCrouchFactor);
		WEAPON_READ_FIELD(float, idleProneFactor);
		WEAPON_READ_FIELD(float, gunMaxPitch);
		WEAPON_READ_FIELD(float, gunMaxYaw);
		WEAPON_READ_FIELD(float, adsIdleLerpStartTime);
		WEAPON_READ_FIELD(float, adsIdleLerpTime);
		WEAPON_READ_FIELD(int, adsTransInTime);
		//WEAPON_READ_FIELD(int, xU_011);
		WEAPON_READ_FIELD(int, adsTransOutTime);
		//WEAPON_READ_FIELD(int, xU_012);
		WEAPON_READ_FIELD(float, swayMaxAngle);
		WEAPON_READ_FIELD(float, swayLerpSpeed);
		WEAPON_READ_FIELD(float, swayPitchScale);
		WEAPON_READ_FIELD(float, swayYawScale);
		WEAPON_READ_FIELD(float, swayVertScale);
		WEAPON_READ_FIELD(float, swayHorizScale);
		WEAPON_READ_FIELD(float, swayShellShockScale);
		WEAPON_READ_FIELD(float, adsSwayMaxAngle);
		WEAPON_READ_FIELD(float, adsSwayLerpSpeed);
		WEAPON_READ_FIELD(float, adsSwayPitchScale);
		WEAPON_READ_FIELD(float, adsSwayYawScale);
		WEAPON_READ_FIELD(float, adsSwayHorizScale);
		WEAPON_READ_FIELD(float, adsSwayVertScale);
		WEAPON_READ_FIELD(float, adsViewErrorMin);
		WEAPON_READ_FIELD(float, adsViewErrorMax);
		WEAPON_READ_FIELD(float, adsFireAnimFrac);
		WEAPON_READ_FIELD(float, dualWieldViewModelOffset);
		WEAPON_READ_FIELD(float, scopeDriftDelay);
		WEAPON_READ_FIELD(float, scopeDriftLerpInTime);
		WEAPON_READ_FIELD(float, scopeDriftSteadyTime);
		WEAPON_READ_FIELD(float, scopeDriftLerpOutTime);
		WEAPON_READ_FIELD(float, scopeDriftSteadyFactor);
		WEAPON_READ_FIELD(float, scopeDriftUnsteadyFactor);
		WEAPON_READ_FIELD(float, bobVerticalFactor);
		WEAPON_READ_FIELD(float, bobHorizontalFactor);
		WEAPON_READ_FIELD(float, bobViewVerticalFactor);
		WEAPON_READ_FIELD(float, bobViewHorizontalFactor);
		WEAPON_READ_FIELD(float, stationaryZoomFov);
		WEAPON_READ_FIELD(float, stationaryZoomDelay);
		WEAPON_READ_FIELD(float, stationaryZoomLerpInTime);
		WEAPON_READ_FIELD(float, stationaryZoomLerpOutTime);
		WEAPON_READ_FIELD(float, adsDofStart);
		WEAPON_READ_FIELD(float, adsDofEnd);
		//WEAPON_READ_FIELD(float, xU_020);
		WEAPON_READ_FIELD(weaponIconRatioType_t, killIconRatio);
		WEAPON_READ_FIELD(weaponIconRatioType_t, dpadIconRatio);
		WEAPON_READ_FIELD(int, fireAnimLength);
		WEAPON_READ_FIELD(int, fireAnimLengthAkimbo);
		WEAPON_READ_FIELD(int, reloadAmmoAdd);
		WEAPON_READ_FIELD(int, reloadStartAdd);
		WEAPON_READ_FIELD(int, ammoDropStockMin);
		WEAPON_READ_FIELD(int, ammoDropStockMax);
		WEAPON_READ_FIELD(int, ammoDropClipPercentMin);
		WEAPON_READ_FIELD(int, ammoDropClipPercentMax);
		WEAPON_READ_FIELD(int, explosionRadius);
		WEAPON_READ_FIELD(int, explosionRadiusMin);
		WEAPON_READ_FIELD(int, explosionInnerDamage);
		WEAPON_READ_FIELD(int, explosionOuterDamage);
		WEAPON_READ_FIELD(float, damageConeAngle);
		WEAPON_READ_FIELD(float, bulletExplDmgMult);
		WEAPON_READ_FIELD(float, bulletExplRadiusMult);
		WEAPON_READ_FIELD(int, projectileSpeed);
		WEAPON_READ_FIELD(int, projectileSpeedUp);
		WEAPON_READ_FIELD(int, projectileSpeedForward);
		WEAPON_READ_FIELD(int, projectileActivateDist);
		WEAPON_READ_FIELD(float, projLifetime);
		WEAPON_READ_FIELD(float, timeToAccelerate);
		WEAPON_READ_FIELD(float, projectileCurvature);
		//WEAPON_READ_FIELD(float, xU_021);
		WEAPON_READ_FIELD(weapProjExposion_t, projExplosion);
		WEAPON_READ_FIELD(WeapStickinessType, stickiness);
		WEAPON_READ_FIELD(float, lowAmmoWarningThreshold);
		WEAPON_READ_FIELD(float, ricochetChance);
		WEAPON_READ_FIELD(int, riotShieldHealth);
		WEAPON_READ_FIELD(float, riotShieldDamageMult);
		WEAPON_READ_FIELD_ARR(float, parallelBounce, 53);
		WEAPON_READ_FIELD_ARR(float, perpendicularBounce, 53);
		WEAPON_READ_FIELD_ARR(float, projectileColor, 3);
		WEAPON_READ_FIELD(guidedMissileType_t, guidedMissileType);
		WEAPON_READ_FIELD(float, maxSteeringAccel);
		WEAPON_READ_FIELD(int, projIgnitionDelay);
		WEAPON_READ_FIELD(float, adsAimPitch);
		WEAPON_READ_FIELD(float, adsCrosshairInFrac);
		WEAPON_READ_FIELD(float, adsCrosshairOutFrac);
		WEAPON_READ_FIELD(int, adsGunKickReducedKickBullets);
		WEAPON_READ_FIELD(float, adsGunKickReducedKickPercent);
		WEAPON_READ_FIELD(float, adsGunKickPitchMin);
		WEAPON_READ_FIELD(float, adsGunKickPitchMax);
		WEAPON_READ_FIELD(float, adsGunKickYawMin);
		WEAPON_READ_FIELD(float, adsGunKickYawMax);
		WEAPON_READ_FIELD(float, adsGunKickMagMin);
		WEAPON_READ_FIELD(float, adsGunKickAccel);
		WEAPON_READ_FIELD(float, adsGunKickSpeedMax);
		WEAPON_READ_FIELD(float, adsGunKickSpeedDecay);
		WEAPON_READ_FIELD(float, adsGunKickStaticDecay);
		WEAPON_READ_FIELD(float, adsViewKickPitchMin);
		WEAPON_READ_FIELD(float, adsViewKickPitchMax);
		WEAPON_READ_FIELD(float, adsViewKickYawMin);
		WEAPON_READ_FIELD(float, adsViewKickYawMax);
		WEAPON_READ_FIELD(float, adsViewKickMagMin);
		WEAPON_READ_FIELD(float, adsViewKickCenterSpeed);
		WEAPON_READ_FIELD(float, adsViewScatterMin);
		WEAPON_READ_FIELD(float, adsViewScatterMax);
		WEAPON_READ_FIELD(float, adsSpread);
		WEAPON_READ_FIELD(int, hipGunKickReducedKickBullets);
		WEAPON_READ_FIELD(float, hipGunKickReducedKickPercent);
		WEAPON_READ_FIELD(float, hipGunKickPitchMin);
		WEAPON_READ_FIELD(float, hipGunKickPitchMax);
		WEAPON_READ_FIELD(float, hipGunKickYawMin);
		WEAPON_READ_FIELD(float, hipGunKickYawMax);
		WEAPON_READ_FIELD(float, hipGunKickMagMin);
		WEAPON_READ_FIELD(float, hipGunKickAccel);
		WEAPON_READ_FIELD(float, hipGunKickSpeedMax);
		WEAPON_READ_FIELD(float, hipGunKickSpeedDecay);
		WEAPON_READ_FIELD(float, hipGunKickStaticDecay);
		WEAPON_READ_FIELD(float, hipViewKickPitchMin);
		WEAPON_READ_FIELD(float, hipViewKickPitchMax);
		WEAPON_READ_FIELD(float, hipViewKickYawMin);
		WEAPON_READ_FIELD(float, hipViewKickYawMax);
		WEAPON_READ_FIELD(float, hipViewKickMagMin);
		WEAPON_READ_FIELD(float, hipViewKickCenterSpeed);
		WEAPON_READ_FIELD(float, hipViewScatterMin);
		WEAPON_READ_FIELD(float, hipViewScatterMax);
		//WEAPON_READ_FIELD(float, xU_043);
		WEAPON_READ_FIELD(int, adsReloadTransTime);
		WEAPON_READ_FIELD(float, fightDist);
		WEAPON_READ_FIELD(float, maxDist);
		WEAPON_READ_FIELD(int, positionReloadTransTime);
		WEAPON_READ_FIELD(float, leftArc);
		WEAPON_READ_FIELD(float, rightArc);
		WEAPON_READ_FIELD(float, topArc);
		WEAPON_READ_FIELD(float, bottomArc);
		WEAPON_READ_FIELD(float, accuracy);
		WEAPON_READ_FIELD(float, aiSpread);
		WEAPON_READ_FIELD(float, playerSpread);
		WEAPON_READ_FIELD_ARR(float, minTurnSpeed, 2);
		WEAPON_READ_FIELD_ARR(float, maxTurnSpeed, 2);
		WEAPON_READ_FIELD(float, pitchConvergenceTime);
		WEAPON_READ_FIELD(float, yawConvergenceTime);
		WEAPON_READ_FIELD(float, suppressTime);
		WEAPON_READ_FIELD(float, maxRange);
		WEAPON_READ_FIELD(float, animHorRotateInc);
		WEAPON_READ_FIELD(float, playerPositionDist);
		//WEAPON_READ_FIELD(unsigned int, iUseHintStringIndex);
		//WEAPON_READ_FIELD(unsigned int, dropHintStringIndex);
		WEAPON_READ_FIELD(float, horizViewJitter);
		WEAPON_READ_FIELD(float, vertViewJitter);
		WEAPON_READ_FIELD(float, scanSpeed);
		WEAPON_READ_FIELD(float, scanAccel);
		WEAPON_READ_FIELD(int, scanPauseTime);
		WEAPON_READ_FIELD(int, minDamage);
		WEAPON_READ_FIELD(int, midDamage);
		WEAPON_READ_FIELD(int, minPlayerDamage);
		WEAPON_READ_FIELD(int, midPlayerDamage);
		WEAPON_READ_FIELD(float, maxDamageRange);
		WEAPON_READ_FIELD(float, minDamageRange);
		//WEAPON_READ_FIELD(int, iU_045);
		//WEAPON_READ_FIELD(int, iU_046);
		//WEAPON_READ_FIELD(int, iU_047);
		//WEAPON_READ_FIELD(int, iU_048);
		//WEAPON_READ_FIELD(float, fU_049);
		//WEAPON_READ_FIELD(float, fU_050);
		WEAPON_READ_FIELD(float, destabilizationRateTime);
		WEAPON_READ_FIELD(float, destabilizationCurvatureMax);
		WEAPON_READ_FIELD(int, destabilizeDistance);
		WEAPON_READ_FIELD(float, turretADSTime);
		WEAPON_READ_FIELD(float, turretFov);
		WEAPON_READ_FIELD(float, turretFovADS);
		WEAPON_READ_FIELD(float, turretScopeZoomRate);
		WEAPON_READ_FIELD(float, turretScopeZoomMin);
		WEAPON_READ_FIELD(float, turretScopeZoomMax);
		//WEAPON_READ_FIELD(float, xU_056);
		//WEAPON_READ_FIELD(float, xU_057);
		//WEAPON_READ_FIELD(float, xU_058);
		//WEAPON_READ_FIELD(float, xU_059);
		WEAPON_READ_FIELD(float, turretBarrelSpinSpeed);
		WEAPON_READ_FIELD(float, turretBarrelSpinUpTime);
		WEAPON_READ_FIELD(float, turretBarrelSpinDownTime);
		WEAPON_READ_FIELD(float, missileConeSoundRadiusAtTop);
		WEAPON_READ_FIELD(float, missileConeSoundRadiusAtBase);
		WEAPON_READ_FIELD(float, missileConeSoundHeight);
		WEAPON_READ_FIELD(float, missileConeSoundOriginOffset);
		WEAPON_READ_FIELD(float, missileConeSoundVolumescaleAtCore);
		WEAPON_READ_FIELD(float, missileConeSoundVolumescaleAtEdge);
		WEAPON_READ_FIELD(float, missileConeSoundVolumescaleCoreSize);
		WEAPON_READ_FIELD(float, missileConeSoundPitchAtTop);
		WEAPON_READ_FIELD(float, missileConeSoundPitchAtBottom);
		WEAPON_READ_FIELD(float, missileConeSoundPitchTopSize);
		WEAPON_READ_FIELD(float, missileConeSoundPitchBottomSize);
		WEAPON_READ_FIELD(float, missileConeSoundCrossfadeTopSize);
		WEAPON_READ_FIELD(float, missileConeSoundCrossfadeBottomSize);
		WEAPON_READ_FIELD(float, aim_automelee_lerp);
		WEAPON_READ_FIELD(float, aim_automelee_range);
		WEAPON_READ_FIELD(float, aim_automelee_region_height);
		WEAPON_READ_FIELD(float, aim_automelee_region_width);
		WEAPON_READ_FIELD(float, player_meleeHeight);
		WEAPON_READ_FIELD(float, player_meleeRange);
		WEAPON_READ_FIELD(float, player_meleeWidth);
		WEAPON_READ_FIELD(float ,signatureFireTime);
		WEAPON_READ_FIELD(int, signatureNumBullets);
		WEAPON_READ_FIELD(weapFireTimeInterpolation_t, fireTimeInterpolationType);
		//WEAPON_READ_FIELD(int, iU_3408);
		//WEAPON_READ_FIELD(int, iU_3412);
		//WEAPON_READ_FIELD(int, iU_3416);
		//WEAPON_READ_FIELD(int, iU_3420);
		//WEAPON_READ_FIELD(int, iU_3424);
		//WEAPON_READ_FIELD(bool, bU_3432);
		//WEAPON_READ_FIELD(bool, bU_3433);
		//WEAPON_READ_FIELD(bool, bU_3434);
		//WEAPON_READ_FIELD(bool, bU_3435);
		//WEAPON_READ_FIELD(bool, bU_3436);
		//WEAPON_READ_FIELD(bool, bU_3437);
		//WEAPON_READ_FIELD(bool, bU_3438);
		//WEAPON_READ_FIELD(bool, bU_3439);
		WEAPON_READ_FIELD(bool, sharedAmmo);
		WEAPON_READ_FIELD(bool, lockonSupported);
		WEAPON_READ_FIELD(bool, requireLockonToFire);
		WEAPON_READ_FIELD(bool, isAirburstWeapon);
		WEAPON_READ_FIELD(bool, bigExplosion);
		WEAPON_READ_FIELD(bool, noAdsWhenMagEmpty);
		WEAPON_READ_FIELD(bool, avoidDropCleanup);
		WEAPON_READ_FIELD(bool, inheritsPerks);
		WEAPON_READ_FIELD(bool, crosshairColorChange);
		WEAPON_READ_FIELD(bool, rifleBullet);
		WEAPON_READ_FIELD(bool, armorPiercing);
		WEAPON_READ_FIELD(bool, boltAction);
		WEAPON_READ_FIELD(bool, aimDownSight);
		WEAPON_READ_FIELD(bool, canHoldBreath);
		//WEAPON_READ_FIELD(bool, bU_3454);
		//WEAPON_READ_FIELD(bool, bU_3455);
		WEAPON_READ_FIELD(bool, canVariableZoom);
		WEAPON_READ_FIELD(bool, rechamberWhileAds);
		WEAPON_READ_FIELD(bool, bulletExplosiveDamage);
		WEAPON_READ_FIELD(bool, cookOffHold);
		WEAPON_READ_FIELD(bool, reticleSpin45);
		WEAPON_READ_FIELD(bool, reticleSideEnabled);
		WEAPON_READ_FIELD(bool, clipOnly);
		WEAPON_READ_FIELD(bool, noAmmoPickup);
		WEAPON_READ_FIELD(bool, disableSwitchToWhenEmpty);
		//WEAPON_READ_FIELD(bool, bU_3465);
		WEAPON_READ_FIELD(bool, hasMotionTracker);
		//WEAPON_READ_FIELD(bool, bU_3467);
		WEAPON_READ_FIELD(bool, noDualWield);
		WEAPON_READ_FIELD(bool, flipKillIcon);
		WEAPON_READ_FIELD(bool, actionSlotShowAmmo);
		WEAPON_READ_FIELD(bool, noPartialReload);
		WEAPON_READ_FIELD(bool, segmentedReload);
		WEAPON_READ_FIELD(bool, multipleReload);
		WEAPON_READ_FIELD(bool, blocksProne);
		WEAPON_READ_FIELD(bool, silenced);
		WEAPON_READ_FIELD(bool, isRollingGrenade);
		WEAPON_READ_FIELD(bool, projExplosionEffectForceNormalUp);
		WEAPON_READ_FIELD(bool, projExplosionEffectInheritParentDirection);
		WEAPON_READ_FIELD(bool, projImpactExplode);
		WEAPON_READ_FIELD(bool, projTrajectoryEvents);
		WEAPON_READ_FIELD(bool, projWhizByEnabled);
		WEAPON_READ_FIELD(bool, stickToPlayers);
		WEAPON_READ_FIELD(bool, stickToVehicles);
		WEAPON_READ_FIELD(bool, stickToTurrets);
		WEAPON_READ_FIELD(bool, thrownSideways);
		WEAPON_READ_FIELD(bool, hasDetonatorEmptyThrow);
		WEAPON_READ_FIELD(bool, hasDetonatorDoubleTap);
		WEAPON_READ_FIELD(bool, disableFiring);
		WEAPON_READ_FIELD(bool, timedDetonation);
		//WEAPON_READ_FIELD(bool, bU_3528);
		//WEAPON_READ_FIELD(bool, bU_3529);
		WEAPON_READ_FIELD(bool, rotate);
		WEAPON_READ_FIELD(bool, holdButtonToThrow);
		WEAPON_READ_FIELD(bool, freezeMovementWhenFiring);
		WEAPON_READ_FIELD(bool, thermalScope);
		WEAPON_READ_FIELD(bool, thermalToggle);
		WEAPON_READ_FIELD(bool, outlineEnemies);
		WEAPON_READ_FIELD(bool, altModeSameWeapon);
		WEAPON_READ_FIELD(bool, turretBarrelSpinEnabled);
		WEAPON_READ_FIELD(bool, missileConeSoundEnabled);
		WEAPON_READ_FIELD(bool, missileConeSoundPitchshiftEnabled);
		WEAPON_READ_FIELD(bool, missileConeSoundCrossfadeEnabled);
		WEAPON_READ_FIELD(bool, offhandHoldIsCancelable);
		WEAPON_READ_FIELD(bool, doNotAllowAttachmentsToOverrideSpread);
		WEAPON_READ_FIELD(bool, useFastReloadAnims);
		WEAPON_READ_FIELD(bool, dualMagReloadSupported);
		WEAPON_READ_FIELD(bool, reloadStopsAlt);
		//WEAPON_READ_FIELD(bool, bU_3546);
		WEAPON_READ_FIELD(bool, alwaysShatterGlassOnImpact);
		WEAPON_READ_FIELD(bool, oldWeapon);
		//WEAPON_READ_FIELD(bool, bU_3511);
		//WEAPON_READ_FIELD(bool, bU_3512);
		//WEAPON_READ_FIELD(bool, bU_3513);
		WEAPON_READ_FIELD(bool, hasCounterSilencer);
		//WEAPON_READ_FIELD(bool, bU_3515);
		//WEAPON_READ_FIELD(bool, bU_3516);
		WEAPON_READ_FIELD(bool, disableVariableAutosimRate);
		//WEAPON_READ_FIELD(bool, bU_3518);
		//WEAPON_READ_FIELD(bool, bU_3519);
		//WEAPON_READ_FIELD(bool, bU_3520);
		//WEAPON_READ_FIELD(bool, bU_3521);
		//WEAPON_READ_FIELD(bool, bU_3522);
		WEAPON_READ_FIELD(bool, cloakedWeapon);
		WEAPON_READ_FIELD(bool, adsHideWeapon);
		//WEAPON_READ_FIELD(bool, bU_3525);
		//WEAPON_READ_FIELD(bool, bU_3526);
		//WEAPON_READ_FIELD(bool, bU_3527);
		WEAPON_READ_FIELD(float, adsDofPhysicalFStop);
		WEAPON_READ_FIELD(float, adsDofPhysicalFocusDistance);
		// unknown data

		return weapon;
	}

	void weapon_def::init(const std::string& name, zone_memory* mem)
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
			ZONETOOL_FATAL("Weapon file \"%s\" not found.", name.data());
		}
	}

	void weapon_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* weapon = this->asset_;

#define WEAPON_SCRIPTSTRING_ARRAY_PREPARE(__field__,__count__) \
		if (weapon->__field__) \
		{ \
			for (int nt = 0; nt < __count__; nt++) \
			{ \
				weapon->__field__[nt] = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&weapon->__field__[nt]))); \
			} \
		}

		if (weapon->hideTags)
		{
			for (int i = 0; i < 32; i++)
			{
				weapon->hideTags[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&weapon->hideTags[i])));
			}
		}

		if (weapon->notetrackOverrides)
		{
			for (auto i = 0u; i < weapon->numNotetrackOverrides; i++)
			{
				WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackOverrides[i].notetrackSoundMapKeys, 36);
				WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackOverrides[i].notetrackSoundMapValues, 36);
			}
		}

		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackSoundMapKeys, 36);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackSoundMapValues, 36);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackRumbleMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackRumbleMapValues, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackFXMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackFXMapTagValues, 16);

		weapon->stowTag = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&weapon->stowTag)));
	}

	void weapon_def::load_depending(zone_base* zone)
	{
		auto weapon = this->asset_;
#define WEAPON_SUBASSET_DEPENDING(__field__,__type__/*,__struct__*/) \
		if (weapon->__field__) \
		{ \
			zone->add_asset_of_type(__type__, weapon->__field__->name); \
		}

		for (int i = 0; i < 2; i++)
		{
			if (weapon->gunModel)
			{
				WEAPON_SUBASSET_DEPENDING(gunModel[i], ASSET_TYPE_XMODEL);
			}
			if (weapon->worldModel)
			{
				WEAPON_SUBASSET_DEPENDING(worldModel[i], ASSET_TYPE_XMODEL);
			}
		}

		WEAPON_SUBASSET_DEPENDING(camoWorldModelMaterialOverride, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(camoViewModelMaterialOverride, ASSET_TYPE_MATERIAL);

		WEAPON_SUBASSET_DEPENDING(handModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(unknownModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(worldClipModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(rocketModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(knifeModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(worldKnifeModel, ASSET_TYPE_XMODEL);

		for (auto i = 0; i < 180; i++)
		{
			if (weapon->szXAnimsRightHanded)
			{
				WEAPON_SUBASSET_DEPENDING(szXAnimsRightHanded[i], ASSET_TYPE_XANIMPARTS);
			}
			if (weapon->szXAnimsLeftHanded)
			{
				WEAPON_SUBASSET_DEPENDING(szXAnimsLeftHanded[i], ASSET_TYPE_XANIMPARTS);
			}
			if (weapon->szXAnims)
			{
				WEAPON_SUBASSET_DEPENDING(szXAnims[i], ASSET_TYPE_XANIMPARTS);
			}
		}

		for (auto i = 0; i < weapon->numWeaponAttachments; i++)
		{
			if (weapon->attachments)
			{
				WEAPON_SUBASSET_DEPENDING(attachments[i], ASSET_TYPE_ATTACHMENT);
			}
		}

		if (weapon->animOverrides)
		{
			for (auto i = 0u; i < weapon->numAnimOverrides; i++)
			{
				WEAPON_SUBASSET_DEPENDING(animOverrides[i].overrideAnim, ASSET_TYPE_XANIMPARTS);
				WEAPON_SUBASSET_DEPENDING(animOverrides[i].altmodeAnim, ASSET_TYPE_XANIMPARTS);
			}
		}

		if (weapon->soundOverrides)
		{
			for (auto i = 0u; i < weapon->numSoundOverrides; i++)
			{
				WEAPON_SUBASSET_DEPENDING(soundOverrides[i].overrideSound, ASSET_TYPE_SOUND);
				WEAPON_SUBASSET_DEPENDING(soundOverrides[i].altmodeSound, ASSET_TYPE_SOUND);
			}
		}

		if (weapon->fxOverrides)
		{
			for (auto i = 0u; i < weapon->numFXOverrides; i++)
			{
				WEAPON_SUBASSET_DEPENDING(fxOverrides[i].overrideFX, ASSET_TYPE_FX);
				WEAPON_SUBASSET_DEPENDING(fxOverrides[i].altmodeFX, ASSET_TYPE_FX);
			}
		}

		for (auto i = 0; i < 36; i++)
		{
			auto str = this->get_script_string(&weapon->notetrackSoundMapValues[i]);
			if (str != nullptr)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND, str);
			}
		}

		for (auto i = 0; i < 16; i++)
		{
			if (weapon->notetrackFXMapValues)
			{
				WEAPON_SUBASSET_DEPENDING(notetrackFXMapValues[i], ASSET_TYPE_FX);
			}
		}
		
		WEAPON_SUBASSET_DEPENDING(viewFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewBodyFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(worldFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewFlashADSEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewBodyFlashADSEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(effect06, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(effect07, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(effect08, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(effect09, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(effect10, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(effect11, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(effect12, ASSET_TYPE_FX);
		
		WEAPON_SUBASSET_DEPENDING(pickupSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(pickupSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(ammoPickupSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(ammoPickupSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(projectileSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(pullbackSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(pullbackSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSoundPlayerAkimbo, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound13, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound14, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound15, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound16, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLoopSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLoopSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound19, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound20, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound21, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound22, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLoopEndPointSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLoopEndPointSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireStopSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireStopSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound27, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound28, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound29, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound30, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLastShotSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLastShotSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireFirstSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireFirstSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLastSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLastSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound39, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeSwipeSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeSwipeSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeHitSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeHitSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeMissSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeMissSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(rechamberSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(rechamberSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(reloadSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(reloadSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(reloadEmptySound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(reloadEmptySoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(reloadStartSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(reloadStartSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(reloadEndSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(reloadEndSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(detonateSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(detonateSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(nightVisionWearSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(nightVisionWearSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(nightVisionRemoveSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(nightVisionRemoveSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(raiseSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(raiseSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound64, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound65, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound66, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound67, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(putawaySound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(putawaySoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound70, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound71, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(adsEnterSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(adsLeaveSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(adsCrosshairEnemySound, ASSET_TYPE_SOUND);
		
		for (auto i = 0; i < 53; i++)
		{
			if (weapon->bounceSound)
			{
				WEAPON_SUBASSET_DEPENDING(bounceSound[i], ASSET_TYPE_SOUND);
			}

			if (weapon->rollingSound)
			{
				WEAPON_SUBASSET_DEPENDING(rollingSound[i], ASSET_TYPE_SOUND);
			}
		}

		WEAPON_SUBASSET_DEPENDING(viewShellEjectEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(worldShellEjectEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewLastShotEjectEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(worldLastShotEjectEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewMagEjectEffect, ASSET_TYPE_FX);

		WEAPON_SUBASSET_DEPENDING(reticleCenter, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(reticleSide, ASSET_TYPE_MATERIAL);

		WEAPON_SUBASSET_DEPENDING(hudIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(pickupIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(unknownIcon2, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(unknownIcon3, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(unknownIcon4, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(ammoCounterIcon, ASSET_TYPE_MATERIAL);
		
		WEAPON_SUBASSET_DEPENDING(physCollmap, ASSET_TYPE_PHYSCOLLMAP);
		WEAPON_SUBASSET_DEPENDING(physPreset, ASSET_TYPE_PHYSPRESET);

		WEAPON_SUBASSET_DEPENDING(tracer1, ASSET_TYPE_TRACER);
		WEAPON_SUBASSET_DEPENDING(tracer2, ASSET_TYPE_TRACER);

		WEAPON_SUBASSET_DEPENDING(laser, ASSET_TYPE_LASER);

		WEAPON_SUBASSET_DEPENDING(turretOverheatSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(turretOverheatEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(turretBarrelSpinMaxSnd, ASSET_TYPE_SOUND);
		for (int i = 0; i < 4; i++)
		{
			WEAPON_SUBASSET_DEPENDING(turretBarrelSpinUpSnd[i], ASSET_TYPE_SOUND);
			WEAPON_SUBASSET_DEPENDING(turretBarrelSpinDownSnd[i], ASSET_TYPE_SOUND);
		}
		WEAPON_SUBASSET_DEPENDING(missileConeSoundAlias, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(missileConeSoundAliasAtBase, ASSET_TYPE_SOUND);

		WEAPON_SUBASSET_DEPENDING(stowOffsetModel, ASSET_TYPE_XMODEL);

		WEAPON_SUBASSET_DEPENDING(overlay.shader, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderLowRes, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderEMP, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL);

		WEAPON_SUBASSET_DEPENDING(killIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(dpadIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(hudProximityWarningIcon, ASSET_TYPE_MATERIAL);
		
		WEAPON_SUBASSET_DEPENDING(projectileModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(projExplosionEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projDudEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projExplosionSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(projDudSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(projTrailEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projBeaconEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projIgnitionEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projIgnitionSound, ASSET_TYPE_SOUND);
	}

	std::string weapon_def::name()
	{
		return this->name_;
	}

	std::int32_t weapon_def::type()
	{
		return ASSET_TYPE_WEAPON;
	}

	void weapon_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

#define WEAPON_STRING(__field__) \
		if(data->__field__) \
		{ \
			dest->__field__ = buf->write_str(data->__field__); \
		}

#define WEAPON_SUBASSET(__field__,__type__,__struct__) \
		if (data->__field__) \
		{ \
			dest->__field__ = reinterpret_cast<__struct__*>(zone->get_asset_pointer(__type__, data->__field__->name)); \
		}

#define WEAPON_SOUND_CUSTOM(__field__) \
		if (data->__field__) \
		{ \
			auto ptr = 0xFDFDFDFFFFFFFFFF; \
			buf->align(7); \
			buf->write(&ptr); \
			buf->write_str(data->__field__->name); \
			buf->clear_pointer(&dest->__field__); \
		}

#define WEAPON_SCRIPTSTRING_ARRAY(__field__,__count__) \
		if (data->__field__) \
		{ \
			buf->align(3); \
			buf->write(data->__field__,__count__); \
			buf->clear_pointer(&dest->__field__); \
		}

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->szInternalName = buf->write_str(this->name());

		WEAPON_STRING(szDisplayName);
		WEAPON_STRING(szOverlayName);

		if (data->gunModel)
		{
			buf->align(7);
			auto destModels = buf->write(data->gunModel, 2);

			for (auto i = 0; i < 2; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->gunModel[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->gunModel);
		}

		if (data->handModel)
		{
			dest->handModel = reinterpret_cast<XModel*>(
				zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->handModel->name)
				);
		}

		if (data->unknownModel)
		{
			dest->unknownModel = reinterpret_cast<XModel*>(
				zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->unknownModel->name)
				);
		}

		WEAPON_STRING(szModeName);

		if (data->camoWorldModelMaterialOverride)
		{
			dest->camoWorldModelMaterialOverride = reinterpret_cast<Material*>(
				zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->camoWorldModelMaterialOverride->name));
		}

		if (data->szXAnimsRightHanded)
		{
			buf->align(7);
			auto destAnims = buf->write(data->szXAnimsRightHanded, 180);

			for (auto i = 0; i < 180; i++)
			{
				if (destAnims[i])
				{
					destAnims[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->szXAnimsRightHanded[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->szXAnimsRightHanded);
		}

		if (data->szXAnimsLeftHanded)
		{
			buf->align(7);
			auto destAnims = buf->write(data->szXAnimsLeftHanded, 180);

			for (auto i = 0; i < 180; i++)
			{
				if (destAnims[i])
				{
					destAnims[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->szXAnimsLeftHanded[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->szXAnimsLeftHanded);
		}

		if (data->hideTags)
		{
			buf->align(3);
			buf->write(data->hideTags, 32);
			buf->clear_pointer(&dest->hideTags);
		}

		if (data->attachments)
		{
			buf->align(7);
			auto destAttachments = buf->write(data->attachments, data->numWeaponAttachments);

			for (auto i = 0; i < data->numWeaponAttachments; i++)
			{
				if (destAttachments[i])
				{
					destAttachments[i] = reinterpret_cast<WeaponAttachment*>(
						zone->get_asset_pointer(ASSET_TYPE_ATTACHMENT, data->attachments[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->attachments);
		}

		if (data->szXAnims)
		{
			buf->align(7);
			auto destAnims = buf->write(data->szXAnims, 180);

			for (auto i = 0; i < 180; i++)
			{
				if (destAnims[i])
				{
					destAnims[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->szXAnims[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->szXAnims);
		}

		if (data->animOverrides)
		{
			buf->align(3);
			auto destAnimOverrides = buf->write(data->animOverrides, data->numAnimOverrides);

			for (auto i = 0; i < data->numAnimOverrides; i++)
			{
				if (destAnimOverrides[i].overrideAnim)
				{
					destAnimOverrides[i].overrideAnim = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->animOverrides[i].overrideAnim->name)
						);
				}

				if (destAnimOverrides[i].altmodeAnim)
				{
					destAnimOverrides[i].altmodeAnim = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->animOverrides[i].altmodeAnim->name)
						);
				}
			}

			buf->clear_pointer(&dest->animOverrides);
		}

		if (data->soundOverrides)
		{
			buf->align(3);
			dest->soundOverrides = buf->write(data->soundOverrides, data->numSoundOverrides);

			for (auto i = 0; i < data->numSoundOverrides; i++)
			{
				if (dest->soundOverrides[i].overrideSound)
				{
					WEAPON_SOUND_CUSTOM(soundOverrides[i].overrideSound);
				}

				if (dest->soundOverrides[i].altmodeSound)
				{
					WEAPON_SOUND_CUSTOM(soundOverrides[i].altmodeSound);
				}
			}

			buf->clear_pointer(&dest->soundOverrides);
		}

		if (data->fxOverrides)
		{
			buf->align(3);
			auto destFxOverrides = buf->write(data->fxOverrides, data->numFXOverrides);

			for (auto i = 0; i < data->numFXOverrides; i++)
			{
				if (destFxOverrides[i].overrideFX)
				{
					destFxOverrides[i].overrideFX = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->fxOverrides[i].overrideFX->name
					));
				}

				if (destFxOverrides[i].altmodeFX)
				{
					destFxOverrides[i].altmodeFX = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->fxOverrides[i].altmodeFX->name
					));
				}
			}

			buf->clear_pointer(&dest->fxOverrides);
		}

		if (data->reloadOverrides)
		{
			buf->align(3);
			buf->write(data->reloadOverrides, data->numReloadStateTimerOverrides);
			buf->clear_pointer(&dest->reloadOverrides);
		}

		if (data->notetrackOverrides)
		{
			buf->align(3);
			auto destNoteTrackOverrides = buf->write(data->notetrackOverrides, data->numNotetrackOverrides);

			for (auto i = 0; i < data->numNotetrackOverrides; i++)
			{
				if (destNoteTrackOverrides[i].notetrackSoundMapKeys)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapKeys, 36);
					buf->clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapKeys);
				}

				if (destNoteTrackOverrides[i].notetrackSoundMapValues)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapValues, 36);
					buf->clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapValues);
				}
			}

			buf->clear_pointer(&dest->notetrackOverrides);
		}

		WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapKeys, 36);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapValues, 36);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapValues, 16);

		WEAPON_SCRIPTSTRING_ARRAY(notetrackFXMapKeys, 16);
		if (data->notetrackFXMapValues)
		{
			buf->align(7);
			auto destNotetrackFXMapValues = buf->write(data->notetrackFXMapValues, 16);

			for (auto i = 0; i < 16; i++)
			{
				if (destNotetrackFXMapValues[i])
				{
					destNotetrackFXMapValues[i] = reinterpret_cast<FxEffectDef*>(
						zone->get_asset_pointer(ASSET_TYPE_FX, data->notetrackFXMapValues[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->notetrackFXMapValues);
		}
		WEAPON_SCRIPTSTRING_ARRAY(notetrackFXMapTagValues, 16);

		WEAPON_STRING(szAltWeaponName);

		WEAPON_SUBASSET(viewFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewBodyFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(worldFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewFlashADSEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewBodyFlashADSEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(effect06, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(effect07, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(effect08, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(effect09, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(effect10, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(effect11, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(effect12, ASSET_TYPE_FX, FxEffectDef);
		
		WEAPON_SOUND_CUSTOM(pickupSound);
		WEAPON_SOUND_CUSTOM(pickupSoundPlayer);
		WEAPON_SOUND_CUSTOM(ammoPickupSound);
		WEAPON_SOUND_CUSTOM(ammoPickupSoundPlayer);
		WEAPON_SOUND_CUSTOM(projectileSound);
		WEAPON_SOUND_CUSTOM(pullbackSound);
		WEAPON_SOUND_CUSTOM(pullbackSoundPlayer);
		WEAPON_SOUND_CUSTOM(pullbackSoundQuick);
		WEAPON_SOUND_CUSTOM(pullbackSoundQuickPlayer);
		WEAPON_SOUND_CUSTOM(fireSound);
		WEAPON_SOUND_CUSTOM(fireSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireSoundPlayerAkimbo);
		WEAPON_SOUND_CUSTOM(sound13);
		WEAPON_SOUND_CUSTOM(sound14);
		WEAPON_SOUND_CUSTOM(sound15);
		WEAPON_SOUND_CUSTOM(sound16);
		WEAPON_SOUND_CUSTOM(fireLoopSound);
		WEAPON_SOUND_CUSTOM(fireLoopSoundPlayer);
		WEAPON_SOUND_CUSTOM(sound19);
		WEAPON_SOUND_CUSTOM(sound20);
		WEAPON_SOUND_CUSTOM(sound21);
		WEAPON_SOUND_CUSTOM(sound22);
		WEAPON_SOUND_CUSTOM(fireLoopEndPointSound);
		WEAPON_SOUND_CUSTOM(fireLoopEndPointSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireStopSound);
		WEAPON_SOUND_CUSTOM(fireStopSoundPlayer);
		WEAPON_SOUND_CUSTOM(sound27);
		WEAPON_SOUND_CUSTOM(sound28);
		WEAPON_SOUND_CUSTOM(sound29);
		WEAPON_SOUND_CUSTOM(sound30);
		WEAPON_SOUND_CUSTOM(fireLastShotSound);
		WEAPON_SOUND_CUSTOM(fireLastShotSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireFirstSound);
		WEAPON_SOUND_CUSTOM(fireFirstSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireLastSound);
		WEAPON_SOUND_CUSTOM(fireLastSoundPlayer);
		WEAPON_SOUND_CUSTOM(emptyFireSound);
		WEAPON_SOUND_CUSTOM(emptyFireSoundPlayer);
		WEAPON_SOUND_CUSTOM(sound39);
		WEAPON_SOUND_CUSTOM(meleeSwipeSound);
		WEAPON_SOUND_CUSTOM(meleeSwipeSoundPlayer);
		WEAPON_SOUND_CUSTOM(meleeHitSound);
		WEAPON_SOUND_CUSTOM(meleeHitSoundPlayer);
		WEAPON_SOUND_CUSTOM(meleeMissSound);
		WEAPON_SOUND_CUSTOM(meleeMissSoundPlayer);
		WEAPON_SOUND_CUSTOM(rechamberSound);
		WEAPON_SOUND_CUSTOM(rechamberSoundPlayer);
		WEAPON_SOUND_CUSTOM(reloadSound);
		WEAPON_SOUND_CUSTOM(reloadSoundPlayer);
		WEAPON_SOUND_CUSTOM(reloadEmptySound);
		WEAPON_SOUND_CUSTOM(reloadEmptySoundPlayer);
		WEAPON_SOUND_CUSTOM(reloadStartSound);
		WEAPON_SOUND_CUSTOM(reloadStartSoundPlayer);
		WEAPON_SOUND_CUSTOM(reloadEndSound);
		WEAPON_SOUND_CUSTOM(reloadEndSoundPlayer);
		WEAPON_SOUND_CUSTOM(detonateSound);
		WEAPON_SOUND_CUSTOM(detonateSoundPlayer);
		WEAPON_SOUND_CUSTOM(nightVisionWearSound);
		WEAPON_SOUND_CUSTOM(nightVisionWearSoundPlayer);
		WEAPON_SOUND_CUSTOM(nightVisionRemoveSound);
		WEAPON_SOUND_CUSTOM(nightVisionRemoveSoundPlayer);
		WEAPON_SOUND_CUSTOM(raiseSound);
		WEAPON_SOUND_CUSTOM(raiseSoundPlayer);
		WEAPON_SOUND_CUSTOM(sound64);
		WEAPON_SOUND_CUSTOM(sound65);
		WEAPON_SOUND_CUSTOM(sound66);
		WEAPON_SOUND_CUSTOM(sound67);
		WEAPON_SOUND_CUSTOM(putawaySound);
		WEAPON_SOUND_CUSTOM(putawaySoundPlayer);
		WEAPON_SOUND_CUSTOM(sound70);
		WEAPON_SOUND_CUSTOM(sound71);
		WEAPON_SOUND_CUSTOM(adsEnterSoundPlayer);
		WEAPON_SOUND_CUSTOM(adsLeaveSoundPlayer);
		WEAPON_SOUND_CUSTOM(adsCrosshairEnemySound);
		
		if (data->bounceSound)
		{
			buf->align(7);
			dest->bounceSound = buf->write(data->bounceSound, 53);

			for (auto i = 0; i < 53; i++)
			{
				WEAPON_SOUND_CUSTOM(bounceSound[i]);
			}

			buf->clear_pointer(&dest->bounceSound);
		}

		if (data->rollingSound)
		{
			buf->align(7);
			dest->rollingSound = buf->write(data->rollingSound, 53);

			for (auto i = 0; i < 53; i++)
			{
				WEAPON_SOUND_CUSTOM(rollingSound[i]);
			}

			buf->clear_pointer(&dest->rollingSound);
		}

		WEAPON_SUBASSET(viewShellEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(worldShellEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewLastShotEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(worldLastShotEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewMagEjectEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SUBASSET(reticleCenter, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(reticleSide, ASSET_TYPE_MATERIAL, Material);

		if (data->worldModel)
		{
			buf->align(7);
			auto destModels = buf->write(data->worldModel, 2);

			for (auto i = 0; i < 2; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->worldModel[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->worldModel);
		}

		WEAPON_SUBASSET(worldClipModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(rocketModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(knifeModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(worldKnifeModel, ASSET_TYPE_XMODEL, XModel);
		
		WEAPON_SUBASSET(hudIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(pickupIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(unknownIcon2, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(unknownIcon3, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(unknownIcon4, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(ammoCounterIcon, ASSET_TYPE_MATERIAL, Material);

		WEAPON_STRING(szAmmoName);
		WEAPON_STRING(szClipName);
		WEAPON_STRING(szSharedAmmoCapName);

		WEAPON_SUBASSET(physCollmap, ASSET_TYPE_PHYSCOLLMAP, PhysCollmap);
		WEAPON_SUBASSET(physPreset, ASSET_TYPE_PHYSPRESET, PhysPreset);

		WEAPON_STRING(szUseHintString);
		WEAPON_STRING(dropHintString);

		if (data->locationDamageMultipliers)
		{
			buf->align(3);
			buf->write(data->locationDamageMultipliers, 22);
			buf->clear_pointer(&dest->locationDamageMultipliers);
		}

		WEAPON_STRING(fireRumble);
		WEAPON_STRING(fireMedRumble);
		WEAPON_STRING(fireHighRumble);
		WEAPON_STRING(meleeImpactRumble);
		
		WEAPON_SUBASSET(tracer1, ASSET_TYPE_TRACER, TracerDef);
		WEAPON_SUBASSET(tracer2, ASSET_TYPE_TRACER, TracerDef);

		WEAPON_SUBASSET(laser, ASSET_TYPE_LASER, LaserDef);

		WEAPON_SOUND_CUSTOM(turretOverheatSound);
		WEAPON_SUBASSET(turretOverheatEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_STRING(turretBarrelSpinRumble);

		WEAPON_SOUND_CUSTOM(turretBarrelSpinMaxSnd);

		for (int i = 0; i < 4; i++)
		{
			WEAPON_SOUND_CUSTOM(turretBarrelSpinUpSnd[i]);
			WEAPON_SOUND_CUSTOM(turretBarrelSpinDownSnd[i]);
		}

		WEAPON_SOUND_CUSTOM(missileConeSoundAlias);
		WEAPON_SOUND_CUSTOM(missileConeSoundAliasAtBase);

		WEAPON_SUBASSET(stowOffsetModel, ASSET_TYPE_XMODEL, XModel);

		if (data->turretHydraulicSettings)
		{
			buf->align(3);
			auto destTurretHydraulicSettings = buf->write(data->turretHydraulicSettings);

#define HYDRAULIC_SOUND_CUSTOM(__data__, __dest__) \
		if (__data__) \
		{ \
			auto ptr = 0xFDFDFDFFFFFFFFFF; \
			buf->align(7); \
			buf->write(&ptr); \
			buf->write_str(__data__->name); \
			buf->clear_pointer(&__dest__); \
		}

			HYDRAULIC_SOUND_CUSTOM(data->turretHydraulicSettings->verticalSound, destTurretHydraulicSettings->verticalSound);
			HYDRAULIC_SOUND_CUSTOM(data->turretHydraulicSettings->verticalStopSound, destTurretHydraulicSettings->verticalStopSound);
			HYDRAULIC_SOUND_CUSTOM(data->turretHydraulicSettings->horizontalSound, destTurretHydraulicSettings->horizontalSound);
			HYDRAULIC_SOUND_CUSTOM(data->turretHydraulicSettings->horizontalStopSound, destTurretHydraulicSettings->horizontalStopSound);
#undef HYDRAULIC_SOUND_CUSTOM

			buf->clear_pointer(&dest->turretHydraulicSettings);
		}

		WEAPON_SUBASSET(overlay.shader, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderLowRes, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderEMP, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL, Material);
		
		WEAPON_SUBASSET(killIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(dpadIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(hudProximityWarningIcon, ASSET_TYPE_MATERIAL, Material);

		WEAPON_STRING(projectileName);

		WEAPON_SUBASSET(projectileModel, ASSET_TYPE_XMODEL, XModel);

		WEAPON_SUBASSET(projExplosionEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projDudEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(projExplosionSound);
		WEAPON_SOUND_CUSTOM(projDudSound);

		if (data->parallelBounce)
		{
			buf->align(3);
			buf->write(data->parallelBounce, 53);
			buf->clear_pointer(&dest->parallelBounce);
		}

		if (data->perpendicularBounce)
		{
			buf->align(3);
			buf->write(data->perpendicularBounce, 53);
			buf->clear_pointer(&dest->perpendicularBounce);
		}

		WEAPON_SUBASSET(projTrailEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projBeaconEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projIgnitionEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(projIgnitionSound);

		for (auto i = 0; i < 2; i++)
		{
			WEAPON_STRING(accuracyGraphName[i]);

			if (data->accuracyGraphKnots[i])
			{
				buf->align(3);
				buf->write(data->accuracyGraphKnots[i], data->accuracyGraphKnotCount[i]);
				buf->clear_pointer(&dest->accuracyGraphKnots[i]);
			}

			if (data->originalAccuracyGraphKnots[i])
			{
				buf->align(3);
				buf->write(data->originalAccuracyGraphKnots[i], data->accuracyGraphKnotCount[i]);
				buf->clear_pointer(&dest->originalAccuracyGraphKnots[i]);
			}
		}

		WEAPON_STRING(szScript);

		buf->pop_stream();
	}

#define WEAPON_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__

#define WEAPON_DUMP_STRING WEAPON_DUMP_FIELD

#define WEAPON_DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

#define WEAPON_DUMP_FIELD_ARR_STR(__field__, __size__) \
	for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
	{ \
		if (asset->__field__[idx##__field__] != nullptr) \
			data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
		else \
			data[#__field__][idx##__field__] = ""; \
	}

#define WEAPON_DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

#define WEAPON_DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			if (asset->__field__[idx##__field__]) \
			{ \
				data[#__field__][idx##__field__] = asset->__field__[idx##__field__]->name; \
			} \
			else \
			{ \
				data[#__field__][idx##__field__] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_ANIM_ARR(__field__, __size__) \
	if (asset->__field__) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto name##__field__ = get_anim_name_from_index(idx##__field__); \
			if (asset->__field__[idx##__field__]) \
			{ \
				data[#__field__][name##__field__] = asset->__field__[idx##__field__]->name; \
			} \
			else \
			{ \
				data[#__field__][name##__field__] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_SOUND(__field__) \
	if (asset->__field__) \
	{ \
		data["sounds"][#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data["sounds"][#__field__] = ""; \
	}

	ordered_json dump_overlay(ADSOverlay* asset)
	{
		ordered_json data;

		WEAPON_DUMP_ASSET(shader);
		WEAPON_DUMP_ASSET(shaderLowRes);
		WEAPON_DUMP_ASSET(shaderEMP);
		WEAPON_DUMP_ASSET(shaderEMPLowRes);
		WEAPON_DUMP_FIELD(reticle);
		WEAPON_DUMP_FIELD(width);
		WEAPON_DUMP_FIELD(height);
		WEAPON_DUMP_FIELD(widthSplitscreen);
		WEAPON_DUMP_FIELD(heightSplitscreen);

		return data;
	}

	json dump_turret_hydraulic_settings(TurretHydraulicSettings* settings)
	{
		json data;

		data["minVelocity"] = settings->minVelocity;
		data["maxVelocity"] = settings->maxVelocity;
		data["verticalSound"] = settings->verticalSound ? settings->verticalSound->name : "";
		data["verticalStopSound"] = settings->verticalStopSound ? settings->verticalStopSound->name : "";
		data["horizontalSound"] = settings->horizontalSound ? settings->horizontalSound->name : "";
		data["horizontalStopSound"] = settings->horizontalStopSound ? settings->horizontalStopSound->name : "";

		return data;
	}

	json dump_accuracy_graph(WeaponDef* asset)
	{
		json data;

		for (auto i = 0; i < 2; i++)
		{
			if (asset->accuracyGraphName[i])
			{
				data["accuracyGraphName"][i] = asset->accuracyGraphName[i];
			}
			else
			{
				data["accuracyGraphName"][i] = "";
			}

			for (auto o = 0; o < asset->accuracyGraphKnotCount[i]; o++)
			{
				if (asset->accuracyGraphKnots[i])
				{
					data["accuracyGraphKnots"][i][o][0] = asset->accuracyGraphKnots[i][o][0];
					data["accuracyGraphKnots"][i][o][1] = asset->accuracyGraphKnots[i][o][1];
				}
				else
				{
					data["accuracyGraphKnots"] = nullptr;
				}
				if (asset->accuracyGraphKnots[i])
				{
					data["originalAccuracyGraphKnots"][i][o][0] = asset->originalAccuracyGraphKnots[i][o][0];
					data["originalAccuracyGraphKnots"][i][o][1] = asset->originalAccuracyGraphKnots[i][o][1];
				}
				else
				{
					data["originalAccuracyGraphKnots"] = nullptr;
				}
			}
		}

		return data;
	}

	json dump_statetimers(StateTimers* asset)
	{
		json data;

		WEAPON_DUMP_FIELD(fireDelay);
		WEAPON_DUMP_FIELD(meleeDelay);
		WEAPON_DUMP_FIELD(meleeChargeDelay);
		WEAPON_DUMP_FIELD(detonateDelay);
		WEAPON_DUMP_FIELD(fireTime);
		WEAPON_DUMP_FIELD(rechamberTime);
		WEAPON_DUMP_FIELD(rechamberTimeOneHanded);
		WEAPON_DUMP_FIELD(rechamberBoltTime);
		WEAPON_DUMP_FIELD(holdFireTime);
		WEAPON_DUMP_FIELD(grenadePrimeReadyToThrowTime);
		WEAPON_DUMP_FIELD(detonateTime);
		WEAPON_DUMP_FIELD(meleeTime);
		WEAPON_DUMP_FIELD(meleeChargeTime);
		WEAPON_DUMP_FIELD(reloadTime);
		WEAPON_DUMP_FIELD(reloadShowRocketTime);
		WEAPON_DUMP_FIELD(reloadEmptyTime);
		WEAPON_DUMP_FIELD(reloadAddTime);
		WEAPON_DUMP_FIELD(reloadEmptyAddTime);
		WEAPON_DUMP_FIELD(reloadStartTime);
		WEAPON_DUMP_FIELD(reloadStartAddTime);
		WEAPON_DUMP_FIELD(reloadEndTime);
		WEAPON_DUMP_FIELD(reloadTimeDualWield);
		WEAPON_DUMP_FIELD(reloadAddTimeDualWield);
		WEAPON_DUMP_FIELD(reloadEmptyDualMag);
		WEAPON_DUMP_FIELD(reloadEmptyAddTimeDualMag);
		//WEAPON_DUMP_FIELD(u25);
		//WEAPON_DUMP_FIELD(u26);
		WEAPON_DUMP_FIELD(dropTime);
		WEAPON_DUMP_FIELD(raiseTime);
		WEAPON_DUMP_FIELD(altDropTime);
		WEAPON_DUMP_FIELD(altRaiseTime);
		WEAPON_DUMP_FIELD(quickDropTime);
		WEAPON_DUMP_FIELD(quickRaiseTime);
		WEAPON_DUMP_FIELD(firstRaiseTime);
		WEAPON_DUMP_FIELD(breachRaiseTime);
		WEAPON_DUMP_FIELD(emptyRaiseTime);
		WEAPON_DUMP_FIELD(emptyDropTime);
		WEAPON_DUMP_FIELD(sprintInTime);
		WEAPON_DUMP_FIELD(sprintLoopTime);
		WEAPON_DUMP_FIELD(sprintOutTime);
		WEAPON_DUMP_FIELD(stunnedTimeBegin);
		WEAPON_DUMP_FIELD(stunnedTimeLoop);
		WEAPON_DUMP_FIELD(stunnedTimeEnd);
		WEAPON_DUMP_FIELD(nightVisionWearTime);
		WEAPON_DUMP_FIELD(nightVisionWearTimeFadeOutEnd);
		WEAPON_DUMP_FIELD(nightVisionWearTimePowerUp);
		WEAPON_DUMP_FIELD(nightVisionRemoveTime);
		WEAPON_DUMP_FIELD(nightVisionRemoveTimePowerDown);
		WEAPON_DUMP_FIELD(nightVisionRemoveTimeFadeInStart);
		WEAPON_DUMP_FIELD(aiFuseTime);
		WEAPON_DUMP_FIELD(fuseTime);
		WEAPON_DUMP_FIELD(missileTime);
		WEAPON_DUMP_FIELD(primeTime);
		WEAPON_DUMP_FIELD(bHoldFullPrime);
		WEAPON_DUMP_FIELD(bHoldFullPrime);
		WEAPON_DUMP_FIELD(blastRightTime);
		WEAPON_DUMP_FIELD(blastBackTime);
		WEAPON_DUMP_FIELD(blastLeftTime);
		//WEAPON_DUMP_FIELD(u58);
		//WEAPON_DUMP_FIELD(u59);
		//WEAPON_DUMP_FIELD(u60);
		//WEAPON_DUMP_FIELD(u61);
		//WEAPON_DUMP_FIELD(u62);
		//WEAPON_DUMP_FIELD(u63);
		//WEAPON_DUMP_FIELD(u64);
		//WEAPON_DUMP_FIELD(u65);
		//WEAPON_DUMP_FIELD(u66);
		//WEAPON_DUMP_FIELD(u67);
		//WEAPON_DUMP_FIELD(u68);
		WEAPON_DUMP_FIELD(offhandSwitchTime);
		//WEAPON_DUMP_FIELD(u70);
		//WEAPON_DUMP_FIELD(u71);
		//WEAPON_DUMP_FIELD(u72);
		//WEAPON_DUMP_FIELD(u73);
		//WEAPON_DUMP_FIELD(u74);

		return data;
	}

	void weapon_def::dump(WeaponDef* asset)
	{
		const auto path = "weapons\\"s + asset->name + ".json"s;

		ordered_json data;

		data["baseAsset"] = asset->szInternalName;

		WEAPON_DUMP_STRING(szInternalName);
		WEAPON_DUMP_STRING(szDisplayName);
		WEAPON_DUMP_STRING(szOverlayName);

		WEAPON_DUMP_ASSET_ARR(gunModel, 2);
		WEAPON_DUMP_ASSET_ARR(worldModel, 2);

		WEAPON_DUMP_ASSET(camoWorldModelMaterialOverride);
		WEAPON_DUMP_ASSET(camoViewModelMaterialOverride);

		WEAPON_DUMP_ASSET(handModel);
		WEAPON_DUMP_ASSET(unknownModel);
		WEAPON_DUMP_ASSET(worldClipModel);
		WEAPON_DUMP_ASSET(rocketModel);
		WEAPON_DUMP_ASSET(knifeModel);
		WEAPON_DUMP_ASSET(worldKnifeModel);

		WEAPON_DUMP_ANIM_ARR(szXAnimsRightHanded, 180);
		WEAPON_DUMP_ANIM_ARR(szXAnimsLeftHanded, 180);
		WEAPON_DUMP_ANIM_ARR(szXAnims, 180);

		for (auto i = 0; i < 32; i++)
		{
			if (asset->hideTags && asset->hideTags[i])
			{
				data["hideTags"][i] = SL_ConvertToString(asset->hideTags[i]);
			}
			else
			{
				data["hideTags"][i] = "";
			}
		}

		WEAPON_DUMP_ASSET_ARR(attachments, asset->numWeaponAttachments);

		for (auto i = 0u; i < asset->numAnimOverrides; i++)
		{
			data["animOverrides"][i]["altmodeAnim"] = (asset->animOverrides[i].altmodeAnim)
				? asset->animOverrides[i].altmodeAnim->name
				: "";
			data["animOverrides"][i]["overrideAnim"] = (asset->animOverrides[i].overrideAnim)
				? asset->animOverrides[i].overrideAnim->name
				: "";
			data["animOverrides"][i]["attachment1"] = asset->animOverrides[i].attachment1;
			data["animOverrides"][i]["attachment2"] = asset->animOverrides[i].attachment2;
			data["animOverrides"][i]["altTime"] = asset->animOverrides[i].altTime;
			data["animOverrides"][i]["animTime"] = asset->animOverrides[i].animTime;
			//data["animOverrides"][i]["animTreeType"] = asset->animOverrides[i].animTreeType;
		}

		for (auto i = 0u; i < asset->numSoundOverrides; i++)
		{
			data["soundOverrides"][i]["altmodeSound"] = (asset->soundOverrides[i].altmodeSound)
				? asset->soundOverrides[i].altmodeSound->name
				: "";
			data["soundOverrides"][i]["attachment1"] = asset->soundOverrides[i].attachment1;
			data["soundOverrides"][i]["attachment2"] = asset->soundOverrides[i].attachment2;
			data["soundOverrides"][i]["overrideSound"] = (asset->soundOverrides[i].overrideSound)
				? asset->soundOverrides[i].overrideSound->name
				: "";
			//data["soundOverrides"][i]["soundType"] = asset->soundOverrides[i].soundType;
		}

		for (auto i = 0u; i < asset->numFXOverrides; i++)
		{
			data["fxOverrides"][i]["altmodeFX"] = (asset->fxOverrides[i].altmodeFX)
				? asset->fxOverrides[i].altmodeFX->name
				: "";
			data["fxOverrides"][i]["attachment1"] = asset->fxOverrides[i].attachment1;
			data["fxOverrides"][i]["attachment2"] = asset->fxOverrides[i].attachment2;
			//data["fxOverrides"][i]["fxType"] = asset->fxOverrides[i].fxType;
			data["fxOverrides"][i]["overrideFX"] = (asset->fxOverrides[i].overrideFX)
				? asset->fxOverrides[i].overrideFX->name
				: "";
		}

		for (auto i = 0u; i < asset->numReloadStateTimerOverrides; i++)
		{
			data["reloadOverrides"][i]["attachment"] = asset->reloadOverrides[i].attachment;
			data["reloadOverrides"][i]["reloadAddTime"] = asset->reloadOverrides[i].reloadAddTime;
			data["reloadOverrides"][i]["reloadEmptyAddTime"] = asset->reloadOverrides[i].reloadEmptyAddTime;
			data["reloadOverrides"][i]["reloadStartAddTime"] = asset->reloadOverrides[i].reloadStartAddTime;
		}

		for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
		{
			data["notetrackOverrides"][i]["attachment"] = asset->notetrackOverrides[i].attachment;

			for (auto j = 0u; j < 36; j++)
			{
				data["notetrackOverrides"][i]["notetrackSoundMapKeys"][j] = (asset->notetrackOverrides[i].notetrackSoundMapKeys[j])
					? SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapKeys[j])
					: "";

				data["notetrackOverrides"][i]["notetrackSoundMapValues"][j] = (asset->notetrackOverrides[i].notetrackSoundMapValues[j])
					? SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapValues[j])
					: "";
			}
		}

		for (auto i = 0; i < 36; i++)
		{
			if (asset->notetrackSoundMapKeys && asset->notetrackSoundMapKeys[i])
			{
				data["notetrackSoundMapKeys"][i] = SL_ConvertToString(asset->notetrackSoundMapKeys[i]);
			}
			else
			{
				data["notetrackSoundMapKeys"][i] = "";
			}

			if (asset->notetrackSoundMapValues && asset->notetrackSoundMapValues[i])
			{
				data["notetrackSoundMapValues"][i] = SL_ConvertToString(asset->notetrackSoundMapValues[i]);
			}
			else
			{
				data["notetrackSoundMapValues"][i] = "";
			}
		}

		for (auto i = 0; i < 16; i++)
		{
			if (asset->notetrackRumbleMapKeys && asset->notetrackRumbleMapKeys[i])
			{
				data["notetrackRumbleMapKeys"][i] = SL_ConvertToString(asset->notetrackRumbleMapKeys[i]);
			}
			else
			{
				data["notetrackRumbleMapKeys"][i] = "";
			}

			if (asset->notetrackRumbleMapValues && asset->notetrackRumbleMapValues[i])
			{
				data["notetrackRumbleMapValues"][i] = SL_ConvertToString(asset->notetrackRumbleMapValues[i]);
			}
			else
			{
				data["notetrackRumbleMapValues"][i] = "";
			}
		}

		for (auto i = 0; i < 16; i++)
		{
			if (asset->notetrackFXMapKeys && asset->notetrackFXMapKeys[i])
			{
				data["notetrackFXMapKeys"][i] = SL_ConvertToString(asset->notetrackFXMapKeys[i]);
			}
			else
			{
				data["notetrackFXMapKeys"][i] = "";
			}

			if (asset->notetrackFXMapValues && asset->notetrackFXMapValues[i])
			{
				data["notetrackFXMapValues"][i] = asset->notetrackFXMapValues[i]->name;
			}
			else
			{
				data["notetrackFXMapValues"][i] = "";
			}

			if (asset->notetrackFXMapTagValues && asset->notetrackFXMapTagValues[i])
			{
				data["notetrackFXMapTagValues"][i] = SL_ConvertToString(asset->notetrackFXMapTagValues[i]);
			}
			else
			{
				data["notetrackFXMapTagValues"][i] = "";
			}
		}

		WEAPON_DUMP_STRING(szModeName);
		WEAPON_DUMP_STRING(szAltWeaponName);
		WEAPON_DUMP_STRING(szAmmoName);
		WEAPON_DUMP_STRING(szClipName);
		WEAPON_DUMP_STRING(szSharedAmmoCapName);

		WEAPON_DUMP_ASSET(viewFlashEffect);
		WEAPON_DUMP_ASSET(viewBodyFlashEffect);
		WEAPON_DUMP_ASSET(worldFlashEffect);
		WEAPON_DUMP_ASSET(viewFlashADSEffect);
		WEAPON_DUMP_ASSET(viewBodyFlashADSEffect);
		WEAPON_DUMP_ASSET(effect06);
		WEAPON_DUMP_ASSET(effect07);
		WEAPON_DUMP_ASSET(effect08);
		WEAPON_DUMP_ASSET(effect09);
		WEAPON_DUMP_ASSET(effect10);
		WEAPON_DUMP_ASSET(effect11);
		WEAPON_DUMP_ASSET(effect12);

		WEAPON_DUMP_SOUND(pickupSound);
		WEAPON_DUMP_SOUND(pickupSoundPlayer);
		WEAPON_DUMP_SOUND(ammoPickupSound);
		WEAPON_DUMP_SOUND(ammoPickupSoundPlayer);
		WEAPON_DUMP_SOUND(projectileSound);
		WEAPON_DUMP_SOUND(pullbackSound);
		WEAPON_DUMP_SOUND(pullbackSoundPlayer);
		WEAPON_DUMP_SOUND(pullbackSoundQuick);
		WEAPON_DUMP_SOUND(pullbackSoundQuickPlayer);
		WEAPON_DUMP_SOUND(fireSound);
		WEAPON_DUMP_SOUND(fireSoundPlayer);
		WEAPON_DUMP_SOUND(fireSoundPlayerAkimbo);
		WEAPON_DUMP_SOUND(sound13);
		WEAPON_DUMP_SOUND(sound14);
		WEAPON_DUMP_SOUND(sound15);
		WEAPON_DUMP_SOUND(sound16);
		WEAPON_DUMP_SOUND(fireLoopSound);
		WEAPON_DUMP_SOUND(fireLoopSoundPlayer);
		WEAPON_DUMP_SOUND(sound19);
		WEAPON_DUMP_SOUND(sound20);
		WEAPON_DUMP_SOUND(sound21);
		WEAPON_DUMP_SOUND(sound22);
		WEAPON_DUMP_SOUND(fireLoopEndPointSound);
		WEAPON_DUMP_SOUND(fireLoopEndPointSoundPlayer);
		WEAPON_DUMP_SOUND(fireStopSound);
		WEAPON_DUMP_SOUND(fireStopSoundPlayer);
		WEAPON_DUMP_SOUND(sound27);
		WEAPON_DUMP_SOUND(sound28);
		WEAPON_DUMP_SOUND(sound29);
		WEAPON_DUMP_SOUND(sound30);
		WEAPON_DUMP_SOUND(fireLastShotSound);
		WEAPON_DUMP_SOUND(fireLastShotSoundPlayer);
		WEAPON_DUMP_SOUND(fireFirstSound);
		WEAPON_DUMP_SOUND(fireFirstSound);
		WEAPON_DUMP_SOUND(fireFirstSoundPlayer);
		WEAPON_DUMP_SOUND(fireLastSound);
		WEAPON_DUMP_SOUND(fireLastSoundPlayer);
		WEAPON_DUMP_SOUND(emptyFireSound);
		WEAPON_DUMP_SOUND(emptyFireSoundPlayer);
		WEAPON_DUMP_SOUND(sound39);
		WEAPON_DUMP_SOUND(meleeSwipeSound);
		WEAPON_DUMP_SOUND(meleeSwipeSoundPlayer);
		WEAPON_DUMP_SOUND(meleeHitSound);
		WEAPON_DUMP_SOUND(meleeHitSoundPlayer);
		WEAPON_DUMP_SOUND(meleeMissSound);
		WEAPON_DUMP_SOUND(meleeMissSoundPlayer);
		WEAPON_DUMP_SOUND(rechamberSound);
		WEAPON_DUMP_SOUND(rechamberSoundPlayer);
		WEAPON_DUMP_SOUND(reloadSound);
		WEAPON_DUMP_SOUND(reloadSoundPlayer);
		WEAPON_DUMP_SOUND(reloadEmptySound);
		WEAPON_DUMP_SOUND(reloadEmptySoundPlayer);
		WEAPON_DUMP_SOUND(reloadStartSound);
		WEAPON_DUMP_SOUND(reloadStartSoundPlayer);
		WEAPON_DUMP_SOUND(reloadEndSound);
		WEAPON_DUMP_SOUND(reloadEndSoundPlayer);
		WEAPON_DUMP_SOUND(detonateSound);
		WEAPON_DUMP_SOUND(detonateSoundPlayer);
		WEAPON_DUMP_SOUND(nightVisionWearSound);
		WEAPON_DUMP_SOUND(nightVisionWearSoundPlayer);
		WEAPON_DUMP_SOUND(nightVisionRemoveSound);
		WEAPON_DUMP_SOUND(nightVisionRemoveSoundPlayer);
		WEAPON_DUMP_SOUND(raiseSound);
		WEAPON_DUMP_SOUND(raiseSoundPlayer);
		WEAPON_DUMP_SOUND(sound64);
		WEAPON_DUMP_SOUND(sound65);
		WEAPON_DUMP_SOUND(sound66);
		WEAPON_DUMP_SOUND(sound67);
		WEAPON_DUMP_SOUND(putawaySound);
		WEAPON_DUMP_SOUND(putawaySoundPlayer);
		WEAPON_DUMP_SOUND(sound70);
		WEAPON_DUMP_SOUND(sound71);
		WEAPON_DUMP_SOUND(adsEnterSoundPlayer);
		WEAPON_DUMP_SOUND(adsLeaveSoundPlayer);
		WEAPON_DUMP_SOUND(adsCrosshairEnemySound);

		WEAPON_DUMP_ASSET_ARR(bounceSound, 53);
		WEAPON_DUMP_ASSET_ARR(rollingSound, 53);

		WEAPON_DUMP_ASSET(viewShellEjectEffect);
		WEAPON_DUMP_ASSET(worldShellEjectEffect);
		WEAPON_DUMP_ASSET(viewLastShotEjectEffect);
		WEAPON_DUMP_ASSET(worldLastShotEjectEffect);
		WEAPON_DUMP_ASSET(viewMagEjectEffect);

		WEAPON_DUMP_ASSET(reticleCenter);
		WEAPON_DUMP_ASSET(reticleSide);

		WEAPON_DUMP_ASSET(hudIcon);
		WEAPON_DUMP_ASSET(pickupIcon);
		WEAPON_DUMP_ASSET(unknownIcon2);
		WEAPON_DUMP_ASSET(unknownIcon3);
		WEAPON_DUMP_ASSET(unknownIcon4);
		WEAPON_DUMP_ASSET(ammoCounterIcon);

		WEAPON_DUMP_ASSET(physCollmap);
		WEAPON_DUMP_ASSET(physPreset);

		WEAPON_DUMP_STRING(szUseHintString);
		WEAPON_DUMP_STRING(dropHintString);

		WEAPON_DUMP_FIELD_ARR(locationDamageMultipliers, 22);

		WEAPON_DUMP_STRING(fireRumble);
		WEAPON_DUMP_STRING(fireMedRumble);
		WEAPON_DUMP_STRING(fireHighRumble);
		WEAPON_DUMP_STRING(meleeImpactRumble);

		WEAPON_DUMP_ASSET(tracer1);
		WEAPON_DUMP_ASSET(tracer2);

		WEAPON_DUMP_ASSET(laser);

		WEAPON_DUMP_ASSET(turretOverheatSound);
		WEAPON_DUMP_ASSET(turretOverheatEffect);
		WEAPON_DUMP_STRING(turretBarrelSpinRumble);
		WEAPON_DUMP_ASSET(turretBarrelSpinMaxSnd);
		WEAPON_DUMP_ASSET_ARR(turretBarrelSpinUpSnd, 4);
		WEAPON_DUMP_ASSET_ARR(turretBarrelSpinDownSnd, 4);
		WEAPON_DUMP_ASSET(missileConeSoundAlias);
		WEAPON_DUMP_ASSET(missileConeSoundAliasAtBase);

		WEAPON_DUMP_ASSET(stowOffsetModel);

		WEAPON_DUMP_ASSET(killIcon);
		WEAPON_DUMP_ASSET(dpadIcon);
		WEAPON_DUMP_ASSET(hudProximityWarningIcon);

		WEAPON_DUMP_STRING(projectileName);
		WEAPON_DUMP_ASSET(projectileModel);
		WEAPON_DUMP_ASSET(projExplosionEffect);
		WEAPON_DUMP_ASSET(projDudEffect);
		WEAPON_DUMP_ASSET(projExplosionSound);
		WEAPON_DUMP_ASSET(projDudSound);
		WEAPON_DUMP_ASSET(projTrailEffect);
		WEAPON_DUMP_ASSET(projBeaconEffect);
		WEAPON_DUMP_ASSET(projIgnitionEffect);
		WEAPON_DUMP_ASSET(projIgnitionSound);

		WEAPON_DUMP_STRING(szScript);

		if (asset->turretHydraulicSettings)
		{
			data["turretHydraulicSettings"] = dump_turret_hydraulic_settings(asset->turretHydraulicSettings);
		}

		data["stateTimers"] = dump_statetimers(&asset->stateTimers);
		data["stateTimersAkimbo"] = dump_statetimers(&asset->akimboStateTimers);

		data["overlay"] = dump_overlay(&asset->overlay);

		data["accuracy_graph"] = dump_accuracy_graph(asset);

		data["unkScriptString"] = SL_ConvertToString(asset->unkScriptString) ? SL_ConvertToString(asset->unkScriptString) : "";

		data["stowTag"] = SL_ConvertToString(asset->stowTag) ? SL_ConvertToString(asset->stowTag) : "";

		WEAPON_DUMP_FIELD(altWeapon);
		WEAPON_DUMP_FIELD(playerAnimType);
		WEAPON_DUMP_FIELD(weapType);
		WEAPON_DUMP_FIELD(weapClass);
		WEAPON_DUMP_FIELD(penetrateType);
		WEAPON_DUMP_FIELD(penetrateDepth);
		WEAPON_DUMP_FIELD(impactType);
		WEAPON_DUMP_FIELD(inventoryType);
		WEAPON_DUMP_FIELD(fireType);
		WEAPON_DUMP_FIELD(fireBarrels);
		WEAPON_DUMP_FIELD(adsFireMode);
		WEAPON_DUMP_FIELD(burstFireCooldown);
		WEAPON_DUMP_FIELD(greebleType);
		WEAPON_DUMP_FIELD(autoReloadType);
		WEAPON_DUMP_FIELD(slotRestriction);
		WEAPON_DUMP_FIELD(offhandClass);
		WEAPON_DUMP_FIELD(stance);
		WEAPON_DUMP_FIELD(reticleCenterSize);
		WEAPON_DUMP_FIELD(reticleSideSize);
		WEAPON_DUMP_FIELD(reticleMinOfs);
		WEAPON_DUMP_FIELD(activeReticleType);
		WEAPON_DUMP_FIELD_ARR(standMove, 3);
		WEAPON_DUMP_FIELD_ARR(standRot, 3);
		WEAPON_DUMP_FIELD_ARR(strafeMove, 3);
		WEAPON_DUMP_FIELD_ARR(strafeRot, 3);
		WEAPON_DUMP_FIELD_ARR(duckedOfs, 3);
		WEAPON_DUMP_FIELD_ARR(duckedMove, 3);
		WEAPON_DUMP_FIELD_ARR(duckedRot, 3);
		WEAPON_DUMP_FIELD_ARR(proneOfs, 3);
		WEAPON_DUMP_FIELD_ARR(proneMove, 3);
		WEAPON_DUMP_FIELD_ARR(proneRot, 3);
		WEAPON_DUMP_FIELD(posMoveRate);
		WEAPON_DUMP_FIELD(posProneMoveRate);
		WEAPON_DUMP_FIELD(standMoveMinSpeed);
		WEAPON_DUMP_FIELD(duckedMoveMinSpeed);
		WEAPON_DUMP_FIELD(proneMoveMinSpeed);
		WEAPON_DUMP_FIELD(posRotRate);
		WEAPON_DUMP_FIELD(posProneRotRate);
		WEAPON_DUMP_FIELD(hudIconRatio);
		WEAPON_DUMP_FIELD(pickupIconRatio);
		WEAPON_DUMP_FIELD(ammoCounterIconRatio);
		WEAPON_DUMP_FIELD(ammoCounterClip);
		WEAPON_DUMP_FIELD(startAmmo);
		WEAPON_DUMP_FIELD(ammoIndex);
		WEAPON_DUMP_FIELD(clipIndex);
		WEAPON_DUMP_FIELD(maxAmmo);
		WEAPON_DUMP_FIELD(minAmmoReq);
		WEAPON_DUMP_FIELD(clipSize);
		WEAPON_DUMP_FIELD(shotCount);
		WEAPON_DUMP_FIELD(sharedAmmoCapIndex);
		WEAPON_DUMP_FIELD(sharedAmmoCap);
		WEAPON_DUMP_FIELD(damage);
		WEAPON_DUMP_FIELD(playerDamage);
		WEAPON_DUMP_FIELD(meleeDamage);
		WEAPON_DUMP_FIELD(damageType);
		WEAPON_DUMP_FIELD(autoAimRange);
		WEAPON_DUMP_FIELD(aimAssistRange);
		WEAPON_DUMP_FIELD(aimAssistRangeAds);
		WEAPON_DUMP_FIELD(aimPadding);
		WEAPON_DUMP_FIELD(enemyCrosshairRange);
		WEAPON_DUMP_FIELD(moveSpeedScale);
		WEAPON_DUMP_FIELD(adsMoveSpeedScale);
		WEAPON_DUMP_FIELD(sprintDurationScale);
		WEAPON_DUMP_FIELD(adsZoomFov);
		WEAPON_DUMP_FIELD(adsZoomInFrac);
		WEAPON_DUMP_FIELD(adsZoomOutFrac);
		WEAPON_DUMP_FIELD(adsSceneBlur);
		//WEAPON_DUMP_FIELD(fU_007);
		//WEAPON_DUMP_FIELD(xU_008);
		WEAPON_DUMP_FIELD(adsBobFactor);
		WEAPON_DUMP_FIELD(adsViewBobMult);
		WEAPON_DUMP_FIELD(hipSpreadStandMin);
		WEAPON_DUMP_FIELD(hipSpreadDuckedMin);
		WEAPON_DUMP_FIELD(hipSpreadProneMin);
		WEAPON_DUMP_FIELD(hipSpreadStandMax);
		//WEAPON_DUMP_FIELD(xU_009);
		//WEAPON_DUMP_FIELD(xU_010);
		WEAPON_DUMP_FIELD(hipSpreadDuckedMax);
		WEAPON_DUMP_FIELD(hipSpreadProneMax);
		WEAPON_DUMP_FIELD(hipSpreadDecayRate);
		WEAPON_DUMP_FIELD(hipSpreadFireAdd);
		WEAPON_DUMP_FIELD(hipSpreadTurnAdd);
		WEAPON_DUMP_FIELD(hipSpreadMoveAdd);
		WEAPON_DUMP_FIELD(hipSpreadDuckedDecay);
		WEAPON_DUMP_FIELD(hipSpreadProneDecay);
		WEAPON_DUMP_FIELD(hipReticleSidePos);
		WEAPON_DUMP_FIELD(adsIdleAmount);
		WEAPON_DUMP_FIELD(hipIdleAmount);
		WEAPON_DUMP_FIELD(adsIdleSpeed);
		WEAPON_DUMP_FIELD(hipIdleSpeed);
		WEAPON_DUMP_FIELD(idleCrouchFactor);
		WEAPON_DUMP_FIELD(idleProneFactor);
		WEAPON_DUMP_FIELD(gunMaxPitch);
		WEAPON_DUMP_FIELD(gunMaxYaw);
		WEAPON_DUMP_FIELD(adsIdleLerpStartTime);
		WEAPON_DUMP_FIELD(adsIdleLerpTime);
		WEAPON_DUMP_FIELD(adsTransInTime);
		//WEAPON_DUMP_FIELD(xU_011);
		WEAPON_DUMP_FIELD(adsTransOutTime);
		//WEAPON_DUMP_FIELD(xU_012);
		WEAPON_DUMP_FIELD(swayMaxAngle);
		WEAPON_DUMP_FIELD(swayLerpSpeed);
		WEAPON_DUMP_FIELD(swayPitchScale);
		WEAPON_DUMP_FIELD(swayYawScale);
		WEAPON_DUMP_FIELD(swayVertScale);
		WEAPON_DUMP_FIELD(swayHorizScale);
		WEAPON_DUMP_FIELD(swayShellShockScale);
		WEAPON_DUMP_FIELD(adsSwayMaxAngle);
		WEAPON_DUMP_FIELD(adsSwayLerpSpeed);
		WEAPON_DUMP_FIELD(adsSwayPitchScale);
		WEAPON_DUMP_FIELD(adsSwayYawScale);
		WEAPON_DUMP_FIELD(adsSwayHorizScale);
		WEAPON_DUMP_FIELD(adsSwayVertScale);
		WEAPON_DUMP_FIELD(adsViewErrorMin);
		WEAPON_DUMP_FIELD(adsViewErrorMax);
		WEAPON_DUMP_FIELD(adsFireAnimFrac);
		WEAPON_DUMP_FIELD(dualWieldViewModelOffset);
		WEAPON_DUMP_FIELD(scopeDriftDelay);
		WEAPON_DUMP_FIELD(scopeDriftLerpInTime);
		WEAPON_DUMP_FIELD(scopeDriftSteadyTime);
		WEAPON_DUMP_FIELD(scopeDriftLerpOutTime);
		WEAPON_DUMP_FIELD(scopeDriftSteadyFactor);
		WEAPON_DUMP_FIELD(scopeDriftUnsteadyFactor);
		WEAPON_DUMP_FIELD(bobVerticalFactor);
		WEAPON_DUMP_FIELD(bobHorizontalFactor);
		WEAPON_DUMP_FIELD(bobViewVerticalFactor);
		WEAPON_DUMP_FIELD(bobViewHorizontalFactor);
		WEAPON_DUMP_FIELD(stationaryZoomFov);
		WEAPON_DUMP_FIELD(stationaryZoomDelay);
		WEAPON_DUMP_FIELD(stationaryZoomLerpInTime);
		WEAPON_DUMP_FIELD(stationaryZoomLerpOutTime);
		WEAPON_DUMP_FIELD(adsDofStart);
		WEAPON_DUMP_FIELD(adsDofEnd);
		//WEAPON_DUMP_FIELD(xU_020);
		WEAPON_DUMP_FIELD(killIconRatio);
		WEAPON_DUMP_FIELD(dpadIconRatio);
		WEAPON_DUMP_FIELD(fireAnimLength);
		WEAPON_DUMP_FIELD(fireAnimLengthAkimbo);
		WEAPON_DUMP_FIELD(reloadAmmoAdd);
		WEAPON_DUMP_FIELD(reloadStartAdd);
		WEAPON_DUMP_FIELD(ammoDropStockMin);
		WEAPON_DUMP_FIELD(ammoDropStockMax);
		WEAPON_DUMP_FIELD(ammoDropClipPercentMin);
		WEAPON_DUMP_FIELD(ammoDropClipPercentMax);
		WEAPON_DUMP_FIELD(explosionRadius);
		WEAPON_DUMP_FIELD(explosionRadiusMin);
		WEAPON_DUMP_FIELD(explosionInnerDamage);
		WEAPON_DUMP_FIELD(explosionOuterDamage);
		WEAPON_DUMP_FIELD(damageConeAngle);
		WEAPON_DUMP_FIELD(bulletExplDmgMult);
		WEAPON_DUMP_FIELD(bulletExplRadiusMult);
		WEAPON_DUMP_FIELD(projectileSpeed);
		WEAPON_DUMP_FIELD(projectileSpeedUp);
		WEAPON_DUMP_FIELD(projectileSpeedForward);
		WEAPON_DUMP_FIELD(projectileActivateDist);
		WEAPON_DUMP_FIELD(projLifetime);
		WEAPON_DUMP_FIELD(timeToAccelerate);
		WEAPON_DUMP_FIELD(projectileCurvature);
		//WEAPON_DUMP_FIELD(xU_021);
		WEAPON_DUMP_FIELD(projExplosion);
		WEAPON_DUMP_FIELD(stickiness);
		WEAPON_DUMP_FIELD(lowAmmoWarningThreshold);
		WEAPON_DUMP_FIELD(ricochetChance);
		WEAPON_DUMP_FIELD(riotShieldHealth);
		WEAPON_DUMP_FIELD(riotShieldDamageMult);
		WEAPON_DUMP_FIELD_ARR(parallelBounce, 53);
		WEAPON_DUMP_FIELD_ARR(perpendicularBounce, 53);
		WEAPON_DUMP_FIELD_ARR(projectileColor, 3);
		WEAPON_DUMP_FIELD(guidedMissileType);
		WEAPON_DUMP_FIELD(maxSteeringAccel);
		WEAPON_DUMP_FIELD(projIgnitionDelay);
		WEAPON_DUMP_FIELD(adsAimPitch);
		WEAPON_DUMP_FIELD(adsCrosshairInFrac);
		WEAPON_DUMP_FIELD(adsCrosshairOutFrac);
		WEAPON_DUMP_FIELD(adsGunKickReducedKickBullets);
		WEAPON_DUMP_FIELD(adsGunKickReducedKickPercent);
		WEAPON_DUMP_FIELD(adsGunKickPitchMin);
		WEAPON_DUMP_FIELD(adsGunKickPitchMax);
		WEAPON_DUMP_FIELD(adsGunKickYawMin);
		WEAPON_DUMP_FIELD(adsGunKickYawMax);
		WEAPON_DUMP_FIELD(adsGunKickMagMin);
		WEAPON_DUMP_FIELD(adsGunKickAccel);
		WEAPON_DUMP_FIELD(adsGunKickSpeedMax);
		WEAPON_DUMP_FIELD(adsGunKickSpeedDecay);
		WEAPON_DUMP_FIELD(adsGunKickStaticDecay);
		WEAPON_DUMP_FIELD(adsViewKickPitchMin);
		WEAPON_DUMP_FIELD(adsViewKickPitchMax);
		WEAPON_DUMP_FIELD(adsViewKickYawMin);
		WEAPON_DUMP_FIELD(adsViewKickYawMax);
		WEAPON_DUMP_FIELD(adsViewKickMagMin);
		WEAPON_DUMP_FIELD(adsViewKickCenterSpeed);
		WEAPON_DUMP_FIELD(adsViewScatterMin);
		WEAPON_DUMP_FIELD(adsViewScatterMax);
		WEAPON_DUMP_FIELD(adsSpread);
		WEAPON_DUMP_FIELD(hipGunKickReducedKickBullets);
		WEAPON_DUMP_FIELD(hipGunKickReducedKickPercent);
		WEAPON_DUMP_FIELD(hipGunKickPitchMin);
		WEAPON_DUMP_FIELD(hipGunKickPitchMax);
		WEAPON_DUMP_FIELD(hipGunKickYawMin);
		WEAPON_DUMP_FIELD(hipGunKickYawMax);
		WEAPON_DUMP_FIELD(hipGunKickMagMin);
		WEAPON_DUMP_FIELD(hipGunKickAccel);
		WEAPON_DUMP_FIELD(hipGunKickSpeedMax);
		WEAPON_DUMP_FIELD(hipGunKickSpeedDecay);
		WEAPON_DUMP_FIELD(hipGunKickStaticDecay);
		WEAPON_DUMP_FIELD(hipViewKickPitchMin);
		WEAPON_DUMP_FIELD(hipViewKickPitchMax);
		WEAPON_DUMP_FIELD(hipViewKickYawMin);
		WEAPON_DUMP_FIELD(hipViewKickYawMax);
		WEAPON_DUMP_FIELD(hipViewKickMagMin);
		WEAPON_DUMP_FIELD(hipViewKickCenterSpeed);
		WEAPON_DUMP_FIELD(hipViewScatterMin);
		WEAPON_DUMP_FIELD(hipViewScatterMax);
		//WEAPON_DUMP_FIELD(xU_043);
		WEAPON_DUMP_FIELD(adsReloadTransTime);
		WEAPON_DUMP_FIELD(fightDist);
		WEAPON_DUMP_FIELD(maxDist);
		WEAPON_DUMP_FIELD(positionReloadTransTime);
		WEAPON_DUMP_FIELD(leftArc);
		WEAPON_DUMP_FIELD(rightArc);
		WEAPON_DUMP_FIELD(topArc);
		WEAPON_DUMP_FIELD(bottomArc);
		WEAPON_DUMP_FIELD(accuracy);
		WEAPON_DUMP_FIELD(aiSpread);
		WEAPON_DUMP_FIELD(playerSpread);
		WEAPON_DUMP_FIELD_ARR(minTurnSpeed, 2);
		WEAPON_DUMP_FIELD_ARR(maxTurnSpeed, 2);
		WEAPON_DUMP_FIELD(pitchConvergenceTime);
		WEAPON_DUMP_FIELD(yawConvergenceTime);
		WEAPON_DUMP_FIELD(suppressTime);
		WEAPON_DUMP_FIELD(maxRange);
		WEAPON_DUMP_FIELD(animHorRotateInc);
		WEAPON_DUMP_FIELD(playerPositionDist);
		//WEAPON_DUMP_FIELD(iUseHintStringIndex);
		//WEAPON_DUMP_FIELD(dropHintStringIndex);
		WEAPON_DUMP_FIELD(horizViewJitter);
		WEAPON_DUMP_FIELD(vertViewJitter);
		WEAPON_DUMP_FIELD(scanSpeed);
		WEAPON_DUMP_FIELD(scanAccel);
		WEAPON_DUMP_FIELD(scanPauseTime);
		WEAPON_DUMP_FIELD(minDamage);
		WEAPON_DUMP_FIELD(midDamage);
		WEAPON_DUMP_FIELD(minPlayerDamage);
		WEAPON_DUMP_FIELD(midPlayerDamage);
		WEAPON_DUMP_FIELD(maxDamageRange);
		WEAPON_DUMP_FIELD(minDamageRange);
		//WEAPON_DUMP_FIELD(iU_045);
		//WEAPON_DUMP_FIELD(iU_046);
		//WEAPON_DUMP_FIELD(iU_047);
		//WEAPON_DUMP_FIELD(iU_048);
		//WEAPON_DUMP_FIELD(fU_049);
		//WEAPON_DUMP_FIELD(fU_050);
		WEAPON_DUMP_FIELD(destabilizationRateTime);
		WEAPON_DUMP_FIELD(destabilizationCurvatureMax);
		WEAPON_DUMP_FIELD(destabilizeDistance);
		WEAPON_DUMP_FIELD(turretADSTime);
		WEAPON_DUMP_FIELD(turretFov);
		WEAPON_DUMP_FIELD(turretFovADS);
		WEAPON_DUMP_FIELD(turretScopeZoomRate);
		WEAPON_DUMP_FIELD(turretScopeZoomMin);
		WEAPON_DUMP_FIELD(turretScopeZoomMax);
		//WEAPON_DUMP_FIELD(xU_056);
		//WEAPON_DUMP_FIELD(xU_057);
		//WEAPON_DUMP_FIELD(xU_058);
		//WEAPON_DUMP_FIELD(xU_059);
		WEAPON_DUMP_FIELD(turretBarrelSpinSpeed);
		WEAPON_DUMP_FIELD(turretBarrelSpinUpTime);
		WEAPON_DUMP_FIELD(turretBarrelSpinDownTime);
		WEAPON_DUMP_FIELD(missileConeSoundRadiusAtTop);
		WEAPON_DUMP_FIELD(missileConeSoundRadiusAtBase);
		WEAPON_DUMP_FIELD(missileConeSoundHeight);
		WEAPON_DUMP_FIELD(missileConeSoundOriginOffset);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleAtCore);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleAtEdge);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleCoreSize);
		WEAPON_DUMP_FIELD(missileConeSoundPitchAtTop);
		WEAPON_DUMP_FIELD(missileConeSoundPitchAtBottom);
		WEAPON_DUMP_FIELD(missileConeSoundPitchTopSize);
		WEAPON_DUMP_FIELD(missileConeSoundPitchBottomSize);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeTopSize);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeBottomSize);
		WEAPON_DUMP_FIELD(aim_automelee_lerp);
		WEAPON_DUMP_FIELD(aim_automelee_range);
		WEAPON_DUMP_FIELD(aim_automelee_region_height);
		WEAPON_DUMP_FIELD(aim_automelee_region_width);
		WEAPON_DUMP_FIELD(player_meleeHeight);
		WEAPON_DUMP_FIELD(player_meleeRange);
		WEAPON_DUMP_FIELD(player_meleeWidth);
		WEAPON_DUMP_FIELD(signatureFireTime);
		WEAPON_DUMP_FIELD(signatureNumBullets);
		WEAPON_DUMP_FIELD(fireTimeInterpolationType);
		//WEAPON_DUMP_FIELD(iU_3408);
		//WEAPON_DUMP_FIELD(iU_3412);
		//WEAPON_DUMP_FIELD(iU_3416);
		//WEAPON_DUMP_FIELD(iU_3420);
		//WEAPON_DUMP_FIELD(iU_3424);
		//WEAPON_DUMP_FIELD(bU_3432);
		//WEAPON_DUMP_FIELD(bU_3433);
		//WEAPON_DUMP_FIELD(bU_3434);
		//WEAPON_DUMP_FIELD(bU_3435);
		//WEAPON_DUMP_FIELD(bU_3437);
		//WEAPON_DUMP_FIELD(bU_3438);
		//WEAPON_DUMP_FIELD(bU_3439);
		WEAPON_DUMP_FIELD(sharedAmmo);
		WEAPON_DUMP_FIELD(lockonSupported);
		WEAPON_DUMP_FIELD(requireLockonToFire);
		WEAPON_DUMP_FIELD(isAirburstWeapon);
		WEAPON_DUMP_FIELD(bigExplosion);
		WEAPON_DUMP_FIELD(noAdsWhenMagEmpty);
		WEAPON_DUMP_FIELD(avoidDropCleanup);
		WEAPON_DUMP_FIELD(inheritsPerks);
		WEAPON_DUMP_FIELD(crosshairColorChange);
		WEAPON_DUMP_FIELD(rifleBullet);
		WEAPON_DUMP_FIELD(armorPiercing);
		WEAPON_DUMP_FIELD(boltAction);
		WEAPON_DUMP_FIELD(aimDownSight);
		WEAPON_DUMP_FIELD(canHoldBreath);
		//WEAPON_DUMP_FIELD(bU_3454);
		//WEAPON_DUMP_FIELD(bU_3455);
		WEAPON_DUMP_FIELD(canVariableZoom);
		WEAPON_DUMP_FIELD(rechamberWhileAds);
		WEAPON_DUMP_FIELD(bulletExplosiveDamage);
		WEAPON_DUMP_FIELD(cookOffHold);
		WEAPON_DUMP_FIELD(reticleSpin45);
		WEAPON_DUMP_FIELD(reticleSideEnabled);
		WEAPON_DUMP_FIELD(clipOnly);
		WEAPON_DUMP_FIELD(noAmmoPickup);
		WEAPON_DUMP_FIELD(disableSwitchToWhenEmpty);
		//WEAPON_DUMP_FIELD(bU_3465);
		WEAPON_DUMP_FIELD(hasMotionTracker);
		//WEAPON_DUMP_FIELD(bU_3467);
		WEAPON_DUMP_FIELD(noDualWield);
		WEAPON_DUMP_FIELD(flipKillIcon);
		WEAPON_DUMP_FIELD(actionSlotShowAmmo);
		WEAPON_DUMP_FIELD(noPartialReload);
		WEAPON_DUMP_FIELD(segmentedReload);
		WEAPON_DUMP_FIELD(multipleReload);
		WEAPON_DUMP_FIELD(blocksProne);
		WEAPON_DUMP_FIELD(silenced);
		WEAPON_DUMP_FIELD(isRollingGrenade);
		WEAPON_DUMP_FIELD(projExplosionEffectForceNormalUp);
		WEAPON_DUMP_FIELD(projExplosionEffectInheritParentDirection);
		WEAPON_DUMP_FIELD(projImpactExplode);
		WEAPON_DUMP_FIELD(projTrajectoryEvents);
		WEAPON_DUMP_FIELD(projWhizByEnabled);
		WEAPON_DUMP_FIELD(stickToPlayers);
		WEAPON_DUMP_FIELD(stickToVehicles);
		WEAPON_DUMP_FIELD(stickToTurrets);
		WEAPON_DUMP_FIELD(thrownSideways);
		WEAPON_DUMP_FIELD(hasDetonatorEmptyThrow);
		WEAPON_DUMP_FIELD(hasDetonatorDoubleTap);
		WEAPON_DUMP_FIELD(disableFiring);
		WEAPON_DUMP_FIELD(timedDetonation);
		//WEAPON_DUMP_FIELD(bU_3528);
		//WEAPON_DUMP_FIELD(bU_3529);
		WEAPON_DUMP_FIELD(rotate);
		WEAPON_DUMP_FIELD(holdButtonToThrow);
		WEAPON_DUMP_FIELD(freezeMovementWhenFiring);
		WEAPON_DUMP_FIELD(thermalScope);
		WEAPON_DUMP_FIELD(thermalToggle);
		WEAPON_DUMP_FIELD(outlineEnemies);
		WEAPON_DUMP_FIELD(altModeSameWeapon);
		WEAPON_DUMP_FIELD(turretBarrelSpinEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundPitchshiftEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeEnabled);
		WEAPON_DUMP_FIELD(offhandHoldIsCancelable);
		WEAPON_DUMP_FIELD(doNotAllowAttachmentsToOverrideSpread);
		WEAPON_DUMP_FIELD(useFastReloadAnims);
		WEAPON_DUMP_FIELD(dualMagReloadSupported);
		WEAPON_DUMP_FIELD(reloadStopsAlt);
		//WEAPON_DUMP_FIELD(bU_3546);
		WEAPON_DUMP_FIELD(alwaysShatterGlassOnImpact);
		WEAPON_DUMP_FIELD(oldWeapon);
		//WEAPON_DUMP_FIELD(bU_3511);
		//WEAPON_DUMP_FIELD(bU_3512);
		//WEAPON_DUMP_FIELD(bU_3513);
		WEAPON_DUMP_FIELD(hasCounterSilencer);
		//WEAPON_DUMP_FIELD(bU_3515);
		//WEAPON_DUMP_FIELD(bU_3516);
		WEAPON_DUMP_FIELD(disableVariableAutosimRate);
		//WEAPON_DUMP_FIELD(bU_3518);
		//WEAPON_DUMP_FIELD(bU_3519);
		//WEAPON_DUMP_FIELD(bU_3520);
		//WEAPON_DUMP_FIELD(bU_3521);
		//WEAPON_DUMP_FIELD(bU_3522);
		WEAPON_DUMP_FIELD(cloakedWeapon);
		WEAPON_DUMP_FIELD(adsHideWeapon);
		//WEAPON_DUMP_FIELD(bU_3525);
		//WEAPON_DUMP_FIELD(bU_3526);
		//WEAPON_DUMP_FIELD(bU_3527);
		WEAPON_DUMP_FIELD(adsDofPhysicalFStop);
		WEAPON_DUMP_FIELD(adsDofPhysicalFocusDistance);
		// TODO: name and dump rest

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}