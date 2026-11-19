#include <std_include.hpp>
#include "weapon.hpp"

namespace zonetool::iw8
{
	namespace
	{
		constexpr std::array weapon_def_models
		{
			&WeaponDef::gunXModel, &WeaponDef::gunXModelLeftHand, &WeaponDef::gunXModelRightHand, &WeaponDef::defaultViewModel,
			&WeaponDef::defaultWorldModelLeftHand, &WeaponDef::defaultWorldModelRightHand, &WeaponDef::worldModel,
			&WeaponDef::worldXModelLeftHand, &WeaponDef::worldXModelRightHand, &WeaponDef::defaultWorldModel,
			&WeaponDef::censorshipWorldModel, &WeaponDef::censorshipWorldModelLeftHand, &WeaponDef::censorshipWorldModelRightHand,
			&WeaponDef::playerShadowModel, &WeaponDef::playerShadowModelLeftHand, &WeaponDef::playerShadowModelRightHand,
			&WeaponDef::handXModel,
		};

		constexpr std::array weapon_def_shield_gestures
		{
			&WeaponDef::shieldDeployGesture, &WeaponDef::shieldFireWeapGesture, &WeaponDef::shieldDeployWhileFiring,
			&WeaponDef::shieldRetractWhileFiring, &WeaponDef::shieldBashGesture,
		};

		constexpr std::array weapon_def_gestures
		{
			&WeaponDef::gestureAnimation, &WeaponDef::gesturePullback, &WeaponDef::gesturePullbackAlt, &WeaponDef::gestureThrow,
			&WeaponDef::gestureThrowAlt, &WeaponDef::gestureDetonate, &WeaponDef::gestureDetonateAlt,
		};

		constexpr std::array weapon_def_charge_sounds
		{
			&WeaponDef::chargeUpSound, &WeaponDef::chargeDownSound, &WeaponDef::chargeUpSoundPlayer, &WeaponDef::chargeDownSoundPlayer,
			&WeaponDef::chargeDownToUpSound, &WeaponDef::chargeDownToUpSoundPlayer, &WeaponDef::chargeUpToDownSound,
			&WeaponDef::chargeUpToDownSoundPlayer, &WeaponDef::chargeMaxSound, &WeaponDef::chargeMaxSoundPlayer,
		};

		constexpr std::array vfx_package_effects
		{
			&WeaponVFXPackageDefs::viewFlashEffect, &WeaponVFXPackageDefs::worldFlashEffect, &WeaponVFXPackageDefs::viewFlashADSEffect,
			&WeaponVFXPackageDefs::viewFlashFirstEffect, &WeaponVFXPackageDefs::worldFlashFirstEffect,
			&WeaponVFXPackageDefs::viewFlashFirstADSEffect, &WeaponVFXPackageDefs::viewFlashOverchargeEffect,
			&WeaponVFXPackageDefs::viewFlashOverchargeADSEffect, &WeaponVFXPackageDefs::worldFlashOverchargeEffect,
			&WeaponVFXPackageDefs::viewShellEjectEffect, &WeaponVFXPackageDefs::worldShellEjectEffect,
			&WeaponVFXPackageDefs::viewLastShotEjectEffect, &WeaponVFXPackageDefs::worldLastShotEjectEffect,
			&WeaponVFXPackageDefs::viewMagEjectEffect, &WeaponVFXPackageDefs::viewMuzzleSmokeEffect,
			&WeaponVFXPackageDefs::worldMuzzleSmokeEffect, &WeaponVFXPackageDefs::viewBarrelSmokeEffect,
			&WeaponVFXPackageDefs::worldBarrelSmokeEffect, &WeaponVFXPackageDefs::viewEjectionPortSmokeEffect,
			&WeaponVFXPackageDefs::worldEjectionPortSmokeEffect, &WeaponVFXPackageDefs::viewDlcEffect0,
			&WeaponVFXPackageDefs::worldDlcEffect0, &WeaponVFXPackageDefs::viewDlcEffect1, &WeaponVFXPackageDefs::worldDlcEffect1,
			&WeaponVFXPackageDefs::viewDlcEffect2, &WeaponVFXPackageDefs::worldDlcEffect2, &WeaponVFXPackageDefs::viewDlcEffect3,
			&WeaponVFXPackageDefs::worldDlcEffect3,
		};

		void read_vfx(zone_reader& reader, FxCombinedDef& effect)
		{
			reader.read_asset(ASSET_TYPE_VFX, effect.particleSystemDef);
		}

		void read_sound(zone_reader& reader, SndAliasLookup& sound)
		{
			reader.read_string(sound.name);
		}

		void read_override_bindings(zone_reader& reader, OverrideBinding*& field, const unsigned int count)
		{
			if (const auto bindings = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_array(bindings[i].bindingLists, 3, bindings[i].numBindingLists);
				}
			}
		}

