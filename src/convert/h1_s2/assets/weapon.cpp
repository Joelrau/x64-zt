#include <std_include.hpp>

#include "../include.hpp"
#include "attachment.hpp"
#include "sound.hpp"
#include "techset.hpp"
#include "weapon.hpp"
#include "xanim.hpp"
#include "xmodel.hpp"

namespace zonetool::h1::converter::s2::weapon
{
	namespace
	{
		constexpr std::size_t model_count = 2;
		constexpr std::size_t s2_anim_count = 188;
		constexpr std::size_t hide_tag_count = 32;
		constexpr std::size_t notetrack_sound_count = 36;
		constexpr std::size_t notetrack_map_count = 16;
		constexpr std::size_t location_damage_count = 22;
		constexpr std::size_t h1_surface_type_count = 53;
		constexpr std::size_t s2_surface_type_count = 62;

		std::unordered_set<std::string> dumped_weapons;

		const char* donor_weapon_name(const weapClass_t weap_class)
		{
			switch (weap_class)
			{
			case WEAPCLASS_SNIPER:
				return "kar98_mp";
			case WEAPCLASS_MG:
				return "mg42_mp";
			case WEAPCLASS_SMG:
				return "thompson_mp";
			case WEAPCLASS_SPREAD:
				return "winchester1897_mp";
			case WEAPCLASS_PISTOL:
				return "m1911_mp";
			case WEAPCLASS_GRENADE:
				return "frag_grenade_mp";
			case WEAPCLASS_ROCKETLAUNCHER:
				return "bazooka_mp";
			case WEAPCLASS_THROWINGKNIFE:
				return "throwingknife_mp";
			case WEAPCLASS_SHIELD:
				return "riotshield_mp";
			default:
				return "stg44_mp";
			}
		}

		bool has_same_s2_class(const weapClass_t weap_class)
		{
			return weap_class <= WEAPCLASS_THROWINGKNIFE || weap_class == WEAPCLASS_SHIELD;
		}

		const zonetool::s2::WeaponCompleteDef* find_donor(const char* name)
		{
			for (const auto* asset : techset::common_zone().find_assets(zonetool::s2::ASSET_TYPE_WEAPON))
			{
				const auto* weapon = static_cast<const zonetool::s2::WeaponCompleteDef*>(asset);
				if (weapon->szInternalName && !_stricmp(weapon->szInternalName, name))
				{
					return weapon;
				}
			}

			return nullptr;
		}

		zonetool::s2::scr_string_t convert_script_string(const scr_string_t value)
		{
			return static_cast<zonetool::s2::scr_string_t>(value);
		}

		zonetool::s2::scr_string_t* convert_script_strings(const scr_string_t* values, const std::size_t count,
			utils::memory::allocator& allocator)
		{
			if (!values)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<zonetool::s2::scr_string_t>(count);
			for (auto i = 0u; i < count; i++)
			{
				result[i] = convert_script_string(values[i]);
			}

			return result;
		}

		zonetool::s2::XModel** convert_models(XModel* const* models, utils::memory::allocator& allocator)
		{
			if (!models)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<zonetool::s2::XModel*>(model_count);
			for (auto i = 0u; i < model_count; i++)
			{
				result[i] = xmodel::reference(models[i], allocator);
			}

			return result;
		}

		std::optional<std::size_t> convert_anim_index(const std::size_t h1_index)
		{
			if (h1_index >= NUM_WEAP_ANIMS)
			{
				return std::nullopt;
			}

			return zonetool::s2::weapon_anim_index(anim_names[h1_index]);
		}

		zonetool::s2::XAnimParts** convert_anims(XAnimParts* const* anims, utils::memory::allocator& allocator)
		{
			if (!anims)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<zonetool::s2::XAnimParts*>(s2_anim_count);
			for (auto i = 0u; i < NUM_WEAP_ANIMS; i++)
			{
				if (!anims[i])
				{
					continue;
				}

				const auto index = convert_anim_index(i);
				if (!index)
				{
					ZONETOOL_WARNING("weapon anim \"%s\" (%s) has no s2 slot", anim_names[i], anims[i]->name);
					continue;
				}

				result[*index] = xanim::reference(anims[i], allocator);
			}

			return result;
		}

		template <typename T, typename Convert>
		auto* convert_surface_array(const T* h1_values, utils::memory::allocator& allocator, const Convert& convert_value)
		{
			using value_type = std::remove_cvref_t<std::invoke_result_t<Convert, const T&>>;
			if (!h1_values)
			{
				return static_cast<value_type*>(nullptr);
			}

			const auto result = allocator.allocate_array<value_type>(s2_surface_type_count);
			std::vector<bool> has_source(s2_surface_type_count);
			for (auto i = 0u; i < h1_surface_type_count; i++)
			{
				const auto s2_type = convert_surface_type(i);
				if (!has_source[s2_type])
				{
					result[s2_type] = convert_value(h1_values[i]);
					has_source[s2_type] = true;
				}
			}

			for (auto i = 0u; i < s2_surface_type_count; i++)
			{
				if (!has_source[i])
				{
					result[i] = result[0];
				}
			}

			return result;
		}

		void dump_notetrack_sounds(const scr_string_t* values, const std::size_t count)
		{
			for (auto i = 0u; values && i < count; i++)
			{
				const auto* name = SL_ConvertToString(values[i]);
				if (name && *name && !sound::dump_used(name))
				{
					ZONETOOL_WARNING("notetrack sound \"%s\" not found in the loaded h1 zones", name);
				}
			}
		}

		void convert_attachments(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->numAttachments = 0;
			weapon->attachments = nullptr;
			std::memset(weapon->unknown80, 0, sizeof(weapon->unknown80));
			if (!asset->attachments || !asset->numAttachments)
			{
				return;
			}

			weapon->attachments = allocator.allocate_array<zonetool::s2::WeaponAttachment*>(asset->numAttachments);
			for (auto i = 0u; i < asset->numAttachments; i++)
			{
				weapon->attachments[i] = attachment::reference(asset->attachments[i], allocator);
			}

			weapon->numAttachments = asset->numAttachments;
		}

