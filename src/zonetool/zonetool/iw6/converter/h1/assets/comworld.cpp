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
			zonetool::h1::ComWorld* convert(ComWorld* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::ComWorld>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(isInUse);
				COPY_VALUE(primaryLightCount);

				new_asset->primaryLights = mem->Alloc<zonetool::h1::ComPrimaryLight>(asset->primaryLightCount);
				for (unsigned int i = 0; i < asset->primaryLightCount; i++)
				{
					//auto* light = &asset->primaryLights[i];
					auto* new_light = &new_asset->primaryLights[i];

					COPY_VALUE_CAST(primaryLights[i].type); // should be the same
					COPY_VALUE_CAST(primaryLights[i].canUseShadowMap);
					new_light->needsDynamicShadows = false;
					COPY_VALUE_CAST(primaryLights[i].exponent);
					new_light->isVolumetric = false;
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
				}

				COPY_VALUE(primaryLightEnvCount);
				REINTERPRET_CAST_SAFE(primaryLightEnvs);

				return new_asset;
			}

			void dump(ComWorld* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IComWorld::dump(converted_asset);
			}
		}
	}
}