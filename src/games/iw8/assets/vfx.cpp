#include <std_include.hpp>
#include "vfx.hpp"

namespace zonetool::iw8
{
	namespace
	{
		bool is_test_module(const ParticleModuleType type)
		{
			return type >= PARTICLE_MODULE_TEST_BEGIN && type <= PARTICLE_MODULE_TEST_END;
		}

		void read_curve(zone_reader& reader, ParticleCurveDef& curve)
		{
			reader.read_array(curve.controlPoints, 15, curve.numControlPoints);
		}

		template <std::size_t Count>
		void read_curves(zone_reader& reader, ParticleCurveDef (&curves)[Count])
		{
			for (auto& curve : curves)
			{
				read_curve(reader, curve);
			}
		}

		void read_linked_asset(zone_reader& reader, ParticleLinkedAssetDef& asset, const ParticleModuleType type)
		{
			switch (type)
			{
			case PARTICLE_MODULE_INIT_MATERIAL:
				reader.read_asset(ASSET_TYPE_MATERIAL, asset.material);
				break;
			case PARTICLE_MODULE_INIT_MODEL:
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				reader.read_asset(ASSET_TYPE_XMODEL, asset.model);
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				reader.read_asset(ASSET_TYPE_PHYSICS_FX_PIPELINE, asset.physicsFXData.physicsFXPipeline);
				reader.read_asset(ASSET_TYPE_PHYSICS_FX_SHAPE, asset.physicsFXData.physicsFXShape);
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				reader.read_asset(ASSET_TYPE_PARTICLE_SIM_ANIMATION, asset.particleSim);
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				for (auto& material : asset.decal.materials)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, material);
				}
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				reader.read_string(asset.sound);
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				reader.read_asset(ASSET_TYPE_VECTORFIELD, asset.vectorField);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				reader.read_asset(ASSET_TYPE_LIGHT_DEF, asset.lightDef);
				break;
			case PARTICLE_MODULE_INIT_BEAM:
			case PARTICLE_MODULE_INIT_RUNNER:
				reader.read_asset(ASSET_TYPE_VFX, asset.particleSystem);
				break;
			default:
				if (is_test_module(type))
				{
					reader.read_asset(ASSET_TYPE_VFX, asset.particleSystem);
				}
				break;
			}
		}

		void read_linked_assets(zone_reader& reader, ParticleLinkedAssetListDef& list, const ParticleModuleType type)
		{
			if (const auto assets = reader.read_array(list.assetList, 7, list.numAssets))
			{
				for (auto i = 0; i < list.numAssets; i++)
				{
					read_linked_asset(reader, assets[i], type);
				}
			}
		}

		void read_module(zone_reader& reader, ParticleModuleDef& module)
		{
			auto& data = module.moduleData;
			const auto type = module.moduleType;

			switch (type)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
				read_curves(reader, data.initAtlas.m_curves);
				break;
			case PARTICLE_MODULE_INIT_BEAM:
				read_linked_assets(reader, data.initBeam.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				read_linked_assets(reader, data.initDecal.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_FLARE:
				read_curves(reader, data.initFlare.m_curves);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
				read_linked_assets(reader, data.initLightOmni.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				read_linked_assets(reader, data.initLightSpot.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_MATERIAL:
				read_linked_assets(reader, data.initMaterial.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_MODEL:
				read_linked_assets(reader, data.initModel.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				read_linked_assets(reader, data.initParticleSim.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_RUNNER:
				read_linked_assets(reader, data.initRunner.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				read_linked_assets(reader, data.initSound.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_INIT_SPAWN:
				read_curves(reader, data.initSpawn.m_curves);
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
				read_curves(reader, data.initSpawnShapeBox.m_curves);
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
				read_curves(reader, data.initSpawnShapeCylinder.m_curves);
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
				read_curves(reader, data.initSpawnShapeEllipsoid.m_curves);
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				read_linked_assets(reader, data.initSpawnShapeMesh.m_linkedAssetList, type);
				reader.read_array(data.initSpawnShapeMesh.m_meshAssetData, 3, data.initSpawnShapeMesh.m_numMeshAssets);
				read_curves(reader, data.initSpawnShapeMesh.m_curves);
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
				read_curves(reader, data.initSpawnShapeSphere.m_curves);
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				read_linked_assets(reader, data.initVectorField.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_COLOR_GRAPH:
				read_curves(reader, data.colorGraph.m_curves);
				break;
			case PARTICLE_MODULE_EMISSION_GRAPH:
				read_curves(reader, data.emissionGraph.m_curves);
				break;
			case PARTICLE_MODULE_EMISSIVE_GRAPH:
				read_curves(reader, data.emissiveGraph.m_curves);
				break;
			case PARTICLE_MODULE_SHADER_GRAPH:
				read_curves(reader, data.shaderGraph.m_curves);
				break;
			case PARTICLE_MODULE_FORCE_DRAG_GRAPH:
				read_curves(reader, data.forceDragGraph.m_curves);
				break;
			case PARTICLE_MODULE_INTENSITY_GRAPH:
				read_curves(reader, data.intensityGraph.m_curves);
				break;
			case PARTICLE_MODULE_TEMPERATURE_GRAPH:
				read_curves(reader, data.temperatureGraph.m_curves);
				break;
			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
				read_curves(reader, data.parentVelocityGraph.m_curves);
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				read_linked_assets(reader, data.physicsLight.m_linkedAssetList, type);
				break;
			case PARTICLE_MODULE_POSITION_GRAPH:
				read_curves(reader, data.positionGraph.m_curves);
				break;
			case PARTICLE_MODULE_ROTATION_GRAPH:
				read_curves(reader, data.rotationGraph.m_curves);
				break;
			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
				read_curves(reader, data.rotationGraph3D.m_curves);
				break;
			case PARTICLE_MODULE_SCALE_BY_DISTANCE:
				read_curves(reader, data.scaleByDistance.m_curves);
				break;
			case PARTICLE_MODULE_SIZE_GRAPH:
				read_curves(reader, data.sizeGraph.m_curves);
				break;
			case PARTICLE_MODULE_VELOCITY_GRAPH:
				read_curves(reader, data.velocityGraph.m_curves);
				break;
			case PARTICLE_MODULE_VECTOR_FIELD_GRAPH:
				read_curves(reader, data.vectorFieldGraph.m_curves);
				break;
			default:
				if (is_test_module(type))
				{
					read_linked_assets(reader, data.testAge.m_eventHandlerData.m_linkedAssetList, type);
				}
				break;
			}
		}

		void read_state(zone_reader& reader, ParticleStateDef& state)
		{
			if (const auto groups = reader.read_array(state.moduleGroupDefs, 15, 3))
			{
				for (auto i = 0; i < 3; i++)
				{
					if (const auto modules = reader.read_array(groups[i].moduleDefs, 15, groups[i].numModules))
					{
						for (auto j = 0; j < groups[i].numModules; j++)
						{
							read_module(reader, modules[j]);
						}
					}
				}
			}
		}

		void read_emitter(zone_reader& reader, ParticleEmitterDef& emitter)
		{
			if (const auto states = reader.read_array(emitter.stateDefs, 15, emitter.numStates))
			{
				for (auto i = 0; i < emitter.numStates; i++)
				{
					read_state(reader, states[i]);
				}
			}

			read_curve(reader, emitter.fadeCurveDef);
		}
	}

	void vfx::read(zone_reader& reader, ParticleSystemDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto emitters = reader.read_array(asset->emitterDefs, 15, asset->numEmitters))
		{
			for (auto i = 0; i < asset->numEmitters; i++)
			{
				read_emitter(reader, emitters[i]);
			}
		}

		reader.read_array(asset->scriptedInputNodeDefs, 15, asset->numScriptedInputNodes);
		reader.pop_stream();
	}

	void vector_field::read(zone_reader& reader, VectorField* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto fields = reader.read_array(asset->subFields, 15, asset->numSubFields))
		{
			for (auto i = 0u; i < asset->numSubFields; i++)
			{
				const auto& entries = fields[i].numEntries;
				reader.read_array(fields[i].linearData, 3, entries[0] * entries[1] * entries[2]);
			}
		}
		reader.pop_stream();
	}

	void footstep_vfx::read(zone_reader& reader, FootstepVFX* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		for (auto& effect : asset->footstepVFX)
		{
			reader.read_asset(ASSET_TYPE_VFX, effect.particleSystemDef);
		}

		for (auto& effect : asset->smallFootstepVFX)
		{
			reader.read_asset(ASSET_TYPE_VFX, effect.particleSystemDef);
		}
		reader.pop_stream();
	}
}
