#include <std_include.hpp>
#include "weaponattachment.hpp"

namespace zonetool::iw6
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define PARSE_FIELD_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			asset->__field__[idx##__field__] = data[#__field__][idx##__field__].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__])>::type>(); \
		} \
	}

#define PARSE_FIELD_RENAME(__field__, __name__) \
	if (!data[__name__].is_null()) asset->__field__ = data[__name__].get<decltype(asset->__field__)>();

#define PARSE_ASSET_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__ = mem->allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(__size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (!data[#__field__][idx##__field__].is_null() && !data[#__field__][idx##__field__].get<std::string>().empty()) \
			{ \
				asset->__field__[idx##__field__] = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__[idx##__field__])>::type>(sizeof(const char*)); \
				asset->__field__[idx##__field__]->name = mem->duplicate_string(data[#__field__][idx##__field__].get<std::string>()); \
			} \
			else \
			{ \
				asset->__field__[idx##__field__] = nullptr; \
			} \
		} \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}

#define PARSE_ASSET(__field__) \
	if (!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty()) \
	{ \
		asset->__field__ = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(sizeof(const char*)); \
		asset->__field__->name = mem->duplicate_string(data[#__field__].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}

	void parse_att(AttAmmoGeneral* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(penetrateType);
		PARSE_FIELD(penetrateMultiplier);
		PARSE_FIELD(impactType);
		PARSE_FIELD(fireType);
		PARSE_ASSET(tracerType);
		PARSE_FIELD(rifleBullet);
		PARSE_FIELD(armorPiercing);
	}

	void parse_att(AttSight* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(aimDownSight);
		PARSE_FIELD(adsFire);
		PARSE_FIELD(rechamberWhileAds);
		PARSE_FIELD(noAdsWhenMagEmpty);
		PARSE_FIELD(canHoldBreath);
		PARSE_FIELD(canVariableZoom);
		PARSE_FIELD(hideRailWithThisScope);
		PARSE_FIELD(useScopeDrift);
		PARSE_FIELD(useDualFOV);
	}

	void parse_att(AttReload* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(noPartialReload);
		PARSE_FIELD(segmentedReload);
	}

	void parse_att(AttAddOns* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(motionTracker);
		PARSE_FIELD(silenced);
		PARSE_FIELD(riotShield);
	}

	void parse_att(AttGeneral* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(boltAction);
		PARSE_FIELD(inheritsPerks);
		PARSE_FIELD(reticleSpin45);
		PARSE_FIELD(enemyCrosshairRange);
		PARSE_ASSET(reticleCenter);
		PARSE_ASSET(reticleSide);
		PARSE_FIELD(reticleCenterSize);
		PARSE_FIELD(reticleSideSize);
		PARSE_FIELD(moveSpeedScale);
		PARSE_FIELD(adsMoveSpeedScale);
	}

	void parse_att(AttAimAssist* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(autoAimRange);
		PARSE_FIELD(aimAssistRange);
		PARSE_FIELD(aimAssistRangeAds);
	}

	void parse_att(AttAmmunition* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(maxAmmo);
		PARSE_FIELD(startAmmo);
		PARSE_FIELD(clipSize);
		PARSE_FIELD(shotCount);
		PARSE_FIELD(reloadAmmoAdd);
		PARSE_FIELD(reloadStartAdd);
	}

	void parse_att(AttDamage* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(damage);
		PARSE_FIELD(minDamage);
		PARSE_FIELD(meleeDamage);
		PARSE_FIELD(maxDamageRange);
		PARSE_FIELD(minDamageRange);
		PARSE_FIELD(playerDamage);
		PARSE_FIELD(minPlayerDamage);
	}

	void parse_att(AttIdleSettings* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(hipIdleAmount);
		PARSE_FIELD(hipIdleSpeed);
		PARSE_FIELD(idleCrouchFactor);
		PARSE_FIELD(idleProneFactor);
		PARSE_FIELD(adsIdleLerpStartTime);
		PARSE_FIELD(adsIdleLerpTime);
	}

	void parse_att(AttADSSettings* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(adsSpread);
		PARSE_FIELD(adsAimPitch);
		PARSE_FIELD(adsTransInTime);
		PARSE_FIELD(adsTransOutTime);
		PARSE_FIELD(adsReloadTransTime);
		PARSE_FIELD(adsCrosshairInFrac);
		PARSE_FIELD(adsCrosshairOutFrac);
		PARSE_FIELD(adsZoomFov);
		PARSE_FIELD(adsZoomInFrac);
		PARSE_FIELD(adsZoomOutFrac);
		PARSE_FIELD(adsFovLerpTime);
		PARSE_FIELD(adsBobFactor);
		PARSE_FIELD(adsViewBobMult);
		PARSE_FIELD(adsFireRateScale);
		PARSE_FIELD(adsDamageRangeScale);
		PARSE_FIELD(adsFireAnimFrac);
	}

	void parse_att(AttScopeDriftSettings* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(fScopeDriftDelay);
		PARSE_FIELD(fScopeDriftLerpInTime);
		PARSE_FIELD(fScopeDriftSteadyTime);
		PARSE_FIELD(fScopeDriftLerpOutTime);
		PARSE_FIELD(fScopeDriftSteadyFactor);
		PARSE_FIELD(fScopeDriftUnsteadyFactor);
	}

	void parse_att(AttHipSpread* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(hipSpreadStandMin);
		PARSE_FIELD(hipSpreadDuckedMin);
		PARSE_FIELD(hipSpreadProneMin);
		PARSE_FIELD(hipSpreadMax);
		PARSE_FIELD(hipSpreadDuckedMax);
		PARSE_FIELD(hipSpreadProneMax);
		PARSE_FIELD(hipSpreadFireAdd);
		PARSE_FIELD(hipSpreadTurnAdd);
		PARSE_FIELD(hipSpreadMoveAdd);
		PARSE_FIELD(hipSpreadDecayRate);
		PARSE_FIELD(hipSpreadDuckedDecay);
		PARSE_FIELD(hipSpreadProneDecay);
	}

	void parse_att(AttGunKick* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(hipGunKickReducedKickBullets);
		PARSE_FIELD(hipGunKickReducedKickPercent);
		PARSE_FIELD(hipGunKickPitchMin);
		PARSE_FIELD(hipGunKickPitchMax);
		PARSE_FIELD(hipGunKickYawMin);
		PARSE_FIELD(hipGunKickYawMax);
		PARSE_FIELD(hipGunKickMagMin);
		PARSE_FIELD(hipGunKickAccel);
		PARSE_FIELD(hipGunKickSpeedMax);
		PARSE_FIELD(hipGunKickSpeedDecay);
		PARSE_FIELD(hipGunKickStaticDecay);
		PARSE_FIELD(adsGunKickReducedKickBullets);
		PARSE_FIELD(adsGunKickReducedKickPercent);
		PARSE_FIELD(adsGunKickPitchMin);
		PARSE_FIELD(adsGunKickPitchMax);
		PARSE_FIELD(adsGunKickYawMin);
		PARSE_FIELD(adsGunKickYawMax);
		PARSE_FIELD(adsGunKickMagMin);
		PARSE_FIELD(adsGunKickAccel);
		PARSE_FIELD(adsGunKickSpeedMax);
		PARSE_FIELD(adsGunKickSpeedDecay);
		PARSE_FIELD(adsGunKickStaticDecay);
	}

	void parse_att(AttViewKick* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(hipViewKickPitchMin);
		PARSE_FIELD(hipViewKickPitchMax);
		PARSE_FIELD(hipViewKickYawMin);
		PARSE_FIELD(hipViewKickYawMax);
		PARSE_FIELD(hipViewKickMagMin);
		PARSE_FIELD(hipViewKickCenterSpeed);
		PARSE_FIELD(adsViewKickPitchMin);
		PARSE_FIELD(adsViewKickPitchMax);
		PARSE_FIELD(adsViewKickYawMin);
		PARSE_FIELD(adsViewKickYawMax);
		PARSE_FIELD(adsViewKickMagMin);
		PARSE_FIELD(adsViewKickCenterSpeed);
	}

	void parse_att(AttADSOverlay* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_ASSET(overlay.shader);
		PARSE_ASSET(overlay.shaderLowRes);
		PARSE_ASSET(overlay.shaderEMP);
		PARSE_ASSET(overlay.shaderEMPLowRes);
		PARSE_FIELD(overlay.reticle);
		PARSE_FIELD(overlay.width);
		PARSE_FIELD(overlay.height);
		PARSE_FIELD(overlay.widthSplitscreen);
		PARSE_FIELD(overlay.heightSplitscreen);
		PARSE_FIELD(hybridToggle);
		PARSE_FIELD(thermalScope);
		PARSE_FIELD(thermalToggle);
		PARSE_FIELD(outlineEnemies);
	}

	void parse_att(AttUI* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_ASSET(dpadIcon);
		PARSE_ASSET(ammoCounterIcon);
		PARSE_FIELD(dpadIconRatio);
		PARSE_FIELD(ammoCounterIconRatio);
		PARSE_FIELD(ammoCounterClip);
	}

	void parse_att(AttRumbles* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_STRING(fireRumble);
		PARSE_STRING(meleeImpactRumble);
	}

	void parse_att(AttProjectile* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(explosionRadius);
		PARSE_FIELD(explosionInnerDamage);
		PARSE_FIELD(explosionOuterDamage);
		PARSE_FIELD(damageConeAngle);
		PARSE_FIELD(projectileSpeed);
		PARSE_FIELD(projectileSpeedUp);
		PARSE_FIELD(projectileActivateDist);
		PARSE_FIELD(projectileLifetime);
		PARSE_ASSET(projectileModel);
		PARSE_FIELD(projExplosionType);
		PARSE_ASSET(projExplosionEffect);
		PARSE_FIELD(projExplosionEffectForceNormalUp);
		PARSE_ASSET(projExplosionSound);
		PARSE_ASSET(projDudEffect);
		PARSE_ASSET(projDudSound);
		PARSE_FIELD(projImpactExplode);
		PARSE_FIELD(destabilizationRateTime);
		PARSE_FIELD(destabilizationCurvatureMax);
		PARSE_FIELD(destabilizeDistance);
		PARSE_ASSET(projTrailEffect);
		PARSE_FIELD(projIgnitionDelay);
		PARSE_ASSET(projIgnitionEffect);
		PARSE_ASSET(projIgnitionSound);
	}

#define PARSE_ATT(__att__) \
	if (!data[#__att__].is_null()) \
	{ \
		asset->__att__ = mem->allocate<typename std::remove_reference<decltype(*asset->__att__)>::type>(); \
		parse_att(asset->__att__, data[#__att__], mem);  \
	} \
	else \
	{ \
		 asset->__att__ = nullptr; \
	}

	WeaponAttachment* weapon_attachment::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "attachments\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing attachment \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		ordered_json data = json::parse(bytes);

		auto* asset = mem->allocate<WeaponAttachment>();

		PARSE_STRING(szInternalName);
		PARSE_STRING(szDisplayName);
		PARSE_FIELD(type);
		PARSE_FIELD(weaponType);
		PARSE_FIELD_RENAME(weapClass, "weaponClass");
		PARSE_ASSET_ARR(worldModels, 64);
		PARSE_ASSET_ARR(viewModels, 64);
		PARSE_ASSET_ARR(reticleViewModels, 16);
		PARSE_ATT(ammogeneral);
		PARSE_ATT(sight);
		PARSE_ATT(reload);
		PARSE_ATT(addOns);
		PARSE_ATT(general);
		PARSE_ATT(aimAssist);
		PARSE_ATT(ammunition);
		PARSE_ATT(damage);
		PARSE_ATT(idleSettings);
		PARSE_ATT(adsSettings);
		PARSE_ATT(adsSettingsMain);
		PARSE_ATT(scopeDriftSettings);
		PARSE_ATT(scopeDriftSettingsMain);
		PARSE_ATT(hipSpread);
		PARSE_ATT(gunKick);
		PARSE_ATT(viewKick);
		PARSE_ATT(adsOverlay);
		PARSE_ATT(ui);
		PARSE_ATT(rumbles);
		PARSE_ATT(projectile);
		PARSE_FIELD(ammunitionScale);
		PARSE_FIELD(damageScale);
		PARSE_FIELD(damageScaleMin);
		PARSE_FIELD(stateTimersScale);
		PARSE_FIELD(fireTimersScale);
		PARSE_FIELD(adsSettingsScale);
		PARSE_FIELD(adsSettingsScaleMain);
		PARSE_FIELD(hipSpreadScale);
		PARSE_FIELD(gunKickScale);
		PARSE_FIELD(viewKickScale);
		PARSE_FIELD(viewCenterScale);
		PARSE_FIELD(loadIndex); // probably runtime data
		PARSE_FIELD(hideIronSightsWithThisAttachment);
		PARSE_FIELD(shareAmmoWithAlt);

		return asset;
	}

	void weapon_attachment::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
		this->asset_ = this->parse(name, mem);

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
		}

		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).attachment;
		}
	}

	void weapon_attachment::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void weapon_attachment::load_depending(zone_base* zone)
	{
		auto data = this->asset_;

		for (auto i = 0; i < 64; i++)
		{
			if (data->worldModels && data->worldModels[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->worldModels[i]->name);
			}
		}

		for (auto i = 0; i < 64; i++)
		{
			if (data->viewModels && data->viewModels[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->viewModels[i]->name);
			}
		}

		for (auto i = 0; i < 16; i++)
		{
			if (data->reticleViewModels && data->reticleViewModels[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->reticleViewModels[i]->name);
			}
		}

		if (data->ammogeneral)
		{
			if (data->ammogeneral->tracerType)
			{
				zone->add_asset_of_type(ASSET_TYPE_TRACER, data->ammogeneral->tracerType->name);
			}
		}

		if (data->general)
		{
			if (data->general->reticleCenter)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->general->reticleCenter->name);
			}

			if (data->general->reticleSide)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->general->reticleSide->name);
			}
		}

		if (data->adsOverlay)
		{
			if (data->adsOverlay->overlay.shader)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->adsOverlay->overlay.shader->name);
			}

			if (data->adsOverlay->overlay.shaderLowRes)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->adsOverlay->overlay.shaderLowRes->name);
			}

			if (data->adsOverlay->overlay.shaderEMP)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->adsOverlay->overlay.shaderEMP->name);
			}

			if (data->adsOverlay->overlay.shaderEMPLowRes)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->adsOverlay->overlay.shaderEMPLowRes->name);
			}
		}

		if (data->ui)
		{
			if (data->ui->ammoCounterIcon)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->ui->ammoCounterIcon->name);
			}

			if (data->ui->dpadIcon)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->ui->dpadIcon->name);
			}
		}

		if (data->projectile)
		{
			if (data->projectile->projectileModel)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->projectile->projectileModel->name);
			}

			if (data->projectile->projExplosionEffect)
			{
				zone->add_asset_of_type(ASSET_TYPE_FX, data->projectile->projExplosionEffect->name);
			}

			if (data->projectile->projExplosionSound)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND, data->projectile->projExplosionSound->aliasName);
			}

			if (data->projectile->projDudEffect)
			{
				zone->add_asset_of_type(ASSET_TYPE_FX, data->projectile->projDudEffect->name);
			}

			if (data->projectile->projDudSound)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND, data->projectile->projDudSound->aliasName);
			}

			if (data->projectile->projTrailEffect)
			{
				zone->add_asset_of_type(ASSET_TYPE_FX, data->projectile->projTrailEffect->name);
			}

			if (data->projectile->projIgnitionEffect)
			{
				zone->add_asset_of_type(ASSET_TYPE_FX, data->projectile->projIgnitionEffect->name);
			}

			if (data->projectile->projIgnitionSound)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND, data->projectile->projIgnitionSound->aliasName);
			}
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

