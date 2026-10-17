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

			const auto brushes = w.sub(offsetof(ClipInfo, brushes));
			brushes.entries(offsetof(ClipBrushInfo, sides), sizeof(cbrushside_t), brushes.template at<unsigned int>(offsetof(ClipBrushInfo, sideCount)), [](const auto& side, std::size_t)
			{
				if (const auto index_count = side.template at<int>(offsetof(cbrushside_t, indexCount)); index_count > 0)
				{
					side.array(S2_ARRAY_FIELD(cbrushside_t, indices), 3, index_count);
				}
			});
			brushes.array(S2_ARRAY_FIELD(ClipBrushInfo, indices), 3, brushes.template at<unsigned int>(offsetof(ClipBrushInfo, indexCount)));

			const auto bounds = w.sub(offsetof(ClipInfo, brushBounds));
			bounds.array(S2_ARRAY_FIELD(ClipBrushBounds, data), 15, bounds.template at<int>(offsetof(ClipBrushBounds, count)));
			const auto leaf_brushes = w.sub(offsetof(ClipInfo, leafBrushes));
			leaf_brushes.array(S2_ARRAY_FIELD(ClipLeafBrushes, data), 3, leaf_brushes.template at<unsigned short>(offsetof(ClipLeafBrushes, count)));

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