		std::optional<std::string> attachment_anim_name(const WAField& field, const XAnimParts* base_anim)
		{
			const std::string_view value = field.parm.string ? field.parm.string : "";
			if (field.code == FIELD_OP_STRING_SET)
			{
				return std::string(value);
			}

			if (!base_anim || !base_anim->name)
			{
				return std::nullopt;
			}

			std::string name = base_anim->name;
			if (field.code == FIELD_OP_STRING_APPEND)
			{
				return name + std::string(value);
			}

			const auto separator = value.find(' ');
			if (field.code != FIELD_OP_STRING_REPLACE || separator == std::string_view::npos)
			{
				return std::nullopt;
			}

			const auto from = value.substr(0, separator);
			const auto position = name.find(from);
			if (position == std::string::npos)
			{
				return std::nullopt;
			}

			return name.replace(position, from.size(), value.substr(separator + 1));
		}

		void add_attachment_anim_overrides(const WeaponDef* asset, std::vector<zonetool::s2::AnimOverrideEntry>& overrides,
			utils::memory::allocator& allocator)
		{
			// s2 attachments do not change anims, the weapon has an anim override for each attachment anim
			for (auto i = 0u; asset->attachments && i < asset->numAttachments; i++)
			{
				const auto* attachment = asset->attachments[i] && asset->attachments[i]->name
					? db_find_x_asset_header_safe(ASSET_TYPE_ATTACHMENT, asset->attachments[i]->name).attachment
					: nullptr;
				for (auto f = 0u; attachment && attachment->fields && f < attachment->numFields; f++)
				{
					const auto& field = attachment->fields[f];
					if (field.type != WAFIELD_TYPE_ANIM || field.index >= NUM_WEAP_ANIMS || weapon_field_name(attachment->fieldOffsets[f]) != "szXAnims")
					{
						continue;
					}

					const auto* base_anim = asset->szXAnims ? asset->szXAnims[field.index] : nullptr;
					const auto name = attachment_anim_name(field, base_anim);
					if (name && name->empty())
					{
						continue;
					}

					const auto index = convert_anim_index(field.index);
					const auto* anim = name ? db_find_x_asset_header_safe(ASSET_TYPE_XANIMPARTS, name->data()).parts : nullptr;
					if (!anim || !index)
					{
						ZONETOOL_WARNING("weapon \"%s\": attachment \"%s\" anim %s (\"%s\") is not converted",
							asset->name, attachment->name, anim_names[field.index], name ? name->data() : field.parm.string);
						continue;
					}

					auto& entry = overrides.emplace_back();
					entry.attachment1 = static_cast<unsigned char>(i + 1);
					entry.animTreeType = static_cast<unsigned char>(*index);
					entry.overrideAnim = xanim::reference(anim, allocator);
				}
			}
		}

		void convert_overrides(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			std::vector<zonetool::s2::AnimOverrideEntry> anim_overrides;
			for (auto i = 0u; asset->animOverrides && i < asset->numAnimOverrides; i++)
			{
				const auto& entry = asset->animOverrides[i];
				const auto index = convert_anim_index(entry.animTreeType);
				if (!index)
				{
					continue;
				}

				auto& override_entry = anim_overrides.emplace_back();
				override_entry.animHand = entry.animHand;
				override_entry.attachment1 = entry.attachment1;
				override_entry.attachment2 = entry.attachment2;
				override_entry.animTreeType = static_cast<unsigned char>(*index);
				override_entry.overrideAnim = xanim::reference(entry.overrideAnim, allocator);
				override_entry.altmodeAnim = xanim::reference(entry.altmodeAnim, allocator);
				override_entry.animTime = entry.animTime;
				override_entry.altTime = entry.altTime;
			}

			add_attachment_anim_overrides(asset, anim_overrides, allocator);

			weapon->numAnimOverrides = static_cast<unsigned char>(anim_overrides.size());
			weapon->animOverrides = copy_array(anim_overrides.data(), anim_overrides.size(), allocator);

			weapon->numReloadStateTimerOverrides = asset->numReloadStateTimerOverrides;
			weapon->reloadOverrides = retype<zonetool::s2::ReloadStateTimerEntry>(asset->reloadOverrides);

			weapon->numNotetrackOverrides = asset->numNotetrackOverrides;
			weapon->notetrackOverrides = nullptr;
			if (asset->notetrackOverrides && asset->numNotetrackOverrides)
			{
				weapon->notetrackOverrides = allocator.allocate_array<zonetool::s2::NoteTrackToSoundEntry>(asset->numNotetrackOverrides);
				for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
				{
					const auto& entry = asset->notetrackOverrides[i];
					auto& override_entry = weapon->notetrackOverrides[i];
					override_entry.attachment = entry.attachment;
					override_entry.notetrackSoundMapKeys = convert_script_strings(entry.notetrackSoundMapKeys, notetrack_sound_count, allocator);
					override_entry.notetrackSoundMapValues = convert_script_strings(entry.notetrackSoundMapValues, notetrack_sound_count, allocator);
					dump_notetrack_sounds(entry.notetrackSoundMapValues, notetrack_sound_count);
				}
			}

			if (asset->numSoundOverrides || asset->numFXOverrides)
			{
				ZONETOOL_WARNING("weapon \"%s\": %d sound and %d effect overrides are not converted",
					asset->name, asset->numSoundOverrides, asset->numFXOverrides);
			}

			weapon->numSoundOverrides = 0;
			weapon->soundOverrides = nullptr;
			weapon->numFXOverrides = 0;
			weapon->fxOverrides = nullptr;
		}

		void convert_notetrack_maps(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->notetrackSoundMapKeys = convert_script_strings(asset->notetrackSoundMapKeys, notetrack_sound_count, allocator);
			weapon->notetrackSoundMapValues = convert_script_strings(asset->notetrackSoundMapValues, notetrack_sound_count, allocator);
			dump_notetrack_sounds(asset->notetrackSoundMapValues, notetrack_sound_count);

			// TODO: h1 rumbles and effects are not converted
			weapon->notetrackRumbleMapKeys = allocator.allocate_array<zonetool::s2::scr_string_t>(notetrack_map_count);
			weapon->notetrackRumbleMapValues = allocator.allocate_array<zonetool::s2::scr_string_t>(notetrack_map_count);
			weapon->notetrackFXMapKeys = allocator.allocate_array<zonetool::s2::scr_string_t>(notetrack_map_count);
			weapon->notetrackFXMapValues = allocator.allocate_array<zonetool::s2::FxEffectDef*>(notetrack_map_count);
			weapon->notetrackFXMapTagValues = allocator.allocate_array<zonetool::s2::scr_string_t>(notetrack_map_count);
		}

