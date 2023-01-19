#include <std_include.hpp>
#include "tracerdef.hpp"

namespace zonetool::s1
{
	TracerDef* ITracerDef::parse(const std::string& name, ZoneMemory* mem)
	{
		return nullptr;
	}

	void ITracerDef::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).tracerDef;
		}
	}

	void ITracerDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void ITracerDef::load_depending(IZone* zone)
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

	std::string ITracerDef::name()
	{
		return this->name_;
	}

	std::int32_t ITracerDef::type()
	{
		return ASSET_TYPE_TRACER;
	}

	void ITracerDef::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

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

	void ITracerDef::dump(TracerDef* asset)
	{
	}
}