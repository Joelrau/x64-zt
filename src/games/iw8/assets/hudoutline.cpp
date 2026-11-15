#include <std_include.hpp>
#include "hudoutline.hpp"

namespace zonetool::iw8
{
	void hud_outline::read(zone_reader& reader, HudOutlineDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void hud_outline::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		buf->pop_stream();
	}

	void hud_outline::dump(HudOutlineDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("hudoutline\\"s + asset->name + ".hudoutlineb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.close();
	}

	HudOutlineDef* hud_outline::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("hudoutline\\"s + name + ".hudoutlineb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing hudoutline \"%s\"...", name.data());

		auto* asset = read.read_single<HudOutlineDef>();
		asset->name = read.read_string();
		read.close();
		return asset;
	}

	void hud_outline::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<HudOutlineDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string hud_outline::name()
	{
		return this->name_;
	}

	std::int32_t hud_outline::type()
	{
		return ASSET_TYPE_HUDOUTLINE;
	}
}