		void convert_state_timers(const StateTimers& h1, zonetool::s2::StateTimers& s2)
		{
			s2.fireDelay = h1.fireDelay;
			s2.meleeDelay = h1.meleeDelay;
			s2.meleeChargeDelay = h1.meleeChargeDelay;
			s2.detonateDelay = h1.detonateDelay;
			s2.fireTime = h1.fireTime;
			s2.rechamberTime = h1.rechamberTime;
			s2.rechamberTimeOneHanded = h1.rechamberTimeOneHanded;
			s2.rechamberBoltTime = h1.rechamberBoltTime;
			s2.holdFireTime = h1.holdFireTime;
			s2.grenadePrimeReadyToThrowTime = h1.grenadePrimeReadyToThrowTime;
			s2.detonateTime = h1.detonateTime;
			s2.meleeTime = h1.meleeTime;
			s2.meleeChargeTime = h1.meleeChargeTime;
			s2.reloadTime = h1.reloadTime;
			s2.reloadShowRocketTime = h1.reloadShowRocketTime;
			s2.reloadEmptyTime = h1.reloadEmptyTime;
			s2.reloadAddTime = h1.reloadAddTime;
			s2.reloadEmptyAddTime = h1.reloadEmptyAddTime;
			s2.reloadStartTime = h1.reloadStartTime;
			s2.reloadStartAddTime = h1.reloadStartAddTime;
			s2.reloadEndTime = h1.reloadEndTime;
			s2.reloadTimeDualWield = h1.reloadTimeDualWield;
			s2.reloadAddTimeDualWield = h1.reloadAddTimeDualWield;
			s2.reloadEmptyDualMag = h1.reloadEmptyDualMag;
			s2.reloadEmptyAddTimeDualMag = h1.reloadEmptyAddTimeDualMag;
			s2.speedReloadTime = h1.speedReloadTime;
			s2.speedReloadAddTime = h1.speedReloadAddTime;
			s2.dropTime = h1.dropTime;
			s2.raiseTime = h1.raiseTime;
			s2.altDropTime = h1.altDropTime;
			s2.altRaiseTime = h1.altRaiseTime;
			s2.quickDropTime = h1.quickDropTime;
			s2.quickRaiseTime = h1.quickRaiseTime;
			s2.firstRaiseTime = h1.firstRaiseTime;
			s2.breachRaiseTime = h1.breachRaiseTime;
			s2.emptyRaiseTime = h1.emptyRaiseTime;
			s2.emptyDropTime = h1.emptyDropTime;
			s2.sprintInTime = h1.sprintInTime;
			s2.sprintLoopTime = h1.sprintLoopTime;
			s2.sprintOutTime = h1.sprintOutTime;
			s2.stunnedTimeBegin = h1.stunnedTimeBegin;
			s2.stunnedTimeLoop = h1.stunnedTimeLoop;
			s2.stunnedTimeEnd = h1.stunnedTimeEnd;
			s2.nightVisionWearTime = h1.nightVisionWearTime;
			s2.nightVisionWearTimeFadeOutEnd = h1.nightVisionWearTimeFadeOutEnd;
			s2.nightVisionWearTimePowerUp = h1.nightVisionWearTimePowerUp;
			s2.nightVisionRemoveTime = h1.nightVisionRemoveTime;
			s2.nightVisionRemoveTimePowerDown = h1.nightVisionRemoveTimePowerDown;
			s2.nightVisionRemoveTimeFadeInStart = h1.nightVisionRemoveTimeFadeInStart;
			s2.aiFuseTime = h1.aiFuseTime;
			s2.fuseTime = h1.fuseTime;
			s2.missileTime = h1.missileTime;
			s2.primeTime = h1.primeTime;
			s2.bHoldFullPrime = h1.bHoldFullPrime;
			s2.blastFrontTime = h1.blastFrontTime;
			s2.blastRightTime = h1.blastRightTime;
			s2.blastBackTime = h1.blastBackTime;
			s2.blastLeftTime = h1.blastLeftTime;
			s2.slideInTime = h1.slideInTime;
			s2.slideLoopTime = h1.slideLoopTime;
			s2.slideOutTime = h1.slideOutTime;
			s2.offhandSwitchTime = h1.offhandSwitchTime;
		}

