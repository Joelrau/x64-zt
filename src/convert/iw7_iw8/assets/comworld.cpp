#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"

#include <iw8/assets/comworld.hpp>

namespace zonetool::iw7::converter::iw8::comworld
{
	namespace
	{
		namespace target = zonetool::iw8;

		void orthonormal_up(const float* dir, const float* up, float* out)
		{
			const auto project = [&](const float* axis)
			{
				const auto along = axis[0] * dir[0] + axis[1] * dir[1] + axis[2] * dir[2];
				for (auto i = 0; i < 3; i++)
				{
					out[i] = axis[i] - along * dir[i];
				}
				return std::sqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]);
			};

			const auto dir_length = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
			if (dir_length < 0.999f || dir_length > 1.001f)
			{
				std::memcpy(out, up, sizeof(float[3]));
				return;
			}

			auto length = project(up);
			if (length < 0.001f)
			{
				const float fallback[2][3] = {{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}};
				length = project(fallback[std::abs(dir[2]) > 0.9f ? 1 : 0]);
			}

			for (auto i = 0; i < 3; i++)
			{
				out[i] /= length;
			}
		}

		target::ComPrimaryLight convert_light(const ComPrimaryLight& light)
		{
			target::ComPrimaryLight converted{};
			if (light.type == GFX_LIGHT_TYPE_SPOT || light.type == GFX_LIGHT_TYPE_OMNI)
			{
				converted.spawnFlags = !light.canUseShadowMap ? target::PRIMARY_LIGHT_SPAWN_NO_SHADOW : light.needsDynamicShadows ? 0 : target::PRIMARY_LIGHT_SPAWN_STATIC_SHADOW; // TODO: stock mp_rust pattern, not proven in IW8 asm
				converted.shadowNearPlaneBias = 8.0f; // TODO: value of every stock mp_rust light, IW7 has no field
			}

			converted.type = light.type;
			converted.canUseShadowMap = light.canUseShadowMap;
			converted.needsDynamicShadows = light.needsDynamicShadows;
			converted.isVolumetric = light.isVolumetric;
			converted.transientZoneList = light.transientZoneList;
			converted.entityId = light.entityId;
			converted.uvIntensity = light.uvIntensity;
			converted.irIntensity = light.irIntensity;

			const auto luminance = 0.2126f * light.color[0] + 0.7152f * light.color[1] + 0.0722f * light.color[2]; // TODO: stock mp_rust colors have luminance 1, not proven in IW8 asm
			converted.intensity = luminance;
			for (auto i = 0; i < 3; i++)
			{
				converted.colorLinearSrgb[i] = luminance > 0.0f ? light.color[i] / luminance : 0.0f;
			}

			std::memcpy(converted.dir, light.dir, sizeof(converted.dir));
			std::memcpy(converted.origin, light.origin, sizeof(converted.origin));
			orthonormal_up(light.dir, light.up, converted.up); // TODO: stock mp_rust up is orthonormal to dir, not proven in IW8 asm
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
