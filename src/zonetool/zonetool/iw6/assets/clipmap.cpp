#include <std_include.hpp>
#include "clipmap.hpp"

namespace zonetool::iw6
{
	void clip_map::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* clip_map::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	void clip_map::parse_info(ClipInfo* info, assetmanager::reader& read, zone_memory* mem)
	{
		info->planes = read.read_array<cplane_s>();
		info->materials = read.read_array<ClipMaterial>();
		for (unsigned int i = 0; i < info->numMaterials; i++)
		{
			info->materials[i].name = read.read_string();
		}

		info->brushsides = read.read_array<cbrushside_t>();
		for (unsigned int i = 0; i < info->numBrushSides; i++)
		{
			info->brushsides[i].plane = read.read_array<cplane_s>();
		}

		info->brushEdges = read.read_array<unsigned char>();
		info->leafbrushNodes = read.read_array<cLeafBrushNode_s>();
		for (unsigned int i = 0; i < info->leafbrushNodesCount; i++)
		{
			if (info->leafbrushNodes[i].leafBrushCount > 0)
			{
				info->leafbrushNodes[i].data.leaf.brushes = read.read_array<unsigned short>();
			}
		}

		info->leafbrushes = read.read_array<unsigned short>();
		info->brushes = read.read_array<cbrush_t>();
		for (unsigned int i = 0; i < info->numBrushes; i++)
		{
			if (info->brushes[i].sides)
			{
				info->brushes[i].sides = read.read_array<cbrushside_t>();
				if (info->brushes[i].sides->plane)
				{
					info->brushes[i].sides->plane = read.read_array<cplane_s>();
				}
			}
			if (info->brushes[i].baseAdjacentSide)
			{
				info->brushes[i].baseAdjacentSide = read.read_array<cbrushedge_t>();
			}
		}

		info->brushBounds = read.read_array<Bounds>();
		info->brushContents = read.read_array<int>();
	}

