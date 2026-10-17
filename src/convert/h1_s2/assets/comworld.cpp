#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

namespace zonetool::h1::converter::s2::comworld
{
	namespace
	{
		ComWorld* pending_world = nullptr;

		void convert_light(const ComPrimaryLight& light, zonetool::s2::ComPrimaryLight& new_light)
		{
			new_light.type = static_cast<std::uint8_t>(light.type);
			new_light.unk_0x9C = -1;
			if (light.type == GFX_LIGHT_TYPE_NONE)
			{
				return;
			}

			const auto max_color = std::max({light.color[0], light.color[1], light.color[2]});
			const auto is_sun = light.type == GFX_LIGHT_TYPE_DIR;
			new_light.intensity = is_sun ? (max_color > 0.0f ? max_color / 10.0f : 1.0f) : 1.0f;
			const auto scale = is_sun ? new_light.intensity * 10.0f : 20000.0f;

			new_light.canUseShadowMap = light.canUseShadowMap;
			for (auto i = 0; i < 3; i++)
			{
				new_light.color[i] = light.color[i] / scale;
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

		zonetool::s2::ComWorld* convert(ComWorld* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::ComWorld>();
			new_asset->name = asset->name;
			new_asset->isInUse = 1;
			new_asset->unk_0xC = 1;

			const auto count = asset->primaryLightCount;
			const auto lights = allocator.allocate_array<zonetool::s2::ComPrimaryLight>(count);
			for (auto i = 0u; i < count; i++)
			{
				convert_light(asset->primaryLights[i], lights[i]);
			}

			const auto envs = allocator.allocate_array<zonetool::s2::ComPrimaryLightEnv>(count);
			for (auto i = 1u; i < count; i++)
			{
				envs[i].primaryLightIndices[0] = static_cast<std::uint16_t>(i);
				envs[i].numIndices = 1;
			}

			new_asset->primaryLightCount = count;
			new_asset->primaryLights = lights;
			new_asset->primaryLightEnvCount = count;
			new_asset->primaryLightEnvs = envs;
			return new_asset;
		}
	}

	void dump(ComWorld* asset)
	{
		pending_world = asset;
	}

	const ComWorld* source_world()
	{
		return pending_world;
	}

	void finish()
	{
		if (!pending_world)
		{
			return;
		}

		utils::memory::allocator allocator;
		zonetool::s2::com_world::dump(convert(pending_world, allocator));
		add_csv_row("com_map", pending_world->name);
	}

	void clear()
	{
		pending_world = nullptr;
	}
}
