#include <std_include.hpp>

#include "../include.hpp"
#include "vfx.hpp"
#include "material.hpp"
#include "physics.hpp"

#include <iw8/assets/vfx.hpp>

namespace zonetool::s4::converter::iw8::vfx
{
	namespace
	{
		namespace source = zonetool::s4;
		namespace target = zonetool::iw8;

		template <typename Target, typename Source>
		void copy_same(Target& result, const Source& value)
		{
			static_assert(sizeof(Target) == sizeof(Source));
			std::memcpy(&result, &value, sizeof(Target));
		}

		target::ParticleSystemDef* reference(const source::ParticleSystemDef* asset, utils::memory::allocator& allocator)
		{
			if (!asset)
			{
				return nullptr;
			}

			if (const auto loaded = static_cast<source::ParticleSystemDef*>(db_find_asset(source::ASSET_TYPE_VFX, asset->name)))
			{
				dump(loaded);
			}

			return name_reference<target::ParticleSystemDef>(asset, allocator);
		}

		target::PhysicsFXShape* reference(const source::PhysicsFXShape* asset, utils::memory::allocator& allocator)
		{
			if (!asset)
			{
				return nullptr;
			}

			if (const auto loaded = static_cast<source::PhysicsFXShape*>(db_find_asset(source::ASSET_TYPE_PHYSICS_FX_SHAPE, asset->name)))
			{
				physics::dump(loaded);
			}

			return name_reference<target::PhysicsFXShape>(asset, allocator);
		}

		void convert_curve(const source::ParticleCurveDef& curve, target::ParticleCurveDef& result, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(target::ParticleCurveControlPointDef) == sizeof(source::ParticleCurveControlPointDef));

			result.numControlPoints = curve.numControlPoints;
			result.scale = curve.scale;
			if (curve.controlPoints && curve.numControlPoints > 0)
			{
				result.controlPoints = allocator.allocate_array<target::ParticleCurveControlPointDef>(curve.numControlPoints);
				std::memcpy(result.controlPoints, curve.controlPoints, sizeof(target::ParticleCurveControlPointDef) * curve.numControlPoints);
			}
		}

		template <std::size_t Count>
		void convert_curves(const source::ParticleCurveDef (&curves)[Count], target::ParticleCurveDef (&result)[Count], utils::memory::allocator& allocator)
		{
			for (auto i = 0u; i < Count; i++)
			{
				convert_curve(curves[i], result[i], allocator);
			}
		}

		void convert_linked_asset(const source::ParticleLinkedAssetDef& asset, target::ParticleLinkedAssetDef& result,
			const target::ParticleModuleType type, utils::memory::allocator& allocator)
		{
			switch (type)
			{
			case target::PARTICLE_MODULE_INIT_MATERIAL:
				result.material = material::reference(asset.material, allocator);
				break;
			case target::PARTICLE_MODULE_INIT_MODEL:
				result.model = name_reference<target::XModel>(asset.model, allocator);
				break;
			case target::PARTICLE_MODULE_PHYSICS_LIGHT:
				result.physicsFXData.physicsFXPipeline = name_reference<target::PhysicsFXPipeline>(asset.physicsFXData.physicsFXPipeline, allocator);
				result.physicsFXData.physicsFXShape = reference(asset.physicsFXData.physicsFXShape, allocator);
				break;
			case target::PARTICLE_MODULE_INIT_DECAL:
				for (auto i = 0u; i < std::size(asset.decal.materials); i++)
				{
					result.decal.materials[i] = material::reference(asset.decal.materials[i], allocator);
				}
				break;
			case target::PARTICLE_MODULE_INIT_SOUND:
				result.sound = asset.sound ? allocator.duplicate_string(asset.sound) : nullptr;
				break;
			case target::PARTICLE_MODULE_INIT_VECTOR_FIELD:
				result.vectorField = name_reference<target::VectorField>(asset.vectorField, allocator);
				break;
			case target::PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case target::PARTICLE_MODULE_INIT_LIGHT_SPOT:
				result.lightDef = name_reference<target::GfxLightDef>(asset.lightDef, allocator);
				break;
			default:
				result.particleSystem = reference(asset.particleSystem, allocator);
				break;
			}
		}

		void convert_linked_assets(const source::ParticleLinkedAssetListDef& list, target::ParticleLinkedAssetListDef& result,
			const target::ParticleModuleType type, utils::memory::allocator& allocator)
		{
			if (!list.assetList || list.numAssets <= 0)
			{
				return;
			}

			result.numAssets = list.numAssets;
			result.assetList = allocator.allocate_array<target::ParticleLinkedAssetDef>(list.numAssets);
			for (auto i = 0; i < list.numAssets; i++)
			{
				convert_linked_asset(list.assetList[i], result.assetList[i], type, allocator);
			}
		}

		void convert_test(const source::ParticleModuleTest& source, target::ParticleModuleTest& module, utils::memory::allocator& allocator)
		{
			module.m_moduleIndex = source.m_moduleIndex;
			module.m_useOrientationOptions = source.m_useOrientationOptions;
			module.m_useScaleOptions = source.m_useScaleOptions;
			module.m_useVelocityOptions = source.m_useVelocityOptions;
			copy_same(module.m_scaleModifier, source.m_scaleModifier);
			copy_same(module.m_velocityModifier, source.m_velocityModifier);
			module.m_eventHandlerData.m_nextState = source.m_eventHandlerData.m_nextState;
			module.m_eventHandlerData.m_kill = source.m_eventHandlerData.m_kill;
			convert_linked_assets(source.m_eventHandlerData.m_linkedAssetList, module.m_eventHandlerData.m_linkedAssetList,
				target::PARTICLE_MODULE_TEST_AGE, allocator);
		}

