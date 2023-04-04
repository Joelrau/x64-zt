#include <std_include.hpp>

#include "zonetool/h1/converter/h2/include.hpp"
#include "comworld.hpp"
#include "zonetool/h2/assets/comworld.hpp"

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace comworld
		{
#define COPY_VALUE_COMWORLD(name) \
		lights[i].name = h1_lights[i].name; \

#define COPY_ARR_COMWORLD(name) \
		std::memcpy(&lights[i].name, &h1_lights[i].name, sizeof(lights[i].name)); \

			zonetool::h2::ComPrimaryLight* convert_primary_lights(zonetool::h1::ComPrimaryLight* h1_lights, const unsigned int count,
				utils::memory::allocator& allocator)
			{
				const auto lights = allocator.allocate_array<zonetool::h2::ComPrimaryLight>(count);

				for (auto i = 0u; i < count; i++)
				{
					lights[i].type = static_cast<zonetool::h2::GfxLightType>(h1_lights[i].type);
					COPY_VALUE_COMWORLD(canUseShadowMap);
					COPY_VALUE_COMWORLD(needsDynamicShadows);
					COPY_VALUE_COMWORLD(exponent);
					COPY_VALUE_COMWORLD(isVolumetric);
					COPY_ARR_COMWORLD(color);
					std::memcpy(lights[i].color2, lights[i].color, sizeof(lights[i].color));
					COPY_ARR_COMWORLD(dir);
					COPY_ARR_COMWORLD(up);
					COPY_ARR_COMWORLD(origin);
					COPY_ARR_COMWORLD(fadeOffset);
					COPY_VALUE_COMWORLD(bulbRadius);
					COPY_ARR_COMWORLD(bulbLength);
					COPY_VALUE_COMWORLD(radius);
					COPY_VALUE_COMWORLD(cosHalfFovOuter);
					COPY_VALUE_COMWORLD(cosHalfFovInner);
					COPY_VALUE_COMWORLD(cosHalfFovExpanded);
					COPY_VALUE_COMWORLD(rotationLimit);
					COPY_VALUE_COMWORLD(translationLimit);
					COPY_VALUE_COMWORLD(cucRotationOffsetRad);
					COPY_VALUE_COMWORLD(cucRotationSpeedRad);
					COPY_ARR_COMWORLD(cucScrollVector);
					COPY_ARR_COMWORLD(cucScaleVector);
					COPY_ARR_COMWORLD(cucTransVector);
					lights[i].defName = h1_lights[i].defName;
				}

				return lights;
			}

#undef COPY_VALUE_COMWORLD
#undef COPY_ARR_COMWORLD

			zonetool::h2::ComWorld* convert(zonetool::h1::ComWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h2::ComWorld>();

				REINTERPRET_CAST_SAFE(name);
				COPY_VALUE(isInUse);
				COPY_VALUE(primaryLightCount);
				new_asset->primaryLights = convert_primary_lights(asset->primaryLights, asset->primaryLightCount, allocator);
				COPY_VALUE(primaryLightEnvCount);
				REINTERPRET_CAST_SAFE(primaryLightEnvs);

				return new_asset;
			}

			void dump(zonetool::h1::ComWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h2::com_world::dump(converted_asset);
			}
		}
	}
}
