#include <std_include.hpp>

#include "zonetool/h1/converter/h2/include.hpp"
#include "fxeffectdef.hpp"
#include "zonetool/h2/assets/fxeffectdef.hpp"

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace fxeffectdef
		{

#define COPY_VALUE_FX(name) \
		static_assert(sizeof(asset->elemDefs[i].name) == sizeof(h1_asset->elemDefs[i].name)); \
		asset->elemDefs[i].name = h1_asset->elemDefs[i].name;

#define COPY_VALUE_CAST_FX(name) \
		static_assert(sizeof(asset->elemDefs[i].name) == sizeof(h1_asset->elemDefs[i].name)); \
		asset->elemDefs[i].name = *reinterpret_cast<decltype(asset->elemDefs[i].name)*>(&h1_asset->elemDefs[i].name);

#define COPY_ARR_FX(name) \
		static_assert(sizeof(asset->elemDefs[i].name) == sizeof(h1_asset->elemDefs[i].name)); \
		std::memcpy(&asset->elemDefs[i].name, &h1_asset->elemDefs[i].name, sizeof(asset->elemDefs[i].name));

#define REINTERPRET_CAST_SAFE_FX(name) \
		static_assert(sizeof(*asset->elemDefs[i].name) == sizeof(*h1_asset->elemDefs[i].name)); \
		asset->elemDefs[i].name = reinterpret_cast<decltype(asset->elemDefs[i].name)>(h1_asset->elemDefs[i].name);

			zonetool::h2::FxEffectDef* convert(zonetool::h1::FxEffectDef* h1_asset, utils::memory::allocator& allocator)
			{
				const auto asset = allocator.allocate<zonetool::h2::FxEffectDef>();

				std::memcpy(asset, h1_asset, sizeof(zonetool::h1::FxEffectDef));
				const auto elem_count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
				asset->elemDefs = allocator.allocate_array<zonetool::h2::FxElemDef>(elem_count);

				for (auto i = 0; i < elem_count; i++)
				{
					COPY_VALUE_FX(flags);
					COPY_VALUE_FX(flags2);
					COPY_VALUE_CAST_FX(spawn);
					COPY_VALUE_CAST_FX(spawnRange);
					COPY_VALUE_CAST_FX(fadeInRange);
					COPY_VALUE_CAST_FX(fadeOutRange);
					COPY_VALUE_FX(spawnFrustumCullRadius);
					COPY_VALUE_CAST_FX(spawnDelayMsec);
					COPY_VALUE_CAST_FX(lifeSpanMsec);
					COPY_ARR_FX(spawnOrigin);
					COPY_VALUE_CAST_FX(spawnOffsetRadius);
					COPY_VALUE_CAST_FX(spawnOffsetHeight);
					COPY_ARR_FX(spawnAngles);
					COPY_ARR_FX(angularVelocity);
					COPY_VALUE_CAST_FX(initialRotation);
					COPY_VALUE_CAST_FX(gravity);
					COPY_VALUE_CAST_FX(reflectionFactor);
					COPY_VALUE_CAST_FX(atlas);
					COPY_VALUE_CAST_FX(elemType);
					COPY_VALUE_FX(elemLitType);
					COPY_VALUE_FX(visualCount);
					COPY_VALUE_FX(velIntervalCount);
					COPY_VALUE_FX(visStateIntervalCount);

					if ((((asset->elemDefs[i].flags & 0x30) - 16) & 0xFFFFFFEF) != 0)
					{
						std::memset(&asset->elemDefs[i].spawnOffsetRadius, 0, sizeof(zonetool::h2::FxFloatRange));
					}

					const auto vel_count = asset->elemDefs[i].velIntervalCount + 1;
					asset->elemDefs[i].velSamples = allocator.allocate_array<zonetool::h2::FxElemVelStateSample>(vel_count);
					for (auto o = 0; o < vel_count; o++)
					{
						COPY_ARR_FX(velSamples[o].local.velocity.base);
						COPY_ARR_FX(velSamples[o].local.velocity.amplitude);
						std::memcpy(&asset->elemDefs[i].velSamples[o].local.velocity.unk_vec,
							asset->elemDefs[i].velSamples[o].local.velocity.amplitude,
							sizeof(float[3])
						);
						COPY_ARR_FX(velSamples[o].local.totalDelta.base);
						COPY_ARR_FX(velSamples[o].local.totalDelta.amplitude);
						std::memcpy(&asset->elemDefs[i].velSamples[o].local.totalDelta.unk_vec,
							asset->elemDefs[i].velSamples[o].local.totalDelta.amplitude,
							sizeof(float[3])
						);

						COPY_ARR_FX(velSamples[o].world.velocity.base);
						COPY_ARR_FX(velSamples[o].world.velocity.amplitude);
						std::memcpy(&asset->elemDefs[i].velSamples[o].world.totalDelta.unk_vec,
							asset->elemDefs[i].velSamples[o].world.totalDelta.amplitude,
							sizeof(float[3])
						);
						COPY_ARR_FX(velSamples[o].world.totalDelta.base);
						COPY_ARR_FX(velSamples[o].world.totalDelta.amplitude);
						std::memcpy(&asset->elemDefs[i].velSamples[o].world.totalDelta.unk_vec,
							asset->elemDefs[i].velSamples[o].world.totalDelta.amplitude,
							sizeof(float[3])
						);
					}

					REINTERPRET_CAST_SAFE_FX(visSamples);
					REINTERPRET_CAST_SAFE_FX(visuals.markArray);
					COPY_VALUE_CAST_FX(collBounds);
					REINTERPRET_CAST_SAFE_FX(effectOnImpact.name);
					REINTERPRET_CAST_SAFE_FX(effectOnDeath.name);
					REINTERPRET_CAST_SAFE_FX(effectEmitted.name);
					COPY_VALUE_CAST_FX(emitDist);
					COPY_VALUE_CAST_FX(emitDistVariance);
					REINTERPRET_CAST_SAFE_FX(extended.unknownDef);

					if (asset->elemDefs[i].extended.trailDef)
					{
						if (asset->elemDefs[i].elemType == zonetool::h2::FX_ELEM_TYPE_TRAIL)
						{
							asset->elemDefs[i].extended.trailDef = allocator.allocate<zonetool::h2::FxTrailDef>();
							COPY_VALUE_FX(extended.trailDef->scrollTimeMsec);
							COPY_VALUE_FX(extended.trailDef->repeatDist);
							COPY_VALUE_FX(extended.trailDef->invSplitDist);
							COPY_VALUE_FX(extended.trailDef->vertCount);
							REINTERPRET_CAST_SAFE_FX(extended.trailDef->verts);
							COPY_VALUE_FX(extended.trailDef->indCount);
							REINTERPRET_CAST_SAFE_FX(extended.trailDef->inds);
						}
						else if (asset->elemDefs[i].elemType == zonetool::h2::FX_ELEM_TYPE_DECAL)
						{
							asset->elemDefs[i].extended.decalDef = allocator.allocate<zonetool::h2::FxDecalDef>();
						}
					}

					COPY_VALUE_FX(sortOrder);
					COPY_VALUE_FX(lightingFrac);
					COPY_VALUE_FX(useItemClip);
					COPY_VALUE_FX(fadeInfo);
					COPY_VALUE_FX(randomSeed);
				}

				return asset;
			}

			void dump(zonetool::h1::FxEffectDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h2::fx_effect_def::dump(converted_asset);
			}
		}
	}
}
