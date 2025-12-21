#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "weaponattachment.hpp"

#include "zonetool/h1/assets/weaponattachment.hpp"

#include "zonetool/iw6/functions.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace weaponattachment
		{
			// Overloads
#define COPY_FIELD_1(_field) \
	h1_asset->_field = asset->_field;

#define COPY_FIELD_2(_field, _field2) \
	h1_asset->_field = asset->_field2;

// Chooser + forced expansion (MSVC needs this)
#define _COPY_FIELD_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define COPY_FIELD(...) \
	_EXPAND(_COPY_FIELD_GET_MACRO(__VA_ARGS__, COPY_FIELD_2, COPY_FIELD_1)(__VA_ARGS__))


// Helper: destination field's plain type (no ref, no cv)
#define _FIELD_TYPE(_field) std::remove_cv_t<std::remove_reference_t<decltype(h1_asset->_field)>>

// Overloads
#define COPY_FIELD_CAST_1(_field) \
	(h1_asset->_field) = static_cast<_FIELD_TYPE(_field)>((asset->_field))

#define COPY_FIELD_CAST_2(_field, _field2) \
	(h1_asset->_field) = static_cast<_FIELD_TYPE(_field)>((asset->_field2))

// Chooser + forced expansion (MSVC friendly)
#define _COPY_FIELD_CAST_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define COPY_FIELD_CAST(...) \
	_EXPAND(_COPY_FIELD_CAST_GET_MACRO(__VA_ARGS__, COPY_FIELD_CAST_2, COPY_FIELD_CAST_1)(__VA_ARGS__))


#define REINTERPRET_CAST_1(_field) \
	h1_asset->_field = reinterpret_cast<decltype(h1_asset->_field)>(asset->_field);

#define REINTERPRET_CAST_2(_field, _field2) \
	h1_asset->_field = reinterpret_cast<decltype(h1_asset->_field)>(asset->_field2);

		// Chooser + forced expansion (MSVC needs this)
#define _REINTERPRET_CAST_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define REINTERPRET_CAST(...) \
	_EXPAND(_REINTERPRET_CAST_GET_MACRO(__VA_ARGS__, REINTERPRET_CAST_2, REINTERPRET_CAST_1)(__VA_ARGS__))

			zonetool::h1::WeaponAttachment* convert(WeaponAttachment* asset, utils::memory::allocator& mem)
			{
				const auto h1_asset = mem.allocate<zonetool::h1::WeaponAttachment>();

				REINTERPRET_CAST(szInternalName);
				REINTERPRET_CAST(szDisplayName);
				h1_asset->type = static_cast<zonetool::h1::AttachmentType>(asset->type > ATTACHMENT_UNDERBARREL ? asset->type + 1 : asset->type);
				COPY_FIELD_CAST(weaponType); // same enum values
				COPY_FIELD_CAST(weapClass); // same enum values
				h1_asset->greebleType = zonetool::h1::WEAPON_GREEBLE_NONE;

				if (asset->worldModels)
				{
					h1_asset->worldModels = mem.allocate_array<zonetool::h1::XModel*>(2);
					h1_asset->worldModels[0] = reinterpret_cast<zonetool::h1::XModel*>(asset->worldModels[0]);
					h1_asset->worldModels[1] = nullptr;
				}

				if (asset->viewModels)
				{
					h1_asset->viewModels = mem.allocate_array<zonetool::h1::XModel*>(2);
					h1_asset->viewModels[0] = reinterpret_cast<zonetool::h1::XModel*>(asset->viewModels[0]);
					h1_asset->viewModels[1] = nullptr;
				}

				if (asset->reticleViewModels)
				{
					h1_asset->reticleViewModels = mem.allocate_array<zonetool::h1::XModel*>(64);
					for (auto i = 0; i < 8; i++)
					{
						h1_asset->reticleViewModels[i] = reinterpret_cast<zonetool::h1::XModel*>(asset->reticleViewModels[i]);
					}
				}

				h1_asset->bounceSounds = nullptr;
				h1_asset->rollingSounds = nullptr;

				h1_asset->chargeInfo = nullptr;
				h1_asset->hybridSettings = nullptr;

				h1_asset->hideTags = nullptr;
				h1_asset->showTags = nullptr;

				COPY_FIELD(loadIndex);
				h1_asset->unused1 = 0;
				h1_asset->unused2 = 0;

				h1_asset->isAlternateAmmo = false;
				COPY_FIELD(hideIronSightsWithThisAttachment);
				h1_asset->showMasterRail = false;
				h1_asset->showSideRail = false;
				COPY_FIELD(shareAmmoWithAlt);
				h1_asset->knifeAlwaysAttached = false;
				h1_asset->riotShield = false;
				h1_asset->automaticAttachment = false;
				COPY_FIELD(hideIronSightsWithThisAttachment);

				struct WeaponAttachmentField
				{
					zonetool::h1::WAField field;
					unsigned short offset;
				};
				std::vector<WeaponAttachmentField> fields;
				const auto addField = [&](zonetool::h1::WAFieldCode code, zonetool::h1::WAFieldType type, zonetool::h1::WAFieldParm parm, unsigned short offset, unsigned char index = 0)
				{
					WeaponAttachmentField f{};
					f.field.code = code;
					f.field.type = type;
					f.field.parm = parm;
					f.field.index = index;
					f.offset = offset;
					fields.push_back(f);
				};

				float ammunitionScale = 1.0f;
				float damageScale = 1.0f;
				float damageScaleMin = 1.0f;
				float idleSettingsScale = 1.0f;
				float adsSettingsScale = 1.0f;
				float adsSettingsScaleMain = 1.0f;
				float hipSpreadScale = 1.0f;
				float gunKickScale = 1.0f;
				float viewKickScale = 1.0f;
				float viewCenterScale = 1.0f;

				if (asset->ammunitionScale != 0.0f)
				{
					ammunitionScale = asset->ammunitionScale;
				}
				if (asset->damageScale != 0.0f)
				{
					damageScale = asset->damageScale;
				}
				if (asset->damageScaleMin != 0.0f)
				{
					damageScaleMin = asset->damageScaleMin;
				}
				if (asset->idleSettingsScale != 0.0f)
				{
					idleSettingsScale = asset->idleSettingsScale;
				}
				if (asset->adsSettingsScale != 0.0f)
				{
					adsSettingsScale = asset->adsSettingsScale;
				}
				if (asset->adsSettingsScaleMain != 0.0f)
				{
					adsSettingsScaleMain = asset->adsSettingsScaleMain;
				}
				if (asset->hipSpreadScale != 0.0f)
				{
					hipSpreadScale = asset->hipSpreadScale;
				}
				if (asset->gunKickScale != 0.0f)
				{
					gunKickScale = asset->gunKickScale;
				}
				if (asset->viewKickScale != 0.0f)
				{
					viewKickScale = asset->viewKickScale;
				}
				if (asset->viewKickScale != 0.0f)
				{
					viewKickScale = asset->viewKickScale;
				}
				if (asset->viewCenterScale != 0.0f)
				{
					viewCenterScale = asset->viewCenterScale;
				}

				if (asset->ammogeneral)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->ammogeneral->penetrateType}, offsetof(zonetool::h1::WeaponDef, penetrateType));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->ammogeneral->penetrateMultiplier}, offsetof(zonetool::h1::WeaponDef, penetrateDepth));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT,
						{.p_int = asset->ammogeneral->impactType > 6 ? asset->ammogeneral->impactType + 1 : asset->ammogeneral->impactType}, offsetof(zonetool::h1::WeaponDef, impactType));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT,
						{.p_int = asset->ammogeneral->fireType > 4 ? asset->ammogeneral->fireType + 1 : asset->ammogeneral->fireType}, offsetof(zonetool::h1::WeaponDef, fireType));
					if (asset->ammogeneral->tracerType)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_TRACER, {.string = asset->ammogeneral->tracerType->name}, offsetof(zonetool::h1::WeaponDef, tracerType));
					}
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->ammogeneral->rifleBullet}, offsetof(zonetool::h1::WeaponDef, rifleBullet));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->ammogeneral->armorPiercing}, offsetof(zonetool::h1::WeaponDef, armorPiercing));
				}

				if (asset->sight)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->sight->aimDownSight}, offsetof(zonetool::h1::WeaponDef, aimDownSight));
					//addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, { .p_bool = asset->sight->adsFire }, offsetof(zonetool::h1::WeaponDef, adsFireOnly));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->sight->rechamberWhileAds}, offsetof(zonetool::h1::WeaponDef, rechamberWhileAds));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->sight->noAdsWhenMagEmpty}, offsetof(zonetool::h1::WeaponDef, noAdsWhenMagEmpty));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->sight->canHoldBreath}, offsetof(zonetool::h1::WeaponDef, canHoldBreath));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->sight->canVariableZoom}, offsetof(zonetool::h1::WeaponDef, canVariableZoom));
					h1_asset->showSideRail = asset->sight->hideRailWithThisScope == false;
					h1_asset->showMasterRail = asset->sight->hideRailWithThisScope == false;
				}

				if (asset->reload)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->reload->noPartialReload}, offsetof(zonetool::h1::WeaponDef, noPartialReload));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->reload->segmentedReload}, offsetof(zonetool::h1::WeaponDef, segmentedReload));
				}

				if (asset->addOns)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->addOns->motionTracker}, offsetof(zonetool::h1::WeaponDef, motionTracker));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->addOns->silenced}, offsetof(zonetool::h1::WeaponDef, silenced));
				}

				if (asset->general)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->general->boltAction}, offsetof(zonetool::h1::WeaponDef, boltAction));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->general->inheritsPerks}, offsetof(zonetool::h1::WeaponDef, inheritsPerks));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->general->enemyCrosshairRange}, offsetof(zonetool::h1::WeaponDef, enemyCrosshairRange));
					if (asset->general->reticleCenter)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MATERIAL, {.string = asset->general->reticleCenter->name}, offsetof(zonetool::h1::WeaponDef, reticleCenter));
					}
					if (asset->general->reticleSide)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MATERIAL, {.string = asset->general->reticleSide->name}, offsetof(zonetool::h1::WeaponDef, reticleSide));
					}
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->general->reticleCenterSize}, offsetof(zonetool::h1::WeaponDef, reticleCenterSize));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->general->reticleSideSize}, offsetof(zonetool::h1::WeaponDef, reticleSideSize));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->general->moveSpeedScale}, offsetof(zonetool::h1::WeaponDef, moveSpeedScale));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->general->adsMoveSpeedScale}, offsetof(zonetool::h1::WeaponDef, adsMoveSpeedScale));
				}

				if (asset->aimAssist)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->aimAssist->autoAimRange}, offsetof(zonetool::h1::WeaponDef, autoAimRange));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->aimAssist->aimAssistRange}, offsetof(zonetool::h1::WeaponDef, aimAssistRange));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->aimAssist->aimAssistRangeAds}, offsetof(zonetool::h1::WeaponDef, aimAssistRangeAds));
				}

				if (asset->ammunition)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = (int)std::round(asset->ammunition->maxAmmo * ammunitionScale)}, offsetof(zonetool::h1::WeaponDef, maxAmmo));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = (int)std::round(asset->ammunition->startAmmo * ammunitionScale)}, offsetof(zonetool::h1::WeaponDef, startAmmo));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = (int)std::round(asset->ammunition->clipSize * ammunitionScale)}, offsetof(zonetool::h1::WeaponDef, clipSize));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->ammunition->shotCount}, offsetof(zonetool::h1::WeaponDef, shotCount));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = (int)std::round(asset->ammunition->reloadAmmoAdd * ammunitionScale)}, offsetof(zonetool::h1::WeaponDef, reloadAmmoAdd));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->ammunition->reloadStartAdd}, offsetof(zonetool::h1::WeaponDef, reloadStartAdd));
				}

				if (asset->damage)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->damage->damage}, offsetof(zonetool::h1::WeaponDef, damage));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->damage->minDamage}, offsetof(zonetool::h1::WeaponDef, minDamage));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->damage->meleeDamage}, offsetof(zonetool::h1::WeaponDef, meleeDamage));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->damage->maxDamageRange * damageScale}, offsetof(zonetool::h1::WeaponDef, maxDamageRange));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->damage->minDamageRange * damageScaleMin}, offsetof(zonetool::h1::WeaponDef, minDamageRange));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->damage->playerDamage}, offsetof(zonetool::h1::WeaponDef, playerDamage));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->damage->minPlayerDamage}, offsetof(zonetool::h1::WeaponDef, minPlayerDamage));
				}

				if (asset->locationDamage)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locNone}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_NONE);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locHelmet}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_HELMET);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locHead}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_HEAD);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locNeck}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_NECK);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locTorsoUpper}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_TORSO_UPR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locTorsoLower}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_TORSO_LWR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locRightArmUpper}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_R_ARM_UPR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locRightArmLower}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_L_ARM_UPR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locRightHand}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_R_ARM_LWR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locLeftArmUpper}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_L_ARM_LWR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locLeftArmLower}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_R_HAND);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locLeftHand}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_L_HAND);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locRightLegUpper}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_R_LEG_UPR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locRightLegLower}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_L_LEG_UPR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locRightFoot}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_R_LEG_LWR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locLeftLegUpper}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_L_LEG_LWR);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locLeftLegLower}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_R_FOOT);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locLeftFoot}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_L_FOOT);
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->locationDamage->locGun}, offsetof(zonetool::h1::WeaponDef, locationDamageMultipliers), zonetool::h1::HITLOC_GUN);
				}

				if (asset->idleSettings)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->idleSettings->hipIdleAmount}, offsetof(zonetool::h1::WeaponDef, hipIdleAmount));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->idleSettings->hipIdleSpeed}, offsetof(zonetool::h1::WeaponDef, hipIdleSpeed));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->idleSettings->idleCrouchFactor * idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, idleCrouchFactor));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->idleSettings->idleProneFactor * idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, idleProneFactor));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->idleSettings->adsIdleLerpStartTime * idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsIdleLerpStartTime));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->idleSettings->adsIdleLerpTime * idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsIdleLerpTime));
				}

				if (!asset->shareAmmoWithAlt)
				{
					if (asset->adsSettings)
					{
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsSpread * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsSpread));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsAimPitch * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsAimPitch));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT32, {.p_float = asset->adsSettings->adsTransInTime * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsTransInTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT32, {.p_float = asset->adsSettings->adsTransOutTime * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsTransOutTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = (int)std::round(asset->adsSettings->adsReloadTransTime * adsSettingsScaleMain)}, offsetof(zonetool::h1::WeaponDef, positionReloadTransTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsCrosshairInFrac}, offsetof(zonetool::h1::WeaponDef, adsCrosshairInFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsCrosshairOutFrac}, offsetof(zonetool::h1::WeaponDef, adsCrosshairOutFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsZoomFov * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomFov));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsZoomInFrac * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomInFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsZoomOutFrac * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomOutFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsBobFactor * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsBobFactor));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsViewBobMult * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsViewBobMult));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsFireRateScale}, offsetof(zonetool::h1::WeaponDef, adsViewErrorMin));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettings->adsDamageRangeScale}, offsetof(zonetool::h1::WeaponDef, adsViewErrorMax));
					}
				}
				else
				{
					if (asset->adsSettingsMain)
					{
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsSpread * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsSpread));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsAimPitch * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsAimPitch));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsTransInTime * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsTransInTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsTransOutTime * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsTransOutTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = (int)std::round(asset->adsSettingsMain->adsReloadTransTime * adsSettingsScaleMain)}, offsetof(zonetool::h1::WeaponDef, positionReloadTransTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsCrosshairInFrac}, offsetof(zonetool::h1::WeaponDef, adsCrosshairInFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsCrosshairOutFrac}, offsetof(zonetool::h1::WeaponDef, adsCrosshairOutFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsZoomFov * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomFov));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsZoomInFrac * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomInFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsZoomOutFrac * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomOutFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsBobFactor * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsBobFactor));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsViewBobMult * adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsViewBobMult));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsFireRateScale}, offsetof(zonetool::h1::WeaponDef, adsViewErrorMin));
						addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsSettingsMain->adsDamageRangeScale}, offsetof(zonetool::h1::WeaponDef, adsViewErrorMax));
					}
				}

				if (asset->hipSpread)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadStandMin * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadStandMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadDuckedMin * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadDuckedMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadProneMin * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadProneMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadMax * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadStandMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadDuckedMax * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadDuckedMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadProneMax * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadProneMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadFireAdd * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadFireAdd));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadTurnAdd * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadTurnAdd));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadMoveAdd * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadMoveAdd));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadDecayRate * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadDecayRate));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadDuckedDecay * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadDuckedDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->hipSpread->hipSpreadProneDecay * hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadProneDecay));
				}

				if (asset->gunKick)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = (int)std::round(asset->gunKick->hipGunKickReducedKickBullets * gunKickScale)}, offsetof(zonetool::h1::WeaponDef, hipGunKickReducedKickBullets));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickReducedKickPercent * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickReducedKickPercent));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickPitchMin * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickPitchMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickPitchMax * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickPitchMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickYawMin * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickYawMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickYawMax * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickYawMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickAccel * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickAccel));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickSpeedMax * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickSpeedMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickSpeedDecay * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickSpeedDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->hipGunKickStaticDecay * gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickStaticDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = (int)std::round(asset->gunKick->adsGunKickReducedKickBullets * gunKickScale)}, offsetof(zonetool::h1::WeaponDef, adsGunKickReducedKickBullets));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickReducedKickPercent * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickReducedKickPercent));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickPitchMin * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickPitchMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickPitchMax * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickPitchMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickYawMin * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickYawMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickYawMax * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickYawMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickAccel * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickAccel));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickSpeedMax * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickSpeedMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickSpeedDecay * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickSpeedDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->gunKick->adsGunKickStaticDecay * gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickStaticDecay));
				}

				if (asset->viewKick)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->hipViewKickPitchMin * viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickPitchMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->hipViewKickPitchMax * viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickPitchMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->hipViewKickYawMin * viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickYawMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->hipViewKickYawMax * viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickYawMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->hipViewKickCenterSpeed * viewCenterScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickCenterSpeed));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->adsViewKickPitchMin * viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickPitchMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->adsViewKickPitchMax * viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickPitchMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->adsViewKickYawMin * viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickYawMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->adsViewKickYawMax * viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickYawMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->viewKick->adsViewKickCenterSpeed * viewCenterScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickCenterSpeed));
				}

				if (asset->adsOverlay)
				{
					if (asset->adsOverlay->overlay.shader)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MATERIAL, {.string = asset->adsOverlay->overlay.shader->name}, offsetof(zonetool::h1::WeaponDef, overlay.shader));
					}
					if (asset->adsOverlay->overlay.shaderLowRes)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MATERIAL, {.string = asset->adsOverlay->overlay.shaderLowRes->name}, offsetof(zonetool::h1::WeaponDef, overlay.shaderLowRes));
					}
					if (asset->adsOverlay->overlay.shaderEMP)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MATERIAL, {.string = asset->adsOverlay->overlay.shaderEMP->name}, offsetof(zonetool::h1::WeaponDef, overlay.shaderEMP));
					}
					if (asset->adsOverlay->overlay.shaderEMPLowRes)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MATERIAL, {.string = asset->adsOverlay->overlay.shaderEMPLowRes->name}, offsetof(zonetool::h1::WeaponDef, overlay.shaderEMPLowRes));
					}
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->adsOverlay->overlay.reticle}, offsetof(zonetool::h1::WeaponDef, overlay.reticle));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsOverlay->overlay.width}, offsetof(zonetool::h1::WeaponDef, overlay.width));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsOverlay->overlay.height}, offsetof(zonetool::h1::WeaponDef, overlay.height));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsOverlay->overlay.widthSplitscreen}, offsetof(zonetool::h1::WeaponDef, overlay.widthSplitscreen));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->adsOverlay->overlay.heightSplitscreen}, offsetof(zonetool::h1::WeaponDef, overlay.heightSplitscreen));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->adsOverlay->thermalScope}, offsetof(zonetool::h1::WeaponDef, thermalScope));
				}

				if (asset->ui)
				{
					if (asset->ui->dpadIcon)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MATERIAL, {.string = asset->ui->dpadIcon->name}, offsetof(zonetool::h1::WeaponDef, dpadIcon));
					}
					if (asset->ui->ammoCounterIcon)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MATERIAL, {.string = asset->ui->ammoCounterIcon->name}, offsetof(zonetool::h1::WeaponDef, ammoCounterIcon));
					}
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->ui->dpadIconRatio}, offsetof(zonetool::h1::WeaponDef, dpadIconRatio));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->ui->ammoCounterIconRatio}, offsetof(zonetool::h1::WeaponDef, ammoCounterIconRatio));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->ui->ammoCounterClip}, offsetof(zonetool::h1::WeaponDef, ammoCounterClip));
				}

				if (asset->rumbles)
				{
					if (asset->rumbles->fireRumble)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_STRING, {.string = asset->rumbles->fireRumble}, offsetof(zonetool::h1::WeaponDef, fireRumble));
					}
					if (asset->rumbles->meleeImpactRumble)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_STRING, {.string = asset->rumbles->meleeImpactRumble}, offsetof(zonetool::h1::WeaponDef, meleeImpactRumble));
					}
				}

				if (asset->projectile)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->explosionRadius}, offsetof(zonetool::h1::WeaponDef, explosionRadius));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->explosionInnerDamage}, offsetof(zonetool::h1::WeaponDef, explosionInnerDamage));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->explosionOuterDamage}, offsetof(zonetool::h1::WeaponDef, explosionOuterDamage));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->projectile->damageConeAngle}, offsetof(zonetool::h1::WeaponDef, damageConeAngle));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->projectileSpeed}, offsetof(zonetool::h1::WeaponDef, projectileSpeed));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->projectileSpeedUp}, offsetof(zonetool::h1::WeaponDef, projectileSpeedUp));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->projectileActivateDist}, offsetof(zonetool::h1::WeaponDef, projectileActivateDist));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->projectile->projectileLifetime}, offsetof(zonetool::h1::WeaponDef, projLifetime));
					if (asset->projectile->projectileModel)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_MODEL, {.string = asset->projectile->projectileModel->name}, offsetof(zonetool::h1::WeaponDef, projectileModel));
					}
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->projExplosionType}, offsetof(zonetool::h1::WeaponDef, projExplosion));
					if (asset->projectile->projExplosionEffect)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_FX, {.string = asset->projectile->projExplosionEffect->name}, offsetof(zonetool::h1::WeaponDef, projExplosionEffect));
					}
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->projectile->projExplosionEffectForceNormalUp}, offsetof(zonetool::h1::WeaponDef, projExplosionEffectForceNormalUp));
					if (asset->projectile->projExplosionSound)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_SOUND, {.string = asset->projectile->projExplosionSound->aliasName}, offsetof(zonetool::h1::WeaponDef, projExplosionSound));
					}
					if (asset->projectile->projDudEffect)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_FX, {.string = asset->projectile->projDudEffect->name}, offsetof(zonetool::h1::WeaponDef, projDudEffect));
					}
					if (asset->projectile->projDudSound)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_SOUND, {.string = asset->projectile->projDudSound->aliasName}, offsetof(zonetool::h1::WeaponDef, projDudSound));
					}
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_BOOL, {.p_bool = asset->projectile->projImpactExplode}, offsetof(zonetool::h1::WeaponDef, projImpactExplode));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->projectile->destabilizationRateTime}, offsetof(zonetool::h1::WeaponDef, destabilizationRateTime));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->projectile->destabilizationCurvatureMax}, offsetof(zonetool::h1::WeaponDef, destabilizationCurvatureMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->destabilizeDistance}, offsetof(zonetool::h1::WeaponDef, destabilizeDistance));
					if (asset->projectile->projTrailEffect)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_FX, {.string = asset->projectile->projTrailEffect->name}, offsetof(zonetool::h1::WeaponDef, projTrailEffect));
					}
					addField(zonetool::h1::FIELD_OP_NUMBER_SET, zonetool::h1::WAFIELD_TYPE_INT, {.p_int = asset->projectile->projIgnitionDelay}, offsetof(zonetool::h1::WeaponDef, projIgnitionDelay));
					if (asset->projectile->projIgnitionEffect)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_FX, {.string = asset->projectile->projIgnitionEffect->name}, offsetof(zonetool::h1::WeaponDef, projIgnitionEffect));
					}
					if (asset->projectile->projIgnitionSound)
					{
						addField(zonetool::h1::FIELD_OP_STRING_SET, zonetool::h1::WAFIELD_TYPE_SOUND, {.string = asset->projectile->projIgnitionSound->aliasName}, offsetof(zonetool::h1::WeaponDef, projIgnitionSound));
					}
				}

				if (ammunitionScale != 0.0f && ammunitionScale != 1.0f && asset->ammunition == nullptr)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = ammunitionScale}, offsetof(zonetool::h1::WeaponDef, clipSize));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = ammunitionScale}, offsetof(zonetool::h1::WeaponDef, maxAmmo));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = ammunitionScale}, offsetof(zonetool::h1::WeaponDef, reloadAmmoAdd));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = ammunitionScale}, offsetof(zonetool::h1::WeaponDef, startAmmo));
				}
				if (damageScale != 0.0f && damageScale != 1.0f && asset->damage == nullptr)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = damageScale}, offsetof(zonetool::h1::WeaponDef, maxDamageRange));
				}
				if (damageScaleMin != 0.0f && damageScaleMin != 1.0f && asset->damage == nullptr)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = damageScaleMin}, offsetof(zonetool::h1::WeaponDef, minDamageRange));
				}
				if (asset->stateTimersScale != 0.0f && asset->stateTimersScale != 1.0f)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->stateTimersScale}, offsetof(zonetool::h1::WeaponDef, stateTimers.meleeTime));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->stateTimersScale}, offsetof(zonetool::h1::WeaponDef, stateTimers.meleeDelay));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->stateTimersScale}, offsetof(zonetool::h1::WeaponDef, stateTimers.meleeChargeTime));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->stateTimersScale}, offsetof(zonetool::h1::WeaponDef, stateTimers.meleeChargeDelay));
				}
				if (asset->fireTimersScale != 0.0f && asset->fireTimersScale != 1.0f)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = asset->fireTimersScale}, offsetof(zonetool::h1::WeaponDef, stateTimers.fireTime));
				}
				if (idleSettingsScale != 0.0f && idleSettingsScale != 1.0f)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsIdleAmount));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsIdleSpeed));
				}
				if (idleSettingsScale != 0.0f && idleSettingsScale != 1.0f && asset->idleSettings == nullptr)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, idleCrouchFactor));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, idleProneFactor));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsIdleLerpStartTime));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = idleSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsIdleLerpTime));
				}
				if (!asset->shareAmmoWithAlt)
				{
					if (adsSettingsScale != 0.0f && adsSettingsScale != 1.0f && asset->adsSettings == nullptr)
					{
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsAimPitch));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsBobFactor));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsFireAnimFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, positionReloadTransTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsViewBobMult));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsZoomFov));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsTransInTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsTransOutTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsZoomInFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsZoomOutFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScale}, offsetof(zonetool::h1::WeaponDef, adsSpread));
					}
				}
				else
				{
					if (adsSettingsScaleMain != 0.0f && adsSettingsScaleMain != 1.0f && asset->adsSettingsMain == nullptr)
					{
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsAimPitch));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsBobFactor));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsFireAnimFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, positionReloadTransTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsViewBobMult));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomFov));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsTransInTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsTransOutTime));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomInFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsZoomOutFrac));
						addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = adsSettingsScaleMain}, offsetof(zonetool::h1::WeaponDef, adsSpread));
					}
				}

				if (hipSpreadScale != 0.0f && hipSpreadScale != 1.0f && asset->hipSpread == nullptr)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadStandMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadStandMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadDuckedMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadDuckedMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadProneMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadProneMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadDecayRate));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadProneDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadDuckedDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadTurnAdd));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadMoveAdd));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = hipSpreadScale}, offsetof(zonetool::h1::WeaponDef, hipSpreadFireAdd));
				}
				if (gunKickScale != 0.0f && gunKickScale != 1.0f && asset->gunKick == nullptr)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickReducedKickBullets));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickReducedKickPercent));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickAccel));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickSpeedMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickSpeedDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickStaticDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickAccel));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickSpeedMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickSpeedDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickStaticDecay));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickReducedKickBullets));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickReducedKickPercent));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickPitchMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickPitchMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickYawMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickYawMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, hipGunKickMagMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickPitchMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickPitchMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickYawMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickYawMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = gunKickScale}, offsetof(zonetool::h1::WeaponDef, adsGunKickMagMin));
				}
				if (viewKickScale != 0.0f && viewKickScale != 1.0f && asset->viewKick == nullptr)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickPitchMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickPitchMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickYawMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickYawMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickPitchMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickPitchMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickYawMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickYawMax));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickMagMin));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewKickScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickMagMin));
				}
				if (viewCenterScale != 0.0f && viewCenterScale != 1.0f && asset->viewKick == nullptr)
				{
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewCenterScale}, offsetof(zonetool::h1::WeaponDef, adsViewKickCenterSpeed));
					addField(zonetool::h1::FIELD_OP_NUMBER_MULTIPLY, zonetool::h1::WAFIELD_TYPE_FLOAT, {.p_float = viewCenterScale}, offsetof(zonetool::h1::WeaponDef, hipViewKickCenterSpeed));
				}

				std::sort(fields.begin(), fields.end(), [](const auto a, const auto b)
				{
					if (a.offset == b.offset)
					{
						return a.field.index < b.field.index;
					}

					return a.offset < b.offset;
				});

				h1_asset->numFields = static_cast<unsigned int>(fields.size());
				h1_asset->fields = mem.allocate_array<zonetool::h1::WAField>(h1_asset->numFields);
				h1_asset->fieldOffsets = mem.allocate_array<unsigned short>(h1_asset->numFields);
				for (size_t i = 0; i < fields.size(); i++)
				{
					h1_asset->fields[i] = fields[i].field;
					h1_asset->fieldOffsets[i] = fields[i].offset;
				}

				return h1_asset;
			}

			void dump(WeaponAttachment* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::weapon_attachment::dump(converted_asset);
			}
		}
	}
}
