#include <std_include.hpp>
#include "gfxworldtransientzone.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_raw_images(Walk walk, GfxImageRaw* images, const std::size_t count)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				walk.push_stream(XFILE_BLOCK_PHYSICAL);
				walk.array(images[i].pixels, images[i].desc.totalSize, 15);
				walk.pop_stream();
			}
		}

		template <typename Walk>
		void walk_draw_verts(Walk walk, GfxWorldDrawVerts& verts)
		{
			walk.array(verts.posData, verts.posDataSize, 3);
			walk.array(verts.auxData, verts.auxDataSize, 3);
			walk.array(verts.indices, verts.indexCount, 3);
		}

		template <typename Walk>
		void walk_draw_cells(Walk walk, GfxWorldDrawCells& cells)
		{
			const auto tree_counts = walk.array(cells.aabbTreeCounts, cells.cellCount, 3);

			if (const auto cell_trees = walk.array(cells.aabbTrees, cells.cellCount, 7))
			{
				for (auto i = 0u; i < cells.cellCount; i++)
				{
					if (!cell_trees[i].aabbTree)
					{
						continue;
					}

					const auto count = walk.temp_count(tree_counts[i].aabbTreeCount);
					if (const auto trees = walk.array(cell_trees[i].aabbTree, count, 7))
					{
						for (auto j = 0u; j < count; j++)
						{
							walk.array(trees[j].smodelIndexes, trees[j].smodelIndexCount, 1);
						}
					}
				}
			}
		}

		template <typename Walk>
		void walk_lightmaps(Walk walk, GfxLightmapAtlasDataTemp*& field)
		{
			if (const auto lightmaps = walk.array(field, 1, 7))
			{
				for (auto& lightmap : lightmaps->lightmaps)
				{
					walk_raw_images(walk, lightmap.textures, std::size(lightmap.textures));
				}
			}
		}

		template <typename Walk>
		void walk_voxel_tree(Walk walk, GfxGpuLightGridVoxelTree& tree)
		{
			walk.array(tree.treeHeader, 1, 15);
			walk.array(tree.topDownViewNodes, tree.topDownViewNodeCount, 3);
			walk.array(tree.internalNodes, tree.internalNodeCount, 3);
		}

		template <typename Walk>
		void walk_gpu_light_grid(Walk walk, GfxGpuLightGrid*& field)
		{
			const auto grid = walk.array(field, 1, 7);
			if (!grid)
			{
				return;
			}

			walk.array(grid->probes, grid->probeCount, 31);
			walk.array(grid->probePositions, grid->probeCount, 3);
			walk.array(grid->zones, grid->zoneCount, 63);

			if (const auto trees = walk.array(grid->zoneVoxelTrees, grid->zoneCount, 7))
			{
				for (auto i = 0u; i < grid->zoneCount; i++)
				{
					walk_voxel_tree(walk, trees[i]);
				}
			}

			walk.array(grid->tetrahedrons, grid->tetrahedronCount, 63);
			walk.array(grid->tetrahedronNeighbors, grid->tetrahedronCount, 63);
			walk.array(grid->tetrahedronVisibility, grid->tetrahedronCountVisible, 63);
			walk.array(grid->voxelStartTetrahedron, grid->voxelStartTetrahedronCount, 63);
		}

		template <typename Walk>
		void walk_light_grid_volumes(Walk walk, GfxLightGridVolumeAtlasDataTemp*& field)
		{
			const auto volumes = walk.array(field, 1, 7);
			if (!volumes)
			{
				return;
			}

			walk_raw_images(walk, volumes->m_textures, std::size(volumes->m_textures));

			walk.push_stream(XFILE_BLOCK_PHYSICAL);
			walk.array(volumes->m_packingParams, volumes->m_numVolumes, 3);
			walk.pop_stream();

			walk.push_stream(XFILE_BLOCK_PHYSICAL);
			walk.array(volumes->m_transformParams, volumes->m_numVolumes, 3);
			walk.pop_stream();
		}

		template <typename Walk>
		void walk_light_grid(Walk walk, GfxGpuLightGridTransientData& light_grid)
		{
			walk_gpu_light_grid(walk, light_grid.gpuLightGrid);
			walk.array(light_grid.staticModelVolumes, light_grid.staticModelVolumeCount, 3);

			if (const auto zone = walk.array(light_grid.SMLGppZone, 1, 7))
			{
				walk.array(zone->lgvReferences, zone->numOrigSMLGppProbes, 3);
				walk_lightset_probe_data(walk, zone->lgvProbeData);
			}

			walk_light_grid_volumes(walk, light_grid.tempLightGridVolumeData);
		}

		template <typename Walk>
		void walk_reflection_probes(Walk walk, GfxReflectionProbeTransientData*& field)
		{
			const auto probes = walk.array(field, 1, 7);
			if (!probes)
			{
				return;
			}

			walk.array(probes->reflectionProbeInstances, probes->reflectionProbeInstanceCount, 1);

			if (const auto buckets = walk.array(probes->exclusiveProbeBuckets, probes->exclusiveProbeBucketCount, 7))
			{
				for (auto i = 0; i < probes->exclusiveProbeBucketCount; i++)
				{
					auto& bucket = buckets[i];
					walk.push_stream(XFILE_BLOCK_TEMP);
					walk.array(bucket.pixels, bucket.totalSize, 15);
					walk.keep_temp(bucket.pixels, bucket.totalSize);
					walk.pop_stream();
				}
			}
		}

		template <typename Walk>
		void walk_decal_volumes(Walk walk, GfxDecalVolumeTransientData*& field)
		{
			if (const auto decals = walk.array(field, 1, 7))
			{
				walk.array(decals->decalVolumes, decals->decalVolumeCount, 3);
				walk.array(decals->collectionBounds, decals->collectionCount, 3);
				walk.array(decals->decalVolumeUids, decals->decalVolumeCount, 3);
			}
		}

		template <typename Walk>
		void walk_sun_shadow(Walk walk, GfxCompressedSunShadowDataTemp*& field)
		{
			if (const auto shadow = walk.array(field, 1, 7))
			{
				walk.array(shadow->tilesIndices, shadow->tilesCount, 3);
				walk.array(shadow->tilesOffsets, shadow->tilesCount, 3);

				walk.push_stream(XFILE_BLOCK_PHYSICAL);
				walk.array(shadow->tilesData, shadow->tilesDataSize, 0);
				walk.pop_stream();
			}
		}

		template <typename Walk>
		void walk_gfx_world_transient_zone(Walk walk, GfxWorldTransientZone& asset)
		{
			walk_draw_verts(walk, asset.drawVerts);
			walk_draw_cells(walk, asset.drawCells);
			walk_lightmaps(walk, asset.tempLightmapData);
			walk_light_grid(walk, asset.gpuLightGrid);
			walk_reflection_probes(walk, asset.reflectionProbes);
			walk.array(asset.collections, asset.collectionCount, 1);
			walk_decal_volumes(walk, asset.decalVolumes);
			walk_sun_shadow(walk, asset.compressedSunShadow);
		}
	}

	void gfx_world_transient_zone::read(zone_reader& reader, GfxWorldTransientZone* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_gfx_world_transient_zone(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void gfx_world_transient_zone::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_gfx_world_transient_zone(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void gfx_world_transient_zone::dump(GfxWorldTransientZone* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("transient_zones\\"s + asset->name + ".gfxmap_tr"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_gfx_world_transient_zone(dump_walk{dump}, *asset);
		dump.close();
	}

	GfxWorldTransientZone* gfx_world_transient_zone::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("transient_zones\\"s + name + ".gfxmap_tr"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gfxworld_tr \"%s\"...", name.data());

		auto* asset = read.read_single<GfxWorldTransientZone>();
		asset->name = read.read_string();
		walk_gfx_world_transient_zone(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void gfx_world_transient_zone::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<GfxWorldTransientZone>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string gfx_world_transient_zone::name()
	{
		return this->name_;
	}

	std::int32_t gfx_world_transient_zone::type()
	{
		return ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE;
	}
}
