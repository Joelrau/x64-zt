#include <std_include.hpp>

#include "../include.hpp"
#include "fx.hpp"

#include <iw7/assets/fxeffectdef.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace fx
	{
		namespace
		{
			zonetool::iw7::FxElemType convert_elem_type(const unsigned char type)
			{
				switch (type)
				{
				case zonetool::iw5::FX_ELEM_TYPE_SPRITE_BILLBOARD: return zonetool::iw7::FX_ELEM_TYPE_SPRITE_BILLBOARD;
				case zonetool::iw5::FX_ELEM_TYPE_SPRITE_ORIENTED: return zonetool::iw7::FX_ELEM_TYPE_SPRITE_ORIENTED;
				case zonetool::iw5::FX_ELEM_TYPE_TAIL: return zonetool::iw7::FX_ELEM_TYPE_TAIL;
				case zonetool::iw5::FX_ELEM_TYPE_TRAIL: return zonetool::iw7::FX_ELEM_TYPE_TRAIL;
				case zonetool::iw5::FX_ELEM_TYPE_CLOUD: return zonetool::iw7::FX_ELEM_TYPE_CLOUD;
				case zonetool::iw5::FX_ELEM_TYPE_SPARKCLOUD: return zonetool::iw7::FX_ELEM_TYPE_SPARK_CLOUD;
				case zonetool::iw5::FX_ELEM_TYPE_SPARKFOUNTAIN: return zonetool::iw7::FX_ELEM_TYPE_SPARK_FOUNTAIN;
				case zonetool::iw5::FX_ELEM_TYPE_MODEL: return zonetool::iw7::FX_ELEM_TYPE_MODEL;
				case zonetool::iw5::FX_ELEM_TYPE_OMNI_LIGHT: return zonetool::iw7::FX_ELEM_TYPE_OMNI_LIGHT;
				case zonetool::iw5::FX_ELEM_TYPE_SPOT_LIGHT: return zonetool::iw7::FX_ELEM_TYPE_SPOT_LIGHT;
				case zonetool::iw5::FX_ELEM_TYPE_SOUND: return zonetool::iw7::FX_ELEM_TYPE_SOUND;
				case zonetool::iw5::FX_ELEM_TYPE_DECAL: return zonetool::iw7::FX_ELEM_TYPE_DECAL;
				case zonetool::iw5::FX_ELEM_TYPE_RUNNER: return zonetool::iw7::FX_ELEM_TYPE_RUNNER;
				default: return zonetool::iw7::FX_ELEM_TYPE_SPRITE_BILLBOARD;
				}
			}

			unsigned int convert_flags(const unsigned int flags)
			{
				unsigned int result = 0;
				const auto convert = [&](const unsigned int from, const unsigned int to, const unsigned int mask = 0)
				{
					result |= ((flags & (mask ? mask : from)) == from) ? to : 0;
				};

				using source = zonetool::iw5::FxElemDefFlags;
				using target = zonetool::iw7::FxElemDefFlags;

				convert(source::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT, target::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT);
				convert(source::FX_ELEM_SPAWN_FRUSTUM_CULL, target::FX_ELEM_SPAWN_FRUSTUM_CULL);
				convert(source::FX_ELEM_RUNNER_USES_RAND_ROT, target::FX_ELEM_RUNNER_USES_RAND_ROT);
				convert(source::FX_ELEM_SPAWN_OFFSET_NONE, target::FX_ELEM_SPAWN_OFFSET_NONE, source::FX_ELEM_SPAWN_OFFSET_MASK);
				convert(source::FX_ELEM_SPAWN_OFFSET_SPHERE, target::FX_ELEM_SPAWN_OFFSET_SPHERE, source::FX_ELEM_SPAWN_OFFSET_MASK);
				convert(source::FX_ELEM_SPAWN_OFFSET_CYLINDER, target::FX_ELEM_SPAWN_OFFSET_CYLINDER, source::FX_ELEM_SPAWN_OFFSET_MASK);
				convert(source::FX_ELEM_RUN_RELATIVE_TO_WORLD, target::FX_ELEM_RUN_RELATIVE_TO_WORLD, source::FX_ELEM_RUN_MASK);
				convert(source::FX_ELEM_RUN_RELATIVE_TO_SPAWN, target::FX_ELEM_RUN_RELATIVE_TO_SPAWN, source::FX_ELEM_RUN_MASK);
				convert(source::FX_ELEM_RUN_RELATIVE_TO_EFFECT, target::FX_ELEM_RUN_RELATIVE_TO_EFFECT, source::FX_ELEM_RUN_MASK);
				convert(source::FX_ELEM_RUN_RELATIVE_TO_OFFSET, target::FX_ELEM_RUN_RELATIVE_TO_OFFSET, source::FX_ELEM_RUN_MASK);
				convert(source::FX_ELEM_DIE_ON_TOUCH, target::FX_ELEM_DIE_ON_TOUCH);
				convert(source::FX_ELEM_DRAW_PAST_FOG, target::FX_ELEM_DRAW_PAST_FOG);
				convert(source::FX_ELEM_DRAW_WITH_VIEWMODEL, target::FX_ELEM_DRAW_WITH_VIEWMODEL);
				convert(source::FX_ELEM_BLOCK_SIGHT, target::FX_ELEM_BLOCK_SIGHT);
				convert(source::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY, target::FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY);
				convert(source::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY, target::FX_ELEM_TRAIL_ORIENT_BY_VELOCITY);
				convert(source::FX_ELEM_EMIT_ORIENT_BY_ELEM, target::FX_ELEM_EMIT_ORIENT_BY_ELEM);
				convert(source::FX_ELEM_USE_OCCLUSION_QUERY, target::FX_ELEM_USE_OCCLUSION_QUERY);
				convert(source::FX_ELEM_USE_COLLISION, target::FX_ELEM_USE_COLLISION);
				convert(source::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL, target::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL);
				convert(source::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD, target::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD);
				convert(source::FX_ELEM_HAS_GRAVITY, target::FX_ELEM_HAS_GRAVITY);
				convert(source::FX_ELEM_USE_MODEL_PHYSICS, target::FX_ELEM_USE_MODEL_PHYSICS);
				convert(source::FX_ELEM_NONUNIFORM_SCALE, target::FX_ELEM_NONUNIFORM_SCALE);
				convert(source::FX_ELEM_FOUNTAIN_DISABLE_COLLISION, target::FX_ELEM_FOUNTAIN_DISABLE_COLLISION);

				return result;
			}

			zonetool::iw7::Material* material_reference(const char* name, utils::memory::allocator& allocator)
			{
				const auto reference = allocator.allocate<zonetool::iw7::Material>();
				reference->name = allocator.duplicate_string(name);
				return reference;
			}

			zonetool::iw7::XModel* model_reference(const char* name, utils::memory::allocator& allocator)
			{
				const auto reference = allocator.allocate<zonetool::iw7::XModel>();
				reference->name = allocator.duplicate_string(name);
				return reference;
			}

			zonetool::iw7::FxEffectDef* effect_reference(const char* name, utils::memory::allocator& allocator)
			{
				const auto reference = allocator.allocate<zonetool::iw7::FxEffectDef>();
				reference->name = allocator.duplicate_string(name);
				return reference;
			}

			void set_effect(zonetool::iw7::FxEffectDefRef& destination, const x86::pointer<const char>& name, utils::memory::allocator& allocator)
			{
				if (const auto text = name.get())
				{
					destination.handle = effect_reference(text, allocator);
				}
			}

			void convert_visuals(const zonetool::iw5::FxElemDef* element, const zonetool::iw5::FxElemVisuals& source,
				zonetool::iw7::FxElemVisuals& destination, utils::memory::allocator& allocator)
			{
				switch (element->elemType)
				{
				case zonetool::iw5::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				case zonetool::iw5::FX_ELEM_TYPE_SPRITE_ORIENTED:
				case zonetool::iw5::FX_ELEM_TYPE_TAIL:
				case zonetool::iw5::FX_ELEM_TYPE_TRAIL:
				case zonetool::iw5::FX_ELEM_TYPE_CLOUD:
				case zonetool::iw5::FX_ELEM_TYPE_SPARKCLOUD:
				case zonetool::iw5::FX_ELEM_TYPE_SPARKFOUNTAIN:
					if (const auto material = source.material.get())
					{
						destination.material = material_reference(material->name.get(), allocator);
					}
					break;
				case zonetool::iw5::FX_ELEM_TYPE_MODEL:
					if (const auto model = source.model.get())
					{
						destination.model = model_reference(model->name.get(), allocator);
					}
					break;
				case zonetool::iw5::FX_ELEM_TYPE_RUNNER:
					if (const auto name = static_cast<const char*>(x86::pointer<const char>{source.anonymous.value}.get()))
					{
						destination.effectDef.handle = effect_reference(name, allocator);
					}
					break;
				case zonetool::iw5::FX_ELEM_TYPE_SOUND:
					if (const auto name = x86::pointer<const char>{source.anonymous.value}.get())
					{
						destination.soundName = allocator.duplicate_string(name);
					}
					break;
				default:
					break;
				}
			}

			void convert_visual_state(const zonetool::iw5::FxElemVisualState& source, zonetool::iw7::FxElemVisualState& destination)
			{
				destination.color[0] = static_cast<float>(source.color[2]) / 255.0f; // BGRA bytes to RGBA floats
				destination.color[1] = static_cast<float>(source.color[1]) / 255.0f;
				destination.color[2] = static_cast<float>(source.color[0]) / 255.0f;
				destination.color[3] = static_cast<float>(source.color[3]) / 255.0f;
				destination.rotationDelta = source.rotationDelta;
				destination.rotationTotal = source.rotationTotal;
				destination.size[0] = source.size[0];
				destination.size[1] = source.size[1];
				destination.scale = source.scale;
			}

			void convert_element(const zonetool::iw5::FxElemDef* element, zonetool::iw7::FxElemDef* result, utils::memory::allocator& allocator)
			{
				result->flags = static_cast<int>(convert_flags(element->flags));
				result->flags2 = 0;
				result->spawn = *reinterpret_cast<const zonetool::iw7::FxSpawnDef*>(&element->spawn);
				std::memcpy(&result->spawnRange, &element->spawnRange, sizeof(result->spawnRange));
				std::memcpy(&result->fadeInRange, &element->fadeInRange, sizeof(result->fadeInRange));
				std::memcpy(&result->fadeOutRange, &element->fadeOutRange, sizeof(result->fadeOutRange));
				result->spawnFrustumCullRadius = element->spawnFrustumCullRadius;
				std::memcpy(&result->spawnDelayMsec, &element->spawnDelayMsec, sizeof(result->spawnDelayMsec));
				std::memcpy(&result->lifeSpanMsec, &element->lifeSpanMsec, sizeof(result->lifeSpanMsec));
				std::memcpy(result->spawnOrigin, element->spawnOrigin, sizeof(result->spawnOrigin));
				std::memcpy(&result->spawnOffsetRadius, &element->spawnOffsetRadius, sizeof(result->spawnOffsetRadius));
				std::memcpy(&result->spawnOffsetHeight, &element->spawnOffsetHeight, sizeof(result->spawnOffsetHeight));
				std::memcpy(result->spawnAngles, element->spawnAngles, sizeof(result->spawnAngles));
				std::memcpy(result->angularVelocity, element->angularVelocity, sizeof(result->angularVelocity));
				std::memcpy(&result->initialRotation, &element->initialRotation, sizeof(result->initialRotation));
				std::memcpy(&result->gravity, &element->gravity, sizeof(result->gravity));
				std::memcpy(&result->reflectionFactor, &element->reflectionFactor, sizeof(result->reflectionFactor));

				result->atlas.behavior = element->atlas.behavior;
				result->atlas.index = element->atlas.index;
				result->atlas.fps = element->atlas.fps;
				result->atlas.loopCount = element->atlas.loopCount;
				result->atlas.colIndexBits = element->atlas.colIndexBits;
				result->atlas.rowIndexBits = element->atlas.rowIndexBits;
				result->atlas.entryCount = element->atlas.entryCount;

				result->elemType = convert_elem_type(static_cast<unsigned char>(element->elemType));
				result->visualCount = element->visualCount;
				result->velIntervalCount = element->velIntervalCount;
				result->visStateIntervalCount = element->visStateIntervalCount;

				static_assert(sizeof(zonetool::iw5::FxElemVelStateSample) == sizeof(zonetool::iw7::FxElemVelStateSample));
				if (const auto velocity = element->velSamples.get())
				{
					const auto count = element->velIntervalCount + 1;
					result->velSamples = allocator.allocate_array<zonetool::iw7::FxElemVelStateSample>(count);
					std::memcpy(result->velSamples, velocity, sizeof(zonetool::iw7::FxElemVelStateSample) * count);
				}

				if (const auto visual = element->visSamples.get())
				{
					const auto count = element->visStateIntervalCount + 1;
					result->visSamples = allocator.allocate_array<zonetool::iw7::FxElemVisStateSample>(count);
					for (auto i = 0; i < count; i++)
					{
						convert_visual_state(visual[i].base, result->visSamples[i].base);
						convert_visual_state(visual[i].amplitude, result->visSamples[i].amplitude);
					}
				}

				if (element->elemType == zonetool::iw5::FX_ELEM_TYPE_DECAL)
				{
					if (const auto marks = element->visuals.markArray.get())
					{
						result->visuals.markArray = allocator.allocate_array<zonetool::iw7::FxElemMarkVisuals>(element->visualCount);
						for (auto i = 0; i < element->visualCount; i++)
						{
							const auto first = marks[i].materials[0].get();
							const auto second = marks[i].materials[1].get();
							if (first)
							{
								result->visuals.markArray[i].materials[0] = material_reference(first->name.get(), allocator);
							}
							if (second)
							{
								result->visuals.markArray[i].materials[1] = material_reference(second->name.get(), allocator);
								result->visuals.markArray[i].materials[2] = material_reference(second->name.get(), allocator); // displacement decal reuses the second material
							}
						}
					}
				}
				else if (element->visualCount > 1)
				{
					if (const auto array = element->visuals.array.get())
					{
						result->visuals.array = allocator.allocate_array<zonetool::iw7::FxElemVisuals>(element->visualCount);
						for (auto i = 0; i < element->visualCount; i++)
						{
							convert_visuals(element, array[i], result->visuals.array[i], allocator);
						}
					}
				}
				else if (element->visualCount)
				{
					convert_visuals(element, element->visuals.instance, result->visuals.instance, allocator);
				}

				std::memcpy(result->collBounds.midPoint, element->collBounds.midPoint, sizeof(result->collBounds.midPoint));
				std::memcpy(result->collBounds.halfSize, element->collBounds.halfSize, sizeof(result->collBounds.halfSize));

				set_effect(result->effectOnImpact, element->effectOnImpact.name, allocator);
				set_effect(result->effectOnDeath, element->effectOnDeath.name, allocator);
				set_effect(result->effectEmitted, element->effectEmitted.name, allocator);
				std::memcpy(&result->emitDist, &element->emitDist, sizeof(result->emitDist));
				std::memcpy(&result->emitDistVariance, &element->emitDistVariance, sizeof(result->emitDistVariance));

				switch (element->elemType)
				{
				case zonetool::iw5::FX_ELEM_TYPE_TRAIL:
					if (const auto trail = element->extended.trailDef.get())
					{
						const auto copy = allocator.allocate<zonetool::iw7::FxTrailDef>();
						copy->scrollTimeMsec = trail->scrollTimeMsec;
						copy->repeatDist = trail->repeatDist;
						copy->invSplitDist = trail->invSplitDist;
						copy->invSplitArcDist = trail->invSplitArcDist;
						copy->invSplitTime = trail->invSplitTime;
						copy->vertCount = trail->vertCount;
						copy->verts = allocator.allocate_array<zonetool::iw7::FxTrailVertex>(trail->vertCount);
						for (auto i = 0; i < trail->vertCount; i++)
						{
							const auto vertex = &trail->verts.get()[i];
							copy->verts[i].pos[0] = vertex->pos[0];
							copy->verts[i].pos[1] = vertex->pos[1];
							copy->verts[i].normal[0] = vertex->normal[0];
							copy->verts[i].normal[1] = vertex->normal[1];
							copy->verts[i].texCoord[0] = vertex->texCoord;
							copy->verts[i].texCoord[1] = 0.0f; // IW5 stores a single texture coordinate
						}
						copy->indCount = trail->indCount;
						copy->inds = allocator.allocate_array<unsigned short>(trail->indCount);
						std::memcpy(copy->inds, trail->inds.get(), sizeof(unsigned short) * trail->indCount);
						result->extended.trailDef = copy;
					}
					break;
				case zonetool::iw5::FX_ELEM_TYPE_SPARKFOUNTAIN:
					if (const auto fountain = element->extended.sparkFountainDef.get())
					{
						static_assert(sizeof(zonetool::iw5::FxSparkFountainDef) == sizeof(zonetool::iw7::FxSparkFountainDef));
						result->extended.sparkFountainDef = allocator.allocate<zonetool::iw7::FxSparkFountainDef>();
						std::memcpy(result->extended.sparkFountainDef, fountain, sizeof(zonetool::iw7::FxSparkFountainDef));
					}
					break;
				case zonetool::iw5::FX_ELEM_TYPE_SPOT_LIGHT:
					result->extended.spotLightDef = allocator.allocate<zonetool::iw7::FxSpotLightDef>();
					if (const auto spot = element->extended.spotLightDef.get())
					{
						std::memcpy(result->extended.spotLightDef, spot, sizeof(zonetool::iw7::FxSpotLightDef));
					}
					break;
				default:
					break;
				}

				result->sortOrder = element->sortOrder;
				result->lightingFrac = element->lightingFrac;
				result->useItemClip = element->useItemClip;
				result->fadeInfo = element->fadeInfo;
				result->randomSeed = element->randomSeed;
				result->litUnlitBlendFactor = 1.0f;
			}
		}

		zonetool::iw7::FxEffectDef* convert(zonetool::iw5::FxEffectDef* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw7::FxEffectDef>();

			new_asset->name = asset->name.get();
			new_asset->flags = asset->flags;
			new_asset->totalSize = asset->totalSize;
			new_asset->msecLoopingLife = asset->msecLoopingLife;
			new_asset->elemDefCountLooping = asset->elemDefCountLooping;
			new_asset->elemDefCountOneShot = asset->elemDefCountOneShot;
			new_asset->elemDefCountEmission = asset->elemDefCountEmission;
			new_asset->elemMaxRadius = 0.0f;
			new_asset->occlusionQueryDepthBias = asset->occlusionQueryDepthBias;
			new_asset->occlusionQueryFadeIn = asset->occlusionQueryFadeIn;
			new_asset->occlusionQueryFadeOut = asset->occlusionQueryFadeOut;
			new_asset->occlusionQueryScaleRange.base = asset->occlusionQueryScaleRange.base;
			new_asset->occlusionQueryScaleRange.amplitude = asset->occlusionQueryScaleRange.amplitude;

			const auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
			new_asset->elemDefs = allocator.allocate_array<zonetool::iw7::FxElemDef>(count);
			for (auto i = 0; i < count; i++)
			{
				convert_element(&asset->elemDefs.get()[i], &new_asset->elemDefs[i], allocator);
			}

			return new_asset;
		}

		void dump(zonetool::iw5::FxEffectDef* asset)
		{
			utils::memory::allocator allocator;
			zonetool::iw7::fx_effect_def::dump(convert(asset, allocator));
		}
	}
}
