#include <std_include.hpp>
#include "decalvolumematerial.hpp"

namespace zonetool::s4
{
	void decal_volume_material::read(zone_reader& reader, GfxDecalVolumeMaterial* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		for (auto& channel : asset->channels)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, channel);
		}
		reader.pop_stream();
	}

	void decal_volume_material::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		for (auto& channel : dest->channels)
		{
			write_asset(zone, ASSET_TYPE_IMAGE, channel);
		}
		buf->pop_stream();
	}

	void decal_volume_material::dump(GfxDecalVolumeMaterial* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("decalvolumematerial\\"s + asset->name + ".decalvolumematerialb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		for (auto& channel : asset->channels)
		{
			dump.dump_asset(channel);
		}
		dump.close();
	}

	GfxDecalVolumeMaterial* decal_volume_material::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("decalvolumematerial\\"s + name + ".decalvolumematerialb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing decalvolumematerial \"%s\"...", name.data());

		auto* asset = read.read_single<GfxDecalVolumeMaterial>();
		asset->name = read.read_string();
		for (auto& channel : asset->channels)
		{
			parse_asset(read, channel);
		}
		read.close();
		return asset;
	}

	void decal_volume_material::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<GfxDecalVolumeMaterial>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void decal_volume_material::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		for (const auto channel : this->asset_->channels)
		{
			add_asset(zone, ASSET_TYPE_IMAGE, channel);
		}
	}

	std::string decal_volume_material::name()
	{
		return this->name_;
	}

	std::int32_t decal_volume_material::type()
	{
		return ASSET_TYPE_DECAL_VOLUME_MATERIAL;
	}
}
