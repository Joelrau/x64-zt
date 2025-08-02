#include "std_include.hpp"
#include "impactfx.hpp"

namespace zonetool::iw7
{
#define PARSE_FXCOMBINED_ARRAY(__array__, __count__) \
	{ \
		const auto& arr = data[#__array__]; \
		for (int j = 0; j < (__count__); ++j) \
		{ \
			auto& elem = asset->__array__[j]; \
			if (!arr[j].is_null()) \
			{ \
				elem.u.data = mem->manual_allocate<char>(sizeof(const char*)); \
				elem.type = arr[j]["type"].get<FxCombinedType>(); \
				elem.u.fx->name = mem->duplicate_string(arr[j]["fx"].get<std::string>()); \
			} \
			else \
			{ \
				elem.u.data = nullptr; \
			} \
		} \
	}

	void parse_entry(FxImpactEntry* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FXCOMBINED_ARRAY(nonflesh, 64);
		PARSE_FXCOMBINED_ARRAY(flesh, 23);
	}

	FxImpactTable* impact_fx::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "impactfx\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing impactfx \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<FxImpactTable>();
		asset->name = mem->duplicate_string(name);

		asset->table = mem->allocate<FxImpactEntry>(49);
		for (auto i = 0; i < 49; i++)
		{
			parse_entry(&asset->table[i], data[i], mem);
		}

		return asset;
	}

	void impact_fx::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).impactFx;
		}
	}

	void impact_fx::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

#define SUBASSET_DEPENDING_FXCOMBINED(__field__) \
	if (asset->__field__.u.data) \
	{ \
		if(asset->__field__.type == FX_COMBINED_VFX) zone->add_asset_of_type(ASSET_TYPE_VFX, asset->__field__.u.vfx->name); \
		else zone->add_asset_of_type(ASSET_TYPE_FX, asset->__field__.u.fx->name); \
	}

	void impact_fx::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		for (auto i = 0; i < 49; i++)
		{
			for (auto j = 0; j < 64; j++)
			{
				SUBASSET_DEPENDING_FXCOMBINED(table[i].nonflesh[j]);
			}
			for (auto j = 0; j < 23; j++)
			{
				SUBASSET_DEPENDING_FXCOMBINED(table[i].flesh[j]);
			}
		}
	}

	std::string impact_fx::name()
	{
		return this->name_;
	}

	std::int32_t impact_fx::type()
	{
		return ASSET_TYPE_IMPACT_FX;
	}

#define WRITE_FXCOMBINED(__field__) \
	if (data->__field__.u.data) \
	{ \
		if(data->__field__.type == FX_COMBINED_VFX) \
			dest->__field__.u.vfx = reinterpret_cast<ParticleSystemDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->__field__.u.vfx->name)); \
		else \
			dest->__field__.u.fx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->__field__.u.fx->name)); \
	}

	void impact_fx::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->table)
		{
			buf->align(7);
			dest->table = buf->write(data->table, 49);
			for (auto i = 0; i < 49; i++)
			{
				for (auto j = 0; j < 64; j++)
				{
					WRITE_FXCOMBINED(table[i].nonflesh[j]);
				}
				for (auto j = 0; j < 23; j++)
				{
					WRITE_FXCOMBINED(table[i].flesh[j]);
				}
			}
			buf->clear_pointer(&dest->table);
		}

		buf->pop_stream();
	}

#define DUMP_FXCOMBINED_ARRAY(__array__, __count__) \
	{ \
		ordered_json arr = ordered_json::array(); \
		for (int j = 0; j < (__count__); ++j) \
		{ \
			const auto& elem = asset->__array__[j]; \
			if (elem.u.data) \
			{ \
				ordered_json entry; \
				entry["type"] = elem.type; \
				entry["fx"] = elem.u.fx->name; \
				arr.push_back(std::move(entry)); \
			} \
			else \
			{ \
				arr.push_back(nullptr); \
			} \
		} \
		data[#__array__] = std::move(arr); \
	}

	void dump_entry(FxImpactEntry* asset, ordered_json& data)
	{
		DUMP_FXCOMBINED_ARRAY(nonflesh, 64);
		DUMP_FXCOMBINED_ARRAY(flesh, 23);
	}

	void impact_fx::dump(FxImpactTable* asset)
	{
		const auto path = "impactfx\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		for (auto i = 0; i < 49; i++)
		{
			dump_entry(&asset->table[i], data[i]);
		}

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}