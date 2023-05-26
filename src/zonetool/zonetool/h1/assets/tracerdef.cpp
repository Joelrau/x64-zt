#include <std_include.hpp>
#include "tracerdef.hpp"

namespace zonetool::h1
{
	TracerDef* tracer_def::parse(const std::string& name, zone_memory* mem)
	{
		return nullptr;
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

		if (this->asset_->effect)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, this->asset_->effect->name);
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

		if (data->effect)
		{
			dest->effect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->effect->name));
		}

		buf->pop_stream();
	}

	void tracer_def::dump(TracerDef* asset)
	{
	}
}