#include <std_include.hpp>
#include "s2.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::s2
{
	namespace
	{
		// glaze miscounts aggregate members with C arrays, so the light structs need explicit lists
		constexpr auto com_world_fields = glz::tuple
		{
			&ComWorld::isInUse,
			&ComWorld::unk_0xC,
		};

		constexpr auto primary_light_fields = glz::tuple
		{
			&ComPrimaryLight::type,
			&ComPrimaryLight::canUseShadowMap,
			&ComPrimaryLight::exponent,
			&ComPrimaryLight::targetnameHash,
			&ComPrimaryLight::unk_0x8,
			&ComPrimaryLight::color,
			&ComPrimaryLight::intensity,
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
			&ComPrimaryLight::airLightOffset,
			&ComPrimaryLight::airLightIntensityScale,
			&ComPrimaryLight::airLightRadiusScale,
			&ComPrimaryLight::airLightDensityScale,
			&ComPrimaryLight::airLightSourceRadius,
			&ComPrimaryLight::airLightVertexInnerMutedRadius,
			&ComPrimaryLight::airLightMaxDistanceSq,
			&ComPrimaryLight::airLightExponent,
			&ComPrimaryLight::airLightHighQuality,
			&ComPrimaryLight::iesProfileIndex,
			&ComPrimaryLight::defName,
		};

		constexpr auto primary_light_env_fields = glz::tuple
		{
			&ComPrimaryLightEnv::primaryLightIndices,
			&ComPrimaryLightEnv::numIndices,
		};

		std::string json_path(const char* name)
		{
			return dump_path(ASSET_TYPE_COMWORLD, name, ".json");
		}

		ComWorld* parse_json(const std::string& name, zone_memory* mem)
		{
			auto file = filesystem::file(json_path(name.data()));
			if (!file.exists())
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing comworld json \"%s\"...", name.data());

			file.open("rb");
			const auto data = json::parse(file.read_bytes(file.size()));
			file.close();

			auto* asset = mem->allocate<ComWorld>();
			asset->name = mem->duplicate_string(name);
			json_reflect::read<com_world_fields>(data, *asset, mem);

			const auto& lights = data["primaryLights"];
			asset->primaryLightCount = static_cast<unsigned int>(lights.size());
			asset->primaryLights = asset->primaryLightCount ? mem->allocate<ComPrimaryLight>(asset->primaryLightCount) : nullptr;
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				auto& light = asset->primaryLights[i];
				json_reflect::read<primary_light_fields>(lights[i], light, mem, {.strings = json_reflect::null_string::null});
			}

			json_reflect::read_array<primary_light_env_fields>(data["primaryLightEnvs"], asset->primaryLightEnvs, asset->primaryLightEnvCount, mem);
			return asset;
		}

		void dump_json(ComWorld* asset)
		{
			ordered_json data;
			json_reflect::write<com_world_fields>(data, *asset);

			auto& lights = data["primaryLights"] = ordered_json::array();
			for (auto i = 0u; asset->primaryLights && i < asset->primaryLightCount; i++)
			{
				const auto& light = asset->primaryLights[i];
				auto& entry = lights.emplace_back();
				entry["index"] = i; // informational, the array order is the light index
				json_reflect::write<primary_light_fields>(entry, light, {.strings = json_reflect::null_string::null});
			}

			json_reflect::write_array<primary_light_env_fields>(data["primaryLightEnvs"], asset->primaryLightEnvs, asset->primaryLightEnvCount);

			const auto str = data.dump(4);
			auto file = filesystem::file(json_path(asset->name));
			file.open("wb");
			file.write(str.data(), str.size(), 1);
			file.close();
		}
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
		reader.read_array(asset->primaryLightEnvs, 1, asset->primaryLightEnvCount);
		reader.pop_stream();
	}

	ComWorld* com_world::parse(const std::string& name, zone_memory* mem)
	{
		if (auto* asset = parse_json(name, mem))
		{
			return asset;
		}

		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_COMWORLD, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing comworld \"%s\"...", name.data());

		auto* asset = read.read_single<ComWorld>();
		asset->name = read.read_string();
		if (const auto lights = parse_array(read, mem, asset->primaryLights, asset->primaryLightCount))
		{
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				lights[i].defName = read.read_string();
			}
		}
		parse_array(read, mem, asset->primaryLightEnvs, asset->primaryLightEnvCount);
		read.close();

		return asset;
	}

	void com_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		if (const auto dest_lights = write_array(buf, 7, data->primaryLights, data->primaryLightCount, dest->primaryLights))
		{
			for (auto i = 0u; i < data->primaryLightCount; i++)
			{
				write_string(buf, data->primaryLights[i].defName, dest_lights[i].defName);
			}
		}
		write_array(buf, 1, data->primaryLightEnvs, data->primaryLightEnvCount, dest->primaryLightEnvs);

		buf->pop_stream();
	}

	void com_world::dump(ComWorld* asset)
	{
		dump_json(asset);
	}
}
