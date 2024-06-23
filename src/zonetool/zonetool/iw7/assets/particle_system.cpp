#include "std_include.hpp"
#include "particle_system.hpp"

#define DUMP_JSON

namespace zonetool::iw7
{
	const std::string PARTICLE_ELEMENT_TYPE_s[15] =
	{
		"PARTICLE_ELEMENT_TYPE_BILLBOARD_SPRITE",
		"PARTICLE_ELEMENT_TYPE_BEAM",
		"PARTICLE_ELEMENT_TYPE_CLOUD",
		"PARTICLE_ELEMENT_TYPE_SPARK_CLOUD",
		"PARTICLE_ELEMENT_TYPE_DECAL",
		"PARTICLE_ELEMENT_TYPE_FLARE",
		"PARTICLE_ELEMENT_TYPE_GEO_TRAIL",
		"PARTICLE_ELEMENT_TYPE_LIGHT_OMNI",
		"PARTICLE_ELEMENT_TYPE_LIGHT_SPOT",
		"PARTICLE_ELEMENT_TYPE_MODEL",
		"PARTICLE_ELEMENT_TYPE_ORIENTED_SPRITE",
		"PARTICLE_ELEMENT_TYPE_RUNNER",
		"PARTICLE_ELEMENT_TYPE_TAIL",
		"PARTICLE_ELEMENT_TYPE_VECTOR_FIELD",
		"PARTICLE_ELEMENT_TYPE_VOLUMETRIC",
	};

	const std::string ParticleModuleType_s[] =
	{
		"PARTICLE_MODULE_INIT_ATLAS",              // 0x0
		"PARTICLE_MODULE_INIT_ATTRIBUTES",         // 0x1
		"PARTICLE_MODULE_INIT_BEAM",               // 0x2
		"PARTICLE_MODULE_INIT_CAMERA_OFFSET",      // 0x3
		"PARTICLE_MODULE_INIT_CLOUD",              // 0x4
		"PARTICLE_MODULE_INIT_DECAL",              // 0x5
		"PARTICLE_MODULE_INIT_FLARE",              // 0x6
		"PARTICLE_MODULE_INIT_GEO_TRAIL",          // 0x7
		"PARTICLE_MODULE_INIT_LIGHT_OMNI",         // 0x8
		"PARTICLE_MODULE_INIT_LIGHT_SPOT",         // 0x9
		"PARTICLE_MODULE_INIT_MATERIAL",           // 0xA
		"PARTICLE_MODULE_INIT_MIRROR_TEXTURE",     // 0xB
		"PARTICLE_MODULE_INIT_MODEL",              // 0xC
		"PARTICLE_MODULE_INIT_OCCLUSION_QUERY",    // 0xD
		"PARTICLE_MODULE_INIT_ORIENTED_SPRITE",    // 0xE
		"PARTICLE_MODULE_INIT_PARTICLE_SIM",       // 0xF
		"PARTICLE_MODULE_INIT_PLAYER_FACING",      // 0x10
		"PARTICLE_MODULE_INIT_RELATIVE_VELOCITY",  // 0x11
		"PARTICLE_MODULE_INIT_ROTATION",           // 0x12
		"PARTICLE_MODULE_INIT_ROTATION_3D",        // 0x13
		"PARTICLE_MODULE_INIT_RUNNER",             // 0x14
		"PARTICLE_MODULE_INIT_SOUND",              // 0x15
		"PARTICLE_MODULE_INIT_SPAWN",              // 0x16
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX",    // 0x17
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER", // 0x18
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID", // 0x19
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH",   // 0x1A
		"PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE", // 0x1B
		"PARTICLE_MODULE_INIT_TAIL",               // 0x1C
		"PARTICLE_MODULE_INIT_VECTOR_FIELD",       // 0x1D
		"PARTICLE_MODULE_INIT_VOLUMETRIC",         // 0x1E
		"PARTICLE_MODULE_ATTRACTOR",               // 0x1F
		"PARTICLE_MODULE_COLOR_GRAPH",             // 0x20
		"PARTICLE_MODULE_COLOR_LERP",              // 0x21
		"PARTICLE_MODULE_EMISSION_GRAPH",          // 0x22
		"PARTICLE_MODULE_EMISSIVE_GRAPH",          // 0x23
		"PARTICLE_MODULE_FORCE",                   // 0x24
		"PARTICLE_MODULE_GRAVITY",                 // 0x25
		"PARTICLE_MODULE_INTENSITY_GRAPH",         // 0x26
		"PARTICLE_MODULE_PARENT_VELOCITY_GRAPH",   // 0x27
		"PARTICLE_MODULE_PHYSICS_LIGHT",           // 0x28
		"PARTICLE_MODULE_PHYSICS_RAY_CAST",        // 0x29
		"PARTICLE_MODULE_POSITION_GRAPH",          // 0x2A
		"PARTICLE_MODULE_ROTATION_GRAPH",          // 0x2B
		"PARTICLE_MODULE_ROTATION_GRAPH_3D",       // 0x2C
		"PARTICLE_MODULE_SIZE_GRAPH",              // 0x2D
		"PARTICLE_MODULE_SIZE_LERP",               // 0x2E
		"PARTICLE_MODULE_VELOCITY_GRAPH",          // 0x2F
		"PARTICLE_MODULE_TEST_AGE",                // 0x30
		"PARTICLE_MODULE_TEST_BIRTH",              // 0x31
		"PARTICLE_MODULE_TEST_DEATH",              // 0x32
		"PARTICLE_MODULE_TEST_GRAVITY",            // 0x33
		"PARTICLE_MODULE_TEST_IMPACT",             // 0x34
		"PARTICLE_MODULE_TEST_POS",                // 0x35
	};

	const std::string ParticleModuleGroup_s[] =
	{
		"PARTICLE_MODULE_GROUP_INIT",
		"PARTICLE_MODULE_GROUP_UPDATE",
		"PARTICLE_MODULE_GROUP_TEST",
	};

	const std::string PARTICLE_RELATIVE_VELOCITY_TYPE_s[] =
	{
		"PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL_WITH_BOLT_INFO",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_WORLD_WITH_BOLT_INFO",
		"PARTICLE_RELATIVE_VELOCITY_TYPE_RELATIVE_TO_EFFECT_ORIGIN_WITH_BOLT_INFO",
	};

	ParticleModuleDef* parent_module;

	namespace vfx::binary
	{
		void read_particle_linked_asset_def(ParticleLinkedAssetDef* asset, assetmanager::reader& read)
		{
			switch (parent_module->moduleType)
			{
			case PARTICLE_MODULE_INIT_MATERIAL:
				asset->material = read.read_asset<Material>();
				break;
			case PARTICLE_MODULE_INIT_MODEL:
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				asset->model = read.read_asset<XModel>();
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				asset->physicsFXData.physicsFXPipeline = read.read_asset<PhysicsFXPipeline>();
				asset->physicsFXData.physicsFXShape = read.read_asset<PhysicsFXShape>();
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				asset->particleSim = read.read_asset<FxParticleSimAnimation>();
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				asset->decal.materials[0] = read.read_asset<Material>();
				asset->decal.materials[1] = read.read_asset<Material>();
				asset->decal.materials[2] = read.read_asset<Material>();
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				asset->sound = read.read_string();
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				asset->vectorField = read.read_asset<VectorField>();
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				asset->lightDef = read.read_asset<GfxLightDef>();
				break;
			case PARTICLE_MODULE_INIT_BEAM:
			case PARTICLE_MODULE_INIT_RUNNER:

			case PARTICLE_MODULE_TEST_AGE:
			case PARTICLE_MODULE_TEST_BIRTH:
			case PARTICLE_MODULE_TEST_DEATH:
			case PARTICLE_MODULE_TEST_GRAVITY:
			case PARTICLE_MODULE_TEST_IMPACT:
			case PARTICLE_MODULE_TEST_POS:
				asset->particleSystem = read.read_asset<ParticleSystemDef>();
				break;
			default:
				__debugbreak();
				break;
			}
		}

		void read_particle_linked_asset_list(ParticleLinkedAssetListDef& asset, assetmanager::reader& read)
		{
			asset.assetList = read.read_array<ParticleLinkedAssetDef>();
			for (auto i = 0; i < asset.numAssets; i++)
			{
				read_particle_linked_asset_def(&asset.assetList[i], read);
			}
		}

