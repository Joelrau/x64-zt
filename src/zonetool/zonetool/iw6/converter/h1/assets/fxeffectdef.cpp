#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "fxeffectdef.hpp"

#include "zonetool/h1/assets/fxeffectdef.hpp"

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

			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::FxEffectDef>();

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
				COPY_VALUE_CAST(occlusionQueryScaleRange);
				
				auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
				new_asset->elemDefs = allocator.allocate_array<zonetool::h1::FxElemDef>(count);
				for (auto elem_index = 0; elem_index < count; elem_index++)
				{
					auto* elem = &asset->elemDefs[elem_index];
					auto* new_elem = &new_asset->elemDefs[elem_index];

					if (elem->spawnFrustumCullRadius > new_asset->elemMaxRadius)
					{
						new_asset->elemMaxRadius = elem->spawnFrustumCullRadius;
					}

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
							std::string mat_name = mat->name;
							if (mat_name.find("distort") != std::string::npos || mat->cameraRegion == CAMERA_REGION_EMISSIVE)
							{
								emissive = true;
							}
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

					new_elem->flags = static_cast<zonetool::h1::FxElemDefFlags>(convert_elem_flags(elem->flags));
					COPY_VALUE_CAST(elemDefs[elem_index].flags2);

					if (emissive)
					{
						new_asset->flags |= zonetool::h1::FX_EFFECT_NEEDS_EMISSIVE_DRAW;
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
					COPY_VALUE_CAST(elemDefs[elem_index].elemLitType); // convert?
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
							new_elem->visSamples[i].amplitude.pivot[1] = 0.0f;
							new_elem->visSamples[i].amplitude.pivot[1] = 0.0f;

							if (emissive)
							{
								new_elem->visSamples[i].base.emissiveScale[0] = 1.0f;
								new_elem->visSamples[i].base.emissiveScale[1] = 1.0f;
								new_elem->visSamples[i].base.emissiveScale[2] = 1.0f;

								new_elem->visSamples[i].amplitude.emissiveScale[0] = 1.0f;
								new_elem->visSamples[i].amplitude.emissiveScale[1] = 1.0f;
								new_elem->visSamples[i].amplitude.emissiveScale[2] = 1.0f;
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
						// check
						new_elem->extended.spotLightDef = allocator.allocate<zonetool::h1::FxSpotLightDef>();
						if (elem->extended.spotLightDef)
						{
							// ai giving you the following code:

							auto* dst = new_elem->extended.spotLightDef;
							auto* src = elem->extended.spotLightDef;

							// Defensive clamps
							const float epsilon = 1e-4f;
							const float maxLen = std::max(src->maxLength, epsilon);
							const float r0 = std::max(src->startRadius, 0.0f);
							const float r1 = std::max(src->endRadius, 0.0f);
							const float dR = std::max(r1 - r0, 0.0f);                 // beam widening
							const float frac = std::clamp(src->fovInnerFraction, 0.0f, 1.0f);

							// Half-angles (radians) from beam slope.
							// Outer: atan( (end-start) / length )
							// Inner uses the same slope scaled by the fraction.
							const float halfFovOuter = std::atan2(dR, maxLen);
							const float halfFovInner = std::atan2(frac * dR, maxLen);

							// Assign to H1
							dst->halfFovOuter = halfFovOuter;
							dst->halfFovInner = std::min(halfFovOuter, halfFovInner);      // safety
							dst->radius = r1;                                         // end radius at max distance
							dst->brightness = src->brightness;                            // might need a scale, engine-dependent
							dst->maxLength = maxLen;
							dst->exponent = src->exponent;

							// Use startRadius to populate emitter/bulb; near clip small to avoid acne
							dst->bulbRadius = r0;                                         // emitter base radius
							dst->bulbLength = 0.0f;                                       // unknown in src; 0 is safest
							dst->nearClip = 0.01f;                                      // tiny, or tie to % of maxLen if you prefer

							// Keep your fade ramp unless you have better data
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

					// hdr
					if (new_elem->elemLitType == zonetool::h1::FX_ELEM_LIT_TYPE_NONE)
					{
						new_elem->hdrLightingFrac = 0.0f;
					}

					if (emissive)
					{
						new_elem->hdrLightingFrac = 0.0f;
						new_elem->emissiveScaleScale = 1.0f;
					}
				}

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