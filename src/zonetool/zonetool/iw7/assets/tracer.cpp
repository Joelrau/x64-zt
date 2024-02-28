#include "std_include.hpp"
#include "tracer.hpp"

namespace zonetool::iw7
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

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

	TracerDef* tracer::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "tracer\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing reticle \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<TracerDef>();

		PARSE_STRING(name);
		PARSE_ASSET(material);
		PARSE_FIELD(fadeOverTime);
		PARSE_FIELD(fadeTime);
		PARSE_FIELD(speed);
		PARSE_FIELD(beamLength);
		PARSE_FIELD(beamWidth);
		PARSE_FIELD(screwRadius);
		PARSE_FIELD(screwDist);
		for (auto i = 0; i < 5; i++)
		{
			asset->colors[i][0] = data["colors"][i][0].get<float>();
			asset->colors[i][1] = data["colors"][i][1].get<float>();
			asset->colors[i][2] = data["colors"][i][2].get<float>();
			asset->colors[i][3] = data["colors"][i][3].get<float>();
		}

		return asset;
	}

	void tracer::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).tracerDef;
		}
	}

	void tracer::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void tracer::load_depending(zone_base* zone)
	{
	}

	std::string tracer::name()
	{
		return this->name_;
	}

	std::int32_t tracer::type()
	{
		return ASSET_TYPE_TRACER;
	}

	void tracer::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());
		if (data->material)
		{
			dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(type(), name()));
		}

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

	void tracer::dump(TracerDef* asset)
	{
		const auto path = "tracer\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_STRING(name);
		DUMP_ASSET(material);
		DUMP_FIELD(fadeOverTime);
		DUMP_FIELD(fadeTime);
		DUMP_FIELD(speed);
		DUMP_FIELD(beamLength);
		DUMP_FIELD(beamWidth);
		DUMP_FIELD(screwRadius);
		DUMP_FIELD(screwDist);
		for (auto i = 0; i < 5; i++)
		{
			data["colors"][i][0] = asset->colors[i][0];
			data["colors"][i][1] = asset->colors[i][1];
			data["colors"][i][2] = asset->colors[i][2];
			data["colors"][i][3] = asset->colors[i][3];
		}

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}