		void convert_sounds(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			const auto convert = [&](const snd_alias_list_t* alias)
			{
				return sound::reference(alias, allocator);
			};

			weapon->pickupSound = convert(asset->pickupSound);
			weapon->pickupSoundPlayer = convert(asset->pickupSoundPlayer);
			weapon->ammoPickupSound = convert(asset->ammoPickupSound);
			weapon->ammoPickupSoundPlayer = convert(asset->ammoPickupSoundPlayer);
			weapon->projectileSound = convert(asset->projectileSound);
			weapon->pullbackSound = convert(asset->pullbackSound);
			weapon->pullbackSoundPlayer = convert(asset->pullbackSoundPlayer);
			weapon->fireSound = convert(asset->fireSound);
			weapon->fireSoundPlayer = convert(asset->fireSoundPlayer);
			weapon->fireSoundPlayerAkimbo = convert(asset->fireSoundPlayerAkimbo);
			weapon->fireLoopSound = convert(asset->fireLoopSound);
			weapon->fireLoopSoundPlayer = convert(asset->fireLoopSoundPlayer);
			weapon->fireLoopEndPointSound = convert(asset->fireLoopEndPointSound);
			weapon->fireLoopEndPointSoundPlayer = convert(asset->fireLoopEndPointSoundPlayer);
			weapon->fireStopSound = convert(asset->fireStopSound);
			weapon->fireStopSoundPlayer = convert(asset->fireStopSoundPlayer);
			weapon->fireLastSound = convert(asset->fireLastSound);
			weapon->fireLastSoundPlayer = convert(asset->fireLastSoundPlayer);
			weapon->fireFirstSound = convert(asset->fireFirstSound);
			weapon->fireFirstSoundPlayer = convert(asset->fireFirstSoundPlayer);
			weapon->fireCustomSound = convert(asset->fireCustomSound);
			weapon->fireCustomSoundPlayer = convert(asset->fireCustomSoundPlayer);
			weapon->emptyFireSound = convert(asset->emptyFireSound);
			weapon->emptyFireSoundPlayer = convert(asset->emptyFireSoundPlayer);
			weapon->adsRequiredFireSoundPlayer = convert(asset->adsRequiredFireSoundPlayer);
			weapon->meleeSwipeSound = convert(asset->meleeSwipeSound);
			weapon->meleeSwipeSoundPlayer = convert(asset->meleeSwipeSoundPlayer);
			weapon->meleeHitSound = convert(asset->meleeHitSound);
			weapon->meleeMissSound = convert(asset->meleeMissSound);
			weapon->meleeHitSoundPlayer = convert(asset->meleeHitSoundPlayer);
			weapon->meleeMissSoundPlayer = convert(asset->meleeMissSoundPlayer);
			weapon->rechamberSound = convert(asset->rechamberSound);
			weapon->rechamberSoundPlayer = convert(asset->rechamberSoundPlayer);
			weapon->reloadSound = convert(asset->reloadSound);
			weapon->reloadSoundPlayer = convert(asset->reloadSoundPlayer);
			weapon->reloadEmptySound = convert(asset->reloadEmptySound);
			weapon->reloadEmptySoundPlayer = convert(asset->reloadEmptySoundPlayer);
			weapon->reloadStartSound = convert(asset->reloadStartSound);
			weapon->reloadStartSoundPlayer = convert(asset->reloadStartSoundPlayer);
			weapon->reloadEndSound = convert(asset->reloadEndSound);
			weapon->reloadEndSoundPlayer = convert(asset->reloadEndSoundPlayer);
			weapon->detonateSound = convert(asset->detonateSound);
			weapon->detonateSoundPlayer = convert(asset->detonateSoundPlayer);
			weapon->nightVisionWearSound = convert(asset->nightVisionWearSound);
			weapon->nightVisionWearSoundPlayer = convert(asset->nightVisionWearSoundPlayer);
			weapon->nightVisionRemoveSound = convert(asset->nightVisionRemoveSound);
			weapon->nightVisionRemoveSoundPlayer = convert(asset->nightVisionRemoveSoundPlayer);
			weapon->altSwitchSound = convert(asset->altSwitchSound);
			weapon->altSwitchSoundPlayer = convert(asset->altSwitchSoundPlayer);
			weapon->raiseSound = convert(asset->raiseSound);
			weapon->raiseSoundPlayer = convert(asset->raiseSoundPlayer);
			weapon->firstRaiseSound = convert(asset->firstRaiseSound);
			weapon->firstRaiseSoundPlayer = convert(asset->firstRaiseSoundPlayer);
			weapon->putawaySound = convert(asset->putawaySound);
			weapon->putawaySoundPlayer = convert(asset->putawaySoundPlayer);
			weapon->scanSound = convert(asset->scanSound);
			weapon->changeVariableZoomSound = convert(asset->changeVariableZoomSound);
			weapon->adsUpSound = convert(asset->adsUpSound);
			weapon->adsDownSound = convert(asset->adsDownSound);
			weapon->adsCrosshairEnemySound = convert(asset->adsCrosshairEnemySound);
			weapon->bounceSound = convert_surface_array(asset->bounceSound, allocator, convert);
			weapon->rollingSound = convert_surface_array(asset->rollingSound, allocator, convert);
			weapon->projExplosionSound = convert(asset->projExplosionSound);
			weapon->projDudSound = convert(asset->projDudSound);
			weapon->projIgnitionSound = convert(asset->projIgnitionSound);
			weapon->turretOverheatSound = convert(asset->turretOverheatSound);
			weapon->turretBarrelSpinMaxSnd = convert(asset->turretBarrelSpinMaxSnd);
			for (auto i = 0u; i < std::size(weapon->turretBarrelSpinUpSnd); i++)
			{
				weapon->turretBarrelSpinUpSnd[i] = convert(asset->turretBarrelSpinUpSnd[i]);
				weapon->turretBarrelSpinDownSnd[i] = convert(asset->turretBarrelSpinDownSnd[i]);
			}
			weapon->missileConeSoundAlias = convert(asset->missileConeSoundAlias);
			weapon->missileConeSoundAliasAtBase = convert(asset->missileConeSoundAliasAtBase);
		}

		void convert_weapon_models(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->gunModel = convert_models(asset->gunModel, allocator);
			weapon->worldModel = convert_models(asset->worldModel, allocator);
			weapon->gunModelConditions = allocator.allocate_array<zonetool::s2::XModel*>(model_count); // stock weapons always have the arrays
			weapon->worldModelConditions = allocator.allocate_array<zonetool::s2::XModel*>(model_count);
			weapon->worldClipModel = xmodel::reference(asset->worldClipModel, allocator);
			weapon->rocketModel = xmodel::reference(asset->rocketModel, allocator);
			weapon->projectileModel = xmodel::reference(asset->projectileModel, allocator);
			weapon->stowOffsetModel = xmodel::reference(asset->stowOffsetModel, allocator);
			weapon->lobWorldModelName = asset->lobWorldModelName;
		}

		void convert_damage(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->damageFalloffLerpNear = false;
			weapon->damageFalloffLerpFar = false;
			weapon->damage = asset->damage;
			weapon->playerDamage = asset->playerDamage;
			weapon->maxDamageRange = asset->maxDamageRange;
			weapon->mid1Damage = asset->midDamage;
			weapon->mid1PlayerDamage = asset->midPlayerDamage;
			weapon->mid1DamageRange = asset->minDamageRange;
			weapon->mid2Damage = asset->minDamage;
			weapon->mid2PlayerDamage = asset->minPlayerDamage;
			weapon->minDamage = asset->minDamage;
			weapon->minPlayerDamage = asset->minPlayerDamage;
			weapon->minDamageRange = asset->minDamageRange;
			weapon->meleeDamage = asset->meleeDamage;
			weapon->damageType = asset->damageType;
			weapon->signatureDamage = asset->signatureDamage;
			weapon->signatureMid1Damage = asset->signatureMidDamage;
			weapon->signatureMinDamage = asset->signatureMinDamage;
			weapon->signatureMaxDamageRange = asset->signatureMaxDamageRange;
			weapon->signatureMinDamageRange = asset->signatureMinDamageRange;
			weapon->locationDamageMultipliers = asset->locationDamageMultipliers;
			weapon->explosionRadius = asset->explosionRadius;
			weapon->explosionRadiusMin = asset->explosionRadiusMin;
			weapon->explosionInnerDamage = asset->explosionInnerDamage;
			weapon->explosionOuterDamage = asset->explosionOuterDamage;
			weapon->damageConeAngle = asset->damageConeAngle;
			weapon->riotShieldHealth = asset->riotShieldHealth;
			weapon->riotShieldDamageMult = asset->riotShieldDamageMult;
		}

