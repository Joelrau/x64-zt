#include <std_include.hpp>

#include "../include.hpp"
#include "vfx.hpp"
#include "lightdef.hpp"
#include "material.hpp"
#include "xmodel.hpp"

#include <iw8/assets/vfx.hpp>

namespace zonetool::iw7::converter::iw8::vfx
{
	namespace
	{
		namespace target = zonetool::iw8;

		using dropped_modules = std::set<std::uint16_t>;

		target::float4 convert_float4(const float4& value)
		{
			target::float4 result{};
			std::memcpy(result.v, value.v, sizeof(result.v));
			return result;
		}

		target::ParticleFloatRange convert_range(const ParticleFloatRange& range)
		{
			return {range.min, range.max};
		}

		target::ParticleCurveDef convert_curve(const ParticleCurveDef& curve, utils::memory::allocator& allocator)
		{
			target::ParticleCurveDef result{};
			result.numControlPoints = curve.controlPoints ? curve.numControlPoints : 0;
			result.scale = curve.scale;
			result.controlPoints = allocator.allocate_array<target::ParticleCurveControlPointDef>(result.numControlPoints);

			for (auto i = 0; i < result.numControlPoints; i++)
			{
				result.controlPoints[i].time = curve.controlPoints[i].time;
				result.controlPoints[i].value = curve.controlPoints[i].value;
				result.controlPoints[i].invTimeDelta = curve.controlPoints[i].invTimeDelta;
			}

			return result;
		}

		template <std::size_t Count>
		void convert_curves(const ParticleCurveDef (&curves)[Count], target::ParticleCurveDef (&result)[Count],
			utils::memory::allocator& allocator)
		{
			for (auto i = 0u; i < Count; i++)
			{
				result[i] = convert_curve(curves[i], allocator);
			}
		}

		target::ParticleCurveDef linear_curve(const float start, const float end, const float scale,
			utils::memory::allocator& allocator)
		{
			target::ParticleCurveDef result{};
			result.numControlPoints = 2;
			result.scale = scale;
			result.controlPoints = allocator.allocate_array<target::ParticleCurveControlPointDef>(2);
			result.controlPoints[0] = {0.0f, start, 0.0f};
			result.controlPoints[1] = {1.0f, end, 1.0f};
			return result;
		}

		template <std::size_t Count>
		void fill_constant_curves(target::ParticleCurveDef (&result)[Count], utils::memory::allocator& allocator)
		{
			for (auto& curve : result)
			{
				curve = linear_curve(1.0f, 1.0f, 0.0f, allocator);
			}
		}

		target::ParticleModifier identity_modifier()
		{
			target::ParticleModifier result{};
			result.min = {1.0f, 1.0f, 1.0f, 0.0f};
			result.max = {1.0f, 1.0f, 1.0f, 0.0f};
			return result;
		}

		target::Particle_MaterialData stock_material_data()
		{
			return {
				.falloffParms = {4e8f, 2.66666667e-09f, 1.21749806f, -0.217452645f},
				.opaqueColor = {1.0f, 1.0f, 1.0f, 1.0f},
				.transparentColor = {0.5f, 0.5f, 0.5f, 0.5f},
				.translucentShadowParams = {0.0f, 0.0f},
				.beamParms = {0.00200000009f, 1.0f},
				.eyeOffsetParms = {0.0f, 0.0f, 0.0f, 0.0f},
				.alphaDissolveParms = {1.0f, 2.0f, 1.0f, 0.0f},
				.featherParms0 = {0.0250000004f, 40.0f, 40.0f, 0.0250000004f},
				.featherParms1 = {0.400000006f, 0.0f, 0.400000006f, 0.400000006f},
				.regionHighlightParms = {0.0f, 0.0f, 0.0f, 0.0f},
				.depthScanColor = {1.0f, 1.0f, 1.0f, 1.0f},
				.depthScanOutlineColor = {1.0f, 1.0f, 1.0f, 1.0f},
				.thermalParams = {-0.100000001f, 0.0f},
				.anisotropy = 0.0f,
				.featureEnabledFlags = 0,
			};
		}

		target::ParticleSystemDef* convert_child_system(ParticleSystemDef* asset, utils::memory::allocator& allocator);

		Material* find_decal_material(const ParticleMarkVisuals& decal)
		{
			for (const auto index : {1, 0, 2}) // world material first
			{
				if (decal.materials[index])
				{
					return decal.materials[index];
				}
			}

			return nullptr;
		}

		bool convert_linked_assets(const ParticleLinkedAssetListDef& list, const ParticleModuleType type,
			target::ParticleLinkedAssetListDef& result, utils::memory::allocator& allocator)
		{
			result.numAssets = list.assetList ? list.numAssets : 0;
			result.assetList = allocator.allocate_array<target::ParticleLinkedAssetDef>(result.numAssets);

			for (auto i = 0; i < result.numAssets; i++)
			{
				const auto& asset = list.assetList[i];
				auto& converted = result.assetList[i];

				switch (type)
				{
				case PARTICLE_MODULE_INIT_MATERIAL:
					if (asset.material)
					{
						converted.material = allocator.allocate<target::Material>();
						converted.material->name = asset.material->name;
					}
					break;
				case PARTICLE_MODULE_INIT_MODEL:
					if (asset.model)
					{
						converted.model = allocator.allocate<target::XModel>();
						converted.model->name = asset.model->name;
						xmodel::dump(asset.model);
					}
					break;
				case PARTICLE_MODULE_INIT_DECAL:
					if (const auto source = find_decal_material(asset.decal))
					{
						const auto decal = allocator.allocate<target::Material>();
						decal->name = allocator.duplicate_string(material::dump_decal(source));
						std::ranges::fill(converted.decal.materials, decal);
					}
					break;
				case PARTICLE_MODULE_INIT_LIGHT_OMNI:
					if (asset.lightDef)
					{
						converted.lightDef = allocator.allocate<target::GfxLightDef>();
						converted.lightDef->name = asset.lightDef->name;
						lightdef::dump(asset.lightDef);
					}
					break;
				case PARTICLE_MODULE_INIT_RUNNER:
				case PARTICLE_MODULE_TEST_DEATH:
				case PARTICLE_MODULE_TEST_IMPACT:
					converted.particleSystem = convert_child_system(asset.particleSystem, allocator);
					break;
				default:
					return false;
				}
			}

			return true;
		}

