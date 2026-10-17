#pragma once

namespace zonetool::s2
{
	struct clip_info_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.array(S2_ARRAY_FIELD(ClipInfo, planes), 3, w.template at<int>(offsetof(ClipInfo, planeCount)));
			w.entries(offsetof(ClipInfo, materials), sizeof(ClipMaterial), w.template at<unsigned int>(offsetof(ClipInfo, numMaterials)), [](const auto& material, std::size_t)
			{
				material.string(offsetof(ClipMaterial, name));
			});

			const auto brush_tree = w.sub(offsetof(ClipInfo, bCollisionTree));
			brush_tree.entries(offsetof(BrushesCollisionTree, leafbrushNodes), sizeof(cLeafBrushNode_s), brush_tree.template at<unsigned int>(offsetof(BrushesCollisionTree, leafbrushNodesCount)), [](const auto& node, std::size_t)
			{
				if (const auto brush_count = node.template at<int>(offsetof(cLeafBrushNode_s, leafBrushCount)); brush_count > 0)
				{
					node.array(S2_ARRAY_FIELD(cLeafBrushNode_s, data.leaf.brushes), 3, brush_count);
				}
			});
			brush_tree.array(S2_ARRAY_FIELD(BrushesCollisionTree, leafbrushes), 3, brush_tree.template at<unsigned int>(offsetof(BrushesCollisionTree, numLeafBrushes)));

			const auto patch_tree = w.sub(offsetof(ClipInfo, pCollisionTree));
			patch_tree.array(S2_ARRAY_FIELD(PatchesCollisionTree, aabbTrees), 15, patch_tree.template at<int>(offsetof(PatchesCollisionTree, aabbTreeCount)));
			const auto smodel_tree = w.sub(offsetof(ClipInfo, sCollisionTree));
			smodel_tree.array(S2_ARRAY_FIELD(SModelsCollisionTree, smodelNodes), 3, smodel_tree.template at<unsigned short>(offsetof(SModelsCollisionTree, smodelNodeCount)));

			w.assets(ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, offsetof(ClipInfo, trZones), std::size(ClipInfo{}.trZones));
		}
	};

	struct clip_map_layout
	{
		template <typename V>
		static void visit(const V& w);
	};

	class clip_map : public field_layout_asset<clipMap_t, ASSET_TYPE_CLIPMAP, clip_map_layout>
	{
	};
}