		void convert_movement(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			std::memcpy(weapon->standMove, asset->standMove, sizeof(weapon->standMove));
			std::memcpy(weapon->standRot, asset->standRot, sizeof(weapon->standRot));
			std::memcpy(weapon->strafeMove, asset->strafeMove, sizeof(weapon->strafeMove));
			std::memcpy(weapon->strafeRot, asset->strafeRot, sizeof(weapon->strafeRot));
			std::memcpy(weapon->duckedOfs, asset->duckedOfs, sizeof(weapon->duckedOfs));
			std::memcpy(weapon->duckedMove, asset->duckedMove, sizeof(weapon->duckedMove));
			std::memcpy(weapon->duckedRot, asset->duckedRot, sizeof(weapon->duckedRot));
			std::memcpy(weapon->proneOfs, asset->proneOfs, sizeof(weapon->proneOfs));
			std::memcpy(weapon->proneMove, asset->proneMove, sizeof(weapon->proneMove));
			std::memcpy(weapon->proneRot, asset->proneRot, sizeof(weapon->proneRot));
			weapon->posMoveRate = asset->posMoveRate;
			weapon->posProneMoveRate = asset->posProneMoveRate;
			weapon->standMoveMinSpeed = asset->standMoveMinSpeed;
			weapon->duckedMoveMinSpeed = asset->duckedMoveMinSpeed;
			weapon->proneMoveMinSpeed = asset->proneMoveMinSpeed;
			weapon->posRotRate = asset->posRotRate;
			weapon->posProneRotRate = asset->posProneRotRate;
			weapon->moveSpeedScale = asset->moveSpeedScale;
			weapon->adsMoveSpeedScale = asset->adsMoveSpeedScale;
			weapon->sprintDurationScale = asset->sprintDurationScale;
		}

		void convert_ads(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->adsZoomFov = asset->adsZoomFov;
			weapon->adsZoomInFrac = asset->adsZoomInFrac;
			weapon->adsZoomOutFrac = asset->adsZoomOutFrac;
			weapon->adsSceneBlur = asset->adsSceneBlurStrength;
			weapon->adsBobFactor = asset->adsBobFactor;
			weapon->adsViewBobMult = asset->adsViewBobMult;
			weapon->adsIdleAmount = asset->adsIdleAmount;
			weapon->hipIdleAmount = asset->hipIdleAmount;
			weapon->adsIdleSpeed = asset->adsIdleSpeed;
			weapon->hipIdleSpeed = asset->hipIdleSpeed;
			weapon->idleCrouchFactor = asset->idleCrouchFactor;
			weapon->idleProneFactor = asset->idleProneFactor;
			weapon->gunMaxPitch = asset->gunMaxPitch;
			weapon->gunMaxYaw = asset->gunMaxYaw;
			weapon->adsTransInTime = asset->adsTransInTime;
			weapon->adsTransInFromSprintTime = asset->adsTransInFromSprintTime;
			weapon->adsTransOutTime = asset->adsTransOutTime;
			weapon->swayMaxAngle = asset->swayMaxAngle;
			weapon->swayMaxAngleSteadyAim = asset->swayMaxAngleSteadyAim;
			weapon->swayLerpSpeed = asset->swayLerpSpeed;
			weapon->swayPitchScale = asset->swayPitchScale;
			weapon->swayYawScale = asset->swayYawScale;
			weapon->swayHorizScale = asset->swayHorizScale;
			weapon->swayVertScale = asset->swayVertScale;
			weapon->swayShellShockScale = asset->swayShellShockScale;
			weapon->adsSwayMaxAngle = asset->adsSwayMaxAngle;
			weapon->adsSwayLerpSpeed = asset->adsSwayLerpSpeed;
			weapon->adsSwayPitchScale = asset->adsSwayPitchScale;
			weapon->adsSwayYawScale = asset->adsSwayYawScale;
			weapon->adsSwayHorizScale = asset->adsSwayHorizScale;
			weapon->adsSwayVertScale = asset->adsSwayVertScale;
			weapon->adsViewErrorMin = asset->adsViewErrorMin;
			weapon->adsViewErrorMax = asset->adsViewErrorMax;
			weapon->adsFireAnimFrac = asset->adsFireAnimFrac;
			weapon->scopeDriftDelay = asset->scopeDriftDelay;
			weapon->scopeDriftLerpInTime = asset->scopeDriftLerpInTime;
			weapon->scopeDriftSteadyTime = asset->scopeDriftSteadyTime;
			weapon->scopeDriftLerpOutTime = asset->scopeDriftLerpOutTime;
			weapon->scopeDriftSteadyFactor = asset->scopeDriftSteadyFactor;
			weapon->scopeDriftUnsteadyFactor = asset->scopeDriftUnsteadyFactor;
			weapon->bobVerticalFactor = asset->bobVerticalFactor;
			weapon->bobHorizontalFactor = asset->bobHorizontalFactor;
			weapon->bobViewVerticalFactor = asset->bobViewVerticalFactor;
			weapon->bobViewHorizontalFactor = asset->bobViewHorizontalFactor;
			weapon->stationaryZoomFov = asset->stationaryZoomFov;
			weapon->stationaryZoomDelay = asset->stationaryZoomDelay;
			weapon->stationaryZoomLerpInTime = asset->stationaryZoomLerpInTime;
			weapon->stationaryZoomLerpOutTime = asset->stationaryZoomLerpOutTime;
			weapon->adsDofStart = asset->adsDofStart;
			weapon->adsDofEnd = asset->adsDofEnd;
			weapon->adsDofPhysicalFstop = asset->adsDofPhysicalFstop;
			weapon->adsDofPhysicalFocusDistance = asset->adsDofPhysicalFocusDistance;
			weapon->adsAimPitch = asset->adsAimPitch;
			weapon->adsCrosshairInFrac = asset->adsCrosshairInFrac;
			weapon->adsCrosshairOutFrac = asset->adsCrosshairOutFrac;
			weapon->adsSpread = asset->adsSpread;
			weapon->adsShouldShowCrosshair = asset->adsShouldShowCrosshair;
			weapon->adsCrosshairShouldScale = asset->adsCrosshairShouldScale;
			weapon->aimDownSight = asset->aimDownSight;
			weapon->noAdsWhenMagEmpty = asset->noAdsWhenMagEmpty;
			weapon->canHoldBreath = asset->canHoldBreath;
			weapon->overlay.width = asset->overlay.width;
			weapon->overlay.height = asset->overlay.height;
			weapon->overlay.widthSplitscreen = asset->overlay.widthSplitscreen;
			weapon->overlay.heightSplitscreen = asset->overlay.heightSplitscreen;
		}

