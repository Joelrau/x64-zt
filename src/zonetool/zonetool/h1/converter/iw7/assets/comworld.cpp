#include <std_include.hpp>

#include "zonetool/h1/converter/iw7/include.hpp"
#include "comworld.hpp"
#include "zonetool/iw7/assets/comworld.hpp"

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace comworld
		{
#define COPY_VALUE_COMWORLD(name) \
		lights[i].name = h1_lights[i].name; \

#define COPY_ARR_COMWORLD(name) \
		std::memcpy(&lights[i].name, &h1_lights[i].name, sizeof(lights[i].name)); \

			zonetool::iw7::ComPrimaryLight* convert_primary_lights(zonetool::h1::ComPrimaryLight* h1_lights, const unsigned int count,
				utils::memory::allocator& allocator)
			{
				const auto lights = allocator.allocate_array<zonetool::iw7::ComPrimaryLight>(count);

				for (auto i = 0u; i < count; i++)
				{
					lights[i].type = static_cast<zonetool::iw7::GfxLightType>(h1_lights[i].type);
					COPY_VALUE_COMWORLD(canUseShadowMap);
					COPY_VALUE_COMWORLD(needsDynamicShadows);
					COPY_VALUE_COMWORLD(exponent);
					COPY_VALUE_COMWORLD(isVolumetric);
					COPY_ARR_COMWORLD(color);
					COPY_ARR_COMWORLD(dir);
					COPY_ARR_COMWORLD(up);
					COPY_ARR_COMWORLD(origin);
					std::memcpy(&lights[i].fadeOffsetRt, &h1_lights[i].fadeOffset, sizeof(lights[i].fadeOffsetRt));
					COPY_VALUE_COMWORLD(bulbRadius);
					COPY_ARR_COMWORLD(bulbLength);
					COPY_VALUE_COMWORLD(radius);
					COPY_VALUE_COMWORLD(cosHalfFovOuter);
					COPY_VALUE_COMWORLD(cosHalfFovInner);
					COPY_VALUE_COMWORLD(rotationLimit);
					COPY_VALUE_COMWORLD(translationLimit);
					lights[i].defName = h1_lights[i].defName;

					lights[i].transientZoneList = 0;
					lights[i].entityId = 0;
					lights[i].uvIntensity = 0.0f;
					lights[i].irIntensity = 0.0f;
					lights[i].shadowSoftness = 0.0f;
					lights[i].shadowBias = 0.0f;
					lights[i].distanceFalloff = 0.0f;
				}

				return lights;
			}

#undef COPY_VALUE_COMWORLD
#undef COPY_ARR_COMWORLD

			zonetool::iw7::ComWorld* convert(zonetool::h1::ComWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::iw7::ComWorld>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->isInUse = 1; //COPY_VALUE(isInUse);
				new_asset->useForwardPlus = 1;
				new_asset->bakeQuality = 3;

				COPY_VALUE(primaryLightCount);
				new_asset->primaryLights = convert_primary_lights(asset->primaryLights, asset->primaryLightCount, allocator);
				COPY_VALUE(primaryLightEnvCount);

				new_asset->firstScriptablePrimaryLight = new_asset->primaryLightCount;
				
				//REINTERPRET_CAST_SAFE(primaryLightEnvs);
				new_asset->primaryLightEnvCount = asset->primaryLightCount;
				new_asset->primaryLightEnvs = allocator.allocate_array<zonetool::iw7::ComPrimaryLightEnv>(asset->primaryLightCount);
				for (unsigned short i = 0; i < asset->primaryLightCount; i++)
				{
					if (asset->primaryLights[i].type != 0)
					{
						new_asset->primaryLightEnvs[i].numIndices = 1;
						new_asset->primaryLightEnvs[i].primaryLightIndices[0] = i;
					}
				}

				new_asset->changeListInfo.changeListNumber = 1232774;
				new_asset->changeListInfo.time = 1480386331;
				new_asset->changeListInfo.userName = allocator.duplicate_string("manyomi");

				new_asset->numUmbraGates = 0;
				new_asset->umbraGateNames = nullptr;
				for (auto i = 0; i < 4; i++)
				{
					new_asset->umbraGateInitialStates[i] = -1;
				}

				return new_asset;
			}

			void dump(zonetool::h1::ComWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::iw7::com_world::dump(converted_asset);
			}
		}
	}
}
