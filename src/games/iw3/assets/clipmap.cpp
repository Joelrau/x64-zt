#include <std_include.hpp>
#include "clipmap.hpp"

namespace zonetool::iw3
{
	namespace
	{
		void read_brush(zone_reader32& reader, cbrush_t* brush)
		{
			if (const auto side = reader.read_single(brush->sides, 3))
			{
				reader.read_single(side->plane, 3);
			}

			reader.read_single(brush->baseAdjacentSide, 0);
		}

		void read_pieces(zone_reader32& reader, XModelPieces* pieces)
		{
			reader.read_string(pieces->name);

			if (const auto list = reader.read_array(pieces->pieces, 3, pieces->numpieces))
			{
				for (auto i = 0; i < pieces->numpieces; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, list[i].model);
				}
			}
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

					if (const auto pieces = reader.read_single(entity.destroyPieces, 3))
					{
						read_pieces(reader, pieces);
					}

					reader.read_asset(ASSET_TYPE_PHYSPRESET, entity.physPreset);
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
		reader.read_array(asset->planes, 3, asset->planeCount);

		if (const auto models = reader.read_array(asset->staticModelList, 3, asset->numStaticModels))
		{
			for (auto i = 0u; i < asset->numStaticModels; i++)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, models[i].xmodel);
			}
		}

		reader.read_array(asset->materials, 3, asset->numMaterials);

		if (const auto sides = reader.read_array(asset->brushsides, 3, asset->numBrushSides))
		{
			for (auto i = 0u; i < asset->numBrushSides; i++)
			{
				reader.read_single(sides[i].plane, 3);
			}
		}

		reader.read_array(asset->brushEdges, 0, asset->numBrushEdges);

		if (const auto nodes = reader.read_array(asset->nodes, 3, asset->numNodes))
		{
			for (auto i = 0u; i < asset->numNodes; i++)
			{
				reader.read_single(nodes[i].plane, 3);
			}
		}

		reader.read_array(asset->leafs, 3, asset->numLeafs);
		reader.read_array(asset->leafbrushes, 1, asset->numLeafBrushes);

		if (const auto nodes = reader.read_array(asset->leafbrushNodes, 3, asset->leafbrushNodesCount))
		{
			for (auto i = 0u; i < asset->leafbrushNodesCount; i++)
			{
				if (nodes[i].leafBrushCount > 0)
				{
					reader.read_array(nodes[i].data.leaf.brushes, 1, nodes[i].leafBrushCount);
				}
			}
		}

		reader.read_array(asset->leafsurfaces, 3, asset->numLeafSurfaces);
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

		reader.read_array(asset->aabbTrees, 3, asset->aabbTreeCount);
		reader.read_array(asset->cmodels, 3, asset->numSubModels);

		if (const auto brushes = reader.read_array(asset->brushes, 15, asset->numBrushes))
		{
			for (auto i = 0; i < asset->numBrushes; i++)
			{
				read_brush(reader, &brushes[i]);
			}
		}

		reader.read_array(asset->visibility, 0, asset->numClusters * asset->clusterBytes);
		reader.read_asset(ASSET_TYPE_MAP_ENTS, asset->mapEnts);

		if (const auto brush = reader.read_single(asset->box_brush, 15))
		{
			read_brush(reader, brush);
		}

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