		void convert_spread(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->hipSpreadDecayRate = asset->hipSpreadDecayRate;
			weapon->hipSpreadStandMin = asset->hipSpreadStandMin;
			weapon->hipSpreadStandMax = asset->hipSpreadStandMax;
			weapon->hipSpreadDuckedMin = asset->hipSpreadDuckedMin;
			weapon->hipSpreadDuckedMax = asset->hipSpreadDuckedMax;
			weapon->hipSpreadDuckedDecay = asset->hipSpreadDuckedDecay;
			weapon->hipSpreadProneMin = asset->hipSpreadProneMin;
			weapon->hipSpreadProneMax = asset->hipSpreadProneMax;
			weapon->hipSpreadProneDecay = asset->hipSpreadProneDecay;
			weapon->hipSpreadSprintMax = asset->hipSpreadSprintMax;
			weapon->hipSpreadSlideMax = asset->hipSpreadSlideMax;
			weapon->hipSpreadMoveAdd = asset->hipSpreadMoveAdd;
			weapon->hipSpreadTurnAdd = asset->hipSpreadTurnAdd;
			weapon->hipSpreadFireAdd = asset->hipSpreadFireAdd;
			weapon->hipReticleSidePos = asset->hipReticleSidePos;
			weapon->doNotAllowAttachmentsToOverrideSpread = asset->doNotAllowAttachmentsToOverrideSpread;
		}

		void convert_kick(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->adsGunKickPitchMin = asset->adsGunKickPitchMin;
			weapon->adsGunKickPitchMax = asset->adsGunKickPitchMax;
			weapon->adsGunKickYawMin = asset->adsGunKickYawMin;
			weapon->adsGunKickYawMax = asset->adsGunKickYawMax;
			weapon->adsGunKickMagMin = asset->adsGunKickMagMin;
			weapon->adsGunKickAccel = asset->adsGunKickAccel;
			weapon->adsGunKickSpeedMax = asset->adsGunKickSpeedMax;
			weapon->adsGunKickSpeedDecay = asset->adsGunKickSpeedDecay;
			weapon->adsGunKickStaticDecay = asset->adsGunKickStaticDecay;
			weapon->adsGunKickReducedKickBullets = asset->adsGunKickReducedKickBullets;
			weapon->adsGunKickReducedKickPercent = asset->adsGunKickReducedKickPercent;
			weapon->adsViewKickPitchMin = asset->adsViewKickPitchMin;
			weapon->adsViewKickPitchMax = asset->adsViewKickPitchMax;
			weapon->adsViewKickYawMin = asset->adsViewKickYawMin;
			weapon->adsViewKickYawMax = asset->adsViewKickYawMax;
			weapon->adsViewKickMagMin = asset->adsViewKickMagMin;
			weapon->adsViewKickCenterSpeed = asset->adsViewKickCenterSpeed;
			weapon->adsViewScatterMin = asset->adsViewScatterMin;
			weapon->adsViewScatterMax = asset->adsViewScatterMax;
			weapon->hipGunKickPitchMin = asset->hipGunKickPitchMin;
			weapon->hipGunKickPitchMax = asset->hipGunKickPitchMax;
			weapon->hipGunKickYawMin = asset->hipGunKickYawMin;
			weapon->hipGunKickYawMax = asset->hipGunKickYawMax;
			weapon->hipGunKickMagMin = asset->hipGunKickMagMin;
			weapon->hipGunKickAccel = asset->hipGunKickAccel;
			weapon->hipGunKickSpeedMax = asset->hipGunKickSpeedMax;
			weapon->hipGunKickSpeedDecay = asset->hipGunKickSpeedDecay;
			weapon->hipGunKickStaticDecay = asset->hipGunKickStaticDecay;
			weapon->hipGunKickReducedKickBullets = asset->hipGunKickReducedKickBullets;
			weapon->hipGunKickReducedKickPercent = asset->hipGunKickReducedKickPercent;
			weapon->hipViewKickPitchMin = asset->hipViewKickPitchMin;
			weapon->hipViewKickPitchMax = asset->hipViewKickPitchMax;
			weapon->hipViewKickYawMin = asset->hipViewKickYawMin;
			weapon->hipViewKickYawMax = asset->hipViewKickYawMax;
			weapon->hipViewKickMagMin = asset->hipViewKickMagMin;
			weapon->hipViewKickCenterSpeed = asset->hipViewKickCenterSpeed;
			weapon->hipViewScatterMin = asset->hipViewScatterMin;
			weapon->hipViewScatterMax = asset->hipViewScatterMax;

			// s2 switches to the alt kick after adsViewKickAltBullets
			weapon->adsViewKickAltBullets = 0;
			weapon->hipViewKickAltBullets = 0;
		}

		void convert_projectile(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->projectileName = asset->projectileName;
			weapon->projectileSpeed = asset->projectileSpeed;
			weapon->projectileSpeedUp = asset->projectileSpeedUp;
			weapon->projectileSpeedForward = asset->projectileSpeedForward;
			weapon->projectileActivateDist = asset->projectileActivateDist;
			weapon->projLifetime = asset->projLifetime;
			weapon->timeToAccelerate = asset->timeToAccelerate;
			weapon->projectileCurvature = asset->projectileCurvature;
			weapon->projExplosion = asset->projExplosion;
			weapon->stickiness = asset->stickiness;
			weapon->ricochetChance = asset->ricochetChance;
			weapon->parallelBounce = convert_surface_array(asset->parallelBounce, allocator, std::identity{});
			weapon->perpendicularBounce = convert_surface_array(asset->perpendicularBounce, allocator, std::identity{});
			std::memcpy(weapon->projectileColor, asset->projectileColor, sizeof(weapon->projectileColor));
			weapon->guidedMissileType = asset->guidedMissileType;
			weapon->maxSteeringAccel = asset->maxSteeringAccel;
			weapon->projIgnitionDelay = asset->projIgnitionDelay;
			weapon->destabilizationRateTime = asset->destabilizationRateTime;
			weapon->destabilizationCurvatureMax = asset->destabilizationCurvatureMax;
			weapon->destabilizeDistance = asset->destabilizeDistance;
			weapon->isRollingGrenade = asset->isRollingGrenade;
			weapon->projExplosionEffectForceNormalUp = asset->projExplosionEffectForceNormalUp;
			weapon->projExplosionEffectInheritParentDirection = asset->projExplosionEffectInheritParentDirection;
			weapon->projImpactExplode = asset->projImpactExplode;
			weapon->stickToPlayers = asset->stickToPlayers;
			weapon->stickToVehicles = asset->stickToVehicles;
			weapon->stickToTurrets = asset->stickToTurrets;
			weapon->thrownSideways = asset->thrownSideways;
			weapon->detonatesOnEmptyThrow = asset->hasDetonatorEmptyThrow;
			weapon->detonatesOnDoubleTap = asset->hasDetonatorDoubleTap;
			weapon->timedDetonation = asset->timedDetonation;
			weapon->fuseLitAfterImpact = asset->fuseLitAfterImpact;
			weapon->rotate = asset->rotate;
			weapon->holdButtonToThrow = asset->holdButtonToThrow;
			weapon->bigExplosion = asset->bigExplosion;
			weapon->isAirburstWeapon = asset->isAirburstWeapon;
			weapon->projSuppressedByEMP = asset->projSuppressedByEMP;
			weapon->projDisabledByEMP = asset->projDisabledByEMP;
			weapon->projPlayTrailEffectForOwnerOnly = asset->projPlayTrailEffectForOwnerOnly;
			weapon->projPlayBeaconEffectForOwnerOnly = asset->projPlayBeaconEffectForOwnerOnly;
			weapon->projKillTrailEffectOnDeath = asset->projKillTrailEffectOnDeath;
			weapon->projKillBeaconEffectOnDeath = asset->projKillBeaconEffectOnDeath;
		}

