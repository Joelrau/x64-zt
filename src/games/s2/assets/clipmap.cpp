#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_shared_info(zone_reader& reader, ClipInfo*& field)
		{
			reader.push_stream(XFILE_BLOCK_TEMP);
			if (const auto info = reader.read_aliased_array(field, 7))
			{
				clip_map::read_info(reader, info);
			}
			reader.pop_stream();
		}

		void parse_shared_info(assetmanager::reader& read, zone_memory* mem, ClipInfo*& field)
		{
			if (const auto info = parse_array(read, mem, field, 1))
			{
				clip_map::parse_info(read, mem, info);
			}
		}

		void dump_shared_info(assetmanager::dumper& write, ClipInfo* info)
		{
			write.dump_single(info);
			if (info)
			{
				clip_map::dump_info(write, info);
			}
		}

		ClipInfo* reserve_insert_slot(zone_buffer* buf)
		{
			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			buf->align(7);
			auto* slot = buf->get_zone_pointer<ClipInfo>(XFILE_BLOCK_VIRTUAL, buf->stream_offset(XFILE_BLOCK_VIRTUAL));
			buf->inc_stream(XFILE_BLOCK_VIRTUAL, sizeof(void*));
			buf->pop_stream();
			return slot;
		}

		void write_shared_info(zone_base* zone, zone_buffer* buf, std::unordered_map<const ClipInfo*, ClipInfo*>& written,
			ClipInfo* info, ClipInfo*& dest_field)
		{
			if (!info)
			{
				return;
			}

			if (const auto found = written.find(info); found != written.end())
			{
				dest_field = found->second;
				return;
			}

			buf->push_stream(XFILE_BLOCK_TEMP);
			buf->align(7);
			written.emplace(info, reserve_insert_slot(buf));
			auto* dest = buf->write(info);
			buf->insert_pointer(&dest_field);
			clip_map::write_info(zone, buf, info, dest);
			buf->pop_stream();
		}

		void read_dyn_entity_defs(zone_reader& reader, DynEntityDef*& field, const unsigned short count)
		{
			if (const auto defs = reader.read_array(field, 7, count))
			{
				for (auto i = 0; i < count; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, defs[i].xModel);
					reader.read_asset(ASSET_TYPE_FX, defs[i].destroyFx);
					reader.read_asset(ASSET_TYPE_SOUND, defs[i].sound);
					reader.read_asset(ASSET_TYPE_PHYSPRESET, defs[i].physPreset);
					reader.read_array(defs[i].hinge, 3);
					reader.read_array(defs[i].linkTo, 3);
				}
			}
		}

		void parse_dyn_entity_defs(assetmanager::reader& read, zone_memory* mem, DynEntityDef*& field, const unsigned short count)
		{
			if (const auto defs = parse_array(read, mem, field, count))
			{
				for (auto i = 0; i < count; i++)
				{
					defs[i].xModel = read.read_asset<XModel>();
					defs[i].destroyFx = read.read_asset<FxEffectDef>();
					defs[i].sound = read.read_asset<snd_alias_list_t>();
					defs[i].physPreset = read.read_asset<PhysPreset>();
					parse_array(read, mem, defs[i].hinge, 1);
					parse_array(read, mem, defs[i].linkTo, 1);
				}
			}
		}

		void write_dyn_entity_defs(zone_base* zone, zone_buffer* buf, DynEntityDef* defs, const unsigned short count, DynEntityDef*& dest_field)
		{
			if (const auto dest = write_array(buf, 7, defs, count, dest_field))
			{
				for (auto i = 0; i < count; i++)
				{
					dest[i].xModel = asset_reference(zone, ASSET_TYPE_XMODEL, defs[i].xModel);
					dest[i].destroyFx = asset_reference(zone, ASSET_TYPE_FX, defs[i].destroyFx);
					dest[i].sound = asset_reference(zone, ASSET_TYPE_SOUND, defs[i].sound);
					dest[i].physPreset = asset_reference(zone, ASSET_TYPE_PHYSPRESET, defs[i].physPreset);
					write_array(buf, 3, defs[i].hinge, 1, dest[i].hinge);
					write_array(buf, 3, defs[i].linkTo, 1, dest[i].linkTo);
				}
			}
		}

		void dump_dyn_entity_defs(assetmanager::dumper& write, DynEntityDef* defs, const unsigned short count)
		{
			write.dump_array(defs, count);
			for (auto i = 0; defs && i < count; i++)
			{
				write.dump_asset(defs[i].xModel);
				write.dump_asset(defs[i].destroyFx);
				write.dump_asset(defs[i].sound);
				write.dump_asset(defs[i].physPreset);
				write.dump_single(defs[i].hinge);
				write.dump_single(defs[i].linkTo);
			}
		}

		void add_dyn_entity_dependencies(zone_base* zone, DynEntityDef* defs, const unsigned short count)
		{
			for (auto i = 0; defs && i < count; i++)
			{
				add_dependency(zone, ASSET_TYPE_XMODEL, defs[i].xModel);
				add_dependency(zone, ASSET_TYPE_FX, defs[i].destroyFx);
				add_dependency(zone, ASSET_TYPE_SOUND, defs[i].sound);
				add_dependency(zone, ASSET_TYPE_PHYSPRESET, defs[i].physPreset);
			}
		}
	}

	void clip_map::read_info(zone_reader& reader, ClipInfo* info)
	{
		reader.read_array(info->planes, 3, info->planeCount);
		if (const auto materials = reader.read_array(info->materials, 7, info->numMaterials))
		{
			for (auto i = 0u; i < info->numMaterials; i++)
			{
				reader.read_string(materials[i].name);
			}
		}

		auto* brush_tree = &info->bCollisionTree;
		if (const auto nodes = reader.read_array(brush_tree->leafbrushNodes, 7, brush_tree->leafbrushNodesCount))
		{
			for (auto i = 0u; i < brush_tree->leafbrushNodesCount; i++)
			{
				if (nodes[i].leafBrushCount > 0)
				{
					reader.read_array(nodes[i].data.leaf.brushes, 3, nodes[i].leafBrushCount);
				}
			}
		}
		reader.read_array(brush_tree->leafbrushes, 3, brush_tree->numLeafBrushes);

		reader.read_array(info->pCollisionTree.aabbTrees, 15, info->pCollisionTree.aabbTreeCount);
		reader.read_array(info->sCollisionTree.smodelNodes, 3, info->sCollisionTree.smodelNodeCount);

		for (auto& zone : info->trZones)
		{
			reader.read_asset(ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, zone);
		}
	}

	void clip_map::parse_info(assetmanager::reader& read, zone_memory* mem, ClipInfo* info)
	{
		parse_array(read, mem, info->planes, info->planeCount);
		if (const auto materials = parse_array(read, mem, info->materials, info->numMaterials))
		{
			for (auto i = 0u; i < info->numMaterials; i++)
			{
				materials[i].name = read.read_string();
			}
		}

		auto* brush_tree = &info->bCollisionTree;
		if (const auto nodes = parse_array(read, mem, brush_tree->leafbrushNodes, brush_tree->leafbrushNodesCount))
		{
			for (auto i = 0u; i < brush_tree->leafbrushNodesCount; i++)
			{
				if (nodes[i].leafBrushCount > 0)
				{
					parse_array(read, mem, nodes[i].data.leaf.brushes, nodes[i].leafBrushCount);
				}
			}
		}
		parse_array(read, mem, brush_tree->leafbrushes, brush_tree->numLeafBrushes);

		parse_array(read, mem, info->pCollisionTree.aabbTrees, info->pCollisionTree.aabbTreeCount);
		parse_array(read, mem, info->sCollisionTree.smodelNodes, info->sCollisionTree.smodelNodeCount);

		for (auto& zone : info->trZones)
		{
			zone = read.read_asset<ClipMapTrZone>();
		}
	}

	void clip_map::write_info(zone_base* zone, zone_buffer* buf, ClipInfo* info, ClipInfo* dest)
	{
		write_array(buf, 3, info->planes, info->planeCount, dest->planes);
		if (const auto dest_materials = write_array(buf, 7, info->materials, info->numMaterials, dest->materials))
		{
			for (auto i = 0u; i < info->numMaterials; i++)
			{
				write_string(buf, info->materials[i].name, dest_materials[i].name);
			}
		}

		auto* brush_tree = &info->bCollisionTree;
		auto* dest_brush_tree = &dest->bCollisionTree;
		if (const auto dest_nodes = write_array(buf, 7, brush_tree->leafbrushNodes, brush_tree->leafbrushNodesCount, dest_brush_tree->leafbrushNodes))
		{
			for (auto i = 0u; i < brush_tree->leafbrushNodesCount; i++)
			{
				auto* node = &brush_tree->leafbrushNodes[i];
				if (node->leafBrushCount > 0)
				{
					write_array(buf, 3, node->data.leaf.brushes, node->leafBrushCount, dest_nodes[i].data.leaf.brushes);
				}
			}
		}
		write_array(buf, 3, brush_tree->leafbrushes, brush_tree->numLeafBrushes, dest_brush_tree->leafbrushes);

		write_array(buf, 15, info->pCollisionTree.aabbTrees, info->pCollisionTree.aabbTreeCount, dest->pCollisionTree.aabbTrees);
		write_array(buf, 3, info->sCollisionTree.smodelNodes, info->sCollisionTree.smodelNodeCount, dest->sCollisionTree.smodelNodes);

		for (auto i = 0u; i < std::size(info->trZones); i++)
		{
			dest->trZones[i] = asset_reference(zone, ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, info->trZones[i]);
		}
	}

	void clip_map::dump_info(assetmanager::dumper& write, ClipInfo* info)
	{
		write.dump_array(info->planes, info->planeCount);
		write.dump_array(info->materials, info->numMaterials);
		for (auto i = 0u; info->materials && i < info->numMaterials; i++)
		{
			write.dump_string(info->materials[i].name);
		}

		auto* brush_tree = &info->bCollisionTree;
		write.dump_array(brush_tree->leafbrushNodes, brush_tree->leafbrushNodesCount);
		for (auto i = 0u; brush_tree->leafbrushNodes && i < brush_tree->leafbrushNodesCount; i++)
		{
			auto* node = &brush_tree->leafbrushNodes[i];
			if (node->leafBrushCount > 0)
			{
				write.dump_array(node->data.leaf.brushes, node->leafBrushCount);
			}
		}
		write.dump_array(brush_tree->leafbrushes, brush_tree->numLeafBrushes);

		write.dump_array(info->pCollisionTree.aabbTrees, info->pCollisionTree.aabbTreeCount);
		write.dump_array(info->sCollisionTree.smodelNodes, info->sCollisionTree.smodelNodeCount);

		for (auto* zone : info->trZones)
		{
			write.dump_asset(zone);
		}
	}

	void clip_map::add_info_dependencies(zone_base* zone, ClipInfo* info)
	{
		add_dependencies(zone, ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, info->trZones, std::size(info->trZones));
	}

	void clip_map::read(zone_reader& reader, clipMap_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_info(reader, &asset->info);
		read_shared_info(reader, asset->pInfo);

		if (const auto nodes = reader.read_array(asset->nodes, 7, asset->numNodes))
		{
			for (auto i = 0u; i < asset->numNodes; i++)
			{
				reader.read_array(nodes[i].plane, 3);
			}
		}
		reader.read_array(asset->leafs, 7, asset->numLeafs);
		if (const auto models = reader.read_array(asset->cmodels, 7, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				read_shared_info(reader, models[i].info);
			}
		}
		reader.read_asset(ASSET_TYPE_MAP_ENTS, asset->mapEnts);
		if (const auto stages = reader.read_array(asset->stages, 7, asset->stageCount))
		{
			for (auto i = 0; i < asset->stageCount; i++)
			{
				reader.read_string(stages[i].name);
			}
		}
		map_ents::read_triggers(reader, &asset->stageTrigger);

		for (auto i = 0u; i < std::size(asset->dynEntDefList); i++)
		{
			read_dyn_entity_defs(reader, asset->dynEntDefList[i], asset->dynEntCount[i]);
		}
		for (auto i = 0u; i < std::size(asset->dynEntPoseList); i++)
		{
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->dynEntPoseList[i], 3, asset->dynEntCount[i]);
		}
		for (auto i = 0u; i < std::size(asset->dynEntClientList); i++)
		{
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->dynEntClientList[i], 7, asset->dynEntCount[i]);
		}
		for (auto i = 0u; i < std::size(asset->dynEntCollList); i++)
		{
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->dynEntCollList[i], 3, asset->dynEntCount[i]);
		}

		reader.read_script_string_array(asset->scrStrings, asset->scrStringCount);
		if (const auto entries = reader.read_array(asset->scrEntries, 3, asset->scrEntryCount))
		{
			for (auto i = 0u; i < asset->scrEntryCount; i++)
			{
				reader.read_script_string(entries[i].name);
			}
		}
		if (const auto mayhems = reader.read_array(asset->mayhems, 7, asset->mayhemCount))
		{
			for (auto i = 0u; i < asset->mayhemCount; i++)
			{
				reader.read_asset(ASSET_TYPE_MAYHEM, mayhems[i].mayhem);
				reader.read_array(mayhems[i].transform, 3);
				reader.read_script_string(mayhems[i].name);
			}
		}

		reader.pop_stream();
	}

	clipMap_t* clip_map::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_CLIPMAP, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing clipmap \"%s\"...", name.data());

		auto* asset = read.read_single<clipMap_t>();
		asset->name = read.read_string();
		parse_info(read, mem, &asset->info);
		parse_shared_info(read, mem, asset->pInfo);

		if (const auto nodes = parse_array(read, mem, asset->nodes, asset->numNodes))
		{
			for (auto i = 0u; i < asset->numNodes; i++)
			{
				parse_array(read, mem, nodes[i].plane, 1);
			}
		}
		parse_array(read, mem, asset->leafs, asset->numLeafs);
		if (const auto models = parse_array(read, mem, asset->cmodels, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				parse_shared_info(read, mem, models[i].info);
			}
		}
		asset->mapEnts = read.read_asset<MapEnts>();
		if (const auto stages = parse_array(read, mem, asset->stages, asset->stageCount))
		{
			for (auto i = 0; i < asset->stageCount; i++)
			{
				stages[i].name = read.read_string();
			}
		}
		map_ents::parse_triggers(read, mem, &asset->stageTrigger);

		for (auto i = 0u; i < std::size(asset->dynEntDefList); i++)
		{
			parse_dyn_entity_defs(read, mem, asset->dynEntDefList[i], asset->dynEntCount[i]);
		}
		for (auto i = 0u; i < std::size(asset->dynEntPoseList); i++)
		{
			parse_runtime_array(mem, asset->dynEntPoseList[i], asset->dynEntCount[i]);
			parse_runtime_array(mem, asset->dynEntClientList[i], asset->dynEntCount[i]);
			parse_runtime_array(mem, asset->dynEntCollList[i], asset->dynEntCount[i]);
		}

		parse_script_string_array(read, mem, asset->scrStrings, asset->scrStringCount);
		if (const auto entries = parse_array(read, mem, asset->scrEntries, asset->scrEntryCount))
		{
			for (auto i = 0u; i < asset->scrEntryCount; i++)
			{
				entries[i].name = parse_script_string(read);
			}
		}
		if (const auto mayhems = parse_array(read, mem, asset->mayhems, asset->mayhemCount))
		{
			for (auto i = 0u; i < asset->mayhemCount; i++)
			{
				mayhems[i].mayhem = read.read_asset<Mayhem>();
				parse_array(read, mem, mayhems[i].transform, 1);
				mayhems[i].name = parse_script_string(read);
			}
		}
		read.close();

		return asset;
	}

	void clip_map::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		script_string_preparer prepare(buf);

		prepare(data->scrStrings, data->scrStringCount);
		for (auto i = 0u; data->scrEntries && i < data->scrEntryCount; i++)
		{
			prepare(data->scrEntries[i].name);
		}
		for (auto i = 0u; data->mayhems && i < data->mayhemCount; i++)
		{
			prepare(data->mayhems[i].name);
		}
	}

	void clip_map::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		add_info_dependencies(zone, &data->info);
		if (data->pInfo)
		{
			add_info_dependencies(zone, data->pInfo);
		}
		for (auto i = 0u; data->cmodels && i < data->numSubModels; i++)
		{
			if (data->cmodels[i].info)
			{
				add_info_dependencies(zone, data->cmodels[i].info);
			}
		}
		add_dependency(zone, ASSET_TYPE_MAP_ENTS, data->mapEnts);
		for (auto i = 0u; i < std::size(data->dynEntDefList); i++)
		{
			add_dyn_entity_dependencies(zone, data->dynEntDefList[i], data->dynEntCount[i]);
		}
		for (auto i = 0u; data->mayhems && i < data->mayhemCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_MAYHEM, data->mayhems[i].mayhem);
		}
	}

	void clip_map::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);
		std::unordered_map<const ClipInfo*, ClipInfo*> written_infos;

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		write_info(zone, buf, &data->info, &dest->info);
		write_shared_info(zone, buf, written_infos, data->pInfo, dest->pInfo);

		if (const auto dest_nodes = write_array(buf, 7, data->nodes, data->numNodes, dest->nodes))
		{
			for (auto i = 0u; i < data->numNodes; i++)
			{
				write_array(buf, 3, data->nodes[i].plane, 1, dest_nodes[i].plane);
			}
		}
		write_array(buf, 7, data->leafs, data->numLeafs, dest->leafs);
		if (const auto dest_models = write_array(buf, 7, data->cmodels, data->numSubModels, dest->cmodels))
		{
			for (auto i = 0u; i < data->numSubModels; i++)
			{
				write_shared_info(zone, buf, written_infos, data->cmodels[i].info, dest_models[i].info);
			}
		}
		dest->mapEnts = asset_reference(zone, ASSET_TYPE_MAP_ENTS, data->mapEnts);
		if (const auto dest_stages = write_array(buf, 7, data->stages, data->stageCount, dest->stages))
		{
			for (auto i = 0; i < data->stageCount; i++)
			{
				write_string(buf, data->stages[i].name, dest_stages[i].name);
			}
		}
		map_ents::write_triggers(buf, &data->stageTrigger, &dest->stageTrigger);

		for (auto i = 0u; i < std::size(data->dynEntDefList); i++)
		{
			write_dyn_entity_defs(zone, buf, data->dynEntDefList[i], data->dynEntCount[i], dest->dynEntDefList[i]);
		}
		for (auto i = 0u; i < std::size(data->dynEntPoseList); i++)
		{
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->dynEntPoseList[i], data->dynEntCount[i], dest->dynEntPoseList[i]);
		}
		for (auto i = 0u; i < std::size(data->dynEntClientList); i++)
		{
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 7, data->dynEntClientList[i], data->dynEntCount[i], dest->dynEntClientList[i]);
		}
		for (auto i = 0u; i < std::size(data->dynEntCollList); i++)
		{
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->dynEntCollList[i], data->dynEntCount[i], dest->dynEntCollList[i]);
		}

		write_array(buf, 3, data->scrStrings, data->scrStringCount, dest->scrStrings);
		write_array(buf, 3, data->scrEntries, data->scrEntryCount, dest->scrEntries);
		if (const auto dest_mayhems = write_array(buf, 7, data->mayhems, data->mayhemCount, dest->mayhems))
		{
			for (auto i = 0u; i < data->mayhemCount; i++)
			{
				dest_mayhems[i].mayhem = asset_reference(zone, ASSET_TYPE_MAYHEM, data->mayhems[i].mayhem);
				write_array(buf, 3, data->mayhems[i].transform, 1, dest_mayhems[i].transform);
			}
		}

		buf->pop_stream();
	}

	void clip_map::dump(clipMap_t* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_CLIPMAP, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		dump_info(write, &asset->info);
		dump_shared_info(write, asset->pInfo);

		write.dump_array(asset->nodes, asset->numNodes);
		for (auto i = 0u; asset->nodes && i < asset->numNodes; i++)
		{
			write.dump_single(asset->nodes[i].plane);
		}
		write.dump_array(asset->leafs, asset->numLeafs);
		write.dump_array(asset->cmodels, asset->numSubModels);
		for (auto i = 0u; asset->cmodels && i < asset->numSubModels; i++)
		{
			dump_shared_info(write, asset->cmodels[i].info);
		}
		write.dump_asset(asset->mapEnts);
		write.dump_array(asset->stages, asset->stageCount);
		for (auto i = 0; asset->stages && i < asset->stageCount; i++)
		{
			write.dump_string(asset->stages[i].name);
		}
		map_ents::dump_triggers(write, &asset->stageTrigger);

		for (auto i = 0u; i < std::size(asset->dynEntDefList); i++)
		{
			dump_dyn_entity_defs(write, asset->dynEntDefList[i], asset->dynEntCount[i]);
		}

		dump_script_string_array(write, asset->scrStrings, asset->scrStringCount);
		write.dump_array(asset->scrEntries, asset->scrEntryCount);
		for (auto i = 0u; asset->scrEntries && i < asset->scrEntryCount; i++)
		{
			dump_script_string(write, asset->scrEntries[i].name);
		}
		write.dump_array(asset->mayhems, asset->mayhemCount);
		for (auto i = 0u; asset->mayhems && i < asset->mayhemCount; i++)
		{
			write.dump_asset(asset->mayhems[i].mayhem);
			write.dump_single(asset->mayhems[i].transform);
			dump_script_string(write, asset->mayhems[i].name);
		}
		write.close();
	}
}
