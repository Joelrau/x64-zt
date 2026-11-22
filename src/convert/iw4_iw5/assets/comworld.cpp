#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace comworld
	{
		namespace
		{
			void perpendicular(const float* direction, float* up)
			{
				const float axis[3] = {std::fabs(direction[2]) < 0.9f ? 0.0f : 1.0f, 0.0f, std::fabs(direction[2]) < 0.9f ? 1.0f : 0.0f};
				up[0] = direction[1] * axis[2] - direction[2] * axis[1];
				up[1] = direction[2] * axis[0] - direction[0] * axis[2];
				up[2] = direction[0] * axis[1] - direction[1] * axis[0];

				const auto length = std::sqrt(up[0] * up[0] + up[1] * up[1] + up[2] * up[2]);
				if (length > 0.0f)
				{
					up[0] /= length;
					up[1] /= length;
					up[2] /= length;
				}
			}
		}

		zonetool::iw5::ComWorld* convert(zonetool::iw4::ComWorld* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw5::ComWorld>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(isInUse);
			COPY_VALUE(primaryLightCount);

			new_asset->primaryLights = allocator.allocate<zonetool::iw5::ComPrimaryLight>(asset->primaryLightCount);
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				const auto& light = asset->primaryLights.get()[i];
				auto& new_light = new_asset->primaryLights.get()[i];

				new_light.type = light.type;
				new_light.canUseShadowMap = light.canUseShadowMap;
				new_light.exponent = light.exponent;
				std::memcpy(new_light.color, light.color, sizeof(light.color));
				std::memcpy(new_light.dir, light.dir, sizeof(light.dir));
				perpendicular(light.dir, new_light.up); // IW4 stores no up vector
				std::memcpy(new_light.origin, light.origin, sizeof(light.origin));
				new_light.radius = light.radius;
				new_light.cosHalfFovOuter = light.cosHalfFovOuter;
				new_light.cosHalfFovInner = light.cosHalfFovInner;
				new_light.cosHalfFovExpanded = light.cosHalfFovExpanded;
				new_light.rotationLimit = light.rotationLimit;
				new_light.translationLimit = light.translationLimit;
				new_light.defName.value = light.defName.value;
			}

			return new_asset;
		}
	}
}
