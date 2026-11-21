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
			zonetool::h1::ComWorld* convert(ComWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::ComWorld>();

				REINTERPRET_CAST_SAFE(name);
				COPY_VALUE(isInUse);
				COPY_VALUE(primaryLightCount);

				new_asset->primaryLights = allocator.allocate_array<zonetool::h1::ComPrimaryLight>(asset->primaryLightCount);
				for (unsigned int i = 0; i < asset->primaryLightCount; i++)
				{
					const auto* src = &asset->primaryLights[i];
					auto* dst = &new_asset->primaryLights[i];

					// IW7 lights are already physically based (UNTESTED mapping: colors copied as-is)
					dst->type = static_cast<zonetool::h1::GfxLightType>(src->type);
					dst->canUseShadowMap = src->canUseShadowMap;
					dst->physicallyBased = 1;
					dst->exponent = src->exponent;
					dst->lightingState = 0;
					std::memcpy(dst->color, src->color, sizeof(dst->color));
					std::memcpy(dst->dir, src->dir, sizeof(dst->dir));
					std::memcpy(dst->up, src->up, sizeof(dst->up));
					std::memcpy(dst->origin, src->origin, sizeof(dst->origin));
					dst->fadeOffset[0] = src->fadeOffsetRt[0];
					dst->fadeOffset[1] = src->fadeOffsetRt[1];
					dst->bulbRadius = src->bulbRadius;
					std::memcpy(dst->bulbLength, src->bulbLength, sizeof(dst->bulbLength));
					dst->radius = src->radius;
					dst->cosHalfFovOuter = src->cosHalfFovOuter;
					dst->cosHalfFovInner = src->cosHalfFovInner;
					dst->cosHalfFovExpanded = src->cosHalfFovOuter;
					dst->rotationLimit = src->rotationLimit;
					dst->translationLimit = src->translationLimit;
					dst->cucRotationOffsetRad = 0.0f;
					dst->cucRotationSpeedRad = 0.0f;
					dst->cucScrollVector[0] = dst->cucScrollVector[1] = 0.0f;
					dst->cucScaleVector[0] = dst->cucScaleVector[1] = 1.0f;
					dst->cucTransVector[0] = dst->cucTransVector[1] = 0.0f;
					dst->defName = src->defName;
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
