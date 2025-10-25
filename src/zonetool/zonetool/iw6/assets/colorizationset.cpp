#include <std_include.hpp>
#include "colorizationset.hpp"

namespace zonetool::iw6
{
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

	ColorizationSet* colorization_set::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "colorization\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing colorization \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<ColorizationSet>();
		asset->name = mem->duplicate_string(name);

		PARSE_FIELD_ARR(lowAndHighEndPoints, 4);
		PARSE_FIELD_ARR(midEndPoints, 4);
		PARSE_FIELD_ARR(alternateDepthScaleAndBias, 2);

		PARSE_ASSET(colorization);
		PARSE_ASSET(colorizationAlternate);
		PARSE_ASSET(colorizationLow);
		PARSE_ASSET(colorizationMid);
		PARSE_ASSET(colorizationHigh);
		PARSE_ASSET(alternateMaskMaterial);

		if (data.contains("UseAlternateDepth") && data["UseAlternateDepth"].is_boolean() && data["UseAlternateDepth"].get<bool>())
		{
			asset->flags |= COLORIZATION_USE_ALTERNATE_DEPTH;
		}
		if (data.contains("UseAlternateMask") && data["UseAlternateMask"].is_boolean() && data["UseAlternateMask"].get<bool>())
		{
			asset->flags |= COLORIZATION_USE_ALTERNATE_MASK;
		}
		if (data.contains("UseLowMidHigh") && data["UseLowMidHigh"].is_boolean() && data["UseLowMidHigh"].get<bool>())
		{
			asset->flags |= COLORIZATION_USE_LOW_MID_HIGH;
		}

		return asset;
	}

	void colorization_set::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).colorizationSet;
		}
	}

	void colorization_set::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void colorization_set::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;
		if (asset->alternateMaskMaterial)
		{
			zone->add_asset_of_type(XAssetType::ASSET_TYPE_MATERIAL, asset->alternateMaskMaterial->name);
		}
	}

	std::string colorization_set::name()
	{
		return this->name_;
	}

	std::int32_t colorization_set::type()
	{
		return ASSET_TYPE_COLORIZATIONSET;
	}

#define WRITE_ASSET(__field__,__type__) \
	if (data->__field__) \
	{ \
		dest->__field__ = reinterpret_cast<typename std::remove_reference<decltype(dest->__field__)>::type>(zone->get_asset_pointer(__type__, data->__field__->name)); \
	}

	void colorization_set::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		WRITE_ASSET(alternateMaskMaterial, ASSET_TYPE_MATERIAL);

		buf->pop_stream();
	}

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

	void colorization_set::dump(ColorizationSet* asset)
	{
		const auto path = "colorization\\"s + asset->name + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_FIELD_ARR(lowAndHighEndPoints, 4);
		DUMP_FIELD_ARR(midEndPoints, 4);
		DUMP_FIELD_ARR(alternateDepthScaleAndBias, 2);
		DUMP_ASSET(alternateMaskMaterial);

		DUMP_ASSET(colorization);
		DUMP_ASSET(colorizationAlternate);
		DUMP_ASSET(colorizationLow);
		DUMP_ASSET(colorizationMid);
		DUMP_ASSET(colorizationHigh);
		DUMP_ASSET(alternateMaskMaterial);

		data["UseAlternateDepth"] = (asset->flags & COLORIZATION_USE_ALTERNATE_DEPTH) != 0;
		data["UseAlternateMask"] = (asset->flags & COLORIZATION_USE_ALTERNATE_MASK) != 0;
		data["UseLowMidHigh"] = (asset->flags & COLORIZATION_USE_LOW_MID_HIGH) != 0;

		auto str = data.dump(4);
		data.clear();

		file.write(str);
		file.close();
	}
}