		float box_volume_cube_root(const ParticleModuleInitSpawnShapeBox& box)
		{
			const auto x = (box.m_dimensionsMax.v[0] - box.m_dimensionsMin.v[0]) * 0.5f;
			const auto y = (box.m_dimensionsMax.v[1] - box.m_dimensionsMin.v[1]) * 0.5f;
			const auto z = (box.m_dimensionsMax.v[2] - box.m_dimensionsMin.v[2]) * 0.5f;

			if (box.m_spawnFlags & PARTICLE_SPAWN_ONLY_ON_SURFACE)
			{
				return std::cbrt(2.0f * (x * y + y * z + z * x));
			}

			return std::cbrt(x * y * z);
		}

		float sphere_volume_cube_root(const ParticleModuleInitSpawnShapeSphere& sphere)
		{
			const auto pi = std::numbers::pi_v<float>;
			const auto inner = sphere.m_radius.min;
			const auto outer = sphere.m_radius.max;

			if (sphere.m_spawnFlags & PARTICLE_SPAWN_ONLY_ON_SURFACE)
			{
				return std::cbrt(4.0f * pi * outer * outer);
			}

			return std::cbrt(4.0f / 3.0f * pi * (outer * outer * outer - inner * inner * inner));
		}

		float cylinder_volume_cube_root(const ParticleModuleInitSpawnShapeCylinder& cylinder)
		{
			const auto pi = std::numbers::pi_v<float>;
			const auto inner = cylinder.m_radius.min;
			const auto outer = cylinder.m_radius.max;
			const auto height = 2.0f * cylinder.m_halfHeight;

			if (cylinder.m_spawnFlags & PARTICLE_SPAWN_ONLY_ON_SURFACE)
			{
				return std::cbrt(2.0f * pi * outer * height);
			}

			return std::cbrt(pi * (outer * outer - inner * inner) * height);
		}

		template <typename T, typename S>
		void convert_spawn_shape(const S& shape, T& result, const target::ParticleModuleType type)
		{
			result.m_axisFlags = shape.m_axisFlags;
			result.m_spawnFlags = shape.m_spawnFlags;
			result.m_normalAxis = static_cast<unsigned char>(shape.m_normalAxisIndex);
			result.m_spawnType = static_cast<unsigned char>(type - target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_FIRST);
			result.m_calculationOffset = convert_float4(shape.m_offset);
		}

		void convert_test(const ParticleModuleTest& test, const ParticleModuleType type, target::ParticleModuleTest& result,
			utils::memory::allocator& allocator)
		{
			result.m_moduleIndex = test.m_moduleIndex;
			result.m_useOrientationOptions = test.m_useOrientationOptions[0];
			result.m_useScaleOptions = test.m_useScaleOptions[0];
			result.m_useVelocityOptions = test.m_useVelocityOptions[0];
			result.m_scaleModifier = identity_modifier();
			result.m_velocityModifier = identity_modifier();
			result.m_eventHandlerData.m_nextState = test.m_eventHandlerData.m_nextState;
			result.m_eventHandlerData.m_kill = test.m_eventHandlerData.m_kill;
			convert_linked_assets(test.m_eventHandlerData.m_linkedAssetList, type, result.m_eventHandlerData.m_linkedAssetList, allocator);
		}

		bool is_enabled(const ParticleModuleDef& module)
		{
			return !(module.moduleData.moduleBase.m_flags & PARTICLE_MODULE_FLAG_DISABLED);
		}

