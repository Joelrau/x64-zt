#include <std_include.hpp>
#include "fxeffectdef.hpp"

namespace zonetool::iw6
{
	void parse_visuals(assetmanager::reader* read, FxElemDef* def, FxElemVisuals* vis)
	{
		switch (def->elemType)
		{
		case FX_ELEM_TYPE_MODEL:
			vis->model = read->read_asset<XModel>();
			break;
		case FX_ELEM_TYPE_RUNNER:
			vis->effectDef.handle = read->read_asset<FxEffectDef>();
			break;
		case FX_ELEM_TYPE_SOUND:
		case FX_ELEM_TYPE_VECTORFIELD:
			vis->soundName = read->read_string();
			break;
		case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
			vis->particleSimAnimation = read->read_asset<FxParticleSimAnimation>();
			break;
		default:
			if (def->elemType - 12 <= 1u)
			{
				if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					vis->lightDef = read->read_asset<GfxLightDef>();
				}
			}
			else
			{
				vis->material = read->read_asset<Material>();
			}
			break;
		}
	}

	FxEffectDef* fx_effect_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = "effects\\"s + name + ".fxe"s;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing fx \"%s\"...", name.data());

		const auto asset = read.read_single<FxEffectDef>();
		asset->name = read.read_string();
		asset->elemDefs = read.read_array<FxElemDef>();

		for (int i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
		{
			auto def = &asset->elemDefs[i];

			def->velSamples = read.read_array<FxElemVelStateSample>();
			def->visSamples = read.read_array<FxElemVisStateSample>();

			if (def->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (def->visuals.markArray)
				{
					def->visuals.markArray = read.read_array<FxElemMarkVisuals>();

					for (unsigned char j = 0; j < def->visualCount; j++)
					{
						if (def->visuals.markArray[j].materials[0])
						{
							def->visuals.markArray[j].materials[0] = read.read_asset<Material>();
						}
						if (def->visuals.markArray[j].materials[1])
						{
							def->visuals.markArray[j].materials[1] = read.read_asset<Material>();
						}
					}
				}
			}
			else if (def->visualCount > 1)
			{
				def->visuals.array = read.read_array<FxElemVisuals>();

				for (unsigned char vis = 0; vis < def->visualCount; vis++)
				{
					parse_visuals(&read, def, &def->visuals.array[vis]);
				}
			}
			else
			{
				parse_visuals(&read, def, &def->visuals.instance);
			}

			def->effectOnImpact.handle = read.read_asset<FxEffectDef>();
			def->effectOnDeath.handle = read.read_asset<FxEffectDef>();
			def->effectEmitted.handle = read.read_asset<FxEffectDef>();

			if (def->extended.trailDef)
			{
				if (def->elemType == FX_ELEM_TYPE_TRAIL)
				{
					def->extended.trailDef = read.read_single<FxTrailDef>();

					if (def->extended.trailDef->verts)
					{
						def->extended.trailDef->verts = read.read_array<FxTrailVertex>();
					}

					if (def->extended.trailDef->inds)
					{
						def->extended.trailDef->inds = read.read_array<unsigned short>();
					}
				}
				else if (def->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
				{
					def->extended.sparkFountainDef = read.read_single<FxSparkFountainDef>();
				}
				else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					def->extended.spotLightDef = read.read_single<FxSpotLightDef>();
				}
				else if (def->elemType == FX_ELEM_TYPE_FLARE)
				{
					def->extended.flareDef = read.read_single<FxFlareDef>();

					if (def->extended.flareDef->intensityX)
					{
						def->extended.flareDef->intensityX = read.read_array<float>();
					}

					if (def->extended.flareDef->intensityY)
					{
						def->extended.flareDef->intensityY = read.read_array<float>();
					}

					if (def->extended.flareDef->srcCosIntensity)
					{
						def->extended.flareDef->srcCosIntensity = read.read_array<float>();
					}

					if (def->extended.flareDef->srcCosScale)
					{
						def->extended.flareDef->srcCosScale = read.read_array<float>();
					}
				}
				else
				{
					def->extended.unknownDef = read.read_single<char>();
				}
			}
		}

		read.close();

		return asset;
	}

	void fx_effect_def::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).fx;
		}
	}

	void fx_effect_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void fx_effect_def::load_depending(zone_base* zone)
	{
		auto data = this->asset_;

		auto load_FxElemVisuals = [zone](FxElemDef* def, FxElemDefVisuals* vis)
		{
			auto load_visuals = [zone](FxElemDef* def, FxElemVisuals* vis)
			{
				if (!vis->anonymous)
				{
					return;
				}

				switch (def->elemType)
				{
				case FX_ELEM_TYPE_MODEL:
					zone->add_asset_of_type(ASSET_TYPE_XMODEL, vis->model->name);
					break;
				case FX_ELEM_TYPE_RUNNER:
					zone->add_asset_of_type(ASSET_TYPE_FX, vis->effectDef.handle->name);
					break;
				case FX_ELEM_TYPE_SOUND:
				case FX_ELEM_TYPE_VECTORFIELD:
					zone->add_asset_of_type(ASSET_TYPE_SOUND, vis->soundName);
					break;
				case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
					zone->add_asset_of_type(ASSET_TYPE_PARTICLE_SIM_ANIMATION, vis->particleSimAnimation->name);
					break;
				default:
					if (def->elemType - 12 <= 1u)
					{
						if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
						{
							zone->add_asset_of_type(ASSET_TYPE_LIGHT_DEF, vis->lightDef->name);
						}
					}
					else
					{
						zone->add_asset_of_type(ASSET_TYPE_MATERIAL, vis->material->name);
					}
					break;
				}
			};

			if (def->elemType == FX_ELEM_TYPE_DECAL)
			{
				for (unsigned char i = 0; i < def->visualCount; i++)
				{
					if (vis->markArray[i].materials)
					{
						if (vis->markArray[i].materials[0])
							zone->add_asset_of_type(ASSET_TYPE_MATERIAL, vis->markArray[i].materials[0]->name);
						if (vis->markArray[i].materials[1])
							zone->add_asset_of_type(ASSET_TYPE_MATERIAL, vis->markArray[i].materials[1]->name);
					}
				}
			}
			else if (def->visualCount > 1)
			{
				for (unsigned char i = 0; i < def->visualCount; i++)
				{
					load_visuals(def, &vis->array[i]);
				}
			}
			else
			{
				load_visuals(def, &vis->instance);
			}
		};

		// Loop through frames
		for (int i = 0; i < data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission; i++)
		{
			auto& def = data->elemDefs[i];

			// Sub-FX effects
			if (def.effectEmitted.handle)
				zone->add_asset_of_type(ASSET_TYPE_FX, def.effectEmitted.handle->name);
			if (def.effectOnDeath.handle)
				zone->add_asset_of_type(ASSET_TYPE_FX, def.effectOnDeath.handle->name);
			if (def.effectOnImpact.handle)
				zone->add_asset_of_type(ASSET_TYPE_FX, def.effectOnImpact.handle->name);

			// Visuals
			load_FxElemVisuals(&def, &def.visuals);
		}
	}

	std::string fx_effect_def::name()
	{
		return this->name_;
	}

	std::int32_t fx_effect_def::type()
	{
		return ASSET_TYPE_FX;
	}

	void write_fx_elem_visuals(zone_base* zone, zone_buffer* buf, FxElemDef* def, FxElemVisuals* dest)
	{
		auto data = dest;

		if (!data->anonymous)
		{
			return;
		}

		switch (def->elemType)
		{
		case FX_ELEM_TYPE_MODEL:
			dest->model = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->model->name));
			break;
		case FX_ELEM_TYPE_RUNNER:
			dest->effectDef.name = buf->write_str(data->effectDef.handle->name);
			break;
		case FX_ELEM_TYPE_SOUND:
		case FX_ELEM_TYPE_VECTORFIELD:
			dest->soundName = buf->write_str(data->soundName);
			break;
		case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
			dest->particleSimAnimation = reinterpret_cast<FxParticleSimAnimation*>(zone->get_asset_pointer(ASSET_TYPE_PARTICLE_SIM_ANIMATION, data->particleSimAnimation->name));
			break;
		default:
			if (def->elemType - 12 <= 1u)
			{
				if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					dest->lightDef = reinterpret_cast<GfxLightDef*>(zone->get_asset_pointer(ASSET_TYPE_LIGHT_DEF, data->lightDef->name));
				}
			}
			else
			{
				dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->material->name));
			}
			break;
		}
	}

	void write_fx_elem_def_visuals(zone_base* zone, zone_buffer* buf, FxElemDef* def, FxElemDefVisuals* dest)
	{
		auto data = dest;

		if (def->elemType == FX_ELEM_TYPE_DECAL)
		{
			if (data->markArray)
			{
				auto destvisuals = buf->write(data->markArray, def->visualCount);

				for (unsigned char i = 0; i < def->visualCount; i++)
				{
					destvisuals[i].materials[0] = (data->markArray[i].materials[0])
						? reinterpret_cast<Material*>(zone->get_asset_pointer(
							ASSET_TYPE_MATERIAL, data->markArray[i].materials[0]->name))
						: nullptr;
					destvisuals[i].materials[1] = (data->markArray[i].materials[1])
						? reinterpret_cast<Material*>(zone->get_asset_pointer(
							ASSET_TYPE_MATERIAL, data->markArray[i].materials[1]->name))
						: nullptr;
				}
			}
		}
		else if (def->visualCount > 1)
		{
			auto vis = buf->write(data->array, def->visualCount);

			for (unsigned char i = 0; i < def->visualCount; i++)
			{
				write_fx_elem_visuals(zone, buf, def, &vis[i]);
			}
		}
		else
		{
			write_fx_elem_visuals(zone, buf, def, &dest->instance);
		}
	}

	void write_fx_elem_def(zone_base* zone, zone_buffer* buf, FxElemDef* dest)
	{
		auto data = dest;

		if (data->velSamples)
		{
			buf->align(3);
			buf->write(data->velSamples, data->velIntervalCount + 1);
			zone_buffer::clear_pointer(&dest->velSamples);
		}

		if (data->visSamples)
		{
			buf->align(3);
			buf->write(data->visSamples, data->visStateIntervalCount + 1);
			zone_buffer::clear_pointer(&dest->velSamples);
		}

		write_fx_elem_def_visuals(zone, buf, data, &dest->visuals);

		if (data->effectOnImpact.handle)
		{
			buf->write_str(data->effectOnImpact.handle->name);
			zone_buffer::clear_pointer(&dest->effectOnImpact);
		}

		if (data->effectOnDeath.handle)
		{
			buf->write_str(data->effectOnDeath.handle->name);
			zone_buffer::clear_pointer(&dest->effectOnDeath);
		}

		if (data->effectEmitted.handle)
		{
			buf->write_str(data->effectEmitted.handle->name);
			zone_buffer::clear_pointer(&dest->effectEmitted);
		}

		if (data->extended.trailDef)
		{
			if (data->elemType == FX_ELEM_TYPE_TRAIL)
			{
				if (data->extended.trailDef)
				{
					buf->align(3);
					buf->write(data->extended.trailDef);

					if (data->extended.trailDef->verts)
					{
						buf->align(3);
						buf->write(data->extended.trailDef->verts, data->extended.trailDef->vertCount);
					}

					if (data->extended.trailDef->inds)
					{
						buf->align(1);
						buf->write(data->extended.trailDef->inds, data->extended.trailDef->indCount);
					}

					zone_buffer::clear_pointer(&dest->extended.trailDef);
				}
			}
			else if (data->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
			{
				if (data->extended.sparkFountainDef)
				{
					buf->align(3);
					buf->write(data->extended.sparkFountainDef);
					zone_buffer::clear_pointer(&dest->extended.sparkFountainDef);
				}
			}
			else if (data->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
			{
				if (data->extended.spotLightDef)
				{
					buf->align(3);
					buf->write(data->extended.spotLightDef);
					zone_buffer::clear_pointer(&dest->extended.spotLightDef);
				}
			}
			else if (data->elemType == FX_ELEM_TYPE_FLARE)
			{
				buf->align(3);
				buf->write(data->extended.flareDef);

				if (data->extended.flareDef->intensityX)
				{
					buf->write(data->extended.flareDef->intensityX, data->extended.flareDef->intensityXIntervalCount + 1);
				}

				if (data->extended.flareDef->intensityY)
				{
					buf->write(data->extended.flareDef->intensityY, data->extended.flareDef->intensityYIntervalCount + 1);
				}

				if (data->extended.flareDef->srcCosIntensity)
				{
					buf->write(data->extended.flareDef->srcCosIntensity, data->extended.flareDef->srcCosIntensityIntervalCount + 1);
				}

				if (data->extended.flareDef->srcCosScale)
				{
					buf->write(data->extended.flareDef->srcCosScale, data->extended.flareDef->srcCosScaleIntervalCount + 1);
				}

				zone_buffer::clear_pointer(&dest->extended.flareDef);
			}
			else
			{
				buf->align(0);
				buf->write_stream(data->extended.unknownDef, 1);
				zone_buffer::clear_pointer(&dest->extended.unknownDef);
			}
		}
	}

	void fx_effect_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->elemDefs)
		{
			buf->align(3);
			auto destdef = buf->write(data->elemDefs,
				data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission);

			for (int i = 0; i < (data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission); i++)
			{
				write_fx_elem_def(zone, buf, &destdef[i]);
			}

			zone_buffer::clear_pointer(&dest->elemDefs);
		}

		buf->pop_stream();
	}

	void dump_visuals(assetmanager::dumper* dump, FxElemDef* def, FxElemVisuals* vis)
	{
		switch (def->elemType)
		{
		case FX_ELEM_TYPE_MODEL:
			dump->dump_asset(vis->model);
			break;
		case FX_ELEM_TYPE_RUNNER:
			dump->dump_asset(vis->effectDef.handle);
			break;
		case FX_ELEM_TYPE_SOUND:
		case FX_ELEM_TYPE_VECTORFIELD:
			dump->dump_string(vis->soundName);
			break;
		case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
			dump->dump_asset(vis->particleSimAnimation);
			break;
		default:
			if (def->elemType - 12 <= 1u)
			{
				if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					dump->dump_asset(vis->lightDef);
				}
			}
			else
			{
				dump->dump_asset(vis->material);
			}
			break;
		}
	}

	void fx_effect_def::dump(FxEffectDef* asset)
	{
		assetmanager::dumper dump;

		const auto path = "effects\\"s + asset->name + ".fxe"s;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->elemDefs,
			asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission);

		// dump elemDefs
		for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
		{
			auto def = &asset->elemDefs[i];

			// dump elem samples
			dump.dump_array(def->velSamples, def->velIntervalCount + 1);
			dump.dump_array(def->visSamples, def->visStateIntervalCount + 1);

			// dump visuals
			if (def->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (def->visuals.markArray)
				{
					dump.dump_array(def->visuals.markArray, def->visualCount);

					for (unsigned char a = 0; a < def->visualCount; a++)
					{
						if (def->visuals.markArray[a].materials[0])
						{
							dump.dump_asset(def->visuals.markArray[a].materials[0]);
						}
						if (def->visuals.markArray[a].materials[1])
						{
							dump.dump_asset(def->visuals.markArray[a].materials[1]);
						}
					}
				}
			}
			else if (def->visualCount > 1)
			{
				if (def->visuals.markArray)
				{
					dump.dump_array(def->visuals.array, def->visualCount);
					for (unsigned char vis = 0; vis < def->visualCount; vis++)
					{
						dump_visuals(&dump, def, &def->visuals.array[vis]);
					}
				}
			}
			else
			{
				dump_visuals(&dump, def, &def->visuals.instance);
			}

			// dump reference FX defs
			dump.dump_asset(def->effectOnImpact.handle);
			dump.dump_asset(def->effectOnDeath.handle);
			dump.dump_asset(def->effectEmitted.handle);

			// dump extended FX data
			if (def->extended.trailDef)
			{
				if (def->elemType == FX_ELEM_TYPE_TRAIL)
				{
					dump.dump_single(def->extended.trailDef);

					if (def->extended.trailDef->verts)
					{
						dump.dump_array(def->extended.trailDef->verts, def->extended.trailDef->vertCount);
					}

					if (def->extended.trailDef->inds)
					{
						dump.dump_array(def->extended.trailDef->inds, def->extended.trailDef->indCount);
					}
				}
				else if (def->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
				{
					dump.dump_single(def->extended.sparkFountainDef);
				}
				else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					dump.dump_single(def->extended.spotLightDef);
				}
				else if (def->elemType == FX_ELEM_TYPE_FLARE)
				{
					dump.dump_single(def->extended.flareDef);

					if (def->extended.flareDef->intensityX)
					{
						dump.dump_array(def->extended.flareDef->intensityX, def->extended.flareDef->intensityXIntervalCount + 1);
					}

					if (def->extended.flareDef->intensityY)
					{
						dump.dump_array(def->extended.flareDef->intensityY, def->extended.flareDef->intensityYIntervalCount + 1);
					}

					if (def->extended.flareDef->srcCosIntensity)
					{
						dump.dump_array(def->extended.flareDef->srcCosIntensity, def->extended.flareDef->srcCosIntensityIntervalCount + 1);
					}

					if (def->extended.flareDef->srcCosScale)
					{
						dump.dump_array(def->extended.flareDef->srcCosScale, def->extended.flareDef->srcCosScaleIntervalCount + 1);
					}
				}
				else
				{
					dump.dump_single(def->extended.unknownDef);
				}
			}
		}

		dump.close();
	}
}