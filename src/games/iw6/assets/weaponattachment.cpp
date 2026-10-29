#include <std_include.hpp>
#include "weaponattachment.hpp"
#include "weapondef.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::iw6
{
	namespace
	{
		constexpr auto ads_overlay_fields = glz::tuple
		{
			&AttADSOverlay::hybridToggle,
			&AttADSOverlay::thermalScope,
			&AttADSOverlay::thermalToggle,
			&AttADSOverlay::outlineEnemies,
		};

		constexpr auto rumbles_fields = glz::tuple
		{
			&AttRumbles::fireRumble,
			&AttRumbles::meleeImpactRumble,
		};

		constexpr auto projectile_fields = glz::tuple
		{
			&AttProjectile::explosionRadius,
			&AttProjectile::explosionInnerDamage,
			&AttProjectile::explosionOuterDamage,
			&AttProjectile::damageConeAngle,
			&AttProjectile::projectileSpeed,
			&AttProjectile::projectileSpeedUp,
			&AttProjectile::projectileActivateDist,
			&AttProjectile::projectileLifetime,
			&AttProjectile::projectileModel,
			&AttProjectile::projExplosionType,
			&AttProjectile::projExplosionEffect,
			&AttProjectile::projExplosionEffectForceNormalUp,
			&AttProjectile::projExplosionSound,
			&AttProjectile::projDudEffect,
			&AttProjectile::projDudSound,
			&AttProjectile::projImpactExplode,
			&AttProjectile::destabilizationRateTime,
			&AttProjectile::destabilizationCurvatureMax,
			&AttProjectile::destabilizeDistance,
			&AttProjectile::projTrailEffect,
			&AttProjectile::projIgnitionDelay,
			&AttProjectile::projIgnitionEffect,
			&AttProjectile::projIgnitionSound,
		};

		constexpr auto attachment_fields = glz::tuple
		{
			&WeaponAttachment::szInternalName,
			&WeaponAttachment::szDisplayName,
			&WeaponAttachment::type,
			&WeaponAttachment::weaponType,
			json_reflect::renamed("weaponClass", &WeaponAttachment::weapClass),
			json_reflect::counted(&WeaponAttachment::worldModels, 64),
			json_reflect::counted(&WeaponAttachment::viewModels, 64),
			json_reflect::counted(&WeaponAttachment::reticleViewModels, 16),
			json_reflect::nested(&WeaponAttachment::ammogeneral),
			json_reflect::nested(&WeaponAttachment::sight),
			json_reflect::nested(&WeaponAttachment::reload),
			json_reflect::nested(&WeaponAttachment::addOns),
			json_reflect::nested(&WeaponAttachment::general),
			json_reflect::nested(&WeaponAttachment::aimAssist),
			json_reflect::nested(&WeaponAttachment::ammunition),
			json_reflect::nested(&WeaponAttachment::damage),
			json_reflect::nested(&WeaponAttachment::locationDamage),
			json_reflect::nested(&WeaponAttachment::idleSettings),
			json_reflect::nested(&WeaponAttachment::adsSettings),
			json_reflect::nested(&WeaponAttachment::adsSettingsMain),
			json_reflect::nested(&WeaponAttachment::scopeDriftSettings),
			json_reflect::nested(&WeaponAttachment::scopeDriftSettingsMain),
			json_reflect::nested(&WeaponAttachment::hipSpread),
			json_reflect::nested(&WeaponAttachment::gunKick),
			json_reflect::nested(&WeaponAttachment::viewKick),
			json_reflect::manual("adsOverlay"),
			json_reflect::nested(&WeaponAttachment::ui),
			json_reflect::nested(&WeaponAttachment::rumbles, rumbles_fields),
			json_reflect::nested(&WeaponAttachment::projectile, projectile_fields), // projectileName is not in the zone
			&WeaponAttachment::ammunitionScale,
			&WeaponAttachment::damageScale,
			&WeaponAttachment::damageScaleMin,
			&WeaponAttachment::stateTimersScale,
			&WeaponAttachment::fireTimersScale,
			&WeaponAttachment::idleSettingsScale,
			&WeaponAttachment::adsSettingsScale,
			&WeaponAttachment::adsSettingsScaleMain,
			&WeaponAttachment::hipSpreadScale,
			&WeaponAttachment::gunKickScale,
			&WeaponAttachment::viewKickScale,
			&WeaponAttachment::viewCenterScale,
			&WeaponAttachment::loadIndex,
			&WeaponAttachment::hideIronSightsWithThisAttachment,
			&WeaponAttachment::shareAmmoWithAlt,
		};

		constexpr json_reflect::options dump_options{.strings = json_reflect::null_string::null};

		// v1 writes the overlay members as "overlay.<member>" keys of the adsOverlay object
		void read_ads_overlay(const ordered_json& data, AttADSOverlay*& asset, zone_memory* mem)
		{
			if (data.is_null())
			{
				return;
			}

			asset = mem->allocate<AttADSOverlay>();
			auto& overlay = asset->overlay;
			const auto read_overlay_field = [&](const char* key, auto& member)
			{
				const auto entry = data.find(key);
				if (entry != data.end() && !entry->is_null())
				{
					json_reflect::read(*entry, member, mem);
				}
			};

			read_overlay_field("overlay.shader", overlay.shader);
			read_overlay_field("overlay.shaderLowRes", overlay.shaderLowRes);
			read_overlay_field("overlay.shaderEMP", overlay.shaderEMP);
			read_overlay_field("overlay.shaderEMPLowRes", overlay.shaderEMPLowRes);
			read_overlay_field("overlay.reticle", overlay.reticle);
			read_overlay_field("overlay.width", overlay.width);
			read_overlay_field("overlay.height", overlay.height);
			read_overlay_field("overlay.widthSplitscreen", overlay.widthSplitscreen);
			read_overlay_field("overlay.heightSplitscreen", overlay.heightSplitscreen);
			json_reflect::read<ads_overlay_fields>(data, *asset, mem);
		}

		void write_ads_overlay(ordered_json& data, const AttADSOverlay* asset)
		{
			if (!asset)
			{
				return;
			}

			const auto& overlay = asset->overlay;
			json_reflect::write(data["overlay.shader"], overlay.shader);
			json_reflect::write(data["overlay.shaderLowRes"], overlay.shaderLowRes);
			json_reflect::write(data["overlay.shaderEMP"], overlay.shaderEMP);
			json_reflect::write(data["overlay.shaderEMPLowRes"], overlay.shaderEMPLowRes);
			json_reflect::write(data["overlay.reticle"], overlay.reticle);
			json_reflect::write(data["overlay.width"], overlay.width);
			json_reflect::write(data["overlay.height"], overlay.height);
			json_reflect::write(data["overlay.widthSplitscreen"], overlay.widthSplitscreen);
			json_reflect::write(data["overlay.heightSplitscreen"], overlay.heightSplitscreen);
			json_reflect::write<ads_overlay_fields>(data, *asset);
		}
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
		json_reflect::read<attachment_fields>(data, *asset, mem);
		read_ads_overlay(data["adsOverlay"], asset->adsOverlay, mem);

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

		if (data->szDisplayName)
		{
			zone->add_asset_of_type(ASSET_TYPE_LOCALIZE_ENTRY, data->szDisplayName);
		}

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

	void weapon_attachment::read(zone_reader& reader, WeaponAttachment* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);
		reader.read_string(asset->szDisplayName);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModels, 64);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->viewModels, 64);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->reticleViewModels, 16);

		if (reader.read_single(asset->ammogeneral, 3))
		{
			reader.read_asset(ASSET_TYPE_TRACER, asset->ammogeneral->tracerType);
		}

		reader.read_single(asset->sight, 3);
		reader.read_single(asset->reload, 1);
		reader.read_single(asset->addOns, 0);

		if (reader.read_single(asset->general, 3))
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->general->reticleCenter);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->general->reticleSide);
		}

		reader.read_single(asset->aimAssist, 3);
		reader.read_single(asset->ammunition, 3);
		reader.read_single(asset->damage, 3);
		reader.read_single(asset->locationDamage, 3);
		reader.read_single(asset->idleSettings, 3);
		reader.read_single(asset->adsSettings, 3);
		reader.read_single(asset->adsSettingsMain, 3);
		reader.read_single(asset->scopeDriftSettings, 3);
		reader.read_single(asset->scopeDriftSettingsMain, 3);
		reader.read_single(asset->hipSpread, 3);
		reader.read_single(asset->gunKick, 3);
		reader.read_single(asset->viewKick, 3);

		if (reader.read_single(asset->adsOverlay, 3))
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->adsOverlay->overlay.shader);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->adsOverlay->overlay.shaderLowRes);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->adsOverlay->overlay.shaderEMP);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->adsOverlay->overlay.shaderEMPLowRes);
		}

		if (reader.read_single(asset->ui, 3))
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->ui->dpadIcon);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->ui->ammoCounterIcon);
		}

		if (reader.read_single(asset->rumbles, 3))
		{
			reader.read_string(asset->rumbles->fireRumble);
			reader.read_string(asset->rumbles->meleeImpactRumble);
		}

		if (reader.read_single(asset->projectile, 3))
		{
			reader.read_asset(ASSET_TYPE_XMODEL, asset->projectile->projectileModel);
			reader.read_asset(ASSET_TYPE_FX, asset->projectile->projExplosionEffect);
			reader.read_name_reference(asset->projectile->projExplosionSound);
			reader.read_asset(ASSET_TYPE_FX, asset->projectile->projDudEffect);
			reader.read_name_reference(asset->projectile->projDudSound);
			reader.read_asset(ASSET_TYPE_FX, asset->projectile->projTrailEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->projectile->projIgnitionEffect);
			reader.read_name_reference(asset->projectile->projIgnitionSound);
		}

		reader.pop_stream();
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

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		const auto path = "attachments\\"s + asset->name + ".json"s;

		ordered_json data;
		json_reflect::write<attachment_fields>(data, *asset, dump_options);
		write_ads_overlay(data["adsOverlay"], asset->adsOverlay);

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}
