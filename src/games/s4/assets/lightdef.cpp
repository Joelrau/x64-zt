#include <std_include.hpp>
#include "lightdef.hpp"

namespace zonetool::s4
{
	void light_def::read(zone_reader& reader, GfxLightDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->iesProfile);
		reader.pop_stream();
	}

	void light_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_string(buf, dest->iesProfile);
		buf->pop_stream();
	}

	void light_def::dump(GfxLightDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("lightdef\\"s + asset->name + ".lightdefb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_string(asset->iesProfile);
		dump.close();
	}

	GfxLightDef* light_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("lightdef\\"s + name + ".lightdefb"))
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

	void light_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<GfxLightDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string light_def::name()
	{
		return this->name_;
	}

	std::int32_t light_def::type()
	{
		return ASSET_TYPE_LIGHT_DEF;
	}
}
