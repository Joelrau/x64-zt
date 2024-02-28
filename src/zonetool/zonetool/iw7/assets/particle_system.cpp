#include "std_include.hpp"
#include "particle_system.hpp"

namespace zonetool::iw7
{
	ParticleModuleDef* parent_module;

	void read_particle_linked_asset_def(ParticleLinkedAssetDef* asset, assetmanager::reader& read)
	{
		switch (parent_module->type)
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

		case PARTICLE_MODULE_TEST_BIRTH:
		case PARTICLE_MODULE_TEST_DEATH:
		case PARTICLE_MODULE_TEST_GRAVITY:
		case PARTICLE_MODULE_TEST_IMPACT:
		case PARTICLE_MODULE_TEST_POS:
		case PARTICLE_MODULE_TEST_TIME_IN_STATE:
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

		switch (asset->type)
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
		case PARTICLE_MODULE_TEMPERATURE_GRAPH:
			for (auto i = 0; i < 2; i++)
			{
				asset->moduleData.temperatureGraph.m_curves[i].controlPoints = read.read_array<ParticleCurveControlPointDef>();
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
		case PARTICLE_MODULE_TEST_TIME_IN_STATE:
			read_particle_linked_asset_list(asset->moduleData.testTimeInState.m_eventHandlerData.m_linkedAssetList, read);
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

	ParticleSystemDef* particle_system::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = name + ".iw7VFX"s;
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

		switch (parent_module->type)
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

		case PARTICLE_MODULE_TEST_BIRTH:
		case PARTICLE_MODULE_TEST_DEATH:
		case PARTICLE_MODULE_TEST_GRAVITY:
		case PARTICLE_MODULE_TEST_IMPACT:
		case PARTICLE_MODULE_TEST_POS:
		case PARTICLE_MODULE_TEST_TIME_IN_STATE:
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

		switch (asset->type)
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
		case PARTICLE_MODULE_TEST_TIME_IN_STATE:
			load_depending_linked_asset_list(asset->moduleData.testTimeInState.m_eventHandlerData.m_linkedAssetList, zone);
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
		switch (parent_module->type)
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

		case PARTICLE_MODULE_TEST_BIRTH:
		case PARTICLE_MODULE_TEST_DEATH:
		case PARTICLE_MODULE_TEST_GRAVITY:
		case PARTICLE_MODULE_TEST_IMPACT:
		case PARTICLE_MODULE_TEST_POS:
		case PARTICLE_MODULE_TEST_TIME_IN_STATE:
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

		switch (data->type)
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
		case PARTICLE_MODULE_TEMPERATURE_GRAPH:
			for (auto i = 0; i < 2; i++)
			{
				if (data->moduleData.temperatureGraph.m_curves[i].controlPoints)
				{
					buf->align(15);
					buf->write(data->moduleData.temperatureGraph.m_curves[i].controlPoints, data->moduleData.temperatureGraph.m_curves[i].numControlPoints);
					buf->clear_pointer(&dest->moduleData.temperatureGraph.m_curves[i].controlPoints);
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
		case PARTICLE_MODULE_TEST_TIME_IN_STATE:
			write_particle_linked_asset_list(data->moduleData.testTimeInState.m_eventHandlerData.m_linkedAssetList, dest->moduleData.testTimeInState.m_eventHandlerData.m_linkedAssetList, zone, buf);
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

	void dump_particle_linked_asset_def(ParticleLinkedAssetDef* asset, assetmanager::dumper& dump)
	{
		switch (parent_module->type)
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

		case PARTICLE_MODULE_TEST_BIRTH:
		case PARTICLE_MODULE_TEST_DEATH:
		case PARTICLE_MODULE_TEST_GRAVITY:
		case PARTICLE_MODULE_TEST_IMPACT:
		case PARTICLE_MODULE_TEST_POS:
		case PARTICLE_MODULE_TEST_TIME_IN_STATE:
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

		switch (asset->type)
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
		case PARTICLE_MODULE_TEMPERATURE_GRAPH:
			for (auto i = 0; i < 2; i++)
			{
				dump.dump_array(asset->moduleData.temperatureGraph.m_curves[i].controlPoints, asset->moduleData.temperatureGraph.m_curves[i].numControlPoints);
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
		case PARTICLE_MODULE_TEST_TIME_IN_STATE:
			dump_particle_linked_asset_list(asset->moduleData.testTimeInState.m_eventHandlerData.m_linkedAssetList, dump);
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

	void particle_system::dump(ParticleSystemDef* asset)
	{
		assetmanager::dumper dump;

		const auto path = asset->name + ".iw7VFX"s;
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