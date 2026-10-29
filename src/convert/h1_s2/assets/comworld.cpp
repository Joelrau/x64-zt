#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

namespace zonetool::h1::converter::s2::comworld
{
	namespace
	{
		ComWorld* pending_world = nullptr;
		constexpr auto omni_shadow_cos_outer = 0.17364818f; // cos 80 deg
		constexpr auto omni_shadow_cos_inner = 0.34202015f; // cos 70 deg

		void convert_up(const ComPrimaryLight& light, float* up)
		{
			// s2 builds the shadow view from dir and up, a light without an up gets the stock 0 1 0 (1 0 0 along y)
			const float cross[3]{light.dir[1] * light.up[2] - light.dir[2] * light.up[1], light.dir[2] * light.up[0] - light.dir[0] * light.up[2],
				light.dir[0] * light.up[1] - light.dir[1] * light.up[0]};
			const auto local = light.type == GFX_LIGHT_TYPE_SPOT || light.type == GFX_LIGHT_TYPE_OMNI;
			const auto degenerate = local && cross[0] * cross[0] + cross[1] * cross[1] + cross[2] * cross[2] < 1e-6f;
			const auto along_y = std::abs(light.dir[1]) > 0.99f;
			for (auto i = 0; i < 3; i++)
			{
				up[i] = !degenerate ? light.up[i] : (i == (along_y ? 0 : 1) ? 1.0f : 0.0f);
			}
		}

		void convert_light(const ComPrimaryLight& light, zonetool::s2::ComPrimaryLight& new_light)
		{
			new_light.type = static_cast<std::uint8_t>(light.type);
			new_light.iesProfileIndex = -1; // h1 lights have no ies profile
			if (light.type == GFX_LIGHT_TYPE_NONE)
			{
				return;
			}

			// s2 renders color * intensity, h1 treats the max color channel as the intensity
			const auto max_color = std::max({light.color[0], light.color[1], light.color[2]});
			new_light.intensity = max_color > 0.0f ? max_color : 1.0f;

			// s2 keeps lights out of the rooms behind walls only with shadows, like the stock spots and omnis
			new_light.canUseShadowMap = light.type == GFX_LIGHT_TYPE_SPOT || light.type == GFX_LIGHT_TYPE_OMNI ? 1 : light.canUseShadowMap;
			new_light.exponent = light.exponent;
			for (auto i = 0; i < 3; i++)
			{
				new_light.color[i] = light.color[i] / new_light.intensity;
				new_light.dir[i] = light.dir[i];
				new_light.origin[i] = light.origin[i];
				new_light.bulbLength[i] = light.bulbLength[i];
			}

			convert_up(light, new_light.up);
			new_light.fadeOffset[0] = light.fadeOffset[0];
			new_light.fadeOffset[1] = light.fadeOffset[1];
			new_light.bulbRadius = light.bulbRadius;
			new_light.radius = light.radius;
			// s2 shadows an omni through one frustum sized by these cones, h1 omnis never read them
			const auto omni = light.type == GFX_LIGHT_TYPE_OMNI;
			new_light.cosHalfFovOuter = omni ? omni_shadow_cos_outer : light.cosHalfFovOuter;
			new_light.cosHalfFovInner = omni ? omni_shadow_cos_inner : light.cosHalfFovInner;
			new_light.cosHalfFovExpanded = omni ? omni_shadow_cos_outer : light.cosHalfFovExpanded;
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
