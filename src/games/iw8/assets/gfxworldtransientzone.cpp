#include <std_include.hpp>
#include "gfxworldtransientzone.hpp"

namespace zonetool::iw8
{
	void read_lightset_probe_data(zone_reader& reader, GfxDynamicLightsetProbeData& data)
	{
		reader.read_array(data.chunkG, 3, data.numGChunk);
		reader.read_array(data.chunkGBF, 1, data.numGChunkBF);
		reader.read_array(data.origProbes, 31, data.numOrigProbes);
		reader.read_array(data.bfProbes, 31, data.numBFProbes);
		reader.read_array(data.skipRecords, 1, data.numSkipRecords);
		reader.read_array(data.palletteRecords, 3, data.numSkipRecords);
		reader.read_array(data.tempRemap, 3, data.numOrigProbes);
	}

	namespace
	{
		void read_raw_images(zone_reader& reader, GfxImageRaw* images, const std::size_t count)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				reader.push_stream(XFILE_BLOCK_PHYSICAL);
				reader.read_array(images[i].pixels, 15, images[i].desc.totalSize);
				reader.pop_stream();
			}
		}

		void read_draw_verts(zone_reader& reader, GfxWorldDrawVerts& verts)
		{
			reader.read_array(verts.posData, 3, verts.posDataSize);
			reader.read_array(verts.auxData, 3, verts.auxDataSize);
			reader.read_array(verts.indices, 3, verts.indexCount);
		}

		unsigned int read_aabb_tree_count(zone_reader& reader)
		{
			reader.push_stream(XFILE_BLOCK_TEMP);
			reader.align(3);
			const auto count = *reader.read<unsigned int>();
			reader.pop_stream();
			return count;
		}

		void read_draw_cells(zone_reader& reader, GfxWorldDrawCells& cells)
		{
			reader.read_array(cells.aabbTreeCounts, 3, cells.cellCount);

			if (const auto cell_trees = reader.read_array(cells.aabbTrees, 7, cells.cellCount))
			{
				for (auto i = 0u; i < cells.cellCount; i++)
				{
					if (!cell_trees[i].aabbTree)
					{
						continue;
					}

					const auto count = read_aabb_tree_count(reader);
					if (const auto trees = reader.read_array(cell_trees[i].aabbTree, 7, count))
					{
						for (auto j = 0u; j < count; j++)
						{
							reader.read_array(trees[j].smodelIndexes, 1, trees[j].smodelIndexCount);
						}
					}
				}
			}
		}

		void read_lightmaps(zone_reader& reader, GfxLightmapAtlasDataTemp*& field)
		{
			if (const auto lightmaps = reader.read_single(field, 7))
			{
				for (auto& lightmap : lightmaps->lightmaps)
				{
					read_raw_images(reader, lightmap.textures, std::size(lightmap.textures));
				}
			}
		}

		void read_voxel_tree(zone_reader& reader, GfxGpuLightGridVoxelTree& tree)
		{
			reader.read_single(tree.treeHeader, 15);
			reader.read_array(tree.topDownViewNodes, 3, tree.topDownViewNodeCount);
			reader.read_array(tree.internalNodes, 3, tree.internalNodeCount);
		}

		void read_gpu_light_grid(zone_reader& reader, GfxGpuLightGrid*& field)
		{
			const auto grid = reader.read_single(field, 7);
			if (!grid)
			{
				return;
			}

			reader.read_array(grid->probes, 31, grid->probeCount);
			reader.read_array(grid->probePositions, 3, grid->probeCount);
			reader.read_array(grid->zones, 63, grid->zoneCount);

			if (const auto trees = reader.read_array(grid->zoneVoxelTrees, 7, grid->zoneCount))
			{
				for (auto i = 0u; i < grid->zoneCount; i++)
				{
					read_voxel_tree(reader, trees[i]);
				}
			}

			reader.read_array(grid->tetrahedrons, 63, grid->tetrahedronCount);
			reader.read_array(grid->tetrahedronNeighbors, 63, grid->tetrahedronCount);
			reader.read_array(grid->tetrahedronVisibility, 63, grid->tetrahedronCountVisible);
			reader.read_array(grid->voxelStartTetrahedron, 63, grid->voxelStartTetrahedronCount);
		}

		void read_light_grid_volumes(zone_reader& reader, GfxLightGridVolumeAtlasDataTemp*& field)
		{
			const auto volumes = reader.read_single(field, 7);
			if (!volumes)
			{
				return;
			}

			read_raw_images(reader, volumes->m_textures, std::size(volumes->m_textures));

			reader.push_stream(XFILE_BLOCK_PHYSICAL);
			reader.read_array(volumes->m_packingParams, 3, volumes->m_numVolumes);
			reader.pop_stream();

			reader.push_stream(XFILE_BLOCK_PHYSICAL);
			reader.read_array(volumes->m_transformParams, 3, volumes->m_numVolumes);
			reader.pop_stream();
		}

		void read_light_grid(zone_reader& reader, GfxGpuLightGridTransientData& light_grid)
		{
			read_gpu_light_grid(reader, light_grid.gpuLightGrid);
			reader.read_array(light_grid.staticModelVolumes, 3, light_grid.staticModelVolumeCount);

			if (const auto zone = reader.read_single(light_grid.SMLGppZone, 7))
			{
				reader.read_array(zone->lgvReferences, 3, zone->numOrigSMLGppProbes);
				read_lightset_probe_data(reader, zone->lgvProbeData);
			}

			read_light_grid_volumes(reader, light_grid.tempLightGridVolumeData);
		}

		void read_reflection_probes(zone_reader& reader, GfxReflectionProbeTransientData*& field)
		{
			const auto probes = reader.read_single(field, 7);
			if (!probes)
			{
				return;
			}

			reader.read_array(probes->reflectionProbeInstances, 1, probes->reflectionProbeInstanceCount);

			if (const auto buckets = reader.read_array(probes->exclusiveProbeBuckets, 7, probes->exclusiveProbeBucketCount))
			{
				for (auto i = 0; i < probes->exclusiveProbeBucketCount; i++)
				{
					auto& bucket = buckets[i];
					reader.push_stream(XFILE_BLOCK_TEMP);
					if (const auto pixels = reader.read_array(bucket.pixels, 15, bucket.totalSize))
					{
						bucket.pixels = reader.duplicate(pixels, bucket.totalSize);
					}
					reader.pop_stream();
				}
			}
		}

		void read_decal_volumes(zone_reader& reader, GfxDecalVolumeTransientData*& field)
		{
			if (const auto decals = reader.read_single(field, 7))
			{
				reader.read_array(decals->decalVolumes, 3, decals->decalVolumeCount);
				reader.read_array(decals->collectionBounds, 3, decals->collectionCount);
				reader.read_array(decals->decalVolumeUids, 3, decals->decalVolumeCount);
			}
		}

		void read_sun_shadow(zone_reader& reader, GfxCompressedSunShadowDataTemp*& field)
		{
			if (const auto shadow = reader.read_single(field, 7))
			{
				reader.read_array(shadow->tilesIndices, 3, shadow->tilesCount);
				reader.read_array(shadow->tilesOffsets, 3, shadow->tilesCount);

				reader.push_stream(XFILE_BLOCK_PHYSICAL);
				reader.read_array(shadow->tilesData, 0, shadow->tilesDataSize);
				reader.pop_stream();
			}
		}
	}

	void gfx_world_transient_zone::read(zone_reader& reader, GfxWorldTransientZone* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_draw_verts(reader, asset->drawVerts);
		read_draw_cells(reader, asset->drawCells);
		read_lightmaps(reader, asset->tempLightmapData);
		read_light_grid(reader, asset->gpuLightGrid);
		read_reflection_probes(reader, asset->reflectionProbes);
		reader.read_array(asset->collections, 1, asset->collectionCount);
		read_decal_volumes(reader, asset->decalVolumes);
		read_sun_shadow(reader, asset->compressedSunShadow);
		reader.pop_stream();
	}
}
