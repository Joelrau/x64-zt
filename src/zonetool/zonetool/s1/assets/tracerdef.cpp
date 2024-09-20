#include <std_include.hpp>
#include "tracerdef.hpp"

namespace zonetool::s1
{
	TracerDef* tracer_def::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "tracer\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing tracer \"%s\"...", name.data());

		auto* asset = read.read_single<TracerDef>();
		asset->name = read.read_string();

		asset->material = read.read_asset<Material>();
		asset->effectDef = read.read_asset<FxEffectDef>();

		read.close();

		return asset;
	}

	void tracer_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).tracerDef;
		}
	}

	void tracer_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void tracer_def::load_depending(zone_base* zone)
	{
		if (this->asset_->material)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, this->asset_->material->name);
		}

		if (this->asset_->effectDef)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, this->asset_->effectDef->name);
		}
	}

	std::string tracer_def::name()
	{
		return this->name_;
	}

	std::int32_t tracer_def::type()
	{
		return ASSET_TYPE_TRACER;
	}

	void tracer_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->material)
		{
			dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->material->name));
		}

		if (data->effectDef)
		{
			dest->effectDef = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->effectDef->name));
		}

		buf->pop_stream();
	}

	void tracer_def::dump(TracerDef* asset)
	{
		const auto path = "tracer\\"s + asset->name;

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_asset(asset->material);
		dump.dump_asset(asset->effectDef);

		dump.close();
	}
}