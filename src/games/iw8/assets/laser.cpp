#include <std_include.hpp>
#include "laser.hpp"

namespace zonetool::iw8
{
	void laser_def::read(zone_reader& reader, LaserDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->laserMaterial);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->laserLightMaterial);
		reader.read_asset(ASSET_TYPE_VFX, asset->beamEffect.particleSystemDef);
		reader.read_asset(ASSET_TYPE_VFX, asset->laserEndEffect.particleSystemDef);
		reader.pop_stream();
	}

	void laser_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_asset(zone, ASSET_TYPE_MATERIAL, dest->laserMaterial);
		write_asset(zone, ASSET_TYPE_MATERIAL, dest->laserLightMaterial);
		write_asset(zone, ASSET_TYPE_VFX, dest->beamEffect.particleSystemDef);
		write_asset(zone, ASSET_TYPE_VFX, dest->laserEndEffect.particleSystemDef);
		buf->pop_stream();
	}

	void laser_def::dump(LaserDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("laser\\"s + asset->name + ".laserb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_asset(asset->laserMaterial);
		dump.dump_asset(asset->laserLightMaterial);
		dump.dump_asset(asset->beamEffect.particleSystemDef);
		dump.dump_asset(asset->laserEndEffect.particleSystemDef);
		dump.close();
	}

	LaserDef* laser_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("laser\\"s + name + ".laserb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing laser \"%s\"...", name.data());

		auto* asset = read.read_single<LaserDef>();
		asset->name = read.read_string();
		parse_asset(read, asset->laserMaterial);
		parse_asset(read, asset->laserLightMaterial);
		parse_asset(read, asset->beamEffect.particleSystemDef);
		parse_asset(read, asset->laserEndEffect.particleSystemDef);
		read.close();
		return asset;
	}

	void laser_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<LaserDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void laser_def::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset(zone, ASSET_TYPE_MATERIAL, this->asset_->laserMaterial);
		add_asset(zone, ASSET_TYPE_MATERIAL, this->asset_->laserLightMaterial);
		add_asset(zone, ASSET_TYPE_VFX, this->asset_->beamEffect.particleSystemDef);
		add_asset(zone, ASSET_TYPE_VFX, this->asset_->laserEndEffect.particleSystemDef);
	}

	std::string laser_def::name()
	{
		return this->name_;
	}

	std::int32_t laser_def::type()
	{
		return ASSET_TYPE_LASER;
	}
}
