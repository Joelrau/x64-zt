#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "particlesystem.hpp"
#include "fxeffectdef.hpp"

// v104 vfx: approximate IW7 ParticleSystemDef (new particle system, ASSET_TYPE_VFX) -> H1 FxEffectDef.
//   One H1 element per IW7 emitter, built from the emitter's first state (later states are test/event transitions):
//     element type : billboard sprite / cloud / spark cloud -> H1 sprite billboard, oriented sprite -> sprite oriented,
//                    tail -> tail, model -> model, light omni/spot -> omni light, runner -> runner (child particle system)
//     skipped      : beam, decal, flare, geo trail, vector field, volumetric, disabled / NVG-thermal-only / MP-disabled emitters
//     emission     : spawn rate (particles/s) -> looping interval; burst mode -> one-shot count. Systems placed by createfx
//                    (ambient) turn bursts into repeating looping bursts so they keep playing under triggerfx.
//                    Infinite-life particles (screens, holograms) become looping 1s particles respawned in place.
//     life / delay : particleLife / particleDelay (+ emitterDelay); seconds unless the value is > 100 (then msec)
//     visuals      : InitMaterial (random pick), InitModel, InitRunner linked assets; assets that were not dumped drop the emitter
//     colour/alpha : InitAttributes colour min/max x ColorGraph (two curve sets = base / random amplitude) or ColorLerp
//     size         : InitAttributes size min/max, SizeGraph (lerp sizeBegin->sizeEnd, or multiplier) or SizeLerp
//     motion       : InitAttributes velocity min/max (+ VelocityGraph), InitRelativeVelocity (local/world), Gravity module,
//                    InitRotation angle/rate, spawn shapes (sphere/ellipsoid/cylinder -> H1 offsets, box -> origin range)
//     atlas        : InitAtlas + the material's atlas rows/columns
//   Not mapped: emission/emissive/intensity graphs, position/rotation-3D graphs, attractors, forces, physics, tests,
//   distance fade curves, scripted inputs, emitter groups. Curve layouts are assumptions (see eval helpers).

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace particlesystem
		{
			namespace
			{
				std::vector<ParticleSystemDef*> vfx_queue;
				std::unordered_set<std::string> vfx_queue_names;

				// v104 vfx: H1 sprite size is a half extent, IW7 sizes are treated as full width/height
				constexpr float sprite_size_scale = 0.5f;
				constexpr int max_vis_intervals = 7;
				constexpr float max_sane_value = 100000.0f;

				float sane(float value, float fallback = 0.0f)
				{
					if (!std::isfinite(value) || std::fabs(value) > max_sane_value)
					{
						return fallback;
					}
					return value;
				}

				float lerp_value(float from, float to, float frac)
				{
					return from + (to - from) * frac;
				}

				// v104 vfx: emitter times are seconds in the editor; values > 100 are taken as msec already
				float to_msec(float value)
				{
					value = sane(value);
					if (value < 0.0f) value = 0.0f;
					return value > 100.0f ? value : value * 1000.0f;
				}

				bool curve_present(const ParticleCurveDef& curve)
				{
					return curve.controlPoints && curve.numControlPoints > 0;
				}

				// v104 vfx: control points are (sample_time 0..1, value), linear between points, value * curve scale
				float eval_curve(const ParticleCurveDef& curve, float sample_time, float fallback)
				{
					if (!curve_present(curve))
					{
						return fallback;
					}
					const auto* points = curve.controlPoints;
					const auto count = curve.numControlPoints;
					const float curve_scale = (curve.scale == 0.0f || !std::isfinite(curve.scale)) ? 1.0f : curve.scale;
					float result = points[count - 1].value;
					if (sample_time <= points[0].time)
					{
						result = points[0].value;
					}
					else
					{
						for (auto idx = 1; idx < count; idx++)
						{
							if (sample_time <= points[idx].time)
							{
								const float span = points[idx].time - points[idx - 1].time;
								const float frac = span > 0.0f ? (sample_time - points[idx - 1].time) / span : 1.0f;
								result = lerp_value(points[idx - 1].value, points[idx].value, frac);
								break;
							}
						}
					}
					return sane(result * curve_scale, fallback);
				}

				struct spawn_shape_info
				{
					ParticleModuleType type = PARTICLE_MODULE_INVALID;
					const ParticleModuleTypeDef* data = nullptr;
				};

				struct emitter_modules
				{
					const ParticleModuleInitAttributes* attributes = nullptr;
					const ParticleModuleInitAtlas* atlas = nullptr;
					const ParticleModuleInitMaterial* material = nullptr;
					const ParticleModuleInitModel* model = nullptr;
					const ParticleModuleInitRunner* runner = nullptr;
					const ParticleModuleInitLightOmni* light_omni = nullptr;
					const ParticleModuleInitLightSpot* light_spot = nullptr;
					const ParticleModuleInitOrientedSprite* oriented = nullptr;
					const ParticleModuleInitRelativeVelocity* relative_velocity = nullptr;
					const ParticleModuleInitRotation* rotation = nullptr;
					const ParticleModuleColorGraph* color_graph = nullptr;
					const ParticleModuleColorLerp* color_lerp = nullptr;
					const ParticleModuleSizeGraph* size_graph = nullptr;
					const ParticleModuleSizeLerp* size_lerp = nullptr;
					const ParticleModuleVelocityGraph* velocity_graph = nullptr;
					const ParticleModuleGravity* gravity = nullptr;
					spawn_shape_info shape;
				};

				const ParticleStateDef* first_state(const ParticleEmitterDef& emitter)
				{
					if (!emitter.stateDefs || emitter.numStates <= 0)
					{
						return nullptr;
					}
					return &emitter.stateDefs[0];
				}

				emitter_modules gather_modules(const ParticleStateDef* state)
				{
					emitter_modules mods{};
					if (!state || !state->moduleGroupDefs)
					{
						return mods;
					}
					// init + update groups (test group = event transitions, not mapped)
					for (auto group = 0; group < PARTICLE_MODULE_GROUP_TEST; group++)
					{
						const auto& group_def = state->moduleGroupDefs[group];
						if (group_def.disabled || !group_def.moduleDefs || group_def.numModules <= 0)
						{
							continue;
						}
						for (auto mod_index = 0; mod_index < group_def.numModules; mod_index++)
						{
							const auto& mod = group_def.moduleDefs[mod_index];
							const auto& data = mod.moduleData;
							switch (mod.moduleType)
							{
							case PARTICLE_MODULE_INIT_ATTRIBUTES: mods.attributes = &data.initAttributes; break;
							case PARTICLE_MODULE_INIT_ATLAS: mods.atlas = &data.initAtlas; break;
							case PARTICLE_MODULE_INIT_MATERIAL: mods.material = &data.initMaterial; break;
							case PARTICLE_MODULE_INIT_MODEL: mods.model = &data.initModel; break;
							case PARTICLE_MODULE_INIT_RUNNER: mods.runner = &data.initRunner; break;
							case PARTICLE_MODULE_INIT_LIGHT_OMNI: mods.light_omni = &data.initLightOmni; break;
							case PARTICLE_MODULE_INIT_LIGHT_SPOT: mods.light_spot = &data.initLightSpot; break;
							case PARTICLE_MODULE_INIT_ORIENTED_SPRITE: mods.oriented = &data.initOrientedSprite; break;
							case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY: mods.relative_velocity = &data.initRelativeVelocity; break;
							case PARTICLE_MODULE_INIT_ROTATION: mods.rotation = &data.initRotation; break;
							case PARTICLE_MODULE_COLOR_GRAPH: mods.color_graph = &data.colorGraph; break;
							case PARTICLE_MODULE_COLOR_LERP: mods.color_lerp = &data.colorLerp; break;
							case PARTICLE_MODULE_SIZE_GRAPH: mods.size_graph = &data.sizeGraph; break;
							case PARTICLE_MODULE_SIZE_LERP: mods.size_lerp = &data.sizeLerp; break;
							case PARTICLE_MODULE_VELOCITY_GRAPH: mods.velocity_graph = &data.velocityGraph; break;
							case PARTICLE_MODULE_GRAVITY: mods.gravity = &data.gravity; break;
							case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
							case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
							case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
							case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
								mods.shape.type = mod.moduleType;
								mods.shape.data = &data;
								break;
							default:
								break;
							}
						}
					}
					return mods;
				}

				// v104 vfx: H1 element type for an IW7 particle element type; FX_ELEM_TYPE_VECTORFIELD = not mappable
				zonetool::h1::FxElemType map_element_type(unsigned int element_type)
				{
					switch (element_type)
					{
					case PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE:
					case PARTICLE_ELEMENT_TYPE_CLOUD:
					case PARTICLE_ELEMENT_TYPE_SPARK_CLOUD:
						return zonetool::h1::FX_ELEM_TYPE_SPRITE_BILLBOARD;
					case PARTICLE_ELEMENT_TYPE_ORIENTED_SPRITE:
						return zonetool::h1::FX_ELEM_TYPE_SPRITE_ORIENTED;
					case PARTICLE_ELEMENT_TYPE_TAIL:
						return zonetool::h1::FX_ELEM_TYPE_TAIL;
					case PARTICLE_ELEMENT_TYPE_MODEL:
						return zonetool::h1::FX_ELEM_TYPE_MODEL;
					case PARTICLE_ELEMENT_TYPE_LIGHT_OMNI:
					case PARTICLE_ELEMENT_TYPE_LIGHT_SPOT:
						return zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT;
					case PARTICLE_ELEMENT_TYPE_RUNNER:
						return zonetool::h1::FX_ELEM_TYPE_RUNNER;
					default:
						return zonetool::h1::FX_ELEM_TYPE_VECTORFIELD;
					}
				}

				bool is_sprite_type(zonetool::h1::FxElemType type)
				{
					return type == zonetool::h1::FX_ELEM_TYPE_SPRITE_BILLBOARD || type == zonetool::h1::FX_ELEM_TYPE_SPRITE_ORIENTED
						|| type == zonetool::h1::FX_ELEM_TYPE_TAIL;
				}

				const ParticleLinkedAssetListDef* linked_list(const emitter_modules& mods, zonetool::h1::FxElemType type)
				{
					if (is_sprite_type(type)) return mods.material ? &mods.material->m_linkedAssetList : nullptr;
					if (type == zonetool::h1::FX_ELEM_TYPE_MODEL) return mods.model ? &mods.model->m_linkedAssetList : nullptr;
					if (type == zonetool::h1::FX_ELEM_TYPE_RUNNER) return mods.runner ? &mods.runner->m_linkedAssetList : nullptr;
					return nullptr;
				}

				bool linked_asset_ok(const ParticleLinkedAssetDef& linked, zonetool::h1::FxElemType type, const alive_set* alive)
				{
					if (is_sprite_type(type))
					{
						if (!linked.material || !linked.material->name) return false;
						return !alive || fxeffectdef::was_dumped(ASSET_TYPE_MATERIAL, linked.material->name);
					}
					if (type == zonetool::h1::FX_ELEM_TYPE_MODEL)
					{
						if (!linked.model || !linked.model->name) return false;
						return !alive || fxeffectdef::was_dumped(ASSET_TYPE_XMODEL, linked.model->name);
					}
					if (type == zonetool::h1::FX_ELEM_TYPE_RUNNER)
					{
						if (!linked.particleSystem || !linked.particleSystem->name) return false;
						return !alive || alive->contains(linked.particleSystem->name);
					}
					return false;
				}

				std::vector<const void*> usable_visuals(const emitter_modules& mods, zonetool::h1::FxElemType type, const alive_set* alive)
				{
					std::vector<const void*> visuals;
					const auto* list = linked_list(mods, type);
					if (!list || !list->assetList || list->numAssets <= 0)
					{
						return visuals;
					}
					for (auto asset_index = 0; asset_index < list->numAssets && asset_index < 255; asset_index++)
					{
						const auto& linked = list->assetList[asset_index];
						if (linked_asset_ok(linked, type, alive))
						{
							// material / model / particle system pointers all start with the asset name (what the H1 dumper reads)
							visuals.push_back(linked.material);
						}
					}
					return visuals;
				}

				enum class skip_reason
				{
					none,
					disabled,
					unsupported_type,
					no_visuals,
					no_emission,
				};

				skip_reason emitter_skip_reason(const ParticleEmitterDef& emitter, const alive_set* alive, zonetool::h1::FxElemType* type_out)
				{
					constexpr unsigned int disabled_flags = PARTICLE_EMITTER_DEF_FLAG_DISABLED | PARTICLE_EMITTER_DEF_FLAG_DISABLE_IN_MP_HPC
						| PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_NVG_ON | PARTICLE_EMITTER_DEF_FLAG_GROUP_ONLY_WHEN_THERMAL_ON;
					if (emitter.flags & disabled_flags)
					{
						return skip_reason::disabled;
					}
					const auto* state = first_state(emitter);
					if (!state || (state->flags & PARTICLE_STATE_DEF_FLAG_DISABLED))
					{
						return skip_reason::disabled;
					}
					const auto type = map_element_type(state->elementType);
					if (type_out) *type_out = type;
					if (type == zonetool::h1::FX_ELEM_TYPE_VECTORFIELD)
					{
						return skip_reason::unsupported_type;
					}
					if (type != zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT)
					{
						const auto mods = gather_modules(state);
						if (usable_visuals(mods, type, alive).empty())
						{
							return skip_reason::no_visuals;
						}
					}
					const bool infinite = (emitter.flags & PARTICLE_EMITTER_DEF_FLAG_INFINITE_PARTICLE_LIFE) != 0;
					const bool burst = (emitter.flags & PARTICLE_EMITTER_DEF_FLAG_USE_BURST_MODE) != 0;
					const float rate = std::max(sane(emitter.particleSpawnRate.min), sane(emitter.particleSpawnRate.max));
					if (!infinite && !burst && rate <= 0.0f)
					{
						return skip_reason::no_emission;
					}
					if (burst && std::max(emitter.particleBurstCount.min, emitter.particleBurstCount.max) <= 0)
					{
						return skip_reason::no_emission;
					}
					return skip_reason::none;
				}

				const char* skip_reason_name(skip_reason reason)
				{
					switch (reason)
					{
					case skip_reason::disabled: return "disabled / NVG-thermal-only / MP-disabled";
					case skip_reason::unsupported_type: return "unsupported element type (beam/decal/flare/geo trail/vector field/volumetric)";
					case skip_reason::no_visuals: return "material/model/child effect missing";
					case skip_reason::no_emission: return "no spawn rate / burst";
					default: return "ok";
					}
				}

				// v104 vfx: quaternion (x, y, z, w) -> euler (pitch, yaw, roll) in radians for H1 spawnAngles
				void quat_to_angles(const float4& quat, float* angles_out)
				{
					const float qx = sane(quat.v[0]), qy = sane(quat.v[1]), qz = sane(quat.v[2]), qw = sane(quat.v[3]);
					const float len = std::sqrt(qx * qx + qy * qy + qz * qz + qw * qw);
					angles_out[0] = angles_out[1] = angles_out[2] = 0.0f;
					if (len < 0.001f)
					{
						return;
					}
					const float nx = qx / len, ny = qy / len, nz = qz / len, nw = qw / len;
					const float sin_pitch = std::clamp(2.0f * (nw * ny - nz * nx), -1.0f, 1.0f);
					angles_out[0] = -std::asin(sin_pitch); // pitch (IW: positive = down)
					angles_out[1] = std::atan2(2.0f * (nw * nz + nx * ny), 1.0f - 2.0f * (ny * ny + nz * nz)); // yaw
					angles_out[2] = std::atan2(2.0f * (nw * nx + ny * nz), 1.0f - 2.0f * (nx * nx + ny * ny)); // roll
				}

				float to_radians_guess(float value)
				{
					value = sane(value);
					// values beyond a full turn are degrees
					return std::fabs(value) > 6.3f ? value * 0.01745329252f : value;
				}

				int log2_bits(int value)
				{
					int bits = 0;
					while ((1 << bits) < value && bits < 8) bits++;
					return bits;
				}

				struct curve_pair_sample
				{
					float base;
					float amplitude;
				};

				// v104 vfx: graph modules keep two curve sets (random blend between them): set k, channel c -> curves[k * channels + c]
				curve_pair_sample sample_graph(const ParticleCurveDef* curves, int channels, int channel, float sample_time, float fallback)
				{
					const float first = eval_curve(curves[channel], sample_time, fallback);
					const float second = curve_present(curves[channels + channel]) ? eval_curve(curves[channels + channel], sample_time, first) : first;
					return { first, second - first };
				}

				struct elem_build
				{
					zonetool::h1::FxElemDef* elem;
					bool looping;
					float max_life_msec;
					float max_radius;
					int extra_size;
				};

				elem_build build_elem(const ParticleSystemDef* sys_def, const ParticleEmitterDef& emitter, zonetool::h1::FxElemType type,
					const alive_set* alive, bool ambient, zonetool::h1::FxElemDef* new_elem, zonetool::h1::FxEffectDef* new_fx,
					utils::memory::allocator& allocator)
				{
					const auto* state = first_state(emitter);
					const auto mods = gather_modules(state);
					const auto visuals = usable_visuals(mods, type, alive);

					elem_build result{ new_elem, true, 0.0f, 0.0f, 0 };

					new_elem->elemType = type;
					new_elem->flags = static_cast<zonetool::h1::FxElemDefFlags>(zonetool::h1::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT);
					unsigned int elem_flags = zonetool::h1::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT;
					if (emitter.flags & PARTICLE_EMITTER_DEF_FLAG_DRAW_PAST_FOG) elem_flags |= zonetool::h1::FX_ELEM_DRAW_PAST_FOG;
					if (state->flags & PARTICLE_STATE_DEF_FLAG_DRAW_WITH_VIEW_MODEL) elem_flags |= zonetool::h1::FX_ELEM_DRAW_WITH_VIEWMODEL;
					if (state->flags & PARTICLE_STATE_DEF_FLAG_BLOCKS_SIGHT) elem_flags |= zonetool::h1::FX_ELEM_BLOCK_SIGHT;
					new_elem->flags2 = 0;
					if (state->flags & PARTICLE_STATE_DEF_FLAG_PLAYER_FACING) new_elem->flags2 |= zonetool::h1::FX_ELEM2_BILLBOARD_FACING_PLAYER;

					// life
					const bool infinite = (emitter.flags & PARTICLE_EMITTER_DEF_FLAG_INFINITE_PARTICLE_LIFE) != 0;
					const bool burst = (emitter.flags & PARTICLE_EMITTER_DEF_FLAG_USE_BURST_MODE) != 0;
					float life_min = to_msec(emitter.particleLife.min);
					float life_max = to_msec(emitter.particleLife.max);
					if (life_max < life_min) std::swap(life_min, life_max);
					if (infinite || life_max <= 0.0f)
					{
						// v104 vfx: persistent particles (screens, holograms): 1s particles respawned in place
						life_min = life_max = 1000.0f;
					}
					life_min = std::max(life_min, 16.0f);
					life_max = std::max(life_max, life_min);
					new_elem->lifeSpanMsec.base = static_cast<int>(life_min);
					new_elem->lifeSpanMsec.amplitude = static_cast<int>(life_max - life_min);
					result.max_life_msec = life_max;
					const float life_avg_sec = (life_min + life_max) * 0.5f * 0.001f;

					float delay_min = to_msec(emitter.particleDelay.min) + to_msec(emitter.emitterDelay.min);
					float delay_max = to_msec(emitter.particleDelay.max) + to_msec(emitter.emitterDelay.max);
					if (delay_max < delay_min) std::swap(delay_min, delay_max);
					new_elem->spawnDelayMsec.base = static_cast<int>(delay_min);
					new_elem->spawnDelayMsec.amplitude = static_cast<int>(delay_max - delay_min);

					// emission
					const int burst_max = std::clamp(std::max(emitter.particleBurstCount.min, emitter.particleBurstCount.max), 1, 64);
					const int burst_min = std::clamp(std::min(emitter.particleBurstCount.min, emitter.particleBurstCount.max), 1, burst_max);
					if (infinite)
					{
						const int count = std::clamp(emitter.particleCountMax ? static_cast<int>(std::min(emitter.particleCountMax, 64u)) : burst_max, 1, 64);
						new_elem->spawn.looping.intervalMsec = static_cast<int>(life_min);
						new_elem->spawn.looping.count = count;
					}
					else if (!burst)
					{
						const float rate = std::max((sane(emitter.particleSpawnRate.min) + sane(emitter.particleSpawnRate.max)) * 0.5f, 0.01f);
						new_elem->spawn.looping.intervalMsec = std::clamp(static_cast<int>(std::lround(1000.0f / rate)), 1, 60000);
						new_elem->spawn.looping.count = rate > 1000.0f ? std::clamp(static_cast<int>(std::lround(rate / 1000.0f)), 1, 64) : 1;
					}
					else if (ambient)
					{
						// v104 vfx: placed by createfx -> keep bursting
						const float repeat = std::max({ life_max, to_msec(emitter.emitterLife.max), 250.0f });
						new_elem->spawn.looping.intervalMsec = static_cast<int>(repeat);
						new_elem->spawn.looping.count = burst_max;
					}
					else
					{
						result.looping = false;
						new_elem->spawn.oneShot.count.base = burst_min;
						new_elem->spawn.oneShot.count.amplitude = burst_max - burst_min;
					}

					new_elem->spawnFrustumCullRadius = std::max(sane(emitter.spawnFrustumCullRadius), 0.0f);
					new_elem->randomSeed = emitter.randomSeed;

					// spawn shape
					if (mods.shape.data)
					{
						const auto& shape_data = *mods.shape.data;
						switch (mods.shape.type)
						{
						case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
						{
							const auto& sphere = shape_data.initSpawnShapeSphere;
							const float rmin = std::max(sane(sphere.m_radius.min), 0.0f);
							const float rmax = std::max(sane(sphere.m_radius.max), rmin);
							elem_flags |= zonetool::h1::FX_ELEM_SPAWN_OFFSET_SPHERE;
							new_elem->spawnOffsetRadius.base = rmin;
							new_elem->spawnOffsetRadius.amplitude = rmax - rmin;
							for (auto axis = 0; axis < 3; axis++) new_elem->spawnOrigin[axis].base = sane(sphere.m_offset.v[axis]);
							result.max_radius = std::max(result.max_radius, rmax);
							break;
						}
						case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
						{
							const auto& ellipsoid = shape_data.initSpawnShapeEllipsoid;
							const float rmin = std::max((sane(ellipsoid.m_radiusMin.v[0]) + sane(ellipsoid.m_radiusMin.v[1]) + sane(ellipsoid.m_radiusMin.v[2])) / 3.0f, 0.0f);
							const float rmax = std::max((sane(ellipsoid.m_radiusMax.v[0]) + sane(ellipsoid.m_radiusMax.v[1]) + sane(ellipsoid.m_radiusMax.v[2])) / 3.0f, rmin);
							elem_flags |= zonetool::h1::FX_ELEM_SPAWN_OFFSET_SPHERE;
							new_elem->spawnOffsetRadius.base = rmin;
							new_elem->spawnOffsetRadius.amplitude = rmax - rmin;
							for (auto axis = 0; axis < 3; axis++) new_elem->spawnOrigin[axis].base = sane(ellipsoid.m_offset.v[axis]);
							result.max_radius = std::max(result.max_radius, rmax);
							break;
						}
						case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
						{
							const auto& cylinder = shape_data.initSpawnShapeCylinder;
							const float rmin = std::max(sane(cylinder.m_radius.min), 0.0f);
							const float rmax = std::max(sane(cylinder.m_radius.max), rmin);
							const float half_height = std::fabs(sane(cylinder.m_halfHeight));
							elem_flags |= zonetool::h1::FX_ELEM_SPAWN_OFFSET_CYLINDER;
							new_elem->spawnOffsetRadius.base = rmin;
							new_elem->spawnOffsetRadius.amplitude = rmax - rmin;
							new_elem->spawnOffsetHeight.base = -half_height;
							new_elem->spawnOffsetHeight.amplitude = 2.0f * half_height;
							for (auto axis = 0; axis < 3; axis++) new_elem->spawnOrigin[axis].base = sane(cylinder.m_offset.v[axis]);
							result.max_radius = std::max({ result.max_radius, rmax, half_height });
							break;
						}
						case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
						{
							const auto& box = shape_data.initSpawnShapeBox;
							for (auto axis = 0; axis < 3; axis++)
							{
								float dmin = sane(box.m_dimensionsMin.v[axis]);
								float dmax = sane(box.m_dimensionsMax.v[axis]);
								if (dmax < dmin) std::swap(dmin, dmax);
								new_elem->spawnOrigin[axis].base = sane(box.m_offset.v[axis]) + dmin;
								new_elem->spawnOrigin[axis].amplitude = dmax - dmin;
								result.max_radius = std::max({ result.max_radius, std::fabs(dmin), std::fabs(dmax) });
							}
							break;
						}
						default:
							break;
						}
					}

					// orientation of oriented sprites
					if (mods.oriented && type == zonetool::h1::FX_ELEM_TYPE_SPRITE_ORIENTED)
					{
						float angles[3];
						quat_to_angles(mods.oriented->m_orientationQuat, angles);
						for (auto axis = 0; axis < 3; axis++) new_elem->spawnAngles[axis].base = angles[axis];
					}

					// rotation
					float rot_rate_min = 0.0f, rot_rate_max = 0.0f;
					if (mods.rotation)
					{
						float angle_min = to_radians_guess(mods.rotation->m_rotationAngle.min);
						float angle_max = to_radians_guess(mods.rotation->m_rotationAngle.max);
						if (angle_max < angle_min) std::swap(angle_min, angle_max);
						new_elem->initialRotation.base = angle_min;
						new_elem->initialRotation.amplitude = angle_max - angle_min;
						rot_rate_min = to_radians_guess(mods.rotation->m_rotationRate.min);
						rot_rate_max = to_radians_guess(mods.rotation->m_rotationRate.max);
					}

					// gravity (percentage of world gravity; values beyond 2 are percent)
					if (mods.gravity && emitter.gravityOptions != PARTICLE_GRAVITY_OPTION_GRAVITY_NEVER)
					{
						float grav_min = sane(mods.gravity->m_gravityPercentage.min);
						float grav_max = sane(mods.gravity->m_gravityPercentage.max);
						if (std::fabs(grav_min) > 2.0f || std::fabs(grav_max) > 2.0f)
						{
							grav_min *= 0.01f;
							grav_max *= 0.01f;
						}
						if (grav_min != 0.0f || grav_max != 0.0f)
						{
							elem_flags |= zonetool::h1::FX_ELEM_HAS_GRAVITY;
							new_elem->gravity.base = grav_min;
							new_elem->gravity.amplitude = grav_max - grav_min;
						}
					}

					// atlas
					new_elem->atlas.entryCount = 1;
					if (is_sprite_type(type) && !visuals.empty())
					{
						const auto* mat = static_cast<const Material*>(visuals[0]);
						const int rows = std::max<int>(mat->info.textureAtlasRowCount, 1);
						const int cols = std::max<int>(mat->info.textureAtlasColumnCount, 1);
						if (rows * cols > 1)
						{
							new_elem->atlas.entryCount = static_cast<short>(rows * cols);
							new_elem->atlas.colIndexBits = static_cast<unsigned char>(log2_bits(cols));
							new_elem->atlas.rowIndexBits = static_cast<unsigned char>(log2_bits(rows));
							unsigned char behavior = 1; // FX_ATLAS_START_RANDOM
							unsigned char fps = 0;
							if (mods.atlas)
							{
								behavior = mods.atlas->m_startFrame < 0 ? 1 : 0;
								new_elem->atlas.index = static_cast<unsigned char>(std::clamp(mods.atlas->m_startFrame, 0, 255));
								fps = static_cast<unsigned char>(std::clamp(mods.atlas->m_playRate, 0, 255));
								if (fps == 0) behavior |= 4; // FX_ATLAS_PLAY_OVER_LIFE
								if (mods.atlas->m_loopCount > 0)
								{
									behavior |= 8; // FX_ATLAS_LOOP_ONLY_N_TIMES
									new_elem->atlas.loopCount = static_cast<unsigned char>(std::clamp(mods.atlas->m_loopCount, 0, 255));
								}
							}
							new_elem->atlas.behavior = behavior;
							new_elem->atlas.fps = fps;
						}
					}

					// emissive / lighting
					bool emissive = false;
					if (is_sprite_type(type))
					{
						for (const auto* vis : visuals)
						{
							if (fxeffectdef::is_fx_emissive_material(static_cast<const Material*>(vis)))
							{
								emissive = true;
							}
						}
					}
					new_elem->elemLitType = zonetool::h1::FX_ELEM_LIT_TYPE_NONE;
					new_elem->lightingFrac = 0;

					// visual state samples
					const bool has_graphs = mods.color_graph || mods.color_lerp || mods.size_graph || mods.size_lerp || mods.rotation;
					const int vis_intervals = has_graphs ? max_vis_intervals : 1;
					new_elem->visStateIntervalCount = static_cast<unsigned char>(vis_intervals);
					new_elem->visSamples = allocator.allocate_array<zonetool::h1::FxElemVisStateSample>(vis_intervals + 1);
					result.extra_size += (vis_intervals + 1) * static_cast<int>(sizeof(zonetool::h1::FxElemVisStateSample));

					float init_color_min[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
					float init_color_max[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
					float init_size_min[2] = { 8.0f, 8.0f };
					float init_size_max[2] = { 8.0f, 8.0f };
					if (mods.attributes)
					{
						for (auto chan = 0; chan < 4; chan++)
						{
							init_color_min[chan] = sane(mods.attributes->m_colorMin.v[chan], 1.0f);
							init_color_max[chan] = sane(mods.attributes->m_colorMax.v[chan], init_color_min[chan]);
						}
						for (auto axis = 0; axis < 2; axis++)
						{
							init_size_min[axis] = std::fabs(sane(mods.attributes->m_sizeMin.v[axis]));
							init_size_max[axis] = std::fabs(sane(mods.attributes->m_sizeMax.v[axis], init_size_min[axis]));
						}
						// uniform sizes often leave y at 0
						if (init_size_min[1] == 0.0f) init_size_min[1] = init_size_min[0];
						if (init_size_max[1] == 0.0f) init_size_max[1] = init_size_max[0];
						if (init_size_min[0] == 0.0f && init_size_max[0] == 0.0f)
						{
							init_size_min[0] = init_size_max[0] = init_size_min[1] = init_size_max[1] = 8.0f;
						}
					}

					const float size_scale = type == zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT ? 1.0f : sprite_size_scale;
					float max_size = 0.0f;
					for (auto sample = 0; sample <= vis_intervals; sample++)
					{
						const float sample_time = static_cast<float>(sample) / static_cast<float>(vis_intervals);
						auto& out = new_elem->visSamples[sample];

						// colour + alpha
						for (auto chan = 0; chan < 4; chan++)
						{
							float base = init_color_min[chan];
							float top = init_color_max[chan];
							if (mods.color_graph)
							{
								const auto graph = sample_graph(mods.color_graph->m_curves, 4, chan, sample_time, 1.0f);
								base *= graph.base;
								top *= graph.base + graph.amplitude;
							}
							else if (mods.color_lerp)
							{
								const float lerped = lerp_value(sane(mods.color_lerp->m_colorBegin.v[chan], 1.0f), sane(mods.color_lerp->m_colorEnd.v[chan], 1.0f), sample_time);
								base *= lerped;
								top *= lerped;
							}
							base = std::clamp(base, 0.0f, 1.0f);
							top = std::clamp(top, 0.0f, 1.0f);
							out.base.color[chan] = base;
							out.amplitude.color[chan] = top - base;
						}

						// size
						for (auto axis = 0; axis < 2; axis++)
						{
							float base = init_size_min[axis];
							float top = init_size_max[axis];
							if (mods.size_graph)
							{
								const auto& graph_mod = *mods.size_graph;
								const auto graph = sample_graph(graph_mod.m_curves, 3, axis, sample_time, 1.0f);
								const float begin = sane(graph_mod.m_sizeBegin.v[axis]);
								const float end = sane(graph_mod.m_sizeEnd.v[axis]);
								if (begin != 0.0f || end != 0.0f)
								{
									base = lerp_value(begin, end, graph.base);
									top = lerp_value(begin, end, graph.base + graph.amplitude);
								}
								else
								{
									base *= graph.base;
									top *= graph.base + graph.amplitude;
								}
							}
							else if (mods.size_lerp)
							{
								const float begin = sane(mods.size_lerp->m_sizeBegin.v[axis]);
								const float end = sane(mods.size_lerp->m_sizeEnd.v[axis]);
								if (begin != 0.0f || end != 0.0f)
								{
									const float lerped = lerp_value(begin, end, sample_time);
									top = lerped * (base > 0.0f ? top / base : 1.0f);
									base = lerped;
								}
							}
							base = std::fabs(base) * size_scale;
							top = std::fabs(top) * size_scale;
							out.base.size[axis] = base;
							out.amplitude.size[axis] = top - base;
							max_size = std::max(max_size, std::max(base, top));
						}

						// rotation (radians, total over life)
						const float rot_total = rot_rate_min * sample_time * life_avg_sec;
						const float rot_total_top = rot_rate_max * sample_time * life_avg_sec;
						out.base.rotationTotal = rot_total;
						out.amplitude.rotationTotal = rot_total_top - rot_total;
						out.base.rotationDelta = rot_rate_min * life_avg_sec;
						out.amplitude.rotationDelta = (rot_rate_max - rot_rate_min) * life_avg_sec;

						out.base.scale = 1.0f;
						out.amplitude.scale = 0.0f;
						const float emissive_scale = 1.0f; // unlit
						for (auto chan = 0; chan < 3; chan++)
						{
							out.base.emissiveScale[chan] = emissive_scale;
							out.amplitude.emissiveScale[chan] = 0.0f;
						}
					}
					result.max_radius += max_size;

					// velocity samples (H1 interpolates totalDelta over the element life)
					const bool world_velocity = mods.relative_velocity && (mods.relative_velocity->m_velocityType == PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD
						|| mods.relative_velocity->m_velocityType == PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD_WITH_BOLT_INFO);
					const int vel_intervals = mods.velocity_graph ? max_vis_intervals : 1;
					new_elem->velIntervalCount = static_cast<unsigned char>(vel_intervals);
					new_elem->velSamples = allocator.allocate_array<zonetool::h1::FxElemVelStateSample>(vel_intervals + 1);
					result.extra_size += (vel_intervals + 1) * static_cast<int>(sizeof(zonetool::h1::FxElemVelStateSample));

					float vel_min[3] = {}, vel_max[3] = {};
					if (mods.attributes)
					{
						for (auto axis = 0; axis < 3; axis++)
						{
							vel_min[axis] = sane(mods.attributes->m_velocityMin.v[axis]);
							vel_max[axis] = sane(mods.attributes->m_velocityMax.v[axis], vel_min[axis]);
						}
					}

					bool any_velocity = false;
					float prev_base[3] = {}, prev_amp[3] = {};
					float delta_base[3] = {}, delta_amp[3] = {};
					float travel = 0.0f;
					for (auto sample = 0; sample <= vel_intervals; sample++)
					{
						const float sample_time = static_cast<float>(sample) / static_cast<float>(vel_intervals);
						auto& frame = world_velocity ? new_elem->velSamples[sample].world : new_elem->velSamples[sample].local;
						for (auto axis = 0; axis < 3; axis++)
						{
							float base = vel_min[axis];
							float top = vel_max[axis];
							if (mods.velocity_graph)
							{
								const auto& graph_mod = *mods.velocity_graph;
								const auto graph = sample_graph(graph_mod.m_curves, 3, axis, sample_time, 0.0f);
								const float begin = sane(graph_mod.m_velocityBegin.v[axis]);
								const float end = sane(graph_mod.m_velocityEnd.v[axis]);
								if (begin != 0.0f || end != 0.0f)
								{
									base += lerp_value(begin, end, graph.base);
									top += lerp_value(begin, end, graph.base + graph.amplitude);
								}
								else
								{
									base += graph.base;
									top += graph.base + graph.amplitude;
								}
							}
							const float amp = top - base;
							if (sample > 0)
							{
								const float step = (1.0f / static_cast<float>(vel_intervals)) * life_avg_sec;
								delta_base[axis] += 0.5f * (prev_base[axis] + base) * step;
								delta_amp[axis] += 0.5f * (prev_amp[axis] + amp) * step;
							}
							prev_base[axis] = base;
							prev_amp[axis] = amp;
							if (base != 0.0f || amp != 0.0f) any_velocity = true;

							// velocity in units per element life (consistent with totalDelta)
							frame.velocity.base[axis] = base * life_avg_sec;
							frame.velocity.amplitude[axis] = amp * life_avg_sec;
							frame.totalDelta.base[axis] = delta_base[axis];
							frame.totalDelta.amplitude[axis] = delta_amp[axis];
						}
					}
					for (auto axis = 0; axis < 3; axis++)
					{
						travel = std::max(travel, std::fabs(delta_base[axis]) + std::fabs(delta_amp[axis]));
					}
					result.max_radius += travel;
					if (any_velocity)
					{
						elem_flags |= world_velocity ? zonetool::h1::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD : zonetool::h1::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL;
					}

					// visuals
					new_elem->visualCount = 0;
					if (type != zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT)
					{
						new_elem->visualCount = static_cast<unsigned char>(visuals.size());
						if (visuals.size() > 1)
						{
							new_elem->visuals.array = allocator.allocate_array<zonetool::h1::FxElemVisuals>(visuals.size());
							for (auto vis_index = 0u; vis_index < visuals.size(); vis_index++)
							{
								new_elem->visuals.array[vis_index].anonymous = visuals[vis_index];
							}
							result.extra_size += static_cast<int>(visuals.size() * sizeof(void*));
						}
						else if (visuals.size() == 1)
						{
							new_elem->visuals.instance.anonymous = visuals[0];
						}
					}
					else
					{
						auto* omni = allocator.allocate<zonetool::h1::FxOmniLightDef>();
						omni->bulbRadius = 1.0f;
						omni->bulbLength = 0.0f;
						omni->fadeOffsetRt[0] = 0.0f;
						omni->fadeOffsetRt[1] = -1.0f;
						if (mods.light_spot)
						{
							omni->bulbRadius = std::max(sane(mods.light_spot->m_bulbRadius, 1.0f), 1.0f);
						}
						new_elem->extended.omniLightDef = omni;
						result.extra_size += static_cast<int>(sizeof(zonetool::h1::FxOmniLightDef));
					}

					new_elem->sortOrder = 5;
					new_elem->fadeInfo = 0;
					new_elem->fadeOutInfo = 0;
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

					new_elem->flags = static_cast<zonetool::h1::FxElemDefFlags>(elem_flags);

					if (false) // v105: IS_DEBUG is not defined in this build
					{
						ZONETOOL_INFO("v104 vfx: %s emitter -> type %d, %s, life %.0f-%.0fms, %d visuals, size %.1f", sys_def->name,
							static_cast<int>(type), result.looping ? "looping" : "oneshot", life_min, life_max, static_cast<int>(visuals.size()), max_size);
					}
					return result;
				}
			}

			void dump(ParticleSystemDef* asset)
			{
				if (!asset || !asset->name || vfx_queue_names.contains(asset->name))
				{
					return;
				}
				vfx_queue_names.insert(asset->name);
				vfx_queue.push_back(asset);
			}

			std::vector<ParticleSystemDef*>& queued()
			{
				return vfx_queue;
			}

			void clear_queue()
			{
				vfx_queue.clear();
				vfx_queue_names.clear();
			}

			int surviving_emitters(ParticleSystemDef* asset, const alive_set* alive, bool ambient)
			{
				(void)ambient;
				if (!asset || !asset->emitterDefs || asset->numEmitters <= 0)
				{
					return 0;
				}
				int count = 0;
				for (auto emitter_index = 0; emitter_index < asset->numEmitters; emitter_index++)
				{
					if (emitter_skip_reason(asset->emitterDefs[emitter_index], alive, nullptr) == skip_reason::none)
					{
						count++;
					}
				}
				return count;
			}

			zonetool::h1::FxEffectDef* convert(ParticleSystemDef* asset, utils::memory::allocator& allocator,
				const alive_set* alive, bool ambient, int* dropped_out)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::FxEffectDef>();
				new_asset->name = asset->name;
				new_asset->flags = 0;
				new_asset->occlusionQueryScaleRange.base = 1.0f;

				const int emitter_count = (asset->emitterDefs && asset->numEmitters > 0) ? asset->numEmitters : 0;
				new_asset->elemDefs = allocator.allocate_array<zonetool::h1::FxElemDef>(std::max(emitter_count, 1));

				// looping elements first, then one-shots (H1 element order)
				std::vector<zonetool::h1::FxElemDef> looping_elems;
				std::vector<zonetool::h1::FxElemDef> oneshot_elems;
				int dropped = 0;
				int extra_size = 0;
				float max_radius = std::max(sane(asset->drawFrustumCullRadius), 0.0f);
				float looping_life = 0.0f;

				for (auto emitter_index = 0; emitter_index < emitter_count; emitter_index++)
				{
					const auto& emitter = asset->emitterDefs[emitter_index];
					auto type = zonetool::h1::FX_ELEM_TYPE_VECTORFIELD;
					const auto reason = emitter_skip_reason(emitter, alive, &type);
					if (reason != skip_reason::none)
					{
						dropped++;
						ZONETOOL_INFO("v104 vfx: %s emitter %d skipped (%s)", asset->name, emitter_index, skip_reason_name(reason));
						continue;
					}

					zonetool::h1::FxElemDef elem{};
					const auto built = build_elem(asset, emitter, type, alive, ambient, &elem, new_asset, allocator);
					max_radius = std::max(max_radius, built.max_radius);
					extra_size += built.extra_size;
					if (built.looping)
					{
						looping_life = std::max(looping_life, built.max_life_msec + static_cast<float>(elem.spawnDelayMsec.base + elem.spawnDelayMsec.amplitude));
						looping_elems.push_back(elem);
					}
					else
					{
						oneshot_elems.push_back(elem);
					}
				}

				int out_index = 0;
				for (const auto& elem : looping_elems) new_asset->elemDefs[out_index++] = elem;
				for (const auto& elem : oneshot_elems) new_asset->elemDefs[out_index++] = elem;
				new_asset->elemDefCountLooping = static_cast<int>(looping_elems.size());
				new_asset->elemDefCountOneShot = static_cast<int>(oneshot_elems.size());
				new_asset->elemDefCountEmission = 0;
				new_asset->msecLoopingLife = static_cast<int>(looping_life);
				new_asset->elemMaxRadius = max_radius;
				new_asset->totalSize = static_cast<int>(sizeof(zonetool::h1::FxEffectDef) + std::strlen(asset->name) + 1
					+ out_index * sizeof(zonetool::h1::FxElemDef)) + extra_size;

				if (dropped_out)
				{
					*dropped_out = dropped;
				}
				return new_asset;
			}
		}
	}
}