		void read_module(ParticleModuleDef* asset, assetmanager::reader& read)
		{
			parent_module = asset;

			switch (asset->moduleType)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
				break;
			case PARTICLE_MODULE_INIT_ATTRIBUTES:
				break;
			case PARTICLE_MODULE_INIT_BEAM:
				read_particle_linked_asset_list(asset->moduleData.initBeam.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_CAMERA_OFFSET:
				break;
			case PARTICLE_MODULE_INIT_CLOUD:
				for (auto i = 0; i < 2; i++)
				{
					asset->moduleData.initCloud.curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				read_particle_linked_asset_list(asset->moduleData.initDecal.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_FLARE:
				for (auto i = 0; i < 4; i++)
				{
					asset->moduleData.initFlare.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_INIT_GEO_TRAIL:
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
				read_particle_linked_asset_list(asset->moduleData.initLightOmni.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				read_particle_linked_asset_list(asset->moduleData.initLightSpot.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_MATERIAL:
				read_particle_linked_asset_list(asset->moduleData.initMaterial.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
				break;
			case PARTICLE_MODULE_INIT_MODEL:
				read_particle_linked_asset_list(asset->moduleData.initModel.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
				break;
			case PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				read_particle_linked_asset_list(asset->moduleData.initParticleSim.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_PLAYER_FACING:
				break;
			case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
				break;
			case PARTICLE_MODULE_INIT_ROTATION:
				break;
			case PARTICLE_MODULE_INIT_ROTATION_3D:
				break;
			case PARTICLE_MODULE_INIT_RUNNER:
				read_particle_linked_asset_list(asset->moduleData.initRunner.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				read_particle_linked_asset_list(asset->moduleData.initSound.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_SPAWN:
				for (auto i = 0; i < 1; i++)
				{
					asset->moduleData.initSpawn.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				read_particle_linked_asset_list(asset->moduleData.initSpawnShapeMesh.m_linkedAssetList, read);
				asset->moduleData.initSpawnShapeMesh.m_meshAssetData = read.read_array<ParticleSpawnMeshAssetDef>();
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
				break;
			case PARTICLE_MODULE_INIT_TAIL:
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				read_particle_linked_asset_list(asset->moduleData.initVectorField.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_INIT_VOLUMETRIC:
				break;
			case PARTICLE_MODULE_ATTRACTOR:
				break;
			case PARTICLE_MODULE_COLOR_GRAPH:
				for (auto i = 0; i < 8; i++)
				{
					asset->moduleData.colorGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_COLOR_LERP:
				break;
			case PARTICLE_MODULE_EMISSION_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					asset->moduleData.emissionGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_EMISSIVE_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					asset->moduleData.emissiveGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_FORCE:
				break;
			case PARTICLE_MODULE_GRAVITY:
				break;
			case PARTICLE_MODULE_INTENSITY_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					asset->moduleData.intensityGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					asset->moduleData.parentVelocityGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				read_particle_linked_asset_list(asset->moduleData.physicsLight.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_PHYSICS_RAY_CAST:
				break;
			case PARTICLE_MODULE_POSITION_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					asset->moduleData.positionGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_ROTATION_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					asset->moduleData.rotationGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
				for (auto i = 0; i < 6; i++)
				{
					asset->moduleData.rotationGraph3D.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_SIZE_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					asset->moduleData.sizeGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_SIZE_LERP:
				break;
			case PARTICLE_MODULE_VELOCITY_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					asset->moduleData.velocityGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
				}
				break;
			case PARTICLE_MODULE_TEST_AGE:
				read_particle_linked_asset_list(asset->moduleData.testAge.m_eventHandlerData.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_TEST_BIRTH:
				read_particle_linked_asset_list(asset->moduleData.testBirth.m_eventHandlerData.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_TEST_DEATH:
				read_particle_linked_asset_list(asset->moduleData.testDeath.m_eventHandlerData.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_TEST_GRAVITY:
				read_particle_linked_asset_list(asset->moduleData.testGravity.m_eventHandlerData.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_TEST_IMPACT:
				read_particle_linked_asset_list(asset->moduleData.testImpact.m_eventHandlerData.m_linkedAssetList, read);
				break;
			case PARTICLE_MODULE_TEST_POS:
				read_particle_linked_asset_list(asset->moduleData.testPos.m_eventHandlerData.m_linkedAssetList, read);
				break;
			}
		}

		void read_module_group(ParticleModuleGroupDef* asset, assetmanager::reader& read)
		{
			asset->moduleDefs = read.read_array<ParticleModuleDef>();
			for (auto i = 0; i < asset->numModules; i++)
			{
				read_module(&asset->moduleDefs[i], read);
			}
		}

		void read_state(ParticleStateDef* asset, assetmanager::reader& read)
		{
			asset->moduleGroupDefs = read.read_array<ParticleModuleGroupDef>();
			for (auto i = 0; i < 3; i++)
			{
				read_module_group(&asset->moduleGroupDefs[i], read);
			}
		}

		void read_emitter(ParticleEmitterDef* asset, assetmanager::reader& read)
		{
			asset->stateDefs = read.read_array<ParticleStateDef>();
			for (auto i = 0; i < asset->numStates; i++)
			{
				read_state(&asset->stateDefs[i], read);
			}

			asset->fadeCurveDef.controlPoints = read.read_array<ParticleCurveControlPointDef>();
		}

		ParticleSystemDef* parse(const std::string& name, zone_memory* mem)
		{
			assetmanager::reader read(mem);

			const auto path = "particlesystem\\"s + name + ".iw7VFX"s;
			if (!read.open(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing vfx \"%s\"...", name.data());

			auto* asset = read.read_single<ParticleSystemDef>();
			asset->name = read.read_string();

			asset->emitterDefs = read.read_array<ParticleEmitterDef>();
			for (auto i = 0; i < asset->numEmitters; i++)
			{
				read_emitter(&asset->emitterDefs[i], read);
			}

			asset->scriptedInputNodeDefs = read.read_array<ParticleScriptedInputNodeDef>();

			return asset;
		}

		void dump_particle_linked_asset_def(ParticleLinkedAssetDef* asset, assetmanager::dumper& dump)
		{
			switch (parent_module->moduleType)
			{
			case PARTICLE_MODULE_INIT_MATERIAL:
				dump.dump_asset(asset->material);
				break;
			case PARTICLE_MODULE_INIT_MODEL:
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				dump.dump_asset(asset->model);
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				dump.dump_asset(asset->physicsFXData.physicsFXPipeline);
				dump.dump_asset(asset->physicsFXData.physicsFXShape);
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				dump.dump_asset(asset->particleSim);
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				dump.dump_asset(asset->decal.materials[0]);
				dump.dump_asset(asset->decal.materials[1]);
				dump.dump_asset(asset->decal.materials[2]);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				dump.dump_string(asset->sound);
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				dump.dump_asset(asset->vectorField);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				dump.dump_asset(asset->lightDef);
				break;
			case PARTICLE_MODULE_INIT_BEAM:
			case PARTICLE_MODULE_INIT_RUNNER:

			case PARTICLE_MODULE_TEST_AGE:
			case PARTICLE_MODULE_TEST_BIRTH:
			case PARTICLE_MODULE_TEST_DEATH:
			case PARTICLE_MODULE_TEST_GRAVITY:
			case PARTICLE_MODULE_TEST_IMPACT:
			case PARTICLE_MODULE_TEST_POS:
				dump.dump_asset(asset->particleSystem);
				break;
			default:
				__debugbreak();
				break;
			}
		}

		void dump_particle_linked_asset_list(ParticleLinkedAssetListDef& asset, assetmanager::dumper& dump)
		{
			dump.dump_array(asset.assetList, asset.numAssets);
			for (auto i = 0; i < asset.numAssets; i++)
			{
				dump_particle_linked_asset_def(&asset.assetList[i], dump);
			}
		}

		void dump_module(ParticleModuleDef* asset, assetmanager::dumper& dump)
		{
			parent_module = asset;

			switch (asset->moduleType)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
				break;
			case PARTICLE_MODULE_INIT_ATTRIBUTES:
				break;
			case PARTICLE_MODULE_INIT_BEAM:
				dump_particle_linked_asset_list(asset->moduleData.initBeam.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_CAMERA_OFFSET:
				break;
			case PARTICLE_MODULE_INIT_CLOUD:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.initCloud.curves[i].controlPoints, asset->moduleData.initCloud.curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				dump_particle_linked_asset_list(asset->moduleData.initDecal.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_FLARE:
				for (auto i = 0; i < 4; i++)
				{
					dump.dump_array(asset->moduleData.initFlare.m_curves[i].controlPoints, asset->moduleData.initFlare.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_INIT_GEO_TRAIL:
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
				dump_particle_linked_asset_list(asset->moduleData.initLightOmni.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				dump_particle_linked_asset_list(asset->moduleData.initLightSpot.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_MATERIAL:
				dump_particle_linked_asset_list(asset->moduleData.initMaterial.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
				break;
			case PARTICLE_MODULE_INIT_MODEL:
				dump_particle_linked_asset_list(asset->moduleData.initModel.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
				break;
			case PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				dump_particle_linked_asset_list(asset->moduleData.initParticleSim.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_PLAYER_FACING:
				break;
			case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
				break;
			case PARTICLE_MODULE_INIT_ROTATION:
				break;
			case PARTICLE_MODULE_INIT_ROTATION_3D:
				break;
			case PARTICLE_MODULE_INIT_RUNNER:
				dump_particle_linked_asset_list(asset->moduleData.initRunner.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				dump_particle_linked_asset_list(asset->moduleData.initSound.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_SPAWN:
				for (auto i = 0; i < 1; i++)
				{
					dump.dump_array(asset->moduleData.initSpawn.m_curves[i].controlPoints, asset->moduleData.initSpawn.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				dump_particle_linked_asset_list(asset->moduleData.initSpawnShapeMesh.m_linkedAssetList, dump);
				dump.dump_array(asset->moduleData.initSpawnShapeMesh.m_meshAssetData, asset->moduleData.initSpawnShapeMesh.m_numMeshAssets);
				break;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
				break;
			case PARTICLE_MODULE_INIT_TAIL:
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				dump_particle_linked_asset_list(asset->moduleData.initVectorField.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_INIT_VOLUMETRIC:
				break;
			case PARTICLE_MODULE_ATTRACTOR:
				break;
			case PARTICLE_MODULE_COLOR_GRAPH:
				for (auto i = 0; i < 8; i++)
				{
					dump.dump_array(asset->moduleData.colorGraph.m_curves[i].controlPoints, asset->moduleData.colorGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_COLOR_LERP:
				break;
			case PARTICLE_MODULE_EMISSION_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.emissionGraph.m_curves[i].controlPoints, asset->moduleData.emissionGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_EMISSIVE_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.emissiveGraph.m_curves[i].controlPoints, asset->moduleData.emissiveGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_FORCE:
				break;
			case PARTICLE_MODULE_GRAVITY:
				break;
			case PARTICLE_MODULE_INTENSITY_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.intensityGraph.m_curves[i].controlPoints, asset->moduleData.intensityGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.parentVelocityGraph.m_curves[i].controlPoints, asset->moduleData.parentVelocityGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				dump_particle_linked_asset_list(asset->moduleData.physicsLight.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_PHYSICS_RAY_CAST:
				break;
			case PARTICLE_MODULE_POSITION_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					dump.dump_array(asset->moduleData.positionGraph.m_curves[i].controlPoints, asset->moduleData.positionGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_ROTATION_GRAPH:
				for (auto i = 0; i < 2; i++)
				{
					dump.dump_array(asset->moduleData.rotationGraph.m_curves[i].controlPoints, asset->moduleData.rotationGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
				for (auto i = 0; i < 6; i++)
				{
					dump.dump_array(asset->moduleData.rotationGraph3D.m_curves[i].controlPoints, asset->moduleData.rotationGraph3D.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_SIZE_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					dump.dump_array(asset->moduleData.sizeGraph.m_curves[i].controlPoints, asset->moduleData.sizeGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_SIZE_LERP:
				break;
			case PARTICLE_MODULE_VELOCITY_GRAPH:
				for (auto i = 0; i < 6; i++)
				{
					dump.dump_array(asset->moduleData.velocityGraph.m_curves[i].controlPoints, asset->moduleData.velocityGraph.m_curves[i].numControlPoints);
				}
				break;
			case PARTICLE_MODULE_TEST_AGE:
				dump_particle_linked_asset_list(asset->moduleData.testAge.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_BIRTH:
				dump_particle_linked_asset_list(asset->moduleData.testBirth.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_DEATH:
				dump_particle_linked_asset_list(asset->moduleData.testDeath.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_GRAVITY:
				dump_particle_linked_asset_list(asset->moduleData.testGravity.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_IMPACT:
				dump_particle_linked_asset_list(asset->moduleData.testImpact.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			case PARTICLE_MODULE_TEST_POS:
				dump_particle_linked_asset_list(asset->moduleData.testPos.m_eventHandlerData.m_linkedAssetList, dump);
				break;
			}
		}

		void dump_module_group(ParticleModuleGroupDef* asset, assetmanager::dumper& dump)
		{
			dump.dump_array(asset->moduleDefs, asset->numModules);
			for (auto i = 0; i < asset->numModules; i++)
			{
				dump_module(&asset->moduleDefs[i], dump);
			}
		}

		void dump_state(ParticleStateDef* asset, assetmanager::dumper& dump)
		{
			dump.dump_array(asset->moduleGroupDefs, 3);
			for (auto i = 0; i < 3; i++)
			{
				dump_module_group(&asset->moduleGroupDefs[i], dump);
			}
		}

		void dump_emitter(ParticleEmitterDef* asset, assetmanager::dumper& dump)
		{
			dump.dump_array(asset->stateDefs, asset->numStates);
			for (auto i = 0; i < asset->numStates; i++)
			{
				dump_state(&asset->stateDefs[i], dump);
			}

			dump.dump_array(asset->fadeCurveDef.controlPoints, asset->fadeCurveDef.numControlPoints);
		}

		void dump(ParticleSystemDef* asset)
		{
			assetmanager::dumper dump;

			const auto path = "particlesystem\\"s + asset->name + ".iw7VFX"s;
			if (!dump.open(path))
			{
				return;
			}

			dump.dump_single(asset);
			dump.dump_string(asset->name);

			dump.dump_array(asset->emitterDefs, asset->numEmitters);
			for (auto i = 0; i < asset->numEmitters; i++)
			{
				dump_emitter(&asset->emitterDefs[i], dump);
			}

			dump.dump_array(asset->scriptedInputNodeDefs, asset->numScriptedInputNodes);

			dump.close();
		}
	}

	namespace vfx::json
	{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define PARSE_FIELD_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		for (auto idx420 = 0u; idx420 < (unsigned int)__size__; idx420++) \
		{ \
			asset->__field__[idx420] = data[#__field__][idx420].get<typename std::remove_reference<decltype(asset->__field__[idx420])>::type>(); \
		} \
	}

#define PARSE_ASSET(__field__) \
	if (!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty()) \
	{ \
		asset->__field__ = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(sizeof(const char*)); \
		asset->__field__->name = mem->duplicate_string(data[#__field__].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}

#define PARSE_ASSET_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		for (auto idx420 = 0u; idx420 < (unsigned int)__size__; idx420++) \
		{ \
			if (!data[#__field__][idx420].is_null() && !data[#__field__][idx420].get<std::string>().empty()) \
			{ \
				asset->__field__[idx420] = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__[idx420])>::type>(sizeof(const char*)); \
				asset->__field__[idx420]->name = mem->duplicate_string(data[#__field__][idx420].get<std::string>()); \
			} \
			else \
			{ \
				asset->__field__[idx420] = nullptr; \
			} \
		} \
	}

		template<typename T> T get_value_index(const std::string& value, const std::string* lookup_table, size_t len, T default_value = 0)
		{
			if (value.empty())
				return default_value;

			for (size_t i = 0; i < len; i++)
			{
				if (lookup_table[i] == value)
					return static_cast<T>(i);
			}

			return default_value;
		}

		void calculate_inv_time_delta(ParticleCurveDef* curve)
		{
			if (!curve->numControlPoints) return;

			assert(curve->controlPoints[0].time == 0.0f);
			//assert(curve->controlPoints[curve->numControlPoints - 1].time == 1.0f);

			float prev_time = 0.0f;

			for (int j = 1; j < curve->numControlPoints; j++)
			{
				curve->controlPoints[j].invTimeDelta = 1.0f / (curve->controlPoints[j].time - prev_time);
				prev_time = curve->controlPoints[j].time;
			}
		}

		void parse_particle_linked_asset_def(ParticleLinkedAssetDef* asset, ordered_json& data, zone_memory* mem)
		{
			switch (parent_module->moduleType)
			{
			case PARTICLE_MODULE_INIT_MATERIAL:
				PARSE_ASSET(material);
				break;
			case PARTICLE_MODULE_INIT_MODEL:
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				PARSE_ASSET(model);
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				PARSE_ASSET(physicsFXData.physicsFXPipeline);
				PARSE_ASSET(physicsFXData.physicsFXShape);
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				PARSE_ASSET(particleSim);
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				PARSE_ASSET_ARR(decal.materials, 3);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				PARSE_STRING(sound);
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				PARSE_ASSET(vectorField);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				PARSE_ASSET(lightDef);
				break;
			case PARTICLE_MODULE_INIT_BEAM:
			case PARTICLE_MODULE_INIT_RUNNER:

			case PARTICLE_MODULE_TEST_AGE:
			case PARTICLE_MODULE_TEST_BIRTH:
			case PARTICLE_MODULE_TEST_DEATH:
			case PARTICLE_MODULE_TEST_GRAVITY:
			case PARTICLE_MODULE_TEST_IMPACT:
			case PARTICLE_MODULE_TEST_POS:
				PARSE_ASSET(particleSystem);
				break;
			default:
				__debugbreak();
				break;
			}
		}

		void parse_particle_linked_asset_list(ParticleLinkedAssetListDef& asset, ordered_json& data, zone_memory* mem)
		{
			asset.numAssets = static_cast<int>(data.size());
			asset.assetList = mem->allocate<ParticleLinkedAssetDef>(asset.numAssets);
			for (auto i = 0; i < asset.numAssets; i++)
			{
				parse_particle_linked_asset_def(&asset.assetList[i], data[i], mem);
			}
		}

		void parse_control_points(ParticleCurveControlPointDef* asset, ordered_json& data)
		{
			PARSE_FIELD(value);
			PARSE_FIELD(time);
			PARSE_FIELD(invTimeDelta);
		}

		void parse_curve(ParticleCurveDef* asset, ordered_json& data, zone_memory* mem)
		{
			PARSE_FIELD(scale);

			asset->numControlPoints = static_cast<int>(data["ControlPoint"].size());
			asset->controlPoints = mem->allocate<ParticleCurveControlPointDef>(asset->numControlPoints);
			for (auto i = 0; i < asset->numControlPoints; i++)
			{
				parse_control_points(&asset->controlPoints[i], data["ControlPoint"][i]);
			}

			calculate_inv_time_delta(asset);
		}

#define PARSE_TYPE(__type__) \
	asset->type = get_value_index<ParticleModuleType>(data["type"].get<std::string>(), ParticleModuleType_s, std::extent_v<decltype(ParticleModuleType_s)>, PARTICLE_MODULE_INVALID);

		void parse_module(ParticleModuleDef* asset_, ordered_json& data, zone_memory* mem)
		{
			parent_module = asset_;

			{
				auto* asset = &asset_->moduleData.moduleBase;
				PARSE_TYPE(type);
				PARSE_FIELD(m_flags);

				asset_->moduleType = asset->type;
			}

			switch (asset_->moduleType)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
			{
				auto* asset = &asset_->moduleData.initAtlas;

				PARSE_FIELD(m_playRate);
				PARSE_FIELD(m_startFrame);
				PARSE_FIELD(m_loopCount);
				break;
			}
			case PARTICLE_MODULE_INIT_ATTRIBUTES:
			{
				auto* asset = &asset_->moduleData.initAttributes;

				PARSE_FIELD(m_useNonUniformInterpolationForColor);
				PARSE_FIELD(m_useNonUniformInterpolationForSize);
				PARSE_FIELD_ARR(m_sizeMin.v, 4);
				PARSE_FIELD_ARR(m_sizeMax.v, 4);
				PARSE_FIELD_ARR(m_colorMin.v, 4);
				PARSE_FIELD_ARR(m_colorMax.v, 4);
				PARSE_FIELD_ARR(m_velocityMin.v, 4);
				PARSE_FIELD_ARR(m_velocityMax.v, 4);
				break;
			}
			case PARTICLE_MODULE_INIT_BEAM:
			{
				auto* asset = &asset_->moduleData.initBeam;

				PARSE_FIELD(m_beamFlags);
				PARSE_FIELD(m_splitDistance);
				PARSE_FIELD_ARR(m_startPos.v, 4);
				PARSE_FIELD_ARR(m_endPos.v, 4);
				PARSE_FIELD_ARR(m_offset.v, 4);
				PARSE_FIELD_ARR(m_curvePoint1.v, 4);
				PARSE_FIELD_ARR(m_curvePoint2.v, 4);
				PARSE_FIELD(m_scrollRateUV);
				PARSE_FIELD(m_scrollRateSpiralGraph);
				PARSE_FIELD(m_spiralRepeatDistance);

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_CAMERA_OFFSET:
			{
				[[maybe_unused]] auto* asset = &asset_->moduleData.initCameraOffset;
				break;
			}
			case PARTICLE_MODULE_INIT_CLOUD:
			{
				auto* asset = &asset_->moduleData.initCloud;

				for (auto i = 0; i < 2; i++)
				{
					parse_curve(&asset->curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_DECAL:
			{
				auto* asset = &asset_->moduleData.initDecal;

				PARSE_FIELD(m_fadeInTime);
				PARSE_FIELD(m_fadeOutTime);
				PARSE_FIELD(m_stoppableFadeOutTime);
				PARSE_FIELD(m_lerpWaitTime);

				PARSE_FIELD_ARR(m_lerpColor.v, 4);

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_FLARE:
			{
				auto* asset = &asset_->moduleData.initFlare;

				PARSE_FIELD(m_position);
				PARSE_FIELD_ARR(m_direction.v, 4);
				PARSE_FIELD(m_angularRotCount);
				PARSE_FIELD(m_flareFlags);
				PARSE_FIELD(m_depthScaleRange.min);
				PARSE_FIELD(m_depthScaleRange.max);
				PARSE_FIELD(m_depthScaleValue.min);
				PARSE_FIELD(m_depthScaleValue.max);
				PARSE_FIELD(m_radialRot.min);
				PARSE_FIELD(m_radialRot.max);
				PARSE_FIELD(m_radialScaleX.min);
				PARSE_FIELD(m_radialScaleX.max);
				PARSE_FIELD(m_radialScaleY.min);
				PARSE_FIELD(m_radialScaleY.max);

				for (auto i = 0; i < 4; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_GEO_TRAIL:
			{
				auto* asset = &asset_->moduleData.initGeoTrail;

				PARSE_FIELD(m_numPointsMax);
				PARSE_FIELD(m_splitDistance);
				PARSE_FIELD(m_splitAngle);
				PARSE_FIELD(m_centerOffset);
				PARSE_FIELD(m_numSheets);
				PARSE_FIELD(m_fadeInDistance);
				PARSE_FIELD(m_fadeOutDistance);
				PARSE_FIELD(m_tileDistance);
				PARSE_FIELD(m_tileOffset.min);
				PARSE_FIELD(m_tileOffset.max);
				PARSE_FIELD(m_scrollTime);
				PARSE_FIELD(m_useLocalVelocity);
				PARSE_FIELD(m_useVerticalTexture);
				PARSE_FIELD(m_cameraFacing);
				PARSE_FIELD(m_fixLeadingEdge);
				PARSE_FIELD(m_clampUVs);

				break;
			}
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			{
				auto* asset = &asset_->moduleData.initLightOmni;

				PARSE_FIELD(m_tonemappingScaleFactor);
				PARSE_FIELD(m_intensityIR);
				PARSE_FIELD(m_disableVolumetric);
				PARSE_FIELD(m_exponent);
				PARSE_FIELD(m_shadowSoftness);
				PARSE_FIELD(m_shadowBias);
				PARSE_FIELD(m_shadowArea);
				PARSE_FIELD_ARR(__pad0, 2);

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
			{
				auto* asset = &asset_->moduleData.initLightSpot;

				PARSE_FIELD(m_fovOuter);
				PARSE_FIELD(m_fovInner);
				PARSE_FIELD(m_bulbRadius);
				PARSE_FIELD(m_bulbLength);
				PARSE_FIELD(m_brightness);
				PARSE_FIELD(unk1);
				PARSE_FIELD(m_intensityUV);
				PARSE_FIELD(m_intensityIR);
				PARSE_FIELD(m_shadowNearPlane);
				PARSE_FIELD(m_toneMappingScaleFactor);
				PARSE_FIELD(m_shadowSoftness);
				PARSE_FIELD(m_shadowBias);
				PARSE_FIELD(m_shadowArea);
				PARSE_FIELD(m_disableVolumetric);
				PARSE_FIELD(m_disableShadowMap);
				PARSE_FIELD(m_disableDynamicShadows);
				PARSE_FIELD(m_exponent);

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_MATERIAL:
			{
				auto* asset = &asset_->moduleData.initMaterial;

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
			{
				auto* asset = &asset_->moduleData.initMirrorTexture;

				PARSE_FIELD(m_mirrorHorizontal);
				PARSE_FIELD(m_mirrorVertical);

				break;
			}
			case PARTICLE_MODULE_INIT_MODEL:
			{
				auto* asset = &asset_->moduleData.initModel;

				PARSE_FIELD(m_usePhysics);
				PARSE_FIELD(m_motionBlurHQ);

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
			{
				auto* asset = &asset_->moduleData.initOcclusionQuery;

				PARSE_FIELD(m_fadeInTime);
				PARSE_FIELD(m_fadeOutTime);
				PARSE_FIELD(m_scale.min);
				PARSE_FIELD(m_scale.max);
				PARSE_FIELD_ARR(m_worldSize, 2);

				break;
			}
			case PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
			{
				auto* asset = &asset_->moduleData.initOrientedSprite;

				PARSE_FIELD_ARR(m_orientationQuat.v, 4);

				PARSE_FIELD_ARR(m_pad, 2);

				for (auto i = 0; i < 2; i++)
				{
					if (asset->m_pad[i] != 0)
					{
						__debugbreak();
					}
				}

				break;
			}
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
			{
				auto* asset = &asset_->moduleData.initParticleSim;

				PARSE_FIELD(m_scaleFactor.min);
				PARSE_FIELD(m_scaleFactor.max);
				PARSE_FIELD(m_holdLastFrame);

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_PLAYER_FACING:
			{
				[[maybe_unused]] auto* asset = &asset_->moduleData.initPlayerFacing;

				break;
			}
			case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
			{
				auto* asset = &asset_->moduleData.initRelativeVelocity;

				asset->m_velocityType = get_value_index<ParticleRelativeVelocityType>
					(data["m_velocityType"].get<std::string>(), PARTICLE_RELATIVE_VELOCITY_TYPE_s, 
						std::extent_v<decltype(PARTICLE_RELATIVE_VELOCITY_TYPE_s)>, PARTICLE_RELATIVE_VELOCITY_TYPE_LOCAL);
				PARSE_FIELD(m_useBoltInfo);

				break;
			}
			case PARTICLE_MODULE_INIT_ROTATION:
			{
				auto* asset = &asset_->moduleData.initRotation;

				PARSE_FIELD(m_rotationAngle.min);
				PARSE_FIELD(m_rotationAngle.max);
				PARSE_FIELD(m_rotationRate.min);
				PARSE_FIELD(m_rotationRate.max);

				break;
			}
			case PARTICLE_MODULE_INIT_ROTATION_3D:
			{
				auto* asset = &asset_->moduleData.initRotation3D;

				PARSE_FIELD_ARR(m_rotationAngleMin.v, 4);
				PARSE_FIELD_ARR(m_rotationAngleMin.v, 4);
				PARSE_FIELD_ARR(m_rotationAngleMax.v, 4);
				PARSE_FIELD_ARR(m_rotationAngleMax.v, 4);
				PARSE_FIELD_ARR(m_rotationRateMin.v, 4);
				PARSE_FIELD_ARR(m_rotationRateMin.v, 4);
				PARSE_FIELD_ARR(m_rotationRateMax.v, 4);
				PARSE_FIELD_ARR(m_rotationRateMax.v, 4);

				break;
			}
			case PARTICLE_MODULE_INIT_RUNNER:
			{
				auto* asset = &asset_->moduleData.initRunner;

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_SOUND:
			{
				auto* asset = &asset_->moduleData.initSound;

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN:
			{
				auto* asset = &asset_->moduleData.initSpawn;

				for (auto i = 0; i < 1; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeBox;

				PARSE_FIELD(m_axisFlags);
				PARSE_FIELD(m_spawnFlags);
				PARSE_FIELD(m_normalAxis);
				PARSE_FIELD(m_spawnType);
				PARSE_FIELD(m_volumeCubeRoot);
				PARSE_FIELD_ARR(m_offset.v, 4);

				PARSE_FIELD(m_useBeamInfo);
				PARSE_FIELD_ARR(m_dimensionsMin.v, 4);
				PARSE_FIELD_ARR(m_dimensionsMax.v, 4);

				PARSE_FIELD_ARR(m_pad, 15);

				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeCylinder;

				PARSE_FIELD(m_axisFlags);
				PARSE_FIELD(m_spawnFlags);
				PARSE_FIELD(m_normalAxis);
				PARSE_FIELD(m_spawnType);
				PARSE_FIELD(m_volumeCubeRoot);
				PARSE_FIELD_ARR(m_offset.v, 4);

				PARSE_FIELD(m_hasRotation);
				PARSE_FIELD(m_rotateCalculatedOffset);
				PARSE_FIELD(m_halfHeight);
				PARSE_FIELD(m_radius.min);
				PARSE_FIELD(m_radius.max);
				PARSE_FIELD_ARR(m_directionQuat.v, 4);

				PARSE_FIELD_ARR(unk.v, 4);

				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeEllipsoid;

				PARSE_FIELD(m_axisFlags);
				PARSE_FIELD(m_spawnFlags);
				PARSE_FIELD(m_normalAxis);
				PARSE_FIELD(m_spawnType);
				PARSE_FIELD(m_volumeCubeRoot);

				PARSE_FIELD_ARR(m_radiusMin.v, 4);
				PARSE_FIELD_ARR(m_radiusMax.v, 4);

				PARSE_FIELD_ARR(m_pad, 4);

				break;
			}

			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeMesh;

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				for (auto i = 0u; i < asset->m_numMeshAssets; i++)
				{
					data["meshdata"][i]["m_numVertsTotal"] = asset->m_meshAssetData[i].m_numVertsTotal;
					data["meshdata"][i]["m_numTrisTotal"] = asset->m_meshAssetData[i].m_numTrisTotal;
				}
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeSphere;

				PARSE_FIELD(m_axisFlags);
				PARSE_FIELD(m_spawnFlags);
				PARSE_FIELD(m_normalAxis);
				PARSE_FIELD(m_spawnType);
				PARSE_FIELD(m_volumeCubeRoot);

				PARSE_FIELD_ARR(m_offset.v, 4);
				PARSE_FIELD_ARR(m_offset2.v, 4);

				PARSE_FIELD(m_radius.min);
				PARSE_FIELD(m_radius.max);

				PARSE_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_INIT_TAIL:
			{
				auto* asset = &asset_->moduleData.initTail;

				PARSE_FIELD(m_averagePastVelocities);
				PARSE_FIELD(m_maxParentSpeed);
				PARSE_FIELD(m_tailLeading);
				PARSE_FIELD(m_scaleWithVelocity);
				PARSE_FIELD(m_rotateAroundPivot);

				break;
			}
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
			{
				auto* asset = &asset_->moduleData.initVectorField;

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_INIT_VOLUMETRIC:
			{
				auto* asset = &asset_->moduleData.initVolumetric;

				PARSE_FIELD(m_density);
				PARSE_FIELD(m_falloff);
				PARSE_FIELD(m_noiseAmplitude);
				PARSE_FIELD(m_noiseMode);

				PARSE_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_ATTRACTOR:
			{
				auto* asset = &asset_->moduleData.attractor;

				PARSE_FIELD(m_killWhenNear);
				PARSE_FIELD(m_forceMag);
				PARSE_FIELD(m_nearDistanceSq);
				PARSE_FIELD_ARR(m_attractPoint.v, 4);

				PARSE_FIELD_ARR(m_pad, 3);

				break;
			}
			case PARTICLE_MODULE_COLOR_GRAPH:
			{
				auto* asset = &asset_->moduleData.colorGraph;

				PARSE_FIELD(firstCurve);
				PARSE_FIELD(m_modulateColorByAlpha);

				PARSE_FIELD_ARR(m_pad, 3);

				for (auto i = 0; i < 8; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_COLOR_LERP:
			{
				auto* asset = &asset_->moduleData.colorLerp;

				PARSE_FIELD_ARR(m_colorBegin.v, 4);
				PARSE_FIELD_ARR(m_colorEnd.v, 4);

				PARSE_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_EMISSION_GRAPH:
			{
				auto* asset = &asset_->moduleData.emissionGraph;

				for (auto i = 0; i < 2; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_EMISSIVE_GRAPH:
			{
				auto* asset = &asset_->moduleData.emissiveGraph;

				PARSE_FIELD(firstCurve);

				PARSE_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 2; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_FORCE:
			{
				auto* asset = &asset_->moduleData.force;

				PARSE_FIELD_ARR(m_forceMin.v, 4);
				PARSE_FIELD_ARR(m_forceMax.v, 4);

				break;
			}
			case PARTICLE_MODULE_GRAVITY:
			{
				auto* asset = &asset_->moduleData.gravity;

				PARSE_FIELD(m_gravityPercentage.min);
				PARSE_FIELD(m_gravityPercentage.max);

				break;
			}
			case PARTICLE_MODULE_INTENSITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.intensityGraph;

				PARSE_FIELD(firstCurve);

				PARSE_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 2; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}

			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.velocityGraph;

				for (auto i = 0; i < 2; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_PHYSICS_LIGHT:
			{
				auto* asset = &asset_->moduleData.physicsLight;

				PARSE_FIELD(m_ignoreEmitterOrientation);
				PARSE_FIELD(m_useSurfaceType);

				PARSE_FIELD_ARR(m_pad, 6);

				parse_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_PHYSICS_RAY_CAST:
			{
				auto* asset = &asset_->moduleData.physicsRayCast;

				PARSE_FIELD(m_bounce.min);
				PARSE_FIELD(m_bounce.max);
				PARSE_FIELD_ARR(m_bounds.midPoint, 3);
				PARSE_FIELD_ARR(m_bounds.halfSize, 3);
				PARSE_FIELD(m_useItemClip);
				PARSE_FIELD(m_useSurfaceType);
				PARSE_FIELD(m_collideWithWater);
				PARSE_FIELD(m_ignoreContentItem);

				PARSE_FIELD_ARR(m_pad, 3);

				break;
			}
			case PARTICLE_MODULE_POSITION_GRAPH:
			{
				auto* asset = &asset_->moduleData.positionGraph;

				for (auto i = 0; i < 6; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_ROTATION_GRAPH:
			{
				auto* asset = &asset_->moduleData.rotationGraph;

				PARSE_FIELD(m_useRotationRate);

				PARSE_FIELD_ARR(m_pad, 1);

				for (auto i = 0; i < 2; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}

			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
			{
				auto* asset = &asset_->moduleData.rotationGraph3D;

				PARSE_FIELD(m_useRotationRate);

				PARSE_FIELD_ARR(m_pad, 1);

				for (auto i = 0; i < 6; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_SIZE_GRAPH:
			{
				auto* asset = &asset_->moduleData.sizeGraph;

				PARSE_FIELD(firstCurve);
				PARSE_FIELD_ARR(m_sizeBegin.v, 4);
				PARSE_FIELD_ARR(m_sizeEnd.v, 4);

				PARSE_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 6; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}

			case PARTICLE_MODULE_SIZE_LERP:
			{
				auto* asset = &asset_->moduleData.sizeLerp;

				PARSE_FIELD_ARR(m_sizeBegin.v, 4);
				PARSE_FIELD_ARR(m_sizeEnd.v, 4);

				PARSE_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_VELOCITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.velocityGraph;

				PARSE_FIELD_ARR(m_velocityBegin.v, 4);
				PARSE_FIELD_ARR(m_velocityEnd.v, 4);

				for (auto i = 0; i < 6; i++)
				{
					parse_curve(&asset->m_curves[i], data["Curves"][i], mem);
				}
				break;
			}
			case PARTICLE_MODULE_TEST_AGE:
			{
				auto* asset = &asset_->moduleData.testAge;

				PARSE_FIELD(m_moduleIndex);
				PARSE_FIELD_ARR(m_useOrientationOptions, 1);
				PARSE_FIELD_ARR(m_useScaleOptions, 1);
				PARSE_FIELD_ARR(m_useVelocityOptions, 1);
				PARSE_FIELD(m_eventHandlerData.m_nextState);
				PARSE_FIELD(m_eventHandlerData.m_kill);
				PARSE_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				PARSE_FIELD(m_age);

				parse_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_TEST_BIRTH:
			{
				auto* asset = &asset_->moduleData.testBirth;

				PARSE_FIELD(m_moduleIndex);
				PARSE_FIELD_ARR(m_useOrientationOptions, 1);
				PARSE_FIELD_ARR(m_useScaleOptions, 1);
				PARSE_FIELD_ARR(m_useVelocityOptions, 1);
				PARSE_FIELD(m_eventHandlerData.m_nextState);
				PARSE_FIELD(m_eventHandlerData.m_kill);
				PARSE_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				parse_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_TEST_DEATH:
			{
				auto* asset = &asset_->moduleData.testDeath;

				PARSE_FIELD(m_moduleIndex);
				PARSE_FIELD_ARR(m_useOrientationOptions, 1);
				PARSE_FIELD_ARR(m_useScaleOptions, 1);
				PARSE_FIELD_ARR(m_useVelocityOptions, 1);
				PARSE_FIELD(m_eventHandlerData.m_nextState);
				PARSE_FIELD(m_eventHandlerData.m_kill);
				PARSE_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				parse_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_TEST_GRAVITY:
			{
				auto* asset = &asset_->moduleData.testGravity;

				PARSE_FIELD(m_moduleIndex);
				PARSE_FIELD_ARR(m_useOrientationOptions, 1);
				PARSE_FIELD_ARR(m_useScaleOptions, 1);
				PARSE_FIELD_ARR(m_useVelocityOptions, 1);
				PARSE_FIELD(m_eventHandlerData.m_nextState);
				PARSE_FIELD(m_eventHandlerData.m_kill);
				PARSE_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				parse_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_TEST_IMPACT:
			{
				auto* asset = &asset_->moduleData.testPos;

				PARSE_FIELD(m_moduleIndex);
				PARSE_FIELD_ARR(m_useOrientationOptions, 1);
				PARSE_FIELD_ARR(m_useScaleOptions, 1);
				PARSE_FIELD_ARR(m_useVelocityOptions, 1);
				PARSE_FIELD(m_eventHandlerData.m_nextState);
				PARSE_FIELD(m_eventHandlerData.m_kill);
				PARSE_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				parse_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			case PARTICLE_MODULE_TEST_POS:
			{
				auto* asset = &asset_->moduleData.testImpact;

				PARSE_FIELD(m_moduleIndex);
				PARSE_FIELD_ARR(m_useOrientationOptions, 1);
				PARSE_FIELD_ARR(m_useScaleOptions, 1);
				PARSE_FIELD_ARR(m_useVelocityOptions, 1);
				PARSE_FIELD(m_eventHandlerData.m_nextState);
				PARSE_FIELD(m_eventHandlerData.m_kill);
				PARSE_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				parse_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"], mem);
				break;
			}
			default:
				__debugbreak();
			}
		}

#undef PARSE_TYPE

		void parse_module_group(ParticleModuleGroupDef* asset, ordered_json& data, zone_memory* mem)
		{
			PARSE_FIELD(disabled);
			asset->numModules = static_cast<int>(data["Module"].size());
			asset->moduleDefs = mem->allocate<ParticleModuleDef>(asset->numModules);
			for (auto i = 0; i < asset->numModules; i++)
			{
				parse_module(&asset->moduleDefs[i], data["Module"][i], mem);
			}
		}

		void parse_state(ParticleStateDef* asset, ordered_json& data, zone_memory* mem)
		{
			asset->elementType = get_value_index<unsigned int>(data["elementType"].get<std::string>(), PARTICLE_ELEMENT_TYPE_s, std::extent_v<decltype(PARTICLE_ELEMENT_TYPE_s)>); //PARSE_FIELD(elementType);
			PARSE_FIELD(flags);
			asset->moduleGroupDefs = mem->allocate<ParticleModuleGroupDef>(3);
			for (auto i = 0; i < 3; i++)
			{
				parse_module_group(&asset->moduleGroupDefs[i], data["ModuleGroup"][ParticleModuleGroup_s[i]], mem);
			}
		}

		void parse_emitter(ParticleEmitterDef* asset, ordered_json& data, zone_memory* mem)
		{
			PARSE_FIELD(particleSpawnRate.min);
			PARSE_FIELD(particleSpawnRate.max);

			PARSE_FIELD(particleLife.min);
			PARSE_FIELD(particleLife.max);

			PARSE_FIELD(particleDelay.min);
			PARSE_FIELD(particleDelay.max);

			PARSE_FIELD(particleCountMax);

			PARSE_FIELD(particleBurstCount.min);
			PARSE_FIELD(particleBurstCount.max);

			PARSE_FIELD(emitterLife.min);
			PARSE_FIELD(emitterLife.max);

			PARSE_FIELD(emitterDelay.min);
			PARSE_FIELD(emitterDelay.max);

			PARSE_FIELD(randomSeed);

			PARSE_FIELD(spawnRangeSq.min);
			PARSE_FIELD(spawnRangeSq.max);

			PARSE_FIELD(fadeOutMaxDistance);

			parse_curve(&asset->fadeCurveDef, data["FadeCurve"], mem);

			PARSE_FIELD(spawnFrustumCullRadius);

			PARSE_FIELD(gravityOptions);

			PARSE_FIELD_ARR(groupIDs, 1);

			PARSE_FIELD(emitByDistanceDensity.min);
			PARSE_FIELD(emitByDistanceDensity.max);

			PARSE_FIELD(soloInstanceMax);
			PARSE_FIELD(instanceAction);

			PARSE_FIELD(particleSpawnShapeRange.min);
			PARSE_FIELD(particleSpawnShapeRange.max);

			PARSE_FIELD_ARR(pad, 1);

			PARSE_FIELD(m_dataFlags);
			PARSE_FIELD(flags);

			asset->numStates = static_cast<int>(data["State"].size());
			asset->stateDefs = mem->allocate<ParticleStateDef>(asset->numStates);
			for (auto i = 0; i < asset->numStates; i++)
			{
				parse_state(&asset->stateDefs[i], data["State"][i], mem);
			}
		}

		void parse_scripted_input_node(ParticleScriptedInputNodeDef* asset, ordered_json& data)
		{
			PARSE_FIELD(nodeBase.m_type);
			PARSE_FIELD(nodeBase.m_flags);
			PARSE_FIELD(nodeBase.m_emitterIndex);

			PARSE_FIELD(KVP.m_inputRecord.key);
			PARSE_FIELD(KVP.m_inputRecord.dirty);
			PARSE_FIELD(KVP.m_inputRecord.value);
		}

		ParticleSystemDef* parse(const std::string& name, zone_memory* mem)
		{
			const auto path = "particlesystem\\"s + name + ".json"s;

			auto file = filesystem::file(path);
			if (!file.exists())
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing vfx \"%s\"...", name.data());

			// parse json file
			file.open("rb");
			ordered_json data = ::json::parse(file.read_bytes(file.size()));
			file.close();

			auto asset = mem->allocate<ParticleSystemDef>();
			asset->name = mem->duplicate_string(name);

			PARSE_STRING(name);

			PARSE_FIELD(version);
			PARSE_FIELD(flags);

			asset->numEmitters = static_cast<int>(data["Emitter"].size());
			asset->emitterDefs = mem->allocate<ParticleEmitterDef>(asset->numEmitters);
			for (auto i = 0; i < asset->numEmitters; i++)
			{
				parse_emitter(&asset->emitterDefs[i], data["Emitter"][i], mem);
			}

			asset->numScriptedInputNodes = static_cast<int>(data["scriptedInputNode"].size());
			asset->scriptedInputNodeDefs = mem->allocate<ParticleScriptedInputNodeDef>(asset->numScriptedInputNodes);
			for (auto i = 0; i < asset->numScriptedInputNodes; i++)
			{
				parse_scripted_input_node(&asset->scriptedInputNodeDefs[i], data["scriptedInputNode"][i]);
			}

			PARSE_FIELD(occlusionOverrideEmitterIndex);
			PARSE_FIELD(phaseOptions);
			PARSE_FIELD(drawFrustumCullRadius);
			PARSE_FIELD(updateFrustumCullRadius);
			PARSE_FIELD(sunDistance);
			PARSE_FIELD(preRollMSec);

			PARSE_FIELD_ARR(editorPosition.v, 4);
			PARSE_FIELD_ARR(editorRotation.v, 4);
			PARSE_FIELD_ARR(gameTweakPosition.v, 4);
			PARSE_FIELD_ARR(gameTweakRotation.v, 4);

			return asset;
		}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx420 = 0u; idx420 < (unsigned int)__size__; idx420++) \
	{ \
		data[#__field__][idx420] = asset->__field__[idx420]; \
	}

#define DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__ && __size__) \
	{ \
		for (auto idx420 = 0u; idx420 < (unsigned int)__size__; idx420++) \
		{ \
			if (asset->__field__[idx420]) \
			{ \
				data[#__field__][idx420] = asset->__field__[idx420]->name; \
			} \
			else \
			{ \
				data[#__field__][idx420] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

		void dump_particle_linked_asset_def(ParticleLinkedAssetDef* asset, ordered_json& data)
		{
			switch (parent_module->moduleType)
			{
			case PARTICLE_MODULE_INIT_MATERIAL:
				DUMP_ASSET(material);
				break;
			case PARTICLE_MODULE_INIT_MODEL:
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				DUMP_ASSET(model);
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				DUMP_ASSET(physicsFXData.physicsFXPipeline);
				DUMP_ASSET(physicsFXData.physicsFXShape);
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				DUMP_ASSET(particleSim);
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				DUMP_ASSET_ARR(decal.materials, 3);
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				DUMP_STRING(sound);
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				DUMP_ASSET(vectorField);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				DUMP_ASSET(lightDef);
				break;
			case PARTICLE_MODULE_INIT_BEAM:
			case PARTICLE_MODULE_INIT_RUNNER:

			case PARTICLE_MODULE_TEST_AGE:
			case PARTICLE_MODULE_TEST_BIRTH:
			case PARTICLE_MODULE_TEST_DEATH:
			case PARTICLE_MODULE_TEST_GRAVITY:
			case PARTICLE_MODULE_TEST_IMPACT:
			case PARTICLE_MODULE_TEST_POS:
				DUMP_ASSET(particleSystem);
				break;
			default:
				__debugbreak();
				break;
			}
		}

		void dump_particle_linked_asset_list(ParticleLinkedAssetListDef& asset, ordered_json& data)
		{
			for (auto i = 0; i < asset.numAssets; i++)
			{
				dump_particle_linked_asset_def(&asset.assetList[i], data[i]);
			}
		}

		void dump_control_points(ParticleCurveControlPointDef* asset, ordered_json& data)
		{
			DUMP_FIELD(value);
			DUMP_FIELD(time);
			DUMP_FIELD(invTimeDelta);
		}

		void dump_curve(ParticleCurveDef* asset, ordered_json& data)
		{
			DUMP_FIELD(scale);

			for (auto i = 0; i < asset->numControlPoints; i++)
			{
				dump_control_points(&asset->controlPoints[i], data["ControlPoint"][i]);
			}
		}

#define DUMP_TYPE(__type__) \
	data["type"] = ParticleModuleType_s[asset->type];

		void dump_module(ParticleModuleDef* asset_, ordered_json& data)
		{
			parent_module = asset_;

			{
				auto* asset = &asset_->moduleData.moduleBase;
				DUMP_TYPE(type);
				DUMP_FIELD(m_flags);
			}

			switch (asset_->moduleType)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
			{
				auto* asset = &asset_->moduleData.initAtlas;

				DUMP_FIELD(m_playRate);
				DUMP_FIELD(m_startFrame);
				DUMP_FIELD(m_loopCount);
				break;
			}
			case PARTICLE_MODULE_INIT_ATTRIBUTES:
			{
				auto* asset = &asset_->moduleData.initAttributes;

				DUMP_FIELD(m_useNonUniformInterpolationForColor);
				DUMP_FIELD(m_useNonUniformInterpolationForSize);
				DUMP_FIELD_ARR(m_sizeMin.v, 4);
				DUMP_FIELD_ARR(m_sizeMax.v, 4);
				DUMP_FIELD_ARR(m_colorMin.v, 4);
				DUMP_FIELD_ARR(m_colorMax.v, 4);
				DUMP_FIELD_ARR(m_velocityMin.v, 4);
				DUMP_FIELD_ARR(m_velocityMax.v, 4);
				break;
			}
			case PARTICLE_MODULE_INIT_BEAM:
			{
				auto* asset = &asset_->moduleData.initBeam;

				DUMP_FIELD(m_beamFlags);
				DUMP_FIELD(m_splitDistance);
				DUMP_FIELD_ARR(m_startPos.v, 4);
				DUMP_FIELD_ARR(m_endPos.v, 4);
				DUMP_FIELD_ARR(m_offset.v, 4);
				DUMP_FIELD_ARR(m_curvePoint1.v, 4);
				DUMP_FIELD_ARR(m_curvePoint2.v, 4);
				DUMP_FIELD(m_scrollRateUV);
				DUMP_FIELD(m_scrollRateSpiralGraph);
				DUMP_FIELD(m_spiralRepeatDistance);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_CAMERA_OFFSET:
			{
				[[maybe_unused]] auto* asset = &asset_->moduleData.initCameraOffset;
				break;
			}
			case PARTICLE_MODULE_INIT_CLOUD:
			{
				auto* asset = &asset_->moduleData.initCloud;

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_DECAL:
			{
				auto* asset = &asset_->moduleData.initDecal;

				DUMP_FIELD(m_fadeInTime);
				DUMP_FIELD(m_fadeOutTime);
				DUMP_FIELD(m_stoppableFadeOutTime);
				DUMP_FIELD(m_lerpWaitTime);

				DUMP_FIELD_ARR(m_lerpColor.v, 4);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_FLARE:
			{
				auto* asset = &asset_->moduleData.initFlare;

				DUMP_FIELD(m_position);
				DUMP_FIELD_ARR(m_direction.v, 4);
				DUMP_FIELD(m_angularRotCount);
				DUMP_FIELD(m_flareFlags);
				DUMP_FIELD(m_depthScaleRange.min);
				DUMP_FIELD(m_depthScaleRange.max);
				DUMP_FIELD(m_depthScaleValue.min);
				DUMP_FIELD(m_depthScaleValue.max);
				DUMP_FIELD(m_radialRot.min);
				DUMP_FIELD(m_radialRot.max);
				DUMP_FIELD(m_radialScaleX.min);
				DUMP_FIELD(m_radialScaleX.max);
				DUMP_FIELD(m_radialScaleY.min);
				DUMP_FIELD(m_radialScaleY.max);

				for (auto i = 0; i < 4; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_GEO_TRAIL:
			{
				auto* asset = &asset_->moduleData.initGeoTrail;

				DUMP_FIELD(m_numPointsMax);
				DUMP_FIELD(m_splitDistance);
				DUMP_FIELD(m_splitAngle);
				DUMP_FIELD(m_centerOffset);
				DUMP_FIELD(m_numSheets);
				DUMP_FIELD(m_fadeInDistance);
				DUMP_FIELD(m_fadeOutDistance);
				DUMP_FIELD(m_tileDistance);
				DUMP_FIELD(m_tileOffset.min);
				DUMP_FIELD(m_tileOffset.max);
				DUMP_FIELD(m_scrollTime);
				DUMP_FIELD(m_useLocalVelocity);
				DUMP_FIELD(m_useVerticalTexture);
				DUMP_FIELD(m_cameraFacing);
				DUMP_FIELD(m_fixLeadingEdge);
				DUMP_FIELD(m_clampUVs);

				break;
			}
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			{
				auto* asset = &asset_->moduleData.initLightOmni;

				DUMP_FIELD(m_tonemappingScaleFactor);
				DUMP_FIELD(m_intensityIR);
				DUMP_FIELD(m_disableVolumetric);
				DUMP_FIELD(m_exponent);
				DUMP_FIELD(m_shadowSoftness);
				DUMP_FIELD(m_shadowBias);
				DUMP_FIELD(m_shadowArea);
				DUMP_FIELD_ARR(__pad0, 2);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
			{
				auto* asset = &asset_->moduleData.initLightSpot;

				DUMP_FIELD(m_fovOuter);
				DUMP_FIELD(m_fovInner);
				DUMP_FIELD(m_bulbRadius);
				DUMP_FIELD(m_bulbLength);
				DUMP_FIELD(m_brightness);
				DUMP_FIELD(unk1);
				DUMP_FIELD(m_intensityUV);
				DUMP_FIELD(m_intensityIR);
				DUMP_FIELD(m_shadowNearPlane);
				DUMP_FIELD(m_toneMappingScaleFactor);
				DUMP_FIELD(m_shadowSoftness);
				DUMP_FIELD(m_shadowBias);
				DUMP_FIELD(m_shadowArea);
				DUMP_FIELD(m_disableVolumetric);
				DUMP_FIELD(m_disableShadowMap);
				DUMP_FIELD(m_disableDynamicShadows);
				DUMP_FIELD(m_exponent);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_MATERIAL:
			{
				auto* asset = &asset_->moduleData.initMaterial;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
			{
				auto* asset = &asset_->moduleData.initMirrorTexture;

				DUMP_FIELD(m_mirrorHorizontal);
				DUMP_FIELD(m_mirrorVertical);

				break;
			}
			case PARTICLE_MODULE_INIT_MODEL:
			{
				auto* asset = &asset_->moduleData.initModel;

				DUMP_FIELD(m_usePhysics);
				DUMP_FIELD(m_motionBlurHQ);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
			{
				auto* asset = &asset_->moduleData.initOcclusionQuery;

				DUMP_FIELD(m_fadeInTime);
				DUMP_FIELD(m_fadeOutTime);
				DUMP_FIELD(m_scale.min);
				DUMP_FIELD(m_scale.max);
				DUMP_FIELD_ARR(m_worldSize, 2);

				break;
			}
			case PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
			{
				auto* asset = &asset_->moduleData.initOrientedSprite;

				DUMP_FIELD_ARR(m_orientationQuat.v, 4);

				DUMP_FIELD_ARR(m_pad, 2);

				for (auto i = 0; i < 2; i++)
				{
					if (asset->m_pad[i] != 0)
					{
						__debugbreak();
					}
				}

				break;
			}
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
			{
				auto* asset = &asset_->moduleData.initParticleSim;

				DUMP_FIELD(m_scaleFactor.min);
				DUMP_FIELD(m_scaleFactor.max);
				DUMP_FIELD(m_holdLastFrame);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_PLAYER_FACING:
			{
				[[maybe_unused]] auto* asset = &asset_->moduleData.initPlayerFacing;

				break;
			}
			case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
			{
				auto* asset = &asset_->moduleData.initRelativeVelocity;

				data["m_velocityType"] = PARTICLE_RELATIVE_VELOCITY_TYPE_s[asset->m_velocityType];
				DUMP_FIELD(m_useBoltInfo);

				break;
			}
			case PARTICLE_MODULE_INIT_ROTATION:
			{
				auto* asset = &asset_->moduleData.initRotation;

				DUMP_FIELD(m_rotationAngle.min);
				DUMP_FIELD(m_rotationAngle.max);
				DUMP_FIELD(m_rotationRate.min);
				DUMP_FIELD(m_rotationRate.max);

				break;
			}
			case PARTICLE_MODULE_INIT_ROTATION_3D:
			{
				auto* asset = &asset_->moduleData.initRotation3D;

				DUMP_FIELD_ARR(m_rotationAngleMin.v, 4);
				DUMP_FIELD_ARR(m_rotationAngleMin.v, 4);
				DUMP_FIELD_ARR(m_rotationAngleMax.v, 4);
				DUMP_FIELD_ARR(m_rotationAngleMax.v, 4);
				DUMP_FIELD_ARR(m_rotationRateMin.v, 4);
				DUMP_FIELD_ARR(m_rotationRateMin.v, 4);
				DUMP_FIELD_ARR(m_rotationRateMax.v, 4);
				DUMP_FIELD_ARR(m_rotationRateMax.v, 4);

				break;
			}
			case PARTICLE_MODULE_INIT_RUNNER:
			{
				auto* asset = &asset_->moduleData.initRunner;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_SOUND:
			{
				auto* asset = &asset_->moduleData.initSound;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN:
			{
				auto* asset = &asset_->moduleData.initSpawn;

				for (auto i = 0; i < 1; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeBox;

				DUMP_FIELD(m_axisFlags);
				DUMP_FIELD(m_spawnFlags);
				DUMP_FIELD(m_normalAxis);
				DUMP_FIELD(m_spawnType);
				DUMP_FIELD(m_volumeCubeRoot);
				DUMP_FIELD_ARR(m_offset.v, 4);

				DUMP_FIELD(m_useBeamInfo);
				DUMP_FIELD_ARR(m_dimensionsMin.v, 4);
				DUMP_FIELD_ARR(m_dimensionsMax.v, 4);

				DUMP_FIELD_ARR(m_pad, 15);

				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeCylinder;

				DUMP_FIELD(m_axisFlags);
				DUMP_FIELD(m_spawnFlags);
				DUMP_FIELD(m_normalAxis);
				DUMP_FIELD(m_spawnType);
				DUMP_FIELD(m_volumeCubeRoot);
				DUMP_FIELD_ARR(m_offset.v, 4);

				DUMP_FIELD(m_hasRotation);
				DUMP_FIELD(m_rotateCalculatedOffset);
				DUMP_FIELD(m_halfHeight);
				DUMP_FIELD(m_radius.min);
				DUMP_FIELD(m_radius.max);
				DUMP_FIELD_ARR(m_directionQuat.v, 4);

				DUMP_FIELD_ARR(unk.v, 4);

				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeEllipsoid;

				DUMP_FIELD(m_axisFlags);
				DUMP_FIELD(m_spawnFlags);
				DUMP_FIELD(m_normalAxis);
				DUMP_FIELD(m_spawnType);
				DUMP_FIELD(m_volumeCubeRoot);

				DUMP_FIELD_ARR(m_radiusMin.v, 4);
				DUMP_FIELD_ARR(m_radiusMax.v, 4);

				DUMP_FIELD_ARR(m_pad, 4);

				break;
			}

			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeMesh;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				for (auto i = 0u; i < asset->m_numMeshAssets; i++)
				{
					data["meshdata"][i]["m_numVertsTotal"] = asset->m_meshAssetData[i].m_numVertsTotal;
					data["meshdata"][i]["m_numTrisTotal"] = asset->m_meshAssetData[i].m_numTrisTotal;
				}
				break;
			}
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
			{
				auto* asset = &asset_->moduleData.initSpawnShapeSphere;

				DUMP_FIELD(m_axisFlags);
				DUMP_FIELD(m_spawnFlags);
				DUMP_FIELD(m_normalAxis);
				DUMP_FIELD(m_spawnType);
				DUMP_FIELD(m_volumeCubeRoot);

				DUMP_FIELD_ARR(m_offset.v, 4);
				DUMP_FIELD_ARR(m_offset2.v, 4);

				DUMP_FIELD(m_radius.min);
				DUMP_FIELD(m_radius.max);

				DUMP_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_INIT_TAIL:
			{
				auto* asset = &asset_->moduleData.initTail;

				DUMP_FIELD(m_averagePastVelocities);
				DUMP_FIELD(m_maxParentSpeed);
				DUMP_FIELD(m_tailLeading);
				DUMP_FIELD(m_scaleWithVelocity);
				DUMP_FIELD(m_rotateAroundPivot);

				break;
			}
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
			{
				auto* asset = &asset_->moduleData.initVectorField;

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_INIT_VOLUMETRIC:
			{
				auto* asset = &asset_->moduleData.initVolumetric;

				DUMP_FIELD(m_density);
				DUMP_FIELD(m_falloff);
				DUMP_FIELD(m_noiseAmplitude);
				DUMP_FIELD(m_noiseMode);

				DUMP_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_ATTRACTOR:
			{
				auto* asset = &asset_->moduleData.attractor;

				DUMP_FIELD(m_killWhenNear);
				DUMP_FIELD(m_forceMag);
				DUMP_FIELD(m_nearDistanceSq);
				DUMP_FIELD_ARR(m_attractPoint.v, 4);

				DUMP_FIELD_ARR(m_pad, 3);

				break;
			}
			case PARTICLE_MODULE_COLOR_GRAPH:
			{
				auto* asset = &asset_->moduleData.colorGraph;

				DUMP_FIELD(firstCurve);
				DUMP_FIELD(m_modulateColorByAlpha);

				DUMP_FIELD_ARR(m_pad, 3);

				for (auto i = 0; i < 8; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_COLOR_LERP:
			{
				auto* asset = &asset_->moduleData.colorLerp;

				DUMP_FIELD_ARR(m_colorBegin.v, 4);
				DUMP_FIELD_ARR(m_colorEnd.v, 4);

				DUMP_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_EMISSION_GRAPH:
			{
				auto* asset = &asset_->moduleData.emissionGraph;

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_EMISSIVE_GRAPH:
			{
				auto* asset = &asset_->moduleData.emissiveGraph;

				DUMP_FIELD(firstCurve);

				DUMP_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_FORCE:
			{
				auto* asset = &asset_->moduleData.force;

				DUMP_FIELD_ARR(m_forceMin.v, 4);
				DUMP_FIELD_ARR(m_forceMax.v, 4);

				break;
			}
			case PARTICLE_MODULE_GRAVITY:
			{
				auto* asset = &asset_->moduleData.gravity;

				DUMP_FIELD(m_gravityPercentage.min);
				DUMP_FIELD(m_gravityPercentage.max);

				break;
			}
			case PARTICLE_MODULE_INTENSITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.intensityGraph;

				DUMP_FIELD(firstCurve);

				DUMP_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}

			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.velocityGraph;

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_PHYSICS_LIGHT:
			{
				auto* asset = &asset_->moduleData.physicsLight;

				DUMP_FIELD(m_ignoreEmitterOrientation);
				DUMP_FIELD(m_useSurfaceType);

				DUMP_FIELD_ARR(m_pad, 6);

				dump_particle_linked_asset_list(asset->m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_PHYSICS_RAY_CAST:
			{
				auto* asset = &asset_->moduleData.physicsRayCast;

				DUMP_FIELD(m_bounce.min);
				DUMP_FIELD(m_bounce.max);
				DUMP_FIELD_ARR(m_bounds.midPoint, 3);
				DUMP_FIELD_ARR(m_bounds.halfSize, 3);
				DUMP_FIELD(m_useItemClip);
				DUMP_FIELD(m_useSurfaceType);
				DUMP_FIELD(m_collideWithWater);
				DUMP_FIELD(m_ignoreContentItem);

				DUMP_FIELD_ARR(m_pad, 3);

				break;
			}
			case PARTICLE_MODULE_POSITION_GRAPH:
			{
				auto* asset = &asset_->moduleData.positionGraph;

				for (auto i = 0; i < 6; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_ROTATION_GRAPH:
			{
				auto* asset = &asset_->moduleData.rotationGraph;

				DUMP_FIELD(m_useRotationRate);

				DUMP_FIELD_ARR(m_pad, 1);

				for (auto i = 0; i < 2; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}

			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
			{
				auto* asset = &asset_->moduleData.rotationGraph3D;

				DUMP_FIELD(m_useRotationRate);

				DUMP_FIELD_ARR(m_pad, 1);

				for (auto i = 0; i < 6; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_SIZE_GRAPH:
			{
				auto* asset = &asset_->moduleData.sizeGraph;

				DUMP_FIELD(firstCurve);
				DUMP_FIELD_ARR(m_sizeBegin.v, 4);
				DUMP_FIELD_ARR(m_sizeEnd.v, 4);

				DUMP_FIELD_ARR(m_pad, 7);

				for (auto i = 0; i < 6; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}

			case PARTICLE_MODULE_SIZE_LERP:
			{
				auto* asset = &asset_->moduleData.sizeLerp;

				DUMP_FIELD_ARR(m_sizeBegin.v, 4);
				DUMP_FIELD_ARR(m_sizeEnd.v, 4);

				DUMP_FIELD_ARR(m_pad, 2);

				break;
			}
			case PARTICLE_MODULE_VELOCITY_GRAPH:
			{
				auto* asset = &asset_->moduleData.velocityGraph;

				DUMP_FIELD_ARR(m_velocityBegin.v, 4);
				DUMP_FIELD_ARR(m_velocityEnd.v, 4);

				for (auto i = 0; i < 6; i++)
				{
					dump_curve(&asset->m_curves[i], data["Curves"][i]);
				}
				break;
			}
			case PARTICLE_MODULE_TEST_AGE:
			{
				auto* asset = &asset_->moduleData.testAge;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				DUMP_FIELD(m_age);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_BIRTH:
			{
				auto* asset = &asset_->moduleData.testBirth;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_DEATH:
			{
				auto* asset = &asset_->moduleData.testDeath;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_GRAVITY:
			{
				auto* asset = &asset_->moduleData.testGravity;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_IMPACT:
			{
				auto* asset = &asset_->moduleData.testPos;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			case PARTICLE_MODULE_TEST_POS:
			{
				auto* asset = &asset_->moduleData.testImpact;

				DUMP_FIELD(m_moduleIndex);
				DUMP_FIELD_ARR(m_useOrientationOptions, 1);
				DUMP_FIELD_ARR(m_useScaleOptions, 1);
				DUMP_FIELD_ARR(m_useVelocityOptions, 1);
				DUMP_FIELD(m_eventHandlerData.m_nextState);
				DUMP_FIELD(m_eventHandlerData.m_kill);
				DUMP_FIELD_ARR(m_eventHandlerData.m_pad, 1);

				dump_particle_linked_asset_list(asset->m_eventHandlerData.m_linkedAssetList, data["AssetList"]);
				break;
			}
			default:
				__debugbreak();
			}
		}

#undef DUMP_TYPE

		void dump_module_group(ParticleModuleGroupDef* asset, ordered_json& data)
		{
			DUMP_FIELD(disabled);
			for (auto i = 0; i < asset->numModules; i++)
			{
				dump_module(&asset->moduleDefs[i], data["Module"][i]);
			}
		}

		void dump_state(ParticleStateDef* asset, ordered_json& data)
		{
			data["elementType"] = PARTICLE_ELEMENT_TYPE_s[asset->elementType]; //DUMP_FIELD(elementType);
			DUMP_FIELD(flags);
			for (auto i = 0; i < 3; i++)
			{
				dump_module_group(&asset->moduleGroupDefs[i], data["ModuleGroup"][ParticleModuleGroup_s[i]]);
			}
		}

		void dump_emitter(ParticleEmitterDef* asset, ordered_json& data)
		{
			DUMP_FIELD(particleSpawnRate.min);
			DUMP_FIELD(particleSpawnRate.max);

			DUMP_FIELD(particleLife.min);
			DUMP_FIELD(particleLife.max);

			DUMP_FIELD(particleDelay.min);
			DUMP_FIELD(particleDelay.max);

			DUMP_FIELD(particleCountMax);

			DUMP_FIELD(particleBurstCount.min);
			DUMP_FIELD(particleBurstCount.max);

			DUMP_FIELD(emitterLife.min);
			DUMP_FIELD(emitterLife.max);

			DUMP_FIELD(emitterDelay.min);
			DUMP_FIELD(emitterDelay.max);

			DUMP_FIELD(randomSeed);

			DUMP_FIELD(spawnRangeSq.min);
			DUMP_FIELD(spawnRangeSq.max);

			DUMP_FIELD(fadeOutMaxDistance);

			dump_curve(&asset->fadeCurveDef, data["FadeCurve"]);

			DUMP_FIELD(spawnFrustumCullRadius);

			DUMP_FIELD(gravityOptions);

			DUMP_FIELD_ARR(groupIDs, 1);

			DUMP_FIELD(emitByDistanceDensity.min);
			DUMP_FIELD(emitByDistanceDensity.max);

			DUMP_FIELD(soloInstanceMax);
			DUMP_FIELD(instanceAction);

			DUMP_FIELD(particleSpawnShapeRange.min);
			DUMP_FIELD(particleSpawnShapeRange.max);

			DUMP_FIELD_ARR(pad, 1);

			DUMP_FIELD(m_dataFlags);
			DUMP_FIELD(flags);

			for (auto i = 0; i < asset->numStates; i++)
			{
				dump_state(&asset->stateDefs[i], data["State"][i]);
			}
		}

		void dump_scripted_input_node(ParticleScriptedInputNodeDef* asset, ordered_json& data)
		{
			DUMP_FIELD(nodeBase.m_type);
			DUMP_FIELD(nodeBase.m_flags);
			DUMP_FIELD(nodeBase.m_emitterIndex);

			DUMP_FIELD(KVP.m_inputRecord.key);
			DUMP_FIELD(KVP.m_inputRecord.dirty);
			DUMP_FIELD(KVP.m_inputRecord.value);
		}

		void dump(ParticleSystemDef* asset)
		{
			const auto path = "particlesystem\\"s + asset->name + ".json"s;

			ordered_json data;

			DUMP_STRING(name);

			DUMP_FIELD(version);
			DUMP_FIELD(flags);

			for (auto i = 0; i < asset->numEmitters; i++)
			{
				dump_emitter(&asset->emitterDefs[i], data["Emitter"][i]);
			}

			for (auto i = 0; i < asset->numScriptedInputNodes; i++)
			{
				dump_scripted_input_node(&asset->scriptedInputNodeDefs[i], data["scriptedInputNode"][i]);
			}

			DUMP_FIELD(occlusionOverrideEmitterIndex);
			DUMP_FIELD(phaseOptions);
			DUMP_FIELD(drawFrustumCullRadius);
			DUMP_FIELD(updateFrustumCullRadius);
			DUMP_FIELD(sunDistance);
			DUMP_FIELD(preRollMSec);

			DUMP_FIELD_ARR(editorPosition.v, 4);
			DUMP_FIELD_ARR(editorRotation.v, 4);
			DUMP_FIELD_ARR(gameTweakPosition.v, 4);
			DUMP_FIELD_ARR(gameTweakRotation.v, 4);

			std::string json_str = data.dump(4);
			auto file = filesystem::file(path);
			file.open("wb");
			file.write(json_str.data(), json_str.size(), 1);
			file.close();
		}
	}

	ParticleSystemDef* particle_system::parse(const std::string& name, zone_memory* mem)
	{
		ParticleSystemDef* asset = nullptr;
		asset = vfx::json::parse(name, mem);
		if (asset)
		{
			return asset;
		}

		return vfx::binary::parse(name, mem);
	}

	void particle_system::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).vfx;
		}
	}

	void particle_system::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void load_depening_particle_linked_asset_def(ParticleLinkedAssetDef* asset, zone_base* zone)
	{
		auto saved = parent_module;

		switch (parent_module->moduleType)
		{
		case PARTICLE_MODULE_INIT_MATERIAL:
			if (asset->material)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, asset->material->name);
			}
			break;
		case PARTICLE_MODULE_INIT_MODEL:
		case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
			if (asset->model)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, asset->model->name);
			}
			break;
		case PARTICLE_MODULE_PHYSICS_LIGHT:
			if (asset->physicsFXData.physicsFXPipeline)
			{
				zone->add_asset_of_type(ASSET_TYPE_PHYSICS_FX_PIPELINE, asset->physicsFXData.physicsFXPipeline->name);
			}
			if (asset->physicsFXData.physicsFXShape)
			{
				zone->add_asset_of_type(ASSET_TYPE_PHYSICS_FX_SHAPE, asset->physicsFXData.physicsFXShape->name);
			}
			break;
		case PARTICLE_MODULE_INIT_PARTICLE_SIM:
			if (asset->particleSim)
			{
				zone->add_asset_of_type(ASSET_TYPE_PARTICLE_SIM_ANIMATION, asset->particleSim->name);
			}
			break;
		case PARTICLE_MODULE_INIT_DECAL:
			if (asset->decal.materials[0])
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, asset->decal.materials[0]->name);
			}
			if (asset->decal.materials[1])
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, asset->decal.materials[1]->name);
			}
			if (asset->decal.materials[2])
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, asset->decal.materials[2]->name);
			}
			break;
		case PARTICLE_MODULE_INIT_SOUND:
			// asset->sound
			break;
		case PARTICLE_MODULE_INIT_VECTOR_FIELD:
			if (asset->vectorField)
			{
				zone->add_asset_of_type(ASSET_TYPE_VECTORFIELD, asset->vectorField->name);
			}
			break;
		case PARTICLE_MODULE_INIT_LIGHT_OMNI:
		case PARTICLE_MODULE_INIT_LIGHT_SPOT:
			if (asset->lightDef)
			{
				zone->add_asset_of_type(ASSET_TYPE_LIGHT_DEF, asset->lightDef->name);
			}
			break;
		case PARTICLE_MODULE_INIT_BEAM:
		case PARTICLE_MODULE_INIT_RUNNER:

		case PARTICLE_MODULE_TEST_AGE:
		case PARTICLE_MODULE_TEST_BIRTH:
		case PARTICLE_MODULE_TEST_DEATH:
		case PARTICLE_MODULE_TEST_GRAVITY:
		case PARTICLE_MODULE_TEST_IMPACT:
		case PARTICLE_MODULE_TEST_POS:
			if (asset->particleSystem)
			{
				zone->add_asset_of_type(ASSET_TYPE_VFX, asset->particleSystem->name);
			}
			break;
		default:
			__debugbreak();
			break;
		}

		parent_module = saved;
	}

	void load_depending_linked_asset_list(ParticleLinkedAssetListDef& asset, zone_base* zone)
	{
		if (asset.assetList)
		{
			for (auto i = 0; i < asset.numAssets; i++)
			{
				load_depening_particle_linked_asset_def(&asset.assetList[i], zone);
			}
		}
	}

	void load_depending_module(ParticleModuleDef* asset, zone_base* zone)
	{
		parent_module = asset;

		switch (asset->moduleType)
		{
		case PARTICLE_MODULE_INIT_BEAM:
			load_depending_linked_asset_list(asset->moduleData.initBeam.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_DECAL:
			load_depending_linked_asset_list(asset->moduleData.initDecal.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			load_depending_linked_asset_list(asset->moduleData.initLightOmni.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_LIGHT_SPOT:
			load_depending_linked_asset_list(asset->moduleData.initLightSpot.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_MATERIAL:
			load_depending_linked_asset_list(asset->moduleData.initMaterial.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_MODEL:
			load_depending_linked_asset_list(asset->moduleData.initModel.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_PARTICLE_SIM:
			load_depending_linked_asset_list(asset->moduleData.initParticleSim.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_RUNNER:
			load_depending_linked_asset_list(asset->moduleData.initRunner.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_SOUND:
			load_depending_linked_asset_list(asset->moduleData.initSound.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
			load_depending_linked_asset_list(asset->moduleData.initSpawnShapeMesh.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_INIT_VECTOR_FIELD:
			load_depending_linked_asset_list(asset->moduleData.initVectorField.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_PHYSICS_LIGHT:
			load_depending_linked_asset_list(asset->moduleData.physicsLight.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_TEST_AGE:
			load_depending_linked_asset_list(asset->moduleData.testAge.m_eventHandlerData.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_TEST_BIRTH:
			load_depending_linked_asset_list(asset->moduleData.testBirth.m_eventHandlerData.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_TEST_DEATH:
			load_depending_linked_asset_list(asset->moduleData.testDeath.m_eventHandlerData.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_TEST_GRAVITY:
			load_depending_linked_asset_list(asset->moduleData.testGravity.m_eventHandlerData.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_TEST_IMPACT:
			load_depending_linked_asset_list(asset->moduleData.testImpact.m_eventHandlerData.m_linkedAssetList, zone);
			break;
		case PARTICLE_MODULE_TEST_POS:
			load_depending_linked_asset_list(asset->moduleData.testPos.m_eventHandlerData.m_linkedAssetList, zone);
			break;
		}
	}

	void particle_system::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;
		if (asset->emitterDefs)
		{
			for (auto i = 0; i < asset->numEmitters; i++)
			{
				if (asset->emitterDefs[i].stateDefs)
				{
					for (auto ii = 0; ii < asset->emitterDefs[i].numStates; ii++)
					{
						if (asset->emitterDefs[i].stateDefs[ii].moduleGroupDefs)
						{
							for (auto iii = 0; iii < 3; iii++)
							{
								if (asset->emitterDefs[i].stateDefs[ii].moduleGroupDefs[iii].moduleDefs)
								{
									for (auto iiii = 0; iiii < asset->emitterDefs[i].stateDefs[ii].moduleGroupDefs[iii].numModules; iiii++)
									{
										load_depending_module(&asset->emitterDefs[i].stateDefs[ii].moduleGroupDefs[iii].moduleDefs[iiii], zone);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	std::string particle_system::name()
	{
		return this->name_;
	}

	std::int32_t particle_system::type()
	{
		return ASSET_TYPE_VFX;
	}

	void write_particle_linked_asset_def(ParticleLinkedAssetDef* data, ParticleLinkedAssetDef* dest, zone_base* zone, zone_buffer* buf)
	{
		switch (parent_module->moduleType)
		{
		case PARTICLE_MODULE_INIT_MATERIAL:
			if (data->material)
			{
				dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->material->name));
			}
			break;
		case PARTICLE_MODULE_INIT_MODEL:
		case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
			if (data->model)
			{
				dest->model = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->model->name));
			}
			break;
		case PARTICLE_MODULE_PHYSICS_LIGHT:
			if (data->physicsFXData.physicsFXPipeline)
			{
				dest->physicsFXData.physicsFXPipeline = reinterpret_cast<PhysicsFXPipeline*>(zone->get_asset_pointer(ASSET_TYPE_PHYSICS_FX_PIPELINE, data->physicsFXData.physicsFXPipeline->name));
			}
			if (data->physicsFXData.physicsFXShape)
			{
				dest->physicsFXData.physicsFXShape = reinterpret_cast<PhysicsFXShape*>(zone->get_asset_pointer(ASSET_TYPE_PHYSICS_FX_SHAPE, data->physicsFXData.physicsFXShape->name));
			}
			break;
		case PARTICLE_MODULE_INIT_PARTICLE_SIM:
			if (data->particleSim)
			{
				dest->particleSim = reinterpret_cast<FxParticleSimAnimation*>(zone->get_asset_pointer(ASSET_TYPE_PARTICLE_SIM_ANIMATION, data->particleSim->name));
			}
			break;
		case PARTICLE_MODULE_INIT_DECAL:
			if (data->decal.materials[0])
			{
				dest->decal.materials[0] = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->decal.materials[0]->name));
			}
			if (data->decal.materials[1])
			{
				dest->decal.materials[1] = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->decal.materials[1]->name));
			}
			if (data->decal.materials[2])
			{
				dest->decal.materials[2] = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->decal.materials[2]->name));
			}
			break;
		case PARTICLE_MODULE_INIT_SOUND:
			if (data->sound)
			{
				dest->sound = buf->write_str(data->sound);
			}
			break;
		case PARTICLE_MODULE_INIT_VECTOR_FIELD:
			if (data->vectorField)
			{
				dest->vectorField = reinterpret_cast<VectorField*>(zone->get_asset_pointer(ASSET_TYPE_VECTORFIELD, data->vectorField->name));
			}
			break;
		case PARTICLE_MODULE_INIT_LIGHT_OMNI:
		case PARTICLE_MODULE_INIT_LIGHT_SPOT:
			if (data->lightDef)
			{
				dest->lightDef = reinterpret_cast<GfxLightDef*>(zone->get_asset_pointer(ASSET_TYPE_LIGHT_DEF, data->lightDef->name));
			}
			break;
		case PARTICLE_MODULE_INIT_BEAM:
		case PARTICLE_MODULE_INIT_RUNNER:

		case PARTICLE_MODULE_TEST_AGE:
		case PARTICLE_MODULE_TEST_BIRTH:
		case PARTICLE_MODULE_TEST_DEATH:
		case PARTICLE_MODULE_TEST_GRAVITY:
		case PARTICLE_MODULE_TEST_IMPACT:
		case PARTICLE_MODULE_TEST_POS:
			if (data->particleSystem)
			{
				dest->particleSystem = reinterpret_cast<ParticleSystemDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->particleSystem->name));
			}
			break;
		default:
			__debugbreak();
			break;
		}
	}

	void write_particle_linked_asset_list(ParticleLinkedAssetListDef& data, ParticleLinkedAssetListDef& dest, zone_base* zone, zone_buffer* buf)
	{
		if (data.assetList)
		{
			buf->align(7);
			dest.assetList = buf->write(data.assetList, data.numAssets);
			for (auto i = 0; i < data.numAssets; i++)
			{
				write_particle_linked_asset_def(&data.assetList[i], &dest.assetList[i], zone, buf);
			}
			buf->clear_pointer(&dest.assetList);
		}
	}

	void write_module(ParticleModuleDef* data, ParticleModuleDef* dest, zone_base* zone, zone_buffer* buf)
	{
		parent_module = data;

		switch (data->moduleType)
		{
		case PARTICLE_MODULE_INIT_ATLAS:
			break;
		case PARTICLE_MODULE_INIT_ATTRIBUTES:
			break;
		case PARTICLE_MODULE_INIT_BEAM:
			write_particle_linked_asset_list(data->moduleData.initBeam.m_linkedAssetList, dest->moduleData.initBeam.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_CAMERA_OFFSET:
			break;
		case PARTICLE_MODULE_INIT_CLOUD:
			for (auto i = 0; i < 2; i++)
			{
				if (data->moduleData.initCloud.curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.initCloud.curves[i].controlPoints, data->moduleData.initCloud.curves->numControlPoints);
					buf->clear_pointer(&dest->moduleData.initCloud.curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_INIT_DECAL:
			write_particle_linked_asset_list(data->moduleData.initDecal.m_linkedAssetList, dest->moduleData.initDecal.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_FLARE:
			for (auto i = 0; i < 4; i++)
			{
				if (data->moduleData.initFlare.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.initFlare.m_curves[i].controlPoints, data->moduleData.initFlare.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.initFlare.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_INIT_GEO_TRAIL:
			break;
		case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			write_particle_linked_asset_list(data->moduleData.initLightOmni.m_linkedAssetList, dest->moduleData.initLightOmni.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_LIGHT_SPOT:
			write_particle_linked_asset_list(data->moduleData.initLightSpot.m_linkedAssetList, dest->moduleData.initLightSpot.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_MATERIAL:
			write_particle_linked_asset_list(data->moduleData.initMaterial.m_linkedAssetList, dest->moduleData.initMaterial.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_MIRROR_TEXTURE:
			break;
		case PARTICLE_MODULE_INIT_MODEL:
			write_particle_linked_asset_list(data->moduleData.initModel.m_linkedAssetList, dest->moduleData.initModel.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_OCCLUSION_QUERY:
			break;
		case PARTICLE_MODULE_INIT_ORIENTED_SPRITE:
			break;
		case PARTICLE_MODULE_INIT_PARTICLE_SIM:
			write_particle_linked_asset_list(data->moduleData.initParticleSim.m_linkedAssetList, dest->moduleData.initParticleSim.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_PLAYER_FACING:
			break;
		case PARTICLE_MODULE_INIT_RELATIVE_VELOCITY:
			break;
		case PARTICLE_MODULE_INIT_ROTATION:
			break;
		case PARTICLE_MODULE_INIT_ROTATION_3D:
			break;
		case PARTICLE_MODULE_INIT_RUNNER:
			write_particle_linked_asset_list(data->moduleData.initRunner.m_linkedAssetList, dest->moduleData.initRunner.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_SOUND:
			write_particle_linked_asset_list(data->moduleData.initSound.m_linkedAssetList, dest->moduleData.initSound.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_SPAWN:
			for (auto i = 0; i < 1; i++)
			{
				if (data->moduleData.initSpawn.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.initSpawn.m_curves[i].controlPoints, data->moduleData.initSpawn.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.initSpawn.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
			break;
		case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
			break;
		case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
			break;
		case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
			write_particle_linked_asset_list(data->moduleData.initSpawnShapeMesh.m_linkedAssetList, dest->moduleData.initSpawnShapeMesh.m_linkedAssetList, zone, buf);
			if (data->moduleData.initSpawnShapeMesh.m_meshAssetData)
			{
				buf->align(3);
				buf->write(data->moduleData.initSpawnShapeMesh.m_meshAssetData, data->moduleData.initSpawnShapeMesh.m_numMeshAssets);
				buf->clear_pointer(&dest->moduleData.initSpawnShapeMesh.m_meshAssetData);
			}
			break;
		case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
			break;
		case PARTICLE_MODULE_INIT_TAIL:
			break;
		case PARTICLE_MODULE_INIT_VECTOR_FIELD:
			write_particle_linked_asset_list(data->moduleData.initVectorField.m_linkedAssetList, dest->moduleData.initVectorField.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_INIT_VOLUMETRIC:
			break;
		case PARTICLE_MODULE_ATTRACTOR:
			break;
		case PARTICLE_MODULE_COLOR_GRAPH:
			for (auto i = 0; i < 8; i++)
			{
				if (data->moduleData.colorGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.colorGraph.m_curves[i].controlPoints, data->moduleData.colorGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.colorGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_COLOR_LERP:
			break;
		case PARTICLE_MODULE_EMISSION_GRAPH:
			for (auto i = 0; i < 2; i++)
			{
				if (data->moduleData.emissionGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.emissionGraph.m_curves[i].controlPoints, data->moduleData.emissionGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.emissionGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_EMISSIVE_GRAPH:
			for (auto i = 0; i < 2; i++)
			{
				if (data->moduleData.emissiveGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.emissiveGraph.m_curves[i].controlPoints, data->moduleData.emissiveGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.emissiveGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_FORCE:
			break;
		case PARTICLE_MODULE_GRAVITY:
			break;
		case PARTICLE_MODULE_INTENSITY_GRAPH:
			for (auto i = 0; i < 2; i++)
			{
				if (data->moduleData.intensityGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.intensityGraph.m_curves[i].controlPoints, data->moduleData.intensityGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.intensityGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
			for (auto i = 0; i < 2; i++)
			{
				if (data->moduleData.parentVelocityGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.parentVelocityGraph.m_curves[i].controlPoints, data->moduleData.parentVelocityGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.parentVelocityGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_PHYSICS_LIGHT:
			write_particle_linked_asset_list(data->moduleData.physicsLight.m_linkedAssetList, dest->moduleData.physicsLight.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_PHYSICS_RAY_CAST:
			break;
		case PARTICLE_MODULE_POSITION_GRAPH:
			for (auto i = 0; i < 6; i++)
			{
				if (data->moduleData.positionGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.positionGraph.m_curves[i].controlPoints, data->moduleData.positionGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.positionGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_ROTATION_GRAPH:
			for (auto i = 0; i < 2; i++)
			{
				if (data->moduleData.rotationGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.rotationGraph.m_curves[i].controlPoints, data->moduleData.rotationGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.rotationGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_ROTATION_GRAPH_3D:
			for (auto i = 0; i < 6; i++)
			{
				if (data->moduleData.rotationGraph3D.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.rotationGraph3D.m_curves[i].controlPoints, data->moduleData.rotationGraph3D.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.rotationGraph3D.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_SIZE_GRAPH:
			for (auto i = 0; i < 6; i++)
			{
				if (data->moduleData.sizeGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.sizeGraph.m_curves[i].controlPoints, data->moduleData.sizeGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.sizeGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_SIZE_LERP:
			break;
		case PARTICLE_MODULE_VELOCITY_GRAPH:
			for (auto i = 0; i < 6; i++)
			{
				if (data->moduleData.velocityGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.velocityGraph.m_curves[i].controlPoints, data->moduleData.velocityGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.velocityGraph.m_curves[i].controlPoints);
				}
			}
			break;
		case PARTICLE_MODULE_TEST_AGE:
			write_particle_linked_asset_list(data->moduleData.testAge.m_eventHandlerData.m_linkedAssetList, dest->moduleData.testAge.m_eventHandlerData.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_TEST_BIRTH:
			write_particle_linked_asset_list(data->moduleData.testBirth.m_eventHandlerData.m_linkedAssetList, dest->moduleData.testBirth.m_eventHandlerData.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_TEST_DEATH:
			write_particle_linked_asset_list(data->moduleData.testDeath.m_eventHandlerData.m_linkedAssetList, dest->moduleData.testDeath.m_eventHandlerData.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_TEST_GRAVITY:
			write_particle_linked_asset_list(data->moduleData.testGravity.m_eventHandlerData.m_linkedAssetList, dest->moduleData.testGravity.m_eventHandlerData.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_TEST_IMPACT:
			write_particle_linked_asset_list(data->moduleData.testImpact.m_eventHandlerData.m_linkedAssetList, dest->moduleData.testImpact.m_eventHandlerData.m_linkedAssetList, zone, buf);
			break;
		case PARTICLE_MODULE_TEST_POS:
			write_particle_linked_asset_list(data->moduleData.testPos.m_eventHandlerData.m_linkedAssetList, dest->moduleData.testPos.m_eventHandlerData.m_linkedAssetList, zone, buf);
			break;
		}
	}

	void write_module_group(ParticleModuleGroupDef* data, ParticleModuleGroupDef* dest, zone_base* zone, zone_buffer* buf)
	{
		if (data->moduleDefs)
		{
			buf->align(15);
			dest->moduleDefs = buf->write(data->moduleDefs, data->numModules);
			for (auto i = 0; i < data->numModules; i++)
			{
				write_module(&data->moduleDefs[i], &dest->moduleDefs[i], zone, buf);
			}
			buf->clear_pointer(&dest->moduleDefs);
		}
	}

	void write_state(ParticleStateDef* data, ParticleStateDef* dest, zone_base* zone, zone_buffer* buf)
	{
		if (data->moduleGroupDefs)
		{
			buf->align(15);
			dest->moduleGroupDefs = buf->write(data->moduleGroupDefs, 3);
			for (auto i = 0; i < 3; i++)
			{
				write_module_group(&data->moduleGroupDefs[i], &dest->moduleGroupDefs[i], zone, buf);
			}
			buf->clear_pointer(&dest->moduleGroupDefs);
		}
	}

	void write_emitter(ParticleEmitterDef* data, ParticleEmitterDef* dest, zone_base* zone, zone_buffer* buf)
	{
		if (data->stateDefs)
		{
			buf->align(15);
			dest->stateDefs = buf->write(data->stateDefs, data->numStates);
			for (auto i = 0; i < data->numStates; i++)
			{
				write_state(&data->stateDefs[i], &dest->stateDefs[i], zone, buf);
			}
			buf->clear_pointer(&dest->stateDefs);
		}

		if (data->fadeCurveDef.controlPoints)
		{
			buf->align(15);
			buf->write(data->fadeCurveDef.controlPoints, data->fadeCurveDef.numControlPoints);
			buf->clear_pointer(&dest->fadeCurveDef.controlPoints);
		}
	}

	void particle_system::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->emitterDefs)
		{
			buf->align(15);
			dest->emitterDefs = buf->write(data->emitterDefs, data->numEmitters);
			for (auto i = 0; i < data->numEmitters; i++)
			{
				write_emitter(&data->emitterDefs[i], &dest->emitterDefs[i], zone, buf);
			}
			buf->clear_pointer(&dest->emitterDefs);
		}

		if (data->scriptedInputNodeDefs)
		{
			buf->align(15);
			buf->write(data->scriptedInputNodeDefs, data->numScriptedInputNodes);
			buf->clear_pointer(&dest->scriptedInputNodeDefs);
		}

		buf->pop_stream();
	}

	void particle_system::dump(ParticleSystemDef* asset)
	{
#ifdef DUMP_JSON
		vfx::json::dump(asset);
#else
		vfx::binary::dump(asset);
#endif
	}
}