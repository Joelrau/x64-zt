#include "std_include.hpp"
#include "physwaterpreset.hpp"

namespace zonetool::h1
{
	PhysWaterPreset* phys_water_preset::parse(std::string name, zone_memory* mem)
	{
		const auto path = "physwaterpreset\\"s + name + ".pwp";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physwaterpreset \"%s\"...", name.data());

		auto* asset = read.read_single<PhysWaterPreset>();
		asset->name = read.read_string();

		asset->fx0 = read.read_asset<FxEffectDef>();
		asset->fx1 = read.read_asset<FxEffectDef>();
		asset->fx2 = read.read_asset<FxEffectDef>();
		asset->fx3 = read.read_asset<FxEffectDef>();
		asset->fx4 = read.read_asset<FxEffectDef>();
		asset->fx5 = read.read_asset<FxEffectDef>();
		asset->fx6 = read.read_asset<FxEffectDef>();

		read.close();

		return asset;
	}

	void phys_water_preset::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physWaterPreset;
	}

	void phys_water_preset::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void phys_water_preset::load_depending(zone_base* zone)
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

	std::string phys_water_preset::name()
	{
		return this->name_;
	}

	std::int32_t phys_water_preset::type()
	{
		return ASSET_TYPE_PHYSWATERPRESET;
	}

	void phys_water_preset::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

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

	void phys_water_preset::dump(PhysWaterPreset* asset)
	{
		const auto path = "physwaterpreset\\"s + asset->name + ".pwp";

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_asset(asset->fx0);
		dump.dump_asset(asset->fx1);
		dump.dump_asset(asset->fx2);
		dump.dump_asset(asset->fx3);
		dump.dump_asset(asset->fx4);
		dump.dump_asset(asset->fx5);
		dump.dump_asset(asset->fx6);

		dump.close();
	}
}