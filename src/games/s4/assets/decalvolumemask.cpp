#include <std_include.hpp>
#include "decalvolumemask.hpp"

namespace zonetool::s4
{
	void decal_volume_mask::read(zone_reader& reader, GfxDecalVolumeMask* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->blendMap);
		reader.pop_stream();
	}

	void decal_volume_mask::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_asset(zone, ASSET_TYPE_IMAGE, dest->blendMap);
		buf->pop_stream();
	}

	void decal_volume_mask::dump(GfxDecalVolumeMask* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("decalvolumemask\\"s + asset->name + ".decalvolumemaskb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_asset(asset->blendMap);
		dump.close();
	}

	GfxDecalVolumeMask* decal_volume_mask::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("decalvolumemask\\"s + name + ".decalvolumemaskb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing decalvolumemask \"%s\"...", name.data());

		auto* asset = read.read_single<GfxDecalVolumeMask>();
		asset->name = read.read_string();
		parse_asset(read, asset->blendMap);
		read.close();
		return asset;
	}

	void decal_volume_mask::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<GfxDecalVolumeMask>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void decal_volume_mask::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset(zone, ASSET_TYPE_IMAGE, this->asset_->blendMap);
	}

	std::string decal_volume_mask::name()
	{
		return this->name_;
	}

	std::int32_t decal_volume_mask::type()
	{
		return ASSET_TYPE_DECAL_VOLUME_MASK;
	}
}
