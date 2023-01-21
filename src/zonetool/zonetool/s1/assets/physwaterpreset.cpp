#include "std_include.hpp"
#include "physwaterpreset.hpp"

namespace zonetool::s1
{
	void IPhysWaterPreset::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).physWaterPreset;
	}

	void IPhysWaterPreset::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IPhysWaterPreset::load_depending(IZone* zone)
	{
		auto* data = this->asset_;

		if (data->fx0)
		{
			zone->add_asset_of_type(ASSET_TYPE_FX, data->fx0->name);
		}
		if (data->fx1)
		{
			zone->add_asset_of_type(ASSET_TYPE_FX, data->fx1->name);
		}
		if (data->fx2)
		{
			zone->add_asset_of_type(ASSET_TYPE_FX, data->fx2->name);
		}
		if (data->fx3)
		{
			zone->add_asset_of_type(ASSET_TYPE_FX, data->fx3->name);
		}
		if (data->fx4)
		{
			zone->add_asset_of_type(ASSET_TYPE_FX, data->fx4->name);
		}
		if (data->fx5)
		{
			zone->add_asset_of_type(ASSET_TYPE_FX, data->fx5->name);
		}
		if (data->fx6)
		{
			zone->add_asset_of_type(ASSET_TYPE_FX, data->fx6->name);
		}
	}

	std::string IPhysWaterPreset::name()
	{
		return this->name_;
	}

	std::int32_t IPhysWaterPreset::type()
	{
		return ASSET_TYPE_PHYSWATERPRESET;
	}

	void IPhysWaterPreset::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->fx0)
		{
			dest->fx0 = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->fx0->name));
		}
		if (data->fx1)
		{
			dest->fx1 = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->fx1->name));
		}
		if (data->fx2)
		{
			dest->fx2 = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->fx2->name));
		}
		if (data->fx3)
		{
			dest->fx3 = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->fx3->name));
		}
		if (data->fx4)
		{
			dest->fx4 = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->fx4->name));
		}
		if (data->fx5)
		{
			dest->fx5 = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->fx5->name));
		}
		if (data->fx6)
		{
			dest->fx6 = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->fx6->name));
		}

		buf->pop_stream();
	}

	void IPhysWaterPreset::dump(PhysWaterPreset* asset)
	{
	}
}