#include "std_include.hpp"
#include "rumble.hpp"

namespace zonetool::iw7
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define PARSE_ASSET(__field__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__ = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(sizeof(const char*)); \
		asset->__field__->name = mem->duplicate_string(data[#__field__].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}

	RumbleInfo* rumble::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "rumble\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing rumble \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<RumbleInfo>();

		PARSE_STRING(name);
		PARSE_FIELD(duration);
		PARSE_FIELD(range);
		PARSE_ASSET(highRumbleGraph);
		PARSE_ASSET(lowRumbleGraph);
		PARSE_FIELD(fadeWithDistance);
		PARSE_FIELD(broadcast);

		return asset;
	}

	void rumble::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).rumble;
		}
	}

	void rumble::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void rumble::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;
		if (asset->highRumbleGraph)
		{
			zone->add_asset_of_type(ASSET_TYPE_RUMBLE_GRAPH, asset->highRumbleGraph->name);
		}
		if (asset->lowRumbleGraph)
		{
			zone->add_asset_of_type(ASSET_TYPE_RUMBLE_GRAPH, asset->lowRumbleGraph->name);
		}
	}

	std::string rumble::name()
	{
		return this->name_;
	}

	std::int32_t rumble::type()
	{
		return ASSET_TYPE_RUMBLE;
	}

	void rumble::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->highRumbleGraph)
		{
			dest->highRumbleGraph = reinterpret_cast<RumbleGraph*>(
				zone->get_asset_pointer(ASSET_TYPE_RUMBLE_GRAPH, data->highRumbleGraph->name));
		}

		if (data->lowRumbleGraph)
		{
			dest->lowRumbleGraph = reinterpret_cast<RumbleGraph*>(
				zone->get_asset_pointer(ASSET_TYPE_RUMBLE_GRAPH, data->lowRumbleGraph->name));
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

	void rumble::dump(RumbleInfo* asset)
	{
		const auto path = "rumble\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_STRING(name);
		DUMP_FIELD(duration);
		DUMP_FIELD(range);
		DUMP_ASSET(highRumbleGraph);
		DUMP_ASSET(lowRumbleGraph);
		DUMP_FIELD(fadeWithDistance);
		DUMP_FIELD(broadcast);

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}