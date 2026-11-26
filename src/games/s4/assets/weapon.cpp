#include <std_include.hpp>
#include "weapon.hpp"

namespace zonetool::s4
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

		template <typename Walk>
		void walk_effect(Walk& walk, FxCombinedDef& effect)
		{
			walk.asset(ASSET_TYPE_VFX, effect.particleSystemDef);
		}

		template <typename Walk>
		void walk_script_string_array(Walk& walk, scr_string_t*& field, const std::size_t count)
		{
			if (const auto strings = walk.array(field, count, 3))
			{
				walk.inline_script_strings(strings, count);
			}
		}

		template <typename Walk>
		void walk_overlay(Walk& walk, ADSOverlay& overlay)
		{
			walk.asset(ASSET_TYPE_MATERIAL, overlay.shaderMat);
			walk.asset(ASSET_TYPE_MATERIAL, overlay.shaderLowResMat);
			walk.asset(ASSET_TYPE_MATERIAL, overlay.shaderEMPMat);
			walk.asset(ASSET_TYPE_MATERIAL, overlay.shaderEMPLowResMat);
			walk.asset(ASSET_TYPE_IMAGE, overlay.shaderImg);
			walk.asset(ASSET_TYPE_IMAGE, overlay.shaderLowResImg);
			walk.asset(ASSET_TYPE_IMAGE, overlay.shaderEMPImg);
			walk.asset(ASSET_TYPE_IMAGE, overlay.shaderEMPLowResImg);
			walk.string(overlay.visionSetName);
		}

		template <typename Walk>
		void walk_sway_graphs(Walk& walk, SwaySettings& sway)
		{
			walk.asset(ASSET_TYPE_RUMBLE_GRAPH, sway.adv.torsoGoalViewSpeedToMaxDeadzone_graph);
			walk.asset(ASSET_TYPE_RUMBLE_GRAPH, sway.adv.gunGoalViewSpeedToOffset_graph);
		}

		template <typename Walk>
		void walk_pattern_scales(Walk& walk, WeaponOffsetPatternScaleInfo& info)
		{
			if (const auto scales = walk.array(info.patternScales, info.numPatternScales, 3))
			{
				for (auto i = 0; i < info.numPatternScales; i++)
				{
					walk.script_string(scales[i].patternKey);
				}
			}
		}

		template <typename Walk>
		void walk_ballistic(Walk& walk, BallisticInfoCalculated*& field)
		{
			if (const auto calculated = walk.array(field, 1, 7))
			{
				walk.array(calculated->distances, calculated->numDistanceEntries, 3);
			}
		}

		template <typename Walk, typename T>
		void walk_overrides(Walk& walk, const XAssetType type, T*& field, const unsigned int count)
		{
			if (const auto overrides = walk.array(field, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					auto& entry = overrides[i];
					if (const auto bindings = walk.array(entry.bindings, entry.numBindings, 7))
					{
						for (auto j = 0u; j < entry.numBindings; j++)
						{
							walk.array(bindings[j].bindingLists, bindings[j].numBindingLists, 3);
						}
					}

					walk.asset(type, entry.overrides);
					walk.asset(type, entry.overridesAlt);
				}
			}
		}

		template <typename Walk>
		void walk_offset_patterns(Walk& walk, WeaponDef& def)
		{
			if (const auto patterns = walk.array(def.weaponOffsetPatterns, def.numWeaponOffsetPatterns, 3))
			{
				for (auto i = 0; i < def.numWeaponOffsetPatterns; i++)
				{
					walk.script_string(patterns[i].patternKey);
				}
			}

			if (const auto patterns = walk.array(def.weaponOffsetPatternsKickOrSnapDecay, def.numWeaponOffsetPatternsKickOrSnapDecay, 7))
			{
				for (auto i = 0; i < def.numWeaponOffsetPatternsKickOrSnapDecay; i++)
				{
					if (const auto pattern = walk.array(patterns[i], 1, 3))
					{
						walk.script_string(pattern->patternKey);
					}
				}
			}
		}

		template <typename Walk>
		void walk_weapon_def(Walk& walk, WeaponDef& def)
		{
			walk.string(def.szOverlayName);
			for (const auto model : weapon_def_models)
			{
				walk.asset(ASSET_TYPE_XMODEL, def.*model);
			}

			walk.asset(ASSET_TYPE_ANIM_PACKAGE, def.szXAnims);
			walk.asset(ASSET_TYPE_ANIM_PACKAGE, def.szXAnimsRightHanded);
			walk.asset(ASSET_TYPE_ANIM_PACKAGE, def.szXAnimsLeftHanded);
			walk.asset(ASSET_TYPE_XANIMPARTS, def.turretRaiseAnim);
			walk.asset(ASSET_TYPE_XANIMPARTS, def.turretIdleAnim);
			walk.asset(ASSET_TYPE_XANIMPARTS, def.turretFireAnim);
			walk.asset(ASSET_TYPE_CURVE, def.damageInfo.damageCurve);
			walk.string(def.szModeName);
			walk_script_string_array(walk, def.notetrackRumbleMapKeys, 16);
			walk_script_string_array(walk, def.notetrackRumbleMapValues, 16);
			walk_script_string_array(walk, def.notetrackFXMapKeys, 16);
			if (const auto effects = walk.array(def.notetrackFXMapValues, 16, 7))
			{
				for (auto i = 0; i < 16; i++)
				{
					walk_effect(walk, effects[i]);
				}
			}

			walk_script_string_array(walk, def.notetrackFXMapTagValues, 16);
			walk.asset(ASSET_TYPE_VFX_PACKAGE, def.vfxPackage);
			walk.asset(ASSET_TYPE_SFX_PACKAGE, def.sfxPackage);
			walk.asset(ASSET_TYPE_MATERIAL, def.reticleCenter);
			walk.asset(ASSET_TYPE_MATERIAL, def.reticleSide);
			walk.asset(ASSET_TYPE_MATERIAL, def.reticleOnePiece);
			walk.asset(ASSET_TYPE_XMODEL, def.worldClipModel);
			walk.asset(ASSET_TYPE_XMODEL, def.rocketModel);
			for (auto& image : def.iconImages)
			{
				walk.asset(ASSET_TYPE_IMAGE, image);
			}

			for (auto& material : def.iconMaterials)
			{
				walk.asset(ASSET_TYPE_MATERIAL, material);
			}

			walk.string(def.szAmmoName);
			walk.string(def.szSharedAmmoCapName);
			walk_overlay(walk, def.overlay);
			walk_sway_graphs(walk, def.swaySettings);
			walk.asset(ASSET_TYPE_XMODEL, def.projectileModel);
			walk.asset(ASSET_TYPE_CURVE, def.projectileCurve);
			walk_effect(walk, def.projExplosionEffect);
			walk_effect(walk, def.projDudEffect);
			walk_effect(walk, def.projTimeoutEffect);
			walk.array(def.parallelBounce, 64, 3);
			walk.array(def.perpendicularBounce, 64, 3);
			walk_effect(walk, def.vmProjBodyEffect);
			walk_effect(walk, def.projBodyEffect);
			walk_effect(walk, def.projTrailEffect);
			walk_effect(walk, def.projBeaconEffect);
			walk_effect(walk, def.unknownProjEffect);
			walk_effect(walk, def.projIgnitionEffect);
			walk_offset_patterns(walk, def);
			for (auto& accuracy : def.accuracy)
			{
				walk.asset(ASSET_TYPE_WEAPON_ACCURACY, accuracy);
			}

			walk.string(def.szUseHintString);
			walk.string(def.dropHintString);
			walk.string(def.szScript);
			for (auto& beam : def.beams)
			{
				walk.asset(ASSET_TYPE_BEAM, beam);
			}

			walk.array(def.locationDamageMultipliers, 22, 3);
			walk.array(def.hitLocPriorityMap, 22, 0);
			walk.asset(ASSET_TYPE_RUMBLE, def.fireRumble);
			walk.asset(ASSET_TYPE_RUMBLE, def.meleeImpactRumble);
			for (auto& effect : def.triggerEffects)
			{
				walk.asset(ASSET_TYPE_TRIGGER_EFFECT, effect);
			}

			walk.asset(ASSET_TYPE_TRACER, def.tracerType);
			walk.asset(ASSET_TYPE_TRACER, def.overchargeTracerType);
			walk.asset(ASSET_TYPE_LASER, def.laserTypeViewModel);
			walk.asset(ASSET_TYPE_LASER, def.laserTypeViewModelAlt);
			walk.asset(ASSET_TYPE_LASER, def.laserTypeFriendly);
			walk.asset(ASSET_TYPE_LASER, def.laserTypeEnemy);
			walk_effect(walk, def.turretOverheatEffect);
			walk.asset(ASSET_TYPE_RUMBLE, def.turretBarrelSpinRumble);
			for (const auto gesture : weapon_def_shield_gestures)
			{
				walk.asset(ASSET_TYPE_GESTURE, def.*gesture);
			}

			walk_effect(walk, def.shieldMeleeFx);
			walk.asset(ASSET_TYPE_RUMBLE, def.shieldHitRumble);
			walk.asset(ASSET_TYPE_RUMBLE, def.shieldMissRumble);
			walk.asset(ASSET_TYPE_XMODEL, def.detonatorWorldModel);
			for (const auto gesture : weapon_def_gestures)
			{
				walk.asset(ASSET_TYPE_GESTURE, def.*gesture);
			}

			walk_effect(walk, def.cameraFireEffect);
			walk_effect(walk, def.chargeMeterEffect);
			walk.asset(ASSET_TYPE_RUMBLE, def.chargeRumble);
			walk.script_string(def.stowTag);
			walk.asset(ASSET_TYPE_XMODEL, def.stowOffsetModel);
			walk.script_string(def.stowOffsetAttachTag);
			walk_effect(walk, def.deployEffect);
			walk_overrides(walk, ASSET_TYPE_ANIM_PACKAGE, def.animOverrides, def.numAnimOverrides);
			for (auto& carry : def.carryAnimOverrides)
			{
				walk.asset(ASSET_TYPE_ANIM_PACKAGE, carry.animPackage);
				walk.asset(ASSET_TYPE_ANIM_PACKAGE, carry.animPackageAlt);
			}

			walk_overrides(walk, ASSET_TYPE_SFX_PACKAGE, def.sfxOverrides, def.numSfxOverrides);
			walk_overrides(walk, ASSET_TYPE_VFX_PACKAGE, def.vfxOverrides, def.numVfxOverrides);
			walk.asset(ASSET_TYPE_RUMBLE, def.mountRumble);
			for (auto& curve : def.mountCurves)
			{
				walk.asset(ASSET_TYPE_CURVE, curve);
			}

			walk_pattern_scales(walk, def.mountWeaponOffsetPatternScaleInfo);
			walk_pattern_scales(walk, def.crouchedWeaponOffsetPatternScaleInfo);
			walk_pattern_scales(walk, def.proneWeaponOffsetPatternScaleInfo);
			walk_ballistic(walk, def.ballisticInfo.calculated);
			for (auto& notify : def.notifyTypes)
			{
				walk.array(notify, 1, 3);
			}

			walk.asset(ASSET_TYPE_WEAPON_TRIGGER, def.weaponTrigger);
			walk.asset(ASSET_TYPE_REACTIVE_STAGE_SET, def.reactiveStageSet);
		}

		template <typename Walk>
		void walk_weapon(Walk walk, WeaponCompleteDef& asset)
		{
			if (const auto def = walk.array(asset.weapDef, 1, 7))
			{
				walk_weapon_def(walk, *def);
			}

			walk.string(asset.szDisplayName);
			walk.string(asset.szLootTable);
			walk_script_string_array(walk, asset.hideTags, 32);
			for (auto& list : asset.attachments)
			{
				walk_asset_array(walk, ASSET_TYPE_ATTACHMENT, list.attachments, list.attachmentCount);
			}

			walk_script_string_array(walk, asset.notetrackSoundMapKeys, asset.numNotetrackSoundMappings);
			walk_script_string_array(walk, asset.notetrackSoundMapValues, asset.numNotetrackSoundMappings);
			if (const auto entries = walk.array(asset.notetrackOverrides, asset.numNotetrackOverrides, 7))
			{
				for (auto i = 0u; i < asset.numNotetrackOverrides; i++)
				{
					auto& entry = entries[i];
					walk.script_string(entry.attachment);
					walk_script_string_array(walk, entry.notetrackSoundMapKeys, entry.numSoundMappings);
					walk_script_string_array(walk, entry.notetrackSoundMapValues, entry.numSoundMappings);
				}
			}

			if (const auto entries = walk.array(asset.notetrackSuitEntries, asset.numNotetrackSuitEntries, 7))
			{
				for (auto i = 0u; i < asset.numNotetrackSuitEntries; i++)
				{
					auto& entry = entries[i];
					walk_script_string_array(walk, entry.notetrackSoundMapKeys, entry.numSoundMappings);
					walk_script_string_array(walk, entry.notetrackSoundMapValues, entry.numSoundMappings);
				}
			}

			walk.asset(ASSET_TYPE_MATERIAL, asset.killIconMat);
			walk.asset(ASSET_TYPE_MATERIAL, asset.dpadIconMat);
			walk.asset(ASSET_TYPE_MATERIAL, asset.dpadIconInactiveMat);
			walk.asset(ASSET_TYPE_IMAGE, asset.killIconImg);
			walk.asset(ASSET_TYPE_IMAGE, asset.dpadIconImg);
			walk.asset(ASSET_TYPE_IMAGE, asset.dpadIconInactiveImg);
			walk.asset(ASSET_TYPE_IMAGE, asset.unknownIconImg);
			walk.string(asset.szAltModeInactiveName);
			walk.string(asset.dynamicIconAnimationName);
			walk.string(asset.szLUIWeaponInfoWidgetName);
			walk.string(asset.szLUIWeaponInfoWidgetTag);
			walk.string(asset.szLUICrosshairWidget);
		}

		template <typename Walk>
		void walk_attachment(Walk walk, WeaponAttachment& asset)
		{
			walk.string(asset.szDisplayName);
			walk.script_string(asset.internalName);
			walk.script_string(asset.overrideCategoryName);
			walk.asset(ASSET_TYPE_XMODEL, asset.fallbackWorldModel);
			walk.asset(ASSET_TYPE_XMODEL, asset.fallbackViewModel);
			walk_asset_array(walk, ASSET_TYPE_XMODEL, asset.worldModelVariations, asset.numModelVariations);
			walk_asset_array(walk, ASSET_TYPE_XMODEL, asset.viewModelVariations, asset.numModelVariations);
			walk.script_string(asset.attachPoint);
			walk_script_string_array(walk, asset.hideTags, asset.numHideTags);
			walk_script_string_array(walk, asset.hideTagsNonViewModel, asset.numHideTagsNonViewModel);
			walk_script_string_array(walk, asset.showTags, asset.numShowTags);
			walk_asset_array(walk, ASSET_TYPE_RETICLE, asset.reticles, asset.numReticles);
			walk_asset_array(walk, ASSET_TYPE_RETICLE, asset.altReticles, asset.numAltReticles);
			walk.array(asset.ammogeneral, 1, 3);
			if (const auto ammo_type = walk.array(asset.ammotype, 1, 7))
			{
				walk.string(ammo_type->szAmmoName);
			}

			walk.array(asset.impact, 1, 3);
			walk_effect(walk, asset.unknownEffect);
			walk.string(asset.dynamicAttachmentAnimateSeq);
			walk.string(asset.dynamicAttachmentIcon);
			if (const auto tracer = walk.array(asset.tracer, 1, 7))
			{
				walk.asset(ASSET_TYPE_TRACER, tracer->tracerType);
			}

			walk.array(asset.penetration, 1, 3);
			walk.array(asset.sight, 1, 3);
			walk.array(asset.holdBreath, 1, 0);
			walk.array(asset.reload, 1, 3);
			walk.array(asset.addOns, 1, 0);
			if (const auto general = walk.array(asset.general, 1, 7))
			{
				walk.asset(ASSET_TYPE_MATERIAL, general->reticleCenter);
				walk.asset(ASSET_TYPE_MATERIAL, general->reticleSide);
				walk.asset(ASSET_TYPE_MATERIAL, general->reticleOnePiece);
				walk.string(general->szLUICrosshairWidget);
			}

			if (const auto laser = walk.array(asset.laser, 1, 7))
			{
				walk.asset(ASSET_TYPE_LASER, laser->laserTypeViewModel);
				walk.asset(ASSET_TYPE_LASER, laser->laserTypeViewModelAlt);
				walk.asset(ASSET_TYPE_LASER, laser->laserTypeFriendly);
				walk.asset(ASSET_TYPE_LASER, laser->laserTypeEnemy);
			}

			walk.array(asset.post, 1, 3);
			walk.array(asset.aimAssist, 1, 3);
			walk.array(asset.targetAssist, 1, 3);
			walk.array(asset.ammunition, 1, 3);
			if (const auto damage = walk.array(asset.damage, 1, 7))
			{
				walk.asset(ASSET_TYPE_CURVE, damage->damageInfo.damageCurve);
			}

			walk.array(asset.melee, 1, 3);
			walk.array(asset.idleSettings, 1, 3);
			walk.array(asset.adsSettings, 1, 3);
			walk.array(asset.adsSettingsMain, 1, 3);
			walk.array(asset.adsGunBobSettings, 1, 3);
			walk.array(asset.adsGunBobSettingsMain, 1, 3);
			walk.array(asset.adsReloadSettings, 1, 0);
			walk.array(asset.scopeDriftSettings, 1, 3);
			walk.array(asset.scopeDriftSettingsMain, 1, 3);
			if (const auto scales = walk.array(asset.adsStanceScales, 1, 7))
			{
				walk_pattern_scales(walk, scales->crouchedWeaponOffsetPatternScales);
				walk_pattern_scales(walk, scales->proneWeaponOffsetPatternScales);
			}

			if (const auto scales = walk.array(asset.unknownScales, 1, 7))
			{
				walk_pattern_scales(walk, scales->weaponOffsetPatternScales);
			}

			walk.array(asset.hipSpread, 1, 3);
			walk.array(asset.unknownSpread, 1, 3);
			walk.array(asset.slideSpread, 1, 3);
			walk.array(asset.gunKick, 1, 3);
			walk.array(asset.gunTilt, 1, 3);
			walk.array(asset.kickScaling, 1, 3);
			walk.array(asset.unknownKickScaling, 1, 3);
			walk.array(asset.viewKick, 1, 3);
			if (const auto overlay = walk.array(asset.adsOverlay, 1, 7))
			{
				walk_overlay(walk, overlay->overlay);
				walk.asset(ASSET_TYPE_ATTACHMENT, overlay->hybridScopeAttachment);
			}

			walk.array(asset.outline, 1, 3);
			if (const auto ui = walk.array(asset.ui, 1, 7))
			{
				walk.asset(ASSET_TYPE_IMAGE, ui->hudIcon);
				walk.asset(ASSET_TYPE_IMAGE, ui->pickupIcon);
				walk.asset(ASSET_TYPE_IMAGE, ui->unknownImage0);
				walk.asset(ASSET_TYPE_IMAGE, ui->unknownImage1);
				walk.asset(ASSET_TYPE_IMAGE, ui->unknownImage2);
				walk.asset(ASSET_TYPE_MATERIAL, ui->unknownMaterial0);
				walk.asset(ASSET_TYPE_MATERIAL, ui->unknownMaterial1);
				walk.asset(ASSET_TYPE_IMAGE, ui->dpadIconImg);
				walk.asset(ASSET_TYPE_IMAGE, ui->dpadIconInactiveImg);
				walk.asset(ASSET_TYPE_MATERIAL, ui->dpadIconMat);
				walk.asset(ASSET_TYPE_MATERIAL, ui->dpadIconInactiveMat);
			}

			if (const auto rumbles = walk.array(asset.rumbles, 1, 7))
			{
				walk.asset(ASSET_TYPE_RUMBLE, rumbles->fireRumble);
				walk.asset(ASSET_TYPE_RUMBLE, rumbles->meleeImpactRumble);
			}

			if (const auto projectile = walk.array(asset.projectile, 1, 7))
			{
				walk.asset(ASSET_TYPE_XMODEL, projectile->projectileModel);
				walk_effect(walk, projectile->projExplosionEffect);
				walk_effect(walk, projectile->projDudEffect);
				walk_effect(walk, projectile->projTimeoutEffect);
				walk_effect(walk, projectile->vmProjBodyEffect);
				walk_effect(walk, projectile->projBodyEffect);
				walk_effect(walk, projectile->projTrailEffect);
				walk_effect(walk, projectile->projBeaconEffect);
				walk_effect(walk, projectile->projIgnitionEffect);
			}

			if (const auto charged = walk.array(asset.charged, 1, 7))
			{
				walk_effect(walk, charged->chargeMeterEffect);
				walk.asset(ASSET_TYPE_RUMBLE, charged->chargeRumble);
			}

			walk.array(asset.adsAltSwitch, 1, 3);
			walk.array(asset.regeneration, 1, 3);
			walk.array(asset.heat, 1, 3);
			walk.array(asset.movement, 1, 3);
			walk.array(asset.movementScale, 1, 3);
			walk.array(asset.movementScaleMain, 1, 3);
			walk.array(asset.burst, 1, 3);
			walk.array(asset.hitmarker, 1, 3);
			walk.array(asset.adsMovement, 1, 3);
			if (const auto ballistic = walk.array(asset.ballisticInfo, 1, 7))
			{
				walk_ballistic(walk, ballistic->calculated);
			}

			walk.array(asset.unknownData0, 0xC, 3);
			if (const auto sway = walk.array(asset.swaySettings, 1, 7))
			{
				walk_sway_graphs(walk, *sway);
			}

			if (const auto scales = walk.array(asset.weaponOffsetPatternScaleInfo, 1, 7))
			{
				walk_pattern_scales(walk, *scales);
			}

			walk.array(asset.unknownData1, 0x10, 3);
			walk.array(asset.unknownData2, 8, 3);
			walk.array(asset.locationDamage, 0x16, 3);
			walk.array(asset.unknownLocationDamage, 0x16, 3);
			walk.string(asset.szAltModeName);
			walk.string(asset.szAltModeInactiveName);
			walk.string(asset.szLUIWeaponInfoWidgetName);
			walk.string(asset.szLUIWeaponInfoWidgetTag);
			walk.asset(ASSET_TYPE_IMAGE, asset.altModeIcon);
			walk.array(asset.dlcOverrides, 1, 3);
			walk.array(asset.dlcScalars, 1, 3);
			if (const auto trigger = walk.array(asset.weaponTrigger, 1, 7))
			{
				walk.asset(ASSET_TYPE_WEAPON_TRIGGER, trigger->weaponTrigger);
			}

			walk.string(asset.unknownString0);
			walk.string(asset.unknownString1);
			walk.array(asset.unknownData5, 6, 0);
		}

		template <typename Walk>
		void walk_vfx_package(Walk walk, WeaponVFXPackage& asset)
		{
			if (const auto defs = walk.array(asset.vfx, 1, 7))
			{
				for (const auto effect : vfx_package_effects)
				{
					walk_effect(walk, defs->*effect);
				}
			}
		}
	}

	void weapon::read(zone_reader& reader, WeaponCompleteDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_weapon(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void weapon::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_weapon(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void weapon::dump(WeaponCompleteDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("weapons\\"s + asset->name + ".weapb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_weapon(dump_walk{dump}, *asset);
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
		asset->name = read.read_string();
		walk_weapon(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void weapon::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponCompleteDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void weapon::prepare(zone_buffer* buf, zone_memory*)
	{
		if (!this->referenced())
		{
			walk_weapon(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	void weapon::load_depending(zone_base* zone)
	{
		if (!this->referenced())
		{
			walk_weapon(depend_walk{zone}, *this->asset_);
		}
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
		reader.read_string(asset->name);
		walk_attachment(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void weapon_attachment::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_attachment(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("attachments\\"s + asset->name + ".attb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_attachment(dump_walk{dump}, *asset);
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

		auto* asset = read.read_single<WeaponAttachment>();
		asset->name = read.read_string();
		walk_attachment(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void weapon_attachment::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponAttachment>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void weapon_attachment::prepare(zone_buffer* buf, zone_memory*)
	{
		if (!this->referenced())
		{
			walk_attachment(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	void weapon_attachment::load_depending(zone_base* zone)
	{
		if (!this->referenced())
		{
			walk_attachment(depend_walk{zone}, *this->asset_);
		}
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
		walk_vfx_package(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void vfx_package::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_vfx_package(write_walk{zone, buf}, *dest);
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
		walk_vfx_package(dump_walk{dump}, *asset);
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
		walk_vfx_package(parse_walk{read}, *asset);
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
		if (!this->referenced())
		{
			walk_vfx_package(depend_walk{zone}, *this->asset_);
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
