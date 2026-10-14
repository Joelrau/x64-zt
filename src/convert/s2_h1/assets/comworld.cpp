#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

#include <h1/assets/comworld.hpp>

namespace zonetool::s2::converter::h1::comworld
{
	namespace
	{
		namespace target = zonetool::h1;

		struct s2_primary_light
		{
			target::GfxLightType type;
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
			char __pad1[48];
			const char* defName;
		};
		static_assert(sizeof(s2_primary_light) == sizeof(ComPrimaryLight));
		static_assert(offsetof(s2_primary_light, color) == 0xC);
		static_assert(offsetof(s2_primary_light, origin) == 0x34);
		static_assert(offsetof(s2_primary_light, radius) == 0x58);
		static_assert(offsetof(s2_primary_light, defName) == 0xA0);

		// S2 stores a normalized color, H1 stores physical light units (stock mp_crash: sun about 34, spot lights 1.5e4 to 1.4e6)
		constexpr float sun_intensity_scale = 10.0f;
		constexpr float spot_intensity = 20000.0f;

		void convert_light(const s2_primary_light& light, target::ComPrimaryLight& new_light)
		{
			new_light.type = light.type;
			if (light.type == target::GFX_LIGHT_TYPE_NONE)
			{
				return;
			}

			const auto scale = light.type == target::GFX_LIGHT_TYPE_DIR ? light.intensity * sun_intensity_scale : spot_intensity;
			new_light.canUseShadowMap = light.canUseShadowMap;
			new_light.physicallyBased = light.type != target::GFX_LIGHT_TYPE_DIR;
			new_light.exponent = light.type != target::GFX_LIGHT_TYPE_DIR;
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

		target::ComWorld* convert(ComWorld* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<target::ComWorld>();
			new_asset->name = asset->name;
			new_asset->isInUse = 1;
			new_asset->primaryLightCount = asset->primaryLightCount;
			new_asset->primaryLights = allocator.allocate_array<target::ComPrimaryLight>(asset->primaryLightCount);
			const auto lights = reinterpret_cast<const s2_primary_light*>(asset->primaryLights);
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				convert_light(lights[i], new_asset->primaryLights[i]);
			}

			new_asset->primaryLightEnvCount = asset->primaryLightEnvCount;
			new_asset->primaryLightEnvs = reinterpret_cast<target::ComPrimaryLightEnv*>(asset->primaryLightEnvs);
			return new_asset;
		}
	}

	void dump(ComWorld* asset)
	{
		utils::memory::allocator allocator;
		const auto new_asset = convert(asset, allocator);
		target::com_world::dump(new_asset);
		add_csv_row("com_map", new_asset->name);
	}
}