#define WRITE_SOUND_CUSTOM(__field__) \
	if (data->__field__) \
	{ \
		buf->align(7); \
		buf->write(&buf->data_following); \
		buf->write_str_raw(data->__field__->name); \
		buf->clear_pointer(&dest->__field__); \
	}

#define WRITE_ASSET(__field__,__type__) \
	if (data->__field__) \
	{ \
		dest->__field__ = reinterpret_cast<typename std::remove_reference<decltype(data->__field__)>::type>(zone->get_asset_pointer(__type__, data->__field__->name)); \
	}

#define WRITE_STRING(__field__) \
	if (data->__field__) \
	{ \
		dest->__field__ = buf->write_str(data->__field__); \
	}

	void weapon_attachment::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->szInternalName = buf->write_str(this->name());

		WRITE_STRING(szDisplayName);

		if (data->worldModels)
		{
			buf->align(7);
			dest->worldModels = buf->write(data->worldModels, 64);

			for (auto i = 0; i < 64; i++)
			{
				WRITE_ASSET(worldModels[i], ASSET_TYPE_XMODEL);
			}

			buf->clear_pointer(&dest->worldModels);
		}

		if (data->viewModels)
		{
			buf->align(7);
			dest->viewModels = buf->write(data->viewModels, 64);

			for (auto i = 0; i < 64; i++)
			{
				WRITE_ASSET(viewModels[i], ASSET_TYPE_XMODEL);
			}

			buf->clear_pointer(&dest->viewModels);
		}

		if (data->reticleViewModels)
		{
			buf->align(7);
			dest->reticleViewModels = buf->write(data->reticleViewModels, 16);

			for (auto i = 0; i < 16; i++)
			{
				WRITE_ASSET(reticleViewModels[i], ASSET_TYPE_XMODEL);
			}

			buf->clear_pointer(&dest->reticleViewModels);
		}

		if (data->ammogeneral)
		{
			buf->align(3);
			dest->ammogeneral = buf->write(data->ammogeneral);

			WRITE_ASSET(ammogeneral->tracerType, ASSET_TYPE_TRACER);

			buf->clear_pointer(&dest->ammogeneral);
		}

		if (data->sight)
		{
			buf->align(3);
			buf->write(data->sight);
			buf->clear_pointer(&dest->sight);
		}

		if (data->reload)
		{
			buf->align(1);
			buf->write(data->reload);
			buf->clear_pointer(&dest->reload);
		}

		if (data->addOns)
		{
			buf->align(0);
			buf->write(data->addOns);
			buf->clear_pointer(&dest->addOns);
		}

		if (data->general)
		{
			buf->align(3);
			dest->general = buf->write(data->general);

			WRITE_ASSET(general->reticleCenter, ASSET_TYPE_MATERIAL);
			WRITE_ASSET(general->reticleSide, ASSET_TYPE_MATERIAL);

			buf->clear_pointer(&dest->general);
		}

		if (data->aimAssist)
		{
			buf->align(3);
			buf->write(data->aimAssist);
			buf->clear_pointer(&dest->aimAssist);
		}

		if (data->ammunition)
		{
			buf->align(3);
			buf->write(data->ammunition);
			buf->clear_pointer(&dest->ammunition);
		}

		if (data->damage)
		{
			buf->align(3);
			buf->write(data->damage);
			buf->clear_pointer(&dest->damage);
		}

		if (data->locationDamage)
		{
			buf->align(3);
			buf->write(data->locationDamage);
			buf->clear_pointer(&dest->locationDamage);
		}

		if (data->idleSettings)
		{
			buf->align(3);
			buf->write(data->idleSettings);
			buf->clear_pointer(&dest->idleSettings);
		}

		if (data->adsSettings)
		{
			buf->align(3);
			buf->write(data->adsSettings);
			buf->clear_pointer(&dest->adsSettings);
		}

		if (data->adsSettingsMain)
		{
			buf->align(3);
			buf->write(data->adsSettingsMain);
			buf->clear_pointer(&dest->adsSettingsMain);
		}

		if (data->scopeDriftSettings)
		{
			buf->align(3);
			buf->write(data->scopeDriftSettings);
			buf->clear_pointer(&dest->scopeDriftSettings);
		}

		if (data->scopeDriftSettingsMain)
		{
			buf->align(3);
			buf->write(data->scopeDriftSettingsMain);
			buf->clear_pointer(&dest->scopeDriftSettingsMain);
		}

		if (data->hipSpread)
		{
			buf->align(3);
			buf->write(data->hipSpread);
			buf->clear_pointer(&dest->hipSpread);
		}

		if (data->gunKick)
		{
			buf->align(3);
			buf->write(data->gunKick);
			buf->clear_pointer(&dest->gunKick);
		}

		if (data->viewKick)
		{
			buf->align(3);
			buf->write(data->viewKick);
			buf->clear_pointer(&dest->viewKick);
		}

		if (data->adsOverlay)
		{
			buf->align(3);
			dest->adsOverlay = buf->write(data->adsOverlay);

			WRITE_ASSET(adsOverlay->overlay.shader, ASSET_TYPE_MATERIAL);
			WRITE_ASSET(adsOverlay->overlay.shaderLowRes, ASSET_TYPE_MATERIAL);
			WRITE_ASSET(adsOverlay->overlay.shaderEMP, ASSET_TYPE_MATERIAL);
			WRITE_ASSET(adsOverlay->overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL);

			buf->clear_pointer(&dest->adsOverlay);
		}

		if (data->ui)
		{
			buf->align(3);
			dest->ui = buf->write(data->ui);

			WRITE_ASSET(ui->dpadIcon, ASSET_TYPE_MATERIAL);
			WRITE_ASSET(ui->ammoCounterIcon, ASSET_TYPE_MATERIAL);

			buf->clear_pointer(&dest->ui);
		}

		if (data->rumbles)
		{
			buf->align(3);
			dest->rumbles = buf->write(data->rumbles);

			WRITE_STRING(rumbles->fireRumble);
			WRITE_STRING(rumbles->meleeImpactRumble);

			buf->clear_pointer(&dest->rumbles);
		}

		if (data->projectile)
		{
			buf->align(3);
			dest->projectile = buf->write(data->projectile);

			WRITE_ASSET(projectile->projectileModel, ASSET_TYPE_XMODEL);
			WRITE_ASSET(projectile->projExplosionEffect, ASSET_TYPE_FX);
			WRITE_SOUND_CUSTOM(projectile->projExplosionSound);
			WRITE_ASSET(projectile->projDudEffect, ASSET_TYPE_FX);
			WRITE_SOUND_CUSTOM(projectile->projDudSound);
			WRITE_ASSET(projectile->projTrailEffect, ASSET_TYPE_FX);
			WRITE_ASSET(projectile->projIgnitionEffect, ASSET_TYPE_FX);
			WRITE_SOUND_CUSTOM(projectile->projIgnitionSound);

			buf->clear_pointer(&dest->projectile);
		}

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = nullptr;

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

