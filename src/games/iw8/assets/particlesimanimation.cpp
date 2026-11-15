#include <std_include.hpp>
#include "particlesimanimation.hpp"

namespace zonetool::iw8
{
	void particle_sim_animation::read(zone_reader& reader, FxParticleSimAnimation* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		const auto& header = asset->header;
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
		reader.read_array(asset->particleData, 31, header.particleDataCount);
		reader.read_array(asset->particleDataOrientation3D, 7, header.hasOrientation3D ? header.particleDataCount : 0);
		reader.read_array(asset->particleDataIndex, 0, header.hasIndex ? header.particleDataCount : 0);
		reader.read_array(asset->frames, 3, header.frameCount);
		reader.read_array(asset->colorTable, 3, header.colorTableSize);
		reader.pop_stream();
	}

	void particle_sim_animation::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		const auto& header = data->header;
		write_asset(zone, ASSET_TYPE_MATERIAL, dest->material);
		write_array(buf, dest->particleData, header.particleDataCount, 31);
		write_array(buf, dest->particleDataOrientation3D, header.hasOrientation3D ? header.particleDataCount : 0, 7);
		write_array(buf, dest->particleDataIndex, header.hasIndex ? header.particleDataCount : 0, 0);
		write_array(buf, dest->frames, header.frameCount, 3);
		write_array(buf, dest->colorTable, header.colorTableSize, 3);
		buf->pop_stream();
	}

	void particle_sim_animation::dump(FxParticleSimAnimation* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("particlesimanimation\\"s + asset->name + ".particlesimanimationb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		const auto& header = asset->header;
		dump.dump_asset(asset->material);
		dump.dump_array(asset->particleData, header.particleDataCount);
		dump.dump_array(asset->particleDataOrientation3D, header.hasOrientation3D ? header.particleDataCount : 0);
		dump.dump_array(asset->particleDataIndex, header.hasIndex ? header.particleDataCount : 0);
		dump.dump_array(asset->frames, header.frameCount);
		dump.dump_array(asset->colorTable, header.colorTableSize);
		dump.close();
	}

	FxParticleSimAnimation* particle_sim_animation::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("particlesimanimation\\"s + name + ".particlesimanimationb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing particlesimanimation \"%s\"...", name.data());

		auto* asset = read.read_single<FxParticleSimAnimation>();
		asset->name = read.read_string();
		parse_asset(read, asset->material);
		asset->particleData = read.read_array<FxParticleSimAnimationParticleData>();
		asset->particleDataOrientation3D = read.read_array<FxParticleSimAnimationParticleDataOrientation3D>();
		asset->particleDataIndex = read.read_array<FxParticleSimAnimationParticleDataIndex>();
		asset->frames = read.read_array<FxParticleSimAnimationFrame>();
		asset->colorTable = read.read_array<vec4_t>();
		read.close();
		return asset;
	}

	void particle_sim_animation::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<FxParticleSimAnimation>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void particle_sim_animation::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset(zone, ASSET_TYPE_MATERIAL, this->asset_->material);
	}

	std::string particle_sim_animation::name()
	{
		return this->name_;
	}

	std::int32_t particle_sim_animation::type()
	{
		return ASSET_TYPE_PARTICLE_SIM_ANIMATION;
	}
}