	clipMap_t* clip_map::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".colmap";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing colmap \"%s\"...", name.data());

		clipMap_t* asset = read.read_single<clipMap_t>();
		asset->name = read.read_string();

		parse_info(&asset->info, read, mem);

		//asset->pInfo = read.read_single<ClipInfo>();
		//parse_info(asset->pInfo, read, mem);

		asset->staticModelList = read.read_array<cStaticModel_s>();
		for (unsigned int i = 0; i < asset->numStaticModels; i++)
		{
			if (asset->staticModelList[i].xmodel)
			{
				asset->staticModelList[i].xmodel = read.read_asset<XModel>();
			}
		}

		asset->nodes = read.read_array<cNode_t>();
		for (unsigned int i = 0; i < asset->numNodes; i++)
		{
			if (asset->nodes[i].plane)
			{
				asset->nodes[i].plane = read.read_array<cplane_s>();
			}
		}

		asset->leafs = read.read_array<cLeaf_t>();
		asset->verts = read.read_array<vec3_t>();
		asset->triIndices = read.read_array<unsigned short>();
		asset->triEdgeIsWalkable = read.read_array<unsigned char>();
		asset->borders = read.read_array<CollisionBorder>();

		asset->partitions = read.read_array<CollisionPartition>();
		for (int i = 0; i < asset->partitionCount; i++)
		{
			if (asset->partitions[i].borders)
			{
				asset->partitions[i].borders = read.read_array<CollisionBorder>();
			}
		}

		asset->aabbTrees = read.read_array<CollisionAabbTree>();

		asset->cmodels = read.read_array<cmodel_t>();
		//for (unsigned int i = 0; i < asset->numSubModels; i++)
		//{
		//	if (asset->cmodels[i].info)
		//	{
		//		asset->cmodels[i].info = read.read_single<ClipInfo>();
		//		parse_info(asset->cmodels[i].info, read, mem);
		//		auto x = 0;
		//	}
		//}

		asset->smodelNodes = read.read_array<SModelAabbNode>();
		asset->mapEnts = read.read_asset<MapEnts>();

		asset->stages = read.read_array<Stage>();
		for (int i = 0; i < asset->stageCount; i++)
		{
			asset->stages[i].name = read.read_string();
		}
		//asset->stageTrigger.models = read.read_array<TriggerModel>();
		//asset->stageTrigger.hulls = read.read_array<TriggerHull>();
		//asset->stageTrigger.slabs = read.read_array<TriggerSlab>();

		for (int i = 0; i < 2; i++)
		{
			asset->dynEntCount[i] = static_cast<unsigned short>(read.read_int());

			if (asset->dynEntCount[i] <= 0)
			{
				continue;
			}

			asset->dynEntDefList[i] = read.read_array<DynEntityDef>();
			asset->dynEntPoseList[i] = read.read_array<DynEntityPose>();
			asset->dynEntClientList[i] = read.read_array<DynEntityClient>();
			asset->dynEntCollList[i] = read.read_array<DynEntityColl>();
			for (int j = 0; j < asset->dynEntCount[i]; j++)
			{
				asset->dynEntDefList[i][j].baseModel = read.read_asset<XModel>();
				asset->dynEntDefList[i][j].destroyFx = read.read_asset<FxEffectDef>();
				asset->dynEntDefList[i][j].physPreset = read.read_asset<PhysPreset>();
				asset->dynEntDefList[i][j].hinge = read.read_single<DynEntityHingeDef>();
				asset->dynEntDefList[i][j].linkTo = read.read_single<DynEntityLinkToDef>();
			}
		}

		asset->dynEntAnchorNames = mem->allocate<scr_string_t>(asset->dynEntAnchorCount);
		for (unsigned int i = 0; i < asset->dynEntAnchorCount; i++)
		{
			this->add_script_string(&asset->dynEntAnchorNames[i], read.read_string());
		}

		asset->scriptableMapEnts.instances = read.read_array<ScriptableInstance>();
		if (asset->scriptableMapEnts.instances)
		{
			for (unsigned int i = 0; i < asset->scriptableMapEnts.instanceCount; i++)
			{
				if (asset->scriptableMapEnts.instances[i].def)
				{
					asset->scriptableMapEnts.instances[i].def = read.read_asset<ScriptableDef>();
				}
				if (asset->scriptableMapEnts.instances[i].eventConstantsBuf && asset->scriptableMapEnts.instances[i].def)
				{
					asset->scriptableMapEnts.instances[i].eventConstantsBuf = read.read_array<unsigned char>();
				}
				this->add_script_string(&asset->scriptableMapEnts.instances[i].targetname, read.read_string());
				if (asset->scriptableMapEnts.instances[i].currentModel)
				{
					asset->scriptableMapEnts.instances[i].currentModel = read.read_asset<XModel>();
				}
				if (asset->scriptableMapEnts.instances[i].partStates && asset->scriptableMapEnts.instances[i].def)
				{
					asset->scriptableMapEnts.instances[i].partStates = read.read_array<ScriptableInstancePartState>();
				}
				if (asset->scriptableMapEnts.instances[i].eventStreamBuf && asset->scriptableMapEnts.instances[i].def)
				{
					asset->scriptableMapEnts.instances[i].eventStreamBuf = read.read_array<unsigned char>();
				}
			}
		}

		asset->scriptableMapEnts.animEntries = read.read_array<ScriptableAnimationEntry>();
		if (asset->scriptableMapEnts.animEntries)
		{
			for (unsigned int i = 0; i < asset->scriptableMapEnts.animEntryCount; i++)
			{
				if (asset->scriptableMapEnts.animEntries[i].animName)
				{
					asset->scriptableMapEnts.animEntries[i].animName = read.read_string();
				}
			}
		}

		read.close();

		// copy info into pInfo
		//asset->pInfo = &asset->info;
		asset->pInfo = mem->allocate<ClipInfo>();
		memcpy(asset->pInfo, &asset->info, sizeof(ClipInfo));

		return asset;
	}

	void clip_map::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse clipmap \"%s\"", name.data());
		}
	}

	void clip_map::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		if (data->dynEntAnchorNames)
		{
			for (unsigned int i = 0; i < data->dynEntAnchorCount; i++)
			{
				data->dynEntAnchorNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->dynEntAnchorNames[i])));
			}
		}

		if (data->scriptableMapEnts.instances)
		{
			for (unsigned int i = 0; i < data->scriptableMapEnts.instanceCount; i++)
			{
				data->scriptableMapEnts.instances[i].targetname = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->scriptableMapEnts.instances[i].targetname)));
			}
		}
	}

	void clip_map::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->staticModelList)
		{
			for (unsigned int i = 0; i < data->numStaticModels; i++)
			{
				if (data->staticModelList[i].xmodel)
				{
					zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->staticModelList[i].xmodel->name);
				}
			}
		}

		if (data->mapEnts)
		{
			zone->add_asset_of_type(ASSET_TYPE_MAP_ENTS, this->asset_->mapEnts->name);
		}

		for (int i = 0; i < 2; i++)
		{
			if (data->dynEntDefList[i])
			{
				for (int j = 0; j < data->dynEntCount[i]; j++)
				{
					if (data->dynEntDefList[i][j].baseModel)
					{
						zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->dynEntDefList[i][j].baseModel->name);
					}
					if (data->dynEntDefList[i][j].destroyFx)
					{
						zone->add_asset_of_type(ASSET_TYPE_FX, data->dynEntDefList[i][j].destroyFx->name);
					}
					if (data->dynEntDefList[i][j].physPreset)
					{
						zone->add_asset_of_type(ASSET_TYPE_PHYSPRESET, data->dynEntDefList[i][j].physPreset->name);
					}
				}
			}

			if (data->dynEntClientList[i])
			{
				for (int j = 0; j < data->dynEntCount[i]; j++)
				{
					if (data->dynEntClientList[i][j].activeModel)
					{
						zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->dynEntClientList[i][j].activeModel->name);
					}
				}
			}
		}

		if (data->scriptableMapEnts.instances)
		{
			for (unsigned int i = 0; i < data->scriptableMapEnts.instanceCount; i++)
			{
				if (data->scriptableMapEnts.instances[i].def)
				{
					zone->add_asset_of_type(ASSET_TYPE_SCRIPTABLE, data->scriptableMapEnts.instances[i].def->name);
				}
				if (data->scriptableMapEnts.instances[i].currentModel)
				{
					zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->scriptableMapEnts.instances[i].currentModel->name);
				}
			}
		}
	}

	std::string clip_map::name()
	{
		return this->name_;
	}

	std::int32_t clip_map::type()
	{
		return ASSET_TYPE_CLIPMAP;
	}

	void clip_map::write_info(zone_base* zone, zone_buffer* buf, ClipInfo* data, ClipInfo* dest)
	{
		if (data->planes)
		{
			dest->planes = buf->write_s(3, data->planes, data->planeCount);
		}

		if (data->materials)
		{
			ClipMaterial* clipmaterial;
			dest->materials = buf->write_s(3, data->materials, data->numMaterials, sizeof ClipMaterial, &clipmaterial);

			if (dest->materials == reinterpret_cast<ClipMaterial*>(buf->data_following))
			{
				for (std::uint32_t i = 0; i < data->numMaterials; i++)
				{
					if (data->materials[i].name)
					{
						clipmaterial[i].name = buf->write_str(data->materials[i].name);
					}
				}
			}
		}

		if (data->brushsides)
		{
			cbrushside_t* brush_side;
			dest->brushsides = buf->write_s(3, data->brushsides, data->numBrushSides, sizeof cbrushside_t,
				&brush_side);

			if (dest->brushsides == reinterpret_cast<cbrushside_t*>(buf->data_following))
			{
				for (std::uint32_t i = 0; i < data->numBrushSides; i++)
				{
					if (data->brushsides[i].plane)
					{
						brush_side[i].plane = buf->write_s(3, data->brushsides[i].plane);
					}
				}
			}
		}

		if (data->brushEdges)
		{
			dest->brushEdges = buf->write_s(0, data->brushEdges, data->numBrushEdges);
		}

		if (data->leafbrushNodes)
		{
			cLeafBrushNode_s* leaf_brush_node = nullptr;
			dest->leafbrushNodes = buf->write_s(3, data->leafbrushNodes, data->leafbrushNodesCount,
				sizeof cLeafBrushNode_s, &leaf_brush_node);

			if (dest->leafbrushNodes == reinterpret_cast<cLeafBrushNode_s*>(buf->data_following))
			{
				for (std::uint32_t i = 0; i < data->leafbrushNodesCount; i++)
				{
					if (data->leafbrushNodes[i].leafBrushCount > 0 && data->leafbrushNodes[i].data.leaf.brushes)
					{
						leaf_brush_node[i].data.leaf.brushes = buf->write_s(
							1, data->leafbrushNodes[i].data.leaf.brushes, data->leafbrushNodes[i].leafBrushCount);
					}
				}
			}
		}

		if (data->leafbrushes)
		{
			dest->leafbrushes = buf->write_s(1, data->leafbrushes, data->numLeafBrushes);
		}

		if (data->brushes)
		{
			cbrush_t* brush = nullptr;
			dest->brushes = buf->write_s(127, data->brushes, data->numBrushes, sizeof cbrush_t, &brush);

			if (dest->brushes == reinterpret_cast<cbrush_t*>(buf->data_following))
			{
				for (int i = 0; i < data->numBrushes; i++)
				{
					if (data->brushes[i].sides)
					{
						cbrushside_t* side = nullptr;
						brush[i].sides = buf->write_s(3, data->brushes[i].sides, 1, sizeof cbrushside_t, &side);

						if (brush[i].sides == (cbrushside_t*)buf->data_following && side)
						{
							if (side->plane)
							{
								side->plane = buf->write_s(3, side->plane);
							}
						}
					}

					if (data->brushes[i].baseAdjacentSide)
					{
						brush[i].baseAdjacentSide = buf->write_s(0, data->brushes[i].baseAdjacentSide);
					}
				}
			}
		}

		if (data->brushBounds)
		{
			dest->brushBounds = buf->write_s(127, data->brushBounds, data->numBrushes);
		}

		if (data->brushContents)
		{
			dest->brushContents = buf->write_s(3, data->brushContents, data->numBrushes);
		}
	}

	void clip_map::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		ClipInfo* info_ptr = reinterpret_cast<ClipInfo*>(0xFDFDFDF000000000 + offsetof(clipMap_t, pInfo) + 1);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name_);

		write_info(zone, buf, &data->info, &dest->info);

		buf->push_stream(XFILE_BLOCK_TEMP);
		if (data->pInfo)
		{
			buf->align(3);
			auto destInfo = buf->write(data->pInfo);
			write_info(zone, buf, data->pInfo, destInfo);
			buf->clear_pointer(&dest->pInfo);
		}
		buf->pop_stream();

		if (data->staticModelList)
		{
			buf->align(3);
			auto static_model = buf->write(data->staticModelList, data->numStaticModels);
			for (unsigned int i = 0; i < data->numStaticModels; i++)
			{
				if (data->staticModelList[i].xmodel)
				{
					static_model[i].xmodel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
						ASSET_TYPE_XMODEL, data->staticModelList[i].xmodel->name));
				}
			}
			buf->clear_pointer(&dest->staticModelList);
		}

		if (data->nodes)
		{
			buf->align(3);
			auto nodes = buf->write(data->nodes, data->numNodes);
			for (unsigned int i = 0; i < data->numNodes; i++)
			{
				if (data->nodes[i].plane)
				{
					nodes[i].plane = buf->write_s(3, data->nodes[i].plane);
				}
			}
			buf->clear_pointer(&dest->nodes);
		}

		if (data->leafs)
		{
			buf->align(3);
			buf->write(data->leafs, data->numLeafs);
			buf->clear_pointer(&dest->leafs);
		}

		if (data->verts)
		{
			buf->align(3);
			buf->write(data->verts, data->vertCount);
			buf->clear_pointer(&dest->verts);
		}

		if (data->triIndices)
		{
			buf->align(1);
			buf->write(data->triIndices, data->triCount * 3);
			buf->clear_pointer(&dest->triIndices);
		}

		if (data->triEdgeIsWalkable)
		{
			buf->align(0);
			buf->write(data->triEdgeIsWalkable, ((data->triCount + 2 * data->triCount + 31) >> 3) & 0xFFFFFFFFFFFFFFFCui64);
			buf->clear_pointer(&dest->triEdgeIsWalkable);
		}

		if (data->borders)
		{
			buf->align(3);
			buf->write(data->borders, data->borderCount);
			buf->clear_pointer(&dest->borders);
		}

		if (data->partitions)
		{
			buf->align(3);
			auto destpartitions = buf->write(data->partitions, data->partitionCount);
			for (int i = 0; i < data->partitionCount; i++)
			{
				if (data->partitions[i].borders)
				{
					destpartitions[i].borders = buf->write_s(3, data->partitions[i].borders);
				}
			}
			buf->clear_pointer(&dest->partitions);
		}

		if (data->aabbTrees)
		{
			buf->align(15);
			buf->write(data->aabbTrees, data->aabbTreeCount);
			buf->clear_pointer(&dest->aabbTrees);
		}

		if (data->cmodels)
		{
			buf->align(3);
			auto destcmodels = buf->write(data->cmodels, data->numSubModels);

			for (unsigned int i = 0; i < data->numSubModels; i++)
			{
				destcmodels[i].info = info_ptr;
			}

			buf->clear_pointer(&dest->cmodels);
		}

		if (data->smodelNodes)
		{
			buf->align(3);
			buf->write(data->smodelNodes, data->smodelNodeCount);
			buf->clear_pointer(&dest->smodelNodes);
		}

		if (data->mapEnts)
		{
			dest->mapEnts = reinterpret_cast<MapEnts*>(zone->get_asset_pointer(ASSET_TYPE_MAP_ENTS, this->name()));
		}

		if (data->stages)
		{
			buf->align(3);
			auto destStages = buf->write(data->stages, data->stageCount);

			for (unsigned char i = 0; i < data->stageCount; i++)
			{
				if (data->stages[i].name)
				{
					destStages[i].name = buf->write_str(data->stages[i].name);
				}
			}

			buf->clear_pointer(&dest->stages);
		}

		// copy trigger data from mapents
		auto* mapEntsAsset = zone->find_asset(ASSET_TYPE_MAP_ENTS, this->name());
		auto* mapEnts = mapEntsAsset ? reinterpret_cast<MapEnts*>(mapEntsAsset->pointer()) : nullptr;
		if (mapEnts)
		{
			memcpy(&dest->stageTrigger, &mapEnts->trigger, sizeof(MapTriggers));
		}
		else
		{
			memset(&dest->stageTrigger, 0, sizeof(MapTriggers));
		}

		// write triggers
		map_ents::write_triggers(buf, &dest->stageTrigger);

		if (data->dynEntDefList[0])
		{
			buf->align(3);
			auto dyn_entity_def = buf->write(data->dynEntDefList[0], data->dynEntCount[0]);

			for (std::uint16_t i = 0; i < data->dynEntCount[0]; i++)
			{
				if (data->dynEntDefList[0][i].baseModel)
				{
					dyn_entity_def[i].baseModel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
						ASSET_TYPE_XMODEL, data->dynEntDefList[0][i].baseModel->name));
				}
				if (data->dynEntDefList[0][i].destroyFx)
				{
					dyn_entity_def[i].destroyFx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->dynEntDefList[0][i].destroyFx->name));
				}
				if (data->dynEntDefList[0][i].physPreset)
				{
					dyn_entity_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
						ASSET_TYPE_PHYSPRESET, data->dynEntDefList[0][i].physPreset->name));
				}

				if (data->dynEntDefList[0][i].hinge)
				{
					dyn_entity_def[i].hinge = buf->write_s(3, dyn_entity_def[i].hinge, 1);
				}

				if (data->dynEntDefList[0][i].linkTo)
				{
					dyn_entity_def[i].linkTo = buf->write_s(3, dyn_entity_def[i].linkTo, 1);
				}
			}

			buf->clear_pointer(&dest->dynEntDefList[0]);
		}

		if (data->dynEntDefList[1])
		{
			buf->align(3);
			auto dyn_entity_def = buf->write(data->dynEntDefList[1], data->dynEntCount[1]);

			for (std::uint16_t i = 0; i < data->dynEntCount[1]; i++)
			{
				if (data->dynEntDefList[1][i].baseModel)
				{
					dyn_entity_def[i].baseModel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
						ASSET_TYPE_XMODEL, data->dynEntDefList[1][i].baseModel->name));
				}
				if (data->dynEntDefList[1][i].destroyFx)
				{
					dyn_entity_def[i].destroyFx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->dynEntDefList[1][i].destroyFx->name));
				}
				if (data->dynEntDefList[1][i].physPreset)
				{
					dyn_entity_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
						ASSET_TYPE_PHYSPRESET, data->dynEntDefList[1][i].physPreset->name));
				}

				if (data->dynEntDefList[1][i].hinge)
				{
					dyn_entity_def[i].hinge = buf->write_s(3, data->dynEntDefList[1][i].hinge, 1);
				}

				if (data->dynEntDefList[1][i].linkTo)
				{
					dyn_entity_def[i].linkTo = buf->write_s(3, data->dynEntDefList[1][i].linkTo, 1);
				}
			}

			buf->clear_pointer(&dest->dynEntDefList[1]);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dynEntPoseList[0])
		{
			buf->align(3);
			buf->write(data->dynEntPoseList[0], data->dynEntCount[0]);
			buf->clear_pointer(&dest->dynEntPoseList[0]);
		}

		if (data->dynEntPoseList[1])
		{
			buf->align(3);
			buf->write(data->dynEntPoseList[1], data->dynEntCount[1]);
			buf->clear_pointer(&dest->dynEntPoseList[1]);
		}

		if (data->dynEntClientList[0])
		{
			buf->align(3);
			buf->write(data->dynEntClientList[0], data->dynEntCount[0]);
			buf->clear_pointer(&dest->dynEntClientList[0]);
		}

		if (data->dynEntClientList[1])
		{
			buf->align(3);
			buf->write(data->dynEntClientList[1], data->dynEntCount[1]);
			buf->clear_pointer(&dest->dynEntClientList[1]);
		}

		if (data->dynEntCollList[0])
		{
			buf->align(3);
			buf->write(data->dynEntCollList[0], data->dynEntCount[0]);
			buf->clear_pointer(&dest->dynEntCollList[0]);
		}

		if (data->dynEntCollList[1])
		{
			buf->align(3);
			buf->write(data->dynEntCollList[1], data->dynEntCount[1]);
			buf->clear_pointer(&dest->dynEntCollList[1]);
		}
		buf->pop_stream();

		if (data->dynEntAnchorNames)
		{
			buf->align(3);
			buf->write(data->dynEntAnchorNames, data->dynEntAnchorCount);
			buf->clear_pointer(&dest->dynEntAnchorNames);
		}

		if (data->scriptableMapEnts.instances)
		{
			buf->align(3);
			auto destscriptables = buf->write(data->scriptableMapEnts.instances, data->scriptableMapEnts.instanceCount);
			for (unsigned int i = 0; i < data->scriptableMapEnts.instanceCount; i++)
			{
				auto* scriptable = &data->scriptableMapEnts.instances[i];
				auto* destscriptable = &destscriptables[i];

				ScriptableDef* scriptableDef = nullptr;
				if (scriptable->def)
				{
					scriptableDef = reinterpret_cast<ScriptableDef*>(
						zone->find_asset(ASSET_TYPE_SCRIPTABLE, scriptable->def->name)->pointer());
				}

				if (scriptable->def && scriptableDef)
				{
					destscriptable->def = reinterpret_cast<ScriptableDef*>(
						zone->get_asset_pointer(ASSET_TYPE_SCRIPTABLE, scriptable->def->name));

					if (scriptable->eventConstantsBuf)
					{
						buf->align(0);
						buf->write(scriptable->eventConstantsBuf, scriptableDef->eventConstantsSize);
						buf->clear_pointer(&destscriptable->eventConstantsBuf);
					}

					buf->push_stream(XFILE_BLOCK_RUNTIME);
					if (scriptable->currentModel)
					{
						destscriptable->currentModel = reinterpret_cast<XModel*>(
							zone->get_asset_pointer(ASSET_TYPE_XMODEL, scriptable->currentModel->name));
					}

					if (scriptable->partStates)
					{
						buf->align(3);
						buf->write(scriptable->partStates, scriptableDef->partCount);
						buf->clear_pointer(&destscriptable->partStates);
					}

					if (scriptable->eventStreamBuf)
					{
						buf->align(0);
						buf->write(scriptable->eventStreamBuf, scriptableDef->eventStreamSize);
						buf->clear_pointer(&destscriptable->eventStreamBuf);
					}
					buf->pop_stream();
				}
				else
				{
					memset(destscriptable, 0, sizeof(ScriptableInstance));
				}
			}
			buf->clear_pointer(&dest->scriptableMapEnts.instances);
		}

		if (data->scriptableMapEnts.animEntries)
		{
			buf->align(3);
			auto animentries = buf->write(data->scriptableMapEnts.animEntries, data->scriptableMapEnts.animEntryCount);
			for (unsigned int i = 0; i < data->scriptableMapEnts.animEntryCount; i++)
			{
				if (data->scriptableMapEnts.animEntries[i].animName)
				{
					animentries[i].animName = buf->write_str(data->scriptableMapEnts.animEntries[i].animName);
				}
			}
			buf->clear_pointer(&dest->scriptableMapEnts.animEntries);
		}

		buf->pop_stream();
	}

	void clip_map::dump_info(ClipInfo* info, assetmanager::dumper& write)
	{
		write.dump_array(info->planes, info->planeCount);
		write.dump_array(info->materials, info->numMaterials);
		for (unsigned int i = 0; i < info->numMaterials; i++)
		{
			write.dump_string(info->materials[i].name);
		}

		write.dump_array(info->brushsides, info->numBrushSides);
		for (unsigned int i = 0; i < info->numBrushSides; i++)
		{
			write.dump_array(info->brushsides[i].plane, 1);
		}

		write.dump_array(info->brushEdges, info->numBrushEdges);
		write.dump_array(info->leafbrushNodes, info->leafbrushNodesCount);
		for (unsigned int i = 0; i < info->leafbrushNodesCount; i++)
		{
			if (info->leafbrushNodes[i].leafBrushCount > 0)
			{
				write.dump_array(info->leafbrushNodes[i].data.leaf.brushes, info->leafbrushNodes[i].leafBrushCount);
			}
		}

		write.dump_array(info->leafbrushes, info->numLeafBrushes);
		write.dump_array(info->brushes, info->numBrushes);
		for (unsigned int i = 0; i < info->numBrushes; i++)
		{
			if (info->brushes[i].sides)
			{
				write.dump_array(info->brushes[i].sides, 1);
				if (info->brushes[i].sides->plane)
				{
					write.dump_array(info->brushes[i].sides->plane, 1);
				}
			}
			if (info->brushes[i].baseAdjacentSide)
			{
				write.dump_array(info->brushes[i].baseAdjacentSide, 1);
			}
		}

		write.dump_array(info->brushBounds, info->numBrushes);
		write.dump_array(info->brushContents, info->numBrushes);
	}

	void clip_map::dump(clipMap_t* asset)
	{
		const auto path = asset->name + ".colmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		dump_info(&asset->info, write);

		//write.dump_single(asset->pInfo);
		//dump_info(asset->pInfo, write);

		write.dump_array(asset->staticModelList, asset->numStaticModels);
		for (unsigned int i = 0; i < asset->numStaticModels; i++)
		{
			if (asset->staticModelList[i].xmodel)
			{
				write.dump_asset(asset->staticModelList[i].xmodel);
			}
		}

		write.dump_array(asset->nodes, asset->numNodes);
		for (unsigned int i = 0; i < asset->numNodes; i++)
		{
			if (asset->nodes[i].plane)
			{
				write.dump_array(asset->nodes[i].plane, 1);
			}
		}

		write.dump_array(asset->leafs, asset->numLeafs);
		write.dump_array(asset->verts, asset->vertCount);
		write.dump_array(asset->triIndices, asset->triCount * 3);
		write.dump_array(asset->triEdgeIsWalkable, ((asset->triCount + 2 * asset->triCount + 31) >> 3) & 0xFFFFFFFFFFFFFFFCui64);
		write.dump_array(asset->borders, asset->borderCount);

		write.dump_array(asset->partitions, asset->partitionCount);
		for (int i = 0; i < asset->partitionCount; i++)
		{
			if (asset->partitions[i].borders)
			{
				write.dump_array(asset->partitions[i].borders, 1);
			}
		}

		write.dump_array(asset->aabbTrees, asset->aabbTreeCount);

		write.dump_array(asset->cmodels, asset->numSubModels);
		//for (unsigned int i = 0; i < asset->numSubModels; i++)
		//{
		//	if (asset->cmodels[i].info)
		//	{
		//		write.dump_single(asset->cmodels[i].info);
		//		dump_info(asset->cmodels[i].info, write);
		//	}
		//}

		write.dump_array(asset->smodelNodes, asset->smodelNodeCount);
		write.dump_asset(asset->mapEnts);

		write.dump_array(asset->stages, asset->stageCount);
		for (unsigned char i = 0; i < asset->stageCount; i++)
		{
			write.dump_string(asset->stages[i].name);
		}
		//write.dump_array(asset->stageTrigger.models, asset->stageTrigger.count);
		//write.dump_array(asset->stageTrigger.hulls, asset->stageTrigger.hullCount);
		//write.dump_array(asset->stageTrigger.slabs, asset->stageTrigger.slabCount);

		for (int i = 0; i < 2; i++)
		{
			write.dump_int(asset->dynEntCount[i]);

			if (asset->dynEntCount[i] <= 0)
			{
				continue;
			}

			write.dump_array(asset->dynEntDefList[i], asset->dynEntCount[i]);
			write.dump_array(asset->dynEntPoseList[i], asset->dynEntCount[i]);
			write.dump_array(asset->dynEntClientList[i], asset->dynEntCount[i]);
			write.dump_array(asset->dynEntCollList[i], asset->dynEntCount[i]);
			for (int j = 0; j < asset->dynEntCount[i]; j++)
			{
				write.dump_asset(asset->dynEntDefList[i][j].baseModel);
				write.dump_asset(asset->dynEntDefList[i][j].destroyFx);
				write.dump_asset(asset->dynEntDefList[i][j].physPreset);
				write.dump_single(asset->dynEntDefList[i][j].hinge);
				write.dump_single(asset->dynEntDefList[i][j].linkTo);
			}
		}

		for (unsigned int i = 0; i < asset->dynEntAnchorCount; i++)
		{
			write.dump_string(SL_ConvertToString(asset->dynEntAnchorNames[i]));
		}

		write.dump_array(asset->scriptableMapEnts.instances, asset->scriptableMapEnts.instanceCount);
		if (asset->scriptableMapEnts.instances)
		{
			for (unsigned int i = 0; i < asset->scriptableMapEnts.instanceCount; i++)
			{
				if (asset->scriptableMapEnts.instances[i].def)
				{
					write.dump_asset(asset->scriptableMapEnts.instances[i].def);
				}
				if (asset->scriptableMapEnts.instances[i].eventConstantsBuf && asset->scriptableMapEnts.instances[i].def)
				{
					write.dump_array(asset->scriptableMapEnts.instances[i].eventConstantsBuf,
						asset->scriptableMapEnts.instances[i].def->eventConstantsSize);
				}
				auto str = SL_ConvertToString(asset->scriptableMapEnts.instances[i].targetname);
				write.dump_string(str ? str : "");
				if (asset->scriptableMapEnts.instances[i].currentModel)
				{
					write.dump_asset(asset->scriptableMapEnts.instances[i].currentModel);
				}
				if (asset->scriptableMapEnts.instances[i].partStates && asset->scriptableMapEnts.instances[i].def)
				{
					write.dump_array(asset->scriptableMapEnts.instances[i].partStates,
						asset->scriptableMapEnts.instances[i].def->partCount);
				}
				if (asset->scriptableMapEnts.instances[i].eventStreamBuf && asset->scriptableMapEnts.instances[i].def)
				{
					write.dump_array(asset->scriptableMapEnts.instances[i].eventStreamBuf,
						asset->scriptableMapEnts.instances[i].def->eventStreamSize);
				}
			}
		}

		write.dump_array(asset->scriptableMapEnts.animEntries, asset->scriptableMapEnts.animEntryCount);
		if (asset->scriptableMapEnts.animEntries)
		{
			for (unsigned int i = 0; i < asset->scriptableMapEnts.animEntryCount; i++)
			{
				if (asset->scriptableMapEnts.animEntries[i].animName)
				{
					write.dump_string(asset->scriptableMapEnts.animEntries[i].animName);
				}
			}
		}

		write.close();
	}
}