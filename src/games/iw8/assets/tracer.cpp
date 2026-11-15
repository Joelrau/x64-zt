#include <std_include.hpp>
#include "tracer.hpp"

namespace zonetool::iw8
{
	void tracer_def::read(zone_reader& reader, TracerDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_VFX, asset->effect.particleSystemDef);
		reader.read_asset(ASSET_TYPE_VFX, asset->viewmodelEffect.particleSystemDef);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
		reader.pop_stream();
	}

	void tracer_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_asset(zone, ASSET_TYPE_VFX, dest->effect.particleSystemDef);
		write_asset(zone, ASSET_TYPE_VFX, dest->viewmodelEffect.particleSystemDef);
		write_asset(zone, ASSET_TYPE_MATERIAL, dest->material);
		buf->pop_stream();
	}

	void tracer_def::dump(TracerDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("tracer\\"s + asset->name + ".tracerb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_asset(asset->effect.particleSystemDef);
		dump.dump_asset(asset->viewmodelEffect.particleSystemDef);
		dump.dump_asset(asset->material);
		dump.close();
	}

	TracerDef* tracer_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("tracer\\"s + name + ".tracerb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing tracer \"%s\"...", name.data());

		auto* asset = read.read_single<TracerDef>();
		asset->name = read.read_string();
		parse_asset(read, asset->effect.particleSystemDef);
		parse_asset(read, asset->viewmodelEffect.particleSystemDef);
		parse_asset(read, asset->material);
		read.close();
		return asset;
	}

	void tracer_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<TracerDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void tracer_def::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset(zone, ASSET_TYPE_VFX, this->asset_->effect.particleSystemDef);
		add_asset(zone, ASSET_TYPE_VFX, this->asset_->viewmodelEffect.particleSystemDef);
		add_asset(zone, ASSET_TYPE_MATERIAL, this->asset_->material);
	}

	std::string tracer_def::name()
	{
		return this->name_;
	}

	std::int32_t tracer_def::type()
	{
		return ASSET_TYPE_TRACER;
	}
}
