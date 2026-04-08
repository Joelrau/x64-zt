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

			unsigned char convert_fx_elem_lit_type(unsigned char type)
			{
				static std::unordered_map<unsigned char, unsigned char> type_map =
				{
					{FX_ELEM_LIT_TYPE_NONE, zonetool::h2::FX_ELEM_LIT_TYPE_NONE},
					{FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE, zonetool::h2::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE},
					{FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SINGLE, zonetool::h2::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SINGLE},
					{FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE, zonetool::h2::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE},
					{FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_VERTEX, zonetool::h2::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_VERTEX},
				};

				const auto iter = type_map.find(type);
				if (iter == type_map.end())
				{
					ZONETOOL_FATAL("invalid FxElem elemLitType %i", type);
				}

				return iter->second;
			}

			zonetool::h2::FxEffectDef* convert(zonetool::h1::FxEffectDef* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h2::FxEffectDef>();

				auto warnings = 0;

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
				new_asset->elemDefs = allocator.allocate_array<zonetool::h2::FxElemDef>(elem_count);

				//asset->flags |= 0x400;
				new_asset->xU_01 = 1.f;

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
					new_asset->elemDefs[i].elemLitType = convert_fx_elem_lit_type(asset->elemDefs[i].elemLitType);
					COPY_VALUE_FX(visualCount);
					COPY_VALUE_FX(velIntervalCount);
					COPY_VALUE_FX(visStateIntervalCount);

					new_asset->elemDefs[i].flags = asset->elemDefs[i].flags;

					new_asset->elemDefs[i].flags &= ~zonetool::h2::FX_ELEM_SPAWN_OFFSET_MASK;
					new_asset->elemDefs[i].flags &= ~zonetool::h2::FX_ELEM_RUN_MASK;

					switch (asset->elemDefs[i].flags & FX_ELEM_SPAWN_OFFSET_MASK)
					{
					case FX_ELEM_SPAWN_OFFSET_SPHERE:
						new_asset->elemDefs[i].flags |= zonetool::h2::FX_ELEM_SPAWN_OFFSET_SPHERE;
						break;
					case FX_ELEM_SPAWN_OFFSET_CYLINDER:
					case FX_ELEM_SPAWN_OFFSET_MASK:
						new_asset->elemDefs[i].flags |= zonetool::h2::FX_ELEM_SPAWN_OFFSET_CYLINDER;
						break;
					}

					switch (asset->elemDefs[i].flags & FX_ELEM_RUN_MASK)
					{
					case FX_ELEM_RUN_RELATIVE_TO_WORLD:
						new_asset->elemDefs[i].flags |= zonetool::h2::FX_ELEM_RUN_RELATIVE_TO_WORLD;
						break;
					case FX_ELEM_RUN_RELATIVE_TO_SPAWN:
						new_asset->elemDefs[i].flags |= zonetool::h2::FX_ELEM_RUN_RELATIVE_TO_EFFECT;
						new_asset->elemDefs[i].flags |= zonetool::h2::FX_ELEM_RUN_RELATIVE_TO_CAMERA;
						break;
					case FX_ELEM_RUN_RELATIVE_TO_EFFECT:
						new_asset->elemDefs[i].flags |= zonetool::h2::FX_ELEM_RUN_RELATIVE_TO_EFFECT;
						break;
					case FX_ELEM_RUN_RELATIVE_TO_OFFSET:
						new_asset->elemDefs[i].flags2 |= zonetool::h2::FX_ELEM2_RUN_RELATIVE_TO_OFFSET;
						break;
					case FX_ELEM_RUN_RELATIVE_TO_CAMERA:
						new_asset->elemDefs[i].flags |= zonetool::h2::FX_ELEM_RUN_RELATIVE_TO_CAMERA;
						break;
					}

					if (asset->elemDefs[i].useItemClip)
					{
						new_asset->elemDefs[i].flags2 |= zonetool::h2::FX_ELEM2_USE_ITEM_CLIP;
					}

					if ((((new_asset->elemDefs[i].flags & 0x30) - 16) & 0xFFFFFFEF) != 0)
					{
						std::memset(&new_asset->elemDefs[i].spawnOffsetRadius, 0, sizeof(zonetool::h2::FxFloatRange));
					}
					else
					{
						new_asset->elemDefs[i].unkRange.amplitude = 6.2831855f;
						new_asset->elemDefs[i].unkRange.base = 0.f;
					}

					const auto vel_count = new_asset->elemDefs[i].velIntervalCount + 1;
					new_asset->elemDefs[i].velSamples = allocator.allocate_array<zonetool::h2::FxElemVelStateSample>(vel_count);
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
						if (new_asset->elemDefs[i].elemType == zonetool::h2::FX_ELEM_TYPE_TRAIL)
						{
							new_asset->elemDefs[i].extended.trailDef = allocator.allocate<zonetool::h2::FxTrailDef>();
							COPY_VALUE_FX(extended.trailDef->scrollTimeMsec);
							COPY_VALUE_FX(extended.trailDef->repeatDist);
							COPY_VALUE_FX(extended.trailDef->invSplitDist);
							COPY_VALUE_FX(extended.trailDef->vertCount);
							REINTERPRET_CAST_SAFE_FX(extended.trailDef->verts);
							COPY_VALUE_FX(extended.trailDef->indCount);
							REINTERPRET_CAST_SAFE_FX(extended.trailDef->inds);
						}
						else if (new_asset->elemDefs[i].elemType == zonetool::h2::FX_ELEM_TYPE_DECAL)
						{
							new_asset->elemDefs[i].extended.decalDef = allocator.allocate<zonetool::h2::FxDecalDef>();
						}
					}
					else if (new_asset->elemDefs[i].elemType == zonetool::h2::FX_ELEM_TYPE_DECAL)
					{
						new_asset->elemDefs[i].extended.decalDef = allocator.allocate<zonetool::h2::FxDecalDef>();
					}

					// atlas looping fx is broken
					if (new_asset->elemDefs[i].elemType <= zonetool::h2::FX_ELEM_TYPE_SPRITE_ROTATED &&
						i < new_asset->elemDefCountLooping &&
						(new_asset->elemDefs[i].atlas.rowIndexBits > 1 ||
							new_asset->elemDefs[i].atlas.colIndexBits > 1) &&
						(new_asset->elemDefs[i].atlas.behavior & 16) != 0)
					{
						new_asset->elemDefs[i].atlas.behavior &= ~4;
						new_asset->elemDefs[i].atlas.behavior |= 8;
						new_asset->elemDefs[i].atlas.loopCount = 0;
						warnings |= 1;
					}

					COPY_VALUE_FX(sortOrder);
					COPY_VALUE_FX(lightingFrac);
					COPY_VALUE_FX(fadeInfo);
					COPY_VALUE_FX(fadeOutInfo);

					COPY_VALUE_FX(randomSeed);

					COPY_VALUE_FX(emissiveScaleScale);
					COPY_VALUE_FX(hdrLightingFrac);
					COPY_VALUE_FX(shadowDensityScale);
				}

				const auto reordered_elems = allocator.allocate_array<zonetool::h2::FxElemDef>(elem_count);

				const auto reordered_elems_looping = &reordered_elems[0];
				const auto reordered_elems_emission = &reordered_elems[new_asset->elemDefCountLooping];
				const auto reordered_elems_oneshot = &reordered_elems[new_asset->elemDefCountLooping + new_asset->elemDefCountEmission];

				const auto elems_looping = &new_asset->elemDefs[0];
				const auto elems_oneshot = &new_asset->elemDefs[asset->elemDefCountLooping];
				const auto elems_emission = &new_asset->elemDefs[asset->elemDefCountLooping + new_asset->elemDefCountOneShot];

				std::memcpy(reordered_elems_looping, elems_looping, asset->elemDefCountLooping * sizeof(zonetool::h2::FxElemDef));
				std::memcpy(reordered_elems_oneshot, elems_oneshot, asset->elemDefCountOneShot * sizeof(zonetool::h2::FxElemDef));
				std::memcpy(reordered_elems_emission, elems_emission, asset->elemDefCountEmission * sizeof(zonetool::h2::FxElemDef));

				new_asset->elemDefs = reordered_elems;

				if ((warnings & 1) != 0)
				{
					ZONETOOL_WARNING("fx \"%s\" has elems that use atlas textures which probably wont work properly", new_asset->name);
				}

				return new_asset;
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