		void convert_ammo(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->szAmmoName = asset->szAmmoName;
			weapon->szClipName = asset->szClipName;
			weapon->szSharedAmmoCapName = asset->szSharedAmmoCapName;
			weapon->maxAmmo = asset->maxAmmo;
			weapon->minAmmoReq = asset->minAmmoReq;
			weapon->startAmmo = asset->startAmmo;
			weapon->clipSize = asset->clipSize;
			weapon->shotCount = asset->shotCount;
			weapon->sharedAmmoCapIndex = asset->sharedAmmoCapIndex;
			weapon->sharedAmmoCap = asset->sharedAmmoCap;
			weapon->reloadAmmoAdd = asset->reloadAmmoAdd;
			weapon->reloadStartAdd = asset->reloadStartAdd;
			weapon->ammoDropStockMin = asset->ammoDropStockMin;
			weapon->ammoDropStockMax = asset->ammoDropStockMax;
			weapon->ammoDropClipPercentMin = asset->ammoDropClipPercentMin;
			weapon->ammoDropClipPercentMax = asset->ammoDropClipPercentMax;
			weapon->lowAmmoWarningThreshold = asset->lowAmmoWarningThreshold;
			weapon->generateAmmo = asset->generateAmmo;
			weapon->ammoPerShot = asset->ammoPerShot;
			weapon->explodeCount = asset->explodeCount;
			weapon->clipOnly = asset->clipOnly;
			weapon->noAmmoPickup = asset->noAmmoPickup;
			weapon->noPartialReload = asset->noPartialReload;
			weapon->segmentedReload = asset->segmentedReload;
			weapon->multipleReload = asset->multipleReload;
			weapon->useDualMagReloadAnims = asset->dualMagReloadSupported;

			// s2 gives the ammo as clips when these are set, h1 gives it as rounds
			weapon->maxAmmoClips = 0;
			weapon->startAmmoClips = 0;
		}

		void convert_turret(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->leftArc = asset->leftArc;
			weapon->rightArc = asset->rightArc;
			weapon->topArc = asset->topArc;
			weapon->bottomArc = asset->bottomArc;
			weapon->turretADSTime = asset->turretADSTime;
			weapon->turretFov = asset->turretFov;
			weapon->turretFovADS = asset->turretFovADS;
			weapon->turretScopeZoomRate = asset->turretScopeZoomRate;
			weapon->turretScopeZoomMin = asset->turretScopeZoomMin;
			weapon->turretScopeZoomMax = asset->turretScopeZoomMax;
			weapon->overheatUpRate = asset->overheatUpRate;
			weapon->overheatDownRate = asset->overheatDownRate;
			weapon->overheatCooldownRate = asset->overheatCooldownRate;
			weapon->overheatPenalty = asset->overheatPenalty;
			weapon->turretBarrelSpinSpeed = asset->turretBarrelSpinSpeed;
			weapon->turretBarrelSpinUpTime = asset->turretBarrelSpinUpTime;
			weapon->turretADSEnabled = asset->turretADSEnabled;
			weapon->turretBarrelSpinEnabled = asset->turretBarrelSpinEnabled;
			weapon->turretHydraulicSettings = nullptr;
		}

		void convert_flags(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->rattleSoundType = asset->rattleSoundType;
			weapon->meleeOverrideValues = asset->meleeOverrideValues;
			weapon->lockonSupported = asset->lockonSupported;
			weapon->requireLockonToFire = asset->requireLockonToFire;
			weapon->inheritsPerks = asset->inheritsPerks;
			weapon->crosshairColorChange = asset->crosshairColorChange;
			weapon->rifleBullet = asset->rifleBullet;
			weapon->armorPiercing = asset->armorPiercing;
			weapon->boltAction = asset->boltAction;
			weapon->bulletExplosiveDamage = asset->bulletExplosiveDamage;
			weapon->cookOffHold = asset->cookOffHold;
			weapon->reticleSpin45 = asset->reticleSpin45;
			weapon->motionTracker = asset->motionTracker;
			weapon->markableViewmodel = asset->markableViewmodel;
			weapon->flipKillIcon = asset->flipKillIcon;
			weapon->dpadIconShowsAmmo = asset->actionSlotShowAmmo;
			weapon->blocksProne = asset->blocksProne;
			weapon->silenced = asset->silenced;
			weapon->disableFiring = asset->disableFiring;
			weapon->freezeMovementWhenFiring = asset->freezeMovementWhenFiring;
			weapon->offhandHoldIsCancelable = asset->offhandHoldIsCancelable;
			weapon->alwaysShatterGlassOnImpact = asset->alwaysShatterGlassOnImpact;
			weapon->counterSilencer = asset->counterSilencer;
			weapon->cloaked = asset->cloaked;
			weapon->useScriptCallbackForHit = asset->useScriptCallbackForHit;
			weapon->hasTransientModels = false; // TODO: tr fastfile support later on will change this
		}