#define DUMP_FIELD_RENAME(__field__, __name__) \
	data[__name__] = asset->__field__;

#define DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__ && __size__) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
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

#define DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

	void dump_att(AttAmmoGeneral* asset, ordered_json& data)
	{
		DUMP_FIELD(penetrateType);
		DUMP_FIELD(penetrateMultiplier);
		DUMP_FIELD(impactType);
		DUMP_FIELD(fireType);
		DUMP_ASSET(tracerType);
		DUMP_FIELD(rifleBullet);
		DUMP_FIELD(armorPiercing);
	}

	void dump_att(AttSight* asset, ordered_json& data)
	{
		DUMP_FIELD(aimDownSight);
		DUMP_FIELD(adsFire);
		DUMP_FIELD(rechamberWhileAds);
		DUMP_FIELD(noAdsWhenMagEmpty);
		DUMP_FIELD(canHoldBreath);
		DUMP_FIELD(canVariableZoom);
		DUMP_FIELD(hideRailWithThisScope);
		DUMP_FIELD(useScopeDrift);
		DUMP_FIELD(useDualFOV);
	}

	void dump_att(AttReload* asset, ordered_json& data)
	{
		DUMP_FIELD(noPartialReload);
		DUMP_FIELD(segmentedReload);
	}

	void dump_att(AttAddOns* asset, ordered_json& data)
	{
		DUMP_FIELD(motionTracker);
		DUMP_FIELD(silenced);
		DUMP_FIELD(riotShield);
	}

	void dump_att(AttGeneral* asset, ordered_json& data)
	{
		DUMP_FIELD(boltAction);
		DUMP_FIELD(inheritsPerks);
		DUMP_FIELD(reticleSpin45);
		DUMP_FIELD(enemyCrosshairRange);
		DUMP_ASSET(reticleCenter);
		DUMP_ASSET(reticleSide);
		DUMP_FIELD(reticleCenterSize);
		DUMP_FIELD(reticleSideSize);
		DUMP_FIELD(moveSpeedScale);
		DUMP_FIELD(adsMoveSpeedScale);
	}

	void dump_att(AttAimAssist* asset, ordered_json& data)
	{
		DUMP_FIELD(autoAimRange);
		DUMP_FIELD(aimAssistRange);
		DUMP_FIELD(aimAssistRangeAds);
	}

	void dump_att(AttAmmunition* asset, ordered_json& data)
	{
		DUMP_FIELD(maxAmmo);
		DUMP_FIELD(startAmmo);
		DUMP_FIELD(clipSize);
		DUMP_FIELD(shotCount);
		DUMP_FIELD(reloadAmmoAdd);
		DUMP_FIELD(reloadStartAdd);
	}

	void dump_att(AttDamage* asset, ordered_json& data)
	{
		DUMP_FIELD(damage);
		DUMP_FIELD(minDamage);
		DUMP_FIELD(meleeDamage);
		DUMP_FIELD(maxDamageRange);
		DUMP_FIELD(minDamageRange);
		DUMP_FIELD(playerDamage);
		DUMP_FIELD(minPlayerDamage);
	}

	void dump_att(AttIdleSettings* asset, ordered_json& data)
	{
		DUMP_FIELD(hipIdleAmount);
		DUMP_FIELD(hipIdleSpeed);
		DUMP_FIELD(idleCrouchFactor);
		DUMP_FIELD(idleProneFactor);
		DUMP_FIELD(adsIdleLerpStartTime);
		DUMP_FIELD(adsIdleLerpTime);
	}

	void dump_att(AttADSSettings* asset, ordered_json& data)
	{
		DUMP_FIELD(adsSpread);
		DUMP_FIELD(adsAimPitch);
		DUMP_FIELD(adsTransInTime);
		DUMP_FIELD(adsTransOutTime);
		DUMP_FIELD(adsReloadTransTime);
		DUMP_FIELD(adsCrosshairInFrac);
		DUMP_FIELD(adsCrosshairOutFrac);
		DUMP_FIELD(adsZoomFov);
		DUMP_FIELD(adsZoomInFrac);
		DUMP_FIELD(adsZoomOutFrac);
		DUMP_FIELD(adsFovLerpTime);
		DUMP_FIELD(adsBobFactor);
		DUMP_FIELD(adsViewBobMult);
		DUMP_FIELD(adsFireRateScale);
		DUMP_FIELD(adsDamageRangeScale);
		DUMP_FIELD(adsFireAnimFrac);
	}

	void dump_att(AttScopeDriftSettings* asset, ordered_json& data)
	{
		DUMP_FIELD(fScopeDriftDelay);
		DUMP_FIELD(fScopeDriftLerpInTime);
		DUMP_FIELD(fScopeDriftSteadyTime);
		DUMP_FIELD(fScopeDriftLerpOutTime);
		DUMP_FIELD(fScopeDriftSteadyFactor);
		DUMP_FIELD(fScopeDriftUnsteadyFactor);
	}

	void dump_att(AttHipSpread* asset, ordered_json& data)
	{
		DUMP_FIELD(hipSpreadStandMin);
		DUMP_FIELD(hipSpreadDuckedMin);
		DUMP_FIELD(hipSpreadProneMin);
		DUMP_FIELD(hipSpreadMax);
		DUMP_FIELD(hipSpreadDuckedMax);
		DUMP_FIELD(hipSpreadProneMax);
		DUMP_FIELD(hipSpreadFireAdd);
		DUMP_FIELD(hipSpreadTurnAdd);
		DUMP_FIELD(hipSpreadMoveAdd);
		DUMP_FIELD(hipSpreadDecayRate);
		DUMP_FIELD(hipSpreadDuckedDecay);
		DUMP_FIELD(hipSpreadProneDecay);
	}

	void dump_att(AttGunKick* asset, ordered_json& data)
	{
		DUMP_FIELD(hipGunKickReducedKickBullets);
		DUMP_FIELD(hipGunKickReducedKickPercent);
		DUMP_FIELD(hipGunKickPitchMin);
		DUMP_FIELD(hipGunKickPitchMax);
		DUMP_FIELD(hipGunKickYawMin);
		DUMP_FIELD(hipGunKickYawMax);
		DUMP_FIELD(hipGunKickMagMin);
		DUMP_FIELD(hipGunKickAccel);
		DUMP_FIELD(hipGunKickSpeedMax);
		DUMP_FIELD(hipGunKickSpeedDecay);
		DUMP_FIELD(hipGunKickStaticDecay);
		DUMP_FIELD(adsGunKickReducedKickBullets);
		DUMP_FIELD(adsGunKickReducedKickPercent);
		DUMP_FIELD(adsGunKickPitchMin);
		DUMP_FIELD(adsGunKickPitchMax);
		DUMP_FIELD(adsGunKickYawMin);
		DUMP_FIELD(adsGunKickYawMax);
		DUMP_FIELD(adsGunKickMagMin);
		DUMP_FIELD(adsGunKickAccel);
		DUMP_FIELD(adsGunKickSpeedMax);
		DUMP_FIELD(adsGunKickSpeedDecay);
		DUMP_FIELD(adsGunKickStaticDecay);
	}

	void dump_att(AttViewKick* asset, ordered_json& data)
	{
		DUMP_FIELD(hipViewKickPitchMin);
		DUMP_FIELD(hipViewKickPitchMax);
		DUMP_FIELD(hipViewKickYawMin);
		DUMP_FIELD(hipViewKickYawMax);
		DUMP_FIELD(hipViewKickMagMin);
		DUMP_FIELD(hipViewKickCenterSpeed);
		DUMP_FIELD(adsViewKickPitchMin);
		DUMP_FIELD(adsViewKickPitchMax);
		DUMP_FIELD(adsViewKickYawMin);
		DUMP_FIELD(adsViewKickYawMax);
		DUMP_FIELD(adsViewKickMagMin);
		DUMP_FIELD(adsViewKickCenterSpeed);
	}

	void dump_att(AttADSOverlay* asset, ordered_json& data)
	{
		DUMP_ASSET(overlay.shader);
		DUMP_ASSET(overlay.shaderLowRes);
		DUMP_ASSET(overlay.shaderEMP);
		DUMP_ASSET(overlay.shaderEMPLowRes);
		DUMP_FIELD(overlay.reticle);
		DUMP_FIELD(overlay.width);
		DUMP_FIELD(overlay.height);
		DUMP_FIELD(overlay.widthSplitscreen);
		DUMP_FIELD(overlay.heightSplitscreen);
		DUMP_FIELD(hybridToggle);
		DUMP_FIELD(thermalScope);
		DUMP_FIELD(thermalToggle);
		DUMP_FIELD(outlineEnemies);
	}

	void dump_att(AttUI* asset, ordered_json& data)
	{
		DUMP_ASSET(dpadIcon);
		DUMP_ASSET(ammoCounterIcon);
		DUMP_FIELD(dpadIconRatio);
		DUMP_FIELD(ammoCounterIconRatio);
		DUMP_FIELD(ammoCounterClip);
	}

	void dump_att(AttRumbles* asset, ordered_json& data)
	{
		DUMP_STRING(fireRumble);
		DUMP_STRING(meleeImpactRumble);
	}

	void dump_att(AttProjectile* asset, ordered_json& data)
	{
		DUMP_FIELD(explosionRadius);
		DUMP_FIELD(explosionInnerDamage);
		DUMP_FIELD(explosionOuterDamage);
		DUMP_FIELD(damageConeAngle);
		DUMP_FIELD(projectileSpeed);
		DUMP_FIELD(projectileSpeedUp);
		DUMP_FIELD(projectileActivateDist);
		DUMP_FIELD(projectileLifetime);
		DUMP_ASSET(projectileModel);
		DUMP_FIELD(projExplosionType);
		DUMP_ASSET(projExplosionEffect);
		DUMP_FIELD(projExplosionEffectForceNormalUp);
		DUMP_ASSET(projExplosionSound);
		DUMP_ASSET(projDudEffect);
		DUMP_ASSET(projDudSound);
		DUMP_FIELD(projImpactExplode);
		DUMP_FIELD(destabilizationRateTime);
		DUMP_FIELD(destabilizationCurvatureMax);
		DUMP_FIELD(destabilizeDistance);
		DUMP_ASSET(projTrailEffect);
		DUMP_FIELD(projIgnitionDelay);
		DUMP_ASSET(projIgnitionEffect);
		DUMP_ASSET(projIgnitionSound);
	}

