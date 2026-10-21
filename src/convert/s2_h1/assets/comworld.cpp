#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

#include <h1/assets/comworld.hpp>

namespace zonetool::s2::converter::h1::comworld
{
	namespace
	{
		void convert_light(const ComPrimaryLight& light, zonetool::h1::ComPrimaryLight& new_light)
		{
			new_light.type = static_cast<zonetool::h1::GfxLightType>(light.type);
			if (light.type == zonetool::h1::GFX_LIGHT_TYPE_NONE)
			{
				return;
			}

			const auto scale = light.type == zonetool::h1::GFX_LIGHT_TYPE_DIR ? light.intensity * 10.0f : 20000.0f;

			new_light.canUseShadowMap = 0;
			new_light.physicallyBased = light.type != zonetool::h1::GFX_LIGHT_TYPE_DIR;
			new_light.exponent = light.type != zonetool::h1::GFX_LIGHT_TYPE_DIR;
			for (auto i = 0; i < 3; i++)
			{
				new_light.color[i] = light.color[i] * scale;
				new_light.dir[i] = light.dir[i];
				new_light.up[i] = light.up[i];
				new_light.origin[i] = light.origin[i];
				new_light.bulbLength[i] = light.bulbLength[i];
			}

			new_light.fadeOffset[0] = light.fadeOffset[0];
			new_light.fadeOffset[1] = light.fadeOffset[1];
			new_light.bulbRadius = light.bulbRadius;
			new_light.radius = light.radius;
			new_light.cosHalfFovOuter = light.cosHalfFovOuter;
			new_light.cosHalfFovInner = light.cosHalfFovInner;
			new_light.cosHalfFovExpanded = light.cosHalfFovExpanded;
			new_light.rotationLimit = light.rotationLimit;
			new_light.translationLimit = light.translationLimit;
			new_light.defName = light.defName;
		}

		constexpr std::size_t max_env_lights = 4;

		ComWorld* pending_world = nullptr;
		std::vector<zonetool::h1::ComPrimaryLightEnv> envs;
		std::map<std::vector<std::uint16_t>, std::uint16_t> env_lookup;

		std::vector<std::uint16_t> get_env_lights(const zonetool::h1::ComPrimaryLightEnv& env)
		{
			return {env.primaryLightIndices, env.primaryLightIndices + std::min<std::size_t>(env.numIndices, max_env_lights)};
		}

		zonetool::h1::ComWorld* convert(ComWorld* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::h1::ComWorld>();
			new_asset->name = asset->name;
			new_asset->isInUse = 1;
			new_asset->primaryLightCount = asset->primaryLightCount;
			new_asset->primaryLights = allocator.allocate_array<zonetool::h1::ComPrimaryLight>(asset->primaryLightCount);
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				convert_light(asset->primaryLights[i], new_asset->primaryLights[i]);
			}

			auto final_envs = envs;
			final_envs.push_back({{2047}, 1});
			new_asset->primaryLightEnvCount = static_cast<unsigned int>(final_envs.size());
			new_asset->primaryLightEnvs = allocator.allocate_array<zonetool::h1::ComPrimaryLightEnv>(final_envs.size());
			std::memcpy(new_asset->primaryLightEnvs, final_envs.data(), final_envs.size() * sizeof(zonetool::h1::ComPrimaryLightEnv));
			return new_asset;
		}
	}

	void dump(ComWorld* asset)
	{
		pending_world = asset;
		envs.assign(reinterpret_cast<const zonetool::h1::ComPrimaryLightEnv*>(asset->primaryLightEnvs),
			reinterpret_cast<const zonetool::h1::ComPrimaryLightEnv*>(asset->primaryLightEnvs) + asset->primaryLightEnvCount);
		env_lookup.clear();
		for (auto i = 0u; i < envs.size(); i++)
		{
			env_lookup.try_emplace(get_env_lights(envs[i]), static_cast<std::uint16_t>(i));
		}
	}

	std::uint16_t get_light_env(std::vector<std::uint16_t> lights)
	{
		if (lights.size() > max_env_lights)
		{
			lights.resize(max_env_lights);
		}

		if (const auto itr = env_lookup.find(lights); itr != env_lookup.end())
		{
			return itr->second;
		}

		zonetool::h1::ComPrimaryLightEnv env{};
		std::copy(lights.begin(), lights.end(), env.primaryLightIndices);
		env.numIndices = static_cast<unsigned char>(lights.size());
		const auto index = static_cast<std::uint16_t>(envs.size());
		envs.push_back(env);
		env_lookup.emplace(std::move(lights), index);
		return index;
	}

	void finish()
	{
		if (!pending_world)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto new_asset = convert(pending_world, allocator);
		zonetool::h1::com_world::dump(new_asset);
		add_csv_row("com_map", new_asset->name);
		pending_world = nullptr;
		envs.clear();
		env_lookup.clear();
	}
}
