#pragma once

namespace zonetool::s2
{
	struct clip_map_trzone_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(ClipMapTrZone, name));
			visit_tree(w.sub(offsetof(ClipMapTrZone, tree)));
			visit_mesh(w.sub(offsetof(ClipMapTrZone, mesh)));

			const auto models = w.sub(offsetof(ClipMapTrZone, staticModels));
			models.entries(offsetof(ClipMapTrZoneStaticModels, models), sizeof(ClipMapTrZoneStaticModel),
				models.template at<unsigned int>(offsetof(ClipMapTrZoneStaticModels, count)), [](const auto& model, std::size_t)
			{
				model.asset(ASSET_TYPE_XMODEL, offsetof(ClipMapTrZoneStaticModel, model));
			});
		}

		template <typename V>
		static void visit_tree(const V& tree)
		{
			const auto node_count = tree.template at<unsigned int>(offsetof(ClipMapTrZoneTree, nodeCount));
			tree.array(S2_ARRAY_FIELD(ClipMapTrZoneTree, nodeRefs), 3, tree.template at<unsigned int>(offsetof(ClipMapTrZoneTree, nodeRefCount)));
			tree.aligned_entries(offsetof(ClipMapTrZoneTree, nodes), 127, sizeof(ClipMapTrZoneNode), node_count, [](const auto& node, std::size_t)
			{
				node.bytes(offsetof(ClipMapTrZoneNode, data), 3, 8);
			});
			tree.array(S2_ARRAY_FIELD(ClipMapTrZoneTree, bounds), 127, node_count);
			tree.array(S2_ARRAY_FIELD(ClipMapTrZoneTree, nodeData), 7, node_count);
		}

		template <typename V>
		static void visit_mesh(const V& mesh)
		{
			const auto count = [&](const std::size_t offset)
			{
				return mesh.template at<unsigned int>(offset);
			};
			const auto tri_count = count(offsetof(ClipMapTrZoneMesh, triCount));
			mesh.array(S2_ARRAY_FIELD(ClipMapTrZoneMesh, verts), 3, count(offsetof(ClipMapTrZoneMesh, vertCount)));
			mesh.array(S2_ARRAY_FIELD(ClipMapTrZoneMesh, triIndices), 1, 3 * tri_count);
			mesh.bytes(offsetof(ClipMapTrZoneMesh, triEdgeIsWalkable), 0, ((3ull * tri_count + 31) >> 3) & ~3ull);
			mesh.array(S2_ARRAY_FIELD(ClipMapTrZoneMesh, partitions), 3, count(offsetof(ClipMapTrZoneMesh, partitionCount)));
			mesh.array(S2_ARRAY_FIELD(ClipMapTrZoneMesh, aabbs), 3, count(offsetof(ClipMapTrZoneMesh, aabbCount)));
		}
	};

	class clip_map_trzone : public field_layout_asset<ClipMapTrZone, ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, clip_map_trzone_layout>
	{
	};
}
