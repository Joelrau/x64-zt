#include <std_include.hpp>
#include "gfxlightdef.hpp"

namespace zonetool::iw7
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

	GfxLightDef* gfx_light_def::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "lights\\"s + name + ".json";

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing lightdef \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto* asset = mem->allocate<GfxLightDef>();
		asset->name = mem->duplicate_string(name);

		PARSE_STRING(iesProfile);
		PARSE_FIELD(coordOffset);
		PARSE_FIELD(coordScale);

		PARSE_FIELD(unk0);
		PARSE_FIELD(unk1);
		PARSE_FIELD(unk2);

		return asset;
	}

	void gfx_light_def::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).lightDef;
		}
	}

	void gfx_light_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_light_def::load_depending(zone_base* zone)
	{
	}

	std::string gfx_light_def::name()
	{
		return this->name_;
	}

	std::int32_t gfx_light_def::type()
	{
		return ASSET_TYPE_LIGHT_DEF;
	}

	void gfx_light_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->iesProfile)
		{
			dest->iesProfile = buf->write_str(data->iesProfile);
		}

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

	void gfx_light_def::dump(GfxLightDef* asset)
	{
		const auto path = "lights\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_STRING(iesProfile);
		DUMP_FIELD(coordOffset);
		DUMP_FIELD(coordScale);

		DUMP_FIELD(unk0);
		DUMP_FIELD(unk1);
		DUMP_FIELD(unk2);

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}