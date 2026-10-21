#include <std_include.hpp>
#include "comworld.hpp"

#include "io/json_reflect.hpp"

#define DUMP_JSON

namespace zonetool::h1
{
	namespace
	{
		constexpr auto com_world_fields = glz::tuple
		{
			&ComWorld::isInUse,
		};

		constexpr auto primary_light_fields = glz::tuple
		{
			&ComPrimaryLight::type,
			&ComPrimaryLight::canUseShadowMap,
			&ComPrimaryLight::physicallyBased,
			&ComPrimaryLight::exponent,
			&ComPrimaryLight::lightingState,
			&ComPrimaryLight::color,
			&ComPrimaryLight::dir,
			&ComPrimaryLight::up,
			&ComPrimaryLight::origin,
			&ComPrimaryLight::fadeOffset,
			&ComPrimaryLight::bulbRadius,
			&ComPrimaryLight::bulbLength,
			&ComPrimaryLight::radius,
			&ComPrimaryLight::cosHalfFovOuter,
			&ComPrimaryLight::cosHalfFovInner,
			&ComPrimaryLight::cosHalfFovExpanded,
			&ComPrimaryLight::rotationLimit,
			&ComPrimaryLight::translationLimit,
			&ComPrimaryLight::cucRotationOffsetRad,
			&ComPrimaryLight::cucRotationSpeedRad,
			&ComPrimaryLight::cucScrollVector,
			&ComPrimaryLight::cucScaleVector,
			&ComPrimaryLight::cucTransVector,
			&ComPrimaryLight::defName,
		};

		constexpr auto primary_light_env_fields = glz::tuple
		{
			&ComPrimaryLightEnv::primaryLightIndices,
			&ComPrimaryLightEnv::numIndices,
		};
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

		json_reflect::read<com_world_fields>(data, *asset, mem);

		asset->primaryLightCount = static_cast<unsigned int>(data["primaryLight"].size());
		asset->primaryLights = mem->allocate<ComPrimaryLight>(asset->primaryLightCount);
		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			json_reflect::read<primary_light_fields>(data["primaryLight"][i], asset->primaryLights[i], mem);
		}

		asset->primaryLightEnvCount = static_cast<unsigned int>(data["primaryLightEnv"].size());
		asset->primaryLightEnvs = mem->allocate<ComPrimaryLightEnv>(asset->primaryLightEnvCount);
		for (auto i = 0u; i < asset->primaryLightEnvCount; i++)
		{
			json_reflect::read<primary_light_env_fields>(data["primaryLightEnv"][i], asset->primaryLightEnvs[i], mem);
		}

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

		read.close();

		return asset;
	}

	void com_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
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
		if (const auto lights = reader.read_array(asset->primaryLights, 3, asset->primaryLightCount))
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				reader.read_string(lights[i].defName);
			}
		}
		reader.read_array(asset->primaryLightEnvs, 3, asset->primaryLightEnvCount);
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
			buf->align(3);
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

		buf->pop_stream();
	}

	void dump_json(ComWorld* asset)
	{
		const auto path = asset->name + ".commap"s + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		json_reflect::write<com_world_fields>(data, *asset);

		data["primaryLight"] = {};
		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			data["primaryLight"][i]["primaryLightIndex"] = i;
			json_reflect::write<primary_light_fields>(data["primaryLight"][i], asset->primaryLights[i]);
		}

		data["primaryLightEnv"] = {};
		for (auto i = 0u; i < asset->primaryLightEnvCount; i++)
		{
			data["primaryLightEnv"][i]["envIndex"] = i;
			json_reflect::write<primary_light_env_fields>(data["primaryLightEnv"][i], asset->primaryLightEnvs[i]);
		}

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

		write.close();
#endif
	}
}
