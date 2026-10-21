#pragma once

namespace zonetool::s2
{
	class gfx_map_trzone : public asset_base<GfxMapTrZone, ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE>
	{
	public:
		static void read(zone_reader& reader, GfxMapTrZone* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			auto* vertices = &asset->vertices.data;
			reader.read_array(vertices->positions, 3, asset->vertexCount);
			reader.read_array(vertices->data0, 3, asset->vertexCount);
			reader.read_array(vertices->data1, 3, asset->vertexCount);
			reader.read_array(vertices->data2, 3, asset->vertexCount);
			reader.read_array(vertices->data3, 3, asset->vertexCount);
			reader.read_array(vertices->data4, 3, asset->vertexCount);
			reader.read_array(vertices->data5, 3, asset->vertexCount);

			reader.read_array(asset->indices.data, 3, asset->indexDataSize);
			reader.pop_stream();
		}

		GfxMapTrZone* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing gfxworld transient zone \"%s\"...", name.data());

			auto* asset = read.read_single<GfxMapTrZone>();
			asset->name = read.read_string();

			auto* vertices = &asset->vertices.data;
			parse_array(read, mem, vertices->positions, asset->vertexCount);
			parse_array(read, mem, vertices->data0, asset->vertexCount);
			parse_array(read, mem, vertices->data1, asset->vertexCount);
			parse_array(read, mem, vertices->data2, asset->vertexCount);
			parse_array(read, mem, vertices->data3, asset->vertexCount);
			parse_array(read, mem, vertices->data4, asset->vertexCount);
			parse_array(read, mem, vertices->data5, asset->vertexCount);

			parse_array(read, mem, asset->indices.data, asset->indexDataSize);
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);

			auto* vertices = &data->vertices.data;
			auto* dest_vertices = &dest->vertices.data;
			write_array(buf, 3, vertices->positions, data->vertexCount, dest_vertices->positions);
			write_array(buf, 3, vertices->data0, data->vertexCount, dest_vertices->data0);
			write_array(buf, 3, vertices->data1, data->vertexCount, dest_vertices->data1);
			write_array(buf, 3, vertices->data2, data->vertexCount, dest_vertices->data2);
			write_array(buf, 3, vertices->data3, data->vertexCount, dest_vertices->data3);
			write_array(buf, 3, vertices->data4, data->vertexCount, dest_vertices->data4);
			write_array(buf, 3, vertices->data5, data->vertexCount, dest_vertices->data5);

			write_array(buf, 3, data->indices.data, data->indexDataSize, dest->indices.data);

			buf->pop_stream();
		}

		static void dump(GfxMapTrZone* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);

			auto* vertices = &asset->vertices.data;
			write.dump_array(vertices->positions, asset->vertexCount);
			write.dump_array(vertices->data0, asset->vertexCount);
			write.dump_array(vertices->data1, asset->vertexCount);
			write.dump_array(vertices->data2, asset->vertexCount);
			write.dump_array(vertices->data3, asset->vertexCount);
			write.dump_array(vertices->data4, asset->vertexCount);
			write.dump_array(vertices->data5, asset->vertexCount);

			write.dump_array(asset->indices.data, asset->indexDataSize);
			write.close();
		}
	};
}
