#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

#include <iw8/assets/comworld.hpp>

namespace zonetool::iw7::converter::iw8::comworld
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::ComPrimaryLight convert_light(const ComPrimaryLight& light)
		{
			target::ComPrimaryLight converted{};
			converted.type = light.type;
			converted.canUseShadowMap = light.canUseShadowMap;
			converted.needsDynamicShadows = light.needsDynamicShadows;
			converted.isVolumetric = light.isVolumetric;
			converted.transientZoneList = light.transientZoneList;
			converted.entityId = light.entityId;
			converted.uvIntensity = light.uvIntensity;
			converted.irIntensity = light.irIntensity;

			const auto intensity = std::max({light.color[0], light.color[1], light.color[2]});
			converted.intensity = intensity / std::numbers::pi_v<float>; // TODO: IW7 artist unit inferred from sun ratio, not proven in IW7 asm
			for (auto i = 0; i < 3; i++)
			{
				converted.colorLinearSrgb[i] = intensity > 0.0f ? light.color[i] / intensity : 0.0f;
			}

			std::memcpy(converted.dir, light.dir, sizeof(converted.dir));
			std::memcpy(converted.up, light.up, sizeof(converted.up));
			std::memcpy(converted.origin, light.origin, sizeof(converted.origin));
			converted.radius = light.radius;
			std::memcpy(converted.fadeOffset, light.fadeOffsetRt, sizeof(converted.fadeOffset));
			converted.bulbRadius = light.bulbRadius;
			std::memcpy(converted.bulbLength, light.bulbLength, sizeof(converted.bulbLength));
			converted.cosHalfFovOuter = light.cosHalfFovOuter;
			converted.cosHalfFovInner = light.cosHalfFovInner;
			converted.shadowSoftness = light.shadowSoftness;
			converted.shadowBias = light.shadowBias;
			converted.shadowArea = light.shadowArea;
			converted.distanceFalloff = light.distanceFalloff;
			converted.cosHalfFovCollimation = std::cos(std::numbers::pi_v<float> / 2.0f);
			converted.rotationLimit = light.rotationLimit;
			converted.translationLimit = light.translationLimit;
			converted.defName = light.defName;
			return converted;
		}
	}

	void dump(ComWorld* asset)
	{
		utils::memory::allocator allocator;

		auto* world = allocator.allocate<target::ComWorld>();
		world->name = asset->name;
		world->isInUse = asset->isInUse;
		world->useForwardPlus = asset->useForwardPlus;
		world->bakeQuality = asset->bakeQuality;
		world->trVisRadii[0] = 4500.0f;
		world->trVisRadii[1] = 25000.0f;
		world->trVisRadii[2] = 30000.0f;
		world->trVisRadii[3] = 40000.0f;
		world->trVisRadii[4] = 50000.0f;
		world->trVisFacingDistAdd = 1500.0f;
		world->scriptablePrimaryLightCount = asset->scriptablePrimaryLightCount;
		world->firstScriptablePrimaryLight = asset->firstScriptablePrimaryLight;
		world->changeListInfo = std::bit_cast<target::ComChangeListInfo>(asset->changeListInfo);

		world->transientTableSize = 1;
		world->transientTable = allocator.allocate<unsigned short>();
		*world->transientTable = 0x8000;

		world->numUmbraGates = asset->numUmbraGates;
		world->umbraGateNames = asset->umbraGateNames;
		std::memset(world->umbraGateInitialStates, 0xFF, sizeof(world->umbraGateInitialStates));
		std::memcpy(world->umbraGateInitialStates, asset->umbraGateInitialStates, sizeof(asset->umbraGateInitialStates));

		world->primaryLightCount = asset->primaryLightCount;
		world->primaryLights = allocator.allocate_array<target::ComPrimaryLight>(asset->primaryLightCount);
		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			world->primaryLights[i] = convert_light(asset->primaryLights[i]);
		}

		target::com_world::dump(world);
		add_csv_row("com_map", asset->name);
	}
}