		void convert_aim(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->autoAimRange = asset->autoAimRange;
			weapon->aimAssistRange = asset->aimAssistRange;
			weapon->aimAssistRangeAds = asset->aimAssistRangeAds;
			weapon->aimPadding = asset->aimPadding;
			weapon->enemyCrosshairRange = asset->enemyCrosshairRange;
			weapon->reticleCenterSize = asset->reticleCenterSize;
			weapon->reticleSideSize = asset->reticleSideSize;
			weapon->reticleMinOfs = asset->reticleMinOfs;
			weapon->accuracy = asset->accuracy;
			weapon->aiSpread = asset->aiSpread;
			weapon->playerSpread = asset->playerSpread;
			std::memcpy(weapon->minTurnSpeed, asset->minTurnSpeed, sizeof(weapon->minTurnSpeed));
			std::memcpy(weapon->maxTurnSpeed, asset->maxTurnSpeed, sizeof(weapon->maxTurnSpeed));
			weapon->pitchConvergenceTime = asset->pitchConvergenceTime;
			weapon->yawConvergenceTime = asset->yawConvergenceTime;
			weapon->suppressTime = asset->suppressTime;
			weapon->maxRange = asset->maxRange;
			weapon->animHorRotateInc = asset->animHorRotateInc;
			weapon->playerPositionDist = asset->playerPositionDist;
			weapon->horizViewJitter = asset->horizViewJitter;
			weapon->vertViewJitter = asset->vertViewJitter;
			weapon->scanSpeed = asset->scanSpeed;
			weapon->scanAccel = asset->scanAccel;
			weapon->scanPauseTime = asset->scanPauseTime;
			weapon->aim_automelee_lerp = asset->aim_automelee_lerp;
			weapon->aim_automelee_range = asset->aim_automelee_range;
			weapon->aim_automelee_region_width = asset->aim_automelee_region_width;
			weapon->aim_automelee_region_height = asset->aim_automelee_region_height;
			weapon->player_meleeHeight = asset->player_meleeHeight;
			weapon->player_meleeRange = asset->player_meleeRange;
			weapon->player_meleeWidth = asset->player_meleeWidth;
			weapon->accuracyGraphName[0] = asset->accuracyGraphName[0];
			weapon->accuracyGraphName[1] = asset->accuracyGraphName[1];
			for (auto i = 0u; i < std::size(weapon->accuracyGraphKnots); i++)
			{
				weapon->accuracyGraphKnots[i] = asset->accuracyGraphKnots[i];
				weapon->originalAccuracyGraphKnots[i] = asset->originalAccuracyGraphKnots[i];
				weapon->accuracyGraphKnotCount[i] = asset->accuracyGraphKnotCount[i];
				weapon->originalAccuracyGraphKnotCount[i] = asset->originalAccuracyGraphKnotCount[i];
			}
		}

		void convert_type(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->weapType = asset->weapType;
			if (has_same_s2_class(asset->weapClass))
			{
				weapon->weapClass = asset->weapClass;
			}

			weapon->inventoryType = asset->inventoryType;
			weapon->fireType = asset->fireType;
			weapon->fireBarrels = asset->fireBarrels;
			weapon->adsFireMode = asset->adsFireMode;
			weapon->burstFireCooldown = asset->burstFireCooldown;
			weapon->greebleType = asset->greebleType;
			weapon->autoReloadType = asset->autoReloadType;
			weapon->autoHolsterType = asset->autoHolsterType;
			weapon->offhandClass = asset->offhandClass;
			weapon->stance = asset->stance;
			weapon->penetrateDepth = asset->penetrateDepth;
			weapon->fireAnimLength = asset->fireAnimLength;
			weapon->fireAnimLengthAkimbo = asset->fireAnimLengthAkimbo;
			weapon->positionReloadTransTime = asset->positionReloadTransTime;
			weapon->changedFireTime = asset->changedFireTime;
			weapon->changedFireTimeNumBullets = asset->changedFireTimeNumBullets;
			weapon->fireTimeInterpolationType = asset->fireTimeInterpolationType;
		}
	}

	void dump(WeaponDef* asset)
	{
		if (!dumped_weapons.insert(asset->name).second)
		{
			return;
		}

		const auto* donor_name = donor_weapon_name(asset->weapClass);
		const auto* donor = find_donor(donor_name);
		if (!donor)
		{
			ZONETOOL_ERROR("weapon \"%s\": donor weapon \"%s\" not found in s2 common_mp", asset->name, donor_name);
			return;
		}

		ZONETOOL_INFO("weapon \"%s\": s2 values that h1 does not have come from \"%s\"", asset->name, donor_name);

		utils::memory::allocator allocator;
		auto* weapon = allocator.allocate<zonetool::s2::WeaponCompleteDef>();
		*weapon = *donor;
		weapon->szInternalName = asset->szInternalName;
		weapon->szDisplayName = asset->szDisplayName;
		weapon->szAdsrBaseSetting = asset->szAdsrBaseSetting;
		weapon->szUseHintString = asset->szUseHintString;
		weapon->dropHintString = asset->dropHintString;
		weapon->useHintStringIndex = asset->useHintStringIndex;
		weapon->dropHintStringIndex = asset->dropHintStringIndex;
		weapon->szScript = asset->szScript;
		weapon->hideTags = convert_script_strings(asset->hideTags, hide_tag_count, allocator);
		weapon->szXAnimsRightHanded = convert_anims(asset->szXAnimsRightHanded, allocator);
		weapon->szXAnimsLeftHanded = convert_anims(asset->szXAnimsLeftHanded, allocator);
		weapon->szXAnims = convert_anims(asset->szXAnims, allocator);

		convert_type(asset, weapon);
		convert_weapon_models(asset, weapon, allocator);
		convert_attachments(asset, weapon, allocator);
		convert_overrides(asset, weapon, allocator);
		convert_notetrack_maps(asset, weapon, allocator);
		convert_sounds(asset, weapon, allocator);
		convert_state_timers(asset->stateTimers, weapon->stateTimers);
		convert_state_timers(asset->akimboStateTimers, weapon->akimboStateTimers);
		weapon->stateTimers.inspectTime = asset->inspectAnimTime;
		weapon->akimboStateTimers.inspectTime = asset->inspectAnimTime;
		convert_damage(asset, weapon);
		convert_movement(asset, weapon);
		convert_ads(asset, weapon);
		convert_spread(asset, weapon);
		convert_kick(asset, weapon);
		convert_projectile(asset, weapon, allocator);
		convert_ammo(asset, weapon);
		convert_turret(asset, weapon);
		convert_flags(asset, weapon);
		convert_aim(asset, weapon);

		zonetool::s2::weapon_def::dump(weapon);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_WEAPON), asset->name);
	}

	void clear()
	{
		dumped_weapons.clear();
	}
}
