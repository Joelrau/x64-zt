#include <std_include.hpp>
#include "tonemapping.hpp"

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

	ToneMapping* tonemapping::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "tonemapping\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing tonemapping \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<ToneMapping>();

		PARSE_FIELD_ARR(toeStrength, 3);
		PARSE_FIELD_ARR(toeAngleNumerator, 3);
		PARSE_FIELD_ARR(toeAngleDenominator, 3);
		PARSE_FIELD_ARR(linearStrength, 3);
		PARSE_FIELD_ARR(linearAngle, 3);
		PARSE_FIELD_ARR(shoulderStrength, 3);
		PARSE_FIELD_ARR(whitePoint, 3);
		PARSE_FIELD_ARR(exposure, 3);
		PARSE_FIELD_ARR(adaptationRates, 2);

		return asset;
	}

	void tonemapping::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).toneMapping;
		}
	}

	void tonemapping::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void tonemapping::load_depending(zone_base* zone)
	{
	}

	std::string tonemapping::name()
	{
		return this->name_;
	}

	std::int32_t tonemapping::type()
	{
		return ASSET_TYPE_TONEMAPPING;
	}

	void tonemapping::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

#define DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

	void tonemapping::dump(ToneMapping* asset)
	{
		const auto path = "tonemapping\\"s + asset->name + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_FIELD_ARR(toeStrength, 3);
		DUMP_FIELD_ARR(toeAngleNumerator, 3);
		DUMP_FIELD_ARR(toeAngleDenominator, 3);
		DUMP_FIELD_ARR(linearStrength, 3);
		DUMP_FIELD_ARR(linearAngle, 3);
		DUMP_FIELD_ARR(shoulderStrength, 3);
		DUMP_FIELD_ARR(whitePoint, 3);
		DUMP_FIELD_ARR(exposure, 3);
		DUMP_FIELD_ARR(adaptationRates, 2);

		auto str = data.dump(4);
		data.clear();

		file.write(str);
		file.close();
	}
}