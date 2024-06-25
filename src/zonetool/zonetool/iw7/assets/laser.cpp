#include "std_include.hpp"
#include "laser.hpp"

namespace zonetool::iw7
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define PARSE_FIELD_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			asset->__field__[idx##__field__] = data[#__field__][idx##__field__].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__])>::type>(); \
		} \
	}

#define PARSE_ASSET(__field__) \
	if (!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty()) \
	{ \
		asset->__field__ = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(sizeof(const char*)); \
		asset->__field__->name = mem->duplicate_string(data[#__field__].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}

#define PARSE_FXCOMBINED(__field__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__.u.data = mem->manual_allocate<char>(sizeof(const char*)); \
		asset->__field__.type = data[#__field__]["type"].get<FxCombinedType>(); \
		asset->__field__.u.fx->name = mem->duplicate_string(data[#__field__]["fx"].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__.u.data = nullptr; \
	}

	LaserDef* laser::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "laser\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing laser \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<LaserDef>();

		PARSE_ASSET(laserMaterial);
		PARSE_ASSET(laserLightMaterial);
		PARSE_FIELD(ownerOnly);
		PARSE_FIELD(range);
		PARSE_FIELD(radius);
		PARSE_FIELD(endOffsetViewmodel);
		PARSE_FIELD(flarePct);
		PARSE_FXCOMBINED(beamEffect);
		PARSE_FXCOMBINED(laserEndEffect);
		PARSE_FIELD_ARR(color, 4);
		PARSE_FIELD_ARR(hdrColorScale, 4);
		PARSE_FIELD(laserLight);
		PARSE_FIELD(laserLightRadius);
		PARSE_FIELD(laserLightBeginOffset);
		PARSE_FIELD(laserLightEndOffset);
		PARSE_FIELD(laserLightBodyTweak);
		PARSE_FIELD_ARR(laserLightColor, 4);
		PARSE_FIELD_ARR(laserLightHdrColorScale, 4);
		PARSE_FIELD(range_alt);
		PARSE_FIELD(radius_alt);
		PARSE_FIELD(laserLightRadius_alt);
		PARSE_FIELD(flarePct_alt);

		return asset;
	}

	void laser::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).laserDef;
		}
	}

	void laser::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

#define SUBASSET_DEPENDING(__field__,__type__) \
	if (asset->__field__) \
	{ \
		zone->add_asset_of_type(__type__, asset->__field__->name); \
	}

#define SUBASSET_DEPENDING_FXCOMBINED(__field__) \
	if (asset->__field__.u.data) \
	{ \
		if(asset->__field__.type == FX_COMBINED_VFX) zone->add_asset_of_type(ASSET_TYPE_VFX, asset->__field__.u.vfx->name); \
		else zone->add_asset_of_type(ASSET_TYPE_FX, asset->__field__.u.fx->name); \
	}

	void laser::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		SUBASSET_DEPENDING(laserMaterial, ASSET_TYPE_MATERIAL);
		SUBASSET_DEPENDING(laserLightMaterial, ASSET_TYPE_MATERIAL);

		SUBASSET_DEPENDING_FXCOMBINED(beamEffect);
		SUBASSET_DEPENDING_FXCOMBINED(laserEndEffect);
	}

	std::string laser::name()
	{
		return this->name_;
	}

	std::int32_t laser::type()
	{
		return ASSET_TYPE_LASER;
	}

#define WRITE_ASSET(__field__,__type__) \
	if (data->__field__) \
	{ \
		dest->__field__ = reinterpret_cast<typename std::remove_reference<decltype(dest->__field__)>::type>(zone->get_asset_pointer(__type__, data->__field__->name)); \
	}

#define WRITE_FXCOMBINED(__field__) \
	if (data->__field__.u.data) \
	{ \
		if(data->__field__.type == FX_COMBINED_VFX) dest->__field__.u.vfx =  \
			reinterpret_cast<ParticleSystemDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->__field__.u.vfx->name)); \
		else dest->__field__.u.fx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->__field__.u.fx->name)); \
	}

	void laser::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		WRITE_ASSET(laserMaterial, ASSET_TYPE_MATERIAL);
		WRITE_ASSET(laserLightMaterial, ASSET_TYPE_MATERIAL);

		WRITE_FXCOMBINED(beamEffect);
		WRITE_FXCOMBINED(laserEndEffect);

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

#define DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

#define DUMP_FXCOMBINED(__field__) \
	if (asset->__field__.u.data) \
	{ \
		data[#__field__]["type"] = asset->__field__.type; \
		data[#__field__]["fx"] = asset->__field__.u.fx->name; \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

	void laser::dump(LaserDef* asset)
	{
		const auto path = "laser\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_ASSET(laserMaterial);
		DUMP_ASSET(laserLightMaterial);
		DUMP_FIELD(ownerOnly);
		DUMP_FIELD(range);
		DUMP_FIELD(radius);
		DUMP_FIELD(endOffsetViewmodel);
		DUMP_FIELD(flarePct);
		DUMP_FXCOMBINED(beamEffect);
		DUMP_FXCOMBINED(laserEndEffect);
		DUMP_FIELD_ARR(color, 4);
		DUMP_FIELD_ARR(hdrColorScale, 4);
		DUMP_FIELD(laserLight);
		DUMP_FIELD(laserLightRadius);
		DUMP_FIELD(laserLightBeginOffset);
		DUMP_FIELD(laserLightEndOffset);
		DUMP_FIELD(laserLightBodyTweak);
		DUMP_FIELD_ARR(laserLightColor, 4);
		DUMP_FIELD_ARR(laserLightHdrColorScale, 4);
		DUMP_FIELD(range_alt);
		DUMP_FIELD(radius_alt);
		DUMP_FIELD(laserLightRadius_alt);
		DUMP_FIELD(flarePct_alt);

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}