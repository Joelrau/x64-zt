#include <std_include.hpp>
#include "../include.hpp"
#include "fxeffectdef.hpp"

#include <h1/assets/fxeffectdef.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			zonetool::h1::FxElemType convert_elem_type(zonetool::iw6::FxElemType type)
			{
				switch (type)
				{
				case zonetool::iw6::FX_ELEM_TYPE_SPRITE_BILLBOARD:
					return zonetool::h1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_SPRITE_ORIENTED:
					return zonetool::h1::FX_ELEM_TYPE_SPRITE_ORIENTED;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_TAIL:
					return zonetool::h1::FX_ELEM_TYPE_TAIL;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_TRAIL:
					return zonetool::h1::FX_ELEM_TYPE_TRAIL;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_FLARE:
					return zonetool::h1::FX_ELEM_TYPE_FLARE;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
					return zonetool::h1::FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_CLOUD:
					return zonetool::h1::FX_ELEM_TYPE_CLOUD;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_SPARK_CLOUD:
					return zonetool::h1::FX_ELEM_TYPE_SPARK_CLOUD;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_SPARK_FOUNTAIN:
					return zonetool::h1::FX_ELEM_TYPE_SPARK_FOUNTAIN;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_MODEL:
					return zonetool::h1::FX_ELEM_TYPE_MODEL;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_OMNI_LIGHT:
					return zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_SPOT_LIGHT:
					return zonetool::h1::FX_ELEM_TYPE_SPOT_LIGHT;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_SOUND:
					return zonetool::h1::FX_ELEM_TYPE_SOUND;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_DECAL:
					return zonetool::h1::FX_ELEM_TYPE_DECAL;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_RUNNER:
					return zonetool::h1::FX_ELEM_TYPE_RUNNER;
					break;
				case zonetool::iw6::FX_ELEM_TYPE_VECTORFIELD:
					return zonetool::h1::FX_ELEM_TYPE_VECTORFIELD;
					break;
				}

				return zonetool::h1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
			}

			unsigned int convert_elem_flags(unsigned int flags)
			{
				unsigned int h1_flags = 0;
				auto convert = [&](unsigned int a, unsigned int b, unsigned int mask = 0)
				{
					h1_flags |= ((flags & (mask ? mask : a)) == a) ? b : 0;
				};

				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT, zonetool::h1::FxElemDefFlags::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL, zonetool::h1::FxElemDefFlags::FX_ELEM_SPAWN_FRUSTUM_CULL);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT, zonetool::h1::FxElemDefFlags::FX_ELEM_RUNNER_USES_RAND_ROT);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, zonetool::h1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_NONE, zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, zonetool::h1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_SPHERE, zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, zonetool::h1::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_CYLINDER, zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_OFFSET_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, zonetool::h1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_WORLD, zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, zonetool::h1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_SPAWN, zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, zonetool::h1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_EFFECT, zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, zonetool::h1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_OFFSET, zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA, zonetool::h1::FxElemDefFlags::FX_ELEM_RUN_RELATIVE_TO_CAMERA);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH, zonetool::h1::FxElemDefFlags::FX_ELEM_DIE_ON_TOUCH);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG, zonetool::h1::FxElemDefFlags::FX_ELEM_DRAW_PAST_FOG);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL, zonetool::h1::FxElemDefFlags::FX_ELEM_DRAW_WITH_VIEWMODEL);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT, zonetool::h1::FxElemDefFlags::FX_ELEM_BLOCK_SIGHT);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY, zonetool::h1::FxElemDefFlags::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY, zonetool::h1::FxElemDefFlags::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_EMIT_BOLT, zonetool::h1::FxElemDefFlags::FX_ELEM_EMIT_BOLT);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM, zonetool::h1::FxElemDefFlags::FX_ELEM_EMIT_ORIENT_BY_ELEM);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY, zonetool::h1::FxElemDefFlags::FX_ELEM_USE_OCCLUSION_QUERY);
				//convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_USE_CAST_SHADOW, zonetool::h1::FxElemDefFlags::FX_ELEM_USE_CAST_SHADOW);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW, zonetool::h1::FxElemDefFlags::FX_ELEM_NODRAW_IN_THERMAL_VIEW);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME, zonetool::h1::FxElemDefFlags::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT, zonetool::h1::FxElemDefFlags::FX_ELEM_RECEIVE_DYNAMIC_LIGHT);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL, zonetool::h1::FxElemDefFlags::FX_ELEM_VOLUMETRIC_TRAIL);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_USE_COLLISION, zonetool::h1::FxElemDefFlags::FX_ELEM_USE_COLLISION);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS, zonetool::h1::FxElemDefFlags::FX_ELEM_USE_VECTORFIELDS);
				//convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR, zonetool::h1::FxElemDefFlags::FX_ELEM_NO_SURFACE_HDR_SCALAR);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL, zonetool::h1::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD, zonetool::h1::FxElemDefFlags::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_HAS_GRAVITY, zonetool::h1::FxElemDefFlags::FX_ELEM_HAS_GRAVITY);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS, zonetool::h1::FxElemDefFlags::FX_ELEM_USE_MODEL_PHYSICS);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE, zonetool::h1::FxElemDefFlags::FX_ELEM_NONUNIFORM_SCALE);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, zonetool::h1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_CUBE, zonetool::iw6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, zonetool::h1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE, zonetool::iw6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, zonetool::h1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM, zonetool::iw6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, zonetool::h1::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL, zonetool::iw6::FxElemDefFlags::FX_ELEM_CLOUD_SHAPE_MASK);
				convert(zonetool::iw6::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION, zonetool::h1::FxElemDefFlags::FX_ELEM_FOUNTAIN_DISABLE_COLLISION);

				return h1_flags;
			}

			std::size_t string_size(const char* str)
			{
				return str ? std::strlen(str) + 1 : 0;
			}

			std::size_t visuals_size(const zonetool::h1::FxElemDef& elem)
			{
				if (elem.elemType == zonetool::h1::FX_ELEM_TYPE_DECAL)
				{
					return elem.visuals.markArray ? sizeof(zonetool::h1::FxElemMarkVisuals) * elem.visualCount : 0;
				}

				const auto has_strings = elem.elemType == zonetool::h1::FX_ELEM_TYPE_SOUND || elem.elemType == zonetool::h1::FX_ELEM_TYPE_VECTORFIELD;
				if (elem.visualCount <= 1)
				{
					return has_strings && elem.visualCount ? string_size(elem.visuals.instance.soundName) : 0;
				}

				if (!elem.visuals.array)
				{
					return 0;
				}

				auto size = sizeof(zonetool::h1::FxElemVisuals) * elem.visualCount;
				for (auto i = 0; has_strings && i < elem.visualCount; i++)
				{
					size += string_size(elem.visuals.array[i].soundName);
				}

				return size;
			}

			std::size_t extended_size(const zonetool::h1::FxElemDef& elem)
			{
				const auto& extended = elem.extended;
				if (!extended.unknownDef)
				{
					return 0;
				}

				switch (elem.elemType)
				{
				case zonetool::h1::FX_ELEM_TYPE_TRAIL:
					return sizeof(zonetool::h1::FxTrailDef)
						+ (extended.trailDef->verts ? sizeof(zonetool::h1::FxTrailVertex) * extended.trailDef->vertCount : 0)
						+ (extended.trailDef->inds ? sizeof(unsigned short) * extended.trailDef->indCount : 0);
				case zonetool::h1::FX_ELEM_TYPE_SPARK_FOUNTAIN:
					return sizeof(zonetool::h1::FxSparkFountainDef);
				case zonetool::h1::FX_ELEM_TYPE_SPOT_LIGHT:
					return sizeof(zonetool::h1::FxSpotLightDef);
				case zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT:
					return sizeof(zonetool::h1::FxOmniLightDef);
				case zonetool::h1::FX_ELEM_TYPE_FLARE:
				{
					const auto* flare = extended.flareDef;
					return sizeof(zonetool::h1::FxFlareDef) + sizeof(float) * (flare->intensityXIntervalCount + flare->intensityYIntervalCount
						+ flare->srcCosIntensityIntervalCount + flare->srcCosScaleIntervalCount + 4);
				}
				default:
					return 0;
				}
			}

			// stock h1 counts the defs, samples, visuals, extended defs and the effect name
			int total_size(const zonetool::h1::FxEffectDef* asset, const int elem_count)
			{
				auto size = sizeof(zonetool::h1::FxEffectDef) + sizeof(zonetool::h1::FxElemDef) * elem_count + string_size(asset->name);
				for (auto i = 0; i < elem_count; i++)
				{
					const auto& elem = asset->elemDefs[i];
					size += elem.velSamples ? sizeof(zonetool::h1::FxElemVelStateSample) * (elem.velIntervalCount + 1) : 0;
					size += elem.visSamples ? sizeof(zonetool::h1::FxElemVisStateSample) * (elem.visStateIntervalCount + 1) : 0;
					size += visuals_size(elem) + extended_size(elem);
				}

				return static_cast<int>(size);
			}

			// stock h1 sets these exactly when an elem runs relative to the camera, casts shadows, draws emissive or queries occlusion
			unsigned int elem_effect_flags(const zonetool::h1::FxElemDef& elem)
			{
				auto flags = 0u;
				if ((elem.flags & zonetool::h1::FX_ELEM_RUN_MASK) == zonetool::h1::FX_ELEM_RUN_RELATIVE_TO_CAMERA)
				{
					flags |= zonetool::h1::FX_EFFECT_HAS_CAMERA_RELATIVE_ELEM;
				}

				if (elem.flags & zonetool::h1::FX_ELEM_USE_CAST_SHADOW)
				{
					flags |= zonetool::h1::FX_EFFECT_NEEDS_CAST_SHADOW;
				}

				if (elem.flags & zonetool::h1::FX_ELEM_USE_OCCLUSION_QUERY || elem.flags2 & zonetool::h1::FX_ELEM2_USE_EMISSIVE_DRAW)
				{
					flags |= zonetool::h1::FX_EFFECT_NEEDS_EMISSIVE_DRAW;
				}

				return flags;
			}

			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::FxEffectDef>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(flags);
				COPY_VALUE(msecLoopingLife);
				COPY_VALUE(elemDefCountLooping);
				COPY_VALUE(elemDefCountOneShot);
				COPY_VALUE(elemDefCountEmission);
				COPY_VALUE(elemMaxRadius);
				COPY_VALUE(occlusionQueryDepthBias);
				COPY_VALUE(occlusionQueryFadeIn);
				COPY_VALUE(occlusionQueryFadeOut);
				COPY_VALUE_CAST(occlusionQueryScaleRange);
				
				auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;

				new_asset->elemDefs = allocator.allocate_array<zonetool::h1::FxElemDef>(count);
				for (auto elem_index = 0; elem_index < count; elem_index++)
				{
					[[maybe_unused]] bool is_looping = elem_index < asset->elemDefCountLooping;
					[[maybe_unused]] bool is_one_shot = elem_index >= asset->elemDefCountLooping && elem_index < asset->elemDefCountLooping + asset->elemDefCountOneShot;
					[[maybe_unused]] bool is_emission = elem_index >= asset->elemDefCountLooping + asset->elemDefCountOneShot && elem_index < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;

					auto* elem = &asset->elemDefs[elem_index];
					auto* new_elem = &new_asset->elemDefs[elem_index];

					if (elem->spawnFrustumCullRadius > new_asset->elemMaxRadius)
					{
						new_asset->elemMaxRadius = elem->spawnFrustumCullRadius;
					}

					new_elem->flags = static_cast<zonetool::h1::FxElemDefFlags>(convert_elem_flags(elem->flags));
					COPY_VALUE_CAST(elemDefs[elem_index].flags2);

					// distortion goes to the emissive draw list like stock, the emissive camera region only sets the emissive scale
					bool distortion = false;
					bool emissive = false;
					const auto emissive_check = [&](const Material* mat)
					{
						switch (elem->elemType)
						{
						case FX_ELEM_TYPE_SPRITE_BILLBOARD:
						case FX_ELEM_TYPE_SPRITE_ORIENTED:
						case FX_ELEM_TYPE_TAIL:
						case FX_ELEM_TYPE_TRAIL:
						{
							if (!mat)
							{
								break;
							}

							const std::string_view techset = mat->techniqueSet && mat->techniqueSet->name ? mat->techniqueSet->name : "";
							distortion |= std::string_view(mat->name).contains("distort") || techset.contains("distort");
							emissive |= distortion || mat->cameraRegion == CAMERA_REGION_EMISSIVE || mat->cameraRegion == CAMERA_REGION_DEPTH_HACK;
						}
						break;
						}
					};

					if (elem->elemType == FX_ELEM_TYPE_DECAL)
					{
					}
					else if (elem->visualCount > 1)
					{
						for (int i = 0; i < elem->visualCount; i++)
						{
							emissive_check(elem->visuals.array[i].material);
						}
					}
					else if (elem->visualCount)
					{
						emissive_check(elem->visuals.instance.material);
					}

					if (distortion)
					{
						new_elem->flags2 |= zonetool::h1::FX_ELEM2_USE_EMISSIVE_DRAW;
					}

					COPY_VALUE_CAST(elemDefs[elem_index].spawn);
					COPY_VALUE_CAST(elemDefs[elem_index].spawnRange);
					COPY_VALUE_CAST(elemDefs[elem_index].fadeInRange);
					COPY_VALUE_CAST(elemDefs[elem_index].fadeOutRange);
					COPY_VALUE_CAST(elemDefs[elem_index].spawnFrustumCullRadius);
					COPY_VALUE_CAST(elemDefs[elem_index].spawnDelayMsec);
					COPY_VALUE_CAST(elemDefs[elem_index].lifeSpanMsec);
					COPY_ARR(elemDefs[elem_index].spawnOrigin);
					COPY_VALUE_CAST(elemDefs[elem_index].spawnOffsetRadius);
					COPY_VALUE_CAST(elemDefs[elem_index].spawnOffsetHeight);
					COPY_ARR(elemDefs[elem_index].spawnAngles);
					COPY_ARR(elemDefs[elem_index].angularVelocity);
					COPY_VALUE_CAST(elemDefs[elem_index].initialRotation);
					COPY_VALUE_CAST(elemDefs[elem_index].gravity);
					COPY_VALUE_CAST(elemDefs[elem_index].reflectionFactor);
					COPY_VALUE_CAST(elemDefs[elem_index].atlas);
					new_elem->elemType = convert_elem_type(elem->elemType);

					// iw6 draws oriented sprites without the spawn angles, h1 rotates them by spawnAngles + angularVelocity
					if (new_elem->elemType == zonetool::h1::FX_ELEM_TYPE_SPRITE_ORIENTED)
					{
						std::memset(new_elem->spawnAngles, 0, sizeof(new_elem->spawnAngles));
						std::memset(new_elem->angularVelocity, 0, sizeof(new_elem->angularVelocity));
					}
					COPY_VALUE_CAST(elemDefs[elem_index].elemLitType);
					//new_elem->elemLitType = generate_elem_lit_type(elem->elemType);
					COPY_VALUE_CAST(elemDefs[elem_index].visualCount);
					COPY_VALUE_CAST(elemDefs[elem_index].velIntervalCount);
					COPY_VALUE_CAST(elemDefs[elem_index].visStateIntervalCount);
					REINTERPRET_CAST_SAFE(elemDefs[elem_index].velSamples);

					if (elem->visSamples)
					{
						new_elem->visSamples = allocator.allocate_array<zonetool::h1::FxElemVisStateSample>(elem->visStateIntervalCount + 1);
						for (int i = 0; i < elem->visStateIntervalCount + 1; i++)
						{
							// base
							new_elem->visSamples[i].base.color[0] = elem->visSamples[i].base.color[0];
							new_elem->visSamples[i].base.color[1] = elem->visSamples[i].base.color[1];
							new_elem->visSamples[i].base.color[2] = elem->visSamples[i].base.color[2];
							new_elem->visSamples[i].base.color[3] = elem->visSamples[i].base.color[3];
							new_elem->visSamples[i].base.emissiveScale[0] = 0.0f;
							new_elem->visSamples[i].base.emissiveScale[1] = 0.0f;
							new_elem->visSamples[i].base.emissiveScale[2] = 0.0f;
							new_elem->visSamples[i].base.rotationDelta = elem->visSamples[i].base.rotationDelta;
							new_elem->visSamples[i].base.rotationTotal = elem->visSamples[i].base.rotationTotal;
							memcpy(&new_elem->visSamples[i].base.size, &elem->visSamples[i].base.size, sizeof(float[2]));
							new_elem->visSamples[i].base.scale = elem->visSamples[i].base.scale;
							new_elem->visSamples[i].base.pivot[0] = 0.0f;
							new_elem->visSamples[i].base.pivot[1] = 0.0f;

							// amplitude
							new_elem->visSamples[i].amplitude.color[0] = elem->visSamples[i].amplitude.color[0];
							new_elem->visSamples[i].amplitude.color[1] = elem->visSamples[i].amplitude.color[1];
							new_elem->visSamples[i].amplitude.color[2] = elem->visSamples[i].amplitude.color[2];
							new_elem->visSamples[i].amplitude.color[3] = elem->visSamples[i].amplitude.color[3];
							new_elem->visSamples[i].amplitude.emissiveScale[0] = 0.0f;
							new_elem->visSamples[i].amplitude.emissiveScale[1] = 0.0f;
							new_elem->visSamples[i].amplitude.emissiveScale[2] = 0.0f;
							new_elem->visSamples[i].amplitude.rotationDelta = elem->visSamples[i].amplitude.rotationDelta;
							new_elem->visSamples[i].amplitude.rotationTotal = elem->visSamples[i].amplitude.rotationTotal;
							memcpy(&new_elem->visSamples[i].amplitude.size, &elem->visSamples[i].amplitude.size, sizeof(float[2]));
							new_elem->visSamples[i].amplitude.scale = elem->visSamples[i].amplitude.scale;
							new_elem->visSamples[i].amplitude.pivot[0] = 0.0f;
							new_elem->visSamples[i].amplitude.pivot[1] = 0.0f;

							if (emissive || new_elem->elemLitType == zonetool::h1::FxElemLitType::FX_ELEM_LIT_TYPE_NONE)
							{
								const auto convert_emissive = [&](const float* src, float* dst, float* emissive_scale)
								{
									const float litScale = 1.0f * elem->litHDRScalar;
									const float blend = elem->litUnlitBlendFactor;
									const float unlitScale = (1.0f - blend) * 1.0f * elem->unlitHDRScalar;

									// Original color components
									const float r = src[0];
									const float g = src[1];
									const float b = src[2];

									// Apply lit scaling
									const float litR = litScale * r;
									const float litG = litScale * g;
									const float litB = litScale * b;

									// Apply blended lighting
									dst[0] = (blend * 1.0f * litR) + (r * unlitScale);
									dst[1] = (blend * 1.0f * litG) + (g * unlitScale);
									dst[2] = (blend * 1.0f * litB) + (b * unlitScale);

									emissive_scale[0] = 1.0f;
									emissive_scale[1] = 1.0f;
									emissive_scale[2] = 1.0f;
								};

								convert_emissive(elem->visSamples[i].base.color, new_elem->visSamples[i].base.color, new_elem->visSamples[i].base.emissiveScale);
								convert_emissive(elem->visSamples[i].amplitude.color, new_elem->visSamples[i].amplitude.color, new_elem->visSamples[i].amplitude.emissiveScale);
							}
						}
					}

					// visuals
					if (elem->elemType == FX_ELEM_TYPE_DECAL)
					{
						if (elem->visuals.markArray)
						{
							new_elem->visuals.markArray = allocator.allocate_array<zonetool::h1::FxElemMarkVisuals>(elem->visualCount);

							for (unsigned char j = 0; j < elem->visualCount; j++)
							{
								new_elem->visuals.markArray[j].materials[0] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[0]); // mc
								new_elem->visuals.markArray[j].materials[1] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[1]); // wc
								new_elem->visuals.markArray[j].materials[2] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[1]); // wc displacement
							}
						}
					}
					else if (elem->visualCount > 1)
					{
						new_elem->visuals.array = allocator.allocate_array<zonetool::h1::FxElemVisuals>(elem->visualCount);
						for (int j = 0; j < elem->visualCount; j++)
						{
							// this is fine
							new_elem->visuals.array[j].anonymous = elem->visuals.array[j].anonymous;
						}
					}
					else if (elem->visualCount)
					{
						// this is fine
						new_elem->visuals.instance.anonymous = elem->visuals.instance.anonymous;
					}

					if (elem->elemType == FX_ELEM_TYPE_APEX_TURBULENCE || elem->elemType == FX_ELEM_TYPE_NVIDIA_HAIR)
					{
						ZONETOOL_WARNING("fx \"%s\": elem %d has iw6 elem type %d, which h1 lacks, it draws nothing", asset->name, elem_index, elem->elemType);
						new_elem->visualCount = 0;
						new_elem->visuals.instance.anonymous = nullptr;
					}

					COPY_VALUE_CAST(elemDefs[elem_index].collBounds);

					new_elem->effectOnImpact.name = elem->effectOnImpact.name;
					new_elem->effectOnDeath.name = elem->effectOnDeath.name;
					new_elem->effectEmitted.name = elem->effectEmitted.name;

					COPY_VALUE_CAST(elemDefs[elem_index].emitDist);
					COPY_VALUE_CAST(elemDefs[elem_index].emitDistVariance);

					switch (elem->elemType)
					{
					case FX_ELEM_TYPE_TRAIL:
						new_elem->extended.trailDef = allocator.allocate<zonetool::h1::FxTrailDef>();

						// check
						new_elem->extended.trailDef->scrollTimeMsec = elem->extended.trailDef->scrollTimeMsec;
						new_elem->extended.trailDef->repeatDist = elem->extended.trailDef->repeatDist;
						new_elem->extended.trailDef->invSplitDist = elem->extended.trailDef->invSplitDist;
						new_elem->extended.trailDef->invSplitArcDist = elem->extended.trailDef->invSplitArcDist;
						new_elem->extended.trailDef->invSplitTime = elem->extended.trailDef->invSplitTime;
						// pad

						new_elem->extended.trailDef->vertCount = elem->extended.trailDef->vertCount;
						new_elem->extended.trailDef->verts = allocator.allocate_array<zonetool::h1::FxTrailVertex>(new_elem->extended.trailDef->vertCount);
						for (int j = 0; j < new_elem->extended.trailDef->vertCount; j++)
						{
							// check
							memcpy(&new_elem->extended.trailDef->verts[j].pos, &elem->extended.trailDef->verts[j].pos, sizeof(float[2]));
							memcpy(&new_elem->extended.trailDef->verts[j].normal, &elem->extended.trailDef->verts[j].normal, sizeof(float[2]));
							memcpy(&new_elem->extended.trailDef->verts[j].texCoord, &elem->extended.trailDef->verts[j].texCoord, sizeof(float[2]));
							// pad
						}

						new_elem->extended.trailDef->indCount = elem->extended.trailDef->indCount;
						new_elem->extended.trailDef->inds = elem->extended.trailDef->inds;
						break;
					case FX_ELEM_TYPE_SPARK_FOUNTAIN:
						// should be fine
						new_elem->extended.sparkFountainDef = reinterpret_cast<zonetool::h1::FxSparkFountainDef*>(elem->extended.sparkFountainDef);
						break;
					case FX_ELEM_TYPE_SPOT_LIGHT:
						new_elem->extended.spotLightDef = allocator.allocate<zonetool::h1::FxSpotLightDef>();
						if (elem->extended.spotLightDef)
						{
							const auto* src = elem->extended.spotLightDef;
							auto* dst = new_elem->extended.spotLightDef;

							// iw6 R_AddSpotLightToScene math, with the max length in place of the per frame light radius
							const auto radius = std::max(src->maxLength, 0.2f);
							const auto end_radius = std::clamp(src->endRadius, src->startRadius + 0.1f, src->startRadius + radius - 0.1f);
							dst->halfFovOuter = std::max(std::atan((end_radius - src->startRadius) / radius), 0.01f);
							dst->halfFovInner = dst->halfFovOuter * src->fovInnerFraction;
							dst->radius = radius;
							dst->brightness = src->brightness;
							dst->maxLength = src->maxLength;
							dst->exponent = src->exponent;
							dst->nearClip = 0.01f;
							dst->bulbRadius = src->startRadius;
							dst->bulbLength = 0.0f;
							dst->fadeOffsetRt[0] = 0.0f;
							dst->fadeOffsetRt[1] = 1.0f;
						}
						break;
					case FX_ELEM_TYPE_OMNI_LIGHT:
						new_elem->extended.omniLightDef = allocator.allocate<zonetool::h1::FxOmniLightDef>();
						new_elem->extended.omniLightDef->bulbRadius = 1.0f;
						new_elem->extended.omniLightDef->bulbLength = 0.0f;
						new_elem->extended.omniLightDef->fadeOffsetRt[0] = 0.0f;
						new_elem->extended.omniLightDef->fadeOffsetRt[1] = -1.0f;
						break;
					case FX_ELEM_TYPE_FLARE:
						// should be fine
						new_elem->extended.flareDef = reinterpret_cast<zonetool::h1::FxFlareDef*>(elem->extended.flareDef);
						break;
					default:
						new_elem->extended.unknownDef = elem->extended.unknownDef;
						break;
					}

					COPY_VALUE(elemDefs[elem_index].sortOrder);
					COPY_VALUE(elemDefs[elem_index].lightingFrac);
					COPY_VALUE(elemDefs[elem_index].useItemClip);
					COPY_VALUE(elemDefs[elem_index].fadeInfo);
					new_elem->fadeOutInfo = 0;
					COPY_VALUE(elemDefs[elem_index].randomSeed);
					
					new_elem->emissiveScaleScale = 0.0f;
					new_elem->hdrLightingFrac = 1.0f;
					new_elem->shadowDensityScale = 1.0f;
					new_elem->scatterRatio = 0.0f;
					new_elem->volumetricTrailFadeStart = -1.0f;

					if (emissive)
					{
						new_elem->hdrLightingFrac = 0.0f;
						new_elem->emissiveScaleScale = 1.0f;
					}
				}

				constexpr auto elem_flags = zonetool::h1::FX_EFFECT_HAS_CAMERA_RELATIVE_ELEM | zonetool::h1::FX_EFFECT_NEEDS_CAST_SHADOW | zonetool::h1::FX_EFFECT_NEEDS_EMISSIVE_DRAW;
				auto flags = static_cast<unsigned int>(asset->flags) & ~static_cast<unsigned int>(elem_flags);
				for (auto i = 0; i < count; i++)
				{
					flags |= elem_effect_flags(new_asset->elemDefs[i]);
				}

				new_asset->flags = static_cast<decltype(new_asset->flags)>(flags);
				new_asset->totalSize = total_size(new_asset, count);

				return new_asset;
			}

			void dump(FxEffectDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::fx_effect_def::dump(converted_asset);
			}
		}
	}
}