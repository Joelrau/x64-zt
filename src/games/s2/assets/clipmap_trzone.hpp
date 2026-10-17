#pragma once

namespace zonetool::s2
{
	struct clip_map_trzone_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(ClipMapTrZone, name));
			visit_brushes(w.sub(offsetof(ClipMapTrZone, bCollisionData)));
			visit_patches(w.sub(offsetof(ClipMapTrZone, pCollisionData)));

			const auto models = w.sub(offsetof(ClipMapTrZone, staticModels));
			models.entries(offsetof(ClipMapTrZoneStaticModels, models), sizeof(ClipMapTrZoneStaticModel),
				models.template at<unsigned int>(offsetof(ClipMapTrZoneStaticModels, count)), [](const auto& model, std::size_t)
			{
				model.asset(ASSET_TYPE_XMODEL, offsetof(ClipMapTrZoneStaticModel, model));
			});
		}

		template <typename V>
		static void visit_brushes(const V& data)
		{
			const auto brush_count = data.template at<unsigned int>(offsetof(BrushesCollisionData, numBrushes));
			data.array(S2_ARRAY_FIELD(BrushesCollisionData, brushSides), 3, data.template at<unsigned int>(offsetof(BrushesCollisionData, numBrushSides)));
			data.aligned_entries(offsetof(BrushesCollisionData, brushes), 127, sizeof(cbrush_t), brush_count, [](const auto& brush, std::size_t)
			{
				brush.bytes(offsetof(cbrush_t, sides), 3, sizeof(cbrushside_t));
			});
			data.array(S2_ARRAY_FIELD(BrushesCollisionData, brushBounds), 127, brush_count);
			data.array(S2_ARRAY_FIELD(BrushesCollisionData, brushContents), 7, brush_count);
		}

		template <typename V>
		static void visit_patches(const V& data)
		{
			const auto count = [&](const std::size_t offset)
			{
				return data.template at<unsigned int>(offset);
			};
			const auto tri_count = count(offsetof(PatchesCollisionData, triCount));
			data.array(S2_ARRAY_FIELD(PatchesCollisionData, verts), 3, count(offsetof(PatchesCollisionData, vertCount)));
			data.array(S2_ARRAY_FIELD(PatchesCollisionData, triIndices), 1, 3 * tri_count);
			data.bytes(offsetof(PatchesCollisionData, triEdgeIsWalkable), 0, ((3ull * tri_count + 31) >> 3) & ~3ull);
			data.array(S2_ARRAY_FIELD(PatchesCollisionData, borders), 3, count(offsetof(PatchesCollisionData, borderCount)));
			data.array(S2_ARRAY_FIELD(PatchesCollisionData, partitions), 3, count(offsetof(PatchesCollisionData, partitionCount)));
		}
	};

	class clip_map_trzone : public field_layout_asset<ClipMapTrZone, ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, clip_map_trzone_layout>
	{
	};
}
