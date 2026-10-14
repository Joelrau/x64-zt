#pragma once

namespace zonetool::s2
{
	class gfx_map_trzone
	{
	public:
		static void read(zone_reader& reader, GfxMapTrZone* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			auto& data = asset->vertices.data;
			const auto count = asset->vertexCount;
			reader.read_array(data.positions, 3, count);
			reader.read_array(data.data0, 3, count);
			reader.read_array(data.data1, 3, count);
			reader.read_array(data.data2, 3, count);
			reader.read_array(data.data3, 3, count);
			reader.read_array(data.data4, 3, count);
			reader.read_array(data.data5, 3, count);

			reader.read_array(asset->indices.data, 3, asset->indexDataSize);
			reader.pop_stream();
		}
	};
}
