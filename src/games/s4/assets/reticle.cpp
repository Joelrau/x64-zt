#include <std_include.hpp>
#include "reticle.hpp"

namespace zonetool::s4
{
	void reticle_def::read(zone_reader& reader, ReticleDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->widgetName);
		reader.read_string(asset->secondaryWidgetName);
		reader.pop_stream();
	}

	void reticle_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_string(buf, dest->widgetName);
		write_string(buf, dest->secondaryWidgetName);
		buf->pop_stream();
	}

	void reticle_def::dump(ReticleDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("reticle\\"s + asset->name + ".reticleb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_string(asset->widgetName);
		dump.dump_string(asset->secondaryWidgetName);
		dump.close();
	}

	ReticleDef* reticle_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("reticle\\"s + name + ".reticleb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing reticle \"%s\"...", name.data());

		auto* asset = read.read_single<ReticleDef>();
		asset->name = read.read_string();
		asset->widgetName = read.read_string();
		asset->secondaryWidgetName = read.read_string();
		read.close();
		return asset;
	}

	void reticle_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<ReticleDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string reticle_def::name()
	{
		return this->name_;
	}

	std::int32_t reticle_def::type()
	{
		return ASSET_TYPE_RETICLE;
	}
}