#define DUMP_ATT(__att__) \
	if (asset->__att__) dump_att(asset->__att__, data[#__att__]); else data[#__att__] = nullptr;

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		const auto path = "attachments\\"s + asset->name + ".json"s;

		ordered_json data;

		DUMP_STRING(szInternalName);
		DUMP_STRING(szDisplayName);
		DUMP_FIELD(type);
		DUMP_FIELD(weaponType);
		DUMP_FIELD_RENAME(weapClass, "weaponClass");
		DUMP_ASSET_ARR(worldModels, 64);
		DUMP_ASSET_ARR(viewModels, 64);
		DUMP_ASSET_ARR(reticleViewModels, 16);
		DUMP_ATT(ammogeneral);
		DUMP_ATT(sight);
		DUMP_ATT(reload);
		DUMP_ATT(addOns);
		DUMP_ATT(general);
		DUMP_ATT(aimAssist);
		DUMP_ATT(ammunition);
		DUMP_ATT(damage);
		DUMP_ATT(idleSettings);
		DUMP_ATT(adsSettings);
		DUMP_ATT(adsSettingsMain);
		DUMP_ATT(scopeDriftSettings);
		DUMP_ATT(scopeDriftSettingsMain);
		DUMP_ATT(hipSpread);
		DUMP_ATT(gunKick);
		DUMP_ATT(viewKick);
		DUMP_ATT(adsOverlay);
		DUMP_ATT(ui);
		DUMP_ATT(rumbles);
		DUMP_ATT(projectile);
		DUMP_FIELD(ammunitionScale);
		DUMP_FIELD(damageScale);
		DUMP_FIELD(damageScaleMin);
		DUMP_FIELD(stateTimersScale);
		DUMP_FIELD(fireTimersScale);
		DUMP_FIELD(adsSettingsScale);
		DUMP_FIELD(adsSettingsScaleMain);
		DUMP_FIELD(hipSpreadScale);
		DUMP_FIELD(gunKickScale);
		DUMP_FIELD(viewKickScale);
		DUMP_FIELD(viewCenterScale);
		DUMP_FIELD(loadIndex); // probably runtime data
		DUMP_FIELD(hideIronSightsWithThisAttachment);
		DUMP_FIELD(shareAmmoWithAlt);

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}