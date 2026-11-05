#pragma once

namespace zonetool::s2
{
	class clip_map_trzone : public asset_base<ClipMapTrZone, ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE>
	{
	public:
		static void read(zone_reader& reader, ClipMapTrZone* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			auto* brushes = &asset->bCollisionData;
			reader.read_array(brushes->brushSides, 3, brushes->numBrushSides);
			if (const auto brush_list = reader.read_array(brushes->brushes, 127, brushes->numBrushes))
			{
				for (auto i = 0u; i < brushes->numBrushes; i++)
				{
					reader.read_array(brush_list[i].sides, 3);
				}
			}
			reader.read_array(brushes->brushBounds, 127, brushes->numBrushes);
			reader.read_array(brushes->brushContents, 7, brushes->numBrushes);

			auto* patches = &asset->pCollisionData;
			reader.read_array(patches->verts, 3, patches->vertCount);
			reader.read_array(patches->triIndices, 1, tri_index_count(patches));
			reader.read_array(patches->triEdgeIsWalkable, 0, walkable_size(patches));
			reader.read_array(patches->borders, 3, patches->borderCount);
			reader.read_array(patches->partitions, 3, patches->partitionCount);

			auto* models = &asset->staticModels;
			if (const auto model_list = reader.read_array(models->models, 7, models->count))
			{
				for (auto i = 0u; i < models->count; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, model_list[i].model);
				}
			}

			reader.pop_stream();
		}

		ClipMapTrZone* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing clipmap_trzone \"%s\"...", name.data());

			auto* asset = read.read_single<ClipMapTrZone>();
			asset->name = read.read_string();

			auto* brushes = &asset->bCollisionData;
			parse_array(read, mem, brushes->brushSides, brushes->numBrushSides);
			if (const auto brush_list = parse_array(read, mem, brushes->brushes, brushes->numBrushes))
			{
				for (auto i = 0u; i < brushes->numBrushes; i++)
				{
					parse_array(read, mem, brush_list[i].sides, 1);
				}
			}
			parse_array(read, mem, brushes->brushBounds, brushes->numBrushes);
			parse_array(read, mem, brushes->brushContents, brushes->numBrushes);

			auto* patches = &asset->pCollisionData;
			parse_array(read, mem, patches->verts, patches->vertCount);
			parse_array(read, mem, patches->triIndices, tri_index_count(patches));
			parse_array(read, mem, patches->triEdgeIsWalkable, walkable_size(patches));
			parse_array(read, mem, patches->borders, patches->borderCount);
			parse_array(read, mem, patches->partitions, patches->partitionCount);

			auto* models = &asset->staticModels;
			if (const auto model_list = parse_array(read, mem, models->models, models->count))
			{
				for (auto i = 0u; i < models->count; i++)
				{
					model_list[i].model = read.read_asset<XModel>();
				}
			}
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			const auto* models = &this->asset_->staticModels;
			for (auto i = 0u; models->models && i < models->count; i++)
			{
				add_dependency(zone, ASSET_TYPE_XMODEL, models->models[i].model);
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);

			auto* brushes = &data->bCollisionData;
			auto* dest_brushes = &dest->bCollisionData;
			write_array(buf, 3, brushes->brushSides, brushes->numBrushSides, dest_brushes->brushSides);
			if (const auto dest_brush_list = write_array(buf, 127, brushes->brushes, brushes->numBrushes, dest_brushes->brushes))
			{
				for (auto i = 0u; i < brushes->numBrushes; i++)
				{
					write_array(buf, 3, brushes->brushes[i].sides, 1, dest_brush_list[i].sides);
				}
			}
			write_array(buf, 127, brushes->brushBounds, brushes->numBrushes, dest_brushes->brushBounds);
			write_array(buf, 7, brushes->brushContents, brushes->numBrushes, dest_brushes->brushContents);

			auto* patches = &data->pCollisionData;
			auto* dest_patches = &dest->pCollisionData;
			write_array(buf, 3, patches->verts, patches->vertCount, dest_patches->verts);
			write_array(buf, 1, patches->triIndices, tri_index_count(patches), dest_patches->triIndices);
			write_array(buf, 0, patches->triEdgeIsWalkable, walkable_size(patches), dest_patches->triEdgeIsWalkable);
			write_array(buf, 3, patches->borders, patches->borderCount, dest_patches->borders);
			write_array(buf, 3, patches->partitions, patches->partitionCount, dest_patches->partitions);

			auto* models = &data->staticModels;
			if (const auto dest_models = write_array(buf, 7, models->models, models->count, dest->staticModels.models))
			{
				for (auto i = 0u; i < models->count; i++)
				{
					dest_models[i].model = asset_reference(zone, ASSET_TYPE_XMODEL, models->models[i].model);
				}
			}

			buf->pop_stream();
		}

		static void dump(ClipMapTrZone* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);

			auto* brushes = &asset->bCollisionData;
			write.dump_array(brushes->brushSides, brushes->numBrushSides);
			write.dump_array(brushes->brushes, brushes->numBrushes);
			for (auto i = 0u; brushes->brushes && i < brushes->numBrushes; i++)
			{
				write.dump_single(brushes->brushes[i].sides);
			}
			write.dump_array(brushes->brushBounds, brushes->numBrushes);
			write.dump_array(brushes->brushContents, brushes->numBrushes);

			auto* patches = &asset->pCollisionData;
			write.dump_array(patches->verts, patches->vertCount);
			write.dump_array(patches->triIndices, tri_index_count(patches));
			write.dump_array(patches->triEdgeIsWalkable, walkable_size(patches));
			write.dump_array(patches->borders, patches->borderCount);
			write.dump_array(patches->partitions, patches->partitionCount);

			auto* models = &asset->staticModels;
			write.dump_array(models->models, models->count);
			for (auto i = 0u; models->models && i < models->count; i++)
			{
				write.dump_asset(models->models[i].model);
			}
			write.close();
		}

	private:
		static unsigned int tri_index_count(const PatchesCollisionData* patches)
		{
			return 3 * patches->triCount;
		}

		static unsigned int walkable_size(const PatchesCollisionData* patches)
		{
			return ((3 * patches->triCount + 31) >> 3) & ~3u;
		}
	};
}
