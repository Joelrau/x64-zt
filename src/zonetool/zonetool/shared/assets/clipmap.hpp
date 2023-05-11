#pragma once

namespace zonetool
{
#define TYPEOF_MEMBER(__struct__, __member__) \
	std::remove_pointer<typename std::remove_all_extents<decltype(std::declval<__struct__>().__member__)>::type>::type \

	template <ASSET_TEMPLATE>
	class clip_map : public asset_interface
	{
		typedef S clipMap_t;
		typedef TYPEOF_MEMBER(S, info) ClipInfo;
		typedef TYPEOF_MEMBER(S, dynEntAnchorNames) scr_string_t;
		typedef TYPEOF_MEMBER(S, mapEnts) MapEnts;
		typedef TYPEOF_MEMBER(S, stages) Stage;
		typedef TYPEOF_MEMBER(S, info.materials) ClipMaterial;
		typedef TYPEOF_MEMBER(S, stageTrigger) MapTriggers;

		typedef TYPEOF_MEMBER(S, dynEntDefList[0]->baseModel) XModel;
		typedef TYPEOF_MEMBER(S, dynEntDefList[0]->destroyFx) FxEffectDef;
		typedef TYPEOF_MEMBER(S, dynEntDefList[0]->sound) snd_alias_list_t;
		typedef TYPEOF_MEMBER(S, dynEntDefList[0]->physPreset) PhysPreset;

		typedef TYPEOF_MEMBER(S, scriptableMapEnts.instances) ScriptableInstance;
		typedef TYPEOF_MEMBER(S, scriptableMapEnts.instances->def) ScriptableDef;

		typedef TYPEOF_MEMBER(S, info.bCollisionTree.leafbrushNodes) cLeafBrushNode_s;
		typedef TYPEOF_MEMBER(S, info.bCollisionData.brushes) cbrush_t;
		typedef TYPEOF_MEMBER(S, info.bCollisionData.brushBounds) Bounds;
		typedef TYPEOF_MEMBER(S, info.pCollisionData.partitions) CollisionPartition;
		typedef TYPEOF_MEMBER(S, info.sCollisionData.staticModelList) cStaticModel_s;

#undef TYPEOF_PTR
#undef TYPEOF_MEMBER

	private:
		std::string name_;
		clipMap_t* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;

		void add_script_string(scr_string_t* ptr, const char* str)
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

		const char* get_script_string(scr_string_t* ptr)
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

		void parse_info(ClipInfo* info, assetmanager::reader& read, zone_memory* mem)
		{
			read.read_array(&info->planes);
			read.read_array(&info->materials);
			for (unsigned int i = 0; i < info->numMaterials; i++)
			{
				info->materials[i].name = read.read_string();
			}

			read.read_array(&info->bCollisionTree.leafbrushNodes);
			for (unsigned int i = 0; i < info->bCollisionTree.leafbrushNodesCount; i++)
			{
				if (info->bCollisionTree.leafbrushNodes[i].leafBrushCount > 0)
				{
					read.read_array(&info->bCollisionTree.leafbrushNodes[i].data.leaf.brushes);
				}
			}
			read.read_array(&info->bCollisionTree.leafbrushes);

			read.read_array(&info->pCollisionTree.aabbTrees);

			read.read_array(&info->sCollisionTree.smodelNodes);

			read.read_array(&info->bCollisionData.brushSides);
			read.read_array(&info->bCollisionData.brushEdges);
			read.read_array(&info->bCollisionData.brushes);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
			{
				if (info->bCollisionData.brushes[i].sides)
				{
					read.read_array(&info->bCollisionData.brushes[i].sides);
				}
				if (info->bCollisionData.brushes[i].baseAdjacentSide)
				{
					read.read_array(&info->bCollisionData.brushes[i].baseAdjacentSide);
				}
			}
			read.read_array(&info->bCollisionData.brushBounds);
			read.read_array(&info->bCollisionData.brushContents);

			read.read_array(&info->pCollisionData.verts);
			read.read_array(&info->pCollisionData.triIndices);
			read.read_array(&info->pCollisionData.triEdgeIsWalkable);
			read.read_array(&info->pCollisionData.borders);
			read.read_array(&info->pCollisionData.partitions);
			for (int i = 0; i < info->pCollisionData.partitionCount; i++)
			{
				if (info->pCollisionData.partitions[i].borders)
				{
					read.read_array(&info->pCollisionData.partitions[i].borders);
				}
			}

			read.read_array(&info->sCollisionData.staticModelList);
			for (unsigned int i = 0; i < info->sCollisionData.numStaticModels; i++)
			{
				if (info->sCollisionData.staticModelList[i].xmodel)
				{
					read.read_asset(&info->sCollisionData.staticModelList[i].xmodel);
				}
			}
		}

