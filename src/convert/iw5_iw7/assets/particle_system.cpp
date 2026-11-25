#include <std_include.hpp>

#include "../include.hpp"
#include "particle_system.hpp"

#include <iw7/assets/particle_system.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace particle_system
	{
		namespace
		{
			namespace source = zonetool::iw5;
			namespace target = zonetool::iw7;

			target::PARTICLE_ELEMENT_TYPE convert_elem_type(const source::FxElemType type)
			{
				switch (type)
				{
				case source::FX_ELEM_TYPE_SPRITE_BILLBOARD: return target::PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE;
				case source::FX_ELEM_TYPE_SPRITE_ORIENTED: return target::PARTICLE_ELEMENT_TYPE_ORIENTED_SPRITE;
				case source::FX_ELEM_TYPE_TAIL: return target::PARTICLE_ELEMENT_TYPE_TAIL;
				case source::FX_ELEM_TYPE_TRAIL: return target::PARTICLE_ELEMENT_TYPE_GEO_TRAIL;
				case source::FX_ELEM_TYPE_CLOUD: return target::PARTICLE_ELEMENT_TYPE_CLOUD;
				case source::FX_ELEM_TYPE_SPARKCLOUD: return target::PARTICLE_ELEMENT_TYPE_SPARK_CLOUD;
				case source::FX_ELEM_TYPE_MODEL: return target::PARTICLE_ELEMENT_TYPE_MODEL;
				case source::FX_ELEM_TYPE_OMNI_LIGHT: return target::PARTICLE_ELEMENT_TYPE_LIGHT_OMNI;
				case source::FX_ELEM_TYPE_SPOT_LIGHT: return target::PARTICLE_ELEMENT_TYPE_LIGHT_SPOT;
				case source::FX_ELEM_TYPE_DECAL: return target::PARTICLE_ELEMENT_TYPE_DECAL;
				case source::FX_ELEM_TYPE_RUNNER: return target::PARTICLE_ELEMENT_TYPE_RUNNER;
				default: return target::PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE;
				}
			}

			bool is_cloud(const source::FxElemDef* elem)
			{
				return elem->elemType == source::FX_ELEM_TYPE_CLOUD || elem->elemType == source::FX_ELEM_TYPE_SPARKCLOUD;
			}

			bool is_sprite(const source::FxElemDef* elem)
			{
				switch (elem->elemType)
				{
				case source::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				case source::FX_ELEM_TYPE_SPRITE_ORIENTED:
				case source::FX_ELEM_TYPE_TAIL:
				case source::FX_ELEM_TYPE_TRAIL:
					return true;
				default:
					return false;
				}
			}

			const source::FxElemVisuals& visual_at(const source::FxElemDef* elem, const int index)
			{
				return elem->visualCount > 1 ? elem->visuals.array.get()[index] : elem->visuals.instance;
			}

			bool is_elem_convertible(const source::FxEffectDef* asset, const source::FxElemDef* elem)
			{
				switch (elem->elemType)
				{
				case source::FX_ELEM_TYPE_SPARKFOUNTAIN:
					ZONETOOL_WARNING("Effect \"%s\": IW7 has no spark fountain element, skipping it", asset->name.get());
					return false;
				case source::FX_ELEM_TYPE_SOUND:
					ZONETOOL_WARNING("Effect \"%s\": IW7 has no sound element, skipping it", asset->name.get());
					return false;
				case source::FX_ELEM_TYPE_OMNI_LIGHT:
				case source::FX_ELEM_TYPE_SPOT_LIGHT:
					return true;
				case source::FX_ELEM_TYPE_DECAL:
					return elem->visualCount && elem->visuals.markArray.get();
				default:
					break;
				}

				if (!elem->visualCount || (elem->visualCount > 1 && !elem->visuals.array.get()))
				{
					ZONETOOL_WARNING("Effect \"%s\": element without visuals, skipping it", asset->name.get());
					return false;
				}

				for (auto i = 0; i < elem->visualCount; i++)
				{
					if (!visual_at(elem, i).anonymous.get())
					{
						ZONETOOL_WARNING("Effect \"%s\": element with a null visual, skipping it", asset->name.get());
						return false;
					}
				}

				return true;
			}

			template <typename T>
			T* name_reference(const std::string& name, utils::memory::allocator& allocator)
			{
				const auto reference = allocator.manual_allocate<T>(sizeof(const char*));
				reference->name = allocator.duplicate_string(name);
				return reference;
			}

			target::ParticleLinkedAssetDef* allocate_assets(target::ParticleLinkedAssetListDef& list, const int count, utils::memory::allocator& allocator)
			{
				list.numAssets = count;
				list.assetList = allocator.allocate_array<target::ParticleLinkedAssetDef>(count);
				return list.assetList;
			}

			void set_child_effect(target::ParticleLinkedAssetListDef& list, const char* effect_name, utils::memory::allocator& allocator)
			{
				allocate_assets(list, 1, allocator)->particleSystem = name_reference<target::ParticleSystemDef>(asset_name(effect_name), allocator);
			}

			template <typename T>
			T& add_module(emitter_build& build, const target::ParticleModuleType type, T target::ParticleModuleTypeDef::* data)
			{
				auto& module_def = build.modules.emplace_back();
				module_def.moduleType = type;
				(module_def.moduleData.*data).type = type;
				return module_def.moduleData.*data;
			}

			void allocate_curve(target::ParticleCurveDef& curve, const int count, const float scale, utils::memory::allocator& allocator)
			{
				curve.scale = scale;
				curve.numControlPoints = count;
				curve.controlPoints = allocator.allocate_array<target::ParticleCurveControlPointDef>(count);
			}

			void set_flat_curve(target::ParticleCurveDef& curve, const float value, const float scale, utils::memory::allocator& allocator)
			{
				allocate_curve(curve, 2, scale, allocator);
				curve.controlPoints[0].value = value;
				curve.controlPoints[1].time = 1.0f;
				curve.controlPoints[1].value = value;
			}

			void set_inverse_time_deltas(target::ParticleCurveDef* curves, const int count)
			{
				for (auto i = 0; i < count; i++)
				{
					for (auto point = 1; point < curves[i].numControlPoints; point++)
					{
						curves[i].controlPoints[point].invTimeDelta = 1.0f / (curves[i].controlPoints[point].time - curves[i].controlPoints[point - 1].time);
					}
				}
			}

			void flag_random_curves(const target::ParticleCurveDef& first, const target::ParticleCurveDef& second, unsigned int& flags)
			{
				if (first.numControlPoints != second.numControlPoints || first.scale != second.scale)
				{
					return;
				}

				for (auto i = 0; i < first.numControlPoints; i++)
				{
					if (first.controlPoints[i].value != second.controlPoints[i].value)
					{
						flags |= target::PARTICLE_MODULE_FLAG_RANDOMIZE_BETWEEN_CURVES;
					}
				}
			}

			int point_count(const int interval_count)
			{
				return std::max(interval_count, 1) + 1;
			}

			float point_time(const int point, const int count)
			{
				return static_cast<float>(point) / static_cast<float>(count - 1);
			}

			const source::FxElemVisStateSample& visual_sample(const source::FxElemDef* elem, const int point)
			{
				return elem->visSamples.get()[std::min<int>(point, elem->visStateIntervalCount)];
			}

			const source::FxElemVelStateSample& velocity_sample(const source::FxElemDef* elem, const int point)
			{
				return elem->velSamples.get()[std::min<int>(point, elem->velIntervalCount)];
			}

			void add_init_spawn_module(emitter_build& build)
			{
				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_SPAWN, &target::ParticleModuleTypeDef::initSpawn);

				allocate_curve(data.m_curves[0], 3, 1.0f, *build.allocator);
				data.m_curves[0].controlPoints[0].value = 1.0f;
				data.m_curves[0].controlPoints[1].time = 0.75f;
				data.m_curves[0].controlPoints[1].value = 1.0f;
				data.m_curves[0].controlPoints[2].time = 1.0f;
				set_inverse_time_deltas(data.m_curves, 1);
			}

			void add_init_attributes_module(emitter_build& build)
			{
				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_ATTRIBUTES, &target::ParticleModuleTypeDef::initAttributes);

				data.m_useNonUniformInterpolationForSize = (build.elem->flags & source::FX_ELEM_NONUNIFORM_SCALE) != 0;
				for (auto i = 0; i < 3; i++)
				{
					data.m_sizeMin.v[i] = 10.0f;
					data.m_sizeMax.v[i] = 10.0f;
				}
				for (auto i = 0; i < 4; i++)
				{
					data.m_colorMin.v[i] = 1.0f;
					data.m_colorMax.v[i] = 1.0f;
				}
			}

			void add_init_cloud_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (!is_cloud(elem))
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_CLOUD, &target::ParticleModuleTypeDef::initCloud);

				const auto count = point_count(elem->visStateIntervalCount);
				auto largest = 0.0f;
				if (elem->visSamples.get())
				{
					for (auto i = 0; i < count; i++)
					{
						const auto& sample = visual_sample(elem, i);
						largest = std::max({ largest, std::abs(sample.base.scale), std::abs(sample.base.scale + sample.amplitude.scale) });
					}
				}

				for (auto curve_index = 0; curve_index < 2; curve_index++)
				{
					auto& curve = data.curves[curve_index];
					if (!largest)
					{
						set_flat_curve(curve, 1.0f, 1.0f, *build.allocator);
						continue;
					}

					allocate_curve(curve, count, largest, *build.allocator);
					for (auto i = 0; i < count; i++)
					{
						const auto& sample = visual_sample(elem, i);
						curve.controlPoints[i].time = point_time(i, count);
						curve.controlPoints[i].value = (curve_index == 0 ? sample.base.scale : sample.base.scale + sample.amplitude.scale) / largest;
					}
				}

				set_inverse_time_deltas(data.curves, 2);
				flag_random_curves(data.curves[0], data.curves[1], data.m_flags);
			}

			void add_init_tail_module(emitter_build& build)
			{
				if (build.elem->elemType != source::FX_ELEM_TYPE_TAIL)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_TAIL, &target::ParticleModuleTypeDef::initTail);
				data.m_tailLeading = true;
			}

			void add_init_geo_trail_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (elem->elemType != source::FX_ELEM_TYPE_TRAIL)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_GEO_TRAIL, &target::ParticleModuleTypeDef::initGeoTrail);
				data.m_numPointsMax = 16;
				data.m_numSheets = 2;
				if (const auto trail = elem->extended.trailDef.get())
				{
					data.m_splitDistance = static_cast<float>(trail->repeatDist);
					data.m_tileDistance = static_cast<float>(trail->repeatDist);
					data.m_scrollTime = static_cast<float>(trail->scrollTimeMsec) / 1000.0f;
				}
			}

			void set_light_def(emitter_build& build, unsigned int& flags, target::ParticleLinkedAssetListDef& list)
			{
				const auto elem = build.elem;
				const auto light_def = elem->visualCount ? visual_at(elem, 0).lightDef.get() : nullptr;
				const auto name = light_def && light_def->name.get() && *light_def->name.get() ? light_def->name.get() : "light_fx_default";

				allocate_assets(list, 1, *build.allocator)->lightDef = name_reference<target::GfxLightDef>(name, *build.allocator);
				flags |= target::PARTICLE_MODULE_FLAG_HAS_LIGHT_DEFS;
				build.system_flags |= target::PARTICLE_SYSTEM_DEF_FLAG_HAS_LIGHTS;
				build.emitter_flags |= target::PARTICLE_EMITTER_DEF_FLAG_HAS_LIGHTS;
			}

			void add_init_omni_light_module(emitter_build& build)
			{
				if (build.elem->elemType != source::FX_ELEM_TYPE_OMNI_LIGHT)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_LIGHT_OMNI, &target::ParticleModuleTypeDef::initLightOmni);
				data.m_tonemappingScaleFactor = 1.0f;
				set_light_def(build, data.m_flags, data.m_linkedAssetList);
			}

			void add_init_spot_light_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (elem->elemType != source::FX_ELEM_TYPE_SPOT_LIGHT)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_LIGHT_SPOT, &target::ParticleModuleTypeDef::initLightSpot);

				const auto spot = elem->extended.spotLightDef.get();
				data.m_fovOuter = 0.7853981852531433f; // IW5 fx has no outer fov
				data.m_fovInner = spot ? data.m_fovOuter * spot->fovInnerFraction : 0.0f;
				data.m_bulbRadius = spot ? spot->startRadius : 1.0f;
				data.m_bulbLength = 0.3f;
				data.m_brightness = spot ? spot->brightness : 1.0f;
				data.m_shadowSoftness = 0.5f;
				data.m_shadowBias = 0.4f;
				data.m_shadowArea = 0.01f;
				data.m_toneMappingScaleFactor = 1.0f;
				data.m_exponent = spot ? spot->exponent : 0;
				set_light_def(build, data.m_flags, data.m_linkedAssetList);
			}

			void add_init_model_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (elem->elemType != source::FX_ELEM_TYPE_MODEL)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_MODEL, &target::ParticleModuleTypeDef::initModel);

				const auto assets = allocate_assets(data.m_linkedAssetList, elem->visualCount, *build.allocator);
				for (auto i = 0; i < elem->visualCount; i++)
				{
					assets[i].model = name_reference<target::XModel>(visual_at(elem, i).model.get()->name.get(), *build.allocator);
				}
			}

			void add_init_runner_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (elem->elemType != source::FX_ELEM_TYPE_RUNNER)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_RUNNER, &target::ParticleModuleTypeDef::initRunner);

				const auto assets = allocate_assets(data.m_linkedAssetList, elem->visualCount, *build.allocator);
				for (auto i = 0; i < elem->visualCount; i++)
				{
					assets[i].particleSystem = name_reference<target::ParticleSystemDef>(asset_name(visual_at(elem, i).effectDef.name.get()), *build.allocator);
				}

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS;
			}

			void add_init_decal_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (elem->elemType != source::FX_ELEM_TYPE_DECAL)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_DECAL, &target::ParticleModuleTypeDef::initDecal);
				data.m_lerpWaitTime = 1280;
				for (auto& channel : data.m_lerpColor.v)
				{
					channel = 1.0f;
				}

				const auto marks = elem->visuals.markArray.get();
				const auto assets = allocate_assets(data.m_linkedAssetList, elem->visualCount, *build.allocator);
				for (auto i = 0; i < elem->visualCount; i++)
				{
					if (const auto model_mark = marks[i].materials[0].get())
					{
						assets[i].decal.materials[0] = name_reference<target::Material>(model_mark->info.name.get(), *build.allocator);
					}
					if (const auto world_mark = marks[i].materials[1].get())
					{
						assets[i].decal.materials[1] = name_reference<target::Material>(world_mark->info.name.get(), *build.allocator);
						assets[i].decal.materials[2] = name_reference<target::Material>(world_mark->info.name.get(), *build.allocator);
					}
				}
			}

			void add_init_oriented_sprite_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (elem->elemType != source::FX_ELEM_TYPE_SPRITE_ORIENTED)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_ORIENTED_SPRITE, &target::ParticleModuleTypeDef::initOrientedSprite);
				for (auto& component : data.m_orientationQuat.v)
				{
					component = 0.5f;
				}

				if ((elem->flags & source::FX_ELEM_RUN_MASK) == source::FX_ELEM_RUN_RELATIVE_TO_SPAWN)
				{
					data.m_orientationQuat.v[1] = -0.5f;
					data.m_orientationQuat.v[2] = -0.5f;
				}
			}

			void add_init_material_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (!is_sprite(elem) && !is_cloud(elem))
				{
					build.system_flags |= target::PARTICLE_SYSTEM_DEF_FLAG_HAS_NON_SPRITES;
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_MATERIAL, &target::ParticleModuleTypeDef::initMaterial);

				const auto assets = allocate_assets(data.m_linkedAssetList, elem->visualCount, *build.allocator);
				for (auto i = 0; i < elem->visualCount; i++)
				{
					assets[i].material = name_reference<target::Material>(visual_at(elem, i).material.get()->info.name.get(), *build.allocator);
				}

				if (is_cloud(elem))
				{
					build.system_flags |= target::PARTICLE_SYSTEM_DEF_FLAG_HAS_NON_SPRITES;
					return;
				}

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_IS_SPRITE;
				build.system_flags |= target::PARTICLE_SYSTEM_DEF_FLAG_HAS_SPRITES;
				if (elem->lightingFrac)
				{
					build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_GPU_LIGHTING;
				}
			}

			void add_init_atlas_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (!is_sprite(elem) && elem->elemType != source::FX_ELEM_TYPE_CLOUD)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_ATLAS, &target::ParticleModuleTypeDef::initAtlas);

				const auto random_start = (elem->atlas.behavior & source::FX_ATLAS_START_MASK) == source::FX_ATLAS_START_RANDOM;
				data.m_startFrame = random_start ? elem->atlas.entryCount - 1 : 0;
				data.m_loopCount = (elem->atlas.behavior & source::FX_ATLAS_LOOP_ONLY_N_TIMES) != 0 ? elem->atlas.loopCount : -1;
				data.m_playRate = elem->atlas.fps;
			}

			void add_init_relative_velocity_module(emitter_build& build)
			{
				const auto elem = build.elem;
				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_RELATIVE_VELOCITY, &target::ParticleModuleTypeDef::initRelativeVelocity);

				switch (elem->flags & source::FX_ELEM_RUN_MASK)
				{
				case source::FX_ELEM_RUN_RELATIVE_TO_WORLD:
					data.m_velocityType = target::PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD;
					break;
				case source::FX_ELEM_RUN_RELATIVE_TO_OFFSET:
					data.m_velocityType = target::PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN;
					break;
				default:
					data.m_velocityType = target::PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL;
					break;
				}

				if (elem->elemType == source::FX_ELEM_TYPE_TRAIL)
				{
					data.m_velocityType = target::PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL_WITH_BOLT_INFO;
					data.m_useBoltInfo = true;
				}
			}

			void add_init_rotation_module(emitter_build& build)
			{
				const auto& rotation = build.elem->initialRotation;
				if (rotation.base == 0.0f && rotation.amplitude == 0.0f)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_ROTATION, &target::ParticleModuleTypeDef::initRotation);
				data.m_rotationAngle.min = rotation.base;
				data.m_rotationAngle.max = rotation.base + rotation.amplitude;

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D_INIT;
			}

			bool is_range_used(const source::FxFloatRange* ranges)
			{
				return std::any_of(ranges, ranges + 3, [](const source::FxFloatRange& range) { return range.base != 0.0f || range.amplitude != 0.0f; });
			}

			void add_init_rotation_3d_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (!is_range_used(elem->spawnAngles) && !is_range_used(elem->angularVelocity) && !is_cloud(elem))
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_ROTATION_3D, &target::ParticleModuleTypeDef::initRotation3D);
				for (auto i = 0; i < 3; i++)
				{
					data.m_rotationRateMin.v[i] = elem->angularVelocity[i].base * 1000.0f;
					data.m_rotationRateMax.v[i] = (elem->angularVelocity[i].base + elem->angularVelocity[i].amplitude) * 1000.0f;
					data.m_rotationAngleMin.v[i] = elem->spawnAngles[i].base;
					data.m_rotationAngleMax.v[i] = elem->spawnAngles[i].base + elem->spawnAngles[i].amplitude;
				}

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_3D_INIT;
			}

			void add_init_spawn_shape_box_module(emitter_build& build)
			{
				const auto elem = build.elem;
				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX, &target::ParticleModuleTypeDef::initSpawnShapeBox);
				data.m_axisFlags = target::PARTICLE_MODULE_AXES_FLAG_ALL;

				if ((elem->flags & source::FX_ELEM_SPAWN_RELATIVE_TO_EFFECT) == 0 && elem->elemType != source::FX_ELEM_TYPE_TRAIL)
				{
					data.m_flags |= target::PARTICLE_MODULE_FLAG_USE_WORLD_SPACE;
				}

				for (auto i = 0; i < 3; i++)
				{
					data.m_dimensionsMin.v[i] = elem->spawnOrigin[i].base;
					data.m_dimensionsMax.v[i] = elem->spawnOrigin[i].base + elem->spawnOrigin[i].amplitude;
				}

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE;
			}

			void add_init_spawn_shape_cylinder_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if ((elem->flags & source::FX_ELEM_SPAWN_OFFSET_MASK) != source::FX_ELEM_SPAWN_OFFSET_CYLINDER)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER, &target::ParticleModuleTypeDef::initSpawnShapeCylinder);
				data.m_axisFlags = target::PARTICLE_MODULE_AXES_FLAG_ALL;

				data.m_hasRotation = true;
				data.m_directionQuat.v[1] = 0.7071067690849304f; // z axis to x axis
				data.m_directionQuat.v[3] = 0.7071067690849304f;
				data.m_radius.min = elem->spawnOffsetRadius.base;
				data.m_radius.max = elem->spawnOffsetRadius.base + elem->spawnOffsetRadius.amplitude;
				data.m_halfHeight = elem->spawnOffsetHeight.amplitude * 0.5f;
				data.unk.v[0] = elem->spawnOffsetHeight.base + data.m_halfHeight;

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE;
			}

			void add_init_spawn_shape_sphere_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if ((elem->flags & source::FX_ELEM_SPAWN_OFFSET_MASK) != source::FX_ELEM_SPAWN_OFFSET_SPHERE)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE, &target::ParticleModuleTypeDef::initSpawnShapeSphere);
				data.m_axisFlags = target::PARTICLE_MODULE_AXES_FLAG_ALL;

				data.m_radius.min = elem->spawnOffsetRadius.base;
				data.m_radius.max = elem->spawnOffsetRadius.base + elem->spawnOffsetRadius.amplitude;

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE;
			}

			void add_color_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (!elem->visSamples.get())
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_COLOR_GRAPH, &target::ParticleModuleTypeDef::colorGraph);

				const auto count = point_count(elem->visStateIntervalCount);
				for (auto& curve : data.m_curves)
				{
					allocate_curve(curve, count, 1.0f, *build.allocator);
				}

				const int channel_order[4] = { 2, 1, 0, 3 }; // bgra to rgba
				for (auto i = 0; i < count; i++)
				{
					const auto& sample = visual_sample(elem, i);
					for (auto channel = 0; channel < 4; channel++)
					{
						data.m_curves[channel].controlPoints[i].value = static_cast<float>(sample.base.color[channel_order[channel]]) / 255.0f;
						data.m_curves[channel + 4].controlPoints[i].value = static_cast<float>(sample.amplitude.color[channel_order[channel]]) / 255.0f;
					}
					for (auto& curve : data.m_curves)
					{
						curve.controlPoints[i].time = point_time(i, count);
					}
				}

				set_inverse_time_deltas(data.m_curves, 8);
				for (auto channel = 0; channel < 4; channel++)
				{
					flag_random_curves(data.m_curves[channel], data.m_curves[channel + 4], data.m_flags);
				}

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_COLOR;
			}

			float size_value(const source::FxElemVisualState& state, const size_channel channel)
			{
				switch (channel)
				{
				case size_channel::width: return state.size[0];
				case size_channel::height: return state.size[1];
				case size_channel::scale: return state.scale;
				default: return 0.0f;
				}
			}

			void add_size_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (!elem->visSamples.get())
				{
					return;
				}

				size_channel axes[3] = { size_channel::none, size_channel::none, size_channel::none };
				switch (elem->elemType)
				{
				case source::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				case source::FX_ELEM_TYPE_SPRITE_ORIENTED:
				case source::FX_ELEM_TYPE_TAIL:
				case source::FX_ELEM_TYPE_TRAIL:
				case source::FX_ELEM_TYPE_CLOUD:
				case source::FX_ELEM_TYPE_SPARKCLOUD:
					axes[0] = size_channel::width;
					axes[1] = size_channel::height;
					break;
				case source::FX_ELEM_TYPE_DECAL:
					axes[0] = size_channel::width;
					axes[1] = size_channel::width;
					break;
				case source::FX_ELEM_TYPE_OMNI_LIGHT:
				case source::FX_ELEM_TYPE_SPOT_LIGHT:
					axes[0] = size_channel::width;
					axes[1] = size_channel::scale;
					break;
				case source::FX_ELEM_TYPE_MODEL:
					axes[0] = size_channel::scale;
					axes[1] = size_channel::scale;
					axes[2] = size_channel::scale;
					break;
				default:
					return;
				}

				const auto count = point_count(elem->visStateIntervalCount);

				float scales[3]{};
				for (auto axis = 0; axis < 3; axis++)
				{
					for (auto i = 0; i < count; i++)
					{
						const auto& sample = visual_sample(elem, i);
						const auto base = size_value(sample.base, axes[axis]);
						scales[axis] = std::max({ scales[axis], std::abs(base), std::abs(base + size_value(sample.amplitude, axes[axis])) });
					}
				}

				if (!scales[0] && !scales[1] && !scales[2])
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_SIZE_GRAPH, &target::ParticleModuleTypeDef::sizeGraph);
				for (auto axis = 0; axis < 3; axis++)
				{
					auto& first = data.m_curves[axis];
					auto& second = data.m_curves[axis + 3];

					if (!scales[axis])
					{
						const auto is_light_scale = axes[axis] == size_channel::scale && elem->elemType != source::FX_ELEM_TYPE_MODEL;
						set_flat_curve(first, 1.0f, is_light_scale ? 1.0f : 0.0f, *build.allocator);
						set_flat_curve(second, 1.0f, is_light_scale ? 1.0f : 0.0f, *build.allocator);
						continue;
					}

					allocate_curve(first, count, scales[axis], *build.allocator);
					allocate_curve(second, count, scales[axis], *build.allocator);
					for (auto i = 0; i < count; i++)
					{
						const auto& sample = visual_sample(elem, i);
						const auto base = size_value(sample.base, axes[axis]);
						first.controlPoints[i].time = point_time(i, count);
						first.controlPoints[i].value = base / scales[axis];
						second.controlPoints[i].time = point_time(i, count);
						second.controlPoints[i].value = (base + size_value(sample.amplitude, axes[axis])) / scales[axis];
					}

					flag_random_curves(first, second, data.m_flags);
				}

				set_inverse_time_deltas(data.m_curves, 6);
			}

			void add_rotation_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (!elem->visSamples.get())
				{
					return;
				}

				const auto count = point_count(elem->visStateIntervalCount);
				const auto rate_per_second = static_cast<float>(count - 1) * 1000.0f;

				auto largest = 0.0f;
				for (auto i = 0; i < count; i++)
				{
					const auto& sample = visual_sample(elem, i);
					largest = std::max({ largest, std::abs(sample.base.rotationDelta), std::abs(sample.amplitude.rotationDelta) });
				}

				const auto scale = largest * rate_per_second * 2.0f;
				if (!scale)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_ROTATION_GRAPH, &target::ParticleModuleTypeDef::rotationGraph);
				data.m_useRotationRate = true;

				allocate_curve(data.m_curves[0], count, scale, *build.allocator);
				allocate_curve(data.m_curves[1], count, scale, *build.allocator);
				for (auto i = 0; i < count; i++)
				{
					const auto& sample = visual_sample(elem, i);
					data.m_curves[0].controlPoints[i].time = point_time(i, count);
					data.m_curves[0].controlPoints[i].value = sample.base.rotationDelta * rate_per_second / scale;
					data.m_curves[1].controlPoints[i].time = point_time(i, count);
					data.m_curves[1].controlPoints[i].value = (sample.base.rotationDelta + sample.amplitude.rotationDelta) * rate_per_second / scale;
				}

				set_inverse_time_deltas(data.m_curves, 2);
				flag_random_curves(data.m_curves[0], data.m_curves[1], data.m_flags);

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_ROTATION_1D_CURVE;
			}

			float length_squared(const float* v)
			{
				return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
			}

			void add_velocity_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if (!elem->velSamples.get())
				{
					return;
				}

				const auto has_local = (elem->flags & source::FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL) != 0;
				const auto has_world = (elem->flags & source::FX_ELEM_HAS_VELOCITY_GRAPH_WORLD) != 0;
				if (!has_local && !has_world)
				{
					return;
				}

				const auto count = point_count(elem->velIntervalCount);
				const auto& last = velocity_sample(elem, count - 1);
				const auto world = has_world && (!has_local || length_squared(last.world.totalDelta.base) > length_squared(last.local.totalDelta.base));

				const auto units_per_second = static_cast<float>(count - 1) * 1000.0f;

				float scales[3]{};
				for (auto i = 0; i < count; i++)
				{
					const auto& sample = velocity_sample(elem, i);
					const auto& velocity = world ? sample.world.velocity : sample.local.velocity;
					for (auto axis = 0; axis < 3; axis++)
					{
						scales[axis] = std::max({ scales[axis], std::abs(velocity.base[axis]), std::abs(velocity.amplitude[axis]) });
					}
				}

				if (!scales[0] && !scales[1] && !scales[2])
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_VELOCITY_GRAPH, &target::ParticleModuleTypeDef::velocityGraph);
				if (world)
				{
					data.m_flags |= target::PARTICLE_MODULE_FLAG_USE_WORLD_SPACE;
				}

				for (auto axis = 0; axis < 3; axis++)
				{
					auto& first = data.m_curves[axis];
					auto& second = data.m_curves[axis + 3];

					const auto scale = scales[axis] * units_per_second * 2.0f;
					if (!scale)
					{
						set_flat_curve(first, 0.0f, 1.0f, *build.allocator);
						set_flat_curve(second, 0.0f, 1.0f, *build.allocator);
						continue;
					}

					allocate_curve(first, count, scale, *build.allocator);
					allocate_curve(second, count, scale, *build.allocator);
					for (auto i = 0; i < count; i++)
					{
						const auto& sample = velocity_sample(elem, i);
						const auto& velocity = world ? sample.world.velocity : sample.local.velocity;
						first.controlPoints[i].time = point_time(i, count);
						first.controlPoints[i].value = velocity.base[axis] * units_per_second / scale;
						second.controlPoints[i].time = point_time(i, count);
						second.controlPoints[i].value = (velocity.base[axis] + velocity.amplitude[axis]) * units_per_second / scale;
					}

					flag_random_curves(first, second, data.m_flags);
				}

				set_inverse_time_deltas(data.m_curves, 6);

				build.state_flags |= world ? target::PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_WORLD : target::PARTICLE_STATE_DEF_FLAG_HAS_VELOCITY_CURVE_LOCAL;
			}

			void add_gravity_module(emitter_build& build)
			{
				const auto& gravity = build.elem->gravity;
				if (gravity.base == 0.0f && gravity.amplitude == 0.0f)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_GRAVITY, &target::ParticleModuleTypeDef::gravity);
				data.m_gravityPercentage.min = gravity.base;
				data.m_gravityPercentage.max = gravity.base + gravity.amplitude;
			}

			void add_physics_ray_cast_module(emitter_build& build)
			{
				const auto elem = build.elem;
				if ((elem->flags & (source::FX_ELEM_USE_COLLISION | source::FX_ELEM_USE_MODEL_PHYSICS)) == 0)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_PHYSICS_RAY_CAST, &target::ParticleModuleTypeDef::physicsRayCast);
				data.m_bounce.min = elem->reflectionFactor.base;
				data.m_bounce.max = elem->reflectionFactor.base + elem->reflectionFactor.amplitude;
				std::memcpy(data.m_bounds.midPoint, elem->collBounds.midPoint, sizeof(data.m_bounds.midPoint));
				std::memcpy(data.m_bounds.halfSize, elem->collBounds.halfSize, sizeof(data.m_bounds.halfSize));
				data.m_useItemClip = elem->useItemClip != 0;

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_RAY_CAST_PHYSICS;
			}

			void add_death_module(emitter_build& build)
			{
				const auto effect = build.elem->effectOnDeath.name.get();
				if (!effect)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_TEST_DEATH, &target::ParticleModuleTypeDef::testDeath);
				data.m_moduleIndex = build.test_module_count++;
				set_child_effect(data.m_eventHandlerData.m_linkedAssetList, effect, *build.allocator);

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS;
			}

			void add_impact_module(emitter_build& build)
			{
				const auto elem = build.elem;
				const auto effect = elem->effectOnImpact.name.get();
				const auto dies_on_touch = (elem->flags & source::FX_ELEM_DIE_ON_TOUCH) != 0;
				if ((elem->flags & source::FX_ELEM_USE_COLLISION) == 0 || (!effect && !dies_on_touch))
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_TEST_IMPACT, &target::ParticleModuleTypeDef::testImpact);
				data.m_moduleIndex = build.test_module_count++;
				data.m_eventHandlerData.m_kill = dies_on_touch;
				if (effect)
				{
					set_child_effect(data.m_eventHandlerData.m_linkedAssetList, effect, *build.allocator);
					build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS;
				}

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HANDLE_ON_IMPACT;
			}

			void add_birth_module(emitter_build& build)
			{
				const auto effect = build.elem->effectEmitted.name.get();
				if (!effect)
				{
					return;
				}

				auto& data = add_module(build, target::PARTICLE_MODULE_TEST_BIRTH, &target::ParticleModuleTypeDef::testBirth);
				data.m_moduleIndex = build.test_module_count++;
				set_child_effect(data.m_eventHandlerData.m_linkedAssetList, effect, *build.allocator);

				build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_CHILD_EFFECTS;
			}

			void move_modules(emitter_build& build, target::ParticleModuleGroupDef& group)
			{
				group.numModules = static_cast<int>(build.modules.size());
				if (group.numModules)
				{
					group.moduleDefs = build.allocator->allocate_array<target::ParticleModuleDef>(build.modules.size());
					std::memcpy(group.moduleDefs, build.modules.data(), sizeof(target::ParticleModuleDef) * build.modules.size());
				}
				build.modules.clear();
			}

			void set_spawn(const source::FxElemDef* elem, const bool looping, target::ParticleEmitterDef* emitter, emitter_build& build)
			{
				emitter->particleSpawnRate.min = 5.0f;
				emitter->particleSpawnRate.max = 5.0f;
				emitter->particleBurstCount.min = 1;
				emitter->particleBurstCount.max = 1;

				if (!looping)
				{
					emitter->particleBurstCount.min = elem->spawn.oneShot.count.base;
					emitter->particleBurstCount.max = elem->spawn.oneShot.count.base + elem->spawn.oneShot.count.amplitude;
					emitter->particleCountMax = std::max<int>(emitter->particleBurstCount.max, 1);
					build.emitter_flags |= target::PARTICLE_EMITTER_DEF_FLAG_USE_BURST_MODE;
					return;
				}

				const auto interval = std::max(elem->spawn.looping.intervalMsec, 1);
				const auto spawn_rate = 1000.0f / static_cast<float>(interval);
				emitter->particleSpawnRate.min = spawn_rate;
				emitter->particleSpawnRate.max = spawn_rate;

				auto alive_count = static_cast<int>(std::ceil(spawn_rate * emitter->particleLife.max)) + 1;
				if (elem->spawn.looping.count != 0x7FFFFFFF)
				{
					const auto particle_count = std::max(elem->spawn.looping.count, 1);
					const auto emitter_life = static_cast<float>(particle_count * interval) / 1000.0f;
					emitter->emitterLife.min = emitter_life;
					emitter->emitterLife.max = emitter_life;
					alive_count = std::min(alive_count, particle_count);
				}

				emitter->particleCountMax = std::max(alive_count, 1);
			}

			void convert_emitter(const source::FxElemDef* elem, const bool looping, target::ParticleEmitterDef* emitter, emitter_build& build)
			{
				build.elem = elem;
				build.state_flags = 0;
				build.emitter_flags = 0;
				build.test_module_count = 0;

				emitter->particleLife.min = static_cast<float>(elem->lifeSpanMsec.base) / 1000.0f;
				emitter->particleLife.max = static_cast<float>(elem->lifeSpanMsec.base + elem->lifeSpanMsec.amplitude) / 1000.0f;
				set_spawn(elem, looping, emitter, build);

				emitter->particleDelay.min = static_cast<float>(elem->spawnDelayMsec.base) / 1000.0f;
				emitter->particleDelay.max = static_cast<float>(elem->spawnDelayMsec.base + elem->spawnDelayMsec.amplitude) / 1000.0f;
				emitter->spawnRangeSq.min = elem->spawnRange.base * elem->spawnRange.base;
				emitter->spawnRangeSq.max = (elem->spawnRange.base + elem->spawnRange.amplitude) * (elem->spawnRange.base + elem->spawnRange.amplitude);
				emitter->spawnFrustumCullRadius = elem->spawnFrustumCullRadius;
				emitter->randomSeed = elem->randomSeed;
				emitter->unk2 = 100.0f;

				if (elem->flags & source::FX_ELEM_DRAW_PAST_FOG)
				{
					build.emitter_flags |= target::PARTICLE_EMITTER_DEF_FLAG_DRAW_PAST_FOG;
				}
				if (elem->flags & source::FX_ELEM_BLOCK_SIGHT)
				{
					build.state_flags |= target::PARTICLE_STATE_DEF_FLAG_BLOCKS_SIGHT;
				}

				emitter->numStates = 1;
				emitter->stateDefs = build.allocator->allocate<target::ParticleStateDef>();

				const auto state = emitter->stateDefs;
				state->elementType = convert_elem_type(static_cast<source::FxElemType>(elem->elemType));
				state->moduleGroupDefs = build.allocator->allocate_array<target::ParticleModuleGroupDef>(target::PARTICLE_MODULE_GROUP_COUNT);

				add_init_spawn_module(build);
				add_init_attributes_module(build);
				add_init_cloud_module(build);
				add_init_tail_module(build);
				add_init_geo_trail_module(build);
				add_init_omni_light_module(build);
				add_init_spot_light_module(build);
				add_init_model_module(build);
				add_init_runner_module(build);
				add_init_decal_module(build);
				add_init_oriented_sprite_module(build);
				add_init_material_module(build);
				add_init_atlas_module(build);
				add_init_relative_velocity_module(build);
				add_init_rotation_module(build);
				add_init_rotation_3d_module(build);
				add_init_spawn_shape_box_module(build);
				add_init_spawn_shape_cylinder_module(build);
				add_init_spawn_shape_sphere_module(build);
				move_modules(build, state->moduleGroupDefs[target::PARTICLE_MODULE_GROUP_INIT]);

				add_color_module(build);
				add_size_module(build);
				add_rotation_module(build);
				add_velocity_module(build);
				add_gravity_module(build);
				add_physics_ray_cast_module(build);
				move_modules(build, state->moduleGroupDefs[target::PARTICLE_MODULE_GROUP_UPDATE]);

				add_death_module(build);
				add_impact_module(build);
				add_birth_module(build);
				move_modules(build, state->moduleGroupDefs[target::PARTICLE_MODULE_GROUP_TEST]);

				emitter->flags = build.emitter_flags;
				state->flags = build.state_flags;
			}
		}

		std::string asset_name(const std::string& effect_name)
		{
			return "vfx/" + effect_name;
		}

		zonetool::iw7::ParticleSystemDef* convert(zonetool::iw5::FxEffectDef* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw7::ParticleSystemDef>();
			new_asset->name = allocator.duplicate_string(asset_name(asset->name.get()));

			emitter_build build{};
			build.allocator = &allocator;
			build.system_flags = zonetool::iw7::PARTICLE_SYSTEM_DEF_FLAG_KILL_STOPPED_INFINITE_EFFECTS;

			const auto elem_count = asset->elemDefCountLooping + asset->elemDefCountOneShot; // emission elems are copies from effectEmitted
			new_asset->emitterDefs = allocator.allocate_array<zonetool::iw7::ParticleEmitterDef>(std::max(elem_count, 1));
			for (auto i = 0; i < elem_count; i++)
			{
				const auto elem = &asset->elemDefs.get()[i];
				if (is_elem_convertible(asset, elem))
				{
					convert_emitter(elem, i < asset->elemDefCountLooping, &new_asset->emitterDefs[new_asset->numEmitters++], build);
				}
			}

			new_asset->flags = build.system_flags;
			new_asset->version = 15;
			new_asset->occlusionOverrideEmitterIndex = -1;
			new_asset->phaseOptions = zonetool::iw7::PARTICLE_PHASE_OPTION_PHASE_NEVER;
			new_asset->drawFrustumCullRadius = -1.0f;
			new_asset->updateFrustumCullRadius = -1.0f;
			new_asset->sunDistance = 100000.0f;
			new_asset->editorRotation.v[3] = 1.0f;

			return new_asset;
		}

		void dump(zonetool::iw5::FxEffectDef* asset)
		{
			utils::memory::allocator allocator;
			zonetool::iw7::particle_system::dump(convert(asset, allocator));
		}
	}
}
