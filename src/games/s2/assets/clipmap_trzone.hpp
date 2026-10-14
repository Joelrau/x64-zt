#pragma once

namespace zonetool::s2
{
	class clip_map_trzone
	{
	public:
		static void read_tree(zone_reader& reader, ClipMapTrZoneTree* tree)
		{
			reader.read_array(tree->nodeRefs, 3, tree->nodeRefCount);
			if (const auto nodes = reader.read_array(tree->nodes, 127, tree->nodeCount))
			{
				for (auto i = 0u; i < tree->nodeCount; i++)
				{
					reader.read_stream(nodes[i].data, 3, 8);
				}
			}
			reader.read_array(tree->bounds, 127, tree->nodeCount);
			reader.read_array(tree->nodeData, 7, tree->nodeCount);
		}

		static void read_mesh(zone_reader& reader, ClipMapTrZoneMesh* mesh)
		{
			reader.read_array(mesh->verts, 3, mesh->vertCount);
			reader.read_array(mesh->triIndices, 1, 3 * mesh->triCount);
			reader.read_array(mesh->triEdgeIsWalkable, 0, ((3ull * mesh->triCount + 31) >> 3) & ~3ull);
			reader.read_array(mesh->partitions, 3, mesh->partitionCount);
			reader.read_array(mesh->aabbs, 3, mesh->aabbCount);
		}

		static void read(zone_reader& reader, ClipMapTrZone* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			read_tree(reader, &asset->tree);
			read_mesh(reader, &asset->mesh);
			if (const auto models = reader.read_array(asset->staticModels.models, 7, asset->staticModels.count))
			{
				for (auto i = 0u; i < asset->staticModels.count; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, models[i].model);
				}
			}
			reader.pop_stream();
		}
	};
}
