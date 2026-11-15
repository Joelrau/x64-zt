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

		ParticleLinkedAssetListDef* module_linked_assets(ParticleModuleDef& module)
		{
			auto& data = module.moduleData;

			switch (module.moduleType)
			{
			case PARTICLE_MODULE_INIT_BEAM:
				return &data.initBeam.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_DECAL:
				return &data.initDecal.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
				return &data.initLightOmni.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				return &data.initLightSpot.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_MATERIAL:
				return &data.initMaterial.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_MODEL:
				return &data.initModel.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				return &data.initParticleSim.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_RUNNER:
				return &data.initRunner.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_SOUND:
				return &data.initSound.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				return &data.initSpawnShapeMesh.m_linkedAssetList;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				return &data.initVectorField.m_linkedAssetList;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				return &data.physicsLight.m_linkedAssetList;
			default:
				return is_test_module(module.moduleType) ? &data.testAge.m_eventHandlerData.m_linkedAssetList : nullptr;
			}
		}

		std::span<ParticleCurveDef> module_curves(ParticleModuleDef& module)
		{
			auto& data = module.moduleData;

			switch (module.moduleType)
			{
			case PARTICLE_MODULE_INIT_ATLAS:
				return data.initAtlas.m_curves;
			case PARTICLE_MODULE_INIT_FLARE:
				return data.initFlare.m_curves;
			case PARTICLE_MODULE_INIT_SPAWN:
				return data.initSpawn.m_curves;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_BOX:
				return data.initSpawnShapeBox.m_curves;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_CYLINDER:
				return data.initSpawnShapeCylinder.m_curves;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_ELLIPSOID:
				return data.initSpawnShapeEllipsoid.m_curves;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				return data.initSpawnShapeMesh.m_curves;
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_SPHERE:
				return data.initSpawnShapeSphere.m_curves;
			case PARTICLE_MODULE_COLOR_GRAPH:
				return data.colorGraph.m_curves;
			case PARTICLE_MODULE_EMISSION_GRAPH:
				return data.emissionGraph.m_curves;
			case PARTICLE_MODULE_EMISSIVE_GRAPH:
				return data.emissiveGraph.m_curves;
			case PARTICLE_MODULE_SHADER_GRAPH:
				return data.shaderGraph.m_curves;
			case PARTICLE_MODULE_FORCE_DRAG_GRAPH:
				return data.forceDragGraph.m_curves;
			case PARTICLE_MODULE_INTENSITY_GRAPH:
				return data.intensityGraph.m_curves;
			case PARTICLE_MODULE_TEMPERATURE_GRAPH:
				return data.temperatureGraph.m_curves;
			case PARTICLE_MODULE_PARENT_VELOCITY_GRAPH:
				return data.parentVelocityGraph.m_curves;
			case PARTICLE_MODULE_POSITION_GRAPH:
				return data.positionGraph.m_curves;
			case PARTICLE_MODULE_ROTATION_GRAPH:
				return data.rotationGraph.m_curves;
			case PARTICLE_MODULE_ROTATION_GRAPH_3D:
				return data.rotationGraph3D.m_curves;
			case PARTICLE_MODULE_SCALE_BY_DISTANCE:
				return data.scaleByDistance.m_curves;
			case PARTICLE_MODULE_SIZE_GRAPH:
				return data.sizeGraph.m_curves;
			case PARTICLE_MODULE_VELOCITY_GRAPH:
				return data.velocityGraph.m_curves;
			case PARTICLE_MODULE_VECTOR_FIELD_GRAPH:
				return data.vectorFieldGraph.m_curves;
			default:
				return {};
			}
		}

		template <typename AssetVisitor, typename StringVisitor>
		void visit_linked_asset(ParticleLinkedAssetDef& asset, const ParticleModuleType type, AssetVisitor&& visit_asset,
			StringVisitor&& visit_string)
		{
			switch (type)
			{
			case PARTICLE_MODULE_INIT_MATERIAL:
				visit_asset(ASSET_TYPE_MATERIAL, asset.material);
				break;
			case PARTICLE_MODULE_INIT_MODEL:
			case PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH:
				visit_asset(ASSET_TYPE_XMODEL, asset.model);
				break;
			case PARTICLE_MODULE_PHYSICS_LIGHT:
				visit_asset(ASSET_TYPE_PHYSICS_FX_PIPELINE, asset.physicsFXData.physicsFXPipeline);
				visit_asset(ASSET_TYPE_PHYSICS_FX_SHAPE, asset.physicsFXData.physicsFXShape);
				break;
			case PARTICLE_MODULE_INIT_PARTICLE_SIM:
				visit_asset(ASSET_TYPE_PARTICLE_SIM_ANIMATION, asset.particleSim);
				break;
			case PARTICLE_MODULE_INIT_DECAL:
				for (auto& material : asset.decal.materials)
				{
					visit_asset(ASSET_TYPE_MATERIAL, material);
				}
				break;
			case PARTICLE_MODULE_INIT_SOUND:
				visit_string(asset.sound);
				break;
			case PARTICLE_MODULE_INIT_VECTOR_FIELD:
				visit_asset(ASSET_TYPE_VECTORFIELD, asset.vectorField);
				break;
			case PARTICLE_MODULE_INIT_LIGHT_OMNI:
			case PARTICLE_MODULE_INIT_LIGHT_SPOT:
				visit_asset(ASSET_TYPE_LIGHT_DEF, asset.lightDef);
				break;
			case PARTICLE_MODULE_INIT_BEAM:
			case PARTICLE_MODULE_INIT_RUNNER:
				visit_asset(ASSET_TYPE_VFX, asset.particleSystem);
				break;
			default:
				if (is_test_module(type))
				{
					visit_asset(ASSET_TYPE_VFX, asset.particleSystem);
				}
				break;
			}
		}

		template <typename AssetVisitor>
		void visit_assets(ParticleSystemDef* asset, AssetVisitor&& visit_asset)
		{
			const auto ignore_string = [](const char*) {};

			for (auto i = 0; asset->emitterDefs && i < asset->numEmitters; i++)
			{
				const auto& emitter = asset->emitterDefs[i];
				for (auto j = 0; emitter.stateDefs && j < emitter.numStates; j++)
				{
					const auto* groups = emitter.stateDefs[j].moduleGroupDefs;
					for (auto k = 0; groups && k < 3; k++)
					{
						for (auto m = 0; groups[k].moduleDefs && m < groups[k].numModules; m++)
						{
							auto& module = groups[k].moduleDefs[m];
							const auto* list = module_linked_assets(module);
							for (auto n = 0; list && list->assetList && n < list->numAssets; n++)
							{
								visit_linked_asset(list->assetList[n], module.moduleType, visit_asset, ignore_string);
							}
						}
					}
				}
			}
		}

		void read_curves(zone_reader& reader, const std::span<ParticleCurveDef> curves)
		{
			for (auto& curve : curves)
			{
				reader.read_array(curve.controlPoints, 15, curve.numControlPoints);
			}
		}

		void read_module(zone_reader& reader, ParticleModuleDef& module)
		{
			if (const auto list = module_linked_assets(module))
			{
				if (const auto assets = reader.read_array(list->assetList, 7, list->numAssets))
				{
					for (auto i = 0; i < list->numAssets; i++)
					{
						visit_linked_asset(assets[i], module.moduleType,
							[&](const XAssetType type, auto& field) { reader.read_asset(type, field); },
							[&](const char*& field) { reader.read_string(field); });
					}
				}
			}

			if (module.moduleType == PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH)
			{
				auto& mesh = module.moduleData.initSpawnShapeMesh;
				reader.read_array(mesh.m_meshAssetData, 3, mesh.m_numMeshAssets);
			}

			read_curves(reader, module_curves(module));
		}

		void read_emitter(zone_reader& reader, ParticleEmitterDef& emitter)
		{
			if (const auto states = reader.read_array(emitter.stateDefs, 15, emitter.numStates))
			{
				for (auto i = 0; i < emitter.numStates; i++)
				{
					if (const auto groups = reader.read_array(states[i].moduleGroupDefs, 15, 3))
					{
						for (auto j = 0; j < 3; j++)
						{
							if (const auto modules = reader.read_array(groups[j].moduleDefs, 15, groups[j].numModules))
							{
								for (auto k = 0; k < groups[j].numModules; k++)
								{
									read_module(reader, modules[k]);
								}
							}
						}
					}
				}
			}

			reader.read_array(emitter.fadeCurveDef.controlPoints, 15, emitter.fadeCurveDef.numControlPoints);
		}

		void write_curves(zone_buffer* buf, const std::span<ParticleCurveDef> curves)
		{
			for (auto& curve : curves)
			{
				write_array(buf, curve.controlPoints, curve.numControlPoints, 15);
			}
		}

		void write_module(zone_base* zone, zone_buffer* buf, ParticleModuleDef& module)
		{
			if (const auto list = module_linked_assets(module))
			{
				if (const auto assets = write_array(buf, list->assetList, list->numAssets, 7))
				{
					for (auto i = 0; i < list->numAssets; i++)
					{
						visit_linked_asset(assets[i], module.moduleType,
							[&](const XAssetType type, auto& field) { write_asset(zone, type, field); },
							[&](const char*& field) { write_string(buf, field); });
					}
				}
			}

			if (module.moduleType == PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH)
			{
				auto& mesh = module.moduleData.initSpawnShapeMesh;
				write_array(buf, mesh.m_meshAssetData, mesh.m_numMeshAssets, 3);
			}

			write_curves(buf, module_curves(module));
		}

		void write_emitter(zone_base* zone, zone_buffer* buf, ParticleEmitterDef& emitter)
		{
			if (const auto states = write_array(buf, emitter.stateDefs, emitter.numStates, 15))
			{
				for (auto i = 0; i < emitter.numStates; i++)
				{
					if (const auto groups = write_array(buf, states[i].moduleGroupDefs, 3, 15))
					{
						for (auto j = 0; j < 3; j++)
						{
							if (const auto modules = write_array(buf, groups[j].moduleDefs, groups[j].numModules, 15))
							{
								for (auto k = 0; k < groups[j].numModules; k++)
								{
									write_module(zone, buf, modules[k]);
								}
							}
						}
					}
				}
			}

			write_array(buf, emitter.fadeCurveDef.controlPoints, emitter.fadeCurveDef.numControlPoints, 15);
		}

		void dump_curves(assetmanager::dumper& dump, const std::span<ParticleCurveDef> curves)
		{
			for (const auto& curve : curves)
			{
				dump.dump_array(curve.controlPoints, curve.numControlPoints);
			}
		}

		void dump_module(assetmanager::dumper& dump, ParticleModuleDef& module)
		{
			if (const auto list = module_linked_assets(module))
			{
				dump.dump_array(list->assetList, list->numAssets);
				for (auto i = 0; list->assetList && i < list->numAssets; i++)
				{
					visit_linked_asset(list->assetList[i], module.moduleType,
						[&](XAssetType, auto& field) { dump.dump_asset(field); },
						[&](const char*& field) { dump.dump_string(field); });
				}
			}

			if (module.moduleType == PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH)
			{
				const auto& mesh = module.moduleData.initSpawnShapeMesh;
				dump.dump_array(mesh.m_meshAssetData, mesh.m_numMeshAssets);
			}

			dump_curves(dump, module_curves(module));
		}

		void dump_emitter(assetmanager::dumper& dump, const ParticleEmitterDef& emitter)
		{
			dump.dump_array(emitter.stateDefs, emitter.numStates);
			for (auto i = 0; emitter.stateDefs && i < emitter.numStates; i++)
			{
				const auto* groups = emitter.stateDefs[i].moduleGroupDefs;
				dump.dump_array(groups, 3);
				for (auto j = 0; groups && j < 3; j++)
				{
					dump.dump_array(groups[j].moduleDefs, groups[j].numModules);
					for (auto k = 0; groups[j].moduleDefs && k < groups[j].numModules; k++)
					{
						dump_module(dump, groups[j].moduleDefs[k]);
					}
				}
			}

			dump.dump_array(emitter.fadeCurveDef.controlPoints, emitter.fadeCurveDef.numControlPoints);
		}

		void parse_curves(assetmanager::reader& read, const std::span<ParticleCurveDef> curves)
		{
			for (auto& curve : curves)
			{
				curve.controlPoints = read.read_array<ParticleCurveControlPointDef>();
			}
		}

		void parse_module(assetmanager::reader& read, ParticleModuleDef& module)
		{
			if (const auto list = module_linked_assets(module))
			{
				list->assetList = read.read_array<ParticleLinkedAssetDef>();
				for (auto i = 0; list->assetList && i < list->numAssets; i++)
				{
					visit_linked_asset(list->assetList[i], module.moduleType,
						[&](XAssetType, auto& field) { parse_asset(read, field); },
						[&](const char*& field) { field = read.read_string(); });
				}
			}

			if (module.moduleType == PARTICLE_MODULE_INIT_SPAWN_SHAPE_MESH)
			{
				module.moduleData.initSpawnShapeMesh.m_meshAssetData = read.read_array<ParticleSpawnMeshAssetDef>();
			}

			parse_curves(read, module_curves(module));
		}

		void parse_emitter(assetmanager::reader& read, ParticleEmitterDef& emitter)
		{
			emitter.stateDefs = read.read_array<ParticleStateDef>();
			for (auto i = 0; emitter.stateDefs && i < emitter.numStates; i++)
			{
				auto* groups = read.read_array<ParticleModuleGroupDef>();
				emitter.stateDefs[i].moduleGroupDefs = groups;
				for (auto j = 0; groups && j < 3; j++)
				{
					groups[j].moduleDefs = read.read_array<ParticleModuleDef>();
					for (auto k = 0; groups[j].moduleDefs && k < groups[j].numModules; k++)
					{
						parse_module(read, groups[j].moduleDefs[k]);
					}
				}
			}

			emitter.fadeCurveDef.controlPoints = read.read_array<ParticleCurveControlPointDef>();
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

	void vfx::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (const auto emitters = write_array(buf, dest->emitterDefs, data->numEmitters, 15))
		{
			for (auto i = 0; i < data->numEmitters; i++)
			{
				write_emitter(zone, buf, emitters[i]);
			}
		}

		write_array(buf, dest->scriptedInputNodeDefs, data->numScriptedInputNodes, 15);
		buf->pop_stream();
	}

	void vfx::dump(ParticleSystemDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("vfx\\"s + asset->name + ".vfxb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->emitterDefs, asset->numEmitters);
		for (auto i = 0; asset->emitterDefs && i < asset->numEmitters; i++)
		{
			dump_emitter(dump, asset->emitterDefs[i]);
		}

		dump.dump_array(asset->scriptedInputNodeDefs, asset->numScriptedInputNodes);
		dump.close();
	}

	ParticleSystemDef* vfx::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("vfx\\"s + name + ".vfxb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vfx \"%s\"...", name.data());

		auto* asset = read.read_single<ParticleSystemDef>();
		asset->name = read.read_string();
		asset->emitterDefs = read.read_array<ParticleEmitterDef>();
		for (auto i = 0; asset->emitterDefs && i < asset->numEmitters; i++)
		{
			parse_emitter(read, asset->emitterDefs[i]);
		}

		asset->scriptedInputNodeDefs = read.read_array<ParticleScriptedInputNodeDef>();
		read.close();
		return asset;
	}

	void vfx::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<ParticleSystemDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void vfx::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		visit_assets(this->asset_, [&](const XAssetType type, auto* field) { add_asset(zone, type, field); });
	}

	std::string vfx::name()
	{
		return this->name_;
	}

	std::int32_t vfx::type()
	{
		return ASSET_TYPE_VFX;
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

	void vector_field::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (const auto fields = write_array(buf, dest->subFields, data->numSubFields, 15))
		{
			for (auto i = 0u; i < data->numSubFields; i++)
			{
				const auto& entries = fields[i].numEntries;
				write_array(buf, fields[i].linearData, entries[0] * entries[1] * entries[2], 3);
			}
		}
		buf->pop_stream();
	}

	void vector_field::dump(VectorField* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("vectorfield\\"s + asset->name + ".vectorfieldb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->subFields, asset->numSubFields);
		for (auto i = 0u; asset->subFields && i < asset->numSubFields; i++)
		{
			const auto& entries = asset->subFields[i].numEntries;
			dump.dump_array(asset->subFields[i].linearData, entries[0] * entries[1] * entries[2]);
		}
		dump.close();
	}

	VectorField* vector_field::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("vectorfield\\"s + name + ".vectorfieldb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vectorfield \"%s\"...", name.data());

		auto* asset = read.read_single<VectorField>();
		asset->name = read.read_string();
		asset->subFields = read.read_array<VectorSubField>();
		for (auto i = 0u; asset->subFields && i < asset->numSubFields; i++)
		{
			asset->subFields[i].linearData = read.read_array<vec4_t>();
		}
		read.close();
		return asset;
	}

	void vector_field::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<VectorField>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string vector_field::name()
	{
		return this->name_;
	}

	std::int32_t vector_field::type()
	{
		return ASSET_TYPE_VECTORFIELD;
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

	void footstep_vfx::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		for (auto& effect : dest->footstepVFX)
		{
			write_asset(zone, ASSET_TYPE_VFX, effect.particleSystemDef);
		}

		for (auto& effect : dest->smallFootstepVFX)
		{
			write_asset(zone, ASSET_TYPE_VFX, effect.particleSystemDef);
		}
		buf->pop_stream();
	}

	void footstep_vfx::dump(FootstepVFX* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("footstepvfx\\"s + asset->name + ".footstepvfxb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		for (const auto& effect : asset->footstepVFX)
		{
			dump.dump_asset(effect.particleSystemDef);
		}

		for (const auto& effect : asset->smallFootstepVFX)
		{
			dump.dump_asset(effect.particleSystemDef);
		}
		dump.close();
	}

	FootstepVFX* footstep_vfx::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("footstepvfx\\"s + name + ".footstepvfxb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing footstepvfx \"%s\"...", name.data());

		auto* asset = read.read_single<FootstepVFX>();
		asset->name = read.read_string();
		for (auto& effect : asset->footstepVFX)
		{
			parse_asset(read, effect.particleSystemDef);
		}

		for (auto& effect : asset->smallFootstepVFX)
		{
			parse_asset(read, effect.particleSystemDef);
		}
		read.close();
		return asset;
	}

	void footstep_vfx::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<FootstepVFX>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void footstep_vfx::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		for (const auto& effect : this->asset_->footstepVFX)
		{
			add_asset(zone, ASSET_TYPE_VFX, effect.particleSystemDef);
		}

		for (const auto& effect : this->asset_->smallFootstepVFX)
		{
			add_asset(zone, ASSET_TYPE_VFX, effect.particleSystemDef);
		}
	}

	std::string footstep_vfx::name()
	{
		return this->name_;
	}

	std::int32_t footstep_vfx::type()
	{
		return ASSET_TYPE_FOOTSTEP_VFX;
	}
}
