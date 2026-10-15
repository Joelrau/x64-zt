#pragma once

namespace zonetool::s2
{
	struct nav_mesh_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			const auto count = [&](const std::size_t offset)
			{
				return w.template at<int>(offset);
			};

			w.string(offsetof(NavMeshData, name));
			w.entries(offsetof(NavMeshData, regions), sizeof(NavMeshRegion), count(offsetof(NavMeshData, regionCount)), [](const auto& region, std::size_t)
			{
				const auto data_size = region.template at<int>(offsetof(NavMeshRegion, dataSize));
				region.script_string(offsetof(NavMeshRegion, name));
				region.bytes(offsetof(NavMeshRegion, data), 3, data_size);
				region.bytes(offsetof(NavMeshRegion, extraData), 3, region.template at<bool>(offsetof(NavMeshRegion, hasExtraData)) ? data_size : 0);
			});
			w.entries(offsetof(NavMeshData, edgeLists), sizeof(NavMeshEdgeList), count(offsetof(NavMeshData, edgeListCount)), [](const auto& list, std::size_t)
			{
				list.entries(offsetof(NavMeshEdgeList, edges), sizeof(NavMeshEdge), list.template at<int>(offsetof(NavMeshEdgeList, edgeCount)), [](const auto& edge, std::size_t)
				{
					edge.array(S2_ARRAY_FIELD(NavMeshEdge, points), 3, edge.template at<int>(offsetof(NavMeshEdge, pointCount)));
				});
			});
			w.entries(offsetof(NavMeshData, polygons), sizeof(NavMeshPolygon), count(offsetof(NavMeshData, polygonCount)), [](const auto& polygon, std::size_t)
			{
				polygon.array(S2_ARRAY_FIELD(NavMeshPolygon, points), 3, polygon.template at<int>(offsetof(NavMeshPolygon, pointCount)));
			});
			w.entries(offsetof(NavMeshData, volumes), sizeof(NavMeshVolume), count(offsetof(NavMeshData, volumeCount)), [](const auto& volume, std::size_t)
			{
				volume.array(S2_ARRAY_FIELD(NavMeshVolume, points), 3, volume.template at<int>(offsetof(NavMeshVolume, pointCount)));
				volume.script_string(offsetof(NavMeshVolume, name));
			});
			w.aligned_entries(offsetof(NavMeshData, links), 3, sizeof(NavMeshLink), count(offsetof(NavMeshData, linkCount)), [](const auto& link, std::size_t)
			{
				for (auto i = 0u; i < std::size(NavMeshLink{}.names); i++)
				{
					link.script_string(offsetof(NavMeshLink, names) + i * sizeof(scr_string_t));
				}
			});
			w.array(S2_ARRAY_FIELD(NavMeshData, nodes), 3, count(offsetof(NavMeshData, nodeCount)));
			w.array(S2_ARRAY_FIELD(NavMeshData, faces), 3, count(offsetof(NavMeshData, faceCount)));
			w.entries(offsetof(NavMeshData, clusters), sizeof(NavMeshCluster), count(offsetof(NavMeshData, clusterCount)), [](const auto& cluster, std::size_t)
			{
				cluster.array(S2_ARRAY_FIELD(NavMeshCluster, data), 3, cluster.template at<int>(offsetof(NavMeshCluster, count)));
			});
		}
	};

	class nav_mesh : public field_layout_asset<NavMeshData, ASSET_TYPE_NAVMESH, nav_mesh_layout>
	{
	};
}
