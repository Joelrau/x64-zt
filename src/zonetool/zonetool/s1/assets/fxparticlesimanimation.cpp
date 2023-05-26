#include <std_include.hpp>
#include "fxparticlesimanimation.hpp"

namespace zonetool::s1
{
	FxParticleSimAnimation* fx_particle_sim_animation::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = "particlesimanimation\\"s + name + ".psa"s;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing particlesimanimation \"%s\"...", name.data());

		auto* asset = read.read_single<FxParticleSimAnimation>();
		asset->name = read.read_string();

		if (asset->material)
		{
			asset->material = read.read_asset<Material>();
		}
		if (asset->particleData)
		{
			asset->particleData = read.read_array<FxParticleSimAnimationParticleData>();
		}
		if (asset->frames)
		{
			asset->frames = read.read_array<FxParticleSimAnimationFrame>();
		}
		if (asset->colorTable)
		{
			asset->colorTable = read.read_array<GfxColorFloat>();
		}

		read.close();

		return asset;
	}

	void fx_particle_sim_animation::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).particleSimAnimation;
		}
	}

	void fx_particle_sim_animation::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void fx_particle_sim_animation::load_depending(zone_base* zone)
	{
		if (this->asset_->material)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, this->asset_->material->name);
		}
	}

	std::string fx_particle_sim_animation::name()
	{
		return this->name_;
	}

	std::int32_t fx_particle_sim_animation::type()
	{
		return ASSET_TYPE_PARTICLE_SIM_ANIMATION;
	}

	void fx_particle_sim_animation::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->material)
		{
			dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->material->name));
		}
		if (data->particleData)
		{
			buf->align(31);
			buf->write(data->particleData, data->header.particleDataCount);
			zone_buffer::clear_pointer(&dest->particleData);
		}
		if (data->frames)
		{
			buf->align(3);
			buf->write(data->frames, data->header.frameCount);
			zone_buffer::clear_pointer(&dest->frames);
		}
		if (data->colorTable)
		{
			buf->align(3);
			buf->write(data->colorTable, data->header.colorTableSize);
			zone_buffer::clear_pointer(&dest->colorTable);
		}

		buf->pop_stream();
	}

	void fx_particle_sim_animation::dump(FxParticleSimAnimation* asset)
	{
		assetmanager::dumper dump;

		const auto path = "particlesimanimation\\"s + asset->name + ".psa"s;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		if (asset->material)
		{
			dump.dump_asset(asset->material);
		}
		if (asset->particleData)
		{
			dump.dump_array(asset->particleData, asset->header.particleDataCount);
		}
		if (asset->frames)
		{
			dump.dump_array(asset->frames, asset->header.frameCount);
		}
		if (asset->colorTable)
		{
			dump.dump_array(asset->colorTable, asset->header.colorTableSize);
		}

		dump.close();
	}
}