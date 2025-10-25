#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "comworld.hpp"

#include "zonetool/h1/assets/comworld.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace comworld
		{
			namespace
			{
				float PhysicallyBasedLight_IntensityFromCandelas(float candelas)
				{
					return candelas * 1550.0f;
				}

				float PhysicallyBasedLight_FramebufferUnitsFromIntensity(float intensity, float radiometricScale)
				{
					return intensity * radiometricScale;
				}

				void convertColorToPhysicallyBased(float* color)
				{
					float extracted_intensity = std::max({ color[0], color[1], color[2] });
					float normalized_color[3] = {
						color[0] / extracted_intensity,
						color[1] / extracted_intensity,
						color[2] / extracted_intensity
					};

					auto intensity = PhysicallyBasedLight_IntensityFromCandelas(extracted_intensity * 1000.0f);
					auto framebuffer_units = PhysicallyBasedLight_FramebufferUnitsFromIntensity(intensity, 0.001f);

					color[0] = normalized_color[0] * framebuffer_units;
					color[1] = normalized_color[1] * framebuffer_units;
					color[2] = normalized_color[2] * framebuffer_units;
				}
			}

			zonetool::h1::ComWorld* convert(ComWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::ComWorld>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(isInUse);
				COPY_VALUE(primaryLightCount);

				new_asset->primaryLights = allocator.allocate_array<zonetool::h1::ComPrimaryLight>(asset->primaryLightCount);
				for (unsigned int i = 0; i < asset->primaryLightCount; i++)
				{
					//auto* light = &asset->primaryLights[i];
					const auto new_light = &new_asset->primaryLights[i];

					COPY_VALUE_CAST(primaryLights[i].type); // should be the same
					COPY_VALUE_CAST(primaryLights[i].canUseShadowMap);
					new_light->physicallyBased = false;
					COPY_VALUE_CAST(primaryLights[i].exponent);
					new_light->lightingState = 0;
					COPY_ARR(primaryLights[i].color);
					COPY_ARR(primaryLights[i].dir);
					COPY_ARR(primaryLights[i].up);
					COPY_ARR(primaryLights[i].origin);
					memset(new_light->fadeOffset, 0, sizeof(new_light->fadeOffset));
					new_light->bulbRadius = 0;
					memset(new_light->bulbLength, 0, sizeof(new_light->bulbLength));
					COPY_VALUE(primaryLights[i].radius);
					COPY_VALUE(primaryLights[i].cosHalfFovOuter);
					COPY_VALUE(primaryLights[i].cosHalfFovInner);
					COPY_VALUE(primaryLights[i].cosHalfFovExpanded);
					COPY_VALUE(primaryLights[i].rotationLimit);
					COPY_VALUE(primaryLights[i].translationLimit);
					COPY_VALUE(primaryLights[i].cucRotationOffsetRad);
					COPY_VALUE(primaryLights[i].cucRotationSpeedRad);
					COPY_ARR(primaryLights[i].cucScrollVector);
					COPY_ARR(primaryLights[i].cucScaleVector);
					COPY_ARR(primaryLights[i].cucTransVector);
					REINTERPRET_CAST_SAFE(primaryLights[i].defName);

					if (new_light->type == zonetool::h1::GFX_LIGHT_TYPE_SPOT || 
						new_light->type == zonetool::h1::GFX_LIGHT_TYPE_OMNI)
					{
						new_light->canUseShadowMap = 1;
						new_light->physicallyBased = 1;
						new_light->lightingState = 0;

						new_light->cucScaleVector[0] = 1.0f;
						new_light->cucScaleVector[1] = 1.0f;

						new_light->bulbRadius = 3.0f;
						new_light->bulbLength[0] = 0.0f;
						new_light->bulbLength[1] = 0.0f;
						new_light->bulbLength[2] = 0.0f;
						new_light->fadeOffset[0] = 0.0f;
						new_light->fadeOffset[1] = 0.0f;

						convertColorToPhysicallyBased(new_light->color);
					}
				}

				COPY_VALUE(primaryLightEnvCount);
				REINTERPRET_CAST_SAFE(primaryLightEnvs);

				return new_asset;
			}

			void dump(ComWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::com_world::dump(converted_asset);
			}
		}
	}
}