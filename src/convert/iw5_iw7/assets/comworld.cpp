#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

#include <iw7/assets/comworld.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace comworld
	{
		namespace
		{
			namespace target = zonetool::iw7;

			void convert_color_to_physically_based(float* color)
			{
				const auto intensity = std::max({color[0], color[1], color[2]});
				if (intensity <= 0.0f)
				{
					return;
				}

				const auto scale = intensity * 1000.0f * 1550.0f * 0.001f;
				for (auto i = 0; i < 3; i++)
				{
					color[i] = color[i] / intensity * scale;
				}
			}

			target::GfxLightType convert_light_type(const unsigned char type)
			{
				switch (type)
				{
				case 2: return target::GFX_LIGHT_TYPE_OMNI;
				case 3: return target::GFX_LIGHT_TYPE_SPOT;
				default: return static_cast<target::GfxLightType>(type);
				}
			}

			target::ComPrimaryLight* convert_primary_lights(zonetool::iw5::ComPrimaryLight* lights, const unsigned int count,
				utils::memory::allocator& allocator)
			{
				const auto new_lights = allocator.allocate_array<target::ComPrimaryLight>(count);

				for (auto i = 0u; i < count; i++)
				{
					const auto& light = lights[i];
					auto& new_light = new_lights[i];

					new_light.type = convert_light_type(light.type);
					new_light.canUseShadowMap = light.canUseShadowMap;
					new_light.exponent = light.exponent;
					std::memcpy(new_light.color, light.color, sizeof(light.color));
					std::memcpy(new_light.dir, light.dir, sizeof(light.dir));
					std::memcpy(new_light.up, light.up, sizeof(light.up));
					std::memcpy(new_light.origin, light.origin, sizeof(light.origin));
					new_light.radius = light.radius;
					new_light.cosHalfFovOuter = light.cosHalfFovOuter;
					new_light.cosHalfFovInner = light.cosHalfFovInner;
					new_light.rotationLimit = light.rotationLimit;
					new_light.translationLimit = light.translationLimit;
					new_light.defName = light.defName.get();

					if (new_light.type == target::GFX_LIGHT_TYPE_SPOT || new_light.type == target::GFX_LIGHT_TYPE_OMNI)
					{
						new_light.entityId = new_light.type == target::GFX_LIGHT_TYPE_SPOT ? 739795875 : 1731316930;
						new_light.canUseShadowMap = new_light.type == target::GFX_LIGHT_TYPE_SPOT;
						new_light.needsDynamicShadows = 1;
						new_light.bulbRadius = 3.0f;
						new_light.shadowSoftness = 0.55f;
						new_light.shadowBias = 0.4f;
						new_light.shadowArea = 0.00179f;
						new_light.distanceFalloff = 0.2f;

						convert_color_to_physically_based(new_light.color);

						if (!new_light.defName)
						{
							new_light.defName = allocator.duplicate_string("light_point_linear");
						}
					}
				}

				return new_lights;
			}

			std::vector<light_reach> primary_lights;
		}

		const std::vector<light_reach>& last_primary_lights()
		{
			return primary_lights;
		}

		target::ComWorld* convert(zonetool::iw5::ComWorld* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<target::ComWorld>();

			new_asset->name = asset->name.get();
			new_asset->isInUse = 1;
			new_asset->useForwardPlus = 1;
			new_asset->bakeQuality = 3;

			new_asset->primaryLightCount = asset->primaryLightCount;
			new_asset->primaryLights = convert_primary_lights(asset->primaryLights.get(), asset->primaryLightCount, allocator);
			new_asset->firstScriptablePrimaryLight = new_asset->primaryLightCount;

			new_asset->primaryLightEnvCount = new_asset->primaryLightCount;
			new_asset->primaryLightEnvs = allocator.allocate_array<target::ComPrimaryLightEnv>(new_asset->primaryLightEnvCount);
			for (auto i = 1u; i < new_asset->primaryLightCount; i++)
			{
				new_asset->primaryLightEnvs[i].numIndices = 1;
				new_asset->primaryLightEnvs[i].primaryLightIndices[0] = static_cast<unsigned short>(i);
			}

			new_asset->changeListInfo.changeListNumber = 1232774;
			new_asset->changeListInfo.time = 1480386331;
			new_asset->changeListInfo.userName = allocator.duplicate_string("manyomi");

			for (auto& state : new_asset->umbraGateInitialStates)
			{
				state = -1;
			}

			return new_asset;
		}

		void dump(zonetool::iw5::ComWorld* asset)
		{
			utils::memory::allocator allocator;
			const auto converted = convert(asset, allocator);

			primary_lights.clear();
			for (auto i = 0u; i < converted->primaryLightCount; i++)
			{
				const auto& light = converted->primaryLights[i];
				light_reach reach{};
				reach.type = static_cast<unsigned char>(light.type);
				std::memcpy(reach.origin, light.origin, sizeof(reach.origin));
				std::memcpy(reach.dir, light.dir, sizeof(reach.dir));
				reach.radius = light.radius;
				reach.cos_half_fov_outer = light.cosHalfFovOuter;
				primary_lights.push_back(reach);
			}

			target::com_world::dump(converted);
		}
	}
}