		void read_overlay(zone_reader& reader, ADSOverlay& overlay)
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay.shaderMat);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay.shaderLowResMat);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay.shaderEMPMat);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay.shaderEMPLowResMat);
			reader.read_asset(ASSET_TYPE_IMAGE, overlay.shaderImg);
			reader.read_asset(ASSET_TYPE_IMAGE, overlay.shaderLowResImg);
			reader.read_asset(ASSET_TYPE_IMAGE, overlay.shaderEMPImg);
			reader.read_asset(ASSET_TYPE_IMAGE, overlay.shaderEMPLowResImg);
			reader.read_string(overlay.visionSetName);
		}

		void read_offset_pattern(zone_reader& reader, WeaponOffsetPatternDescription& pattern)
		{
			reader.read_script_string(pattern.patternKey);
		}

		void read_offset_patterns(zone_reader& reader, WeaponDef* def)
		{
			if (const auto patterns = reader.read_array(def->weaponOffsetPatterns, 3, def->numWeaponOffsetPatterns))
			{
				for (auto i = 0; i < def->numWeaponOffsetPatterns; i++)
				{
					read_offset_pattern(reader, patterns[i]);
				}
			}

			const auto count = def->numWeaponOffsetPatternsKickOrSnapDecay;
			if (const auto patterns = reader.read_array(def->weaponOffsetPatternsKickOrSnapDecay, 7, count))
			{
				for (auto i = 0; i < count; i++)
				{
					if (const auto pattern = reader.read_single(patterns[i], 3))
					{
						read_offset_pattern(reader, *pattern);
					}
				}
			}
		}

		void read_pattern_scales(zone_reader& reader, WeaponOffsetPatternScaleInfo& info)
		{
			if (const auto scales = reader.read_array(info.patternScales, 3, info.numPatternScales))
			{
				for (auto i = 0; i < info.numPatternScales; i++)
				{
					reader.read_script_string(scales[i].patternKey);
				}
			}
		}

		void read_ballistic_info(zone_reader& reader, BallisticInfo& info)
		{
			if (const auto calculated = reader.read_single(info.calculated, 7))
			{
				reader.read_array(calculated->distances, 3, calculated->numDistanceEntries);
			}
		}

		template <typename T>
		void read_overrides(zone_reader& reader, T*& field, const unsigned int count, const XAssetType type)
		{
			if (const auto overrides = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					read_override_bindings(reader, overrides[i].bindings, overrides[i].numBindings);
					reader.read_asset(type, overrides[i].overrides);
					reader.read_asset(type, overrides[i].overridesAlt);
				}
			}
		}

		void read_weapon_def(zone_reader& reader, WeaponDef* def, const WeaponCompleteDef* complete)
		{
			reader.read_string(def->szOverlayName);

			for (const auto model : weapon_def_models)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, def->*model);
			}

			reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, def->szXAnims);
			reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, def->szXAnimsRightHanded);
			reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, def->szXAnimsLeftHanded);
			reader.read_asset(ASSET_TYPE_XANIMPARTS, def->turretRaiseAnim);
			reader.read_asset(ASSET_TYPE_XANIMPARTS, def->turretIdleAnim);
			reader.read_asset(ASSET_TYPE_XANIMPARTS, def->turretFireAnim);
			reader.read_string(def->szModeName);
			reader.read_script_string_array(def->notetrackRumbleMapKeys, 16);
			reader.read_script_string_array(def->notetrackRumbleMapValues, 16);
			reader.read_script_string_array(def->notetrackFXMapKeys, 16);

			if (const auto effects = reader.read_array(def->notetrackFXMapValues, 7, 16))
			{
				for (auto i = 0; i < 16; i++)
				{
					read_vfx(reader, effects[i]);
				}
			}

			reader.read_script_string_array(def->notetrackFXMapTagValues, 16);
			reader.read_asset(ASSET_TYPE_VFX_PACKAGE, def->vfxPackage);
			reader.read_asset(ASSET_TYPE_SFX_PACKAGE, def->sfxPackage);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->reticleCenter);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->reticleSide);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->reticleOnePiece);
			reader.read_asset(ASSET_TYPE_XMODEL, def->worldClipModel);
			reader.read_asset(ASSET_TYPE_XMODEL, def->rocketModel);
			reader.read_asset(ASSET_TYPE_IMAGE, def->hudIcon);
			reader.read_asset(ASSET_TYPE_IMAGE, def->pickupIcon);
			reader.read_asset(ASSET_TYPE_IMAGE, def->dangerIconImg);
			reader.read_asset(ASSET_TYPE_IMAGE, def->throwBackIconImg);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->dangerIconMat);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->throwBackIconMat);
			reader.read_string(def->szAmmoName);
			reader.read_string(def->szSharedAmmoCapName);
			read_overlay(reader, def->overlay);
			reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, def->swaySettings.adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, def->swaySettings.adv.gunGoalViewSpeedToOffset_graph);
			reader.read_asset(ASSET_TYPE_XMODEL, def->projectileModel);
			read_vfx(reader, def->projExplosionEffect);
			read_vfx(reader, def->projDudEffect);
			read_vfx(reader, def->projTimeoutEffect);
			reader.read_array(def->parallelBounce, 3, 64);
			reader.read_array(def->perpendicularBounce, 3, 64);
			read_vfx(reader, def->vmProjBodyEffect);
			read_vfx(reader, def->projBodyEffect);
			read_vfx(reader, def->projTrailEffect);
			read_vfx(reader, def->projBeaconEffect);
			read_vfx(reader, def->projIgnitionEffect);
			read_offset_patterns(reader, def);

			for (auto i = 0; i < 2; i++)
			{
				reader.read_string(def->accuracyGraphName[i]);
				reader.read_array(def->originalAccuracyGraphKnots[i], 3, complete->accuracyGraphKnotCount[i]);
			}

			reader.read_string(def->szUseHintString);
			reader.read_string(def->dropHintString);
			reader.read_string(def->szScript);
			reader.read_array(def->locationDamageMultipliers, 3, 22);
			reader.read_array(def->hitLocPriorityMap, 0, 22);
			reader.read_asset(ASSET_TYPE_RUMBLE, def->fireRumble);
			reader.read_asset(ASSET_TYPE_RUMBLE, def->meleeImpactRumble);
			reader.read_asset(ASSET_TYPE_TRACER, def->tracerType);
			reader.read_asset(ASSET_TYPE_TRACER, def->overchargeTracerType);
			reader.read_asset(ASSET_TYPE_LASER, def->laserTypeViewModel);
			reader.read_asset(ASSET_TYPE_LASER, def->laserTypeViewModelAlt);
			reader.read_asset(ASSET_TYPE_LASER, def->laserTypeFriendly);
			reader.read_asset(ASSET_TYPE_LASER, def->laserTypeEnemy);
			read_sound(reader, def->turretOverheatSound);
			read_vfx(reader, def->turretOverheatEffect);
			reader.read_asset(ASSET_TYPE_RUMBLE, def->turretBarrelSpinRumble);
			read_sound(reader, def->turretBarrelSpinMaxSnd);

			for (auto& sound : def->turretBarrelSpinUpSnd)
			{
				read_sound(reader, sound);
			}

			for (auto& sound : def->turretBarrelSpinDownSnd)
			{
				read_sound(reader, sound);
			}

			read_sound(reader, def->missileConeSoundAlias);
			read_sound(reader, def->missileConeSoundAliasAtBase);

			for (const auto gesture : weapon_def_shield_gestures)
			{
				reader.read_asset(ASSET_TYPE_GESTURE, def->*gesture);
			}

			read_vfx(reader, def->shieldMeleeFx);
			reader.read_asset(ASSET_TYPE_RUMBLE, def->shieldHitRumble);
			reader.read_asset(ASSET_TYPE_RUMBLE, def->shieldMissRumble);
			reader.read_asset(ASSET_TYPE_XMODEL, def->detonatorWorldModel);

			for (const auto gesture : weapon_def_gestures)
			{
				reader.read_asset(ASSET_TYPE_GESTURE, def->*gesture);
			}

			read_vfx(reader, def->cameraFireEffect);
			read_vfx(reader, def->chargeMeterEffect);

			for (const auto sound : weapon_def_charge_sounds)
			{
				read_sound(reader, def->*sound);
			}

			reader.read_asset(ASSET_TYPE_RUMBLE, def->chargeRumble);
			reader.read_script_string(def->stowTag);
			reader.read_asset(ASSET_TYPE_XMODEL, def->stowOffsetModel);
			reader.read_script_string(def->stowOffsetAttachTag);
			read_vfx(reader, def->deployEffect);
			read_overrides(reader, def->animOverrides, def->numAnimOverrides, ASSET_TYPE_ANIM_PACKAGE);

			for (auto& carry : def->carryAnimOverrides)
			{
				reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, carry.animPackage);
				reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, carry.animPackageAlt);
			}

			read_overrides(reader, def->sfxOverrides, def->numSfxOverrides, ASSET_TYPE_SFX_PACKAGE);
			read_overrides(reader, def->vfxOverrides, def->numVfxOverrides, ASSET_TYPE_VFX_PACKAGE);
			reader.read_asset(ASSET_TYPE_RUMBLE, def->mountRumble);
			read_pattern_scales(reader, def->mountWeaponOffsetPatternScaleInfo);
			read_pattern_scales(reader, def->crouchedWeaponOffsetPatternScaleInfo);
			read_pattern_scales(reader, def->proneWeaponOffsetPatternScaleInfo);
			read_ballistic_info(reader, def->ballisticInfo);

			for (auto& notify : def->notifyTypes)
			{
				reader.read_single(notify, 3);
			}
		}

		void read_notetrack_entries(zone_reader& reader, WeaponCompleteDef* asset)
		{
			if (const auto entries = reader.read_array(asset->notetrackOverrides, 7, asset->numNotetrackOverrides))
			{
				for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
				{
					reader.read_script_string(entries[i].attachment);
					reader.read_script_string_array(entries[i].notetrackSoundMapKeys, entries[i].numSoundMappings);
					reader.read_script_string_array(entries[i].notetrackSoundMapValues, entries[i].numSoundMappings);
				}
			}

			if (const auto entries = reader.read_array(asset->notetrackSuitEntries, 7, asset->numNotetrackSuitEntries))
			{
				for (auto i = 0u; i < asset->numNotetrackSuitEntries; i++)
				{
					reader.read_script_string_array(entries[i].notetrackSoundMapKeys, entries[i].numSoundMappings);
					reader.read_script_string_array(entries[i].notetrackSoundMapValues, entries[i].numSoundMappings);
				}
			}
		}

		void write_vfx(zone_base* zone, FxCombinedDef& effect)
		{
			write_asset(zone, ASSET_TYPE_VFX, effect.particleSystemDef);
		}

		void write_sound(zone_buffer* buf, SndAliasLookup& sound)
		{
			write_string(buf, sound.name);
		}

		void write_override_bindings(zone_buffer* buf, OverrideBinding*& field, const unsigned int count)
		{
			if (const auto bindings = write_shared_array(buf, field, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					write_shared_array(buf, bindings[i].bindingLists, bindings[i].numBindingLists, 3);
				}
			}
		}

		void write_overlay(zone_base* zone, zone_buffer* buf, ADSOverlay& overlay)
		{
			write_asset(zone, ASSET_TYPE_MATERIAL, overlay.shaderMat);
			write_asset(zone, ASSET_TYPE_MATERIAL, overlay.shaderLowResMat);
			write_asset(zone, ASSET_TYPE_MATERIAL, overlay.shaderEMPMat);
			write_asset(zone, ASSET_TYPE_MATERIAL, overlay.shaderEMPLowResMat);
			write_asset(zone, ASSET_TYPE_IMAGE, overlay.shaderImg);
			write_asset(zone, ASSET_TYPE_IMAGE, overlay.shaderLowResImg);
			write_asset(zone, ASSET_TYPE_IMAGE, overlay.shaderEMPImg);
			write_asset(zone, ASSET_TYPE_IMAGE, overlay.shaderEMPLowResImg);
			write_string(buf, overlay.visionSetName);
		}

		void write_offset_patterns(zone_buffer* buf, WeaponDef* def)
		{
			write_shared_array(buf, def->weaponOffsetPatterns, def->numWeaponOffsetPatterns, 3);

			const auto count = def->numWeaponOffsetPatternsKickOrSnapDecay;
			if (const auto patterns = write_shared_array(buf, def->weaponOffsetPatternsKickOrSnapDecay, count, 7))
			{
				for (auto i = 0; i < count; i++)
				{
					write_shared_array(buf, patterns[i], 1, 3);
				}
			}
		}

		void write_pattern_scales(zone_buffer* buf, WeaponOffsetPatternScaleInfo& info)
		{
			write_shared_array(buf, info.patternScales, info.numPatternScales, 3);
		}

		void write_ballistic_info(zone_buffer* buf, BallisticInfo& info)
		{
			if (const auto calculated = write_shared_array(buf, info.calculated, 1, 7))
			{
				write_shared_array(buf, calculated->distances, calculated->numDistanceEntries, 3);
			}
		}

		template <typename T>
		void write_overrides(zone_base* zone, zone_buffer* buf, T*& field, const unsigned int count, const XAssetType type)
		{
			if (const auto overrides = write_shared_array(buf, field, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					write_override_bindings(buf, overrides[i].bindings, overrides[i].numBindings);
					write_asset(zone, type, overrides[i].overrides);
					write_asset(zone, type, overrides[i].overridesAlt);
				}
			}
		}

		void write_weapon_def(zone_base* zone, zone_buffer* buf, WeaponDef* def, const WeaponCompleteDef* complete)
		{
			write_string(buf, def->szOverlayName);

			for (const auto model : weapon_def_models)
			{
				write_asset(zone, ASSET_TYPE_XMODEL, def->*model);
			}

			write_asset(zone, ASSET_TYPE_ANIM_PACKAGE, def->szXAnims);
			write_asset(zone, ASSET_TYPE_ANIM_PACKAGE, def->szXAnimsRightHanded);
			write_asset(zone, ASSET_TYPE_ANIM_PACKAGE, def->szXAnimsLeftHanded);
			write_asset(zone, ASSET_TYPE_XANIMPARTS, def->turretRaiseAnim);
			write_asset(zone, ASSET_TYPE_XANIMPARTS, def->turretIdleAnim);
			write_asset(zone, ASSET_TYPE_XANIMPARTS, def->turretFireAnim);
			write_string(buf, def->szModeName);
			write_shared_array(buf, def->notetrackRumbleMapKeys, 16, 3);
			write_shared_array(buf, def->notetrackRumbleMapValues, 16, 3);
			write_shared_array(buf, def->notetrackFXMapKeys, 16, 3);

			if (const auto effects = write_shared_array(buf, def->notetrackFXMapValues, 16, 7))
			{
				for (auto i = 0; i < 16; i++)
				{
					write_vfx(zone, effects[i]);
				}
			}

			write_shared_array(buf, def->notetrackFXMapTagValues, 16, 3);
			write_asset(zone, ASSET_TYPE_VFX_PACKAGE, def->vfxPackage);
			write_asset(zone, ASSET_TYPE_SFX_PACKAGE, def->sfxPackage);
			write_asset(zone, ASSET_TYPE_MATERIAL, def->reticleCenter);
			write_asset(zone, ASSET_TYPE_MATERIAL, def->reticleSide);
			write_asset(zone, ASSET_TYPE_MATERIAL, def->reticleOnePiece);
			write_asset(zone, ASSET_TYPE_XMODEL, def->worldClipModel);
			write_asset(zone, ASSET_TYPE_XMODEL, def->rocketModel);
			write_asset(zone, ASSET_TYPE_IMAGE, def->hudIcon);
			write_asset(zone, ASSET_TYPE_IMAGE, def->pickupIcon);
			write_asset(zone, ASSET_TYPE_IMAGE, def->dangerIconImg);
			write_asset(zone, ASSET_TYPE_IMAGE, def->throwBackIconImg);
			write_asset(zone, ASSET_TYPE_MATERIAL, def->dangerIconMat);
			write_asset(zone, ASSET_TYPE_MATERIAL, def->throwBackIconMat);
			write_string(buf, def->szAmmoName);
			write_string(buf, def->szSharedAmmoCapName);
			write_overlay(zone, buf, def->overlay);
			write_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, def->swaySettings.adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			write_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, def->swaySettings.adv.gunGoalViewSpeedToOffset_graph);
			write_asset(zone, ASSET_TYPE_XMODEL, def->projectileModel);
			write_vfx(zone, def->projExplosionEffect);
			write_vfx(zone, def->projDudEffect);
			write_vfx(zone, def->projTimeoutEffect);
			write_shared_array(buf, def->parallelBounce, 64, 3);
			write_shared_array(buf, def->perpendicularBounce, 64, 3);
			write_vfx(zone, def->vmProjBodyEffect);
			write_vfx(zone, def->projBodyEffect);
			write_vfx(zone, def->projTrailEffect);
			write_vfx(zone, def->projBeaconEffect);
			write_vfx(zone, def->projIgnitionEffect);
			write_offset_patterns(buf, def);

			for (auto i = 0; i < 2; i++)
			{
				write_string(buf, def->accuracyGraphName[i]);
				write_shared_array(buf, def->originalAccuracyGraphKnots[i], complete->accuracyGraphKnotCount[i], 3);
			}

			write_string(buf, def->szUseHintString);
			write_string(buf, def->dropHintString);
			write_string(buf, def->szScript);
			write_shared_array(buf, def->locationDamageMultipliers, 22, 3);
			write_shared_array(buf, def->hitLocPriorityMap, 22, 0);
			write_asset(zone, ASSET_TYPE_RUMBLE, def->fireRumble);
			write_asset(zone, ASSET_TYPE_RUMBLE, def->meleeImpactRumble);
			write_asset(zone, ASSET_TYPE_TRACER, def->tracerType);
			write_asset(zone, ASSET_TYPE_TRACER, def->overchargeTracerType);
			write_asset(zone, ASSET_TYPE_LASER, def->laserTypeViewModel);
			write_asset(zone, ASSET_TYPE_LASER, def->laserTypeViewModelAlt);
			write_asset(zone, ASSET_TYPE_LASER, def->laserTypeFriendly);
			write_asset(zone, ASSET_TYPE_LASER, def->laserTypeEnemy);
			write_sound(buf, def->turretOverheatSound);
			write_vfx(zone, def->turretOverheatEffect);
			write_asset(zone, ASSET_TYPE_RUMBLE, def->turretBarrelSpinRumble);
			write_sound(buf, def->turretBarrelSpinMaxSnd);

			for (auto& sound : def->turretBarrelSpinUpSnd)
			{
				write_sound(buf, sound);
			}

			for (auto& sound : def->turretBarrelSpinDownSnd)
			{
				write_sound(buf, sound);
			}

			write_sound(buf, def->missileConeSoundAlias);
			write_sound(buf, def->missileConeSoundAliasAtBase);

			for (const auto gesture : weapon_def_shield_gestures)
			{
				write_asset(zone, ASSET_TYPE_GESTURE, def->*gesture);
			}

			write_vfx(zone, def->shieldMeleeFx);
			write_asset(zone, ASSET_TYPE_RUMBLE, def->shieldHitRumble);
			write_asset(zone, ASSET_TYPE_RUMBLE, def->shieldMissRumble);
			write_asset(zone, ASSET_TYPE_XMODEL, def->detonatorWorldModel);

			for (const auto gesture : weapon_def_gestures)
			{
				write_asset(zone, ASSET_TYPE_GESTURE, def->*gesture);
			}

			write_vfx(zone, def->cameraFireEffect);
			write_vfx(zone, def->chargeMeterEffect);

			for (const auto sound : weapon_def_charge_sounds)
			{
				write_sound(buf, def->*sound);
			}

			write_asset(zone, ASSET_TYPE_RUMBLE, def->chargeRumble);
			write_asset(zone, ASSET_TYPE_XMODEL, def->stowOffsetModel);
			write_vfx(zone, def->deployEffect);
			write_overrides(zone, buf, def->animOverrides, def->numAnimOverrides, ASSET_TYPE_ANIM_PACKAGE);

			for (auto& carry : def->carryAnimOverrides)
			{
				write_asset(zone, ASSET_TYPE_ANIM_PACKAGE, carry.animPackage);
				write_asset(zone, ASSET_TYPE_ANIM_PACKAGE, carry.animPackageAlt);
			}

			write_overrides(zone, buf, def->sfxOverrides, def->numSfxOverrides, ASSET_TYPE_SFX_PACKAGE);
			write_overrides(zone, buf, def->vfxOverrides, def->numVfxOverrides, ASSET_TYPE_VFX_PACKAGE);
			write_asset(zone, ASSET_TYPE_RUMBLE, def->mountRumble);
			write_pattern_scales(buf, def->mountWeaponOffsetPatternScaleInfo);
			write_pattern_scales(buf, def->crouchedWeaponOffsetPatternScaleInfo);
			write_pattern_scales(buf, def->proneWeaponOffsetPatternScaleInfo);
			write_ballistic_info(buf, def->ballisticInfo);

			for (auto& notify : def->notifyTypes)
			{
				write_shared_array(buf, notify, 1, 3);
			}
		}

		void write_notetrack_entries(zone_buffer* buf, WeaponCompleteDef* asset)
		{
			if (const auto entries = write_shared_array(buf, asset->notetrackOverrides, asset->numNotetrackOverrides, 7))
			{
				for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
				{
					write_shared_array(buf, entries[i].notetrackSoundMapKeys, entries[i].numSoundMappings, 3);
					write_shared_array(buf, entries[i].notetrackSoundMapValues, entries[i].numSoundMappings, 3);
				}
			}

			if (const auto entries = write_shared_array(buf, asset->notetrackSuitEntries, asset->numNotetrackSuitEntries, 7))
			{
				for (auto i = 0u; i < asset->numNotetrackSuitEntries; i++)
				{
					write_shared_array(buf, entries[i].notetrackSoundMapKeys, entries[i].numSoundMappings, 3);
					write_shared_array(buf, entries[i].notetrackSoundMapValues, entries[i].numSoundMappings, 3);
				}
			}
		}

		void dump_override_bindings(assetmanager::dumper& dump, OverrideBinding* bindings, const unsigned int count)
		{
			dump.dump_array(bindings, count);
			if (bindings)
			{
				for (auto i = 0u; i < count; i++)
				{
					dump.dump_array(bindings[i].bindingLists, bindings[i].numBindingLists);
				}
			}
		}

		void dump_overlay(assetmanager::dumper& dump, const ADSOverlay& overlay)
		{
			dump.dump_asset(overlay.shaderMat);
			dump.dump_asset(overlay.shaderLowResMat);
			dump.dump_asset(overlay.shaderEMPMat);
			dump.dump_asset(overlay.shaderEMPLowResMat);
			dump.dump_asset(overlay.shaderImg);
			dump.dump_asset(overlay.shaderLowResImg);
			dump.dump_asset(overlay.shaderEMPImg);
			dump.dump_asset(overlay.shaderEMPLowResImg);
			dump.dump_string(overlay.visionSetName);
		}

		void dump_offset_patterns(assetmanager::dumper& dump, const WeaponDef* def)
		{
			dump.dump_array(def->weaponOffsetPatterns, def->numWeaponOffsetPatterns);
			if (def->weaponOffsetPatterns)
			{
				for (auto i = 0; i < def->numWeaponOffsetPatterns; i++)
				{
					dump_script_string(dump, def->weaponOffsetPatterns[i].patternKey);
				}
			}

			dump.dump_array(def->weaponOffsetPatternsKickOrSnapDecay, def->numWeaponOffsetPatternsKickOrSnapDecay);
			if (def->weaponOffsetPatternsKickOrSnapDecay)
			{
				for (auto i = 0; i < def->numWeaponOffsetPatternsKickOrSnapDecay; i++)
				{
					const auto pattern = def->weaponOffsetPatternsKickOrSnapDecay[i];
					dump.dump_single(pattern);
					if (pattern)
					{
						dump_script_string(dump, pattern->patternKey);
					}
				}
			}
		}

		void dump_pattern_scales(assetmanager::dumper& dump, const WeaponOffsetPatternScaleInfo& info)
		{
			dump.dump_array(info.patternScales, info.numPatternScales);
			if (info.patternScales)
			{
				for (auto i = 0; i < info.numPatternScales; i++)
				{
					dump_script_string(dump, info.patternScales[i].patternKey);
				}
			}
		}

		void dump_ballistic_info(assetmanager::dumper& dump, const BallisticInfo& info)
		{
			dump.dump_single(info.calculated);
			if (info.calculated)
			{
				dump.dump_array(info.calculated->distances, info.calculated->numDistanceEntries);
			}
		}

		template <typename T>
		void dump_overrides(assetmanager::dumper& dump, T* overrides, const unsigned int count)
		{
			dump.dump_array(overrides, count);
			if (overrides)
			{
				for (auto i = 0u; i < count; i++)
				{
					dump_override_bindings(dump, overrides[i].bindings, overrides[i].numBindings);
					dump.dump_asset(overrides[i].overrides);
					dump.dump_asset(overrides[i].overridesAlt);
				}
			}
		}

		void dump_weapon_def(assetmanager::dumper& dump, WeaponDef* def, const WeaponCompleteDef* complete)
		{
			dump.dump_string(def->szOverlayName);

			for (const auto model : weapon_def_models)
			{
				dump.dump_asset(def->*model);
			}

			dump.dump_asset(def->szXAnims);
			dump.dump_asset(def->szXAnimsRightHanded);
			dump.dump_asset(def->szXAnimsLeftHanded);
			dump.dump_asset(def->turretRaiseAnim);
			dump.dump_asset(def->turretIdleAnim);
			dump.dump_asset(def->turretFireAnim);
			dump.dump_string(def->szModeName);
			dump_script_strings(dump, def->notetrackRumbleMapKeys, 16);
			dump_script_strings(dump, def->notetrackRumbleMapValues, 16);
			dump_script_strings(dump, def->notetrackFXMapKeys, 16);

			dump.dump_array(def->notetrackFXMapValues, 16);
			if (def->notetrackFXMapValues)
			{
				for (auto i = 0; i < 16; i++)
				{
					dump.dump_asset(def->notetrackFXMapValues[i].particleSystemDef);
				}
			}

			dump_script_strings(dump, def->notetrackFXMapTagValues, 16);
			dump.dump_asset(def->vfxPackage);
			dump.dump_asset(def->sfxPackage);
			dump.dump_asset(def->reticleCenter);
			dump.dump_asset(def->reticleSide);
			dump.dump_asset(def->reticleOnePiece);
			dump.dump_asset(def->worldClipModel);
			dump.dump_asset(def->rocketModel);
			dump.dump_asset(def->hudIcon);
			dump.dump_asset(def->pickupIcon);
			dump.dump_asset(def->dangerIconImg);
			dump.dump_asset(def->throwBackIconImg);
			dump.dump_asset(def->dangerIconMat);
			dump.dump_asset(def->throwBackIconMat);
			dump.dump_string(def->szAmmoName);
			dump.dump_string(def->szSharedAmmoCapName);
			dump_overlay(dump, def->overlay);
			dump.dump_asset(def->swaySettings.adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			dump.dump_asset(def->swaySettings.adv.gunGoalViewSpeedToOffset_graph);
			dump.dump_asset(def->projectileModel);
			dump.dump_asset(def->projExplosionEffect.particleSystemDef);
			dump.dump_asset(def->projDudEffect.particleSystemDef);
			dump.dump_asset(def->projTimeoutEffect.particleSystemDef);
			dump.dump_array(def->parallelBounce, 64);
			dump.dump_array(def->perpendicularBounce, 64);
			dump.dump_asset(def->vmProjBodyEffect.particleSystemDef);
			dump.dump_asset(def->projBodyEffect.particleSystemDef);
			dump.dump_asset(def->projTrailEffect.particleSystemDef);
			dump.dump_asset(def->projBeaconEffect.particleSystemDef);
			dump.dump_asset(def->projIgnitionEffect.particleSystemDef);
			dump_offset_patterns(dump, def);

			for (auto i = 0; i < 2; i++)
			{
				dump.dump_string(def->accuracyGraphName[i]);
				dump.dump_array(def->originalAccuracyGraphKnots[i], complete->accuracyGraphKnotCount[i]);
			}

			dump.dump_string(def->szUseHintString);
			dump.dump_string(def->dropHintString);
			dump.dump_string(def->szScript);
			dump.dump_array(def->locationDamageMultipliers, 22);
			dump.dump_array(def->hitLocPriorityMap, 22);
			dump.dump_asset(def->fireRumble);
			dump.dump_asset(def->meleeImpactRumble);
			dump.dump_asset(def->tracerType);
			dump.dump_asset(def->overchargeTracerType);
			dump.dump_asset(def->laserTypeViewModel);
			dump.dump_asset(def->laserTypeViewModelAlt);
			dump.dump_asset(def->laserTypeFriendly);
			dump.dump_asset(def->laserTypeEnemy);
			dump.dump_string(def->turretOverheatSound.name);
			dump.dump_asset(def->turretOverheatEffect.particleSystemDef);
			dump.dump_asset(def->turretBarrelSpinRumble);
			dump.dump_string(def->turretBarrelSpinMaxSnd.name);

			for (const auto& sound : def->turretBarrelSpinUpSnd)
			{
				dump.dump_string(sound.name);
			}

			for (const auto& sound : def->turretBarrelSpinDownSnd)
			{
				dump.dump_string(sound.name);
			}

			dump.dump_string(def->missileConeSoundAlias.name);
			dump.dump_string(def->missileConeSoundAliasAtBase.name);

			for (const auto gesture : weapon_def_shield_gestures)
			{
				dump.dump_asset(def->*gesture);
			}

			dump.dump_asset(def->shieldMeleeFx.particleSystemDef);
			dump.dump_asset(def->shieldHitRumble);
			dump.dump_asset(def->shieldMissRumble);
			dump.dump_asset(def->detonatorWorldModel);

			for (const auto gesture : weapon_def_gestures)
			{
				dump.dump_asset(def->*gesture);
			}

			dump.dump_asset(def->cameraFireEffect.particleSystemDef);
			dump.dump_asset(def->chargeMeterEffect.particleSystemDef);

			for (const auto sound : weapon_def_charge_sounds)
			{
				dump.dump_string((def->*sound).name);
			}

			dump.dump_asset(def->chargeRumble);
			dump_script_string(dump, def->stowTag);
			dump.dump_asset(def->stowOffsetModel);
			dump_script_string(dump, def->stowOffsetAttachTag);
			dump.dump_asset(def->deployEffect.particleSystemDef);
			dump_overrides(dump, def->animOverrides, def->numAnimOverrides);

			for (const auto& carry : def->carryAnimOverrides)
			{
				dump.dump_asset(carry.animPackage);
				dump.dump_asset(carry.animPackageAlt);
			}

			dump_overrides(dump, def->sfxOverrides, def->numSfxOverrides);
			dump_overrides(dump, def->vfxOverrides, def->numVfxOverrides);
			dump.dump_asset(def->mountRumble);
			dump_pattern_scales(dump, def->mountWeaponOffsetPatternScaleInfo);
			dump_pattern_scales(dump, def->crouchedWeaponOffsetPatternScaleInfo);
			dump_pattern_scales(dump, def->proneWeaponOffsetPatternScaleInfo);
			dump_ballistic_info(dump, def->ballisticInfo);

			for (const auto notify : def->notifyTypes)
			{
				dump.dump_single(notify);
			}
		}

		WeaponAttachment* parse_attachment(assetmanager::reader& read, zone_memory* mem)
		{
			const auto name = read.read_string();
			if (!name)
			{
				return nullptr;
			}

			auto* attachment = mem->allocate<WeaponAttachment>();
			attachment->szInternalName = name;
			return attachment;
		}

		void parse_override_bindings(assetmanager::reader& read, OverrideBinding*& bindings, const unsigned int count)
		{
			if ((bindings = read.read_array<OverrideBinding>()))
			{
				for (auto i = 0u; i < count; i++)
				{
					bindings[i].bindingLists = read.read_array<OverrideBindingList>();
				}
			}
		}

		void parse_overlay(assetmanager::reader& read, ADSOverlay& overlay)
		{
			overlay.shaderMat = read.read_asset<Material>();
			overlay.shaderLowResMat = read.read_asset<Material>();
			overlay.shaderEMPMat = read.read_asset<Material>();
			overlay.shaderEMPLowResMat = read.read_asset<Material>();
			overlay.shaderImg = read.read_asset<GfxImage>();
			overlay.shaderLowResImg = read.read_asset<GfxImage>();
			overlay.shaderEMPImg = read.read_asset<GfxImage>();
			overlay.shaderEMPLowResImg = read.read_asset<GfxImage>();
			overlay.visionSetName = read.read_string();
		}

		void parse_vfx(assetmanager::reader& read, FxCombinedDef& effect)
		{
			effect.particleSystemDef = read.read_asset<ParticleSystemDef>();
		}

		void parse_sound(assetmanager::reader& read, SndAliasLookup& sound)
		{
			sound.name = read.read_string();
		}

		void parse_ballistic_info(assetmanager::reader& read, BallisticInfo& info)
		{
			if ((info.calculated = read.read_single<BallisticInfoCalculated>()))
			{
				info.calculated->distances = read.read_array<float>();
			}
		}

		template <typename T, typename Package>
		void parse_overrides(assetmanager::reader& read, T*& overrides, const unsigned int count)
		{
			if ((overrides = read.read_array<T>()))
			{
				for (auto i = 0u; i < count; i++)
				{
					parse_override_bindings(read, overrides[i].bindings, overrides[i].numBindings);
					overrides[i].overrides = read.read_asset<Package>();
					overrides[i].overridesAlt = read.read_asset<Package>();
				}
			}
		}

		template <typename T>
		void add_overrides(zone_base* zone, const T* overrides, const unsigned int count, const XAssetType type)
		{
			if (overrides)
			{
				for (auto i = 0u; i < count; i++)
				{
					add_asset(zone, type, overrides[i].overrides);
					add_asset(zone, type, overrides[i].overridesAlt);
				}
			}
		}

		void add_overlay(zone_base* zone, const ADSOverlay& overlay)
		{
			add_asset(zone, ASSET_TYPE_MATERIAL, overlay.shaderMat);
			add_asset(zone, ASSET_TYPE_MATERIAL, overlay.shaderLowResMat);
			add_asset(zone, ASSET_TYPE_MATERIAL, overlay.shaderEMPMat);
			add_asset(zone, ASSET_TYPE_MATERIAL, overlay.shaderEMPLowResMat);
			add_asset(zone, ASSET_TYPE_IMAGE, overlay.shaderImg);
			add_asset(zone, ASSET_TYPE_IMAGE, overlay.shaderLowResImg);
			add_asset(zone, ASSET_TYPE_IMAGE, overlay.shaderEMPImg);
			add_asset(zone, ASSET_TYPE_IMAGE, overlay.shaderEMPLowResImg);
		}

		void add_weapon_def(zone_base* zone, const WeaponDef* def)
		{
			for (const auto model : weapon_def_models)
			{
				add_asset(zone, ASSET_TYPE_XMODEL, def->*model);
			}

			add_asset(zone, ASSET_TYPE_ANIM_PACKAGE, def->szXAnims);
			add_asset(zone, ASSET_TYPE_ANIM_PACKAGE, def->szXAnimsRightHanded);
			add_asset(zone, ASSET_TYPE_ANIM_PACKAGE, def->szXAnimsLeftHanded);
			add_asset(zone, ASSET_TYPE_XANIMPARTS, def->turretRaiseAnim);
			add_asset(zone, ASSET_TYPE_XANIMPARTS, def->turretIdleAnim);
			add_asset(zone, ASSET_TYPE_XANIMPARTS, def->turretFireAnim);

			if (def->notetrackFXMapValues)
			{
				for (auto i = 0; i < 16; i++)
				{
					add_asset(zone, ASSET_TYPE_VFX, def->notetrackFXMapValues[i].particleSystemDef);
				}
			}

			add_asset(zone, ASSET_TYPE_VFX_PACKAGE, def->vfxPackage);
			add_asset(zone, ASSET_TYPE_SFX_PACKAGE, def->sfxPackage);
			add_asset(zone, ASSET_TYPE_MATERIAL, def->reticleCenter);
			add_asset(zone, ASSET_TYPE_MATERIAL, def->reticleSide);
			add_asset(zone, ASSET_TYPE_MATERIAL, def->reticleOnePiece);
			add_asset(zone, ASSET_TYPE_XMODEL, def->worldClipModel);
			add_asset(zone, ASSET_TYPE_XMODEL, def->rocketModel);
			add_asset(zone, ASSET_TYPE_IMAGE, def->hudIcon);
			add_asset(zone, ASSET_TYPE_IMAGE, def->pickupIcon);
			add_asset(zone, ASSET_TYPE_IMAGE, def->dangerIconImg);
			add_asset(zone, ASSET_TYPE_IMAGE, def->throwBackIconImg);
			add_asset(zone, ASSET_TYPE_MATERIAL, def->dangerIconMat);
			add_asset(zone, ASSET_TYPE_MATERIAL, def->throwBackIconMat);
			add_overlay(zone, def->overlay);
			add_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, def->swaySettings.adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			add_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, def->swaySettings.adv.gunGoalViewSpeedToOffset_graph);
			add_asset(zone, ASSET_TYPE_XMODEL, def->projectileModel);

			for (const auto effect : {&def->projExplosionEffect, &def->projDudEffect, &def->projTimeoutEffect, &def->vmProjBodyEffect,
				&def->projBodyEffect, &def->projTrailEffect, &def->projBeaconEffect, &def->projIgnitionEffect, &def->turretOverheatEffect,
				&def->shieldMeleeFx, &def->cameraFireEffect, &def->chargeMeterEffect, &def->deployEffect})
			{
				add_asset(zone, ASSET_TYPE_VFX, effect->particleSystemDef);
			}

			add_asset(zone, ASSET_TYPE_RUMBLE, def->fireRumble);
			add_asset(zone, ASSET_TYPE_RUMBLE, def->meleeImpactRumble);
			add_asset(zone, ASSET_TYPE_TRACER, def->tracerType);
			add_asset(zone, ASSET_TYPE_TRACER, def->overchargeTracerType);
			add_asset(zone, ASSET_TYPE_LASER, def->laserTypeViewModel);
			add_asset(zone, ASSET_TYPE_LASER, def->laserTypeViewModelAlt);
			add_asset(zone, ASSET_TYPE_LASER, def->laserTypeFriendly);
			add_asset(zone, ASSET_TYPE_LASER, def->laserTypeEnemy);
			add_asset(zone, ASSET_TYPE_RUMBLE, def->turretBarrelSpinRumble);

			for (const auto gesture : weapon_def_shield_gestures)
			{
				add_asset(zone, ASSET_TYPE_GESTURE, def->*gesture);
			}

			add_asset(zone, ASSET_TYPE_RUMBLE, def->shieldHitRumble);
			add_asset(zone, ASSET_TYPE_RUMBLE, def->shieldMissRumble);
			add_asset(zone, ASSET_TYPE_XMODEL, def->detonatorWorldModel);

			for (const auto gesture : weapon_def_gestures)
			{
				add_asset(zone, ASSET_TYPE_GESTURE, def->*gesture);
			}

			add_asset(zone, ASSET_TYPE_RUMBLE, def->chargeRumble);
			add_asset(zone, ASSET_TYPE_XMODEL, def->stowOffsetModel);
			add_overrides(zone, def->animOverrides, def->numAnimOverrides, ASSET_TYPE_ANIM_PACKAGE);

			for (const auto& carry : def->carryAnimOverrides)
			{
				add_asset(zone, ASSET_TYPE_ANIM_PACKAGE, carry.animPackage);
				add_asset(zone, ASSET_TYPE_ANIM_PACKAGE, carry.animPackageAlt);
			}

			add_overrides(zone, def->sfxOverrides, def->numSfxOverrides, ASSET_TYPE_SFX_PACKAGE);
			add_overrides(zone, def->vfxOverrides, def->numVfxOverrides, ASSET_TYPE_VFX_PACKAGE);
			add_asset(zone, ASSET_TYPE_RUMBLE, def->mountRumble);
		}

		void parse_pattern_scales(assetmanager::reader& read, script_string_map& strings, WeaponOffsetPatternScaleInfo& info)
		{
			if ((info.patternScales = read.read_array<WeaponOffsetPatternScale>()))
			{
				for (auto i = 0; i < info.numPatternScales; i++)
				{
					strings.parse(read, info.patternScales[i].patternKey);
				}
			}
		}

		void prepare_pattern_scales(zone_buffer* buf, script_string_map& strings, WeaponOffsetPatternScaleInfo& info)
		{
			if (info.patternScales)
			{
				for (auto i = 0; i < info.numPatternScales; i++)
				{
					strings.prepare(buf, info.patternScales[i].patternKey);
				}
			}
		}

		void parse_weapon_def(assetmanager::reader& read, WeaponDef* def, script_string_map& strings)
		{
			def->szOverlayName = read.read_string();

			for (const auto model : weapon_def_models)
			{
				def->*model = read.read_asset<XModel>();
			}

			def->szXAnims = read.read_asset<WeaponAnimPackage>();
			def->szXAnimsRightHanded = read.read_asset<WeaponAnimPackage>();
			def->szXAnimsLeftHanded = read.read_asset<WeaponAnimPackage>();
			def->turretRaiseAnim = read.read_asset<XAnimParts>();
			def->turretIdleAnim = read.read_asset<XAnimParts>();
			def->turretFireAnim = read.read_asset<XAnimParts>();
			def->szModeName = read.read_string();
			strings.parse(read, def->notetrackRumbleMapKeys, 16);
			strings.parse(read, def->notetrackRumbleMapValues, 16);
			strings.parse(read, def->notetrackFXMapKeys, 16);

			if ((def->notetrackFXMapValues = read.read_array<FxCombinedDef>()))
			{
				for (auto i = 0; i < 16; i++)
				{
					parse_vfx(read, def->notetrackFXMapValues[i]);
				}
			}

			strings.parse(read, def->notetrackFXMapTagValues, 16);
			def->vfxPackage = read.read_asset<WeaponVFXPackage>();
			def->sfxPackage = read.read_asset<WeaponSFXPackage>();
			def->reticleCenter = read.read_asset<Material>();
			def->reticleSide = read.read_asset<Material>();
			def->reticleOnePiece = read.read_asset<Material>();
			def->worldClipModel = read.read_asset<XModel>();
			def->rocketModel = read.read_asset<XModel>();
			def->hudIcon = read.read_asset<GfxImage>();
			def->pickupIcon = read.read_asset<GfxImage>();
			def->dangerIconImg = read.read_asset<GfxImage>();
			def->throwBackIconImg = read.read_asset<GfxImage>();
			def->dangerIconMat = read.read_asset<Material>();
			def->throwBackIconMat = read.read_asset<Material>();
			def->szAmmoName = read.read_string();
			def->szSharedAmmoCapName = read.read_string();
			parse_overlay(read, def->overlay);
			def->swaySettings.adv.torsoGoalViewSpeedToMaxDeadzone_graph = read.read_asset<RumbleGraph>();
			def->swaySettings.adv.gunGoalViewSpeedToOffset_graph = read.read_asset<RumbleGraph>();
			def->projectileModel = read.read_asset<XModel>();
			parse_vfx(read, def->projExplosionEffect);
			parse_vfx(read, def->projDudEffect);
			parse_vfx(read, def->projTimeoutEffect);
			def->parallelBounce = read.read_array<float>();
			def->perpendicularBounce = read.read_array<float>();
			parse_vfx(read, def->vmProjBodyEffect);
			parse_vfx(read, def->projBodyEffect);
			parse_vfx(read, def->projTrailEffect);
			parse_vfx(read, def->projBeaconEffect);
			parse_vfx(read, def->projIgnitionEffect);

			if ((def->weaponOffsetPatterns = read.read_array<WeaponOffsetPatternDescription>()))
			{
				for (auto i = 0; i < def->numWeaponOffsetPatterns; i++)
				{
					strings.parse(read, def->weaponOffsetPatterns[i].patternKey);
				}
			}

			if ((def->weaponOffsetPatternsKickOrSnapDecay = read.read_array<WeaponOffsetPatternDescription*>()))
			{
				for (auto i = 0; i < def->numWeaponOffsetPatternsKickOrSnapDecay; i++)
				{
					auto& pattern = def->weaponOffsetPatternsKickOrSnapDecay[i];
					if ((pattern = read.read_single<WeaponOffsetPatternDescription>()))
					{
						strings.parse(read, pattern->patternKey);
					}
				}
			}

			for (auto i = 0; i < 2; i++)
			{
				def->accuracyGraphName[i] = read.read_string();
				def->originalAccuracyGraphKnots[i] = read.read_array<vec2_t>();
			}

			def->szUseHintString = read.read_string();
			def->dropHintString = read.read_string();
			def->szScript = read.read_string();
			def->locationDamageMultipliers = read.read_array<float>();
			def->hitLocPriorityMap = read.read_array<char>();
			def->fireRumble = read.read_asset<RumbleInfo>();
			def->meleeImpactRumble = read.read_asset<RumbleInfo>();
			def->tracerType = read.read_asset<TracerDef>();
			def->overchargeTracerType = read.read_asset<TracerDef>();
			def->laserTypeViewModel = read.read_asset<LaserDef>();
			def->laserTypeViewModelAlt = read.read_asset<LaserDef>();
			def->laserTypeFriendly = read.read_asset<LaserDef>();
			def->laserTypeEnemy = read.read_asset<LaserDef>();
			parse_sound(read, def->turretOverheatSound);
			parse_vfx(read, def->turretOverheatEffect);
			def->turretBarrelSpinRumble = read.read_asset<RumbleInfo>();
			parse_sound(read, def->turretBarrelSpinMaxSnd);

			for (auto& sound : def->turretBarrelSpinUpSnd)
			{
				parse_sound(read, sound);
			}

			for (auto& sound : def->turretBarrelSpinDownSnd)
			{
				parse_sound(read, sound);
			}

			parse_sound(read, def->missileConeSoundAlias);
			parse_sound(read, def->missileConeSoundAliasAtBase);

			for (const auto gesture : weapon_def_shield_gestures)
			{
				def->*gesture = read.read_asset<Gesture>();
			}

			parse_vfx(read, def->shieldMeleeFx);
			def->shieldHitRumble = read.read_asset<RumbleInfo>();
			def->shieldMissRumble = read.read_asset<RumbleInfo>();
			def->detonatorWorldModel = read.read_asset<XModel>();

			for (const auto gesture : weapon_def_gestures)
			{
				def->*gesture = read.read_asset<Gesture>();
			}

			parse_vfx(read, def->cameraFireEffect);
			parse_vfx(read, def->chargeMeterEffect);

			for (const auto sound : weapon_def_charge_sounds)
			{
				parse_sound(read, def->*sound);
			}

			def->chargeRumble = read.read_asset<RumbleInfo>();
			strings.parse(read, def->stowTag);
			def->stowOffsetModel = read.read_asset<XModel>();
			strings.parse(read, def->stowOffsetAttachTag);
			parse_vfx(read, def->deployEffect);
			parse_overrides<AnimOverride, WeaponAnimPackage>(read, def->animOverrides, def->numAnimOverrides);

			for (auto& carry : def->carryAnimOverrides)
			{
				carry.animPackage = read.read_asset<WeaponAnimPackage>();
				carry.animPackageAlt = read.read_asset<WeaponAnimPackage>();
			}

			parse_overrides<SFXOverride, WeaponSFXPackage>(read, def->sfxOverrides, def->numSfxOverrides);
			parse_overrides<VFXOverride, WeaponVFXPackage>(read, def->vfxOverrides, def->numVfxOverrides);
			def->mountRumble = read.read_asset<RumbleInfo>();
			parse_pattern_scales(read, strings, def->mountWeaponOffsetPatternScaleInfo);
			parse_pattern_scales(read, strings, def->crouchedWeaponOffsetPatternScaleInfo);
			parse_pattern_scales(read, strings, def->proneWeaponOffsetPatternScaleInfo);
			parse_ballistic_info(read, def->ballisticInfo);

			for (auto& notify : def->notifyTypes)
			{
				notify = read.read_single<WeaponEntityNotify>();
			}
		}
	}

	void weapon::read(zone_reader& reader, WeaponCompleteDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);

		if (const auto def = reader.read_single(asset->weapDef, 7))
		{
			read_weapon_def(reader, def, asset);
		}

		reader.read_string(asset->szDisplayName);
		reader.read_string(asset->szLootTable);
		reader.read_script_string_array(asset->hideTags, 32);

		for (auto& list : asset->attachments)
		{
			reader.read_asset_array(ASSET_TYPE_ATTACHMENT, list.attachments, list.attachmentCount);
		}

		reader.read_script_string_array(asset->notetrackSoundMapKeys, asset->numNotetrackSoundMappings);
		reader.read_script_string_array(asset->notetrackSoundMapValues, asset->numNotetrackSoundMappings);
		read_notetrack_entries(reader, asset);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->killIconMat);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->dpadIconMat);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->dpadIconInactiveMat);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->killIconImg);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->dpadIconImg);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->dpadIconInactiveImg);

		for (auto i = 0; i < 2; i++)
		{
			reader.read_array(asset->accuracyGraphKnots[i], 3, asset->accuracyGraphKnotCount[i]);
		}

		reader.read_string(asset->szAltModeInactiveName);
		reader.read_string(asset->dynamicIconAnimationName);
		reader.read_string(asset->szLUIWeaponInfoWidgetName);
		reader.read_string(asset->szLUIWeaponInfoWidgetTag);
		reader.read_string(asset->szLUICrosshairWidget);
		reader.pop_stream();
	}

	void weapon::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->szInternalName = buf->write_str(this->name());

		if (const auto def = write_shared_array(buf, dest->weapDef, 1, 7))
		{
			write_weapon_def(zone, buf, def, data);
		}

		write_string(buf, dest->szDisplayName);
		write_string(buf, dest->szLootTable);
		write_shared_array(buf, dest->hideTags, 32, 3);

		for (auto& list : dest->attachments)
		{
			write_asset_array(zone, buf, ASSET_TYPE_ATTACHMENT, list.attachments, list.attachmentCount);
		}

		write_shared_array(buf, dest->notetrackSoundMapKeys, data->numNotetrackSoundMappings, 3);
		write_shared_array(buf, dest->notetrackSoundMapValues, data->numNotetrackSoundMappings, 3);
		write_notetrack_entries(buf, dest);
		write_asset(zone, ASSET_TYPE_MATERIAL, dest->killIconMat);
		write_asset(zone, ASSET_TYPE_MATERIAL, dest->dpadIconMat);
		write_asset(zone, ASSET_TYPE_MATERIAL, dest->dpadIconInactiveMat);
		write_asset(zone, ASSET_TYPE_IMAGE, dest->killIconImg);
		write_asset(zone, ASSET_TYPE_IMAGE, dest->dpadIconImg);
		write_asset(zone, ASSET_TYPE_IMAGE, dest->dpadIconInactiveImg);

		for (auto i = 0; i < 2; i++)
		{
			write_shared_array(buf, dest->accuracyGraphKnots[i], data->accuracyGraphKnotCount[i], 3);
		}

		write_string(buf, dest->szAltModeInactiveName);
		write_string(buf, dest->dynamicIconAnimationName);
		write_string(buf, dest->szLUIWeaponInfoWidgetName);
		write_string(buf, dest->szLUIWeaponInfoWidgetTag);
		write_string(buf, dest->szLUICrosshairWidget);
		buf->pop_stream();
	}

	void weapon::dump(WeaponCompleteDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("weapons\\"s + asset->szInternalName + ".weapb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->szInternalName);
		dump.dump_single(asset->weapDef);
		if (asset->weapDef)
		{
			dump_weapon_def(dump, asset->weapDef, asset);
		}

		dump.dump_string(asset->szDisplayName);
		dump.dump_string(asset->szLootTable);
		dump_script_strings(dump, asset->hideTags, 32);

		for (const auto& list : asset->attachments)
		{
			dump.dump_array(list.attachments, list.attachmentCount);
			if (list.attachments)
			{
				for (auto i = 0u; i < list.attachmentCount; i++)
				{
					dump.dump_string(list.attachments[i] ? list.attachments[i]->szInternalName : nullptr);
				}
			}
		}

		dump_script_strings(dump, asset->notetrackSoundMapKeys, asset->numNotetrackSoundMappings);
		dump_script_strings(dump, asset->notetrackSoundMapValues, asset->numNotetrackSoundMappings);

		dump.dump_array(asset->notetrackOverrides, asset->numNotetrackOverrides);
		if (asset->notetrackOverrides)
		{
			for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
			{
				const auto& entry = asset->notetrackOverrides[i];
				dump_script_string(dump, entry.attachment);
				dump_script_strings(dump, entry.notetrackSoundMapKeys, entry.numSoundMappings);
				dump_script_strings(dump, entry.notetrackSoundMapValues, entry.numSoundMappings);
			}
		}

		dump.dump_array(asset->notetrackSuitEntries, asset->numNotetrackSuitEntries);
		if (asset->notetrackSuitEntries)
		{
			for (auto i = 0u; i < asset->numNotetrackSuitEntries; i++)
			{
				const auto& entry = asset->notetrackSuitEntries[i];
				dump_script_strings(dump, entry.notetrackSoundMapKeys, entry.numSoundMappings);
				dump_script_strings(dump, entry.notetrackSoundMapValues, entry.numSoundMappings);
			}
		}

		dump.dump_asset(asset->killIconMat);
		dump.dump_asset(asset->dpadIconMat);
		dump.dump_asset(asset->dpadIconInactiveMat);
		dump.dump_asset(asset->killIconImg);
		dump.dump_asset(asset->dpadIconImg);
		dump.dump_asset(asset->dpadIconInactiveImg);

		for (auto i = 0; i < 2; i++)
		{
			dump.dump_array(asset->accuracyGraphKnots[i], asset->accuracyGraphKnotCount[i]);
		}

		dump.dump_string(asset->szAltModeInactiveName);
		dump.dump_string(asset->dynamicIconAnimationName);
		dump.dump_string(asset->szLUIWeaponInfoWidgetName);
		dump.dump_string(asset->szLUIWeaponInfoWidgetTag);
		dump.dump_string(asset->szLUICrosshairWidget);
		dump.close();
	}

	WeaponCompleteDef* weapon::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("weapons\\"s + name + ".weapb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing weapon \"%s\"...", name.data());

		auto* asset = read.read_single<WeaponCompleteDef>();
		asset->szInternalName = read.read_string();
		if ((asset->weapDef = read.read_single<WeaponDef>()))
		{
			parse_weapon_def(read, asset->weapDef, this->script_strings_);
		}

		asset->szDisplayName = read.read_string();
		asset->szLootTable = read.read_string();
		this->script_strings_.parse(read, asset->hideTags, 32);

		for (auto& list : asset->attachments)
		{
			if ((list.attachments = read.read_array<WeaponAttachment*>()))
			{
				for (auto i = 0u; i < list.attachmentCount; i++)
				{
					list.attachments[i] = parse_attachment(read, mem);
				}
			}
		}

		this->script_strings_.parse(read, asset->notetrackSoundMapKeys, asset->numNotetrackSoundMappings);
		this->script_strings_.parse(read, asset->notetrackSoundMapValues, asset->numNotetrackSoundMappings);

		if ((asset->notetrackOverrides = read.read_array<NoteTrackToSoundEntry>()))
		{
			for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
			{
				auto& entry = asset->notetrackOverrides[i];
				this->script_strings_.parse(read, entry.attachment);
				this->script_strings_.parse(read, entry.notetrackSoundMapKeys, entry.numSoundMappings);
				this->script_strings_.parse(read, entry.notetrackSoundMapValues, entry.numSoundMappings);
			}
		}

		if ((asset->notetrackSuitEntries = read.read_array<NoteTrackToSuitSoundEntry>()))
		{
			for (auto i = 0u; i < asset->numNotetrackSuitEntries; i++)
			{
				auto& entry = asset->notetrackSuitEntries[i];
				this->script_strings_.parse(read, entry.notetrackSoundMapKeys, entry.numSoundMappings);
				this->script_strings_.parse(read, entry.notetrackSoundMapValues, entry.numSoundMappings);
			}
		}

		asset->killIconMat = read.read_asset<Material>();
		asset->dpadIconMat = read.read_asset<Material>();
		asset->dpadIconInactiveMat = read.read_asset<Material>();
		asset->killIconImg = read.read_asset<GfxImage>();
		asset->dpadIconImg = read.read_asset<GfxImage>();
		asset->dpadIconInactiveImg = read.read_asset<GfxImage>();

		for (auto i = 0; i < 2; i++)
		{
			asset->accuracyGraphKnots[i] = read.read_array<vec2_t>();
		}

		asset->szAltModeInactiveName = read.read_string();
		asset->dynamicIconAnimationName = read.read_string();
		asset->szLUIWeaponInfoWidgetName = read.read_string();
		asset->szLUIWeaponInfoWidgetTag = read.read_string();
		asset->szLUICrosshairWidget = read.read_string();
		read.close();
		return asset;
	}

	void weapon::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponCompleteDef>();
			this->asset_->szInternalName = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void weapon::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (this->referenced())
		{
			return;
		}

		auto* data = this->asset_;
		if (auto* def = data->weapDef)
		{
			this->script_strings_.prepare(buf, def->notetrackRumbleMapKeys, 16);
			this->script_strings_.prepare(buf, def->notetrackRumbleMapValues, 16);
			this->script_strings_.prepare(buf, def->notetrackFXMapKeys, 16);
			this->script_strings_.prepare(buf, def->notetrackFXMapTagValues, 16);

			if (def->weaponOffsetPatterns)
			{
				for (auto i = 0; i < def->numWeaponOffsetPatterns; i++)
				{
					this->script_strings_.prepare(buf, def->weaponOffsetPatterns[i].patternKey);
				}
			}

			if (def->weaponOffsetPatternsKickOrSnapDecay)
			{
				for (auto i = 0; i < def->numWeaponOffsetPatternsKickOrSnapDecay; i++)
				{
					if (const auto pattern = def->weaponOffsetPatternsKickOrSnapDecay[i])
					{
						this->script_strings_.prepare(buf, pattern->patternKey);
					}
				}
			}

			this->script_strings_.prepare(buf, def->stowTag);
			this->script_strings_.prepare(buf, def->stowOffsetAttachTag);
			prepare_pattern_scales(buf, this->script_strings_, def->mountWeaponOffsetPatternScaleInfo);
			prepare_pattern_scales(buf, this->script_strings_, def->crouchedWeaponOffsetPatternScaleInfo);
			prepare_pattern_scales(buf, this->script_strings_, def->proneWeaponOffsetPatternScaleInfo);
		}

		this->script_strings_.prepare(buf, data->hideTags, 32);
		this->script_strings_.prepare(buf, data->notetrackSoundMapKeys, data->numNotetrackSoundMappings);
		this->script_strings_.prepare(buf, data->notetrackSoundMapValues, data->numNotetrackSoundMappings);

		if (data->notetrackOverrides)
		{
			for (auto i = 0u; i < data->numNotetrackOverrides; i++)
			{
				auto& entry = data->notetrackOverrides[i];
				this->script_strings_.prepare(buf, entry.attachment);
				this->script_strings_.prepare(buf, entry.notetrackSoundMapKeys, entry.numSoundMappings);
				this->script_strings_.prepare(buf, entry.notetrackSoundMapValues, entry.numSoundMappings);
			}
		}

		if (data->notetrackSuitEntries)
		{
			for (auto i = 0u; i < data->numNotetrackSuitEntries; i++)
			{
				auto& entry = data->notetrackSuitEntries[i];
				this->script_strings_.prepare(buf, entry.notetrackSoundMapKeys, entry.numSoundMappings);
				this->script_strings_.prepare(buf, entry.notetrackSoundMapValues, entry.numSoundMappings);
			}
		}
	}

	void weapon::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		auto* data = this->asset_;
		if (data->weapDef)
		{
			add_weapon_def(zone, data->weapDef);
		}

		for (const auto& list : data->attachments)
		{
			if (list.attachments)
			{
				for (auto i = 0u; i < list.attachmentCount; i++)
				{
					add_asset(zone, ASSET_TYPE_ATTACHMENT, list.attachments[i]);
				}
			}
		}

		add_asset(zone, ASSET_TYPE_MATERIAL, data->killIconMat);
		add_asset(zone, ASSET_TYPE_MATERIAL, data->dpadIconMat);
		add_asset(zone, ASSET_TYPE_MATERIAL, data->dpadIconInactiveMat);
		add_asset(zone, ASSET_TYPE_IMAGE, data->killIconImg);
		add_asset(zone, ASSET_TYPE_IMAGE, data->dpadIconImg);
		add_asset(zone, ASSET_TYPE_IMAGE, data->dpadIconInactiveImg);
	}

	std::string weapon::name()
	{
		return this->name_;
	}

	std::int32_t weapon::type()
	{
		return ASSET_TYPE_WEAPON;
	}

	void weapon_attachment::read(zone_reader& reader, WeaponAttachment* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);
		reader.read_string(asset->szDisplayName);
		reader.read_script_string(asset->internalName);
		reader.read_script_string(asset->overrideCategoryName);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->fallbackWorldModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->fallbackViewModel);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModelVariations, asset->numModelVariations);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->viewModelVariations, asset->numModelVariations);
		reader.read_script_string(asset->attachPoint);
		reader.read_script_string_array(asset->hideTags, asset->numHideTags);
		reader.read_script_string_array(asset->hideTagsNonViewModel, asset->numHideTagsNonViewModel);
		reader.read_script_string_array(asset->showTags, asset->numShowTags);
		reader.read_asset_array(ASSET_TYPE_RETICLE, asset->reticles, asset->numReticles);
		reader.read_asset_array(ASSET_TYPE_RETICLE, asset->altReticles, asset->numAltReticles);

		reader.read_single(asset->ammogeneral, 3);
		if (const auto ammo_type = reader.read_single(asset->ammotype, 7))
		{
			reader.read_string(ammo_type->szAmmoName);
		}
		reader.read_single(asset->impact, 3);
		reader.read_string(asset->dynamicAttachmentAnimateSeq);
		reader.read_string(asset->dynamicAttachmentIcon);
		if (const auto tracer = reader.read_single(asset->tracer, 7))
		{
			reader.read_asset(ASSET_TYPE_TRACER, tracer->tracerType);
		}
		reader.read_single(asset->penetration, 3);
		reader.read_single(asset->sight, 3);
		reader.read_single(asset->holdBreath, 0);
		reader.read_single(asset->reload, 0);
		reader.read_single(asset->addOns, 0);
		if (const auto general = reader.read_single(asset->general, 7))
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, general->reticleCenter);
			reader.read_asset(ASSET_TYPE_MATERIAL, general->reticleSide);
			reader.read_asset(ASSET_TYPE_MATERIAL, general->reticleOnePiece);
			reader.read_string(general->szLUICrosshairWidget);
		}
		if (const auto laser = reader.read_single(asset->laser, 7))
		{
			reader.read_asset(ASSET_TYPE_LASER, laser->laserTypeViewModel);
			reader.read_asset(ASSET_TYPE_LASER, laser->laserTypeViewModelAlt);
			reader.read_asset(ASSET_TYPE_LASER, laser->laserTypeFriendly);
			reader.read_asset(ASSET_TYPE_LASER, laser->laserTypeEnemy);
		}
		reader.read_single(asset->post, 3);
		reader.read_single(asset->aimAssist, 3);
		reader.read_single(asset->targetAssist, 3);
		reader.read_single(asset->ammunition, 3);
		reader.read_single(asset->damage, 3);
		reader.read_single(asset->melee, 3);
		reader.read_single(asset->idleSettings, 3);
		reader.read_single(asset->adsSettings, 3);
		reader.read_single(asset->adsSettingsMain, 3);
		reader.read_single(asset->adsGunBobSettings, 3);
		reader.read_single(asset->adsGunBobSettingsMain, 3);
		reader.read_single(asset->adsReloadSettings, 0);
		reader.read_single(asset->scopeDriftSettings, 3);
		reader.read_single(asset->scopeDriftSettingsMain, 3);
		if (const auto scales = reader.read_single(asset->adsStanceScales, 7))
		{
			read_pattern_scales(reader, scales->crouchedWeaponOffsetPatternScales);
			read_pattern_scales(reader, scales->proneWeaponOffsetPatternScales);
		}
		reader.read_single(asset->hipSpread, 3);
		reader.read_single(asset->slideSpread, 3);
		reader.read_single(asset->gunKick, 3);
		reader.read_single(asset->gunTilt, 3);
		reader.read_single(asset->kickScaling, 3);
		reader.read_single(asset->viewKick, 3);
		if (const auto overlay = reader.read_single(asset->adsOverlay, 7))
		{
			read_overlay(reader, overlay->overlay);
			reader.read_asset(ASSET_TYPE_ATTACHMENT, overlay->hybridScopeAttachment);
		}
		reader.read_single(asset->outline, 3);
		if (const auto ui = reader.read_single(asset->ui, 7))
		{
			reader.read_asset(ASSET_TYPE_IMAGE, ui->hudIcon);
			reader.read_asset(ASSET_TYPE_IMAGE, ui->pickupIcon);
			reader.read_asset(ASSET_TYPE_IMAGE, ui->dangerIconImg);
			reader.read_asset(ASSET_TYPE_IMAGE, ui->throwBackIconImg);
			reader.read_asset(ASSET_TYPE_MATERIAL, ui->dangerIconMat);
			reader.read_asset(ASSET_TYPE_MATERIAL, ui->throwBackIconMat);
			reader.read_asset(ASSET_TYPE_IMAGE, ui->dpadIconImg);
			reader.read_asset(ASSET_TYPE_IMAGE, ui->dpadIconInactiveImg);
			reader.read_asset(ASSET_TYPE_MATERIAL, ui->dpadIconMat);
			reader.read_asset(ASSET_TYPE_MATERIAL, ui->dpadIconInactiveMat);
		}
		if (const auto rumbles = reader.read_single(asset->rumbles, 7))
		{
			reader.read_asset(ASSET_TYPE_RUMBLE, rumbles->fireRumble);
			reader.read_asset(ASSET_TYPE_RUMBLE, rumbles->meleeImpactRumble);
		}
		if (const auto projectile = reader.read_single(asset->projectile, 7))
		{
			reader.read_asset(ASSET_TYPE_XMODEL, projectile->projectileModel);
			read_vfx(reader, projectile->projExplosionEffect);
			read_sound(reader, projectile->projExplosionSound);
			read_vfx(reader, projectile->projDudEffect);
			read_vfx(reader, projectile->projTimeoutEffect);
			read_sound(reader, projectile->projDudSound);
			read_sound(reader, projectile->projTimeoutSound);
			read_vfx(reader, projectile->vmProjBodyEffect);
			read_vfx(reader, projectile->projBodyEffect);
			read_vfx(reader, projectile->projTrailEffect);
			read_vfx(reader, projectile->projBeaconEffect);
			read_vfx(reader, projectile->projIgnitionEffect);
			read_sound(reader, projectile->projIgnitionSound);
		}
		if (const auto charged = reader.read_single(asset->charged, 7))
		{
			read_vfx(reader, charged->chargeMeterEffect);
			read_sound(reader, charged->chargeUpSound);
			read_sound(reader, charged->chargeDownSound);
			read_sound(reader, charged->chargeDownToUpSound);
			read_sound(reader, charged->chargeUpToDownSound);
			read_sound(reader, charged->chargeMaxSound);
			read_sound(reader, charged->chargeUpSoundPlayer);
			read_sound(reader, charged->chargeDownSoundPlayer);
			read_sound(reader, charged->chargeDownToUpSoundPlayer);
			read_sound(reader, charged->chargeUpToDownSoundPlayer);
			read_sound(reader, charged->chargeMaxSoundPlayer);
			reader.read_asset(ASSET_TYPE_RUMBLE, charged->chargeRumble);
		}
		reader.read_single(asset->adsAltSwitch, 3);
		reader.read_single(asset->regeneration, 3);
		reader.read_single(asset->heat, 3);
		reader.read_single(asset->movement, 3);
		reader.read_single(asset->movementScale, 3);
		reader.read_single(asset->movementScaleMain, 3);
		reader.read_single(asset->burst, 3);
		reader.read_single(asset->hitmarker, 3);
		reader.read_single(asset->adsMovement, 3);
		if (const auto ballistic_info = reader.read_single(asset->ballisticInfo, 7))
		{
			read_ballistic_info(reader, *ballistic_info);
		}
		if (const auto sway = reader.read_single(asset->swaySettings, 7))
		{
			reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, sway->adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, sway->adv.gunGoalViewSpeedToOffset_graph);
		}
		if (const auto scales = reader.read_single(asset->weaponOffsetPatternScaleInfo, 7))
		{
			read_pattern_scales(reader, *scales);
		}
		reader.read_array(asset->locationDamage, 3, 22);
		reader.read_string(asset->szAltModeName);
		reader.read_string(asset->szAltModeInactiveName);
		reader.read_string(asset->szLUIWeaponInfoWidgetName);
		reader.read_string(asset->szLUIWeaponInfoWidgetTag);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->altModeIcon);
		reader.read_single(asset->dlcOverrides, 3);
		reader.read_single(asset->dlcScalars, 3);
		reader.pop_stream();
	}

	void weapon_attachment::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->szInternalName = buf->write_str(this->name());
		write_string(buf, dest->szDisplayName);
		write_asset(zone, ASSET_TYPE_XMODEL, dest->fallbackWorldModel);
		write_asset(zone, ASSET_TYPE_XMODEL, dest->fallbackViewModel);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, dest->worldModelVariations, data->numModelVariations);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, dest->viewModelVariations, data->numModelVariations);
		write_shared_array(buf, dest->hideTags, data->numHideTags, 3);
		write_shared_array(buf, dest->hideTagsNonViewModel, data->numHideTagsNonViewModel, 3);
		write_shared_array(buf, dest->showTags, data->numShowTags, 3);
		write_asset_array(zone, buf, ASSET_TYPE_RETICLE, dest->reticles, data->numReticles);
		write_asset_array(zone, buf, ASSET_TYPE_RETICLE, dest->altReticles, data->numAltReticles);

		write_shared_array(buf, dest->ammogeneral, 1, 3);
		if (const auto ammo_type = write_shared_array(buf, dest->ammotype, 1, 7))
		{
			write_string(buf, ammo_type->szAmmoName);
		}
		write_shared_array(buf, dest->impact, 1, 3);
		write_string(buf, dest->dynamicAttachmentAnimateSeq);
		write_string(buf, dest->dynamicAttachmentIcon);
		if (const auto tracer = write_shared_array(buf, dest->tracer, 1, 7))
		{
			write_asset(zone, ASSET_TYPE_TRACER, tracer->tracerType);
		}
		write_shared_array(buf, dest->penetration, 1, 3);
		write_shared_array(buf, dest->sight, 1, 3);
		write_shared_array(buf, dest->holdBreath, 1, 0);
		write_shared_array(buf, dest->reload, 1, 0);
		write_shared_array(buf, dest->addOns, 1, 0);
		if (const auto general = write_shared_array(buf, dest->general, 1, 7))
		{
			write_asset(zone, ASSET_TYPE_MATERIAL, general->reticleCenter);
			write_asset(zone, ASSET_TYPE_MATERIAL, general->reticleSide);
			write_asset(zone, ASSET_TYPE_MATERIAL, general->reticleOnePiece);
			write_string(buf, general->szLUICrosshairWidget);
		}
		if (const auto laser = write_shared_array(buf, dest->laser, 1, 7))
		{
			write_asset(zone, ASSET_TYPE_LASER, laser->laserTypeViewModel);
			write_asset(zone, ASSET_TYPE_LASER, laser->laserTypeViewModelAlt);
			write_asset(zone, ASSET_TYPE_LASER, laser->laserTypeFriendly);
			write_asset(zone, ASSET_TYPE_LASER, laser->laserTypeEnemy);
		}
		write_shared_array(buf, dest->post, 1, 3);
		write_shared_array(buf, dest->aimAssist, 1, 3);
		write_shared_array(buf, dest->targetAssist, 1, 3);
		write_shared_array(buf, dest->ammunition, 1, 3);
		write_shared_array(buf, dest->damage, 1, 3);
		write_shared_array(buf, dest->melee, 1, 3);
		write_shared_array(buf, dest->idleSettings, 1, 3);
		write_shared_array(buf, dest->adsSettings, 1, 3);
		write_shared_array(buf, dest->adsSettingsMain, 1, 3);
		write_shared_array(buf, dest->adsGunBobSettings, 1, 3);
		write_shared_array(buf, dest->adsGunBobSettingsMain, 1, 3);
		write_shared_array(buf, dest->adsReloadSettings, 1, 0);
		write_shared_array(buf, dest->scopeDriftSettings, 1, 3);
		write_shared_array(buf, dest->scopeDriftSettingsMain, 1, 3);
		if (const auto scales = write_shared_array(buf, dest->adsStanceScales, 1, 7))
		{
			write_pattern_scales(buf, scales->crouchedWeaponOffsetPatternScales);
			write_pattern_scales(buf, scales->proneWeaponOffsetPatternScales);
		}
		write_shared_array(buf, dest->hipSpread, 1, 3);
		write_shared_array(buf, dest->slideSpread, 1, 3);
		write_shared_array(buf, dest->gunKick, 1, 3);
		write_shared_array(buf, dest->gunTilt, 1, 3);
		write_shared_array(buf, dest->kickScaling, 1, 3);
		write_shared_array(buf, dest->viewKick, 1, 3);
		if (const auto overlay = write_shared_array(buf, dest->adsOverlay, 1, 7))
		{
			write_overlay(zone, buf, overlay->overlay);
			write_asset(zone, ASSET_TYPE_ATTACHMENT, overlay->hybridScopeAttachment);
		}
		write_shared_array(buf, dest->outline, 1, 3);
		if (const auto ui = write_shared_array(buf, dest->ui, 1, 7))
		{
			write_asset(zone, ASSET_TYPE_IMAGE, ui->hudIcon);
			write_asset(zone, ASSET_TYPE_IMAGE, ui->pickupIcon);
			write_asset(zone, ASSET_TYPE_IMAGE, ui->dangerIconImg);
			write_asset(zone, ASSET_TYPE_IMAGE, ui->throwBackIconImg);
			write_asset(zone, ASSET_TYPE_MATERIAL, ui->dangerIconMat);
			write_asset(zone, ASSET_TYPE_MATERIAL, ui->throwBackIconMat);
			write_asset(zone, ASSET_TYPE_IMAGE, ui->dpadIconImg);
			write_asset(zone, ASSET_TYPE_IMAGE, ui->dpadIconInactiveImg);
			write_asset(zone, ASSET_TYPE_MATERIAL, ui->dpadIconMat);
			write_asset(zone, ASSET_TYPE_MATERIAL, ui->dpadIconInactiveMat);
		}
		if (const auto rumbles = write_shared_array(buf, dest->rumbles, 1, 7))
		{
			write_asset(zone, ASSET_TYPE_RUMBLE, rumbles->fireRumble);
			write_asset(zone, ASSET_TYPE_RUMBLE, rumbles->meleeImpactRumble);
		}
		if (const auto projectile = write_shared_array(buf, dest->projectile, 1, 7))
		{
			write_asset(zone, ASSET_TYPE_XMODEL, projectile->projectileModel);
			write_vfx(zone, projectile->projExplosionEffect);
			write_sound(buf, projectile->projExplosionSound);
			write_vfx(zone, projectile->projDudEffect);
			write_vfx(zone, projectile->projTimeoutEffect);
			write_sound(buf, projectile->projDudSound);
			write_sound(buf, projectile->projTimeoutSound);
			write_vfx(zone, projectile->vmProjBodyEffect);
			write_vfx(zone, projectile->projBodyEffect);
			write_vfx(zone, projectile->projTrailEffect);
			write_vfx(zone, projectile->projBeaconEffect);
			write_vfx(zone, projectile->projIgnitionEffect);
			write_sound(buf, projectile->projIgnitionSound);
		}
		if (const auto charged = write_shared_array(buf, dest->charged, 1, 7))
		{
			write_vfx(zone, charged->chargeMeterEffect);
			write_sound(buf, charged->chargeUpSound);
			write_sound(buf, charged->chargeDownSound);
			write_sound(buf, charged->chargeDownToUpSound);
			write_sound(buf, charged->chargeUpToDownSound);
			write_sound(buf, charged->chargeMaxSound);
			write_sound(buf, charged->chargeUpSoundPlayer);
			write_sound(buf, charged->chargeDownSoundPlayer);
			write_sound(buf, charged->chargeDownToUpSoundPlayer);
			write_sound(buf, charged->chargeUpToDownSoundPlayer);
			write_sound(buf, charged->chargeMaxSoundPlayer);
			write_asset(zone, ASSET_TYPE_RUMBLE, charged->chargeRumble);
		}
		write_shared_array(buf, dest->adsAltSwitch, 1, 3);
		write_shared_array(buf, dest->regeneration, 1, 3);
		write_shared_array(buf, dest->heat, 1, 3);
		write_shared_array(buf, dest->movement, 1, 3);
		write_shared_array(buf, dest->movementScale, 1, 3);
		write_shared_array(buf, dest->movementScaleMain, 1, 3);
		write_shared_array(buf, dest->burst, 1, 3);
		write_shared_array(buf, dest->hitmarker, 1, 3);
		write_shared_array(buf, dest->adsMovement, 1, 3);
		if (const auto ballistic_info = write_shared_array(buf, dest->ballisticInfo, 1, 7))
		{
			write_ballistic_info(buf, *ballistic_info);
		}
		if (const auto sway = write_shared_array(buf, dest->swaySettings, 1, 7))
		{
			write_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, sway->adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			write_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, sway->adv.gunGoalViewSpeedToOffset_graph);
		}
		if (const auto scales = write_shared_array(buf, dest->weaponOffsetPatternScaleInfo, 1, 7))
		{
			write_pattern_scales(buf, *scales);
		}
		write_shared_array(buf, dest->locationDamage, 22, 3);
		write_string(buf, dest->szAltModeName);
		write_string(buf, dest->szAltModeInactiveName);
		write_string(buf, dest->szLUIWeaponInfoWidgetName);
		write_string(buf, dest->szLUIWeaponInfoWidgetTag);
		write_asset(zone, ASSET_TYPE_IMAGE, dest->altModeIcon);
		write_shared_array(buf, dest->dlcOverrides, 1, 3);
		write_shared_array(buf, dest->dlcScalars, 1, 3);
		buf->pop_stream();
	}

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("attachments\\"s + asset->szInternalName + ".attb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->szInternalName);
		dump.dump_string(asset->szDisplayName);
		dump_script_string(dump, asset->internalName);
		dump_script_string(dump, asset->overrideCategoryName);
		dump.dump_asset(asset->fallbackWorldModel);
		dump.dump_asset(asset->fallbackViewModel);
		dump_asset_array(dump, asset->worldModelVariations, asset->numModelVariations);
		dump_asset_array(dump, asset->viewModelVariations, asset->numModelVariations);
		dump_script_string(dump, asset->attachPoint);
		dump_script_strings(dump, asset->hideTags, asset->numHideTags);
		dump_script_strings(dump, asset->hideTagsNonViewModel, asset->numHideTagsNonViewModel);
		dump_script_strings(dump, asset->showTags, asset->numShowTags);
		dump_asset_array(dump, asset->reticles, asset->numReticles);
		dump_asset_array(dump, asset->altReticles, asset->numAltReticles);

		dump.dump_single(asset->ammogeneral);
		dump.dump_single(asset->ammotype);
		if (asset->ammotype)
		{
			dump.dump_string(asset->ammotype->szAmmoName);
		}
		dump.dump_single(asset->impact);
		dump.dump_string(asset->dynamicAttachmentAnimateSeq);
		dump.dump_string(asset->dynamicAttachmentIcon);
		dump.dump_single(asset->tracer);
		if (asset->tracer)
		{
			dump.dump_asset(asset->tracer->tracerType);
		}
		dump.dump_single(asset->penetration);
		dump.dump_single(asset->sight);
		dump.dump_single(asset->holdBreath);
		dump.dump_single(asset->reload);
		dump.dump_single(asset->addOns);
		dump.dump_single(asset->general);
		if (const auto general = asset->general)
		{
			dump.dump_asset(general->reticleCenter);
			dump.dump_asset(general->reticleSide);
			dump.dump_asset(general->reticleOnePiece);
			dump.dump_string(general->szLUICrosshairWidget);
		}
		dump.dump_single(asset->laser);
		if (const auto laser = asset->laser)
		{
			dump.dump_asset(laser->laserTypeViewModel);
			dump.dump_asset(laser->laserTypeViewModelAlt);
			dump.dump_asset(laser->laserTypeFriendly);
			dump.dump_asset(laser->laserTypeEnemy);
		}
		dump.dump_single(asset->post);
		dump.dump_single(asset->aimAssist);
		dump.dump_single(asset->targetAssist);
		dump.dump_single(asset->ammunition);
		dump.dump_single(asset->damage);
		dump.dump_single(asset->melee);
		dump.dump_single(asset->idleSettings);
		dump.dump_single(asset->adsSettings);
		dump.dump_single(asset->adsSettingsMain);
		dump.dump_single(asset->adsGunBobSettings);
		dump.dump_single(asset->adsGunBobSettingsMain);
		dump.dump_single(asset->adsReloadSettings);
		dump.dump_single(asset->scopeDriftSettings);
		dump.dump_single(asset->scopeDriftSettingsMain);
		dump.dump_single(asset->adsStanceScales);
		if (const auto scales = asset->adsStanceScales)
		{
			dump_pattern_scales(dump, scales->crouchedWeaponOffsetPatternScales);
			dump_pattern_scales(dump, scales->proneWeaponOffsetPatternScales);
		}
		dump.dump_single(asset->hipSpread);
		dump.dump_single(asset->slideSpread);
		dump.dump_single(asset->gunKick);
		dump.dump_single(asset->gunTilt);
		dump.dump_single(asset->kickScaling);
		dump.dump_single(asset->viewKick);
		dump.dump_single(asset->adsOverlay);
		if (const auto overlay = asset->adsOverlay)
		{
			dump_overlay(dump, overlay->overlay);
			dump.dump_string(overlay->hybridScopeAttachment ? overlay->hybridScopeAttachment->szInternalName : nullptr);
		}
		dump.dump_single(asset->outline);
		dump.dump_single(asset->ui);
		if (const auto ui = asset->ui)
		{
			dump.dump_asset(ui->hudIcon);
			dump.dump_asset(ui->pickupIcon);
			dump.dump_asset(ui->dangerIconImg);
			dump.dump_asset(ui->throwBackIconImg);
			dump.dump_asset(ui->dangerIconMat);
			dump.dump_asset(ui->throwBackIconMat);
			dump.dump_asset(ui->dpadIconImg);
			dump.dump_asset(ui->dpadIconInactiveImg);
			dump.dump_asset(ui->dpadIconMat);
			dump.dump_asset(ui->dpadIconInactiveMat);
		}
		dump.dump_single(asset->rumbles);
		if (const auto rumbles = asset->rumbles)
		{
			dump.dump_asset(rumbles->fireRumble);
			dump.dump_asset(rumbles->meleeImpactRumble);
		}
		dump.dump_single(asset->projectile);
		if (const auto projectile = asset->projectile)
		{
			dump.dump_asset(projectile->projectileModel);
			dump.dump_asset(projectile->projExplosionEffect.particleSystemDef);
			dump.dump_string(projectile->projExplosionSound.name);
			dump.dump_asset(projectile->projDudEffect.particleSystemDef);
			dump.dump_asset(projectile->projTimeoutEffect.particleSystemDef);
			dump.dump_string(projectile->projDudSound.name);
			dump.dump_string(projectile->projTimeoutSound.name);
			dump.dump_asset(projectile->vmProjBodyEffect.particleSystemDef);
			dump.dump_asset(projectile->projBodyEffect.particleSystemDef);
			dump.dump_asset(projectile->projTrailEffect.particleSystemDef);
			dump.dump_asset(projectile->projBeaconEffect.particleSystemDef);
			dump.dump_asset(projectile->projIgnitionEffect.particleSystemDef);
			dump.dump_string(projectile->projIgnitionSound.name);
		}
		dump.dump_single(asset->charged);
		if (const auto charged = asset->charged)
		{
			dump.dump_asset(charged->chargeMeterEffect.particleSystemDef);
			dump.dump_string(charged->chargeUpSound.name);
			dump.dump_string(charged->chargeDownSound.name);
			dump.dump_string(charged->chargeDownToUpSound.name);
			dump.dump_string(charged->chargeUpToDownSound.name);
			dump.dump_string(charged->chargeMaxSound.name);
			dump.dump_string(charged->chargeUpSoundPlayer.name);
			dump.dump_string(charged->chargeDownSoundPlayer.name);
			dump.dump_string(charged->chargeDownToUpSoundPlayer.name);
			dump.dump_string(charged->chargeUpToDownSoundPlayer.name);
			dump.dump_string(charged->chargeMaxSoundPlayer.name);
			dump.dump_asset(charged->chargeRumble);
		}
		dump.dump_single(asset->adsAltSwitch);
		dump.dump_single(asset->regeneration);
		dump.dump_single(asset->heat);
		dump.dump_single(asset->movement);
		dump.dump_single(asset->movementScale);
		dump.dump_single(asset->movementScaleMain);
		dump.dump_single(asset->burst);
		dump.dump_single(asset->hitmarker);
		dump.dump_single(asset->adsMovement);
		dump.dump_single(asset->ballisticInfo);
		if (asset->ballisticInfo)
		{
			dump_ballistic_info(dump, *asset->ballisticInfo);
		}
		dump.dump_single(asset->swaySettings);
		if (const auto sway = asset->swaySettings)
		{
			dump.dump_asset(sway->adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			dump.dump_asset(sway->adv.gunGoalViewSpeedToOffset_graph);
		}
		dump.dump_single(asset->weaponOffsetPatternScaleInfo);
		if (asset->weaponOffsetPatternScaleInfo)
		{
			dump_pattern_scales(dump, *asset->weaponOffsetPatternScaleInfo);
		}
		dump.dump_array(asset->locationDamage, 22);
		dump.dump_string(asset->szAltModeName);
		dump.dump_string(asset->szAltModeInactiveName);
		dump.dump_string(asset->szLUIWeaponInfoWidgetName);
		dump.dump_string(asset->szLUIWeaponInfoWidgetTag);
		dump.dump_asset(asset->altModeIcon);
		dump.dump_single(asset->dlcOverrides);
		dump.dump_single(asset->dlcScalars);
		dump.close();
	}

	WeaponAttachment* weapon_attachment::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("attachments\\"s + name + ".attb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing attachment \"%s\"...", name.data());

		auto& strings = this->script_strings_;
		auto* asset = read.read_single<WeaponAttachment>();
		asset->szInternalName = read.read_string();
		asset->szDisplayName = read.read_string();
		strings.parse(read, asset->internalName);
		strings.parse(read, asset->overrideCategoryName);
		parse_asset(read, asset->fallbackWorldModel);
		parse_asset(read, asset->fallbackViewModel);
		parse_asset_array(read, asset->worldModelVariations, asset->numModelVariations);
		parse_asset_array(read, asset->viewModelVariations, asset->numModelVariations);
		strings.parse(read, asset->attachPoint);
		strings.parse(read, asset->hideTags, asset->numHideTags);
		strings.parse(read, asset->hideTagsNonViewModel, asset->numHideTagsNonViewModel);
		strings.parse(read, asset->showTags, asset->numShowTags);
		parse_asset_array(read, asset->reticles, asset->numReticles);
		parse_asset_array(read, asset->altReticles, asset->numAltReticles);

		parse_single(read, asset->ammogeneral);
		if (const auto ammo_type = parse_single(read, asset->ammotype))
		{
			ammo_type->szAmmoName = read.read_string();
		}
		parse_single(read, asset->impact);
		asset->dynamicAttachmentAnimateSeq = read.read_string();
		asset->dynamicAttachmentIcon = read.read_string();
		if (const auto tracer = parse_single(read, asset->tracer))
		{
			parse_asset(read, tracer->tracerType);
		}
		parse_single(read, asset->penetration);
		parse_single(read, asset->sight);
		parse_single(read, asset->holdBreath);
		parse_single(read, asset->reload);
		parse_single(read, asset->addOns);
		if (const auto general = parse_single(read, asset->general))
		{
			parse_asset(read, general->reticleCenter);
			parse_asset(read, general->reticleSide);
			parse_asset(read, general->reticleOnePiece);
			general->szLUICrosshairWidget = read.read_string();
		}
		if (const auto laser = parse_single(read, asset->laser))
		{
			parse_asset(read, laser->laserTypeViewModel);
			parse_asset(read, laser->laserTypeViewModelAlt);
			parse_asset(read, laser->laserTypeFriendly);
			parse_asset(read, laser->laserTypeEnemy);
		}
		parse_single(read, asset->post);
		parse_single(read, asset->aimAssist);
		parse_single(read, asset->targetAssist);
		parse_single(read, asset->ammunition);
		parse_single(read, asset->damage);
		parse_single(read, asset->melee);
		parse_single(read, asset->idleSettings);
		parse_single(read, asset->adsSettings);
		parse_single(read, asset->adsSettingsMain);
		parse_single(read, asset->adsGunBobSettings);
		parse_single(read, asset->adsGunBobSettingsMain);
		parse_single(read, asset->adsReloadSettings);
		parse_single(read, asset->scopeDriftSettings);
		parse_single(read, asset->scopeDriftSettingsMain);
		if (const auto scales = parse_single(read, asset->adsStanceScales))
		{
			parse_pattern_scales(read, strings, scales->crouchedWeaponOffsetPatternScales);
			parse_pattern_scales(read, strings, scales->proneWeaponOffsetPatternScales);
		}
		parse_single(read, asset->hipSpread);
		parse_single(read, asset->slideSpread);
		parse_single(read, asset->gunKick);
		parse_single(read, asset->gunTilt);
		parse_single(read, asset->kickScaling);
		parse_single(read, asset->viewKick);
		if (const auto overlay = parse_single(read, asset->adsOverlay))
		{
			parse_overlay(read, overlay->overlay);
			overlay->hybridScopeAttachment = parse_attachment(read, mem);
		}
		parse_single(read, asset->outline);
		if (const auto ui = parse_single(read, asset->ui))
		{
			parse_asset(read, ui->hudIcon);
			parse_asset(read, ui->pickupIcon);
			parse_asset(read, ui->dangerIconImg);
			parse_asset(read, ui->throwBackIconImg);
			parse_asset(read, ui->dangerIconMat);
			parse_asset(read, ui->throwBackIconMat);
			parse_asset(read, ui->dpadIconImg);
			parse_asset(read, ui->dpadIconInactiveImg);
			parse_asset(read, ui->dpadIconMat);
			parse_asset(read, ui->dpadIconInactiveMat);
		}
		if (const auto rumbles = parse_single(read, asset->rumbles))
		{
			parse_asset(read, rumbles->fireRumble);
			parse_asset(read, rumbles->meleeImpactRumble);
		}
		if (const auto projectile = parse_single(read, asset->projectile))
		{
			parse_asset(read, projectile->projectileModel);
			parse_vfx(read, projectile->projExplosionEffect);
			parse_sound(read, projectile->projExplosionSound);
			parse_vfx(read, projectile->projDudEffect);
			parse_vfx(read, projectile->projTimeoutEffect);
			parse_sound(read, projectile->projDudSound);
			parse_sound(read, projectile->projTimeoutSound);
			parse_vfx(read, projectile->vmProjBodyEffect);
			parse_vfx(read, projectile->projBodyEffect);
			parse_vfx(read, projectile->projTrailEffect);
			parse_vfx(read, projectile->projBeaconEffect);
			parse_vfx(read, projectile->projIgnitionEffect);
			parse_sound(read, projectile->projIgnitionSound);
		}
		if (const auto charged = parse_single(read, asset->charged))
		{
			parse_vfx(read, charged->chargeMeterEffect);
			parse_sound(read, charged->chargeUpSound);
			parse_sound(read, charged->chargeDownSound);
			parse_sound(read, charged->chargeDownToUpSound);
			parse_sound(read, charged->chargeUpToDownSound);
			parse_sound(read, charged->chargeMaxSound);
			parse_sound(read, charged->chargeUpSoundPlayer);
			parse_sound(read, charged->chargeDownSoundPlayer);
			parse_sound(read, charged->chargeDownToUpSoundPlayer);
			parse_sound(read, charged->chargeUpToDownSoundPlayer);
			parse_sound(read, charged->chargeMaxSoundPlayer);
			parse_asset(read, charged->chargeRumble);
		}
		parse_single(read, asset->adsAltSwitch);
		parse_single(read, asset->regeneration);
		parse_single(read, asset->heat);
		parse_single(read, asset->movement);
		parse_single(read, asset->movementScale);
		parse_single(read, asset->movementScaleMain);
		parse_single(read, asset->burst);
		parse_single(read, asset->hitmarker);
		parse_single(read, asset->adsMovement);
		if (const auto ballistic_info = parse_single(read, asset->ballisticInfo))
		{
			parse_ballistic_info(read, *ballistic_info);
		}
		if (const auto sway = parse_single(read, asset->swaySettings))
		{
			parse_asset(read, sway->adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			parse_asset(read, sway->adv.gunGoalViewSpeedToOffset_graph);
		}
		if (const auto scales = parse_single(read, asset->weaponOffsetPatternScaleInfo))
		{
			parse_pattern_scales(read, strings, *scales);
		}
		asset->locationDamage = read.read_array<float>();
		asset->szAltModeName = read.read_string();
		asset->szAltModeInactiveName = read.read_string();
		asset->szLUIWeaponInfoWidgetName = read.read_string();
		asset->szLUIWeaponInfoWidgetTag = read.read_string();
		parse_asset(read, asset->altModeIcon);
		parse_single(read, asset->dlcOverrides);
		parse_single(read, asset->dlcScalars);
		read.close();
		return asset;
	}

	void weapon_attachment::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponAttachment>();
			this->asset_->szInternalName = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void weapon_attachment::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (this->referenced())
		{
			return;
		}

		auto& strings = this->script_strings_;
		auto* data = this->asset_;
		strings.prepare(buf, data->internalName);
		strings.prepare(buf, data->overrideCategoryName);
		strings.prepare(buf, data->attachPoint);
		strings.prepare(buf, data->hideTags, data->numHideTags);
		strings.prepare(buf, data->hideTagsNonViewModel, data->numHideTagsNonViewModel);
		strings.prepare(buf, data->showTags, data->numShowTags);

		if (const auto scales = data->adsStanceScales)
		{
			prepare_pattern_scales(buf, strings, scales->crouchedWeaponOffsetPatternScales);
			prepare_pattern_scales(buf, strings, scales->proneWeaponOffsetPatternScales);
		}

		if (data->weaponOffsetPatternScaleInfo)
		{
			prepare_pattern_scales(buf, strings, *data->weaponOffsetPatternScaleInfo);
		}
	}

	void weapon_attachment::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		auto* data = this->asset_;
		add_asset(zone, ASSET_TYPE_XMODEL, data->fallbackWorldModel);
		add_asset(zone, ASSET_TYPE_XMODEL, data->fallbackViewModel);

		for (auto i = 0u; i < data->numModelVariations; i++)
		{
			add_asset(zone, ASSET_TYPE_XMODEL, data->worldModelVariations ? data->worldModelVariations[i] : nullptr);
			add_asset(zone, ASSET_TYPE_XMODEL, data->viewModelVariations ? data->viewModelVariations[i] : nullptr);
		}

		for (auto i = 0u; data->reticles && i < data->numReticles; i++)
		{
			add_asset(zone, ASSET_TYPE_RETICLE, data->reticles[i]);
		}

		for (auto i = 0u; data->altReticles && i < data->numAltReticles; i++)
		{
			add_asset(zone, ASSET_TYPE_RETICLE, data->altReticles[i]);
		}

		if (data->tracer)
		{
			add_asset(zone, ASSET_TYPE_TRACER, data->tracer->tracerType);
		}

		if (const auto general = data->general)
		{
			add_asset(zone, ASSET_TYPE_MATERIAL, general->reticleCenter);
			add_asset(zone, ASSET_TYPE_MATERIAL, general->reticleSide);
			add_asset(zone, ASSET_TYPE_MATERIAL, general->reticleOnePiece);
		}

		if (const auto laser = data->laser)
		{
			add_asset(zone, ASSET_TYPE_LASER, laser->laserTypeViewModel);
			add_asset(zone, ASSET_TYPE_LASER, laser->laserTypeViewModelAlt);
			add_asset(zone, ASSET_TYPE_LASER, laser->laserTypeFriendly);
			add_asset(zone, ASSET_TYPE_LASER, laser->laserTypeEnemy);
		}

		if (const auto overlay = data->adsOverlay)
		{
			add_overlay(zone, overlay->overlay);
			add_asset(zone, ASSET_TYPE_ATTACHMENT, overlay->hybridScopeAttachment);
		}

		if (const auto ui = data->ui)
		{
			add_asset(zone, ASSET_TYPE_IMAGE, ui->hudIcon);
			add_asset(zone, ASSET_TYPE_IMAGE, ui->pickupIcon);
			add_asset(zone, ASSET_TYPE_IMAGE, ui->dangerIconImg);
			add_asset(zone, ASSET_TYPE_IMAGE, ui->throwBackIconImg);
			add_asset(zone, ASSET_TYPE_MATERIAL, ui->dangerIconMat);
			add_asset(zone, ASSET_TYPE_MATERIAL, ui->throwBackIconMat);
			add_asset(zone, ASSET_TYPE_IMAGE, ui->dpadIconImg);
			add_asset(zone, ASSET_TYPE_IMAGE, ui->dpadIconInactiveImg);
			add_asset(zone, ASSET_TYPE_MATERIAL, ui->dpadIconMat);
			add_asset(zone, ASSET_TYPE_MATERIAL, ui->dpadIconInactiveMat);
		}

		if (const auto rumbles = data->rumbles)
		{
			add_asset(zone, ASSET_TYPE_RUMBLE, rumbles->fireRumble);
			add_asset(zone, ASSET_TYPE_RUMBLE, rumbles->meleeImpactRumble);
		}

		if (const auto projectile = data->projectile)
		{
			add_asset(zone, ASSET_TYPE_XMODEL, projectile->projectileModel);
			for (const auto effect : {&projectile->projExplosionEffect, &projectile->projDudEffect, &projectile->projTimeoutEffect,
				&projectile->vmProjBodyEffect, &projectile->projBodyEffect, &projectile->projTrailEffect, &projectile->projBeaconEffect,
				&projectile->projIgnitionEffect})
			{
				add_asset(zone, ASSET_TYPE_VFX, effect->particleSystemDef);
			}
		}

		if (const auto charged = data->charged)
		{
			add_asset(zone, ASSET_TYPE_VFX, charged->chargeMeterEffect.particleSystemDef);
			add_asset(zone, ASSET_TYPE_RUMBLE, charged->chargeRumble);
		}

		if (const auto sway = data->swaySettings)
		{
			add_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, sway->adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			add_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, sway->adv.gunGoalViewSpeedToOffset_graph);
		}

		add_asset(zone, ASSET_TYPE_IMAGE, data->altModeIcon);
	}

	std::string weapon_attachment::name()
	{
		return this->name_;
	}

	std::int32_t weapon_attachment::type()
	{
		return ASSET_TYPE_ATTACHMENT;
	}

	void vfx_package::read(zone_reader& reader, WeaponVFXPackage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto defs = reader.read_single(asset->vfx, 7))
		{
			for (const auto effect : vfx_package_effects)
			{
				read_vfx(reader, defs->*effect);
			}
		}
		reader.pop_stream();
	}

	void vfx_package::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (const auto defs = write_shared_array(buf, dest->vfx, 1, 7))
		{
			for (const auto effect : vfx_package_effects)
			{
				write_vfx(zone, defs->*effect);
			}
		}
		buf->pop_stream();
	}

	void vfx_package::dump(WeaponVFXPackage* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("vfxpkg\\"s + asset->name + ".vfxpkgb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_single(asset->vfx);
		if (asset->vfx)
		{
			for (const auto effect : vfx_package_effects)
			{
				dump.dump_asset((asset->vfx->*effect).particleSystemDef);
			}
		}
		dump.close();
	}

	WeaponVFXPackage* vfx_package::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("vfxpkg\\"s + name + ".vfxpkgb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vfxpkg \"%s\"...", name.data());

		auto* asset = read.read_single<WeaponVFXPackage>();
		asset->name = read.read_string();
		if (const auto defs = parse_single(read, asset->vfx))
		{
			for (const auto effect : vfx_package_effects)
			{
				parse_vfx(read, defs->*effect);
			}
		}
		read.close();
		return asset;
	}

	void vfx_package::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponVFXPackage>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void vfx_package::load_depending(zone_base* zone)
	{
		if (this->referenced() || !this->asset_->vfx)
		{
			return;
		}

		for (const auto effect : vfx_package_effects)
		{
			add_asset(zone, ASSET_TYPE_VFX, (this->asset_->vfx->*effect).particleSystemDef);
		}
	}

	std::string vfx_package::name()
	{
		return this->name_;
	}

	std::int32_t vfx_package::type()
	{
		return ASSET_TYPE_VFX_PACKAGE;
	}
}
