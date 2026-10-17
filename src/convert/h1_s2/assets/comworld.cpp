#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

namespace zonetool::h1::converter::s2::comworld
{
	namespace
	{
		struct s2_primary_light
		{
			std::uint8_t type;
			std::uint8_t canUseShadowMap;
			char __pad0[10];
			float color[3];
			float intensity;
			float dir[3];
			float up[3];
			float origin[3];
			float fadeOffset[2];
			float bulbRadius;
			float bulbLength[3];
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
			float cosHalfFovExpanded;
			float rotationLimit;
			float translationLimit;
			char __pad1[44];
			std::int32_t unk9C;
			const char* defName;
		};
		static_assert(sizeof(s2_primary_light) == sizeof(zonetool::s2::ComPrimaryLight));
		static_assert(offsetof(s2_primary_light, color) == 0xC);
		static_assert(offsetof(s2_primary_light, origin) == 0x34);
		static_assert(offsetof(s2_primary_light, radius) == 0x58);
		static_assert(offsetof(s2_primary_light, unk9C) == 0x9C);
		static_assert(offsetof(s2_primary_light, defName) == 0xA0);

		struct s2_light_env
		{
			std::uint16_t primaryLightIndices[4];
			std::uint8_t numIndices;
			char __pad0[1];
		};
		static_assert(sizeof(s2_light_env) == 10);

		// inverse of the S2 to H1 light conversion
		constexpr float sun_intensity_scale = 10.0f;
		constexpr float spot_intensity = 20000.0f;

		ComWorld* pending_world = nullptr;

		void convert_light(const ComPrimaryLight& light, s2_primary_light& new_light)
		{
			new_light.type = static_cast<std::uint8_t>(light.type);
			new_light.unk9C = -1;
			if (light.type == GFX_LIGHT_TYPE_NONE)
			{
				return;
			}

			const auto max_color = std::max({light.color[0], light.color[1], light.color[2]});
			const auto is_sun = light.type == GFX_LIGHT_TYPE_DIR;
			new_light.intensity = is_sun ? (max_color > 0.0f ? max_color / sun_intensity_scale : 1.0f) : 1.0f;
			const auto scale = is_sun ? new_light.intensity * sun_intensity_scale : spot_intensity;

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
			field<std::uint32_t>(new_asset, 8) = 1;
			field<std::uint32_t>(new_asset, 12) = 1;

			const auto count = asset->primaryLightCount;
			const auto lights = allocator.allocate_array<s2_primary_light>(count);
			for (auto i = 0u; i < count; i++)
			{
				convert_light(asset->primaryLights[i], lights[i]);			}

			// S2 envs hold one light each, the GfxWorld light lists hold the light sets
			const auto envs = allocator.allocate_array<s2_light_env>(count);
			for (auto i = 1u; i < count; i++)
			{
				envs[i].primaryLightIndices[0] = static_cast<std::uint16_t>(i);
				envs[i].numIndices = 1;
			}

			new_asset->primaryLightCount = count;
			new_asset->primaryLights = reinterpret_cast<zonetool::s2::ComPrimaryLight*>(lights);
			new_asset->primaryLightEnvCount = count;
			new_asset->primaryLightEnvs = reinterpret_cast<char(*)[10]>(envs);
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
