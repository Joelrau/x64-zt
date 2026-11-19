#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

#include <iw7/assets/comworld.hpp>

namespace zonetool::iw8::converter::iw7::comworld
{
	namespace
	{
		namespace target = zonetool::iw7;

		target::ComPrimaryLight convert_light(const ComPrimaryLight& light)
		{
			target::ComPrimaryLight converted{};
			converted.type = static_cast<target::GfxLightType>(light.type);
			converted.canUseShadowMap = light.canUseShadowMap;
			converted.needsDynamicShadows = light.needsDynamicShadows;
			converted.isVolumetric = light.isVolumetric;
			converted.transientZoneList = light.transientZoneList;
			converted.entityId = light.entityId;
			converted.uvIntensity = light.uvIntensity;
			converted.irIntensity = light.irIntensity;

			for (auto i = 0; i < 3; i++)
			{
				converted.color[i] = light.colorLinearSrgb[i] * light.intensity;
			}

			std::memcpy(converted.dir, light.dir, sizeof(converted.dir));
			std::memcpy(converted.up, light.up, sizeof(converted.up));
			std::memcpy(converted.origin, light.origin, sizeof(converted.origin));
			converted.radius = light.radius;
			std::memcpy(converted.fadeOffsetRt, light.fadeOffset, sizeof(converted.fadeOffsetRt));
			converted.bulbRadius = light.bulbRadius;
			std::memcpy(converted.bulbLength, light.bulbLength, sizeof(converted.bulbLength));
			converted.cosHalfFovOuter = light.cosHalfFovOuter;
			converted.cosHalfFovInner = light.cosHalfFovInner;
			converted.shadowSoftness = light.shadowSoftness;
			converted.shadowBias = light.shadowBias;
			converted.shadowArea = light.shadowArea;
			converted.distanceFalloff = light.distanceFalloff;
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
		world->scriptablePrimaryLightCount = asset->scriptablePrimaryLightCount;
		world->firstScriptablePrimaryLight = asset->firstScriptablePrimaryLight;
		world->changeListInfo = std::bit_cast<target::ComChangeListInfo>(asset->changeListInfo);

		if (asset->numUmbraGates > sizeof(world->umbraGateInitialStates) * 8)
		{
			ZONETOOL_WARNING("comworld \"%s\" has %u umbra gates, IW7 supports %zu", asset->name, asset->numUmbraGates, sizeof(world->umbraGateInitialStates) * 8);
		}

		world->numUmbraGates = asset->numUmbraGates;
		world->umbraGateNames = asset->umbraGateNames;
		std::memcpy(world->umbraGateInitialStates, asset->umbraGateInitialStates, sizeof(world->umbraGateInitialStates));

		world->primaryLightCount = asset->primaryLightCount;
		world->primaryLights = allocator.allocate_array<target::ComPrimaryLight>(asset->primaryLightCount);
		world->primaryLightEnvCount = asset->primaryLightCount;
		world->primaryLightEnvs = allocator.allocate_array<target::ComPrimaryLightEnv>(asset->primaryLightCount);

		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			world->primaryLights[i] = convert_light(asset->primaryLights[i]);
			if (i)
			{
				world->primaryLightEnvs[i].primaryLightIndices[0] = static_cast<unsigned short>(i);
				world->primaryLightEnvs[i].numIndices = 1;
			}
		}

		target::com_world::dump(world);
		add_csv_row("com_map", asset->name);
	}
}
