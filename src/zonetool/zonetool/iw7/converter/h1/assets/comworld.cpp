#include <std_include.hpp>

#include "zonetool/iw7/converter/h1/include.hpp"
#include "comworld.hpp"
#include "zonetool/h1/assets/comworld.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace comworld
		{
#define COPY_VALUE_COMWORLD(name) \
		lights[i].name = iw7_lights[i].name; \

#define COPY_ARR_COMWORLD(name) \
		std::memcpy(&lights[i].name, &iw7_lights[i].name, sizeof(lights[i].name)); \

#define COPY_ARR_COMWORLD_(name, iw7_name) \
		std::memcpy(&lights[i].name, &iw7_lights[i].iw7_name, sizeof(lights[i].name)); \

			zonetool::h1::ComPrimaryLight* convert_primary_lights(zonetool::iw7::ComPrimaryLight* iw7_lights, const unsigned int count,
				utils::memory::allocator& allocator)
			{
				const auto lights = allocator.allocate_array<zonetool::h1::ComPrimaryLight>(count);

				for (auto i = 0u; i < count; i++)
				{
					lights[i].type = static_cast<zonetool::h1::GfxLightType>(iw7_lights[i].type);
					COPY_VALUE_COMWORLD(canUseShadowMap);
					COPY_VALUE_COMWORLD(needsDynamicShadows);
					COPY_VALUE_COMWORLD(exponent);
					COPY_VALUE_COMWORLD(isVolumetric);
					COPY_ARR_COMWORLD(color);
					COPY_ARR_COMWORLD(dir);
					COPY_ARR_COMWORLD(up);
					COPY_ARR_COMWORLD(origin);
					COPY_ARR_COMWORLD_(fadeOffset, fadeOffsetRt);
					COPY_VALUE_COMWORLD(bulbRadius);
					COPY_ARR_COMWORLD(bulbLength);
					COPY_VALUE_COMWORLD(radius);
					COPY_VALUE_COMWORLD(cosHalfFovOuter);
					COPY_VALUE_COMWORLD(cosHalfFovInner);
					lights[i].cosHalfFovExpanded = 0.0f;	//COPY_VALUE_COMWORLD(cosHalfFovExpanded);
					COPY_VALUE_COMWORLD(rotationLimit);
					COPY_VALUE_COMWORLD(translationLimit);
					lights[i].cucRotationOffsetRad = 0.0f;	//COPY_VALUE_COMWORLD(cucRotationOffsetRad);
					lights[i].cucRotationSpeedRad = 0.0f;	//COPY_VALUE_COMWORLD(cucRotationSpeedRad);
					lights[i].cucScrollVector[0] = 0.0f;	//COPY_ARR_COMWORLD(cucScrollVector);
					lights[i].cucScrollVector[1] = 0.0f;
					lights[i].cucScaleVector[0] = 0.0f;		//COPY_ARR_COMWORLD(cucScaleVector);
					lights[i].cucScaleVector[1] = 0.0f;
					lights[i].cucTransVector[0] = 0.0f;		//COPY_ARR_COMWORLD(cucTransVector);
					lights[i].cucTransVector[1] = 0.0f;
					lights[i].defName = iw7_lights[i].defName;
				}

				return lights;
			}

#undef COPY_VALUE_COMWORLD
#undef COPY_ARR_COMWORLD

			zonetool::h1::ComWorld* convert(zonetool::iw7::ComWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::ComWorld>();

				REINTERPRET_CAST_SAFE(name);
				COPY_VALUE(isInUse);
				COPY_VALUE(primaryLightCount);
				new_asset->primaryLights = convert_primary_lights(asset->primaryLights, asset->primaryLightCount, allocator);
				
				// priaryLightsEnv
				new_asset->primaryLightEnvCount = asset->primaryLightCount; // use same count as primaryLight
				new_asset->primaryLightEnvs = allocator.allocate_array<zonetool::h1::ComPrimaryLightEnv>(new_asset->primaryLightEnvCount);
				for (auto i = 0; i < new_asset->primaryLightEnvCount; ++i)
				{
					new_asset->primaryLightEnvs[i].numIndices = 1;
					//new_asset->primaryLightEnvs[i].primaryLightIndices = 0; // needed?
				}

				return new_asset;
			}

			void dump(zonetool::iw7::ComWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::com_world::dump(converted_asset);
			}
		}
	}
}
