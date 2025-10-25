#include <std_include.hpp>
#include "colorization.hpp"

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

	Colorization* colorization::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "colorization\\color\\"s + name + ".json"s;
		
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

		auto asset = mem->allocate<Colorization>();
		asset->name = mem->duplicate_string(name);

		PARSE_FIELD_ARR(powerAndDesaturation, 4);
		PARSE_FIELD_ARR(scale, 3);
		PARSE_FIELD_ARR(bias, 3);

		return asset;
	}

	void colorization::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).colorization;
		}
	}

	void colorization::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void colorization::load_depending(zone_base* zone)
	{
	}

	std::string colorization::name()
	{
		return this->name_;
	}

	std::int32_t colorization::type()
	{
		return ASSET_TYPE_COLORIZATION;
	}

	void colorization::write(zone_base* zone, zone_buffer* buf)
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

	void colorization::dump(Colorization* asset)
	{
		const auto path = "colorization\\color\\"s + asset->name + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_FIELD_ARR(powerAndDesaturation, 4);
		DUMP_FIELD_ARR(scale, 3);
		DUMP_FIELD_ARR(bias, 3);

		auto str = data.dump(4);
		data.clear();

		file.write(str);
		file.close();
	}
}