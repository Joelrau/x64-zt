#include <std_include.hpp>
#include "weaponattachment.hpp"

namespace zonetool::iw6
{
	WeaponAttachment* weapon_attachment::parse(const std::string& name, zone_memory* mem)
	{
		return nullptr;
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

		// TODO: projectile?
	}

	std::string weapon_attachment::name()
	{
		return this->name_;
	}

	std::int32_t weapon_attachment::type()
	{
		return ASSET_TYPE_ATTACHMENT;
	}

	void weapon_attachment::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->szInternalName = buf->write_str(this->name());

		if (data->szDisplayName)
		{
			dest->szDisplayName = buf->write_str(data->szDisplayName);
		}

		if (data->worldModels)
		{
			buf->align(7);
			auto models = buf->write(data->worldModels, 64);

			for (auto i = 0; i < 64; i++)
			{
				if (models[i])
				{
					models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, models[i]->name));
				}
			}

			zone_buffer::clear_pointer(&dest->worldModels);
		}

		if (data->viewModels)
		{
			buf->align(7);
			auto models = buf->write(data->viewModels, 64);

			for (auto i = 0; i < 64; i++)
			{
				if (models[i])
				{
					models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, models[i]->name));
				}
			}

			zone_buffer::clear_pointer(&dest->viewModels);
		}

		if (data->reticleViewModels)
		{
			buf->align(7);
			auto models = buf->write(data->reticleViewModels, 16);

			for (auto i = 0; i < 16; i++)
			{
				if (models[i])
				{
					models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, models[i]->name));
				}
			}

			zone_buffer::clear_pointer(&dest->reticleViewModels);
		}

		if (data->ammogeneral)
		{
			buf->align(3);
			auto ammo = buf->write(data->ammogeneral);

			if (ammo->tracerType)
			{
				ammo->tracerType = reinterpret_cast<TracerDef*>(zone->get_asset_pointer(
					ASSET_TYPE_TRACER, ammo->tracerType->name));
			}
		}

		if (data->sight)
		{
			buf->align(3);
			buf->write(data->sight);
		}

		if (data->reload)
		{
			buf->align(1);
			buf->write(data->reload);
		}

		if (data->addOns)
		{
			buf->align(0);
			buf->write(data->addOns);
		}

		if (data->general)
		{
			buf->align(3);
			auto general = buf->write(data->general);

			if (general->reticleCenter)
			{
				general->reticleCenter = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, general->reticleCenter->name));
			}

			if (general->reticleSide)
			{
				general->reticleSide = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, general->reticleSide->name));
			}
		}

		if (data->aimAssist)
		{
			buf->align(3);
			buf->write(data->aimAssist);
		}

		if (data->ammunition)
		{
			buf->align(3);
			buf->write(data->ammunition);
		}

		if (data->damage)
		{
			buf->align(3);
			buf->write(data->damage);
		}

		if (data->locationDamage)
		{
			buf->align(3);
			buf->write(data->locationDamage);
		}

		if (data->idleSettings)
		{
			buf->align(3);
			buf->write(data->idleSettings);
		}

		if (data->adsSettings)
		{
			buf->align(3);
			buf->write(data->adsSettings);
		}

		if (data->adsSettingsMain)
		{
			buf->align(3);
			buf->write(data->adsSettingsMain);
		}

		if (data->scopeDriftSettings)
		{
			buf->align(3);
			buf->write(data->scopeDriftSettings);
		}

		if (data->scopeDriftSettingsMain)
		{
			buf->align(3);
			buf->write(data->scopeDriftSettingsMain);
		}

		if (data->hipSpread)
		{
			buf->align(3);
			buf->write(data->hipSpread);
		}

		if (data->gunKick)
		{
			buf->align(3);
			buf->write(data->gunKick);
		}

		if (data->viewKick)
		{
			buf->align(3);
			buf->write(data->viewKick);
		}

		if (data->adsOverlay)
		{
			buf->align(3);
			auto overlay = buf->write(data->adsOverlay);

			if (overlay->overlay.shader)
			{
				overlay->overlay.shader = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, overlay->overlay.shader->name));
			}

			if (overlay->overlay.shaderLowRes)
			{
				overlay->overlay.shaderLowRes = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, overlay->overlay.shaderLowRes->name));
			}

			if (overlay->overlay.shaderEMP)
			{
				overlay->overlay.shaderEMP = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, overlay->overlay.shaderEMP->name));
			}

			if (overlay->overlay.shaderEMPLowRes)
			{
				overlay->overlay.shaderEMPLowRes = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, overlay->overlay.shaderEMPLowRes->name));
			}
		}

		if (data->ui)
		{
			buf->align(3);
			auto ui = buf->write(data->ui);

			if (ui->dpadIcon)
			{
				ui->dpadIcon = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, ui->dpadIcon->name));
			}

			if (ui->ammoCounterIcon)
			{
				ui->ammoCounterIcon = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, ui->ammoCounterIcon->name));
			}
		}

		if (data->rumbles)
		{
			buf->align(3);
			auto rumbles = buf->write(data->rumbles);

			if (rumbles->fireRumble)
			{
				rumbles->fireRumble = buf->write_str(rumbles->fireRumble);
			}

			if (rumbles->meleeImpactRumble)
			{
				rumbles->meleeImpactRumble = buf->write_str(rumbles->meleeImpactRumble);
			}
		}

		// TODO: projectiles?
		dest->projectile = nullptr;

		buf->pop_stream();
	}

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
	}
}