		float drag_scale(const float scale)
		{
			return 60.0f - std::sqrt(3600.0f - 60.0f * scale); // s4 scale = 2x - x*x/60
		}

		void convert_module(const source::ParticleModuleInitAtlas& source, target::ParticleModuleInitAtlas& module, utils::memory::allocator& allocator)
		{
			module.m_startFrame = source.m_startFrame;
			module.m_loopCount = source.m_loopCount;
			module.m_randomIndex = source.m_randomIndex;
			module.m_playOverLife = source.m_playOverLife;
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleInitAttributes& source, target::ParticleModuleInitAttributes& module, utils::memory::allocator&)
		{
			module.m_useNonUniformInterpolationForColor = source.m_useNonUniformInterpolationForColor;
			module.m_useNonUniformInterpolationForSize = source.m_useNonUniformInterpolationForSize;
			copy_same(module.m_sizeMin, source.m_sizeMin);
			copy_same(module.m_sizeMax, source.m_sizeMax);
			copy_same(module.m_colorMin, source.m_colorMin);
			copy_same(module.m_colorMax, source.m_colorMax);
			copy_same(module.m_velocityMin, source.m_velocityMin);
			copy_same(module.m_velocityMax, source.m_velocityMax);
		}

		void convert_module(const source::ParticleModuleInitBeam& source, target::ParticleModuleInitBeam& module, utils::memory::allocator& allocator)
		{
			module.m_beamFlags = static_cast<target::ParticleModuleInitBeamFlags>(source.m_beamFlags);
			module.m_splitDistance = source.m_splitDistance;
			copy_same(module.m_endPos, source.m_endPos);
			copy_same(module.m_curvePoint1, source.m_curvePoint1);
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_BEAM, allocator);
			module.m_scrollRateUV = source.m_scrollRateUV;
		}

		void convert_module(const source::ParticleModuleInitCameraOffset&, target::ParticleModuleInitCameraOffset&, utils::memory::allocator&)
		{
		}

		void convert_module(const source::ParticleModuleInitDecal& source, target::ParticleModuleInitDecal& module, utils::memory::allocator& allocator)
		{
			module.m_fadeInTime = source.m_fadeInTime;
			module.m_fadeOutTime = source.m_fadeOutTime;
			module.m_stoppableFadeOutTime = source.m_stoppableFadeOutTime;
			module.m_lerpWaitTime = source.m_lerpWaitTime;
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_DECAL, allocator);
			copy_same(module.m_lerpColor, source.m_lerpColor);
			module.m_lerpTime = source.m_lerpTime;
			module.m_dynamicDecal = source.m_dynamicDecal;
			module.m_projectionAxis = source.m_projectionAxis;
			module.m_bypassStackingLimiter = source.m_bypassStackingLimiter;
		}

		void convert_module(const source::ParticleModuleInitFlare& source, target::ParticleModuleInitFlare& module, utils::memory::allocator& allocator)
		{
			module.m_position = source.m_position;
			copy_same(module.m_direction, source.m_direction);
			module.m_flareFlags = source.m_flareFlags;
			copy_same(module.m_depthScaleRange, source.m_depthScaleRange);
			copy_same(module.m_depthScaleValue, source.m_depthScaleValue);
			copy_same(module.m_radialRot, source.m_radialRot);
			copy_same(module.m_radialScaleX, source.m_radialScaleX);
			copy_same(module.m_radialScaleY, source.m_radialScaleY);
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleInitGeoTrail& source, target::ParticleModuleInitGeoTrail& module, utils::memory::allocator&)
		{
			module.m_numPointsMax = source.m_numPointsMax;
			module.m_splitDistance = source.m_splitDistance;
			module.m_splitAngle = source.m_splitAngle;
			module.m_centerOffset = source.m_centerOffset;
			module.m_numSheets = source.m_numSheets;
			module.m_fadeInDistance = source.m_fadeInDistance;
			module.m_fadeOutDistance = source.m_fadeOutDistance;
			module.m_tileDistance = source.m_tileDistance;
			copy_same(module.m_tileOffset, source.m_tileOffset);
			module.m_scrollTime = source.m_scrollTime;
			module.m_useLocalVelocity = source.m_useLocalVelocity;
			module.m_useVerticalTexture = source.m_useVerticalTexture;
			module.m_cameraFacing = source.m_cameraFacing;
			module.m_fixLeadingEdge = source.m_fixLeadingEdge;
			module.m_clampUVs = source.m_clampUVs;
		}

		void convert_module(const source::ParticleModuleInitKillWrapBox& source, target::ParticleModuleInitKillWrapBox& module, utils::memory::allocator&)
		{
			copy_same(module.m_bounds, source.m_bounds);
			copy_same(module.m_offset, source.m_offset);
		}

		void convert_module(const source::ParticleModuleInitLightOmni& source, target::ParticleModuleInitLightOmni& module, utils::memory::allocator& allocator)
		{
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_LIGHT_OMNI, allocator);
			module.m_fovOuter = source.m_fovOuter;
			module.m_fovInner = source.m_fovInner;
			module.m_bulbRadius = source.m_bulbRadius;
			module.m_bulbLength = source.m_bulbLength;
			module.m_distanceFalloff = source.m_distanceFalloff;
			module.m_brightness = source.m_brightness;
			module.m_intensityUV = source.m_intensityUV;
			module.m_intensityIR = source.m_intensityIR;
			module.m_intensityHeat = source.m_intensityHeat;
			module.m_shadowBias = source.m_shadowBias;
			module.m_toneMappingScaleFactor = source.m_toneMappingScaleFactor;
			module.m_disableVolumetric = source.m_disableVolumetric;
			module.m_disableShadowMap = source.m_disableShadowMap;
			module.m_disableDynamicShadows = source.m_disableDynamicShadows;
			module.m_scriptScale = source.m_scriptScale;
		}

		void convert_module(const source::ParticleModuleInitLightSpot& source, target::ParticleModuleInitLightSpot& module, utils::memory::allocator& allocator)
		{
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_LIGHT_SPOT, allocator);
			module.m_fovOuter = source.m_fovOuter;
			module.m_fovInner = source.m_fovInner;
			module.m_bulbRadius = source.m_bulbRadius;
			module.m_bulbLength = source.m_bulbLength;
			module.m_distanceFalloff = source.m_distanceFalloff;
			module.m_fovCollimation = source.m_fovCollimation;
			module.m_brightness = source.m_brightness;
			module.m_intensityUV = source.m_intensityUV;
			module.m_intensityIR = source.m_intensityIR;
			module.m_shadowBias = source.m_shadowBias;
			module.m_shadowNearPlane = source.m_shadowNearPlane;
			module.m_toneMappingScaleFactor = source.m_toneMappingScaleFactor;
			module.m_disableVolumetric = source.m_disableVolumetric;
			module.m_disableShadowMap = source.m_disableShadowMap;
			module.m_disableDynamicShadows = source.m_disableDynamicShadows;
			module.m_scriptScale = source.m_scriptScale;
		}

		void convert_module(const source::ParticleModuleInitMaterial& source, target::ParticleModuleInitMaterial& module, utils::memory::allocator& allocator)
		{
			module.m_renderOptions = static_cast<target::ParticleRenderOptions>(source.m_renderOptions);
			module.m_shaderGraphOptions = static_cast<target::ParticleShaderGraphOptions>(source.m_shaderGraphOptions);
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_MATERIAL, allocator);
			copy_same(module.m_materialData, source.m_materialData);
			module.m_materialData.featureEnabledFlags &= ~0xE000u; // s4 only bits
		}

		void convert_module(const source::ParticleModuleInitMirrorTexture& source, target::ParticleModuleInitMirrorTexture& module, utils::memory::allocator&)
		{
			module.m_mirrorHorizontal = static_cast<target::ParticleMirrorTextureType>(source.m_mirrorHorizontal);
			module.m_mirrorVertical = static_cast<target::ParticleMirrorTextureType>(source.m_mirrorVertical);
		}

		void convert_module(const source::ParticleModuleInitModel& source, target::ParticleModuleInitModel& module, utils::memory::allocator& allocator)
		{
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_MODEL, allocator);
		}

		void convert_module(const source::ParticleModuleInitOcclusionQuery& source, target::ParticleModuleInitOcclusionQuery& module, utils::memory::allocator&)
		{
			module.m_depthBias = source.m_depthBias;
			module.m_fadeInTime = source.m_fadeInTime;
			module.m_fadeOutTime = source.m_fadeOutTime;
			copy_same(module.m_scale, source.m_scale);
			copy_same(module.m_worldSize, source.m_worldSize);
		}

		void convert_module(const source::ParticleModuleInitOrientedSprite& source, target::ParticleModuleInitOrientedSprite& module, utils::memory::allocator&)
		{
			copy_same(module.m_orientationQuat, source.m_orientationQuat);
		}

		void convert_module(const source::ParticleModuleInitRelativeVelocity& source, target::ParticleModuleInitRelativeVelocity& module, utils::memory::allocator&)
		{
			module.m_velocityType = static_cast<target::ParticleRelativeVelocityType>(source.m_velocityType);
			module.m_useBoltInfo = source.m_useBoltInfo;
		}

		void convert_module(const source::ParticleModuleInitRotation& source, target::ParticleModuleInitRotation& module, utils::memory::allocator&)
		{
			copy_same(module.m_rotationAngle, source.m_rotationAngle);
			copy_same(module.m_rotationRate, source.m_rotationRate);
		}

		void convert_module(const source::ParticleModuleInitRotation3D& source, target::ParticleModuleInitRotation3D& module, utils::memory::allocator&)
		{
			copy_same(module.m_rotationAngleMin, source.m_rotationAngleMin);
			copy_same(module.m_rotationAngleMax, source.m_rotationAngleMax);
			copy_same(module.m_rotationRateMin, source.m_rotationRateMin);
			copy_same(module.m_rotationRateMax, source.m_rotationRateMax);
		}

		void convert_module(const source::ParticleModuleInitRunner& source, target::ParticleModuleInitRunner& module, utils::memory::allocator& allocator)
		{
			copy_same(module.m_scaleModifier, source.m_scaleModifier);
			copy_same(module.m_velocityModifier, source.m_velocityModifier);
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_RUNNER, allocator);
			module.m_useOrientationOptions = source.m_useOrientationOptions;
			module.m_useScaleOptions = source.m_useScaleOptions;
			module.m_useVelocityOptions = source.m_useVelocityOptions;
			module.m_attachToParent = source.m_attachToParent;
			module.m_stopChildOnDeath = source.m_stopChildOnDeath;
			module.m_killChildOnDeath = source.m_killChildOnDeath;
			module.m_legacyOrientationVelocity = source.m_legacyOrientationVelocity;
			module.m_legacyOrientationRotation = source.m_legacyOrientationRotation;
		}

		void convert_module(const source::ParticleModuleInitSound& source, target::ParticleModuleInitSound& module, utils::memory::allocator& allocator)
		{
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_SOUND, allocator);
		}

		void convert_module(const source::ParticleModuleInitSpawn& source, target::ParticleModuleInitSpawn& module, utils::memory::allocator& allocator)
		{
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleInitSpawnShapeBox& source, target::ParticleModuleInitSpawnShapeBox& module, utils::memory::allocator& allocator)
		{
			module.m_axisFlags = source.m_axisFlags;
			module.m_spawnFlags = source.m_spawnFlags;
			module.m_normalAxis = source.m_normalAxis;
			module.m_spawnType = source.m_spawnType;
			module.m_volumeCubeRoot = source.m_volumeCubeRoot;
			copy_same(module.m_calculationOffset, source.m_calculationOffset);
			copy_same(module.m_offset, source.m_offset);
			copy_same(module.m_dimensionsMin, source.m_dimensionsMin);
			copy_same(module.m_dimensionsMax, source.m_dimensionsMax);
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleInitSpawnShapeCylinder& source, target::ParticleModuleInitSpawnShapeCylinder& module, utils::memory::allocator& allocator)
		{
			module.m_axisFlags = source.m_axisFlags;
			module.m_spawnFlags = source.m_spawnFlags;
			module.m_normalAxis = source.m_normalAxis;
			module.m_spawnType = source.m_spawnType;
			module.m_volumeCubeRoot = source.m_volumeCubeRoot;
			copy_same(module.m_calculationOffset, source.m_calculationOffset);
			copy_same(module.m_offset, source.m_offset);
			module.m_hasRotation = source.m_hasRotation;
			module.m_rotateCalculatedOffset = source.m_rotateCalculatedOffset;
			module.m_halfHeight = source.m_halfHeight;
			copy_same(module.m_radius, source.m_radius);
			copy_same(module.m_directionQuat, source.m_directionQuat);
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleInitSpawnShapeEllipsoid& source, target::ParticleModuleInitSpawnShapeEllipsoid& module, utils::memory::allocator& allocator)
		{
			module.m_axisFlags = source.m_axisFlags;
			module.m_spawnFlags = source.m_spawnFlags;
			module.m_normalAxis = source.m_normalAxis;
			module.m_spawnType = source.m_spawnType;
			module.m_volumeCubeRoot = source.m_volumeCubeRoot;
			copy_same(module.m_calculationOffset, source.m_calculationOffset);
			copy_same(module.m_offset, source.m_offset);
			copy_same(module.m_radiusMin, source.m_radiusMin);
			copy_same(module.m_radiusMax, source.m_radiusMax);
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleInitSpawnShapeSphere& source, target::ParticleModuleInitSpawnShapeSphere& module, utils::memory::allocator& allocator)
		{
			module.m_axisFlags = source.m_axisFlags;
			module.m_spawnFlags = source.m_spawnFlags;
			module.m_normalAxis = source.m_normalAxis;
			module.m_spawnType = source.m_spawnType;
			module.m_volumeCubeRoot = source.m_volumeCubeRoot;
			copy_same(module.m_calculationOffset, source.m_calculationOffset);
			copy_same(module.m_offset, source.m_offset);
			copy_same(module.m_radius, source.m_radius);
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleInitTail& source, target::ParticleModuleInitTail& module, utils::memory::allocator&)
		{
			module.m_averagePastVelocities = source.m_averagePastVelocities;
			module.m_maxParentSpeed = source.m_maxParentSpeed;
			module.m_tailLeading = source.m_tailLeading;
			module.m_scaleWithVelocity = source.m_scaleWithVelocity;
		}

		void convert_module(const source::ParticleModuleInitVectorField& source, target::ParticleModuleInitVectorField& module, utils::memory::allocator& allocator)
		{
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_INIT_VECTOR_FIELD, allocator);
		}

		void convert_module(const source::ParticleModuleInitDismember& source, target::ParticleModuleInitDismember& module, utils::memory::allocator&)
		{
			module.m_chance = source.m_chance;
		}

		void convert_module(const source::ParticleModuleColorGraph& source, target::ParticleModuleColorGraph& module, utils::memory::allocator& allocator)
		{
			module.firstCurve = source.firstCurve;
			module.m_modulateColorByAlpha = source.m_modulateColorByAlpha;
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleColorLerp& source, target::ParticleModuleColorLerp& module, utils::memory::allocator&)
		{
			copy_same(module.m_colorBegin, source.m_colorBegin);
			copy_same(module.m_colorEnd, source.m_colorEnd);
		}

		void convert_module(const source::ParticleModuleEmissionGraph& source, target::ParticleModuleEmissionGraph& module, utils::memory::allocator& allocator)
		{
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleEmissiveGraph& source, target::ParticleModuleEmissiveGraph& module, utils::memory::allocator& allocator)
		{
			module.firstCurve = source.firstCurve;
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleForce& source, target::ParticleModuleForce& module, utils::memory::allocator&)
		{
			copy_same(module.m_forceMin, source.m_forceMin);
			copy_same(module.m_forceMax, source.m_forceMax);
		}

		void convert_module(const source::ParticleModuleForceDragGraph& source, target::ParticleModuleForceDragGraph& module, utils::memory::allocator& allocator)
		{
			convert_curves(source.m_curves, module.m_curves, allocator);
			module.m_flags &= ~source::PARTICLE_MODULE_FLAG_UNKNOWN_200;
			for (auto& curve : module.m_curves)
			{
				curve.scale = drag_scale(curve.scale);
			}
		}

		void convert_module(const source::ParticleModuleGravity& source, target::ParticleModuleGravity& module, utils::memory::allocator&)
		{
			copy_same(module.m_gravityPercentage, source.m_gravityPercentage);
			module.m_flags &= ~source::PARTICLE_MODULE_FLAG_UNKNOWN_200;
		}

		void convert_module(const source::ParticleModuleIntensityGraph& source, target::ParticleModuleIntensityGraph& module, utils::memory::allocator& allocator)
		{
			module.firstCurve = source.firstCurve;
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleParentVelocityGraph& source, target::ParticleModuleParentVelocityGraph& module, utils::memory::allocator& allocator)
		{
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModulePhysicsLight& source, target::ParticleModulePhysicsLight& module, utils::memory::allocator& allocator)
		{
			module.m_ignoreEmitterOrientation = source.m_ignoreEmitterOrientation;
			module.m_useSurfaceType = source.m_useSurfaceType;
			convert_linked_assets(source.m_linkedAssetList, module.m_linkedAssetList, target::PARTICLE_MODULE_PHYSICS_LIGHT, allocator);
		}

		void convert_module(const source::ParticleModulePhysicsRayCast& source, target::ParticleModulePhysicsRayCast& module, utils::memory::allocator&)
		{
			copy_same(module.m_bounce, source.m_bounce);
			copy_same(module.m_bounds, source.m_bounds);
			module.m_useItemClip = source.m_useItemClip;
			module.m_useSurfaceType = source.m_useSurfaceType;
			module.m_collideWithWater = source.m_collideWithWater;
			module.m_ignoreContentItem = source.m_ignoreContentItem;
		}

		void convert_module(const source::ParticleModulePositionGraph& source, target::ParticleModulePositionGraph& module, utils::memory::allocator& allocator)
		{
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleRotationGraph& source, target::ParticleModuleRotationGraph& module, utils::memory::allocator& allocator)
		{
			module.m_useRotationRate = source.m_useRotationRate;
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleRotationGraph3D& source, target::ParticleModuleRotationGraph3D& module, utils::memory::allocator& allocator)
		{
			module.m_useRotationRate = source.m_useRotationRate;
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleScaleByDistance& source, target::ParticleModuleScaleByDistance& module, utils::memory::allocator& allocator)
		{
			convert_curves(source.m_curves, module.m_curves, allocator);
			module.m_maxDistance = source.m_maxDistance;
		}

		void convert_module(const source::ParticleModuleSizeGraph& source, target::ParticleModuleSizeGraph& module, utils::memory::allocator& allocator)
		{
			module.firstCurve = source.firstCurve;
			convert_curves(source.m_curves, module.m_curves, allocator);
			copy_same(module.m_sizeBegin, source.m_sizeBegin);
			copy_same(module.m_sizeEnd, source.m_sizeEnd);
		}

		void convert_module(const source::ParticleModuleSizeLerp& source, target::ParticleModuleSizeLerp& module, utils::memory::allocator&)
		{
			copy_same(module.m_sizeBegin, source.m_sizeBegin);
			copy_same(module.m_sizeEnd, source.m_sizeEnd);
		}

		void convert_module(const source::ParticleModuleVelocityGraph& source, target::ParticleModuleVelocityGraph& module, utils::memory::allocator& allocator)
		{
			convert_curves(source.m_curves, module.m_curves, allocator);
			copy_same(module.m_velocityBegin, source.m_velocityBegin);
			copy_same(module.m_velocityEnd, source.m_velocityEnd);
		}

		void convert_module(const source::ParticleModuleVectorFieldGraph& source, target::ParticleModuleVectorFieldGraph& module, utils::memory::allocator& allocator)
		{
			module.firstCurve = source.firstCurve;
			convert_curves(source.m_curves, module.m_curves, allocator);
		}

		void convert_module(const source::ParticleModuleTestAge& source, target::ParticleModuleTestAge& module, utils::memory::allocator& allocator)
		{
			convert_test(source, module, allocator);
			module.m_age = source.m_age;
		}

		void convert_module(const source::ParticleModuleTestBirth& source, target::ParticleModuleTestBirth& module, utils::memory::allocator& allocator)
		{
			convert_test(source, module, allocator);
		}

		void convert_module(const source::ParticleModuleTestDeath& source, target::ParticleModuleTestDeath& module, utils::memory::allocator& allocator)
		{
			convert_test(source, module, allocator);
		}

		void convert_module(const source::ParticleModuleTestImpact& source, target::ParticleModuleTestImpact& module, utils::memory::allocator& allocator)
		{
			convert_test(source, module, allocator);
			module.m_impactDirection = source.m_impactDirection;
		}

		void convert_module(const source::ParticleModuleTestTimeInState& source, target::ParticleModuleTestTimeInState& module, utils::memory::allocator& allocator)
		{
			convert_test(source, module, allocator);
			copy_same(module.m_timeInState, source.m_timeInState);
		}

		bool convert_module(const source::ParticleModuleDef& module, target::ParticleModuleDef& result, utils::memory::allocator& allocator)
		{
			const auto& data = module.moduleData;
			result.moduleData.moduleBase.m_flags = data.moduleBase.m_flags;

			switch (module.moduleType)
			{
			case source::PARTICLE_MODULE_INIT_ATLAS:
				convert_module(data.initAtlas, result.moduleData.initAtlas, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_ATLAS;
				break;
			case source::PARTICLE_MODULE_INIT_ATTRIBUTES:
				convert_module(data.initAttributes, result.moduleData.initAttributes, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_ATTRIBUTES;
				break;
			case source::PARTICLE_MODULE_INIT_BEAM:
				convert_module(data.initBeam, result.moduleData.initBeam, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_BEAM;
				break;
			case source::PARTICLE_MODULE_INIT_CAMERA_OFFSET:
				convert_module(data.initCameraOffset, result.moduleData.initCameraOffset, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_CAMERA_OFFSET;
				break;
			case source::PARTICLE_MODULE_INIT_DECAL:
				convert_module(data.initDecal, result.moduleData.initDecal, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_DECAL;
				break;
			case source::PARTICLE_MODULE_INIT_FLARE:
				convert_module(data.initFlare, result.moduleData.initFlare, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_FLARE;
				break;
			case source::PARTICLE_MODULE_INIT_GEO_TRAIL:
				convert_module(data.initGeoTrail, result.moduleData.initGeoTrail, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_GEO_TRAIL;
				break;
			case source::PARTICLE_MODULE_INIT_KILL_WRAP_BOX:
				convert_module(data.initKillWrapBox, result.moduleData.initKillWrapBox, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_KILL_WRAP_BOX;
				break;
			case source::PARTICLE_MODULE_INIT_LIGHT_OMNI:
				convert_module(data.initLightOmni, result.moduleData.initLightOmni, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_LIGHT_OMNI;
				break;
			case source::PARTICLE_MODULE_INIT_LIGHT_SPOT:
				convert_module(data.initLightSpot, result.moduleData.initLightSpot, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_LIGHT_SPOT;
				break;
			case source::PARTICLE_MODULE_INIT_MATERIAL:
				convert_module(data.initMaterial, result.moduleData.initMaterial, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_MATERIAL;
				break;
			case source::PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
				convert_module(data.initMirrorTexture, result.moduleData.initMirrorTexture, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_MIRROR_TEXTURE;
				break;
			case source::PARTICLE_MODULE_INIT_MODEL:
				convert_module(data.initModel, result.moduleData.initModel, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_MODEL;
				break;
			case source::PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
				convert_module(data.initOcclusionQuery, result.moduleData.initOcclusionQuery, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_OCCLUSION_QUERY;
				break;
			case source::PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
				convert_module(data.initOrientedSprite, result.moduleData.initOrientedSprite, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_ORIENTED_SPRITE;
				break;
			case source::PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
				convert_module(data.initRelativeVelocity, result.moduleData.initRelativeVelocity, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_RELATIVE_VELOCITY;
				break;
			case source::PARTICLE_MODULE_INIT_ROTATION:
				convert_module(data.initRotation, result.moduleData.initRotation, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_ROTATION;
				break;
			case source::PARTICLE_MODULE_INIT_ROTATION_3D:
				convert_module(data.initRotation3D, result.moduleData.initRotation3D, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_ROTATION_3D;
				break;
			case source::PARTICLE_MODULE_INIT_RUNNER:
				convert_module(data.initRunner, result.moduleData.initRunner, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_RUNNER;
				break;
			case source::PARTICLE_MODULE_INIT_SOUND:
				convert_module(data.initSound, result.moduleData.initSound, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_SOUND;
				break;
			case source::PARTICLE_MODULE_INIT_SPAWN:
				convert_module(data.initSpawn, result.moduleData.initSpawn, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_SPAWN;
				break;
			case source::PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
				convert_module(data.initSpawnShapeBox, result.moduleData.initSpawnShapeBox, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX;
				break;
			case source::PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
				convert_module(data.initSpawnShapeCylinder, result.moduleData.initSpawnShapeCylinder, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER;
				break;
			case source::PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
				convert_module(data.initSpawnShapeEllipsoid, result.moduleData.initSpawnShapeEllipsoid, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID;
				break;
			case source::PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
				convert_module(data.initSpawnShapeSphere, result.moduleData.initSpawnShapeSphere, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE;
				break;
			case source::PARTICLE_MODULE_INIT_TAIL:
				convert_module(data.initTail, result.moduleData.initTail, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_TAIL;
				break;
			case source::PARTICLE_MODULE_INIT_VECTOR_FIELD:
				convert_module(data.initVectorField, result.moduleData.initVectorField, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_VECTOR_FIELD;
				break;
			case source::PARTICLE_MODULE_INIT_DISMEMBER:
				convert_module(data.initDismember, result.moduleData.initDismember, allocator);
				result.moduleType = target::PARTICLE_MODULE_INIT_DISMEMBER;
				break;
			case source::PARTICLE_MODULE_COLOR_GRAPH:
				convert_module(data.colorGraph, result.moduleData.colorGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_COLOR_GRAPH;
				break;
			case source::PARTICLE_MODULE_COLOR_LERP:
				convert_module(data.colorLerp, result.moduleData.colorLerp, allocator);
				result.moduleType = target::PARTICLE_MODULE_COLOR_LERP;
				break;
			case source::PARTICLE_MODULE_EMISSION_GRAPH:
				convert_module(data.emissionGraph, result.moduleData.emissionGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_EMISSION_GRAPH;
				break;
			case source::PARTICLE_MODULE_EMISSIVE_GRAPH:
				convert_module(data.emissiveGraph, result.moduleData.emissiveGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_EMISSIVE_GRAPH;
				break;
			case source::PARTICLE_MODULE_FORCE:
				convert_module(data.force, result.moduleData.force, allocator);
				result.moduleType = target::PARTICLE_MODULE_FORCE;
				break;
			case source::PARTICLE_MODULE_FORCE_DRAG_GRAPH:
				convert_module(data.forceDragGraph, result.moduleData.forceDragGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_FORCE_DRAG_GRAPH;
				break;
			case source::PARTICLE_MODULE_GRAVITY:
				convert_module(data.gravity, result.moduleData.gravity, allocator);
				result.moduleType = target::PARTICLE_MODULE_GRAVITY;
				break;
			case source::PARTICLE_MODULE_INTENSITY_GRAPH:
				convert_module(data.intensityGraph, result.moduleData.intensityGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_INTENSITY_GRAPH;
				break;
			case source::PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
				convert_module(data.parentVelocityGraph, result.moduleData.parentVelocityGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_PARENT_VELOCITY_GRAPH;
				break;
			case source::PARTICLE_MODULE_PHYSICS_LIGHT:
				convert_module(data.physicsLight, result.moduleData.physicsLight, allocator);
				result.moduleType = target::PARTICLE_MODULE_PHYSICS_LIGHT;
				break;
			case source::PARTICLE_MODULE_PHYSICS_RAY_CAST:
				convert_module(data.physicsRayCast, result.moduleData.physicsRayCast, allocator);
				result.moduleType = target::PARTICLE_MODULE_PHYSICS_RAY_CAST;
				break;
			case source::PARTICLE_MODULE_POSITION_GRAPH:
				convert_module(data.positionGraph, result.moduleData.positionGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_POSITION_GRAPH;
				break;
			case source::PARTICLE_MODULE_ROTATION_GRAPH:
				convert_module(data.rotationGraph, result.moduleData.rotationGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_ROTATION_GRAPH;
				break;
			case source::PARTICLE_MODULE_ROTATION_GRAPH_3D:
				convert_module(data.rotationGraph3D, result.moduleData.rotationGraph3D, allocator);
				result.moduleType = target::PARTICLE_MODULE_ROTATION_GRAPH_3D;
				break;
			case source::PARTICLE_MODULE_SCALE_BY_DISTANCE:
				convert_module(data.scaleByDistance, result.moduleData.scaleByDistance, allocator);
				result.moduleType = target::PARTICLE_MODULE_SCALE_BY_DISTANCE;
				break;
			case source::PARTICLE_MODULE_SIZE_GRAPH:
				convert_module(data.sizeGraph, result.moduleData.sizeGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_SIZE_GRAPH;
				break;
			case source::PARTICLE_MODULE_SIZE_LERP:
				convert_module(data.sizeLerp, result.moduleData.sizeLerp, allocator);
				result.moduleType = target::PARTICLE_MODULE_SIZE_LERP;
				break;
			case source::PARTICLE_MODULE_VELOCITY_GRAPH:
				convert_module(data.velocityGraph, result.moduleData.velocityGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_VELOCITY_GRAPH;
				break;
			case source::PARTICLE_MODULE_VECTOR_FIELD_GRAPH:
				convert_module(data.vectorFieldGraph, result.moduleData.vectorFieldGraph, allocator);
				result.moduleType = target::PARTICLE_MODULE_VECTOR_FIELD_GRAPH;
				break;
			case source::PARTICLE_MODULE_TEST_AGE:
				convert_module(data.testAge, result.moduleData.testAge, allocator);
				result.moduleType = target::PARTICLE_MODULE_TEST_AGE;
				break;
			case source::PARTICLE_MODULE_TEST_BIRTH:
				convert_module(data.testBirth, result.moduleData.testBirth, allocator);
				result.moduleType = target::PARTICLE_MODULE_TEST_BIRTH;
				break;
			case source::PARTICLE_MODULE_TEST_DEATH:
				convert_module(data.testDeath, result.moduleData.testDeath, allocator);
				result.moduleType = target::PARTICLE_MODULE_TEST_DEATH;
				break;
			case source::PARTICLE_MODULE_TEST_IMPACT:
				convert_module(data.testImpact, result.moduleData.testImpact, allocator);
				result.moduleType = target::PARTICLE_MODULE_TEST_IMPACT;
				break;
			case source::PARTICLE_MODULE_TEST_TIME_IN_STATE:
				convert_module(data.testTimeInState, result.moduleData.testTimeInState, allocator);
				result.moduleType = target::PARTICLE_MODULE_TEST_TIME_IN_STATE;
				break;
			default:
				return false;
			}

			result.moduleData.moduleBase.m_type[0] = static_cast<unsigned char>(result.moduleType);
			return true;
		}

		void convert_group(const source::ParticleModuleGroupDef& group, target::ParticleModuleGroupDef& result, const char* name,
			utils::memory::allocator& allocator)
		{
			result.disabled = group.disabled;
			if (!group.moduleDefs || group.numModules <= 0)
			{
				return;
			}

			const auto modules = allocator.allocate_array<target::ParticleModuleDef>(group.numModules);
			for (auto i = 0; i < group.numModules; i++)
			{
				if (convert_module(group.moduleDefs[i], modules[result.numModules], allocator))
				{
					result.numModules++;
				}
				else
				{
					ZONETOOL_WARNING("vfx \"%s\": module type 0x%X has no proven IW8 layout, the module is lost", name, group.moduleDefs[i].moduleType);
				}
			}

			result.moduleDefs = result.numModules ? modules : nullptr;
		}

		void convert_state(const source::ParticleStateDef& state, target::ParticleStateDef& result, const char* name, utils::memory::allocator& allocator)
		{
			result.elementType = state.elementType;
			result.flags = state.flags;
			if (!state.moduleGroupDefs)
			{
				return;
			}

			result.moduleGroupDefs = allocator.allocate_array<target::ParticleModuleGroupDef>(3);
			for (auto i = 0; i < 3; i++)
			{
				convert_group(state.moduleGroupDefs[i], result.moduleGroupDefs[i], name, allocator);
			}
		}

		unsigned int convert_data_flags(const unsigned int flags)
		{
			return (flags & 0xFF) | ((flags >> 1) & 0x01FFFF00) | ((flags >> 2) & 0xFE000000); // s4 adds bits 8 and 26
		}

		void convert_emitter(const source::ParticleEmitterDef& emitter, target::ParticleEmitterDef& result, const char* name,
			utils::memory::allocator& allocator)
		{
			copy_same(result.particleSpawnRate, emitter.particleSpawnRate);
			copy_same(result.particleLife, emitter.particleLife);
			copy_same(result.particleDelay, emitter.particleDelay);
			result.particleCountMax = emitter.particleCountMax;
			copy_same(result.particleBurstCount, emitter.particleBurstCount);
			copy_same(result.emitterLife, emitter.emitterLife);
			copy_same(result.emitterDelay, emitter.emitterDelay);
			result.randomSeed = emitter.randomSeed;
			copy_same(result.spawnRangeSq, emitter.spawnRangeSq);
			result.fadeOutMaxDistance = emitter.fadeOutMaxDistance;
			convert_curve(emitter.fadeCurveDef, result.fadeCurveDef, allocator);
			result.spawnFrustumCullRadius = emitter.spawnFrustumCullRadius;
			result.flags = emitter.flags;
			result.gravityOptions = static_cast<target::ParticleGravityOptions>(emitter.gravityOptions);
			copy_same(result.groupIDs, emitter.groupIDs);
			copy_same(result.emitByDistanceDensity, emitter.emitByDistanceDensity);
			result.instancePool = emitter.instancePool;
			result.soloInstanceMax = emitter.soloInstanceMax;
			result.instanceAction = static_cast<target::ParticleInstancePoolAction>(emitter.instanceAction);
			result.m_dataFlags = static_cast<target::ParticleDataFlags>(convert_data_flags(emitter.m_dataFlags));

			if (!emitter.stateDefs || emitter.numStates <= 0)
			{
				return;
			}

			result.numStates = emitter.numStates;
			result.stateDefs = allocator.allocate_array<target::ParticleStateDef>(emitter.numStates);
			for (auto i = 0; i < emitter.numStates; i++)
			{
				convert_state(emitter.stateDefs[i], result.stateDefs[i], name, allocator);
			}
		}
	}

	target::ParticleSystemDef* convert(const ParticleSystemDef* asset, utils::memory::allocator& allocator)
	{
		const auto result = allocator.allocate<target::ParticleSystemDef>();
		result->name = asset->name;
		result->version = asset->version;
		result->flags = asset->flags & ~0x1000000u; // s4 only bit
		result->occlusionOverrideEmitterIndex = asset->occlusionOverrideEmitterIndex;
		result->phaseOptions = static_cast<target::ParticlePhaseOptions>(asset->phaseOptions);
		result->drawFrustumCullRadius = asset->drawFrustumCullRadius;
		result->updateFrustumCullRadius = asset->updateFrustumCullRadius;
		result->sunDistance = asset->sunDistance;
		result->preRollMSec = asset->preRollMSec;

		if (asset->numScrStrings)
		{
			ZONETOOL_WARNING("vfx \"%s\": %u script strings are lost", asset->name, asset->numScrStrings);
		}

		if (asset->emitterDefs && asset->numEmitters > 0)
		{
			result->numEmitters = asset->numEmitters;
			result->emitterDefs = allocator.allocate_array<target::ParticleEmitterDef>(asset->numEmitters);
			for (auto i = 0; i < asset->numEmitters; i++)
			{
				convert_emitter(asset->emitterDefs[i], result->emitterDefs[i], asset->name, allocator);
			}
		}

		return result;
	}

	void dump(ParticleSystemDef* asset)
	{
		static std::unordered_set<std::string> dumped;
		if (!dumped.emplace(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		target::vfx::dump(converted);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_VFX), converted->name);
	}
}
