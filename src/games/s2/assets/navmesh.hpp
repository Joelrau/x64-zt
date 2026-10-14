#pragma once

namespace zonetool::s2
{
	class nav_mesh
	{
	public:
		static void read(zone_reader& reader, NavMeshData* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			if (const auto regions = reader.read_array(asset->regions, 7, asset->regionCount))
			{
				for (auto i = 0; i < asset->regionCount; i++)
				{
					auto& region = regions[i];
					reader.read_script_string(region.name);
					reader.read_array(region.data, 3, region.dataSize);
					reader.read_array(region.extraData, 3, region.hasExtraData ? region.dataSize : 0);
				}
			}

			if (const auto edge_lists = reader.read_array(asset->edgeLists, 7, asset->edgeListCount))
			{
				for (auto i = 0; i < asset->edgeListCount; i++)
				{
					auto& list = edge_lists[i];
					if (const auto edges = reader.read_array(list.edges, 7, list.edgeCount))
					{
						for (auto j = 0; j < list.edgeCount; j++)
						{
							reader.read_array(edges[j].points, 3, edges[j].pointCount);
						}
					}
				}
			}

			if (const auto polygons = reader.read_array(asset->polygons, 7, asset->polygonCount))
			{
				for (auto i = 0; i < asset->polygonCount; i++)
				{
					reader.read_array(polygons[i].points, 3, polygons[i].pointCount);
				}
			}

			if (const auto volumes = reader.read_array(asset->volumes, 7, asset->volumeCount))
			{
				for (auto i = 0; i < asset->volumeCount; i++)
				{
					reader.read_array(volumes[i].points, 3, volumes[i].pointCount);
					reader.read_script_string(volumes[i].name);
				}
			}

			if (const auto links = reader.read_array(asset->links, 3, asset->linkCount))
			{
				for (auto i = 0; i < asset->linkCount; i++)
				{
					reader.read_script_strings(links[i].names, 3);
				}
			}

			reader.read_array(asset->nodes, 3, asset->nodeCount);
			reader.read_array(asset->faces, 3, asset->faceCount);

			if (const auto clusters = reader.read_array(asset->clusters, 7, asset->clusterCount))
			{
				for (auto i = 0; i < asset->clusterCount; i++)
				{
					reader.read_array(clusters[i].data, 3, clusters[i].count);
				}
			}

			reader.pop_stream();
		}
	};
}
