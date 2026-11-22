#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace comworld
	{
		zonetool::iw4::ComWorld* convert(zonetool::iw3::ComWorld* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::ComWorld>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(isInUse);
			COPY_VALUE(primaryLightCount);

			new_asset->primaryLights = allocator.allocate<zonetool::iw4::ComPrimaryLight>(asset->primaryLightCount);
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				const auto& light = asset->primaryLights.get()[i];
				auto& new_light = new_asset->primaryLights.get()[i];

				new_light.type = light.type;
				new_light.canUseShadowMap = light.canUseShadowMap;
				new_light.exponent = light.exponent;
				std::memcpy(new_light.color, light.color, sizeof(light.color));
				std::memcpy(new_light.dir, light.dir, sizeof(light.dir));
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
