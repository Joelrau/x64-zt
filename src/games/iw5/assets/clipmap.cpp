#include <std_include.hpp>
#include "clipmap.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_brush(zone_reader32& reader, cbrush_t& brush)
		{
			if (const auto side = reader.read_single(brush.sides, 3))
			{
				reader.read_single(side->plane, 3);
			}

			reader.read_single(brush.baseAdjacentSide, 0);
		}

		void read_info(zone_reader32& reader, ClipInfo& info)
		{
			reader.read_array(info.planes, 3, info.planeCount);

			if (const auto materials = reader.read_array(info.materials, 3, info.numMaterials))
			{
				for (auto i = 0u; i < info.numMaterials; i++)
				{
					reader.read_string(materials[i].name);
				}
			}

			if (const auto sides = reader.read_array(info.brushsides, 3, info.numBrushSides))
			{
				for (auto i = 0u; i < info.numBrushSides; i++)
				{
					reader.read_single(sides[i].plane, 3);
				}
			}

			reader.read_array(info.brushEdges, 0, info.numBrushEdges);

			if (const auto nodes = reader.read_array(info.leafbrushNodes, 3, info.leafbrushNodesCount))
			{
				for (auto i = 0u; i < info.leafbrushNodesCount; i++)
				{
					if (nodes[i].leafBrushCount > 0)
					{
						reader.read_array(nodes[i].data.leaf.brushes, 1, nodes[i].leafBrushCount);
					}
				}
			}

			reader.read_array(info.leafbrushes, 1, info.numLeafBrushes);

			if (const auto brushes = reader.read_array(info.brushes, 127, info.numBrushes))
			{
				for (auto i = 0; i < info.numBrushes; i++)
				{
					read_brush(reader, brushes[i]);
				}
			}

			reader.read_array(info.brushBounds, 127, info.numBrushes);
			reader.read_array(info.brushContents, 3, info.numBrushes);
		}

		void read_info_reference(zone_reader32& reader, x86::pointer<ClipInfo>& field)
		{
			reader.push_stream(XFILE_BLOCK_TEMP);
			if (const auto info = reader.read_aliased_array(field, 3))
			{
				read_info(reader, *info);
			}
			reader.pop_stream();
		}

		void read_triggers(zone_reader32& reader, MapTriggers& trigger)
		{
			reader.read_array(trigger.models, 3, trigger.count);
			reader.read_array(trigger.hulls, 3, trigger.hullCount);
			reader.read_array(trigger.slabs, 3, trigger.slabCount);
		}

		void read_dynamic_entities(zone_reader32& reader, clipMap_t* asset, const std::size_t index)
		{
			const auto count = asset->dynEntCount[index];
			if (const auto entities = reader.read_array(asset->dynEntDefList[index], 3, count))
			{
				for (auto i = 0; i < count; i++)
				{
					auto& entity = entities[i];
					reader.read_asset(ASSET_TYPE_XMODEL, entity.xModel);
					reader.read_asset(ASSET_TYPE_FX, entity.destroyFx);
					reader.read_asset(ASSET_TYPE_PHYSPRESET, entity.physPreset);
					reader.read_single(entity.hinge, 3);
				}
			}
		}

		template <typename T>
		void read_runtime(zone_reader32& reader, x86::pointer<T>& field, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(field, 3, count);
			reader.pop_stream();
		}
	}

	void clip_map::read(zone_reader32& reader, clipMap_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_info(reader, asset->info);
		read_info_reference(reader, asset->pInfo);

		if (const auto models = reader.read_array(asset->staticModelList, 3, asset->numStaticModels))
		{
			for (auto i = 0u; i < asset->numStaticModels; i++)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, models[i].xmodel);
			}
		}

		if (const auto nodes = reader.read_array(asset->nodes, 3, asset->numNodes))
		{
			for (auto i = 0u; i < asset->numNodes; i++)
			{
				reader.read_single(nodes[i].plane, 3);
			}
		}

		reader.read_array(asset->leafs, 3, asset->numLeafs);
		reader.read_array(asset->verts, 3, asset->vertCount);
		reader.read_array(asset->triIndices, 1, 3 * asset->triCount);
		reader.read_array(asset->triEdgeIsWalkable, 0, (3 * asset->triCount + 31) / 32 * 4);
		reader.read_array(asset->borders, 3, asset->borderCount);

		if (const auto partitions = reader.read_array(asset->partitions, 3, asset->partitionCount))
		{
			for (auto i = 0; i < asset->partitionCount; i++)
			{
				reader.read_single(partitions[i].borders, 3);
			}
		}

		reader.read_array(asset->aabbTrees, 15, asset->aabbTreeCount);

		if (const auto models = reader.read_array(asset->cmodels, 3, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				read_info_reference(reader, models[i].info);
			}
		}

		reader.read_array(asset->smodelNodes, 3, asset->smodelNodeCount);
		reader.read_asset(ASSET_TYPE_MAP_ENTS, asset->mapEnts);

		if (const auto stages = reader.read_array(asset->stages, 3, asset->stageCount))
		{
			for (auto i = 0; i < asset->stageCount; i++)
			{
				reader.read_string(stages[i].name);
			}
		}

		read_triggers(reader, asset->stageTrigger);
		read_dynamic_entities(reader, asset, 0);
		read_dynamic_entities(reader, asset, 1);

		for (auto i = 0; i < 2; i++)
		{
			read_runtime(reader, asset->dynEntPoseList[i], asset->dynEntCount[i]);
		}
		for (auto i = 0; i < 2; i++)
		{
			read_runtime(reader, asset->dynEntClientList[i], asset->dynEntCount[i]);
		}
		for (auto i = 0; i < 2; i++)
		{
			read_runtime(reader, asset->dynEntCollList[i], asset->dynEntCount[i]);
		}

		reader.pop_stream();
	}
}
