#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "fxeffectdef.hpp"
#include "particlesystem.hpp"
#include "createfx.hpp"

#include "zonetool/h1/assets/fxeffectdef.hpp"

// v102 fx: first-pass IW7 legacy FxEffectDef -> H1 FxEffectDef.
//   Converted : sprite billboard/oriented, tail, trail, flare, cloud, spark cloud, spark fountain, model, omni light,
//               spot light (-> H1 omni light, see below), decal, runner.
//   Skipped   : sound (H1 zone build needs the alias), vector field, particle sim animation (no IW7->H1 asset conversion).
//   Elements whose material / model / child effect was not dumped by this session are dropped as well, so the H1 zone
//   build never hits a missing asset. Effects left with no element are not written at all (and count as missing).
//   IW7's new particle system (ASSET_TYPE_VFX / ParticleSystemDef) is NOT converted here; see createfx.cpp.

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			namespace
			{
				// v102 fx: what this session dumped for H1, by type (materials, models, ...)
				std::unordered_map<std::int32_t, std::unordered_set<std::string>> dumped_assets;
				// v102 fx: queued IW7 effects (zone memory stays valid until stop_dumping)
				std::vector<FxEffectDef*> queued_effects;
				std::unordered_set<std::string> queued_names;
				// v102 fx: effects that were written as H1 json
				std::unordered_set<std::string> written_effects;

				using alive_set = std::unordered_set<std::string>;

				zonetool::h1::FxElemType convert_elem_type(FxElemType type)
				{
					switch (type)
					{
					case FX_ELEM_TYPE_SPRITE_BILLBOARD: return zonetool::h1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
					case FX_ELEM_TYPE_SPRITE_ORIENTED: return zonetool::h1::FX_ELEM_TYPE_SPRITE_ORIENTED;
					case FX_ELEM_TYPE_TAIL: return zonetool::h1::FX_ELEM_TYPE_TAIL;
					case FX_ELEM_TYPE_TRAIL: return zonetool::h1::FX_ELEM_TYPE_TRAIL;
					case FX_ELEM_TYPE_FLARE: return zonetool::h1::FX_ELEM_TYPE_FLARE;
					case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION: return zonetool::h1::FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION;
					case FX_ELEM_TYPE_CLOUD: return zonetool::h1::FX_ELEM_TYPE_CLOUD;
					case FX_ELEM_TYPE_SPARK_CLOUD: return zonetool::h1::FX_ELEM_TYPE_SPARK_CLOUD;
					case FX_ELEM_TYPE_SPARK_FOUNTAIN: return zonetool::h1::FX_ELEM_TYPE_SPARK_FOUNTAIN;
					case FX_ELEM_TYPE_MODEL: return zonetool::h1::FX_ELEM_TYPE_MODEL;
					case FX_ELEM_TYPE_OMNI_LIGHT: return zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT;
					// v102 fx: H1 spot lights need an H1 GfxLightDef (IW7 light defs are not converted) -> omni light
					case FX_ELEM_TYPE_SPOT_LIGHT: return zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT;
					case FX_ELEM_TYPE_SOUND: return zonetool::h1::FX_ELEM_TYPE_SOUND;
					case FX_ELEM_TYPE_DECAL: return zonetool::h1::FX_ELEM_TYPE_DECAL;
					case FX_ELEM_TYPE_RUNNER: return zonetool::h1::FX_ELEM_TYPE_RUNNER;
					case FX_ELEM_TYPE_VECTORFIELD: return zonetool::h1::FX_ELEM_TYPE_VECTORFIELD;
					default: break;
					}
					return zonetool::h1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
				}

				bool is_material_elem(FxElemType type)
				{
					switch (type)
					{
					case FX_ELEM_TYPE_SPRITE_BILLBOARD:
					case FX_ELEM_TYPE_SPRITE_ORIENTED:
					case FX_ELEM_TYPE_TAIL:
					case FX_ELEM_TYPE_TRAIL:
					case FX_ELEM_TYPE_FLARE:
					case FX_ELEM_TYPE_CLOUD:
					case FX_ELEM_TYPE_SPARK_CLOUD:
					case FX_ELEM_TYPE_SPARK_FOUNTAIN:
						return true;
					default:
						return false;
					}
				}

				bool is_sprite_elem(FxElemType type)
				{
					switch (type)
					{
					case FX_ELEM_TYPE_SPRITE_BILLBOARD:
					case FX_ELEM_TYPE_SPRITE_ORIENTED:
					case FX_ELEM_TYPE_TAIL:
					case FX_ELEM_TYPE_TRAIL:
					case FX_ELEM_TYPE_CLOUD:
						return true;
					default:
						return false;
					}
				}

				bool is_skipped_elem(FxElemType type)
				{
					return type == FX_ELEM_TYPE_SOUND || type == FX_ELEM_TYPE_VECTORFIELD || type == FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION
						|| type >= FX_ELEM_TYPE_COUNT;
				}

				// v102 fx: IW7 elem flag bits -> H1 bits (H1 inserted USE_CAST_SHADOW at 0x20000 and RECEIVE_DYNAMIC_LIGHT at 0x100000)
				unsigned int convert_elem_flags(unsigned int flags)
				{
					constexpr unsigned int same_bits = 0x0001FFFEu   // spawn/run/offset ... use occlusion query
						| 0x1F000000u                                 // velocity graphs, gravity, model physics, nonuniform scale
						| 0x60000000u                                 // cloud shape
						| 0x80000000u;                                // emit bolt / fountain disable collision
					unsigned int out = flags & same_bits;
					if (flags & FX_ELEM_NODRAW_IN_THERMAL_VIEW) out |= zonetool::h1::FX_ELEM_NODRAW_IN_THERMAL_VIEW;
					if (flags & FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME) out |= zonetool::h1::FX_ELEM_SPAWN_IMPACT_FX_WITH_SURFACE_NAME;
					if (flags & FX_ELEM_VOLUMETRIC_TRAIL) out |= zonetool::h1::FX_ELEM_VOLUMETRIC_TRAIL;
					if (flags & FX_ELEM_USE_COLLISION) out |= zonetool::h1::FX_ELEM_USE_COLLISION;
					// FX_ELEM_DEPRECATED and FX_ELEM_USE_VECTORFIELDS are dropped (vector fields are not converted)
					return out;
				}

				unsigned int convert_effect_flags(unsigned int flags)
				{
					unsigned int out = 0;
					if (flags & (FX_EFFECT_NEEDS_LIGHTING_AT_SPAWN | FX_EFFECT_NEEDS_LIGHTING_AT_SPAWN_2)) out |= zonetool::h1::FX_EFFECT_NEEDS_LIGHT_GRID_LIGHTING_AT_SPAWN;
					if (flags & FX_EFFECT_NEEDS_LIGHTING_PER_FRAME_AT_ORIGIN) out |= zonetool::h1::FX_EFFECT_NEEDS_LIGHT_GRID_LIGHTING_PER_FRAME;
					if (flags & FX_EFFECT_RECEIVES_DYNAMIC_LIGHTING_PER_FRAME_AT_ORIGIN) out |= zonetool::h1::FX_EFFECT_NEEDS_DYNAMIC_LIGHTING_PER_FRAME;
					return out;
				}

				// v102 fx: the iw7 material converter sends effect techsets (eq_/ev_effect_*), *_add_* and distortion to Ghosts'
				//          additive emissive techset (cameraRegion 4) -> draw those elements in H1's emissive pass
				bool is_emissive_material(const Material* mat)
				{
					if (!mat || !mat->techniqueSet || !mat->techniqueSet->name)
					{
						return false;
					}
					const std::string ts = mat->techniqueSet->name;
					return ts.find("effect") != std::string::npos || ts.find("_add_") != std::string::npos
						|| ts.find("distortion") != std::string::npos || ts.find("blendadd") != std::string::npos;
				}

				FxElemVisuals* visual_list(FxElemDef* elem)
				{
					if (elem->visualCount > 1) return elem->visuals.array;
					if (elem->visualCount == 1) return &elem->visuals.instance;
					return nullptr;
				}

				bool visual_ok(FxElemDef* elem, const FxElemVisuals& vis, const alive_set* alive)
				{
					if (!vis.anonymous)
					{
						return false;
					}
					if (!alive)
					{
						return true;
					}
					if (is_material_elem(elem->elemType))
					{
						return vis.material->name && was_dumped(ASSET_TYPE_MATERIAL, vis.material->name);
					}
					if (elem->elemType == FX_ELEM_TYPE_MODEL)
					{
						return vis.model->name && was_dumped(ASSET_TYPE_XMODEL, vis.model->name);
					}
					if (elem->elemType == FX_ELEM_TYPE_RUNNER)
					{
						return vis.effectDef.handle->name && alive->contains(vis.effectDef.handle->name);
					}
					return true;
				}

				bool mark_ok(const FxElemMarkVisuals& mark, const alive_set* alive)
				{
					if (!mark.materials[0])
					{
						return false;
					}
					if (!alive)
					{
						return true;
					}
					for (auto i = 0; i < 2; i++)
					{
						if (mark.materials[i] && (!mark.materials[i]->name || !was_dumped(ASSET_TYPE_MATERIAL, mark.materials[i]->name)))
						{
							return false;
						}
					}
					return true;
				}

				bool elem_survives(FxElemDef* elem, const alive_set* alive)
				{
					if (is_skipped_elem(elem->elemType))
					{
						return false;
					}
					if (elem->elemType == FX_ELEM_TYPE_OMNI_LIGHT || elem->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						return true;
					}
					if (elem->elemType == FX_ELEM_TYPE_DECAL)
					{
						if (!elem->visuals.markArray || !elem->visualCount)
						{
							return false;
						}
						for (auto i = 0; i < elem->visualCount; i++)
						{
							if (!mark_ok(elem->visuals.markArray[i], alive))
							{
								return false;
							}
						}
						return true;
					}
					auto* list = visual_list(elem);
					if (!list)
					{
						// no visuals: only material-less sprites could be valid, keep them as the source had them
						return is_material_elem(elem->elemType);
					}
					for (auto i = 0; i < elem->visualCount; i++)
					{
						if (visual_ok(elem, list[i], alive))
						{
							return true;
						}
					}
					return false;
				}

				const char* effect_ref_name(const FxEffectDefRef& ref)
				{
					return ref.handle ? ref.handle->name : nullptr;
				}

				void convert_elem(FxElemDef* elem, zonetool::h1::FxElemDef* new_elem, zonetool::h1::FxEffectDef* new_fx,
					const alive_set* alive, utils::memory::allocator& allocator)
				{
					new_elem->flags = static_cast<zonetool::h1::FxElemDefFlags>(convert_elem_flags(static_cast<unsigned int>(elem->flags)));
					// v102 fx: IW7 flags2 0x8+ are turbulence / GPU lighting / curves; H1 0x8 is USE_EMISSIVE_DRAW -> keep only facing + trails
					new_elem->flags2 = elem->flags2 & 0x7;

					std::memcpy(&new_elem->spawn, &elem->spawn, sizeof(new_elem->spawn));
					std::memcpy(&new_elem->spawnRange, &elem->spawnRange, sizeof(new_elem->spawnRange));
					std::memcpy(&new_elem->fadeInRange, &elem->fadeInRange, sizeof(new_elem->fadeInRange));
					std::memcpy(&new_elem->fadeOutRange, &elem->fadeOutRange, sizeof(new_elem->fadeOutRange));
					new_elem->spawnFrustumCullRadius = elem->spawnFrustumCullRadius;
					std::memcpy(&new_elem->spawnDelayMsec, &elem->spawnDelayMsec, sizeof(new_elem->spawnDelayMsec));
					std::memcpy(&new_elem->lifeSpanMsec, &elem->lifeSpanMsec, sizeof(new_elem->lifeSpanMsec));
					std::memcpy(new_elem->spawnOrigin, elem->spawnOrigin, sizeof(new_elem->spawnOrigin));
					std::memcpy(&new_elem->spawnOffsetRadius, &elem->spawnOffsetRadius, sizeof(new_elem->spawnOffsetRadius));
					std::memcpy(&new_elem->spawnOffsetHeight, &elem->spawnOffsetHeight, sizeof(new_elem->spawnOffsetHeight));
					std::memcpy(new_elem->spawnAngles, elem->spawnAngles, sizeof(new_elem->spawnAngles));
					std::memcpy(new_elem->angularVelocity, elem->angularVelocity, sizeof(new_elem->angularVelocity));
					std::memcpy(&new_elem->initialRotation, &elem->initialRotation, sizeof(new_elem->initialRotation));
					std::memcpy(&new_elem->gravity, &elem->gravity, sizeof(new_elem->gravity));
					std::memcpy(&new_elem->reflectionFactor, &elem->reflectionFactor, sizeof(new_elem->reflectionFactor));

					// v102 fx: IW7 atlas has an extra byte after fps
					new_elem->atlas.behavior = elem->atlas.behavior;
					new_elem->atlas.index = elem->atlas.index;
					new_elem->atlas.fps = elem->atlas.fps;
					new_elem->atlas.loopCount = elem->atlas.loopCount;
					new_elem->atlas.colIndexBits = elem->atlas.colIndexBits;
					new_elem->atlas.rowIndexBits = elem->atlas.rowIndexBits;
					new_elem->atlas.entryCount = elem->atlas.entryCount;

					new_elem->elemType = convert_elem_type(elem->elemType);

					// v102 fx: emissive = the material went to the additive emissive techset
					bool emissive = false;
					if (is_material_elem(elem->elemType))
					{
						if (auto* list = visual_list(elem))
						{
							for (auto i = 0; i < elem->visualCount; i++)
							{
								if (list[i].anonymous && is_emissive_material(list[i].material))
								{
									emissive = true;
								}
							}
						}
					}

					// v102 fx: IW7 has no elemLitType; lit when lightingFrac / litUnlitBlendFactor say so
					const bool lit = !emissive && (elem->lightingFrac != 0 || elem->litUnlitBlendFactor > 0.0f);
					new_elem->elemLitType = zonetool::h1::FX_ELEM_LIT_TYPE_NONE;
					if (lit && is_sprite_elem(elem->elemType))
					{
						new_elem->elemLitType = zonetool::h1::FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE;
					}
					else if (lit && elem->elemType == FX_ELEM_TYPE_MODEL)
					{
						new_elem->elemLitType = zonetool::h1::FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE;
					}

					new_elem->velIntervalCount = elem->velIntervalCount;
					new_elem->visStateIntervalCount = elem->visStateIntervalCount;
					static_assert(sizeof(zonetool::h1::FxElemVelStateSample) == sizeof(FxElemVelStateSample));
					new_elem->velSamples = reinterpret_cast<zonetool::h1::FxElemVelStateSample*>(elem->velSamples);

					if (elem->visSamples)
					{
						const auto unlit = emissive || new_elem->elemLitType == zonetool::h1::FX_ELEM_LIT_TYPE_NONE;
						new_elem->visSamples = allocator.allocate_array<zonetool::h1::FxElemVisStateSample>(elem->visStateIntervalCount + 1);
						const auto copy_state = [&](const FxElemVisualState& src, zonetool::h1::FxElemVisualState& dst)
						{
							std::memcpy(dst.color, src.color, sizeof(dst.color));
							const float e = unlit ? 1.0f : 0.0f;
							dst.emissiveScale[0] = e;
							dst.emissiveScale[1] = e;
							dst.emissiveScale[2] = e;
							dst.rotationDelta = src.rotationDelta;
							dst.rotationTotal = src.rotationTotal;
							dst.size[0] = src.size[0];
							dst.size[1] = src.size[1];
							dst.scale = src.scale;
							dst.pivot[0] = 0.0f;
							dst.pivot[1] = 0.0f;
						};
						for (auto i = 0; i < elem->visStateIntervalCount + 1; i++)
						{
							copy_state(elem->visSamples[i].base, new_elem->visSamples[i].base);
							copy_state(elem->visSamples[i].amplitude, new_elem->visSamples[i].amplitude);
						}
					}

					// visuals
					new_elem->visualCount = 0;
					if (elem->elemType == FX_ELEM_TYPE_DECAL)
					{
						new_elem->visualCount = elem->visualCount;
						new_elem->visuals.markArray = allocator.allocate_array<zonetool::h1::FxElemMarkVisuals>(elem->visualCount);
						for (auto j = 0; j < elem->visualCount; j++)
						{
							auto* mats = elem->visuals.markArray[j].materials;
							new_elem->visuals.markArray[j].materials[0] = reinterpret_cast<zonetool::h1::Material*>(mats[0]);
							new_elem->visuals.markArray[j].materials[1] = reinterpret_cast<zonetool::h1::Material*>(mats[1] ? mats[1] : mats[0]);
							new_elem->visuals.markArray[j].materials[2] = reinterpret_cast<zonetool::h1::Material*>(mats[1] ? mats[1] : mats[0]);
						}
					}
					else if (elem->elemType == FX_ELEM_TYPE_OMNI_LIGHT || elem->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						// v102 fx: H1 omni lights carry no visual
					}
					else if (auto* list = visual_list(elem))
					{
						std::vector<const void*> kept;
						for (auto j = 0; j < elem->visualCount; j++)
						{
							if (visual_ok(elem, list[j], alive))
							{
								kept.push_back(list[j].anonymous);
							}
						}
						new_elem->visualCount = static_cast<unsigned char>(kept.size());
						if (kept.size() > 1)
						{
							new_elem->visuals.array = allocator.allocate_array<zonetool::h1::FxElemVisuals>(kept.size());
							for (auto j = 0u; j < kept.size(); j++)
							{
								new_elem->visuals.array[j].anonymous = kept[j];
							}
						}
						else if (kept.size() == 1)
						{
							new_elem->visuals.instance.anonymous = kept[0];
						}
					}

					std::memcpy(&new_elem->collBounds, &elem->collBounds, sizeof(new_elem->collBounds));

					const auto ref = [&](const FxEffectDefRef& src) -> const char*
					{
						const auto* name = effect_ref_name(src);
						if (!name)
						{
							return nullptr;
						}
						if (alive && !alive->contains(name))
						{
							ZONETOOL_INFO("v102 fx: %s drops reference to missing effect \"%s\"", new_fx->name, name);
							return nullptr;
						}
						return name;
					};
					// FxEffectDefRef is a union of handle/name; the H1 dumper reads handle->name, so keep the handle pointer
					new_elem->effectOnImpact.handle = ref(elem->effectOnImpact) ? reinterpret_cast<zonetool::h1::FxEffectDef*>(elem->effectOnImpact.handle) : nullptr;
					new_elem->effectOnDeath.handle = ref(elem->effectOnDeath) ? reinterpret_cast<zonetool::h1::FxEffectDef*>(elem->effectOnDeath.handle) : nullptr;
					new_elem->effectEmitted.handle = ref(elem->effectEmitted) ? reinterpret_cast<zonetool::h1::FxEffectDef*>(elem->effectEmitted.handle) : nullptr;

					std::memcpy(&new_elem->emitDist, &elem->emitDist, sizeof(new_elem->emitDist));
					std::memcpy(&new_elem->emitDistVariance, &elem->emitDistVariance, sizeof(new_elem->emitDistVariance));

					// v102 fx: extended data; anything not understood stays null (the H1 dumper breaks on unknown non-null data)
					new_elem->extended.unknownDef = nullptr;
					switch (elem->elemType)
					{
					case FX_ELEM_TYPE_TRAIL:
						if (elem->extended.trailDef)
						{
							auto* src = elem->extended.trailDef;
							auto* dst = allocator.allocate<zonetool::h1::FxTrailDef>();
							dst->scrollTimeMsec = src->scrollTimeMsec;
							dst->repeatDist = src->repeatDist;
							dst->invSplitDist = src->invSplitDist;
							dst->invSplitArcDist = src->invSplitArcDist;
							dst->invSplitTime = src->invSplitTime;
							dst->headFadingFactor = 0.0f;
							dst->tailFadingFactor = 0.0f;
							dst->vertCount = src->vertCount;
							dst->verts = allocator.allocate_array<zonetool::h1::FxTrailVertex>(src->vertCount);
							for (auto j = 0; j < src->vertCount; j++)
							{
								std::memcpy(dst->verts[j].pos, src->verts[j].pos, sizeof(float[2]));
								std::memcpy(dst->verts[j].normal, src->verts[j].normal, sizeof(float[2]));
								std::memcpy(dst->verts[j].texCoord, src->verts[j].texCoord, sizeof(float[2]));
								std::memcpy(dst->verts[j].radialNormal, src->verts[j].normal, sizeof(float[2]));
							}
							dst->indCount = src->indCount;
							dst->inds = src->inds;
							new_elem->extended.trailDef = dst;
						}
						break;
					case FX_ELEM_TYPE_SPARK_FOUNTAIN:
						static_assert(sizeof(zonetool::h1::FxSparkFountainDef) == sizeof(FxSparkFountainDef));
						new_elem->extended.sparkFountainDef = reinterpret_cast<zonetool::h1::FxSparkFountainDef*>(elem->extended.sparkFountainDef);
						break;
					case FX_ELEM_TYPE_FLARE:
						static_assert(sizeof(zonetool::h1::FxFlareDef) == sizeof(FxFlareDef));
						new_elem->extended.flareDef = reinterpret_cast<zonetool::h1::FxFlareDef*>(elem->extended.flareDef);
						break;
					case FX_ELEM_TYPE_OMNI_LIGHT:
					case FX_ELEM_TYPE_SPOT_LIGHT:
					{
						auto* omni = allocator.allocate<zonetool::h1::FxOmniLightDef>();
						omni->bulbRadius = 1.0f;
						omni->bulbLength = 0.0f;
						omni->fadeOffsetRt[0] = 0.0f;
						omni->fadeOffsetRt[1] = -1.0f;
						if (elem->elemType == FX_ELEM_TYPE_SPOT_LIGHT && elem->extended.spotLightDef)
						{
							omni->bulbRadius = std::max(elem->extended.spotLightDef->startRadius, 1.0f);
						}
						new_elem->extended.omniLightDef = omni;
						break;
					}
					default:
						break;
					}

					new_elem->sortOrder = elem->sortOrder;
					new_elem->lightingFrac = elem->lightingFrac;
					new_elem->useItemClip = elem->useItemClip;
					new_elem->fadeInfo = elem->fadeInfo;
					new_elem->fadeOutInfo = 0;
					new_elem->randomSeed = elem->randomSeed;

					new_elem->emissiveScaleScale = 0.0f;
					new_elem->hdrLightingFrac = 1.0f;
					new_elem->shadowDensityScale = 1.0f;
					new_elem->scatterRatio = 0.0f;
					new_elem->volumetricTrailFadeStart = -1.0f;

					if (emissive)
					{
						new_fx->flags |= zonetool::h1::FX_EFFECT_NEEDS_EMISSIVE_DRAW;
						new_elem->flags2 |= zonetool::h1::FX_ELEM2_USE_EMISSIVE_DRAW;
						new_elem->hdrLightingFrac = 0.0f;
						new_elem->emissiveScaleScale = 1.0f;
					}
				}

				zonetool::h1::FxEffectDef* convert_filtered(FxEffectDef* asset, utils::memory::allocator& allocator, const alive_set* alive,
					int* dropped_out)
				{
					auto* new_asset = allocator.allocate<zonetool::h1::FxEffectDef>();
					new_asset->name = asset->name;
					new_asset->flags = static_cast<int>(convert_effect_flags(static_cast<unsigned int>(asset->flags)));
					new_asset->totalSize = asset->totalSize;
					new_asset->msecLoopingLife = asset->msecLoopingLife;
					new_asset->elemMaxRadius = asset->elemMaxRadius;
					new_asset->occlusionQueryDepthBias = asset->occlusionQueryDepthBias;
					new_asset->occlusionQueryFadeIn = asset->occlusionQueryFadeIn;
					new_asset->occlusionQueryFadeOut = asset->occlusionQueryFadeOut;
					new_asset->occlusionQueryScaleRange.base = asset->occlusionQueryScaleRange.base;
					new_asset->occlusionQueryScaleRange.amplitude = asset->occlusionQueryScaleRange.amplitude;

					const int counts[3] = { asset->elemDefCountLooping, asset->elemDefCountOneShot, asset->elemDefCountEmission };
					const int total = counts[0] + counts[1] + counts[2];

					new_asset->elemDefs = allocator.allocate_array<zonetool::h1::FxElemDef>(std::max(total, 1));
					int out_counts[3] = {};
					int out_index = 0;
					int dropped = 0;
					int in_index = 0;
					for (auto cat = 0; cat < 3; cat++)
					{
						for (auto k = 0; k < counts[cat]; k++, in_index++)
						{
							auto* elem = &asset->elemDefs[in_index];
							if (!elem_survives(elem, alive))
							{
								dropped++;
								if (IS_DEBUG || alive)
								{
									ZONETOOL_INFO("v102 fx: %s elem %d (type %d) skipped", asset->name, in_index, static_cast<int>(elem->elemType));
								}
								continue;
							}
							if (elem->spawnFrustumCullRadius > new_asset->elemMaxRadius)
							{
								new_asset->elemMaxRadius = elem->spawnFrustumCullRadius;
							}
							convert_elem(elem, &new_asset->elemDefs[out_index++], new_asset, alive, allocator);
							out_counts[cat]++;
						}
					}

					new_asset->elemDefCountLooping = out_counts[0];
					new_asset->elemDefCountOneShot = out_counts[1];
					new_asset->elemDefCountEmission = out_counts[2];

					if (dropped_out)
					{
						*dropped_out = dropped;
					}
					return new_asset;
				}

				int surviving_elems(FxEffectDef* asset, const alive_set* alive)
				{
					const int total = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
					int n = 0;
					for (auto i = 0; i < total; i++)
					{
						if (elem_survives(&asset->elemDefs[i], alive))
						{
							n++;
						}
					}
					return n;
				}
			}

			void note_dumped(XAssetType type, const char* name)
			{
				if (!name || !*name)
				{
					return;
				}
				dumped_assets[type].insert(name);
			}

			bool was_dumped(XAssetType type, const char* name)
			{
				if (!name)
				{
					return false;
				}
				const auto itr = dumped_assets.find(type);
				return itr != dumped_assets.end() && itr->second.contains(name);
			}

			const std::unordered_set<std::string>& converted_effects()
			{
				return written_effects;
			}

			bool is_fx_emissive_material(const Material* mat)
			{
				return is_emissive_material(mat);
			}

			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator)
			{
				return convert_filtered(asset, allocator, nullptr, nullptr);
			}

			void dump(FxEffectDef* asset)
			{
				if (!asset || !asset->name || queued_names.contains(asset->name))
				{
					return;
				}
				queued_names.insert(asset->name);
				queued_effects.push_back(asset);
			}

			void flush()
			{
				auto& vfx_queue = particlesystem::queued();
				if (queued_effects.empty() && vfx_queue.empty())
				{
					return;
				}

				// v104 vfx: particle systems share the effects/ namespace; a legacy FX of the same name wins
				std::vector<ParticleSystemDef*> vfx_list;
				for (auto* vfx : vfx_queue)
				{
					if (vfx && vfx->name && !queued_names.contains(vfx->name))
					{
						vfx_list.push_back(vfx);
					}
				}

				// v102 fx: fixed point - an effect is alive while it keeps >= 1 element given the other alive effects
				alive_set alive;
				for (auto* fx : queued_effects)
				{
					alive.insert(fx->name);
				}
				for (auto* vfx : vfx_list)
				{
					alive.insert(vfx->name);
				}
				for (const auto& name : written_effects)
				{
					alive.insert(name); // effects written by an earlier zone of this session
				}

				bool changed = true;
				while (changed)
				{
					changed = false;
					for (auto* fx : queued_effects)
					{
						if (alive.contains(fx->name) && surviving_elems(fx, &alive) == 0)
						{
							alive.erase(fx->name);
							changed = true;
						}
					}
					for (auto* vfx : vfx_list)
					{
						if (alive.contains(vfx->name) && particlesystem::surviving_emitters(vfx, &alive, createfx::is_ambient(vfx->name)) == 0)
						{
							alive.erase(vfx->name);
							changed = true;
						}
					}
				}

				int written = 0;
				int empty = 0;
				int dropped_total = 0;
				for (auto* fx : queued_effects)
				{
					if (!alive.contains(fx->name))
					{
						empty++;
						ZONETOOL_INFO("v102 fx: %s not written (no convertible element: sound/vectorfield/particlesim only, or its materials/models are not in this zone)", fx->name);
						continue;
					}

					try
					{
						utils::memory::allocator allocator;
						int dropped = 0;
						auto* converted = convert_filtered(fx, allocator, &alive, &dropped);
						zonetool::h1::fx_effect_def::dump(converted);
						written_effects.insert(fx->name);
						written++;
						dropped_total += dropped;
						ZONETOOL_INFO("v102 fx: %s -> H1 (%d looping, %d oneshot, %d emission, %d elems skipped)", fx->name,
							converted->elemDefCountLooping, converted->elemDefCountOneShot, converted->elemDefCountEmission, dropped);
					}
					catch (const std::exception& ex)
					{
						ZONETOOL_ERROR("v102 fx: failed to convert %s: %s", fx->name, ex.what());
					}
				}

				ZONETOOL_INFO("v102 fx: %d effects written, %d effects skipped (empty), %d elements skipped", written, empty, dropped_total);

				// v104 vfx: particle systems
				int vfx_written = 0;
				int vfx_empty = 0;
				int vfx_dropped = 0;
				for (auto* vfx : vfx_list)
				{
					const bool ambient = createfx::is_ambient(vfx->name);
					if (!alive.contains(vfx->name))
					{
						vfx_empty++;
						ZONETOOL_INFO("v104 vfx: %s not written (no emitter maps to H1: beams/decals/geo trails/volumetrics only, or materials/models not in this zone)", vfx->name);
						continue;
					}

					try
					{
						utils::memory::allocator allocator;
						int dropped = 0;
						auto* converted = particlesystem::convert(vfx, allocator, &alive, ambient, &dropped);
						zonetool::h1::fx_effect_def::dump(converted);
						written_effects.insert(vfx->name);
						vfx_written++;
						vfx_dropped += dropped;
						ZONETOOL_INFO("v104 vfx: %s -> H1%s (%d emitters: %d looping, %d oneshot, %d skipped)", vfx->name, ambient ? " [createfx ambient]" : "",
							vfx->numEmitters, converted->elemDefCountLooping, converted->elemDefCountOneShot, dropped);
					}
					catch (const std::exception& ex)
					{
						ZONETOOL_ERROR("v104 vfx: failed to convert %s: %s", vfx->name, ex.what());
					}
				}

				if (!vfx_list.empty())
				{
					ZONETOOL_INFO("v104 vfx: %d particle systems written, %d skipped (empty), %d emitters skipped", vfx_written, vfx_empty, vfx_dropped);
				}

				queued_effects.clear();
				queued_names.clear();
				particlesystem::clear_queue();
			}
		}
	}
}
