#include <std_include.hpp>
#include "comworld.hpp"

#define DUMP_JSON

namespace zonetool::iw7
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

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

	void parse(ComPrimaryLight* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD(type);
		PARSE_FIELD(canUseShadowMap);
		PARSE_FIELD(needsDynamicShadows);
		PARSE_FIELD(isVolumetric);
		PARSE_FIELD(exponent);
		PARSE_FIELD(transientZoneList);
		PARSE_FIELD(entityId);
		PARSE_FIELD(uvIntensity);
		PARSE_FIELD(irIntensity);
		PARSE_FIELD_ARR(color, 3);
		PARSE_FIELD_ARR(dir, 3);
		PARSE_FIELD_ARR(up, 3);
		PARSE_FIELD_ARR(origin, 3);
		PARSE_FIELD(radius);
		PARSE_FIELD_ARR(fadeOffsetRt, 2);
		PARSE_FIELD(bulbRadius);
		PARSE_FIELD_ARR(bulbLength, 3);
		PARSE_FIELD(cosHalfFovOuter);
		PARSE_FIELD(cosHalfFovInner);
		PARSE_FIELD(shadowSoftness);
		PARSE_FIELD(shadowBias);
		PARSE_FIELD(shadowArea);
		PARSE_FIELD(distanceFalloff);
		PARSE_FIELD(rotationLimit);
		PARSE_FIELD(translationLimit);
		PARSE_STRING(defName);
	}

	void parse(ComPrimaryLightEnv* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FIELD_ARR(primaryLightIndices, 4);
		PARSE_FIELD(numIndices);
	}

	ComWorld* parse_json(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".commap"s + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing commap json \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<ComWorld>();
		asset->name = mem->duplicate_string(name);

		PARSE_FIELD(isInUse);
		PARSE_FIELD(useForwardPlus);
		PARSE_FIELD(bakeQuality);

		asset->primaryLightCount = static_cast<unsigned int>(data["primaryLight"].size());
		asset->primaryLights = mem->allocate<ComPrimaryLight>(asset->primaryLightCount);
		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			parse(&asset->primaryLights[i], data["primaryLight"][i], mem);
		}

		asset->primaryLightEnvCount = static_cast<unsigned int>(data["primaryLightEnv"].size());
		asset->primaryLightEnvs = mem->allocate<ComPrimaryLightEnv>(asset->primaryLightEnvCount);
		for (auto i = 0u; i < asset->primaryLightEnvCount; i++)
		{
			parse(&asset->primaryLightEnvs[i], data["primaryLightEnv"][i], mem);
		}

		PARSE_FIELD(scriptablePrimaryLightCount);
		PARSE_FIELD(firstScriptablePrimaryLight);
		PARSE_FIELD(changeListInfo.changeListNumber);
		PARSE_FIELD(changeListInfo.time);
		PARSE_STRING(changeListInfo.userName);

		asset->numUmbraGates = static_cast<unsigned int>(data["umbraGateNames"].size());
		asset->umbraGateNames = mem->allocate<const char*>(asset->numUmbraGates);
		for (auto i = 0u; i < asset->numUmbraGates; i++)
		{
			asset->umbraGateNames[i] = mem->duplicate_string(data["umbraGateNames"][i].get<std::string>());
		}
		PARSE_FIELD_ARR(umbraGateInitialStates, 4);

		return asset;
	}

	ComWorld* com_world::parse(const std::string& name, zone_memory* mem)
	{
		if (auto* asset = parse_json(name, mem))
		{
			return asset;
		}

		const auto path = name + ".commap"s;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing commap \"%s\"...", name.data());

		ComWorld* asset = read.read_single<ComWorld>();
		asset->name = read.read_string();

		asset->primaryLights = read.read_array<ComPrimaryLight>();
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			asset->primaryLights[i].defName = read.read_string();
		}
		asset->primaryLightEnvs = read.read_array<ComPrimaryLightEnv>();

		asset->changeListInfo.userName = read.read_string();

		asset->umbraGateNames = mem->allocate<const char*>(asset->numUmbraGates);
		for (unsigned int i = 0; i < asset->numUmbraGates; i++)
		{
			asset->umbraGateNames[i] = read.read_string();
		}

		read.close();

		return asset;
	}

	void com_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse commap \"%s\"", name.data());
		}
	}

	void com_world::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void com_world::load_depending(zone_base* zone)
	{
		auto asset = this->asset_;

		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			if (asset->primaryLights[i].defName)
			{
				zone->add_asset_of_type(ASSET_TYPE_LIGHT_DEF, asset->primaryLights[i].defName);
			}
		}
	}

	std::string com_world::name()
	{
		return this->name_;
	}

	std::int32_t com_world::type()
	{
		return ASSET_TYPE_COMWORLD;
	}

	void com_world::read(zone_reader& reader, ComWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto lights = reader.read_array(asset->primaryLights, 7, asset->primaryLightCount))
		{
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				reader.read_string(lights[i].defName);
			}
		}
		reader.read_array(asset->primaryLightEnvs, 3, asset->primaryLightEnvCount);
		reader.read_string(asset->changeListInfo.userName);
		if (const auto names = reader.read_array(asset->umbraGateNames, 7, asset->numUmbraGates))
		{
			for (auto i = 0u; i < asset->numUmbraGates; i++)
			{
				reader.read_string(names[i]);
			}
		}
		reader.pop_stream();
	}

	void com_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->primaryLights)
		{
			buf->align(7);
			auto* primary_light = buf->write(data->primaryLights, data->primaryLightCount);

			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->primaryLights[i].defName)
				{
					primary_light[i].defName = buf->write_str(data->primaryLights[i].defName);
				}
			}

			buf->clear_pointer(&dest->primaryLights);
		}

		if (data->primaryLightEnvs)
		{
			buf->align(3);
			buf->write(data->primaryLightEnvs, data->primaryLightEnvCount);
			buf->clear_pointer(&dest->primaryLightEnvs);
		}

		if (data->changeListInfo.userName)
		{
			dest->changeListInfo.userName = buf->write_str(data->changeListInfo.userName);
		}

		if (data->umbraGateNames)
		{
			buf->align(7);
			dest->umbraGateNames = buf->write(data->umbraGateNames, data->numUmbraGates);

			for (unsigned int i = 0; i < data->numUmbraGates; i++)
			{
				dest->umbraGateNames[i] = buf->write_str(data->umbraGateNames[i]);
			}

			buf->clear_pointer(&dest->umbraGateNames);
		}
		

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

	void dump(ComPrimaryLight* asset, ordered_json& data, std::uint32_t index)
	{
		data["primaryLightIndex"] = index;
		DUMP_FIELD(type);
		DUMP_FIELD(canUseShadowMap);
		DUMP_FIELD(needsDynamicShadows);
		DUMP_FIELD(isVolumetric);
		DUMP_FIELD(exponent);
		DUMP_FIELD(transientZoneList);
		DUMP_FIELD(entityId);
		DUMP_FIELD(uvIntensity);
		DUMP_FIELD(irIntensity);
		DUMP_FIELD_ARR(color, 3);
		DUMP_FIELD_ARR(dir, 3);
		DUMP_FIELD_ARR(up, 3);
		DUMP_FIELD_ARR(origin, 3);
		DUMP_FIELD(radius);
		DUMP_FIELD_ARR(fadeOffsetRt, 2);
		DUMP_FIELD(bulbRadius);
		DUMP_FIELD_ARR(bulbLength, 3);
		DUMP_FIELD(cosHalfFovOuter);
		DUMP_FIELD(cosHalfFovInner);
		DUMP_FIELD(shadowSoftness);
		DUMP_FIELD(shadowBias);
		DUMP_FIELD(shadowArea);
		DUMP_FIELD(distanceFalloff);
		DUMP_FIELD(rotationLimit);
		DUMP_FIELD(translationLimit);
		DUMP_STRING(defName);
	}

	void dump(ComPrimaryLightEnv* asset, ordered_json& data, std::uint32_t index)
	{
		data["envIndex"] = index;
		DUMP_FIELD_ARR(primaryLightIndices, 4);
		DUMP_FIELD(numIndices);
	}

	void dump_json(ComWorld* asset)
	{
		const auto path = asset->name + ".commap"s + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_FIELD(isInUse);
		DUMP_FIELD(useForwardPlus);
		DUMP_FIELD(bakeQuality);

		data["primaryLight"] = {};
		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			dump(&asset->primaryLights[i], data["primaryLight"][i], i);
		}

		data["primaryLightEnv"] = {};
		for (auto i = 0u; i < asset->primaryLightEnvCount; i++)
		{
			dump(&asset->primaryLightEnvs[i], data["primaryLightEnv"][i], i);
		}

		DUMP_FIELD(scriptablePrimaryLightCount);
		DUMP_FIELD(firstScriptablePrimaryLight);
		DUMP_FIELD(changeListInfo.changeListNumber);
		DUMP_FIELD(changeListInfo.time);
		DUMP_STRING(changeListInfo.userName);

		data["umbraGateNames"] = {};
		for (auto i = 0u; i < asset->numUmbraGates; i++)
		{
			data["umbraGateNames"][i] = asset->umbraGateNames[i];
		}
		DUMP_FIELD_ARR(umbraGateInitialStates, 4);

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}

	void com_world::dump(ComWorld* asset)
	{
#ifdef DUMP_JSON
		dump_json(asset);
		return;
#else
		const auto path = asset->name + ".commap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->primaryLights, asset->primaryLightCount);
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			write.dump_string(asset->primaryLights[i].defName);
		}
		write.dump_array(asset->primaryLightEnvs, asset->primaryLightEnvCount);

		write.dump_string(asset->changeListInfo.userName);

		for (unsigned int i = 0; i < asset->numUmbraGates; i++)
		{
			write.dump_string(asset->umbraGateNames[i]);
		}

		write.close();
#endif
	}
}
