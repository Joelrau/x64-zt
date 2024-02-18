#include "std_include.hpp"
#include "reticle.hpp"

namespace zonetool::iw7
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

	ReticleDef* reticle::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "reticle\\"s + name + ".json"s;

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

		auto asset = mem->allocate<ReticleDef>();

		PARSE_STRING(name);
		PARSE_STRING(widgetName);
		PARSE_FIELD(lockReticleToTag);
		PARSE_FIELD(primaryAffectedByEMP);
		PARSE_FIELD(secondaryAffectedByEMP);
		PARSE_FIELD(scaleWithFOV);
		PARSE_FIELD(naturalDistance);
		PARSE_FIELD(minReticleScale);
		PARSE_FIELD(maxReticleScale);

		return asset;
	}

	void reticle::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).reticleDef;
		}
	}

	void reticle::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void reticle::load_depending(zone_base* zone)
	{
	}

	std::string reticle::name()
	{
		return this->name_;
	}

	std::int32_t reticle::type()
	{
		return ASSET_TYPE_RETICLE;
	}

	void reticle::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());
		if (data->widgetName)
		{
			dest->widgetName = buf->write_str(data->widgetName);
		}

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

	void reticle::dump(ReticleDef* asset)
	{
		const auto path = "reticle\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_STRING(name);
		DUMP_STRING(widgetName);
		DUMP_FIELD(lockReticleToTag);
		DUMP_FIELD(primaryAffectedByEMP);
		DUMP_FIELD(secondaryAffectedByEMP);
		DUMP_FIELD(scaleWithFOV);
		DUMP_FIELD(naturalDistance);
		DUMP_FIELD(minReticleScale);
		DUMP_FIELD(maxReticleScale);

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}