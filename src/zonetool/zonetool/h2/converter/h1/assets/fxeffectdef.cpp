#include <std_include.hpp>

#include "zonetool/h2/converter/h1/include.hpp"
#include "fxeffectdef.hpp"
#include "zonetool/h1/assets/fxeffectdef.hpp"

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{

#define COPY_VALUE_FX(name) \
		static_assert(sizeof(new_asset->elemDefs[i].name) == sizeof(asset->elemDefs[i].name)); \
		new_asset->elemDefs[i].name = asset->elemDefs[i].name;

#define COPY_VALUE_CAST_FX(name) \
		static_assert(sizeof(new_asset->elemDefs[i].name) == sizeof(asset->elemDefs[i].name)); \
		new_asset->elemDefs[i].name = *reinterpret_cast<decltype(new_asset->elemDefs[i].name)*>(&asset->elemDefs[i].name);

#define COPY_ARR_FX(name) \
		static_assert(sizeof(new_asset->elemDefs[i].name) == sizeof(asset->elemDefs[i].name)); \
		std::memcpy(&new_asset->elemDefs[i].name, &asset->elemDefs[i].name, sizeof(new_asset->elemDefs[i].name));

#define REINTERPRET_CAST_SAFE_FX(name) \
		static_assert(sizeof(*new_asset->elemDefs[i].name) == sizeof(*asset->elemDefs[i].name)); \
		new_asset->elemDefs[i].name = reinterpret_cast<decltype(new_asset->elemDefs[i].name)>(asset->elemDefs[i].name);

			zonetool::h1::FxEffectDef* convert(zonetool::h2::FxEffectDef* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::FxEffectDef>();

				REINTERPRET_CAST_SAFE(name);
				COPY_VALUE(flags);
				COPY_VALUE(totalSize);
				COPY_VALUE(msecLoopingLife);
				COPY_VALUE(elemDefCountLooping);
				COPY_VALUE(elemDefCountOneShot);
				COPY_VALUE(elemDefCountEmission);
				COPY_VALUE(elemMaxRadius);
				COPY_VALUE(occlusionQueryDepthBias);
				COPY_VALUE(occlusionQueryFadeIn);
				COPY_VALUE(occlusionQueryFadeOut);
				COPY_VALUE(occlusionQueryScaleRange.base);
				COPY_VALUE(occlusionQueryScaleRange.amplitude);

				const auto elem_count = new_asset->elemDefCountLooping + new_asset->elemDefCountOneShot + new_asset->elemDefCountEmission;
				new_asset->elemDefs = allocator.allocate_array<zonetool::h1::FxElemDef>(elem_count);

				for (auto i = 0; i < elem_count; i++)
				{
					COPY_VALUE_CAST_FX(flags);
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
					COPY_VALUE_CAST_FX(elemLitType);
					COPY_VALUE_FX(visualCount);
					COPY_VALUE_FX(velIntervalCount);
					COPY_VALUE_FX(visStateIntervalCount);

					const auto vel_count = new_asset->elemDefs[i].velIntervalCount + 1;
					new_asset->elemDefs[i].velSamples = allocator.allocate_array<zonetool::h1::FxElemVelStateSample>(vel_count);
					for (auto o = 0; o < vel_count; o++)
					{
						COPY_ARR_FX(velSamples[o].local.velocity.base);
						COPY_ARR_FX(velSamples[o].local.velocity.amplitude);

						COPY_ARR_FX(velSamples[o].local.totalDelta.base);
						COPY_ARR_FX(velSamples[o].local.totalDelta.amplitude);

						COPY_ARR_FX(velSamples[o].world.velocity.base);
						COPY_ARR_FX(velSamples[o].world.velocity.amplitude);

						COPY_ARR_FX(velSamples[o].world.totalDelta.base);
						COPY_ARR_FX(velSamples[o].world.totalDelta.amplitude);
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

					if (new_asset->elemDefs[i].extended.trailDef)
					{
						if (new_asset->elemDefs[i].elemType == zonetool::h1::FX_ELEM_TYPE_TRAIL)
						{
							new_asset->elemDefs[i].extended.trailDef = allocator.allocate<zonetool::h1::FxTrailDef>();
							COPY_VALUE_FX(extended.trailDef->scrollTimeMsec);
							COPY_VALUE_FX(extended.trailDef->repeatDist);
							COPY_VALUE_FX(extended.trailDef->invSplitDist);
							COPY_VALUE_FX(extended.trailDef->vertCount);
							REINTERPRET_CAST_SAFE_FX(extended.trailDef->verts);
							COPY_VALUE_FX(extended.trailDef->indCount);
							REINTERPRET_CAST_SAFE_FX(extended.trailDef->inds);
						}
						else if (new_asset->elemDefs[i].elemType == zonetool::h1::FX_ELEM_TYPE_DECAL)
						{
							new_asset->elemDefs[i].extended.unknownDef = nullptr;
						}
					}

					COPY_VALUE_FX(sortOrder);
					COPY_VALUE_FX(lightingFrac);
					COPY_VALUE_FX(useItemClip);
					COPY_VALUE_FX(fadeInfo);
					COPY_VALUE_FX(randomSeed);

					new_asset->elemDefs[i].__pad0[1] = asset->elemDefs[i].unk_floats[0];
					new_asset->elemDefs[i].__pad0[2] = asset->elemDefs[i].unk_floats[1];
					new_asset->elemDefs[i].__pad0[3] = asset->elemDefs[i].unk_floats[2];
					new_asset->elemDefs[i].__pad0[4] = asset->elemDefs[i].unk_floats[3];
				}

				const auto reordered_elems = allocator.allocate_array<zonetool::h1::FxElemDef>(elem_count);

				const auto reordered_elems_looping = &reordered_elems[0];
				const auto reordered_elems_emission = &reordered_elems[new_asset->elemDefCountLooping];
				const auto reordered_elems_oneshot = &reordered_elems[new_asset->elemDefCountLooping + new_asset->elemDefCountEmission];

				const auto elems_looping = &new_asset->elemDefs[0];
				const auto elems_oneshot = &new_asset->elemDefs[asset->elemDefCountLooping];
				const auto elems_emission = &new_asset->elemDefs[asset->elemDefCountLooping + new_asset->elemDefCountOneShot];

				std::memcpy(reordered_elems_looping, elems_looping, asset->elemDefCountLooping * sizeof(zonetool::h1::FxElemDef));
				std::memcpy(reordered_elems_oneshot, elems_oneshot, asset->elemDefCountOneShot * sizeof(zonetool::h1::FxElemDef));
				std::memcpy(reordered_elems_emission, elems_emission, asset->elemDefCountEmission * sizeof(zonetool::h1::FxElemDef));

				new_asset->elemDefs = reordered_elems;

				return new_asset;
			}

			void dump(zonetool::h2::FxEffectDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::fx_effect_def::dump(converted_asset);
			}
		}
	}
}