		void write_info(zone_base* zone, zone_buffer* buf, ClipInfo* data, ClipInfo* dest)
		{
			if (data->planes)
			{
				dest->planes = buf->write_s(3, data->planes, data->planeCount);
			}

			if (data->materials)
			{
				ClipMaterial* clipmaterial;
				dest->materials = buf->write_s(3, data->materials, data->numMaterials, sizeof(ClipMaterial), &clipmaterial);

				if (dest->materials == reinterpret_cast<ClipMaterial*>(0xFDFDFDFFFFFFFFFF))
				{
					for (unsigned int i = 0; i < data->numMaterials; i++)
					{
						if (data->materials[i].name)
						{
							clipmaterial[i].name = buf->write_str(data->materials[i].name);
						}
					}
				}
			}

			if (data->bCollisionTree.leafbrushNodes)
			{
				cLeafBrushNode_s* leaf_brush_node = nullptr;
				dest->bCollisionTree.leafbrushNodes = buf->write_s(3, data->bCollisionTree.leafbrushNodes, data->bCollisionTree.leafbrushNodesCount,
					sizeof(cLeafBrushNode_s), &leaf_brush_node);

				if (dest->bCollisionTree.leafbrushNodes == reinterpret_cast<cLeafBrushNode_s*>(0xFDFDFDFFFFFFFFFF))
				{
					for (unsigned int i = 0; i < data->bCollisionTree.leafbrushNodesCount; i++)
					{
						if (data->bCollisionTree.leafbrushNodes[i].leafBrushCount > 0 && data->bCollisionTree.leafbrushNodes[i].data.leaf.brushes)
						{
							leaf_brush_node[i].data.leaf.brushes = buf->write_s(
								1, data->bCollisionTree.leafbrushNodes[i].data.leaf.brushes, data->bCollisionTree.leafbrushNodes[i].leafBrushCount);
						}
					}
				}
			}
			if (data->bCollisionTree.leafbrushes)
			{
				dest->bCollisionTree.leafbrushes = buf->write_s(1, data->bCollisionTree.leafbrushes, data->bCollisionTree.numLeafBrushes);
			}

			if (data->pCollisionTree.aabbTrees)
			{
				dest->pCollisionTree.aabbTrees = buf->write_s(15, data->pCollisionTree.aabbTrees, data->pCollisionTree.aabbTreeCount);
			}

			if (data->sCollisionTree.smodelNodes)
			{
				dest->sCollisionTree.smodelNodes = buf->write_s(3, data->sCollisionTree.smodelNodes, data->sCollisionTree.smodelNodeCount);
			}

			if (data->bCollisionData.brushSides)
			{
				dest->bCollisionData.brushSides = buf->write_s(3, data->bCollisionData.brushSides, data->bCollisionData.numBrushSides);
			}
			if (data->bCollisionData.brushEdges)
			{
				dest->bCollisionData.brushEdges = buf->write_s(0, data->bCollisionData.brushEdges, data->bCollisionData.numBrushEdges);
			}
			if (data->bCollisionData.brushes)
			{
				cbrush_t* brush = nullptr;
				dest->bCollisionData.brushes = buf->write_s(127, data->bCollisionData.brushes, data->bCollisionData.numBrushes, sizeof(cbrush_t), &brush);

				if (dest->bCollisionData.brushes == reinterpret_cast<cbrush_t*>(0xFDFDFDFFFFFFFFFF))
				{
					for (unsigned short i = 0; i < data->bCollisionData.numBrushes; i++)
					{
						if (data->bCollisionData.brushes[i].sides)
						{
							brush[i].sides = buf->write_s(3, data->bCollisionData.brushes[i].sides);
						}

						if (data->bCollisionData.brushes[i].baseAdjacentSide)
						{
							brush[i].baseAdjacentSide = buf->write_s(0, data->bCollisionData.brushes[i].baseAdjacentSide);
						}
					}
				}
			}
			if (data->bCollisionData.brushBounds)
			{
				dest->bCollisionData.brushBounds = buf->write_s(127, data->bCollisionData.brushBounds, data->bCollisionData.numBrushes);
			}
			if (data->bCollisionData.brushContents)
			{
				dest->bCollisionData.brushContents = buf->write_s(3, data->bCollisionData.brushContents, data->bCollisionData.numBrushes);
			}

			if (data->pCollisionData.verts)
			{
				dest->pCollisionData.verts = buf->write_s(3, data->pCollisionData.verts, data->pCollisionData.vertCount);
			}
			if (data->pCollisionData.triIndices)
			{
				dest->pCollisionData.triIndices = buf->write_s(1, data->pCollisionData.triIndices, 3 * data->pCollisionData.triCount);
			}
			if (data->pCollisionData.triEdgeIsWalkable)
			{
				dest->pCollisionData.triEdgeIsWalkable = buf->write_s(
					0, data->pCollisionData.triEdgeIsWalkable, ((data->pCollisionData.triCount + 2 * data->pCollisionData.triCount + 31) >> 3) & 0x1FFFFFFC);
			}
			if (data->pCollisionData.borders)
			{
				dest->pCollisionData.borders = buf->write_s(3, data->pCollisionData.borders, data->pCollisionData.borderCount);
			}
			if (data->pCollisionData.partitions)
			{
				CollisionPartition* partition = nullptr;
				dest->pCollisionData.partitions = buf->write_s(
					3, data->pCollisionData.partitions, data->pCollisionData.partitionCount, sizeof(CollisionPartition), &partition);

				if (dest->pCollisionData.partitions == reinterpret_cast<CollisionPartition*>(0xFDFDFDFFFFFFFFFF))
				{
					for (int i = 0; i < data->pCollisionData.partitionCount; i++)
					{
						if (data->pCollisionData.partitions[i].borders)
						{
							partition[i].borders = buf->write_s(3, data->pCollisionData.partitions[i].borders);
						}
					}
				}
			}

			if (data->sCollisionData.staticModelList)
			{
				cStaticModel_s* staticModelList = nullptr;
				dest->sCollisionData.staticModelList = buf->write_s(
					3, data->sCollisionData.staticModelList, data->sCollisionData.numStaticModels, sizeof(cStaticModel_s), &staticModelList);

				if (dest->sCollisionData.staticModelList == reinterpret_cast<cStaticModel_s*>(0xFDFDFDFFFFFFFFFF))
				{
					for (unsigned int i = 0; i < data->sCollisionData.numStaticModels; i++)
					{
						if (data->sCollisionData.staticModelList[i].xmodel)
						{
							staticModelList[i].xmodel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
								Types::ASSET_TYPE_XMODEL, data->sCollisionData.staticModelList[i].xmodel->name));
						}
					}
				}
			}
		}

		static void dump_info(ClipInfo* info, assetmanager::dumper& write)
		{
			write.dump_array(info->planes, info->planeCount);
			write.dump_array(info->materials, info->numMaterials);
			for (unsigned int i = 0; i < info->numMaterials; i++)
			{
				write.dump_string(info->materials[i].name);
			}

			write.dump_array(info->bCollisionTree.leafbrushNodes, info->bCollisionTree.leafbrushNodesCount);
			for (unsigned int i = 0; i < info->bCollisionTree.leafbrushNodesCount; i++)
			{
				if (info->bCollisionTree.leafbrushNodes[i].leafBrushCount > 0)
				{
					write.dump_array(info->bCollisionTree.leafbrushNodes[i].data.leaf.brushes, info->bCollisionTree.leafbrushNodes[i].leafBrushCount);
				}
			}
			write.dump_array(info->bCollisionTree.leafbrushes, info->bCollisionTree.numLeafBrushes);

			write.dump_array(info->pCollisionTree.aabbTrees, info->pCollisionTree.aabbTreeCount);

			write.dump_array(info->sCollisionTree.smodelNodes, info->sCollisionTree.smodelNodeCount);

			write.dump_array(info->bCollisionData.brushSides, info->bCollisionData.numBrushSides);
			write.dump_array(info->bCollisionData.brushEdges, info->bCollisionData.numBrushEdges);
			write.dump_array(info->bCollisionData.brushes, info->bCollisionData.numBrushes);
			for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
			{
				if (info->bCollisionData.brushes[i].sides)
				{
					write.dump_array(info->bCollisionData.brushes[i].sides, 1);
				}
				if (info->bCollisionData.brushes[i].baseAdjacentSide)
				{
					write.dump_array(info->bCollisionData.brushes[i].baseAdjacentSide, 1);
				}
			}
			write.dump_array(info->bCollisionData.brushBounds, info->bCollisionData.numBrushes);
			write.dump_array(info->bCollisionData.brushContents, info->bCollisionData.numBrushes);

			write.dump_array(info->pCollisionData.verts, info->pCollisionData.vertCount);
			write.dump_array(info->pCollisionData.triIndices, info->pCollisionData.triCount * 3);
			write.dump_array(info->pCollisionData.triEdgeIsWalkable, ((info->pCollisionData.triCount + 2 * info->pCollisionData.triCount + 31) >> 3) & 0x1FFFFFFC);
			write.dump_array(info->pCollisionData.borders, info->pCollisionData.borderCount);
			write.dump_array(info->pCollisionData.partitions, info->pCollisionData.partitionCount);
			for (int i = 0; i < info->pCollisionData.partitionCount; i++)
			{
				if (info->pCollisionData.partitions[i].borders)
				{
					write.dump_array(info->pCollisionData.partitions[i].borders, 1);
				}
			}

			write.dump_array(info->sCollisionData.staticModelList, info->sCollisionData.numStaticModels);
			for (unsigned int i = 0; i < info->sCollisionData.numStaticModels; i++)
			{
				if (info->sCollisionData.staticModelList[i].xmodel)
				{
					write.dump_asset(info->sCollisionData.staticModelList[i].xmodel);
				}
			}
		}

	public:
		clipMap_t* parse(const std::string& name, zone_memory* mem)
		{
			const auto path = name + ".colmap";

			assetmanager::reader read(mem);
			if (!read.open(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing colmap \"%s\"...", name.data());

			clipMap_t* asset = read.read_single<clipMap_t>();
			read.read_string(&asset->name);

			parse_info(&asset->info, read, mem);

			read.read_array(&asset->nodes);
			for (unsigned int i = 0; i < asset->numNodes; i++)
			{
				if (asset->nodes[i].plane)
				{
					read.read_array(&asset->nodes[i].plane);
				}
			}

			read.read_array(&asset->leafs);
			read.read_array(&asset->cmodels);

			read.read_asset(&asset->mapEnts);

			read.read_array(&asset->stages);
			for (int i = 0; i < asset->stageCount; i++)
			{
				asset->stages[i].name = read.read_string();
			}

			for (int i = 0; i < 2; i++)
			{
				asset->dynEntCount[i] = static_cast<unsigned short>(read.read_int());

				if (asset->dynEntCount[i] <= 0)
				{
					continue;
				}

				read.read_array(&asset->dynEntDefList[i]);
				read.read_array(&asset->dynEntPoseList[i]);
				read.read_array(&asset->dynEntClientList[i]);
				read.read_array(&asset->dynEntCollList[i]);
				for (int j = 0; j < asset->dynEntCount[i]; j++)
				{
					read.read_asset(&asset->dynEntDefList[i][j].baseModel);
					read.read_asset(&asset->dynEntDefList[i][j].destroyFx);
					read.read_asset(&asset->dynEntDefList[i][j].sound);
					read.read_asset(&asset->dynEntDefList[i][j].physPreset);
					read.read_single(&asset->dynEntDefList[i][j].hinge);
					read.read_single(&asset->dynEntDefList[i][j].linkTo);

					//read.read_single(&asset->dynEntClientList[i][j].hinge);
					//read.read_asset(&asset->dynEntClientList[i][j].activeModel);
				}
			}

			asset->dynEntAnchorNames = mem->allocate<scr_string_t>(asset->dynEntAnchorCount);
			for (unsigned int i = 0; i < asset->dynEntAnchorCount; i++)
			{
				this->add_script_string(&asset->dynEntAnchorNames[i], read.read_string());
			}

			read.read_array(&asset->scriptableMapEnts.instances);
			if (asset->scriptableMapEnts.instances)
			{
				for (unsigned int i = 0; i < asset->scriptableMapEnts.instanceCount; i++)
				{
					if (asset->scriptableMapEnts.instances[i].def)
					{
						read.read_asset(&asset->scriptableMapEnts.instances[i].def);
					}
					if (asset->scriptableMapEnts.instances[i].eventConstantsBuf && asset->scriptableMapEnts.instances[i].def)
					{
						read.read_array(&asset->scriptableMapEnts.instances[i].eventConstantsBuf);
					}
					if (asset->scriptableMapEnts.instances[i].targetData)
					{
						read.read_array(&asset->scriptableMapEnts.instances[i].targetData);
					}
					this->add_script_string(&asset->scriptableMapEnts.instances[i].targetname, read.read_string());
					if (asset->scriptableMapEnts.instances[i].currentModel)
					{
						read.read_asset(&asset->scriptableMapEnts.instances[i].currentModel);
					}
					if (asset->scriptableMapEnts.instances[i].partStates && asset->scriptableMapEnts.instances[i].def)
					{
						read.read_array(&asset->scriptableMapEnts.instances[i].partStates);
					}
					if (asset->scriptableMapEnts.instances[i].eventStreamBuf && asset->scriptableMapEnts.instances[i].def)
					{
						read.read_array(&asset->scriptableMapEnts.instances[i].eventStreamBuf);
					}
				}
			}

			read.read_array(&asset->scriptableMapEnts.animEntries);
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

			read.read_array(&asset->grappleData.sphereTreeData.sphereTree);
			if (asset->grappleData.sphereTreeData.sphereTree)
			{
				for (int i = 0; i < asset->grappleData.sphereTreeData.sphereTreeCount; i++)
				{
					read.read_array(&asset->grappleData.sphereTreeData.sphereTree[i].unk);
				}
			}

			read.read_array(&asset->grappleData.sphereTreeData.sphereTreeObj);

			read.read_array(&asset->grappleData.magnet);

			read.close();

			// copy info into pInfo
			//asset->pInfo = &asset->info;
			asset->pInfo = mem->allocate<ClipInfo>();
			std::memcpy(asset->pInfo, &asset->info, sizeof(ClipInfo));

			return asset;
		}

		void init(const std::string& name, zone_memory* mem) override
		{
			this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") 
				+ filesystem::get_fastfile() + ".d3dbsp"; // name;

			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				ZONETOOL_FATAL("Could not parse clipmap \"%s\"", name.data());
			}
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
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

		void load_depending(zone_base* zone) override
		{
			auto* data = this->asset_;

			if (data->info.sCollisionData.staticModelList)
			{
				for (unsigned int i = 0; i < data->info.sCollisionData.numStaticModels; i++)
				{
					if (data->info.sCollisionData.staticModelList[i].xmodel)
					{
						zone->add_asset_of_type(Types::ASSET_TYPE_XMODEL, data->info.sCollisionData.staticModelList[i].xmodel->name);
					}
				}
			}

			if (data->mapEnts)
			{
				zone->add_asset_of_type(Types::ASSET_TYPE_MAP_ENTS, this->asset_->mapEnts->name);
			}

			for (int i = 0; i < 2; i++)
			{
				if (data->dynEntDefList[i])
				{
					for (int j = 0; j < data->dynEntCount[i]; j++)
					{
						if (data->dynEntDefList[i][j].baseModel)
						{
							zone->add_asset_of_type(Types::ASSET_TYPE_XMODEL, data->dynEntDefList[i][j].baseModel->name);
						}
						if (data->dynEntDefList[i][j].destroyFx)
						{
							zone->add_asset_of_type(Types::ASSET_TYPE_FX, data->dynEntDefList[i][j].destroyFx->name);
						}
						if (data->dynEntDefList[i][j].sound)
						{
							zone->add_asset_of_type(Types::ASSET_TYPE_SOUND, data->dynEntDefList[i][j].sound->name);
						}
						if (data->dynEntDefList[i][j].physPreset)
						{
							zone->add_asset_of_type(Types::ASSET_TYPE_PHYSPRESET, data->dynEntDefList[i][j].physPreset->name);
						}
					}
				}

				if (data->dynEntClientList[i])
				{
					for (int j = 0; j < data->dynEntCount[i]; j++)
					{
						if (data->dynEntClientList[i][j].activeModel)
						{
							zone->add_asset_of_type(Types::ASSET_TYPE_XMODEL, data->dynEntClientList[i][j].activeModel->name);
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
						zone->add_asset_of_type(Types::ASSET_TYPE_SCRIPTABLE, data->scriptableMapEnts.instances[i].def->name);
					}
					if (data->scriptableMapEnts.instances[i].currentModel)
					{
						zone->add_asset_of_type(Types::ASSET_TYPE_XMODEL, data->scriptableMapEnts.instances[i].currentModel->name);
					}
				}
			}
		}

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }

		std::string name() override
		{
			return this->name_;
		}

		std::int32_t type() override
		{
			return Type;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			ClipInfo* info_ptr = reinterpret_cast<ClipInfo*>(0xFDFDFDF000000000 + offsetof(clipMap_t, pInfo) + 1);

			buf->push_stream(3);

			dest->name = buf->write_str(this->name_);

			write_info(zone, buf, &data->info, &dest->info);

			buf->push_stream(0);
			if (data->pInfo)
			{
				buf->align(3);
				auto destInfo = buf->write(data->pInfo);
				write_info(zone, buf, data->pInfo, destInfo);
				zone_buffer::clear_pointer(&dest->pInfo);
			}
			buf->pop_stream();

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
				zone_buffer::clear_pointer(&dest->nodes);
			}

			if (data->leafs)
			{
				buf->align(3);
				buf->write(data->leafs, data->numLeafs);
				zone_buffer::clear_pointer(&dest->leafs);
			}

			if (data->cmodels)
			{
				buf->align(3);
				auto destcmodels = buf->write(data->cmodels, data->numSubModels);

				for (unsigned int i = 0; i < data->numSubModels; i++)
				{
					destcmodels[i].info = info_ptr;
				}

				zone_buffer::clear_pointer(&dest->cmodels);
			}

			if (data->mapEnts)
			{
				dest->mapEnts = reinterpret_cast<MapEnts*>(zone->get_asset_pointer(Types::ASSET_TYPE_MAP_ENTS, this->name()));
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

				zone_buffer::clear_pointer(&dest->stages);
			}

			// copy trigger data from mapents
			auto* map_ents_asset = zone->find_asset(Types::ASSET_TYPE_MAP_ENTS, this->name());
			auto* map_ents = map_ents_asset ? reinterpret_cast<MapEnts*>(map_ents_asset->pointer()) : nullptr;

			if (map_ents)
			{
				std::memcpy(&dest->stageTrigger, &map_ents->trigger, sizeof(MapTriggers));
			}
			else
			{
				std::memset(&dest->stageTrigger, 0, sizeof(MapTriggers));
			}

			const auto write_triggers = [](zone_buffer* buf, MapTriggers* dest)
			{
				if (dest->models)
				{
					dest->models = buf->write_s(3, dest->models, dest->count);
				}

				if (dest->hulls)
				{
					dest->hulls = buf->write_s(3, dest->hulls, dest->hullCount);
				}

				if (dest->slabs)
				{
					dest->slabs = buf->write_s(3, dest->slabs, dest->slabCount);
				}
			};

			//map_ents::write_triggers(buf, &dest->stageTrigger);
			write_triggers(buf, &dest->stageTrigger);

			if (data->dynEntDefList[0])
			{
				buf->align(3);
				auto dyn_entity_def = buf->write(data->dynEntDefList[0], data->dynEntCount[0]);

				for (std::uint16_t i = 0; i < data->dynEntCount[0]; i++)
				{
					if (data->dynEntDefList[0][i].baseModel)
					{
						dyn_entity_def[i].baseModel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
							Types::ASSET_TYPE_XMODEL, data->dynEntDefList[0][i].baseModel->name));
					}
					if (data->dynEntDefList[0][i].destroyFx)
					{
						dyn_entity_def[i].destroyFx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
							Types::ASSET_TYPE_FX, data->dynEntDefList[0][i].destroyFx->name));
					}
					if (data->dynEntDefList[0][i].sound)
					{
						dyn_entity_def[i].sound = reinterpret_cast<snd_alias_list_t*>(zone->get_asset_pointer(
							Types::ASSET_TYPE_SOUND, data->dynEntDefList[0][i].sound->name));
					}
					if (data->dynEntDefList[0][i].physPreset)
					{
						dyn_entity_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
							Types::ASSET_TYPE_PHYSPRESET, data->dynEntDefList[0][i].physPreset->name));
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

				zone_buffer::clear_pointer(&dest->dynEntDefList[0]);
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
							Types::ASSET_TYPE_XMODEL, data->dynEntDefList[1][i].baseModel->name));
					}
					if (data->dynEntDefList[1][i].destroyFx)
					{
						dyn_entity_def[i].destroyFx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
							Types::ASSET_TYPE_FX, data->dynEntDefList[1][i].destroyFx->name));
					}
					if (data->dynEntDefList[1][i].sound)
					{
						dyn_entity_def[i].sound = reinterpret_cast<snd_alias_list_t*>(zone->get_asset_pointer(
							Types::ASSET_TYPE_SOUND, data->dynEntDefList[0][i].sound->name));
					}
					if (data->dynEntDefList[1][i].physPreset)
					{
						dyn_entity_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
							Types::ASSET_TYPE_PHYSPRESET, data->dynEntDefList[1][i].physPreset->name));
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

				zone_buffer::clear_pointer(&dest->dynEntDefList[1]);
			}

			buf->push_stream(2);
			if (data->dynEntPoseList[0])
			{
				buf->align(3);
				buf->write(data->dynEntPoseList[0], data->dynEntCount[0]);
				zone_buffer::clear_pointer(&dest->dynEntPoseList[0]);
			}

			if (data->dynEntPoseList[1])
			{
				buf->align(3);
				buf->write(data->dynEntPoseList[1], data->dynEntCount[1]);
				zone_buffer::clear_pointer(&dest->dynEntPoseList[1]);
			}

			if (data->dynEntClientList[0])
			{
				buf->align(3);
				buf->write(data->dynEntClientList[0], data->dynEntCount[0]);
				zone_buffer::clear_pointer(&dest->dynEntClientList[0]);
			}

			if (data->dynEntClientList[1])
			{
				buf->align(3);
				buf->write(data->dynEntClientList[1], data->dynEntCount[1]);
				zone_buffer::clear_pointer(&dest->dynEntClientList[1]);
			}

			if (data->dynEntCollList[0])
			{
				buf->align(3);
				buf->write(data->dynEntCollList[0], data->dynEntCount[0]);
				zone_buffer::clear_pointer(&dest->dynEntCollList[0]);
			}

			if (data->dynEntCollList[1])
			{
				buf->align(3);
				buf->write(data->dynEntCollList[1], data->dynEntCount[1]);
				zone_buffer::clear_pointer(&dest->dynEntCollList[1]);
			}
			buf->pop_stream();

			if (data->dynEntAnchorNames)
			{
				buf->align(3);
				buf->write(data->dynEntAnchorNames, data->dynEntAnchorCount);
				zone_buffer::clear_pointer(&dest->dynEntAnchorNames);
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
							zone->find_asset(Types::ASSET_TYPE_SCRIPTABLE, scriptable->def->name)->pointer());
					}

					if (scriptable->def && scriptableDef)
					{
						destscriptable->def = reinterpret_cast<ScriptableDef*>(
							zone->get_asset_pointer(Types::ASSET_TYPE_SCRIPTABLE, scriptable->def->name));

						if (scriptable->eventConstantsBuf)
						{
							buf->align(0);
							buf->write(scriptable->eventConstantsBuf, scriptableDef->eventConstantsSize);
							zone_buffer::clear_pointer(&destscriptable->eventConstantsBuf);
						}

						if (scriptable->targetData)
						{
							buf->align(3);
							buf->write(scriptable->targetData, scriptable->targetDataCount);
							zone_buffer::clear_pointer(&destscriptable->targetData);
						}

						buf->push_stream(2);
						if (scriptable->currentModel)
						{
							destscriptable->currentModel = reinterpret_cast<XModel*>(
								zone->get_asset_pointer(Types::ASSET_TYPE_XMODEL, scriptable->currentModel->name));
						}

						if (scriptable->partStates)
						{
							buf->align(3);
							buf->write(scriptable->partStates, scriptableDef->partCount);
							zone_buffer::clear_pointer(&destscriptable->partStates);
						}

						if (scriptable->eventStreamBuf)
						{
							buf->align(0);
							buf->write(scriptable->eventStreamBuf, scriptableDef->eventStreamSize);
							zone_buffer::clear_pointer(&destscriptable->eventStreamBuf);
						}
						buf->pop_stream();
					}
					else
					{
						memset(destscriptable, 0, sizeof(ScriptableInstance));
					}
				}
				zone_buffer::clear_pointer(&dest->scriptableMapEnts.instances);
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
				zone_buffer::clear_pointer(&dest->scriptableMapEnts.animEntries);
			}

			if (data->grappleData.sphereTreeData.sphereTree)
			{
				buf->align(3);
				auto destSphereTree = buf->write(data->grappleData.sphereTreeData.sphereTree, data->grappleData.sphereTreeData.sphereTreeCount);
				for (int i = 0; i < data->grappleData.sphereTreeData.sphereTreeCount; i++)
				{
					if (data->grappleData.sphereTreeData.sphereTree[i].unk)
					{
						buf->align(3);
						buf->write(data->grappleData.sphereTreeData.sphereTree[i].unk, data->grappleData.sphereTreeData.sphereTree[i].unk_count);
						zone_buffer::clear_pointer(&destSphereTree[i].unk);
					}
				}
				zone_buffer::clear_pointer(&dest->grappleData.sphereTreeData.sphereTree);
			}

			if (data->grappleData.sphereTreeData.sphereTreeObj)
			{
				buf->align(3);
				buf->write(data->grappleData.sphereTreeData.sphereTreeObj, data->grappleData.sphereTreeData.sphereTreeObjCount);
				zone_buffer::clear_pointer(&dest->grappleData.sphereTreeData.sphereTreeObj);
			}

			if (data->grappleData.magnet)
			{
				buf->align(3);
				buf->write(data->grappleData.magnet, data->grappleData.magnetCount);
				zone_buffer::clear_pointer(&dest->grappleData.magnet);
			}

			buf->pop_stream();
		}

		static void dump(clipMap_t* asset)
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

			write.dump_array(asset->nodes, asset->numNodes);
			for (unsigned int i = 0; i < asset->numNodes; i++)
			{
				if (asset->nodes[i].plane)
				{
					write.dump_array(asset->nodes[i].plane, 1);
				}
			}

			write.dump_array(asset->leafs, asset->numLeafs);
			write.dump_array(asset->cmodels, asset->numSubModels);

			write.dump_asset(asset->mapEnts);

			write.dump_array(asset->stages, asset->stageCount);
			for (unsigned char i = 0; i < asset->stageCount; i++)
			{
				write.dump_string(asset->stages[i].name);
			}

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
					write.dump_asset(asset->dynEntDefList[i][j].sound);
					write.dump_asset(asset->dynEntDefList[i][j].physPreset);
					write.dump_single(asset->dynEntDefList[i][j].hinge);
					write.dump_single(asset->dynEntDefList[i][j].linkTo);

					//write.dump_single(asset->dynEntClientList[i][j].hinge);
					//write.dump_asset(asset->dynEntClientList[i][j].activeModel);
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
					if (asset->scriptableMapEnts.instances[i].targetData)
					{
						write.dump_array(asset->scriptableMapEnts.instances[i].targetData,
							asset->scriptableMapEnts.instances[i].targetDataCount);
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

			write.dump_array(asset->grappleData.sphereTreeData.sphereTree, asset->grappleData.sphereTreeData.sphereTreeCount);
			if (asset->grappleData.sphereTreeData.sphereTree)
			{
				for (int i = 0; i < asset->grappleData.sphereTreeData.sphereTreeCount; i++)
				{
					write.dump_array(asset->grappleData.sphereTreeData.sphereTree[i].unk, asset->grappleData.sphereTreeData.sphereTree[i].unk_count);
				}
			}

			write.dump_array(asset->grappleData.sphereTreeData.sphereTreeObj, asset->grappleData.sphereTreeData.sphereTreeObjCount);

			write.dump_array(asset->grappleData.magnet, asset->grappleData.magnetCount);

			write.close();
		}
	};
}