		std::optional<target::ParticleModuleType> convert_module_data(const ParticleModuleDef& module, const bool lit,
			target::ParticleModuleTypeDef& result, utils::memory::allocator& allocator)
		{
			const auto& data = module.moduleData;

			switch (module.moduleType)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
			{
				const auto& atlas = data.initAtlas;
				result.initAtlas.m_startFrame = atlas.m_startFrame;
				result.initAtlas.m_loopCount = atlas.m_loopCount;
				result.initAtlas.m_randomIndex = atlas.m_randomIndex;
				result.initAtlas.m_playOverLife = atlas.m_playOverLife;

				for (auto& curve : result.initAtlas.m_curves)
				{
					curve = atlas.m_playOverLife
						? linear_curve(0.0f, 1.0f, 1.0f, allocator)
						: linear_curve(1.0f, 1.0f, static_cast<float>(atlas.m_playRate), allocator);
				}
				return target::PARTICLE_MODULE_INIT_ATLAS;
			}
			case PARTICLE_MODULE_INIT_ATTRIBUTES:
			{
				const auto& attributes = data.initAttributes;
				result.initAttributes.m_useNonUniformInterpolationForColor = attributes.m_useNonUniformInterpolationForColor;
				result.initAttributes.m_useNonUniformInterpolationForSize = attributes.m_useNonUniformInterpolationForSize;
				result.initAttributes.m_sizeMin = convert_float4(attributes.m_sizeMin);
				result.initAttributes.m_sizeMax = convert_float4(attributes.m_sizeMax);
				result.initAttributes.m_colorMin = convert_float4(attributes.m_colorMin);
				result.initAttributes.m_colorMax = convert_float4(attributes.m_colorMax);
				result.initAttributes.m_velocityMin = convert_float4(attributes.m_velocityMin);
				result.initAttributes.m_velocityMax = convert_float4(attributes.m_velocityMax);
				return target::PARTICLE_MODULE_INIT_ATTRIBUTES;
			}
			case PARTICLE_MODULE_INIT_CAMERA_OFFSET:
				return target::PARTICLE_MODULE_INIT_CAMERA_OFFSET;
			case PARTICLE_MODULE_INIT_FLARE:
			{
				const auto& flare = data.initFlare;
				result.initFlare.m_position = flare.m_position;
				result.initFlare.m_direction = convert_float4(flare.m_direction);
				result.initFlare.m_angularRotCount = flare.m_angularRotCount;
				result.initFlare.m_flareFlags = flare.m_flareFlags;
				result.initFlare.m_depthScaleRange = convert_range(flare.m_depthScaleRange);
				result.initFlare.m_depthScaleValue = convert_range(flare.m_depthScaleValue);
				result.initFlare.m_radialRot = convert_range(flare.m_radialRot);
				result.initFlare.m_radialScaleX = convert_range(flare.m_radialScaleX);
				result.initFlare.m_radialScaleY = convert_range(flare.m_radialScaleY);
				convert_curves(flare.m_curves, result.initFlare.m_curves, allocator);
				return target::PARTICLE_MODULE_INIT_FLARE;
			}
			case PARTICLE_MODULE_INIT_DECAL:
			{
				const auto& decal = data.initDecal;
				result.initDecal.m_stoppableFadeOutTime = 0xFFFF; // TODO: stock decal value
				result.initDecal.m_lerpColor = convert_float4(decal.m_lerpColor);
				result.initDecal.m_projectionAxis = static_cast<unsigned char>(decal.m_lerpWaitTime >> 8); // TODO: iw7 decal layout unproven, stock data has the axis in this byte
				convert_linked_assets(decal.m_linkedAssetList, module.moduleType, result.initDecal.m_linkedAssetList, allocator);
				return target::PARTICLE_MODULE_INIT_DECAL;
			}
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			{
				const auto& omni = data.initLightOmni;
				auto& converted = result.initLightOmni;
				converted.m_fovOuter = 0.785f; // TODO: stock iw8 omni value
				converted.m_bulbRadius = 1.0f; // TODO: stock iw8 omni value
				converted.m_bulbLength = 0.004f; // TODO: stock iw8 omni value
				converted.m_distanceFalloff = 1.0f; // TODO: stock iw8 omni value
				converted.m_brightness = 1.0f; // TODO: stock iw8 omni value
				converted.m_intensityIR = omni.m_intensityIR;
				converted.m_shadowSoftness = omni.m_shadowSoftness;
				converted.m_shadowBias = omni.m_shadowBias;
				converted.m_shadowArea = omni.m_shadowArea;
				converted.m_toneMappingScaleFactor = omni.m_tonemappingScaleFactor;
				converted.m_disableVolumetric = omni.m_disableVolumetric;
				converted.m_disableDynamicShadows = true; // TODO: stock iw8 omni value
				converted.m_scriptScale = true; // TODO: stock iw8 omni value
				convert_linked_assets(omni.m_linkedAssetList, module.moduleType, converted.m_linkedAssetList, allocator);
				return target::PARTICLE_MODULE_INIT_LIGHT_OMNI;
			}
			case PARTICLE_MODULE_INIT_MATERIAL:
			{
				result.initMaterial.m_renderOptions = target::PARTICLE_RENDER_OPTION_NONE;
				result.initMaterial.m_shaderGraphOptions = target::PARTICLE_SHADER_GRAPH_NONE;
				result.initMaterial.m_materialData = stock_material_data();
				convert_linked_assets(data.initMaterial.m_linkedAssetList, module.moduleType,
					result.initMaterial.m_linkedAssetList, allocator);
				for (auto i = 0; data.initMaterial.m_linkedAssetList.assetList && i < data.initMaterial.m_linkedAssetList.numAssets; i++)
				{
					material::dump_sprite(data.initMaterial.m_linkedAssetList.assetList[i].material, lit);
				}
				return target::PARTICLE_MODULE_INIT_MATERIAL;
			}
			case PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
			{
				const auto& mirror = data.initMirrorTexture;
				result.initMirrorTexture.m_mirrorHorizontal = static_cast<target::ParticleMirrorTextureType>(mirror.m_mirrorHorizontal);
				result.initMirrorTexture.m_mirrorVertical = static_cast<target::ParticleMirrorTextureType>(mirror.m_mirrorVertical);
				return target::PARTICLE_MODULE_INIT_MIRROR_TEXTURE;
			}
			case PARTICLE_MODULE_INIT_MODEL:
			{
				result.initModel.m_usePhysics = data.initModel.m_usePhysics;
				result.initModel.m_motionBlurHQ = data.initModel.m_motionBlurHQ;
				convert_linked_assets(data.initModel.m_linkedAssetList, module.moduleType,
					result.initModel.m_linkedAssetList, allocator);
				return target::PARTICLE_MODULE_INIT_MODEL;
			}
			case PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
			{
				const auto& query = data.initOcclusionQuery;
				result.initOcclusionQuery.m_fadeInTime = query.m_fadeInTime;
				result.initOcclusionQuery.m_fadeOutTime = query.m_fadeOutTime;
				result.initOcclusionQuery.m_scale = {query.m_worldSize[0], query.m_worldSize[1]}; // iw7 struct has m_scale and m_worldSize swapped
				result.initOcclusionQuery.m_worldSize[0] = query.m_scale.min;
				result.initOcclusionQuery.m_worldSize[1] = query.m_scale.max;
				return target::PARTICLE_MODULE_INIT_OCCLUSION_QUERY;
			}
			case PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
				result.initOrientedSprite.m_orientationQuat = convert_float4(data.initOrientedSprite.m_orientationQuat);
				return target::PARTICLE_MODULE_INIT_ORIENTED_SPRITE;
			case PARTICLE_MODULE_INIT_PLAYER_FACING:
				return target::PARTICLE_MODULE_INIT_PLAYER_FACING;
			case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
				result.initRelativeVelocity.m_velocityType = static_cast<target::ParticleRelativeVelocityType>(data.initRelativeVelocity.m_velocityType);
				result.initRelativeVelocity.m_useBoltInfo = data.initRelativeVelocity.m_useBoltInfo;
				return target::PARTICLE_MODULE_INIT_RELATIVE_VELOCITY;
			case PARTICLE_MODULE_INIT_ROTATION:
				result.initRotation.m_rotationAngle = convert_range(data.initRotation.m_rotationAngle);
				result.initRotation.m_rotationRate = convert_range(data.initRotation.m_rotationRate);
				return target::PARTICLE_MODULE_INIT_ROTATION;
			case PARTICLE_MODULE_INIT_ROTATION_3D:
			{
				const auto& rotation = data.initRotation3D;
				result.initRotation3D.m_rotationAngleMin = convert_float4(rotation.m_rotationAngleMin);
				result.initRotation3D.m_rotationAngleMax = convert_float4(rotation.m_rotationAngleMax);
				result.initRotation3D.m_rotationRateMin = convert_float4(rotation.m_rotationRateMin);
				result.initRotation3D.m_rotationRateMax = convert_float4(rotation.m_rotationRateMax);
				return target::PARTICLE_MODULE_INIT_ROTATION_3D;
			}
			case PARTICLE_MODULE_INIT_RUNNER:
			{
				const auto& runner = data.initRunner;
				result.initRunner.m_scaleModifier = identity_modifier();
				result.initRunner.m_velocityModifier = identity_modifier();
				result.initRunner.m_useOrientationOptions = runner.m_useOrientationOptions;
				result.initRunner.m_useScaleOptions = runner.m_useScaleOptions;
				result.initRunner.m_useVelocityOptions = runner.m_useVelocityOptions;
				result.initRunner.m_attachToParent = runner.m_attachToParent;
				result.initRunner.m_stopChildOnDeath = runner.m_stopChildOnDeath;
				result.initRunner.m_killChildOnDeath = runner.m_killChildOnDeath;
				result.initRunner.m_legacyOrientationVelocity = runner.m_legacyOrientationVelocity;
				result.initRunner.m_legacyOrientationRotation = runner.m_legacyOrientationRotation;
				convert_linked_assets(runner.m_linkedAssetList, module.moduleType, result.initRunner.m_linkedAssetList, allocator);
				return target::PARTICLE_MODULE_INIT_RUNNER;
			}
			case PARTICLE_MODULE_INIT_SPAWN:
				convert_curves(data.initSpawn.m_curves, result.initSpawn.m_curves, allocator);
				return target::PARTICLE_MODULE_INIT_SPAWN;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
			{
				const auto& box = data.initSpawnShapeBox;
				auto& converted = result.initSpawnShapeBox;
				convert_spawn_shape(box, converted, target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX);
				converted.m_volumeCubeRoot = box_volume_cube_root(box);
				converted.m_useBeamInfo = box.m_useBeamInfo;
				converted.m_dimensionsMin = convert_float4(box.m_dimensionsMin);
				converted.m_dimensionsMax = convert_float4(box.m_dimensionsMax);

				for (auto i = 0; i < 4; i++)
				{
					converted.m_offset.v[i] = (box.m_dimensionsMin.v[i] + box.m_dimensionsMax.v[i]) * 0.5f;
				}

				fill_constant_curves(converted.m_curves, allocator);
				return target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
			{
				const auto& cylinder = data.initSpawnShapeCylinder;
				auto& converted = result.initSpawnShapeCylinder;
				convert_spawn_shape(cylinder, converted, target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER);
				converted.m_volumeCubeRoot = cylinder_volume_cube_root(cylinder);
				converted.m_offset = convert_float4(cylinder.unk); // TODO: stock data only, iw7 field unnamed
				converted.m_hasRotation = cylinder.m_hasRotation;
				converted.m_rotateCalculatedOffset = cylinder.m_rotateCalculatedOffset;
				converted.m_halfHeight = cylinder.m_halfHeight;
				converted.m_radius = convert_range(cylinder.m_radius);
				converted.m_directionQuat = convert_float4(cylinder.m_directionQuat);
				fill_constant_curves(converted.m_curves, allocator);
				return target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
			{
				const auto& sphere = data.initSpawnShapeSphere;
				auto& converted = result.initSpawnShapeSphere;
				convert_spawn_shape(sphere, converted, target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE);
				converted.m_volumeCubeRoot = sphere_volume_cube_root(sphere);
				converted.m_offset = convert_float4(sphere.m_offset2);
				converted.m_radius = convert_range(sphere.m_radius);
				fill_constant_curves(converted.m_curves, allocator);
				return target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE;
			}
			case PARTICLE_MODULE_INIT_TAIL:
			{
				const auto& tail = data.initTail;
				result.initTail.m_averagePastVelocities = tail.m_averagePastVelocities;
				result.initTail.m_maxParentSpeed = tail.m_maxParentSpeed;
				result.initTail.m_tailLeading = tail.m_tailLeading;
				result.initTail.m_scaleWithVelocity = tail.m_scaleWithVelocity;
				result.initTail.m_rotateAroundPivot = tail.m_rotateAroundPivot;
				return target::PARTICLE_MODULE_INIT_TAIL;
			}
			case PARTICLE_MODULE_INIT_VOLUMETRIC:
			{
				const auto& volumetric = data.initVolumetric;
				result.initVolumetric.m_density = volumetric.m_density;
				result.initVolumetric.m_falloff = volumetric.m_falloff;
				result.initVolumetric.m_noiseAmplitude = volumetric.m_noiseAmplitude;
				result.initVolumetric.m_noiseMode = volumetric.m_noiseMode;
				return target::PARTICLE_MODULE_INIT_VOLUMETRIC;
			}
			case PARTICLE_MODULE_ATTRACTOR:
			{
				const auto& attractor = data.attractor;
				result.attractor.m_killWhenNear = attractor.m_killWhenNear;
				result.attractor.m_forceMag = attractor.m_forceMag;
				result.attractor.m_nearDistanceSq = attractor.m_nearDistanceSq;
				result.attractor.m_attractPoint = convert_float4(attractor.m_attractPoint);
				return target::PARTICLE_MODULE_ATTRACTOR;
			}
			case PARTICLE_MODULE_COLOR_GRAPH:
				result.colorGraph.firstCurve = is_enabled(module); // TODO: stock IW8 data only, IW7 has 0
				result.colorGraph.m_modulateColorByAlpha = data.colorGraph.m_modulateColorByAlpha;
				convert_curves(data.colorGraph.m_curves, result.colorGraph.m_curves, allocator);
				return target::PARTICLE_MODULE_COLOR_GRAPH;
			case PARTICLE_MODULE_COLOR_LERP:
				result.colorLerp.m_colorBegin = convert_float4(data.colorLerp.m_colorBegin);
				result.colorLerp.m_colorEnd = convert_float4(data.colorLerp.m_colorEnd);
				return target::PARTICLE_MODULE_COLOR_LERP;
			case PARTICLE_MODULE_EMISSION_GRAPH:
				convert_curves(data.emissionGraph.m_curves, result.emissionGraph.m_curves, allocator);
				return target::PARTICLE_MODULE_EMISSION_GRAPH;
			case PARTICLE_MODULE_FORCE:
				result.force.m_forceMin = convert_float4(data.force.m_forceMin);
				result.force.m_forceMax = convert_float4(data.force.m_forceMax);
				return target::PARTICLE_MODULE_FORCE;
			case PARTICLE_MODULE_GRAVITY:
				result.gravity.m_gravityPercentage = convert_range(data.gravity.m_gravityPercentage);
				return target::PARTICLE_MODULE_GRAVITY;
			case PARTICLE_MODULE_INTENSITY_GRAPH:
				result.intensityGraph.firstCurve = is_enabled(module); // TODO: stock IW8 data only, IW7 has 0
				convert_curves(data.intensityGraph.m_curves, result.intensityGraph.m_curves, allocator);
				return target::PARTICLE_MODULE_INTENSITY_GRAPH;
			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
				convert_curves(data.parentVelocityGraph.m_curves, result.parentVelocityGraph.m_curves, allocator);
				return target::PARTICLE_MODULE_PARENT_VELOCITY_GRAPH;
			case PARTICLE_MODULE_PHYSICS_RAY_CAST:
			{
				const auto& ray_cast = data.physicsRayCast;
				auto& converted = result.physicsRayCast;
				converted.m_bounce = convert_range(ray_cast.m_bounce);
				std::memcpy(converted.m_bounds.midPoint, ray_cast.m_bounds.midPoint, sizeof(converted.m_bounds.midPoint));
				std::memcpy(converted.m_bounds.halfSize, ray_cast.m_bounds.halfSize, sizeof(converted.m_bounds.halfSize));
				converted.m_useItemClip = ray_cast.m_useItemClip;
				converted.m_useSurfaceType = ray_cast.m_useSurfaceType;
				converted.m_collideWithWater = ray_cast.m_collideWithWater;
				converted.m_ignoreContentItem = ray_cast.m_ignoreContentItem;
				return target::PARTICLE_MODULE_PHYSICS_RAY_CAST;
			}
			case PARTICLE_MODULE_POSITION_GRAPH:
				convert_curves(data.positionGraph.m_curves, result.positionGraph.m_curves, allocator);
				return target::PARTICLE_MODULE_POSITION_GRAPH;
			case PARTICLE_MODULE_ROTATION_GRAPH:
				result.rotationGraph.m_useRotationRate = data.rotationGraph.m_useRotationRate;
				convert_curves(data.rotationGraph.m_curves, result.rotationGraph.m_curves, allocator);
				return target::PARTICLE_MODULE_ROTATION_GRAPH;
			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
				result.rotationGraph3D.m_useRotationRate = data.rotationGraph3D.m_useRotationRate;
				convert_curves(data.rotationGraph3D.m_curves, result.rotationGraph3D.m_curves, allocator);
				return target::PARTICLE_MODULE_ROTATION_GRAPH_3D;
			case PARTICLE_MODULE_SIZE_GRAPH:
				result.sizeGraph.firstCurve = is_enabled(module); // TODO: stock IW8 data only, IW7 has 0
				convert_curves(data.sizeGraph.m_curves, result.sizeGraph.m_curves, allocator);
				result.sizeGraph.m_sizeBegin = convert_float4(data.sizeGraph.m_sizeBegin);
				result.sizeGraph.m_sizeEnd = convert_float4(data.sizeGraph.m_sizeEnd);
				return target::PARTICLE_MODULE_SIZE_GRAPH;
			case PARTICLE_MODULE_SIZE_LERP:
				result.sizeLerp.m_sizeBegin = convert_float4(data.sizeLerp.m_sizeBegin);
				result.sizeLerp.m_sizeEnd = convert_float4(data.sizeLerp.m_sizeEnd);
				return target::PARTICLE_MODULE_SIZE_LERP;
			case PARTICLE_MODULE_VELOCITY_GRAPH:
				convert_curves(data.velocityGraph.m_curves, result.velocityGraph.m_curves, allocator);
				result.velocityGraph.m_velocityBegin = convert_float4(data.velocityGraph.m_velocityBegin);
				result.velocityGraph.m_velocityEnd = convert_float4(data.velocityGraph.m_velocityEnd);
				return target::PARTICLE_MODULE_VELOCITY_GRAPH;
			case PARTICLE_MODULE_TEST_DEATH:
				convert_test(data.testDeath, module.moduleType, result.testDeath, allocator);
				return target::PARTICLE_MODULE_TEST_DEATH;
			case PARTICLE_MODULE_TEST_IMPACT:
				convert_test(data.testImpact, module.moduleType, result.testImpact, allocator);
				return target::PARTICLE_MODULE_TEST_IMPACT;
			default:
				return {};
			}
		}

		void convert_group(const ParticleModuleGroupDef& group, const bool lit, target::ParticleModuleGroupDef& result,
			dropped_modules& dropped, utils::memory::allocator& allocator)
		{
			result.disabled = group.disabled;
			result.moduleDefs = allocator.allocate_array<target::ParticleModuleDef>(group.moduleDefs ? group.numModules : 0);

			for (auto i = 0; group.moduleDefs && i < group.numModules; i++)
			{
				const auto& module = group.moduleDefs[i];
				auto& converted = result.moduleDefs[result.numModules];
				const auto type = convert_module_data(module, lit, converted.moduleData, allocator);
				if (!type)
				{
					dropped.insert(module.moduleType);
					continue;
				}

				converted.moduleType = *type;
				converted.moduleData.moduleBase.m_type[0] = static_cast<unsigned char>(*type);
				converted.moduleData.moduleBase.m_flags = module.moduleData.moduleBase.m_flags;
				result.numModules++;
			}
		}

		template <typename Predicate>
		bool has_module(const target::ParticleStateDef& state, Predicate&& predicate)
		{
			for (auto i = 0; i < 3; i++)
			{
				const auto& group = state.moduleGroupDefs[i];
				for (auto j = 0; j < group.numModules; j++)
				{
					if (predicate(group.moduleDefs[j].moduleType))
					{
						return true;
					}
				}
			}

			return false;
		}

		bool is_spawn_shape(const target::ParticleModuleType type)
		{
			return type >= target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_FIRST && type <= target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_LAST;
		}

		bool is_cloud_state(const ParticleStateDef& state)
		{
			return state.elementType == PARTICLE_ELEMENT_TYPE_CLOUD || state.elementType == PARTICLE_ELEMENT_TYPE_SPARK_CLOUD;
		}

		unsigned int convert_element_type(const unsigned int type)
		{
			return type < PARTICLE_ELEMENT_TYPE_CLOUD ? type : type - 2;
		}

		unsigned long long convert_state_flags(const unsigned long long flags)
		{
			return (flags & 0x3FF) | ((flags & ~0xFFFull) >> 2);
		}

		void convert_state(const ParticleStateDef& state, target::ParticleStateDef& result, dropped_modules& dropped,
			utils::memory::allocator& allocator)
		{
			result.elementType = convert_element_type(state.elementType);
			result.flags = convert_state_flags(state.flags);
			result.moduleGroupDefs = allocator.allocate_array<target::ParticleModuleGroupDef>(3);

			for (auto i = 0; state.moduleGroupDefs && i < 3; i++)
			{
				convert_group(state.moduleGroupDefs[i], state.flags & PARTICLE_STATE_DEF_FLAG_GPU_LIGHTING, result.moduleGroupDefs[i], dropped, allocator);
			}

			if (has_module(result, [](const auto type) { return type == target::PARTICLE_MODULE_SIZE_GRAPH; }))
			{
				result.flags |= target::PARTICLE_STATE_DEF_FLAG_HAS_SIZE_CURVE;
			}

			if (!has_module(result, is_spawn_shape))
			{
				result.flags &= ~static_cast<unsigned long long>(target::PARTICLE_STATE_DEF_FLAG_HAS_SPAWN_SHAPE);
			}

			if (!has_module(result, [](const auto type) { return type == target::PARTICLE_MODULE_INIT_SOUND; })) // TODO: convert the IW7 sound module
			{
				result.flags &= ~static_cast<unsigned long long>(target::PARTICLE_STATE_DEF_FLAG_PLAY_SOUNDS);
			}
		}

		bool uses_emissive_streams(const target::ParticleEmitterDef& emitter) // sub_1BA3AB0 (0x1BA4271)
		{
			for (auto i = 0; i < emitter.numStates; i++)
			{
				if (emitter.stateDefs[i].flags & (target::PARTICLE_STATE_DEF_FLAG_HAS_EMISSIVE | target::PARTICLE_STATE_DEF_FLAG_HAS_INTENSITY
					| target::PARTICLE_STATE_DEF_FLAG_HAS_LIGHTING_FRACTION))
				{
					return true;
				}
			}

			return false;
		}

		int data_flags(const target::ParticleEmitterDef& emitter)
		{
			auto flags = static_cast<int>(target::USE_BASE);
			const auto any_state = [&](auto&& predicate)
			{
				for (auto i = 0; i < emitter.numStates; i++)
				{
					if (has_module(emitter.stateDefs[i], predicate))
					{
						return true;
					}
				}
				return false;
			};

			if (any_state([](const auto type)
				{
					return type == target::PARTICLE_MODULE_INTENSITY_GRAPH || type == target::PARTICLE_MODULE_EMISSIVE_GRAPH
						|| type == target::PARTICLE_MODULE_EMISSION_GRAPH || type == target::PARTICLE_MODULE_INIT_FLARE
						|| type == target::PARTICLE_MODULE_INIT_MODEL || type == target::PARTICLE_MODULE_INIT_PLAYER_FACING;
				}) || uses_emissive_streams(emitter))
			{
				flags |= target::USE_EMISSIVE_CURVES;
			}

			if (any_state([](const auto type) { return type == target::PARTICLE_MODULE_PARENT_VELOCITY_GRAPH; }))
			{
				flags |= target::USE_PARENT_VELOCITY | target::USE_INHERIT_PERCENTAGE;
			}

			if (any_state([](const auto type) { return type == target::PARTICLE_MODULE_INIT_RUNNER; }))
			{
				flags |= target::USE_CHILD_SYSTEM;
			}

			if (any_state([](const auto type) { return type >= target::PARTICLE_MODULE_TEST_BEGIN && type <= target::PARTICLE_MODULE_TEST_END; }))
			{
				flags |= target::USE_MODULE_TESTS;
			}

			if (any_state([](const auto type)
				{
					return type == target::PARTICLE_MODULE_FORCE || type == target::PARTICLE_MODULE_FORCE_DRAG_GRAPH
						|| type == target::PARTICLE_MODULE_PHYSICS_RAY_CAST || type == target::PARTICLE_MODULE_PHYSICS_LIGHT
						|| type == target::PARTICLE_MODULE_GRAVITY;
				}))
			{
				flags |= target::USE_RAY_CAST_HANDLE;
			}

			if (any_state([](const auto type) { return type == target::PARTICLE_MODULE_INIT_MODEL; }))
			{
				flags |= target::USE_PREV_PLACEMENT;
			}

			return flags;
		}

		void convert_emitter(const ParticleEmitterDef& emitter, target::ParticleEmitterDef& result, dropped_modules& dropped,
			utils::memory::allocator& allocator)
		{
			result.numStates = emitter.stateDefs ? emitter.numStates : 0;
			result.stateDefs = allocator.allocate_array<target::ParticleStateDef>(result.numStates);

			for (auto i = 0; i < result.numStates; i++)
			{
				convert_state(emitter.stateDefs[i], result.stateDefs[i], dropped, allocator);
			}

			result.particleSpawnRate = convert_range(emitter.particleSpawnRate);
			result.particleLife = convert_range(emitter.particleLife);
			result.particleDelay = convert_range(emitter.particleDelay);
			result.particleCountMax = emitter.particleCountMax;
			result.particleBurstCount = {emitter.particleBurstCount.min, emitter.particleBurstCount.max};
			result.emitterLife = convert_range(emitter.emitterLife);
			result.emitterDelay = convert_range(emitter.emitterDelay);
			result.randomSeed = emitter.randomSeed;
			result.spawnRangeSq = convert_range(emitter.spawnRangeSq);
			result.fadeOutMaxDistance = emitter.fadeOutMaxDistance;
			result.fadeCurveDef = convert_curve(emitter.fadeCurveDef, allocator);
			result.spawnFrustumCullRadius = emitter.spawnFrustumCullRadius;
			result.flags = emitter.flags;
			result.gravityOptions = static_cast<target::ParticleGravityOptions>(emitter.gravityOptions);

			for (auto i = 0; i < 4; i++)
			{
				result.groupIDs[i] = emitter.groupIDs[i];
			}

			result.emitByDistanceDensity = {0.1f, 0.1f};
			result.m_dataFlags = static_cast<target::ParticleDataFlags>(data_flags(result));
			result.particleSpawnShapeRange = convert_range(emitter.particleSpawnShapeRange);
		}

		bool has_cloud_state(const ParticleEmitterDef& emitter)
		{
			for (auto i = 0; emitter.stateDefs && i < emitter.numStates; i++)
			{
				if (is_cloud_state(emitter.stateDefs[i]))
				{
					return true;
				}
			}

			return false;
		}

		bool has_unconverted_element(const ParticleEmitterDef& emitter) // TODO: convert the beam, geo trail and spot light modules
		{
			for (auto i = 0; emitter.stateDefs && i < emitter.numStates; i++)
			{
				const auto type = emitter.stateDefs[i].elementType;
				if (type == PARTICLE_ELEMENT_TYPE_BEAM || type == PARTICLE_ELEMENT_TYPE_GEO_TRAIL || type == PARTICLE_ELEMENT_TYPE_LIGHT_SPOT)
				{
					return true;
				}
			}

			return false;
		}

		target::ParticleSystemDef* convert(const ParticleSystemDef* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::ParticleSystemDef>();
			result->name = asset->name;
			result->version = asset->version;
			result->flags = asset->flags;
			result->phaseOptions = static_cast<target::ParticlePhaseOptions>(asset->phaseOptions);
			result->drawFrustumCullRadius = asset->drawFrustumCullRadius;
			result->updateFrustumCullRadius = asset->updateFrustumCullRadius;
			result->sunDistance = asset->sunDistance;
			result->preRollMSec = asset->preRollMSec;
			result->editorPosition = convert_float4(asset->editorPosition);
			result->editorRotation = convert_float4(asset->editorRotation);
			result->gameTweakPosition = convert_float4(asset->gameTweakPosition);
			result->gameTweakRotation = convert_float4(asset->gameTweakRotation);

			const auto num_emitters = asset->emitterDefs ? asset->numEmitters : 0;
			std::vector<int> emitter_index(num_emitters, -1);
			dropped_modules dropped;

			result->emitterDefs = allocator.allocate_array<target::ParticleEmitterDef>(num_emitters);
			for (auto i = 0; i < num_emitters; i++)
			{
				if (has_cloud_state(asset->emitterDefs[i]))
				{
					ZONETOOL_WARNING("VFX \"%s\": emitter %d is a cloud emitter, IW8 has no cloud element, dropped", asset->name, i);
					continue;
				}

				if (has_unconverted_element(asset->emitterDefs[i]))
				{
					ZONETOOL_WARNING("VFX \"%s\": emitter %d has an element without an IW8 conversion, dropped", asset->name, i);
					continue;
				}

				emitter_index[i] = result->numEmitters;
				convert_emitter(asset->emitterDefs[i], result->emitterDefs[result->numEmitters++], dropped, allocator);
			}

			for (const auto type : dropped)
			{
				ZONETOOL_WARNING("VFX \"%s\": IW7 module type 0x%X has no IW8 conversion, dropped", asset->name, type);
			}

			const auto remap_emitter = [&](const int index)
			{
				return index >= 0 && index < num_emitters ? emitter_index[index] : index;
			};

			result->occlusionOverrideEmitterIndex = remap_emitter(asset->occlusionOverrideEmitterIndex);

			const auto num_nodes = asset->scriptedInputNodeDefs ? asset->numScriptedInputNodes : 0;
			result->scriptedInputNodeDefs = allocator.allocate_array<target::ParticleScriptedInputNodeDef>(num_nodes);
			for (auto i = 0; i < num_nodes; i++)
			{
				const auto& node = asset->scriptedInputNodeDefs[i];
				const auto emitter = remap_emitter(static_cast<int>(node.nodeBase.m_emitterIndex));
				if (emitter < 0)
				{
					continue;
				}

				auto& converted = result->scriptedInputNodeDefs[result->numScriptedInputNodes++];
				converted.nodeBase.m_type = static_cast<target::ParticleScriptedInputNodeType>(node.nodeBase.m_type);
				converted.nodeBase.m_flags = node.nodeBase.m_flags;
				converted.nodeBase.m_emitterIndex = emitter;

				if (node.nodeBase.m_type == PARTICLE_SCRIPTED_INPUT_NODE_KVP)
				{
					converted.KVP.m_inputRecord.key = static_cast<target::ParticleScriptedInputKey>(node.KVP.m_inputRecord.key);
					converted.KVP.m_inputRecord.dirty = node.KVP.m_inputRecord.dirty;
					converted.KVP.m_inputRecord.value = node.KVP.m_inputRecord.value;
				}
			}

			return result;
		}

		ParticleSystemDef* find_loaded(ParticleSystemDef* asset)
		{
			if (!asset->emitterDefs)
			{
				if (const auto loaded = static_cast<ParticleSystemDef*>(db_find_asset(ASSET_TYPE_VFX, asset->name)))
				{
					return loaded;
				}
			}

			return asset;
		}

		target::ParticleSystemDef* convert_child_system(ParticleSystemDef* asset, utils::memory::allocator& allocator)
		{
			if (!asset)
			{
				return nullptr;
			}

			const auto result = allocator.allocate<target::ParticleSystemDef>();
			result->name = asset->name;
			dump(asset);
			return result;
		}
	}

	void dump(ParticleSystemDef* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_VFX, asset->name))
		{
			return;
		}

		asset = find_loaded(asset);

		utils::memory::allocator allocator;
		zonetool::iw8::vfx::dump(convert(asset, allocator));
		add_csv_row("vfx", asset->name);
	}
}
