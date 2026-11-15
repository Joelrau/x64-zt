#include <std_include.hpp>
#include "weapon.hpp"

namespace zonetool::iw8
{
	namespace
	{
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

		void read_weapon_def(zone_reader& reader, WeaponDef* def, const WeaponCompleteDef* complete)
		{
			reader.read_string(def->szOverlayName);

			for (auto model : {&def->gunXModel, &def->gunXModelLeftHand, &def->gunXModelRightHand, &def->defaultViewModel,
				&def->defaultWorldModelLeftHand, &def->defaultWorldModelRightHand, &def->worldModel, &def->worldXModelLeftHand,
				&def->worldXModelRightHand, &def->defaultWorldModel, &def->censorshipWorldModel, &def->censorshipWorldModelLeftHand,
				&def->censorshipWorldModelRightHand, &def->playerShadowModel, &def->playerShadowModelLeftHand,
				&def->playerShadowModelRightHand, &def->handXModel})
			{
				reader.read_asset(ASSET_TYPE_XMODEL, *model);
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

			for (auto gesture : {&def->shieldDeployGesture, &def->shieldFireWeapGesture, &def->shieldDeployWhileFiring,
				&def->shieldRetractWhileFiring, &def->shieldBashGesture})
			{
				reader.read_asset(ASSET_TYPE_GESTURE, *gesture);
			}

			read_vfx(reader, def->shieldMeleeFx);
			reader.read_asset(ASSET_TYPE_RUMBLE, def->shieldHitRumble);
			reader.read_asset(ASSET_TYPE_RUMBLE, def->shieldMissRumble);
			reader.read_asset(ASSET_TYPE_XMODEL, def->detonatorWorldModel);

			for (auto gesture : {&def->gestureAnimation, &def->gesturePullback, &def->gesturePullbackAlt, &def->gestureThrow,
				&def->gestureThrowAlt, &def->gestureDetonate, &def->gestureDetonateAlt})
			{
				reader.read_asset(ASSET_TYPE_GESTURE, *gesture);
			}

			read_vfx(reader, def->cameraFireEffect);
			read_vfx(reader, def->chargeMeterEffect);

			for (auto sound : {&def->chargeUpSound, &def->chargeDownSound, &def->chargeUpSoundPlayer, &def->chargeDownSoundPlayer,
				&def->chargeDownToUpSound, &def->chargeDownToUpSoundPlayer, &def->chargeUpToDownSound, &def->chargeUpToDownSoundPlayer,
				&def->chargeMaxSound, &def->chargeMaxSoundPlayer})
			{
				read_sound(reader, *sound);
			}

			reader.read_asset(ASSET_TYPE_RUMBLE, def->chargeRumble);
			reader.read_script_string(def->stowTag);
			reader.read_asset(ASSET_TYPE_XMODEL, def->stowOffsetModel);
			reader.read_script_string(def->stowOffsetAttachTag);
			read_vfx(reader, def->deployEffect);

			if (const auto overrides = reader.read_array(def->animOverrides, 7, def->numAnimOverrides))
			{
				for (auto i = 0u; i < def->numAnimOverrides; i++)
				{
					read_override_bindings(reader, overrides[i].bindings, overrides[i].numBindings);
					reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, overrides[i].overrides);
					reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, overrides[i].overridesAlt);
				}
			}

			for (auto& carry : def->carryAnimOverrides)
			{
				reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, carry.animPackage);
				reader.read_asset(ASSET_TYPE_ANIM_PACKAGE, carry.animPackageAlt);
			}

			if (const auto overrides = reader.read_array(def->sfxOverrides, 7, def->numSfxOverrides))
			{
				for (auto i = 0u; i < def->numSfxOverrides; i++)
				{
					read_override_bindings(reader, overrides[i].bindings, overrides[i].numBindings);
					reader.read_asset(ASSET_TYPE_SFX_PACKAGE, overrides[i].overrides);
					reader.read_asset(ASSET_TYPE_SFX_PACKAGE, overrides[i].overridesAlt);
				}
			}

			if (const auto overrides = reader.read_array(def->vfxOverrides, 7, def->numVfxOverrides))
			{
				for (auto i = 0u; i < def->numVfxOverrides; i++)
				{
					read_override_bindings(reader, overrides[i].bindings, overrides[i].numBindings);
					reader.read_asset(ASSET_TYPE_VFX_PACKAGE, overrides[i].overrides);
					reader.read_asset(ASSET_TYPE_VFX_PACKAGE, overrides[i].overridesAlt);
				}
			}

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

	void vfx_package::read(zone_reader& reader, WeaponVFXPackage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto defs = reader.read_single(asset->vfx, 7))
		{
			for (auto effect : {&defs->viewFlashEffect, &defs->worldFlashEffect, &defs->viewFlashADSEffect, &defs->viewFlashFirstEffect,
				&defs->worldFlashFirstEffect, &defs->viewFlashFirstADSEffect, &defs->viewFlashOverchargeEffect,
				&defs->viewFlashOverchargeADSEffect, &defs->worldFlashOverchargeEffect, &defs->viewShellEjectEffect,
				&defs->worldShellEjectEffect, &defs->viewLastShotEjectEffect, &defs->worldLastShotEjectEffect, &defs->viewMagEjectEffect,
				&defs->viewMuzzleSmokeEffect, &defs->worldMuzzleSmokeEffect, &defs->viewBarrelSmokeEffect, &defs->worldBarrelSmokeEffect,
				&defs->viewEjectionPortSmokeEffect, &defs->worldEjectionPortSmokeEffect, &defs->viewDlcEffect0, &defs->worldDlcEffect0,
				&defs->viewDlcEffect1, &defs->worldDlcEffect1, &defs->viewDlcEffect2, &defs->worldDlcEffect2, &defs->viewDlcEffect3,
				&defs->worldDlcEffect3})
			{
				read_vfx(reader, *effect);
			}
		}
		reader.pop_stream();
	}
}
