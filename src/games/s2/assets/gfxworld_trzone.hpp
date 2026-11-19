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
			reader.read_array(vertices->texelDensities, 3, asset->vertexCount);
			reader.read_array(vertices->colors, 3, asset->vertexCount);
			reader.read_array(vertices->texCoords, 3, asset->vertexCount);
			reader.read_array(vertices->lmapCoords, 3, asset->vertexCount);
			reader.read_array(vertices->normals, 3, asset->vertexCount);
			reader.read_array(vertices->tangents, 3, asset->vertexCount);

			reader.read_array(asset->vld.data, 3, asset->vertexLayerDataSize);
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
			parse_array(read, mem, vertices->texelDensities, asset->vertexCount);
			parse_array(read, mem, vertices->colors, asset->vertexCount);
			parse_array(read, mem, vertices->texCoords, asset->vertexCount);
			parse_array(read, mem, vertices->lmapCoords, asset->vertexCount);
			parse_array(read, mem, vertices->normals, asset->vertexCount);
			parse_array(read, mem, vertices->tangents, asset->vertexCount);

			parse_array(read, mem, asset->vld.data, asset->vertexLayerDataSize);
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
			write_array(buf, 3, vertices->texelDensities, data->vertexCount, dest_vertices->texelDensities);
			write_array(buf, 3, vertices->colors, data->vertexCount, dest_vertices->colors);
			write_array(buf, 3, vertices->texCoords, data->vertexCount, dest_vertices->texCoords);
			write_array(buf, 3, vertices->lmapCoords, data->vertexCount, dest_vertices->lmapCoords);
			write_array(buf, 3, vertices->normals, data->vertexCount, dest_vertices->normals);
			write_array(buf, 3, vertices->tangents, data->vertexCount, dest_vertices->tangents);

			write_array(buf, 3, data->vld.data, data->vertexLayerDataSize, dest->vld.data);

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
			write.dump_array(vertices->texelDensities, asset->vertexCount);
			write.dump_array(vertices->colors, asset->vertexCount);
			write.dump_array(vertices->texCoords, asset->vertexCount);
			write.dump_array(vertices->lmapCoords, asset->vertexCount);
			write.dump_array(vertices->normals, asset->vertexCount);
			write.dump_array(vertices->tangents, asset->vertexCount);

			write.dump_array(asset->vld.data, asset->vertexLayerDataSize);
			write.close();
		}
	};
}
