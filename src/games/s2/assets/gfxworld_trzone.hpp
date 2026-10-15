#pragma once

namespace zonetool::s2
{
	struct gfx_map_trzone_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(GfxMapTrZone, name));

			const auto count = w.template at<unsigned int>(offsetof(GfxMapTrZone, vertexCount));
			const auto data = w.sub(offsetof(GfxMapTrZone, vertices) + offsetof(GfxMapTrZoneVertexBuffer, data));
			data.array(S2_ARRAY_FIELD(GfxMapTrZoneData, positions), 3, count);
			data.array(S2_ARRAY_FIELD(GfxMapTrZoneData, data0), 3, count);
			data.array(S2_ARRAY_FIELD(GfxMapTrZoneData, data1), 3, count);
			data.array(S2_ARRAY_FIELD(GfxMapTrZoneData, data2), 3, count);
			data.array(S2_ARRAY_FIELD(GfxMapTrZoneData, data3), 3, count);
			data.array(S2_ARRAY_FIELD(GfxMapTrZoneData, data4), 3, count);
			data.array(S2_ARRAY_FIELD(GfxMapTrZoneData, data5), 3, count);

			w.bytes(offsetof(GfxMapTrZone, indices) + offsetof(GfxMapTrZoneIndexBuffer, data), 3,
				w.template at<unsigned int>(offsetof(GfxMapTrZone, indexDataSize)));
		}
	};

	class gfx_map_trzone : public field_layout_asset<GfxMapTrZone, ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, gfx_map_trzone_layout>
	{
	};
}
