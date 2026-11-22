#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "fxeffectdef.hpp"
#include "particlesystem.hpp"
#include "createfx.hpp"
#include "material.hpp"
#include "gfximage.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"
#include <fstream>

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
				// v114 fx: assets that live in another dumped zone (IW7 common_core_mp materials), listed one "type,name" per line in
				//          zt_extra_assets.txt (working dir); effects may reference them, the port script copies the ones used
				std::unordered_map<std::int32_t, std::unordered_set<std::string>> external_assets;
				bool external_loaded = false;
				std::set<std::pair<std::int32_t, std::string>> used_external;

				void load_external()
				{
					if (external_loaded)
					{
						return;
					}
					external_loaded = true;
					std::ifstream in("zt_extra_assets.txt");
					std::string line;
					std::size_t count = 0;
					while (std::getline(in, line))
					{
						while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
						const auto comma = line.find(',');
						if (comma == std::string::npos) continue;
						const auto type_str = line.substr(0, comma);
						const auto asset_name = line.substr(comma + 1);
						std::int32_t asset_type = -1;
						if (type_str == "material") asset_type = ASSET_TYPE_MATERIAL;
						else if (type_str == "xmodel") asset_type = ASSET_TYPE_XMODEL;
						if (asset_type < 0 || asset_name.empty()) continue;
						external_assets[asset_type].insert(asset_name);
						count++;
					}
					ZONETOOL_INFO("v114 fx: %zu external assets available (zt_extra_assets.txt)", count);
				}
				// v109 fx: effects converted at dump time into owned H1 data (the IW7 zone memory is reused by
				//          stop_dumping time: v108 read every queued particle system as "mp_flip"); pruned + written in flush()
				struct pending_effect
				{
					std::string name;
					zonetool::h1::FxEffectDef* def = nullptr;
					std::vector<particlesystem::elem_meta> meta;
					bool is_vfx = false;
				};
				std::vector<pending_effect> pending_effects;
				std::unordered_set<std::string> pending_names;
				utils::memory::allocator* owned_allocator = nullptr;
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
					// v108 vfx: also reached from particle systems, whose material pointers come from partly guessed structs
					if (!particlesystem::readable(mat, sizeof(Material)) || !particlesystem::readable(mat->techniqueSet, sizeof(const char*)))
					{
						return false;
					}
					const auto* ts_name = particlesystem::safe_name(mat->techniqueSet->name);
					if (!ts_name)
					{
						return false;
					}
					const std::string ts = ts_name;
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
								if (list[i].anonymous)
								{
									ensure_external_material(list[i].material); // v114
								}
								if (list[i].anonymous && is_emissive_material(list[i].material))
								{
									emissive = true;
								}
							}
						}
					}

					// v124 fx: effect models from another loaded zone (glass shard debris lives in common_core_mp)
					if (elem->elemType == FX_ELEM_TYPE_MODEL)
					{
						if (auto* mlist = visual_list(elem))
						{
							for (auto i = 0; i < elem->visualCount; i++)
							{
								if (mlist[i].anonymous) ensure_external_model(mlist[i].model);
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
								if (alive) // v105: IS_DEBUG undefined here
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
				if (itr != dumped_assets.end() && itr->second.contains(name))
				{
					return true;
				}
				load_external();
				const auto ext = external_assets.find(type);
				if (ext != external_assets.end() && ext->second.contains(name))
				{
					used_external.emplace(static_cast<std::int32_t>(type), name);
					return true;
				}
				return false;
			}

			const std::unordered_set<std::string>& converted_effects()
			{
				return written_effects;
			}

			bool is_fx_emissive_material(const Material* mat)
			{
				return is_emissive_material(mat);
			}

			void ensure_external_model(XModel* model)
			{
				if (!model || !model->name || !*model->name) return;
				const std::string model_name = model->name;
				const auto in_zone = dumped_assets.find(ASSET_TYPE_XMODEL);
				if (in_zone != dumped_assets.end() && in_zone->second.contains(model_name)) return;
				static std::unordered_set<std::string> done_models;
				if (!done_models.insert(model_name).second) return;
				try
				{
					for (auto i = 0; i < model->numsurfs && model->materialHandles; i++) ensure_external_material(model->materialHandles[i]);
					for (auto l = 0; l < model->numLods && l < 6; l++)
					{
						auto* surfs = model->lodInfo[l].modelSurfs;
						if (surfs && surfs->name && !was_dumped(ASSET_TYPE_XMODEL_SURFS, surfs->name))
						{
							xsurface::dump(surfs);
							note_dumped(ASSET_TYPE_XMODEL_SURFS, surfs->name);
						}
					}
					xmodel::dump(model);
					note_dumped(ASSET_TYPE_XMODEL, model->name);
					ZONETOOL_INFO("v124 fx: dumped external effect model %s", model->name);
				}
				catch (const std::exception& ex)
				{
					ZONETOOL_ERROR("v124 fx: external model %s failed: %s", model->name, ex.what());
				}
			}

			void ensure_external_material(const Material* mat)
			{
				constexpr bool v114_dump_external = true;
				if (!v114_dump_external || !mat || !mat->name || !*mat->name)
				{
					return;
				}
				const std::string mat_name = mat->name;
				const auto in_zone = dumped_assets.find(ASSET_TYPE_MATERIAL);
				if (in_zone != dumped_assets.end() && in_zone->second.contains(mat_name))
				{
					return;
				}
				static std::unordered_set<std::string> done;
				if (!done.insert(mat_name).second)
				{
					return;
				}
				try
				{
					material::dump(const_cast<Material*>(mat));
					int images = 0;
					for (auto i = 0; i < mat->textureCount && mat->textureTable; i++)
					{
						auto* image = mat->textureTable[i].image;
						if (image && image->name && !was_dumped(ASSET_TYPE_IMAGE, image->name))
						{
							gfximage::dump(image);
							note_dumped(ASSET_TYPE_IMAGE, image->name);
							images++;
						}
					}
					note_dumped(ASSET_TYPE_MATERIAL, mat->name);
					ZONETOOL_INFO("v114 fx: dumped external effect material %s (%d images)", mat->name, images);
				}
				catch (const std::exception& ex)
				{
					ZONETOOL_ERROR("v114 fx: external material %s failed: %s", mat->name, ex.what());
				}
			}

			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator)
			{
				return convert_filtered(asset, allocator, nullptr, nullptr);
			}

			namespace
			{
				utils::memory::allocator& owned()
				{
					if (!owned_allocator)
					{
						owned_allocator = new utils::memory::allocator();
					}
					return *owned_allocator;
				}

				// v109 fx: name of an IW7 asset pointer (material / model / effect / particle system all start with the name)
				const char* raw_asset_name(const void* ptr)
				{
					if (!particlesystem::readable(ptr, sizeof(const char*)))
					{
						return nullptr;
					}
					return particlesystem::safe_name(*static_cast<const char* const*>(ptr));
				}

				template <typename T>
				T* name_holder(const char* name, utils::memory::allocator& alloc)
				{
					if (!name)
					{
						return nullptr;
					}
					auto* holder = alloc.allocate<T>();
					holder->name = alloc.duplicate_string(name);
					return holder;
				}

				bool is_light_type(zonetool::h1::FxElemType type)
				{
					return type == zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT || type == zonetool::h1::FX_ELEM_TYPE_SPOT_LIGHT;
				}

				// v109 fx: replace an IW7 asset pointer by an owned H1 stub that only carries the name (all the H1 dumper reads)
				const void* own_visual(zonetool::h1::FxElemType type, const void* ptr, utils::memory::allocator& alloc)
				{
					const auto* name = raw_asset_name(ptr);
					switch (type)
					{
					case zonetool::h1::FX_ELEM_TYPE_MODEL: return name_holder<zonetool::h1::XModel>(name, alloc);
					case zonetool::h1::FX_ELEM_TYPE_RUNNER: return name_holder<zonetool::h1::FxEffectDef>(name, alloc);
					default: return name_holder<zonetool::h1::Material>(name, alloc);
					}
				}

				const char* owned_visual_name(zonetool::h1::FxElemType type, const void* ptr)
				{
					if (!ptr) return nullptr;
					switch (type)
					{
					case zonetool::h1::FX_ELEM_TYPE_MODEL: return static_cast<const zonetool::h1::XModel*>(ptr)->name;
					case zonetool::h1::FX_ELEM_TYPE_RUNNER: return static_cast<const zonetool::h1::FxEffectDef*>(ptr)->name;
					default: return static_cast<const zonetool::h1::Material*>(ptr)->name;
					}
				}

				zonetool::h1::FxEffectDef* own_effect_ref(zonetool::h1::FxEffectDef* handle, utils::memory::allocator& alloc)
				{
					return handle ? name_holder<zonetool::h1::FxEffectDef>(raw_asset_name(handle), alloc) : nullptr;
				}

				// v109 fx: make a freshly converted H1 effect independent of the IW7 zone memory
				void own_effect(zonetool::h1::FxEffectDef* def, const std::string& name, utils::memory::allocator& alloc)
				{
					def->name = alloc.duplicate_string(name);
					const int total = def->elemDefCountLooping + def->elemDefCountOneShot + def->elemDefCountEmission;
					for (auto elem_index = 0; elem_index < total; elem_index++)
					{
						auto& elem = def->elemDefs[elem_index];

						if (elem.velSamples)
						{
							const auto count = static_cast<std::size_t>(elem.velIntervalCount) + 1;
							auto* copy = alloc.allocate_array<zonetool::h1::FxElemVelStateSample>(count);
							if (particlesystem::readable(elem.velSamples, count * sizeof(zonetool::h1::FxElemVelStateSample)))
							{
								std::memcpy(copy, elem.velSamples, count * sizeof(zonetool::h1::FxElemVelStateSample));
							}
							elem.velSamples = copy;
						}

						if (elem.elemType == zonetool::h1::FX_ELEM_TYPE_DECAL)
						{
							if (elem.visuals.markArray)
							{
								for (auto vis = 0; vis < elem.visualCount; vis++)
								{
									for (auto& mat : elem.visuals.markArray[vis].materials)
									{
										mat = mat ? name_holder<zonetool::h1::Material>(raw_asset_name(mat), alloc) : nullptr;
									}
								}
							}
						}
						else if (is_light_type(elem.elemType))
						{
							elem.visualCount = 0;
							elem.visuals.instance.anonymous = nullptr;
						}
						else if (elem.visualCount > 1)
						{
							int kept = 0;
							for (auto vis = 0; vis < elem.visualCount; vis++)
							{
								const auto* owned_ptr = own_visual(elem.elemType, elem.visuals.array[vis].anonymous, alloc);
								if (owned_ptr) elem.visuals.array[kept++].anonymous = owned_ptr;
							}
							if (kept == 1)
							{
								const auto* only = elem.visuals.array[0].anonymous;
								elem.visuals.instance.anonymous = only;
							}
							elem.visualCount = static_cast<unsigned char>(kept);
						}
						else if (elem.visualCount == 1)
						{
							elem.visuals.instance.anonymous = own_visual(elem.elemType, elem.visuals.instance.anonymous, alloc);
							if (!elem.visuals.instance.anonymous) elem.visualCount = 0;
						}

						elem.effectOnImpact.handle = own_effect_ref(elem.effectOnImpact.handle, alloc);
						elem.effectOnDeath.handle = own_effect_ref(elem.effectOnDeath.handle, alloc);
						elem.effectEmitted.handle = own_effect_ref(elem.effectEmitted.handle, alloc);

						switch (elem.elemType)
						{
						case zonetool::h1::FX_ELEM_TYPE_TRAIL:
							if (elem.extended.trailDef && elem.extended.trailDef->inds)
							{
								auto* trail = elem.extended.trailDef;
								const auto count = static_cast<std::size_t>(std::clamp(trail->indCount, 0, 65536));
								auto* inds = alloc.allocate_array<unsigned short>(std::max<std::size_t>(count, 1));
								if (particlesystem::readable(trail->inds, count * sizeof(unsigned short)))
								{
									std::memcpy(inds, trail->inds, count * sizeof(unsigned short));
								}
								else
								{
									trail->indCount = 0;
								}
								trail->inds = inds;
							}
							break;
						case zonetool::h1::FX_ELEM_TYPE_SPARK_FOUNTAIN:
							if (elem.extended.sparkFountainDef)
							{
								auto* copy = alloc.allocate<zonetool::h1::FxSparkFountainDef>();
								if (particlesystem::readable(elem.extended.sparkFountainDef, sizeof(*copy))) *copy = *elem.extended.sparkFountainDef;
								elem.extended.sparkFountainDef = copy;
							}
							break;
						case zonetool::h1::FX_ELEM_TYPE_FLARE:
							if (elem.extended.flareDef)
							{
								auto* copy = alloc.allocate<zonetool::h1::FxFlareDef>();
								if (particlesystem::readable(elem.extended.flareDef, sizeof(*copy))) *copy = *elem.extended.flareDef;
								const auto copy_floats = [&](float*& arr, int& count)
								{
									count = std::clamp(count, 0, 1024);
									auto* owned_arr = alloc.allocate_array<float>(std::max(count, 1));
									if (arr && particlesystem::readable(arr, static_cast<std::size_t>(count) * sizeof(float)))
									{
										std::memcpy(owned_arr, arr, static_cast<std::size_t>(count) * sizeof(float));
									}
									else
									{
										count = 0;
									}
									arr = owned_arr;
								};
								copy_floats(copy->intensityX, copy->intensityXIntervalCount);
								copy_floats(copy->intensityY, copy->intensityYIntervalCount);
								copy_floats(copy->srcCosIntensity, copy->srcCosIntensityIntervalCount);
								copy_floats(copy->srcCosScale, copy->srcCosScaleIntervalCount);
								elem.extended.flareDef = copy;
							}
							break;
						case zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT:
							break; // allocated by the converters
						default:
							elem.extended.unknownDef = nullptr;
							break;
						}
					}
				}

				bool owned_visual_alive(zonetool::h1::FxElemType type, const void* ptr, const alive_set& alive)
				{
					const auto* name = owned_visual_name(type, ptr);
					if (!name) return false;
					if (type == zonetool::h1::FX_ELEM_TYPE_MODEL) return was_dumped(ASSET_TYPE_XMODEL, name);
					if (type == zonetool::h1::FX_ELEM_TYPE_RUNNER) return alive.contains(name);
					return was_dumped(ASSET_TYPE_MATERIAL, name);
				}

				bool owned_elem_alive(const zonetool::h1::FxElemDef& elem, const alive_set& alive)
				{
					if (is_light_type(elem.elemType))
					{
						return true;
					}
					if (elem.elemType == zonetool::h1::FX_ELEM_TYPE_DECAL)
					{
						if (!elem.visuals.markArray || !elem.visualCount) return false;
						for (auto vis = 0; vis < elem.visualCount; vis++)
						{
							const auto& mats = elem.visuals.markArray[vis].materials;
							if (!mats[0]) return false;
							for (const auto* mat : mats)
							{
								if (mat && !was_dumped(ASSET_TYPE_MATERIAL, mat->name)) return false;
							}
						}
						return true;
					}
					if (elem.visualCount == 0)
					{
						return elem.elemType != zonetool::h1::FX_ELEM_TYPE_MODEL && elem.elemType != zonetool::h1::FX_ELEM_TYPE_RUNNER;
					}
					if (elem.visualCount == 1)
					{
						return owned_visual_alive(elem.elemType, elem.visuals.instance.anonymous, alive);
					}
					for (auto vis = 0; vis < elem.visualCount; vis++)
					{
						if (owned_visual_alive(elem.elemType, elem.visuals.array[vis].anonymous, alive)) return true;
					}
					return false;
				}

				int owned_alive_count(const pending_effect& pend, const alive_set& alive)
				{
					const auto* def = pend.def;
					const int total = def->elemDefCountLooping + def->elemDefCountOneShot + def->elemDefCountEmission;
					int count = 0;
					for (auto elem_index = 0; elem_index < total; elem_index++)
					{
						if (owned_elem_alive(def->elemDefs[elem_index], alive)) count++;
					}
					return count;
				}

				// v109 fx: final H1 effect: drop elements / visuals / refs to assets that were not dumped, apply the ambient
				//          (createfx) variant of burst emitters
				zonetool::h1::FxEffectDef* prune(const pending_effect& pend, const alive_set& alive, bool ambient,
					utils::memory::allocator& alloc, int* dropped_out)
				{
					const auto* src = pend.def;
					auto* out = alloc.allocate<zonetool::h1::FxEffectDef>();
					*out = *src;
					const int total = src->elemDefCountLooping + src->elemDefCountOneShot + src->elemDefCountEmission;
					std::vector<zonetool::h1::FxElemDef> by_category[3];
					int dropped = 0;
					float looping_life = 0.0f;
					for (auto elem_index = 0; elem_index < total; elem_index++)
					{
						const auto& elem = src->elemDefs[elem_index];
						if (!owned_elem_alive(elem, alive))
						{
							dropped++;
							continue;
						}
						auto copy = elem;
						if (!is_light_type(elem.elemType) && elem.elemType != zonetool::h1::FX_ELEM_TYPE_DECAL && elem.visualCount > 1)
						{
							std::vector<const void*> kept;
							for (auto vis = 0; vis < elem.visualCount; vis++)
							{
								if (owned_visual_alive(elem.elemType, elem.visuals.array[vis].anonymous, alive)) kept.push_back(elem.visuals.array[vis].anonymous);
							}
							copy.visualCount = static_cast<unsigned char>(kept.size());
							if (kept.size() == 1)
							{
								copy.visuals.instance.anonymous = kept[0];
							}
							else
							{
								copy.visuals.array = alloc.allocate_array<zonetool::h1::FxElemVisuals>(kept.size());
								for (auto idx = 0u; idx < kept.size(); idx++) copy.visuals.array[idx].anonymous = kept[idx];
							}
						}
						const auto ref_alive = [&](zonetool::h1::FxEffectDef* handle) -> zonetool::h1::FxEffectDef*
						{
							return (handle && handle->name && alive.contains(handle->name)) ? handle : nullptr;
						};
						copy.effectOnImpact.handle = ref_alive(elem.effectOnImpact.handle);
						copy.effectOnDeath.handle = ref_alive(elem.effectOnDeath.handle);
						copy.effectEmitted.handle = ref_alive(elem.effectEmitted.handle);

						int category = 0;
						if (elem_index < static_cast<int>(pend.meta.size()))
						{
							const auto& meta = pend.meta[elem_index];
							category = meta.category;
							if (ambient && meta.burst && category == 1)
							{
								copy.spawn.looping.intervalMsec = std::max(meta.ambient_interval, 1);
								copy.spawn.looping.count = std::max(meta.ambient_count, 1);
								category = 0;
							}
							if (category == 0) looping_life = std::max(looping_life, meta.life_msec);
						}
						by_category[std::clamp(category, 0, 2)].push_back(copy);
					}

					const auto kept_total = by_category[0].size() + by_category[1].size() + by_category[2].size();
					out->elemDefs = alloc.allocate_array<zonetool::h1::FxElemDef>(std::max<std::size_t>(kept_total, 1));
					int out_index = 0;
					for (const auto& list : by_category)
					{
						for (const auto& elem : list) out->elemDefs[out_index++] = elem;
					}
					out->elemDefCountLooping = static_cast<int>(by_category[0].size());
					out->elemDefCountOneShot = static_cast<int>(by_category[1].size());
					out->elemDefCountEmission = static_cast<int>(by_category[2].size());
					if (pend.is_vfx)
					{
						out->msecLoopingLife = static_cast<int>(looping_life);
					}
					if (dropped_out) *dropped_out = dropped;
					return out;
				}

				// v110 fx: dedup list of assets the converted effects need but this session did not dump
				void collect_missing(const pending_effect& pend, const alive_set& alive, std::map<std::pair<std::string, std::string>, int>& missing)
				{
					const auto* def = pend.def;
					const int total = def->elemDefCountLooping + def->elemDefCountOneShot + def->elemDefCountEmission;
					const auto note = [&](const char* kind, const char* name)
					{
						if (name) missing[{ kind, name }]++;
					};
					for (auto elem_index = 0; elem_index < total; elem_index++)
					{
						const auto& elem = def->elemDefs[elem_index];
						if (elem.elemType == zonetool::h1::FX_ELEM_TYPE_DECAL)
						{
							if (!elem.visuals.markArray) continue;
							for (auto vis = 0; vis < elem.visualCount; vis++)
							{
								for (const auto* mat : elem.visuals.markArray[vis].materials)
								{
									if (mat && !was_dumped(ASSET_TYPE_MATERIAL, mat->name)) note("material", mat->name);
								}
							}
						}
						else if (!is_light_type(elem.elemType) && elem.visualCount > 0)
						{
							for (auto vis = 0; vis < elem.visualCount; vis++)
							{
								const void* ptr = elem.visualCount == 1 ? elem.visuals.instance.anonymous : elem.visuals.array[vis].anonymous;
								const auto* name = owned_visual_name(elem.elemType, ptr);
								if (!name) continue;
								if (elem.elemType == zonetool::h1::FX_ELEM_TYPE_MODEL)
								{
									if (!was_dumped(ASSET_TYPE_XMODEL, name)) note("model", name);
								}
								else if (elem.elemType == zonetool::h1::FX_ELEM_TYPE_RUNNER)
								{
									if (!alive.contains(name)) note("effect", name);
								}
								else if (!was_dumped(ASSET_TYPE_MATERIAL, name))
								{
									note("material", name);
								}
							}
						}
						for (const auto* handle : { elem.effectOnImpact.handle, elem.effectOnDeath.handle, elem.effectEmitted.handle })
						{
							if (handle && handle->name && !alive.contains(handle->name)) note("effect", handle->name);
						}
					}
				}

				void add_pending(const std::string& name, zonetool::h1::FxEffectDef* def, std::vector<particlesystem::elem_meta>&& meta, bool is_vfx)
				{
					pending_names.insert(name);
					pending_effect pend{};
					pend.name = name;
					pend.def = def;
					pend.meta = std::move(meta);
					pend.is_vfx = is_vfx;
					pending_effects.push_back(std::move(pend));
				}
			}

			void dump(FxEffectDef* asset)
			{
				if (!particlesystem::readable(asset, sizeof(FxEffectDef)))
				{
					return;
				}
				const auto* name = particlesystem::safe_name(asset->name);
				if (!name || pending_names.contains(name))
				{
					return;
				}
				const std::string fx_name = name;
				ZONETOOL_INFO("v109 fx: converting legacy effect %s", fx_name.data());
				std::fflush(stdout);
				try
				{
					auto& alloc = owned();
					int dropped = 0;
					auto* def = convert_filtered(asset, alloc, nullptr, &dropped);
					own_effect(def, fx_name, alloc);
					std::vector<particlesystem::elem_meta> meta;
					const int counts[3] = { def->elemDefCountLooping, def->elemDefCountOneShot, def->elemDefCountEmission };
					for (auto cat = 0; cat < 3; cat++)
					{
						for (auto idx = 0; idx < counts[cat]; idx++)
						{
							particlesystem::elem_meta entry{};
							entry.category = cat;
							meta.push_back(entry);
						}
					}
					add_pending(fx_name, def, std::move(meta), false);
				}
				catch (const std::exception& ex)
				{
					ZONETOOL_ERROR("v109 fx: failed to convert %s: %s", fx_name.data(), ex.what());
				}
				catch (...)
				{
					ZONETOOL_ERROR("v109 fx: failed to convert %s (unknown exception)", fx_name.data());
				}
			}

			void dump_vfx(ParticleSystemDef* asset)
			{
				if (!particlesystem::readable(asset, sizeof(ParticleSystemDef)))
				{
					return;
				}
				const auto* name = particlesystem::safe_name(asset->name);
				if (!name || pending_names.contains(name))
				{
					return;
				}
				const std::string vfx_name = name;
				ZONETOOL_INFO("v109 vfx: converting %s (%d emitters)", vfx_name.data(), asset->numEmitters);
				std::fflush(stdout);
				try
				{
					auto& alloc = owned();
					int dropped = 0;
					std::vector<particlesystem::elem_meta> meta;
					auto* def = particlesystem::convert(asset, alloc, nullptr, false, &dropped, &meta);
					own_effect(def, vfx_name, alloc);
					add_pending(vfx_name, def, std::move(meta), true);
				}
				catch (const std::exception& ex)
				{
					ZONETOOL_ERROR("v109 vfx: failed to convert %s: %s", vfx_name.data(), ex.what());
				}
				catch (...)
				{
					ZONETOOL_ERROR("v109 vfx: failed to convert %s (unknown exception)", vfx_name.data());
				}
			}

			void flush()
			{
				if (pending_effects.empty())
				{
					return;
				}
				ZONETOOL_INFO("v109 fx: flush: %zu converted effects pending", pending_effects.size());
				std::fflush(stdout);

				// v102 fx: fixed point - an effect is alive while it keeps >= 1 element given the other alive effects
				alive_set alive;
				for (const auto& pend : pending_effects)
				{
					alive.insert(pend.name);
				}
				for (const auto& name : written_effects)
				{
					alive.insert(name); // effects written by an earlier zone of this session
				}
				bool changed = true;
				while (changed)
				{
					changed = false;
					for (const auto& pend : pending_effects)
					{
						if (alive.contains(pend.name) && owned_alive_count(pend, alive) == 0)
						{
							alive.erase(pend.name);
							changed = true;
						}
					}
				}

				// v110 fx: what is missing, so the zone holding it (common mp?) can be dumped alongside
				{
					std::map<std::pair<std::string, std::string>, int> missing;
					for (const auto& pend : pending_effects)
					{
						collect_missing(pend, alive, missing);
					}
					std::vector<std::pair<std::pair<std::string, std::string>, int>> sorted_missing(missing.begin(), missing.end());
					std::sort(sorted_missing.begin(), sorted_missing.end(), [](const auto& lhs, const auto& rhs)
					{
						return lhs.second != rhs.second ? lhs.second > rhs.second : lhs.first < rhs.first;
					});
					for (const auto& [key, uses] : sorted_missing)
					{
						ZONETOOL_INFO("v110 fx: missing %s %s (%d uses)", key.first.data(), key.second.data(), uses);
					}
					ZONETOOL_INFO("v110 fx: %zu distinct missing assets", sorted_missing.size());
					std::fflush(stdout);
				}

				int written[2] = {};
				int empty[2] = {};
				int dropped_total[2] = {};
				for (const auto& pend : pending_effects)
				{
					const int kind = pend.is_vfx ? 1 : 0;
					const char* prefix = pend.is_vfx ? "v104 vfx" : "v102 fx";
					if (!alive.contains(pend.name))
					{
						empty[kind]++;
						ZONETOOL_INFO("%s: %s not written (no element left: unsupported types, or materials/models/child effects not in this zone)", prefix, pend.name.data());
						continue;
					}
					try
					{
						const bool ambient = pend.is_vfx && createfx::is_ambient(pend.name);
						utils::memory::allocator alloc;
						int dropped = 0;
						auto* final_def = prune(pend, alive, ambient, alloc, &dropped);
						zonetool::h1::fx_effect_def::dump(final_def);
						written_effects.insert(pend.name);
						written[kind]++;
						dropped_total[kind] += dropped;
						ZONETOOL_INFO("%s: %s -> H1%s (%d looping, %d oneshot, %d emission, %d elems dropped for missing assets)", prefix, pend.name.data(),
							ambient ? " [createfx ambient]" : "", final_def->elemDefCountLooping, final_def->elemDefCountOneShot, final_def->elemDefCountEmission, dropped);
					}
					catch (const std::exception& ex)
					{
						ZONETOOL_ERROR("%s: failed to write %s: %s", prefix, pend.name.data(), ex.what());
					}
					catch (...)
					{
						ZONETOOL_ERROR("%s: failed to write %s (unknown exception)", prefix, pend.name.data());
					}
				}

				ZONETOOL_INFO("v102 fx: %d legacy effects written, %d skipped (empty), %d elements dropped", written[0], empty[0], dropped_total[0]);
				ZONETOOL_INFO("v104 vfx: %d particle systems written, %d skipped (empty), %d elements dropped", written[1], empty[1], dropped_total[1]);
				std::fflush(stdout);

				// v114 fx: external (common zone) assets the effects use, for iw7_port2.ps1 to copy into the map's zone source
				{
					std::string list;
					for (const auto& [type, asset_name] : used_external)
					{
						list += (type == ASSET_TYPE_MATERIAL ? "material," : "xmodel,") + asset_name + "\n";
					}
					filesystem::file file("fx_external_assets.txt");
					file.open("wb");
					file.write(list);
					file.close();
					ZONETOOL_INFO("v114 fx: %zu external assets used by effects (fx_external_assets.txt)", used_external.size());
				}

				pending_effects.clear();
				pending_names.clear();
				if (owned_allocator)
				{
					owned_allocator->clear();
				}
			}
		}
	}
}
