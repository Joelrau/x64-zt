#include <std_include.hpp>
#include "gfxlightdef.hpp"

namespace zonetool::iw7
{
	GfxLightDef* gfx_light_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = "lights\\"s + name;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing lightdef \"%s\"...", name.data());

		auto* asset = read.read_single<GfxLightDef>();
		asset->name = read.read_string();

		asset->iesProfile = read.read_string();

		read.close();

		return asset;
	}

	void gfx_light_def::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).lightDef;
		}
	}

	void gfx_light_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_light_def::load_depending(zone_base* zone)
	{
	}

	std::string gfx_light_def::name()
	{
		return this->name_;
	}

	std::int32_t gfx_light_def::type()
	{
		return ASSET_TYPE_LIGHT_DEF;
	}

	void gfx_light_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->iesProfile)
		{
			dest->iesProfile = buf->write_str(data->iesProfile);
		}

		buf->pop_stream();
	}

	void gfx_light_def::dump(GfxLightDef* asset)
	{
		assetmanager::dumper dump;

		const auto path = "lights\\"s + asset->name;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_string(asset->iesProfile);

		dump.close();
	}
}