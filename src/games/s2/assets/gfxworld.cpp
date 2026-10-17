#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		unsigned int cell_word_count(const GfxWorld* world)
		{
			return (world->dpvsPlanes.cellCount + 31) >> 5;
		}

		unsigned int non_sun_light_count(const GfxWorld* world)
		{
			return world->primaryLightCount - world->lastSunPrimaryLightIndex;
		}

		unsigned int light_grid_row_count(const GfxLightGrid* grid)
		{
			return grid->maxs[grid->rowAxis] - grid->mins[grid->rowAxis] + 1;
		}

		void read_cells(zone_reader& reader, GfxWorld* asset)
		{
			const auto cell_count = asset->dpvsPlanes.cellCount;
			reader.read_array(asset->aabbTreeCounts, 3, cell_count);
			if (const auto cell_trees = reader.read_array(asset->aabbTrees, 127, cell_count))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					const auto tree_count = asset->aabbTreeCounts[i].aabbTreeCount;
					if (const auto trees = reader.read_array(cell_trees[i].aabbTree, 7, tree_count))
					{
						for (auto j = 0; j < tree_count; j++)
						{
							reader.read_array(trees[j].smodelIndexes, 1, trees[j].smodelIndexCount);
						}
					}
				}
			}
			if (const auto infos = reader.read_array(asset->cellTransientInfos, 7, cell_count))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					reader.read_array(infos[i].indices, 1, infos[i].indexCount);
				}
			}
			if (const auto cells = reader.read_array(asset->cells, 7, cell_count))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					auto* cell = &cells[i];
					if (const auto portals = reader.read_array(cell->portals, 7, cell->portalCount))
					{
						for (auto j = 0u; j < cell->portalCount; j++)
						{
							reader.read_array(portals[j].vertices, 3, portals[j].vertexCount);
						}
					}
					reader.read_array(cell->reflectionProbes, 0, cell->reflectionProbeCount);
					reader.read_array(cell->reflectionProbeReferences, 0, cell->reflectionProbeReferenceCount);
				}
			}
		}

		void write_cells(zone_buffer* buf, GfxWorld* data, GfxWorld* dest)
		{
			const auto cell_count = data->dpvsPlanes.cellCount;
			write_array(buf, 3, data->aabbTreeCounts, cell_count, dest->aabbTreeCounts);
			if (const auto dest_cell_trees = write_array(buf, 127, data->aabbTrees, cell_count, dest->aabbTrees))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					const auto tree_count = data->aabbTreeCounts[i].aabbTreeCount;
					auto* trees = data->aabbTrees[i].aabbTree;
					if (const auto dest_trees = write_array(buf, 7, trees, tree_count, dest_cell_trees[i].aabbTree))
					{
						for (auto j = 0; j < tree_count; j++)
						{
							write_array(buf, 1, trees[j].smodelIndexes, trees[j].smodelIndexCount, dest_trees[j].smodelIndexes);
						}
					}
				}
			}
			if (const auto dest_infos = write_array(buf, 7, data->cellTransientInfos, cell_count, dest->cellTransientInfos))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					auto* info = &data->cellTransientInfos[i];
					write_array(buf, 1, info->indices, info->indexCount, dest_infos[i].indices);
				}
			}
			if (const auto dest_cells = write_array(buf, 7, data->cells, cell_count, dest->cells))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					auto* cell = &data->cells[i];
					auto* dest_cell = &dest_cells[i];
					if (const auto dest_portals = write_array(buf, 7, cell->portals, cell->portalCount, dest_cell->portals))
					{
						for (auto j = 0u; j < cell->portalCount; j++)
						{
							write_array(buf, 3, cell->portals[j].vertices, cell->portals[j].vertexCount, dest_portals[j].vertices);
						}
					}
					write_array(buf, 0, cell->reflectionProbes, cell->reflectionProbeCount, dest_cell->reflectionProbes);
					write_array(buf, 0, cell->reflectionProbeReferences, cell->reflectionProbeReferenceCount, dest_cell->reflectionProbeReferences);
				}
			}
		}

		void parse_cells(assetmanager::reader& read, zone_memory* mem, GfxWorld* asset)
		{
			const auto cell_count = asset->dpvsPlanes.cellCount;
			parse_array(read, mem, asset->aabbTreeCounts, cell_count);
			if (const auto cell_trees = parse_array(read, mem, asset->aabbTrees, cell_count))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					const auto tree_count = asset->aabbTreeCounts[i].aabbTreeCount;
					if (const auto trees = parse_array(read, mem, cell_trees[i].aabbTree, tree_count))
					{
						for (auto j = 0; j < tree_count; j++)
						{
							parse_array(read, mem, trees[j].smodelIndexes, trees[j].smodelIndexCount);
						}
					}
				}
			}
			if (const auto infos = parse_array(read, mem, asset->cellTransientInfos, cell_count))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					parse_array(read, mem, infos[i].indices, infos[i].indexCount);
				}
			}
			if (const auto cells = parse_array(read, mem, asset->cells, cell_count))
			{
				for (auto i = 0u; i < cell_count; i++)
				{
					auto* cell = &cells[i];
					if (const auto portals = parse_array(read, mem, cell->portals, cell->portalCount))
					{
						for (auto j = 0u; j < cell->portalCount; j++)
						{
							parse_array(read, mem, portals[j].vertices, portals[j].vertexCount);
						}
					}
					parse_array(read, mem, cell->reflectionProbes, cell->reflectionProbeCount);
					parse_array(read, mem, cell->reflectionProbeReferences, cell->reflectionProbeReferenceCount);
				}
			}
		}

		void dump_cells(assetmanager::dumper& write, GfxWorld* asset)
		{
			const auto cell_count = asset->dpvsPlanes.cellCount;
			write.dump_array(asset->aabbTreeCounts, cell_count);
			write.dump_array(asset->aabbTrees, cell_count);
			for (auto i = 0u; asset->aabbTrees && i < cell_count; i++)
			{
				const auto tree_count = asset->aabbTreeCounts[i].aabbTreeCount;
				auto* trees = asset->aabbTrees[i].aabbTree;
				write.dump_array(trees, tree_count);
				for (auto j = 0; trees && j < tree_count; j++)
				{
					write.dump_array(trees[j].smodelIndexes, trees[j].smodelIndexCount);
				}
			}
			write.dump_array(asset->cellTransientInfos, cell_count);
			for (auto i = 0u; asset->cellTransientInfos && i < cell_count; i++)
			{
				write.dump_array(asset->cellTransientInfos[i].indices, asset->cellTransientInfos[i].indexCount);
			}
			write.dump_array(asset->cells, cell_count);
			for (auto i = 0u; asset->cells && i < cell_count; i++)
			{
				auto* cell = &asset->cells[i];
				write.dump_array(cell->portals, cell->portalCount);
				for (auto j = 0u; cell->portals && j < cell->portalCount; j++)
				{
					write.dump_array(cell->portals[j].vertices, cell->portals[j].vertexCount);
				}
				write.dump_array(cell->reflectionProbes, cell->reflectionProbeCount);
				write.dump_array(cell->reflectionProbeReferences, cell->reflectionProbeReferenceCount);
			}
		}

		void read_draw(zone_reader& reader, GfxWorldDraw* draw)
		{
			reader.read_asset_array(ASSET_TYPE_IMAGE, draw->reflectionProbes, draw->reflectionProbeCount);
			if (const auto probes = reader.read_array(draw->reflectionProbeOrigins, 7, draw->reflectionProbeCount))
			{
				for (auto i = 0u; i < draw->reflectionProbeCount; i++)
				{
					if (const auto volumes = reader.read_array(probes[i].probeVolumes, 7, probes[i].probeVolumeCount))
					{
						for (auto j = 0u; j < probes[i].probeVolumeCount; j++)
						{
							reader.read_array(volumes[j].probeVolumePlanes, 1, volumes[j].probeVolumePlaneCount);
						}
					}
				}
			}
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, draw->reflectionProbeTextures, 7, draw->reflectionProbeCount);
			reader.read_array(draw->reflectionProbeReferenceOrigins, 3, draw->reflectionProbeReferenceCount);
			reader.read_array(draw->reflectionProbeReferences, 0, draw->reflectionProbeReferenceCount);

			reader.read_asset_array(ASSET_TYPE_IMAGE, draw->fogSplineImages, draw->fogSplineCount);
			reader.read_array(draw->fogSplineInfo, 3, draw->fogSplineCount);
			reader.read_asset(ASSET_TYPE_IMAGE, draw->iesLookup);

			if (const auto overrides = reader.read_array(draw->lightmapOverrides, 7, draw->lightmapOverrideCount))
			{
				for (auto i = 0u; i < draw->lightmapOverrideCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, overrides[i].primary);
					reader.read_asset(ASSET_TYPE_IMAGE, overrides[i].secondary);
				}
			}
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, draw->lightmapPrimaryTextures, 7, draw->lightmapOverrideCount);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, draw->lightmapSecondaryTextures, 7, draw->lightmapOverrideCount);
			reader.read_asset(ASSET_TYPE_IMAGE, draw->lightmapOverridePrimary);
			reader.read_asset(ASSET_TYPE_IMAGE, draw->lightmapOverrideSecondary);
			reader.read_asset_array(ASSET_TYPE_IMAGE, draw->images, draw->imageCount);

			for (auto& transient_zone : draw->transientZones)
			{
				reader.read_asset(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, transient_zone);
			}
			if (const auto materials = reader.read_array(draw->transientZoneMaterials, 7, draw->transientZoneCount))
			{
				for (auto i = 0u; i < draw->transientZoneCount; i++)
				{
					reader.read_asset_array(ASSET_TYPE_MATERIAL, materials[i].materials, materials[i].materialCount);
				}
			}
			reader.read_array(draw->indices, 3, draw->indexCount);
		}

		void write_draw(zone_base* zone, zone_buffer* buf, GfxWorldDraw* data, GfxWorldDraw* dest)
		{
			write_asset_array(zone, buf, ASSET_TYPE_IMAGE, data->reflectionProbes, data->reflectionProbeCount, dest->reflectionProbes);
			if (const auto dest_probes = write_array(buf, 7, data->reflectionProbeOrigins, data->reflectionProbeCount, dest->reflectionProbeOrigins))
			{
				for (auto i = 0u; i < data->reflectionProbeCount; i++)
				{
					auto* probe = &data->reflectionProbeOrigins[i];
					if (const auto dest_volumes = write_array(buf, 7, probe->probeVolumes, probe->probeVolumeCount, dest_probes[i].probeVolumes))
					{
						for (auto j = 0u; j < probe->probeVolumeCount; j++)
						{
							auto* volume = &probe->probeVolumes[j];
							write_array(buf, 1, volume->probeVolumePlanes, volume->probeVolumePlaneCount, dest_volumes[j].probeVolumePlanes);
						}
					}
				}
			}
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 7, data->reflectionProbeTextures, data->reflectionProbeCount, dest->reflectionProbeTextures);
			write_array(buf, 3, data->reflectionProbeReferenceOrigins, data->reflectionProbeReferenceCount, dest->reflectionProbeReferenceOrigins);
			write_array(buf, 0, data->reflectionProbeReferences, data->reflectionProbeReferenceCount, dest->reflectionProbeReferences);

			write_asset_array(zone, buf, ASSET_TYPE_IMAGE, data->fogSplineImages, data->fogSplineCount, dest->fogSplineImages);
			write_array(buf, 3, data->fogSplineInfo, data->fogSplineCount, dest->fogSplineInfo);
			dest->iesLookup = asset_reference(zone, ASSET_TYPE_IMAGE, data->iesLookup);

			if (const auto dest_overrides = write_array(buf, 7, data->lightmapOverrides, data->lightmapOverrideCount, dest->lightmapOverrides))
			{
				for (auto i = 0u; i < data->lightmapOverrideCount; i++)
				{
					dest_overrides[i].primary = asset_reference(zone, ASSET_TYPE_IMAGE, data->lightmapOverrides[i].primary);
					dest_overrides[i].secondary = asset_reference(zone, ASSET_TYPE_IMAGE, data->lightmapOverrides[i].secondary);
				}
			}
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 7, data->lightmapPrimaryTextures, data->lightmapOverrideCount, dest->lightmapPrimaryTextures);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 7, data->lightmapSecondaryTextures, data->lightmapOverrideCount, dest->lightmapSecondaryTextures);
			dest->lightmapOverridePrimary = asset_reference(zone, ASSET_TYPE_IMAGE, data->lightmapOverridePrimary);
			dest->lightmapOverrideSecondary = asset_reference(zone, ASSET_TYPE_IMAGE, data->lightmapOverrideSecondary);
			write_asset_array(zone, buf, ASSET_TYPE_IMAGE, data->images, data->imageCount, dest->images);

			for (auto i = 0u; i < std::size(data->transientZones); i++)
			{
				dest->transientZones[i] = asset_reference(zone, ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, data->transientZones[i]);
			}
			if (const auto dest_materials = write_array(buf, 7, data->transientZoneMaterials, data->transientZoneCount, dest->transientZoneMaterials))
			{
				for (auto i = 0u; i < data->transientZoneCount; i++)
				{
					auto* materials = &data->transientZoneMaterials[i];
					write_asset_array(zone, buf, ASSET_TYPE_MATERIAL, materials->materials, materials->materialCount, dest_materials[i].materials);
				}
			}
			write_array(buf, 3, data->indices, data->indexCount, dest->indices);
		}

		void parse_draw(assetmanager::reader& read, zone_memory* mem, GfxWorldDraw* draw)
		{
			parse_asset_array(read, mem, draw->reflectionProbes, draw->reflectionProbeCount);
			if (const auto probes = parse_array(read, mem, draw->reflectionProbeOrigins, draw->reflectionProbeCount))
			{
				for (auto i = 0u; i < draw->reflectionProbeCount; i++)
				{
					if (const auto volumes = parse_array(read, mem, probes[i].probeVolumes, probes[i].probeVolumeCount))
					{
						for (auto j = 0u; j < probes[i].probeVolumeCount; j++)
						{
							parse_array(read, mem, volumes[j].probeVolumePlanes, volumes[j].probeVolumePlaneCount);
						}
					}
				}
			}
			parse_runtime_array(mem, draw->reflectionProbeTextures, draw->reflectionProbeCount);
			parse_array(read, mem, draw->reflectionProbeReferenceOrigins, draw->reflectionProbeReferenceCount);
			parse_array(read, mem, draw->reflectionProbeReferences, draw->reflectionProbeReferenceCount);

			parse_asset_array(read, mem, draw->fogSplineImages, draw->fogSplineCount);
			parse_array(read, mem, draw->fogSplineInfo, draw->fogSplineCount);
			draw->iesLookup = read.read_asset<GfxImage>();

			if (const auto overrides = parse_array(read, mem, draw->lightmapOverrides, draw->lightmapOverrideCount))
			{
				for (auto i = 0u; i < draw->lightmapOverrideCount; i++)
				{
					overrides[i].primary = read.read_asset<GfxImage>();
					overrides[i].secondary = read.read_asset<GfxImage>();
				}
			}
			parse_runtime_array(mem, draw->lightmapPrimaryTextures, draw->lightmapOverrideCount);
			parse_runtime_array(mem, draw->lightmapSecondaryTextures, draw->lightmapOverrideCount);
			draw->lightmapOverridePrimary = read.read_asset<GfxImage>();
			draw->lightmapOverrideSecondary = read.read_asset<GfxImage>();
			parse_asset_array(read, mem, draw->images, draw->imageCount);

			for (auto& transient_zone : draw->transientZones)
			{
				transient_zone = read.read_asset<GfxMapTrZone>();
			}
			if (const auto materials = parse_array(read, mem, draw->transientZoneMaterials, draw->transientZoneCount))
			{
				for (auto i = 0u; i < draw->transientZoneCount; i++)
				{
					parse_asset_array(read, mem, materials[i].materials, materials[i].materialCount);
				}
			}
			parse_array(read, mem, draw->indices, draw->indexCount);
		}

		void dump_draw(assetmanager::dumper& write, GfxWorldDraw* draw)
		{
			dump_asset_array(write, draw->reflectionProbes, draw->reflectionProbeCount);
			write.dump_array(draw->reflectionProbeOrigins, draw->reflectionProbeCount);
			for (auto i = 0u; draw->reflectionProbeOrigins && i < draw->reflectionProbeCount; i++)
			{
				auto* probe = &draw->reflectionProbeOrigins[i];
				write.dump_array(probe->probeVolumes, probe->probeVolumeCount);
				for (auto j = 0u; probe->probeVolumes && j < probe->probeVolumeCount; j++)
				{
					write.dump_array(probe->probeVolumes[j].probeVolumePlanes, probe->probeVolumes[j].probeVolumePlaneCount);
				}
			}
			write.dump_array(draw->reflectionProbeReferenceOrigins, draw->reflectionProbeReferenceCount);
			write.dump_array(draw->reflectionProbeReferences, draw->reflectionProbeReferenceCount);

			dump_asset_array(write, draw->fogSplineImages, draw->fogSplineCount);
			write.dump_array(draw->fogSplineInfo, draw->fogSplineCount);
			write.dump_asset(draw->iesLookup);

			write.dump_array(draw->lightmapOverrides, draw->lightmapOverrideCount);
			for (auto i = 0u; draw->lightmapOverrides && i < draw->lightmapOverrideCount; i++)
			{
				write.dump_asset(draw->lightmapOverrides[i].primary);
				write.dump_asset(draw->lightmapOverrides[i].secondary);
			}
			write.dump_asset(draw->lightmapOverridePrimary);
			write.dump_asset(draw->lightmapOverrideSecondary);
			dump_asset_array(write, draw->images, draw->imageCount);

			for (auto* transient_zone : draw->transientZones)
			{
				write.dump_asset(transient_zone);
			}
			write.dump_array(draw->transientZoneMaterials, draw->transientZoneCount);
			for (auto i = 0u; draw->transientZoneMaterials && i < draw->transientZoneCount; i++)
			{
				dump_asset_array(write, draw->transientZoneMaterials[i].materials, draw->transientZoneMaterials[i].materialCount);
			}
			write.dump_array(draw->indices, draw->indexCount);
		}

		void read_light_grid(zone_reader& reader, GfxLightGrid* grid)
		{
			reader.read_array(grid->rowDataStart, 1, light_grid_row_count(grid));
			reader.read_array(grid->rawRowData, 0, grid->rawRowDataSize);
			reader.read_array(grid->entries, 3, grid->entryCount);
			reader.read_array(grid->colors, 1, grid->colorCount);
			reader.read_array(grid->stageLightingContrastGain, 3, grid->stageCount);
			reader.read_array(grid->paletteEntryAddress, 3, grid->paletteEntryCount);
			reader.read_array(grid->paletteBitstream, 0, grid->paletteBitstreamSize);
			for (auto& tree : grid->tree)
			{
				reader.read_array(tree.p_nodeTable, 3, tree.nodeCount);
				reader.read_array(tree.p_leafTable, 0, tree.leafTableSize);
			}
		}

		void write_light_grid(zone_buffer* buf, GfxLightGrid* data, GfxLightGrid* dest)
		{
			write_array(buf, 1, data->rowDataStart, light_grid_row_count(data), dest->rowDataStart);
			write_array(buf, 0, data->rawRowData, data->rawRowDataSize, dest->rawRowData);
			write_array(buf, 3, data->entries, data->entryCount, dest->entries);
			write_array(buf, 1, data->colors, data->colorCount, dest->colors);
			write_array(buf, 3, data->stageLightingContrastGain, data->stageCount, dest->stageLightingContrastGain);
			write_array(buf, 3, data->paletteEntryAddress, data->paletteEntryCount, dest->paletteEntryAddress);
			write_array(buf, 0, data->paletteBitstream, data->paletteBitstreamSize, dest->paletteBitstream);
			for (auto i = 0u; i < std::size(data->tree); i++)
			{
				auto* tree = &data->tree[i];
				write_array(buf, 3, tree->p_nodeTable, tree->nodeCount, dest->tree[i].p_nodeTable);
				write_array(buf, 0, tree->p_leafTable, tree->leafTableSize, dest->tree[i].p_leafTable);
			}
		}

		void parse_light_grid(assetmanager::reader& read, zone_memory* mem, GfxLightGrid* grid)
		{
			parse_array(read, mem, grid->rowDataStart, light_grid_row_count(grid));
			parse_array(read, mem, grid->rawRowData, grid->rawRowDataSize);
			parse_array(read, mem, grid->entries, grid->entryCount);
			parse_array(read, mem, grid->colors, grid->colorCount);
			parse_array(read, mem, grid->stageLightingContrastGain, grid->stageCount);
			parse_array(read, mem, grid->paletteEntryAddress, grid->paletteEntryCount);
			parse_array(read, mem, grid->paletteBitstream, grid->paletteBitstreamSize);
			for (auto& tree : grid->tree)
			{
				parse_array(read, mem, tree.p_nodeTable, tree.nodeCount);
				parse_array(read, mem, tree.p_leafTable, tree.leafTableSize);
			}
		}

		void dump_light_grid(assetmanager::dumper& write, GfxLightGrid* grid)
		{
			write.dump_array(grid->rowDataStart, light_grid_row_count(grid));
			write.dump_array(grid->rawRowData, grid->rawRowDataSize);
			write.dump_array(grid->entries, grid->entryCount);
			write.dump_array(grid->colors, grid->colorCount);
			write.dump_array(grid->stageLightingContrastGain, grid->stageCount);
			write.dump_array(grid->paletteEntryAddress, grid->paletteEntryCount);
			write.dump_array(grid->paletteBitstream, grid->paletteBitstreamSize);
			for (auto& tree : grid->tree)
			{
				write.dump_array(tree.p_nodeTable, tree.nodeCount);
				write.dump_array(tree.p_leafTable, tree.leafTableSize);
			}
		}

		void read_probe_data(zone_reader& reader, GfxLightGridProbeData* probes)
		{
			reader.read_array(probes->probes, 63, probes->probeCount);
			reader.read_array(probes->probePositions, 3, probes->probeCount);
			reader.read_array(probes->zones, 3, probes->zoneCount);
			reader.read_array(probes->tetrahedrons, 63, probes->tetrahedronCount);
			reader.read_array(probes->tetrahedronNeighbors, 63, probes->tetrahedronCount);
			reader.read_array(probes->tetrahedronVisibility, 63, probes->tetrahedronCountVisible);
			reader.read_array(probes->voxelStartTetrahedron, 63, probes->voxelStartTetrahedronCount);
			reader.read_array(probes->smodelProbeGrids, 3, probes->smodelProbeGridCount);
			reader.read_array(probes->smodelProbes, 63, probes->smodelProbeCount);
		}

		void write_probe_data(zone_buffer* buf, GfxLightGridProbeData* data, GfxLightGridProbeData* dest)
		{
			write_array(buf, 63, data->probes, data->probeCount, dest->probes);
			write_array(buf, 3, data->probePositions, data->probeCount, dest->probePositions);
			write_array(buf, 3, data->zones, data->zoneCount, dest->zones);
			write_array(buf, 63, data->tetrahedrons, data->tetrahedronCount, dest->tetrahedrons);
			write_array(buf, 63, data->tetrahedronNeighbors, data->tetrahedronCount, dest->tetrahedronNeighbors);
			write_array(buf, 63, data->tetrahedronVisibility, data->tetrahedronCountVisible, dest->tetrahedronVisibility);
			write_array(buf, 63, data->voxelStartTetrahedron, data->voxelStartTetrahedronCount, dest->voxelStartTetrahedron);
			write_array(buf, 3, data->smodelProbeGrids, data->smodelProbeGridCount, dest->smodelProbeGrids);
			write_array(buf, 63, data->smodelProbes, data->smodelProbeCount, dest->smodelProbes);
		}

		void parse_probe_data(assetmanager::reader& read, zone_memory* mem, GfxLightGridProbeData* probes)
		{
			parse_array(read, mem, probes->probes, probes->probeCount);
			parse_array(read, mem, probes->probePositions, probes->probeCount);
			parse_array(read, mem, probes->zones, probes->zoneCount);
			parse_array(read, mem, probes->tetrahedrons, probes->tetrahedronCount);
			parse_array(read, mem, probes->tetrahedronNeighbors, probes->tetrahedronCount);
			parse_array(read, mem, probes->tetrahedronVisibility, probes->tetrahedronCountVisible);
			parse_array(read, mem, probes->voxelStartTetrahedron, probes->voxelStartTetrahedronCount);
			parse_array(read, mem, probes->smodelProbeGrids, probes->smodelProbeGridCount);
			parse_array(read, mem, probes->smodelProbes, probes->smodelProbeCount);
		}

		void dump_probe_data(assetmanager::dumper& write, GfxLightGridProbeData* probes)
		{
			write.dump_array(probes->probes, probes->probeCount);
			write.dump_array(probes->probePositions, probes->probeCount);
			write.dump_array(probes->zones, probes->zoneCount);
			write.dump_array(probes->tetrahedrons, probes->tetrahedronCount);
			write.dump_array(probes->tetrahedronNeighbors, probes->tetrahedronCount);
			write.dump_array(probes->tetrahedronVisibility, probes->tetrahedronCountVisible);
			write.dump_array(probes->voxelStartTetrahedron, probes->voxelStartTetrahedronCount);
			write.dump_array(probes->smodelProbeGrids, probes->smodelProbeGridCount);
			write.dump_array(probes->smodelProbes, probes->smodelProbeCount);
		}

		void read_unknown_data(zone_reader& reader, GfxWorldUnknownData* data)
		{
			reader.read_array(data->array0.data, 3, data->array0.count);
			reader.read_array(data->array1.data, 3, data->array1.count);
			reader.read_array(data->array2.data, 3, data->array2.count);
			reader.read_array(data->array3.data, 3, data->array3.count);
			reader.read_array(data->array4.data, 3, data->array4.count);
			reader.read_array(data->array5.data, 3, data->array5.count);
			reader.read_array(data->array6.data, 3, data->array6.count);
			reader.read_array(data->array7.data, 3, data->array7.count);
			reader.read_array(data->array8.data, 3, data->array8.count);
			reader.read_array(data->array9.data, 3, data->array9.count);

			reader.read_asset(ASSET_TYPE_IMAGE, data->image0);
			reader.read_array(data->image0Data, 3, data->image0DataCount);
			reader.read_asset(ASSET_TYPE_IMAGE, data->image1);
			reader.read_array(data->image1Data, 1, data->image1DataCount);

			for (auto& group : data->groups)
			{
				reader.read_array(group.data, 63, group.count);
				reader.read_array(group.dataA, 63, group.countA);
				reader.read_array(group.dataB, 3, group.countB);
			}
		}

		void write_unknown_data(zone_base* zone, zone_buffer* buf, GfxWorldUnknownData* data, GfxWorldUnknownData* dest)
		{
			write_array(buf, 3, data->array0.data, data->array0.count, dest->array0.data);
			write_array(buf, 3, data->array1.data, data->array1.count, dest->array1.data);
			write_array(buf, 3, data->array2.data, data->array2.count, dest->array2.data);
			write_array(buf, 3, data->array3.data, data->array3.count, dest->array3.data);
			write_array(buf, 3, data->array4.data, data->array4.count, dest->array4.data);
			write_array(buf, 3, data->array5.data, data->array5.count, dest->array5.data);
			write_array(buf, 3, data->array6.data, data->array6.count, dest->array6.data);
			write_array(buf, 3, data->array7.data, data->array7.count, dest->array7.data);
			write_array(buf, 3, data->array8.data, data->array8.count, dest->array8.data);
			write_array(buf, 3, data->array9.data, data->array9.count, dest->array9.data);

			dest->image0 = asset_reference(zone, ASSET_TYPE_IMAGE, data->image0);
			write_array(buf, 3, data->image0Data, data->image0DataCount, dest->image0Data);
			dest->image1 = asset_reference(zone, ASSET_TYPE_IMAGE, data->image1);
			write_array(buf, 1, data->image1Data, data->image1DataCount, dest->image1Data);

			for (auto i = 0u; i < std::size(data->groups); i++)
			{
				auto* group = &data->groups[i];
				auto* dest_group = &dest->groups[i];
				write_array(buf, 63, group->data, group->count, dest_group->data);
				write_array(buf, 63, group->dataA, group->countA, dest_group->dataA);
				write_array(buf, 3, group->dataB, group->countB, dest_group->dataB);
			}
		}

		void parse_unknown_data(assetmanager::reader& read, zone_memory* mem, GfxWorldUnknownData* data)
		{
			parse_array(read, mem, data->array0.data, data->array0.count);
			parse_array(read, mem, data->array1.data, data->array1.count);
			parse_array(read, mem, data->array2.data, data->array2.count);
			parse_array(read, mem, data->array3.data, data->array3.count);
			parse_array(read, mem, data->array4.data, data->array4.count);
			parse_array(read, mem, data->array5.data, data->array5.count);
			parse_array(read, mem, data->array6.data, data->array6.count);
			parse_array(read, mem, data->array7.data, data->array7.count);
			parse_array(read, mem, data->array8.data, data->array8.count);
			parse_array(read, mem, data->array9.data, data->array9.count);

			data->image0 = read.read_asset<GfxImage>();
			parse_array(read, mem, data->image0Data, data->image0DataCount);
			data->image1 = read.read_asset<GfxImage>();
			parse_array(read, mem, data->image1Data, data->image1DataCount);

			for (auto& group : data->groups)
			{
				parse_array(read, mem, group.data, group.count);
				parse_array(read, mem, group.dataA, group.countA);
				parse_array(read, mem, group.dataB, group.countB);
			}
		}

		void dump_unknown_data(assetmanager::dumper& write, GfxWorldUnknownData* data)
		{
			write.dump_array(data->array0.data, data->array0.count);
			write.dump_array(data->array1.data, data->array1.count);
			write.dump_array(data->array2.data, data->array2.count);
			write.dump_array(data->array3.data, data->array3.count);
			write.dump_array(data->array4.data, data->array4.count);
			write.dump_array(data->array5.data, data->array5.count);
			write.dump_array(data->array6.data, data->array6.count);
			write.dump_array(data->array7.data, data->array7.count);
			write.dump_array(data->array8.data, data->array8.count);
			write.dump_array(data->array9.data, data->array9.count);

			write.dump_asset(data->image0);
			write.dump_array(data->image0Data, data->image0DataCount);
			write.dump_asset(data->image1);
			write.dump_array(data->image1Data, data->image1DataCount);

			for (auto& group : data->groups)
			{
				write.dump_array(group.data, group.count);
				write.dump_array(group.dataA, group.countA);
				write.dump_array(group.dataB, group.countB);
			}
		}

		void read_voxel_trees(zone_reader& reader, GfxWorld* asset)
		{
			if (const auto trees = reader.read_array(asset->voxelTree, 7, asset->voxelTreeCount))
			{
				for (auto i = 0; i < asset->voxelTreeCount; i++)
				{
					auto* tree = &trees[i];
					reader.read_array(tree->voxelTreeHeader, 15, 1);
					reader.read_array(tree->voxelTopDownViewNodeArray, 3, tree->voxelTopDownViewNodeCount);
					reader.read_array(tree->voxelInternalNodeArray, 3, tree->voxelInternalNodeCount);
					reader.read_array(tree->voxelLeafNodeArray, 1, tree->voxelLeafNodeCount);
					reader.read_array(tree->lightListArray, 3, tree->lightListArraySize);
					read_runtime_array(reader, XFILE_BLOCK_RUNTIME, tree->voxelInternalNodeDynamicLightList, 3, tree->voxelInternalNodeCount);
				}
			}
		}

		void write_voxel_trees(zone_buffer* buf, GfxWorld* data, GfxWorld* dest)
		{
			if (const auto dest_trees = write_array(buf, 7, data->voxelTree, data->voxelTreeCount, dest->voxelTree))
			{
				for (auto i = 0; i < data->voxelTreeCount; i++)
				{
					auto* tree = &data->voxelTree[i];
					auto* dest_tree = &dest_trees[i];
					write_array(buf, 15, tree->voxelTreeHeader, 1, dest_tree->voxelTreeHeader);
					write_array(buf, 3, tree->voxelTopDownViewNodeArray, tree->voxelTopDownViewNodeCount, dest_tree->voxelTopDownViewNodeArray);
					write_array(buf, 3, tree->voxelInternalNodeArray, tree->voxelInternalNodeCount, dest_tree->voxelInternalNodeArray);
					write_array(buf, 1, tree->voxelLeafNodeArray, tree->voxelLeafNodeCount, dest_tree->voxelLeafNodeArray);
					write_array(buf, 3, tree->lightListArray, tree->lightListArraySize, dest_tree->lightListArray);
					write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, tree->voxelInternalNodeDynamicLightList, tree->voxelInternalNodeCount, dest_tree->voxelInternalNodeDynamicLightList);
				}
			}
		}

		void parse_voxel_trees(assetmanager::reader& read, zone_memory* mem, GfxWorld* asset)
		{
			if (const auto trees = parse_array(read, mem, asset->voxelTree, asset->voxelTreeCount))
			{
				for (auto i = 0; i < asset->voxelTreeCount; i++)
				{
					auto* tree = &trees[i];
					parse_array(read, mem, tree->voxelTreeHeader, 1);
					parse_array(read, mem, tree->voxelTopDownViewNodeArray, tree->voxelTopDownViewNodeCount);
					parse_array(read, mem, tree->voxelInternalNodeArray, tree->voxelInternalNodeCount);
					parse_array(read, mem, tree->voxelLeafNodeArray, tree->voxelLeafNodeCount);
					parse_array(read, mem, tree->lightListArray, tree->lightListArraySize);
					parse_runtime_array(mem, tree->voxelInternalNodeDynamicLightList, tree->voxelInternalNodeCount);
				}
			}
		}

		void dump_voxel_trees(assetmanager::dumper& write, GfxWorld* asset)
		{
			write.dump_array(asset->voxelTree, asset->voxelTreeCount);
			for (auto i = 0; asset->voxelTree && i < asset->voxelTreeCount; i++)
			{
				auto* tree = &asset->voxelTree[i];
				write.dump_array(tree->voxelTreeHeader, 1);
				write.dump_array(tree->voxelTopDownViewNodeArray, tree->voxelTopDownViewNodeCount);
				write.dump_array(tree->voxelInternalNodeArray, tree->voxelInternalNodeCount);
				write.dump_array(tree->voxelLeafNodeArray, tree->voxelLeafNodeCount);
				write.dump_array(tree->lightListArray, tree->lightListArraySize);
			}
		}

		void read_scene_runtime_arrays(zone_reader& reader, GfxWorld* asset)
		{
			const auto cell_words = cell_word_count(asset);
			const auto non_sun_lights = non_sun_light_count(asset);
			const auto dyn_model_count = asset->dpvsDyn.dynEntClientCount[0];
			const auto dyn_brush_count = asset->dpvsDyn.dynEntClientCount[1];

			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->cellCasterBits, 3, asset->dpvsPlanes.cellCount * cell_words);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->cellHasSunLitSurfsBits, 3, cell_words);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->sceneDynModel, 3, dyn_model_count);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->sceneDynBrush, 3, dyn_brush_count);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->primaryLightEntityShadowVis, 3, (non_sun_lights - 1) * 0x2000);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->primaryLightDynEntShadowVis[0], 3, dyn_model_count * (non_sun_lights - 1));
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->primaryLightDynEntShadowVis[1], 3, dyn_brush_count * (non_sun_lights - 1));
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, asset->nonSunPrimaryLightForModelDynEnt, 1, dyn_model_count);
		}

		void write_scene_runtime_arrays(zone_buffer* buf, GfxWorld* data, GfxWorld* dest)
		{
			const auto cell_words = cell_word_count(data);
			const auto non_sun_lights = non_sun_light_count(data);
			const auto dyn_model_count = data->dpvsDyn.dynEntClientCount[0];
			const auto dyn_brush_count = data->dpvsDyn.dynEntClientCount[1];

			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->cellCasterBits, data->dpvsPlanes.cellCount * cell_words, dest->cellCasterBits);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->cellHasSunLitSurfsBits, cell_words, dest->cellHasSunLitSurfsBits);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->sceneDynModel, dyn_model_count, dest->sceneDynModel);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->sceneDynBrush, dyn_brush_count, dest->sceneDynBrush);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->primaryLightEntityShadowVis, (non_sun_lights - 1) * 0x2000, dest->primaryLightEntityShadowVis);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->primaryLightDynEntShadowVis[0], dyn_model_count * (non_sun_lights - 1), dest->primaryLightDynEntShadowVis[0]);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->primaryLightDynEntShadowVis[1], dyn_brush_count * (non_sun_lights - 1), dest->primaryLightDynEntShadowVis[1]);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 1, data->nonSunPrimaryLightForModelDynEnt, dyn_model_count, dest->nonSunPrimaryLightForModelDynEnt);
		}

		void parse_scene_runtime_arrays(zone_memory* mem, GfxWorld* asset)
		{
			const auto cell_words = cell_word_count(asset);
			const auto non_sun_lights = non_sun_light_count(asset);
			const auto dyn_model_count = asset->dpvsDyn.dynEntClientCount[0];
			const auto dyn_brush_count = asset->dpvsDyn.dynEntClientCount[1];

			parse_runtime_array(mem, asset->cellCasterBits, asset->dpvsPlanes.cellCount * cell_words);
			parse_runtime_array(mem, asset->cellHasSunLitSurfsBits, cell_words);
			parse_runtime_array(mem, asset->sceneDynModel, dyn_model_count);
			parse_runtime_array(mem, asset->sceneDynBrush, dyn_brush_count);
			parse_runtime_array(mem, asset->primaryLightEntityShadowVis, (non_sun_lights - 1) * 0x2000);
			parse_runtime_array(mem, asset->primaryLightDynEntShadowVis[0], dyn_model_count * (non_sun_lights - 1));
			parse_runtime_array(mem, asset->primaryLightDynEntShadowVis[1], dyn_brush_count * (non_sun_lights - 1));
			parse_runtime_array(mem, asset->nonSunPrimaryLightForModelDynEnt, dyn_model_count);
		}

		void read_shadow_geometry(zone_reader& reader, GfxShadowGeometry*& field, const unsigned int count)
		{
			if (const auto geometry = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_array(geometry[i].sortedSurfIndex, 3, geometry[i].surfaceCount);
					reader.read_array(geometry[i].smodelIndex, 1, geometry[i].smodelCount);
				}
			}
		}

		void write_shadow_geometry(zone_buffer* buf, GfxShadowGeometry* data, const unsigned int count, GfxShadowGeometry*& dest_field)
		{
			if (const auto dest = write_array(buf, 7, data, count, dest_field))
			{
				for (auto i = 0u; i < count; i++)
				{
					write_array(buf, 3, data[i].sortedSurfIndex, data[i].surfaceCount, dest[i].sortedSurfIndex);
					write_array(buf, 1, data[i].smodelIndex, data[i].smodelCount, dest[i].smodelIndex);
				}
			}
		}

		void parse_shadow_geometry(assetmanager::reader& read, zone_memory* mem, GfxShadowGeometry*& field, const unsigned int count)
		{
			if (const auto geometry = parse_array(read, mem, field, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					parse_array(read, mem, geometry[i].sortedSurfIndex, geometry[i].surfaceCount);
					parse_array(read, mem, geometry[i].smodelIndex, geometry[i].smodelCount);
				}
			}
		}

		void dump_shadow_geometry(assetmanager::dumper& write, GfxShadowGeometry* geometry, const unsigned int count)
		{
			write.dump_array(geometry, count);
			for (auto i = 0u; geometry && i < count; i++)
			{
				write.dump_array(geometry[i].sortedSurfIndex, geometry[i].surfaceCount);
				write.dump_array(geometry[i].smodelIndex, geometry[i].smodelCount);
			}
		}

		void read_light_regions(zone_reader& reader, GfxWorld* asset)
		{
			if (const auto regions = reader.read_array(asset->lightRegion, 7, asset->primaryLightCount))
			{
				for (auto i = 0u; i < asset->primaryLightCount; i++)
				{
					if (const auto hulls = reader.read_array(regions[i].hulls, 7, regions[i].hullCount))
					{
						for (auto j = 0u; j < regions[i].hullCount; j++)
						{
							reader.read_array(hulls[j].axis, 3, hulls[j].axisCount);
						}
					}
				}
			}
		}

		void write_light_regions(zone_buffer* buf, GfxWorld* data, GfxWorld* dest)
		{
			if (const auto dest_regions = write_array(buf, 7, data->lightRegion, data->primaryLightCount, dest->lightRegion))
			{
				for (auto i = 0u; i < data->primaryLightCount; i++)
				{
					auto* region = &data->lightRegion[i];
					if (const auto dest_hulls = write_array(buf, 7, region->hulls, region->hullCount, dest_regions[i].hulls))
					{
						for (auto j = 0u; j < region->hullCount; j++)
						{
							write_array(buf, 3, region->hulls[j].axis, region->hulls[j].axisCount, dest_hulls[j].axis);
						}
					}
				}
			}
		}

		void parse_light_regions(assetmanager::reader& read, zone_memory* mem, GfxWorld* asset)
		{
			if (const auto regions = parse_array(read, mem, asset->lightRegion, asset->primaryLightCount))
			{
				for (auto i = 0u; i < asset->primaryLightCount; i++)
				{
					if (const auto hulls = parse_array(read, mem, regions[i].hulls, regions[i].hullCount))
					{
						for (auto j = 0u; j < regions[i].hullCount; j++)
						{
							parse_array(read, mem, hulls[j].axis, hulls[j].axisCount);
						}
					}
				}
			}
		}

		void dump_light_regions(assetmanager::dumper& write, GfxWorld* asset)
		{
			write.dump_array(asset->lightRegion, asset->primaryLightCount);
			for (auto i = 0u; asset->lightRegion && i < asset->primaryLightCount; i++)
			{
				auto* region = &asset->lightRegion[i];
				write.dump_array(region->hulls, region->hullCount);
				for (auto j = 0u; region->hulls && j < region->hullCount; j++)
				{
					write.dump_array(region->hulls[j].axis, region->hulls[j].axisCount);
				}
			}
		}

		void read_vis_arrays(zone_reader& reader, unsigned int** arrays, const std::size_t array_count, const std::size_t count)
		{
			for (auto i = 0u; i < array_count; i++)
			{
				read_runtime_array(reader, XFILE_BLOCK_RUNTIME, arrays[i], 127, count);
			}
		}

		void write_vis_arrays(zone_buffer* buf, unsigned int** arrays, const std::size_t array_count, const std::size_t count, unsigned int** dest_arrays)
		{
			for (auto i = 0u; i < array_count; i++)
			{
				write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 127, arrays[i], count, dest_arrays[i]);
			}
		}

		void parse_vis_arrays(zone_memory* mem, unsigned int** arrays, const std::size_t array_count, const std::size_t count)
		{
			for (auto i = 0u; i < array_count; i++)
			{
				parse_runtime_array(mem, arrays[i], count);
			}
		}

		bool has_vertex_lighting(const GfxStaticModelDrawInst& draw_inst)
		{
			return (draw_inst.flags & STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) != 0;
		}

		void read_dpvs_static(zone_reader& reader, GfxWorld* asset)
		{
			auto* dpvs = &asset->dpvs;
			read_vis_arrays(reader, dpvs->smodelVisData, 3, dpvs->smodelVisDataCount);
			read_vis_arrays(reader, dpvs->surfaceVisData, 3, dpvs->surfaceVisDataCount);
			read_vis_arrays(reader, dpvs->smodelVisDataUnused, std::size(dpvs->smodelVisDataUnused), 0);
			read_vis_arrays(reader, dpvs->surfaceVisDataUnused, std::size(dpvs->surfaceVisDataUnused), 0);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->unk_0x2C8, 127, 2 * dpvs->surfaceVisDataCount);
			reader.read_array(dpvs->unk_0x318, 127, 2 * dpvs->smodelVisDataCount);

			read_vis_arrays(reader, &dpvs->smodelVisData[3], 1, dpvs->smodelVisDataCount);
			read_vis_arrays(reader, &dpvs->surfaceVisData[3], 1, dpvs->surfaceVisDataCount);
			for (auto view = 0u; view < 3; view++)
			{
				read_vis_arrays(reader, &dpvs->smodelUnknownVisData[view * 9], 9, dpvs->smodelVisDataCount);
				read_vis_arrays(reader, &dpvs->surfaceUnknownVisData[view * 9], 9, dpvs->surfaceVisDataCount);
			}
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->primaryLightVisData, 127, dpvs->primaryLightVisDataCount);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->unk_0x2D8, 127, dpvs->smodelVisDataCount);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->unk_0x2E0, 127, dpvs->surfaceVisDataCount);

			reader.read_array(dpvs->sortedSurfIndex, 3, dpvs->staticSurfaceCount);
			if (const auto surfaces = reader.read_array(dpvs->surfaces, 7, asset->surfaceCount))
			{
				for (auto i = 0u; i < asset->surfaceCount; i++)
				{
					if (const auto buffer = reader.read_array(surfaces[i].buffer, 7, 1))
					{
						read_runtime_array(reader, XFILE_BLOCK_RUNTIME_UNREAD, buffer->data, 255, buffer->dataSize);
					}
					reader.read_asset(ASSET_TYPE_MATERIAL, surfaces[i].material);
				}
			}
			reader.read_array(dpvs->surfacesBounds, 3, asset->surfaceCount);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->unk_0x300, 1, asset->surfaceCount);

			reader.read_array(dpvs->smodelInsts, 3, dpvs->smodelCount);
			if (const auto draw_insts = reader.read_array(dpvs->smodelDrawInsts, 7, dpvs->smodelCount))
			{
				for (auto i = 0u; i < dpvs->smodelCount; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, draw_insts[i].model);
				}
			}
			if (const auto lighting_insts = reader.read_array(dpvs->smodelLightingInsts, 7, dpvs->smodelCount))
			{
				for (auto i = 0u; i < dpvs->smodelCount; i++)
				{
					if (has_vertex_lighting(dpvs->smodelDrawInsts[i]))
					{
						auto* lighting = &lighting_insts[i].vertexLightingInfo;
						reader.read_array(lighting->lightingValues, 1, lighting->numLightingValues);
					}
				}
			}
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->unk_0x328, 127, dpvs->surfaceVisDataCount);
			reader.read_array(dpvs->sortedSmodelIndices, 1, dpvs->smodelCount);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->surfaceCastsSunShadowOpt, 127, dpvs->sunShadowOptCount * dpvs->sunSurfVisDataCount);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->surfaceMaterials, 3, dpvs->staticSurfaceCount);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dpvs->unk_0x350, 7, dpvs->smodelCount);
		}

		void write_dpvs_static(zone_base* zone, zone_buffer* buf, GfxWorld* data_world, GfxWorld* dest_world)
		{
			auto* data = &data_world->dpvs;
			auto* dest = &dest_world->dpvs;
			write_vis_arrays(buf, data->smodelVisData, 3, data->smodelVisDataCount, dest->smodelVisData);
			write_vis_arrays(buf, data->surfaceVisData, 3, data->surfaceVisDataCount, dest->surfaceVisData);
			write_vis_arrays(buf, data->smodelVisDataUnused, std::size(data->smodelVisDataUnused), 0, dest->smodelVisDataUnused);
			write_vis_arrays(buf, data->surfaceVisDataUnused, std::size(data->surfaceVisDataUnused), 0, dest->surfaceVisDataUnused);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 127, data->unk_0x2C8, 2 * data->surfaceVisDataCount, dest->unk_0x2C8);
			write_array(buf, 127, data->unk_0x318, 2 * data->smodelVisDataCount, dest->unk_0x318);

			write_vis_arrays(buf, &data->smodelVisData[3], 1, data->smodelVisDataCount, &dest->smodelVisData[3]);
			write_vis_arrays(buf, &data->surfaceVisData[3], 1, data->surfaceVisDataCount, &dest->surfaceVisData[3]);
			for (auto view = 0u; view < 3; view++)
			{
				write_vis_arrays(buf, &data->smodelUnknownVisData[view * 9], 9, data->smodelVisDataCount, &dest->smodelUnknownVisData[view * 9]);
				write_vis_arrays(buf, &data->surfaceUnknownVisData[view * 9], 9, data->surfaceVisDataCount, &dest->surfaceUnknownVisData[view * 9]);
			}
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 127, data->primaryLightVisData, data->primaryLightVisDataCount, dest->primaryLightVisData);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 127, data->unk_0x2D8, data->smodelVisDataCount, dest->unk_0x2D8);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 127, data->unk_0x2E0, data->surfaceVisDataCount, dest->unk_0x2E0);

			write_array(buf, 3, data->sortedSurfIndex, data->staticSurfaceCount, dest->sortedSurfIndex);
			if (const auto dest_surfaces = write_array(buf, 7, data->surfaces, data_world->surfaceCount, dest->surfaces))
			{
				for (auto i = 0u; i < data_world->surfaceCount; i++)
				{
					auto* surface = &data->surfaces[i];
					if (const auto dest_buffer = write_array(buf, 7, surface->buffer, 1, dest_surfaces[i].buffer))
					{
						write_runtime_array(buf, XFILE_BLOCK_RUNTIME_UNREAD, 255, surface->buffer->data, surface->buffer->dataSize, dest_buffer->data);
					}
					dest_surfaces[i].material = asset_reference(zone, ASSET_TYPE_MATERIAL, surface->material);
				}
			}
			write_array(buf, 3, data->surfacesBounds, data_world->surfaceCount, dest->surfacesBounds);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 1, data->unk_0x300, data_world->surfaceCount, dest->unk_0x300);

			write_array(buf, 3, data->smodelInsts, data->smodelCount, dest->smodelInsts);
			if (const auto dest_draw_insts = write_array(buf, 7, data->smodelDrawInsts, data->smodelCount, dest->smodelDrawInsts))
			{
				for (auto i = 0u; i < data->smodelCount; i++)
				{
					dest_draw_insts[i].model = asset_reference(zone, ASSET_TYPE_XMODEL, data->smodelDrawInsts[i].model);
				}
			}
			if (const auto dest_lighting_insts = write_array(buf, 7, data->smodelLightingInsts, data->smodelCount, dest->smodelLightingInsts))
			{
				for (auto i = 0u; i < data->smodelCount; i++)
				{
					if (has_vertex_lighting(data->smodelDrawInsts[i]))
					{
						auto* lighting = &data->smodelLightingInsts[i].vertexLightingInfo;
						write_array(buf, 1, lighting->lightingValues, lighting->numLightingValues, dest_lighting_insts[i].vertexLightingInfo.lightingValues);
					}
				}
			}
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 127, data->unk_0x328, data->surfaceVisDataCount, dest->unk_0x328);
			write_array(buf, 1, data->sortedSmodelIndices, data->smodelCount, dest->sortedSmodelIndices);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 127, data->surfaceCastsSunShadowOpt, data->sunShadowOptCount * data->sunSurfVisDataCount, dest->surfaceCastsSunShadowOpt);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->surfaceMaterials, data->staticSurfaceCount, dest->surfaceMaterials);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 7, data->unk_0x350, data->smodelCount, dest->unk_0x350);
		}

		void parse_dpvs_static(assetmanager::reader& read, zone_memory* mem, GfxWorld* asset)
		{
			auto* dpvs = &asset->dpvs;
			parse_vis_arrays(mem, dpvs->smodelVisData, std::size(dpvs->smodelVisData), dpvs->smodelVisDataCount);
			parse_vis_arrays(mem, dpvs->surfaceVisData, std::size(dpvs->surfaceVisData), dpvs->surfaceVisDataCount);
			parse_vis_arrays(mem, dpvs->smodelVisDataUnused, std::size(dpvs->smodelVisDataUnused), 0);
			parse_vis_arrays(mem, dpvs->surfaceVisDataUnused, std::size(dpvs->surfaceVisDataUnused), 0);
			parse_vis_arrays(mem, dpvs->smodelUnknownVisData, std::size(dpvs->smodelUnknownVisData), dpvs->smodelVisDataCount);
			parse_vis_arrays(mem, dpvs->surfaceUnknownVisData, std::size(dpvs->surfaceUnknownVisData), dpvs->surfaceVisDataCount);
			parse_runtime_array(mem, dpvs->unk_0x2C8, 2 * dpvs->surfaceVisDataCount);
			parse_runtime_array(mem, dpvs->primaryLightVisData, dpvs->primaryLightVisDataCount);
			parse_runtime_array(mem, dpvs->unk_0x2D8, dpvs->smodelVisDataCount);
			parse_runtime_array(mem, dpvs->unk_0x2E0, dpvs->surfaceVisDataCount);
			parse_array(read, mem, dpvs->unk_0x318, 2 * dpvs->smodelVisDataCount);

			parse_array(read, mem, dpvs->sortedSurfIndex, dpvs->staticSurfaceCount);
			if (const auto surfaces = parse_array(read, mem, dpvs->surfaces, asset->surfaceCount))
			{
				for (auto i = 0u; i < asset->surfaceCount; i++)
				{
					if (const auto buffer = parse_array(read, mem, surfaces[i].buffer, 1))
					{
						parse_runtime_array(mem, buffer->data, buffer->dataSize);
					}
					surfaces[i].material = read.read_asset<Material>();
				}
			}
			parse_array(read, mem, dpvs->surfacesBounds, asset->surfaceCount);
			parse_runtime_array(mem, dpvs->unk_0x300, asset->surfaceCount);

			parse_array(read, mem, dpvs->smodelInsts, dpvs->smodelCount);
			if (const auto draw_insts = parse_array(read, mem, dpvs->smodelDrawInsts, dpvs->smodelCount))
			{
				for (auto i = 0u; i < dpvs->smodelCount; i++)
				{
					draw_insts[i].model = read.read_asset<XModel>();
				}
			}
			if (const auto lighting_insts = parse_array(read, mem, dpvs->smodelLightingInsts, dpvs->smodelCount))
			{
				for (auto i = 0u; i < dpvs->smodelCount; i++)
				{
					if (has_vertex_lighting(dpvs->smodelDrawInsts[i]))
					{
						auto* lighting = &lighting_insts[i].vertexLightingInfo;
						parse_array(read, mem, lighting->lightingValues, lighting->numLightingValues);
					}
				}
			}
			parse_runtime_array(mem, dpvs->unk_0x328, dpvs->surfaceVisDataCount);
			parse_array(read, mem, dpvs->sortedSmodelIndices, dpvs->smodelCount);
			parse_runtime_array(mem, dpvs->surfaceCastsSunShadowOpt, dpvs->sunShadowOptCount * dpvs->sunSurfVisDataCount);
			parse_runtime_array(mem, dpvs->surfaceMaterials, dpvs->staticSurfaceCount);
			parse_runtime_array(mem, dpvs->unk_0x350, dpvs->smodelCount);
		}

		void dump_dpvs_static(assetmanager::dumper& write, GfxWorld* asset)
		{
			auto* dpvs = &asset->dpvs;
			write.dump_array(dpvs->unk_0x318, 2 * dpvs->smodelVisDataCount);

			write.dump_array(dpvs->sortedSurfIndex, dpvs->staticSurfaceCount);
			write.dump_array(dpvs->surfaces, asset->surfaceCount);
			for (auto i = 0u; dpvs->surfaces && i < asset->surfaceCount; i++)
			{
				write.dump_array(dpvs->surfaces[i].buffer, 1);
				write.dump_asset(dpvs->surfaces[i].material);
			}
			write.dump_array(dpvs->surfacesBounds, asset->surfaceCount);

			write.dump_array(dpvs->smodelInsts, dpvs->smodelCount);
			write.dump_array(dpvs->smodelDrawInsts, dpvs->smodelCount);
			for (auto i = 0u; dpvs->smodelDrawInsts && i < dpvs->smodelCount; i++)
			{
				write.dump_asset(dpvs->smodelDrawInsts[i].model);
			}
			write.dump_array(dpvs->smodelLightingInsts, dpvs->smodelCount);
			for (auto i = 0u; dpvs->smodelLightingInsts && i < dpvs->smodelCount; i++)
			{
				if (has_vertex_lighting(dpvs->smodelDrawInsts[i]))
				{
					auto* lighting = &dpvs->smodelLightingInsts[i].vertexLightingInfo;
					write.dump_array(lighting->lightingValues, lighting->numLightingValues);
				}
			}
			write.dump_array(dpvs->sortedSmodelIndices, dpvs->smodelCount);
		}

		void read_dpvs_dynamic(zone_reader& reader, GfxWorld* asset)
		{
			auto* dyn = &asset->dpvsDyn;
			for (auto set = 0u; set < std::size(dyn->dynEntCellBits); set++)
			{
				read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dyn->dynEntCellBits[set], 3, dyn->dynEntClientWordCount[set] * asset->dpvsPlanes.cellCount);
			}
			for (auto set = 0u; set < std::size(dyn->dynEntVisData); set++)
			{
				for (auto i = 0u; i < 3; i++)
				{
					read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dyn->dynEntVisData[set][i], 15, 32 * dyn->dynEntClientWordCount[set]);
				}
			}
			for (auto set = 0u; set < std::size(dyn->dynEntVisData); set++)
			{
				read_runtime_array(reader, XFILE_BLOCK_RUNTIME, dyn->dynEntVisData[set][3], 15, 32 * dyn->dynEntClientWordCount[set]);
			}
		}

		void write_dpvs_dynamic(zone_buffer* buf, GfxWorld* data_world, GfxWorld* dest_world)
		{
			auto* data = &data_world->dpvsDyn;
			auto* dest = &dest_world->dpvsDyn;
			for (auto set = 0u; set < std::size(data->dynEntCellBits); set++)
			{
				write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, data->dynEntCellBits[set], data->dynEntClientWordCount[set] * data_world->dpvsPlanes.cellCount, dest->dynEntCellBits[set]);
			}
			for (auto set = 0u; set < std::size(data->dynEntVisData); set++)
			{
				for (auto i = 0u; i < 3; i++)
				{
					write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 15, data->dynEntVisData[set][i], 32 * data->dynEntClientWordCount[set], dest->dynEntVisData[set][i]);
				}
			}
			for (auto set = 0u; set < std::size(data->dynEntVisData); set++)
			{
				write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 15, data->dynEntVisData[set][3], 32 * data->dynEntClientWordCount[set], dest->dynEntVisData[set][3]);
			}
		}

		void parse_dpvs_dynamic(zone_memory* mem, GfxWorld* asset)
		{
			auto* dyn = &asset->dpvsDyn;
			for (auto set = 0u; set < std::size(dyn->dynEntCellBits); set++)
			{
				parse_runtime_array(mem, dyn->dynEntCellBits[set], dyn->dynEntClientWordCount[set] * asset->dpvsPlanes.cellCount);
				for (auto& vis_data : dyn->dynEntVisData[set])
				{
					parse_runtime_array(mem, vis_data, 32 * dyn->dynEntClientWordCount[set]);
				}
			}
		}
	}

	void gfx_world::read(zone_reader& reader, GfxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->baseName);

		if (const auto skies = reader.read_array(asset->skies, 7, asset->skyCount))
		{
			for (auto i = 0; i < asset->skyCount; i++)
			{
				reader.read_array(skies[i].skyStartSurfs, 3, skies[i].skySurfCount);
				reader.read_asset(ASSET_TYPE_IMAGE, skies[i].skyImage);
			}
		}

		auto* planes = &asset->dpvsPlanes;
		reader.read_array(planes->planes, 3, asset->planeCount);
		reader.read_array(planes->nodes, 3, asset->nodeCount);
		read_runtime_array(reader, XFILE_BLOCK_RUNTIME, planes->sceneEntCellBits, 3, planes->cellCount << 9);
		read_runtime_array(reader, XFILE_BLOCK_RUNTIME, planes->unk_0x20, 3, 8 * planes->cellCount);

		read_cells(reader, asset);

		if (const auto groups = reader.read_array(asset->portalGroup, 7, asset->portalGroupCount))
		{
			for (auto i = 0u; i < asset->portalGroupCount; i++)
			{
				reader.read_string(groups[i].targetName);
				reader.read_array(groups[i].gfxPortalArray, 1, groups[i].numPortals);
			}
		}
		reader.read_array(asset->portalDistanceAnchorsAndCloseDistSquared, 3, asset->portalDistanceAnchorCount);

		read_draw(reader, &asset->draw);
		read_light_grid(reader, &asset->lightGrid);
		read_probe_data(reader, &asset->probeData);
		read_unknown_data(reader, &asset->unknownData);
		read_voxel_trees(reader, asset);

		reader.read_array(asset->lightLists.surfaceListOffsets, 1, 0);
		reader.read_array(asset->lightLists.smodelListOffsets, 1, 0);
		reader.read_array(asset->lightLists.lists, 1, asset->lightLists.listsSize);

		if (const auto frustum_lights = reader.read_array(asset->frustumLights, 15, asset->primaryLightCount))
		{
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				reader.read_array(frustum_lights[i].indices, 3, frustum_lights[i].indexCount);
				reader.read_array(frustum_lights[i].vertices, 3, frustum_lights[i].vertexCount);
			}
		}

		reader.read_array(asset->models, 3, asset->modelCount);
		if (const auto memory = reader.read_array(asset->materialMemory, 15, asset->materialMemoryCount))
		{
			for (auto i = 0; i < asset->materialMemoryCount; i++)
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, memory[i].material);
			}
		}
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->sun.spriteMaterial);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->sun.flareMaterial);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->outdoorImage);

		read_scene_runtime_arrays(reader, asset);
		read_shadow_geometry(reader, asset->shadowGeom, asset->primaryLightCount);
		read_shadow_geometry(reader, asset->shadowGeomOptimized, asset->primaryLightCount);
		read_light_regions(reader, asset);
		read_dpvs_static(reader, asset);
		read_dpvs_dynamic(reader, asset);

		reader.read_array(asset->heroOnlyLights, 3, asset->heroOnlyLightCount);
		if (const auto volumes = reader.read_array(asset->mdaoVolumes, 7, asset->mdaoVolumesCount))
		{
			for (auto i = 0u; i < asset->mdaoVolumesCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, volumes[i].volumeData);
			}
		}

		reader.read_string(asset->buildInfo.bspCommandline);
		reader.read_string(asset->buildInfo.lightCommandline);
		reader.read_string(asset->buildInfo.bspTimestamp);
		reader.read_string(asset->buildInfo.lightTimestamp);
		reader.pop_stream();
	}

	GfxWorld* gfx_world::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_GFXWORLD, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gfxworld \"%s\"...", name.data());

		auto* asset = read.read_single<GfxWorld>();
		asset->name = read.read_string();
		asset->baseName = read.read_string();

		if (const auto skies = parse_array(read, mem, asset->skies, asset->skyCount))
		{
			for (auto i = 0; i < asset->skyCount; i++)
			{
				parse_array(read, mem, skies[i].skyStartSurfs, skies[i].skySurfCount);
				skies[i].skyImage = read.read_asset<GfxImage>();
			}
		}

		auto* planes = &asset->dpvsPlanes;
		parse_array(read, mem, planes->planes, asset->planeCount);
		parse_array(read, mem, planes->nodes, asset->nodeCount);
		parse_runtime_array(mem, planes->sceneEntCellBits, planes->cellCount << 9);
		parse_runtime_array(mem, planes->unk_0x20, 8 * planes->cellCount);

		parse_cells(read, mem, asset);

		if (const auto groups = parse_array(read, mem, asset->portalGroup, asset->portalGroupCount))
		{
			for (auto i = 0u; i < asset->portalGroupCount; i++)
			{
				groups[i].targetName = read.read_string();
				parse_array(read, mem, groups[i].gfxPortalArray, groups[i].numPortals);
			}
		}
		parse_array(read, mem, asset->portalDistanceAnchorsAndCloseDistSquared, asset->portalDistanceAnchorCount);

		parse_draw(read, mem, &asset->draw);
		parse_light_grid(read, mem, &asset->lightGrid);
		parse_probe_data(read, mem, &asset->probeData);
		parse_unknown_data(read, mem, &asset->unknownData);
		parse_voxel_trees(read, mem, asset);

		parse_array(read, mem, asset->lightLists.surfaceListOffsets, 0);
		parse_array(read, mem, asset->lightLists.smodelListOffsets, 0);
		parse_array(read, mem, asset->lightLists.lists, asset->lightLists.listsSize);

		if (const auto frustum_lights = parse_array(read, mem, asset->frustumLights, asset->primaryLightCount))
		{
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				parse_array(read, mem, frustum_lights[i].indices, frustum_lights[i].indexCount);
				parse_array(read, mem, frustum_lights[i].vertices, frustum_lights[i].vertexCount);
			}
		}

		parse_array(read, mem, asset->models, asset->modelCount);
		if (const auto memory = parse_array(read, mem, asset->materialMemory, asset->materialMemoryCount))
		{
			for (auto i = 0; i < asset->materialMemoryCount; i++)
			{
				memory[i].material = read.read_asset<Material>();
			}
		}
		asset->sun.spriteMaterial = read.read_asset<Material>();
		asset->sun.flareMaterial = read.read_asset<Material>();
		asset->outdoorImage = read.read_asset<GfxImage>();

		parse_scene_runtime_arrays(mem, asset);
		parse_shadow_geometry(read, mem, asset->shadowGeom, asset->primaryLightCount);
		parse_shadow_geometry(read, mem, asset->shadowGeomOptimized, asset->primaryLightCount);
		parse_light_regions(read, mem, asset);
		parse_dpvs_static(read, mem, asset);
		parse_dpvs_dynamic(mem, asset);

		parse_array(read, mem, asset->heroOnlyLights, asset->heroOnlyLightCount);
		if (const auto volumes = parse_array(read, mem, asset->mdaoVolumes, asset->mdaoVolumesCount))
		{
			for (auto i = 0u; i < asset->mdaoVolumesCount; i++)
			{
				volumes[i].volumeData = read.read_asset<GfxImage>();
			}
		}

		asset->buildInfo.bspCommandline = read.read_string();
		asset->buildInfo.lightCommandline = read.read_string();
		asset->buildInfo.bspTimestamp = read.read_string();
		asset->buildInfo.lightTimestamp = read.read_string();
		read.close();

		return asset;
	}

	void gfx_world::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		auto* draw = &data->draw;

		for (auto i = 0; data->skies && i < data->skyCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_IMAGE, data->skies[i].skyImage);
		}

		add_dependencies(zone, ASSET_TYPE_IMAGE, draw->reflectionProbes, draw->reflectionProbeCount);
		add_dependencies(zone, ASSET_TYPE_IMAGE, draw->fogSplineImages, draw->fogSplineCount);
		add_dependency(zone, ASSET_TYPE_IMAGE, draw->iesLookup);
		for (auto i = 0u; draw->lightmapOverrides && i < draw->lightmapOverrideCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_IMAGE, draw->lightmapOverrides[i].primary);
			add_dependency(zone, ASSET_TYPE_IMAGE, draw->lightmapOverrides[i].secondary);
		}
		add_dependency(zone, ASSET_TYPE_IMAGE, draw->lightmapOverridePrimary);
		add_dependency(zone, ASSET_TYPE_IMAGE, draw->lightmapOverrideSecondary);
		add_dependencies(zone, ASSET_TYPE_IMAGE, draw->images, draw->imageCount);
		add_dependencies(zone, ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, draw->transientZones, std::size(draw->transientZones));
		for (auto i = 0u; draw->transientZoneMaterials && i < draw->transientZoneCount; i++)
		{
			add_dependencies(zone, ASSET_TYPE_MATERIAL, draw->transientZoneMaterials[i].materials, draw->transientZoneMaterials[i].materialCount);
		}

		add_dependency(zone, ASSET_TYPE_IMAGE, data->unknownData.image0);
		add_dependency(zone, ASSET_TYPE_IMAGE, data->unknownData.image1);

		for (auto i = 0; data->materialMemory && i < data->materialMemoryCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_MATERIAL, data->materialMemory[i].material);
		}
		add_dependency(zone, ASSET_TYPE_MATERIAL, data->sun.spriteMaterial);
		add_dependency(zone, ASSET_TYPE_MATERIAL, data->sun.flareMaterial);
		add_dependency(zone, ASSET_TYPE_IMAGE, data->outdoorImage);

		for (auto i = 0u; data->dpvs.surfaces && i < data->surfaceCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_MATERIAL, data->dpvs.surfaces[i].material);
		}
		for (auto i = 0u; data->dpvs.smodelDrawInsts && i < data->dpvs.smodelCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_XMODEL, data->dpvs.smodelDrawInsts[i].model);
		}

		for (auto i = 0u; data->mdaoVolumes && i < data->mdaoVolumesCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData);
		}
	}

	void gfx_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = this->write_name(buf);
		write_string(buf, data->baseName, dest->baseName);

		if (const auto dest_skies = write_array(buf, 7, data->skies, data->skyCount, dest->skies))
		{
			for (auto i = 0; i < data->skyCount; i++)
			{
				auto* sky = &data->skies[i];
				write_array(buf, 3, sky->skyStartSurfs, sky->skySurfCount, dest_skies[i].skyStartSurfs);
				dest_skies[i].skyImage = asset_reference(zone, ASSET_TYPE_IMAGE, sky->skyImage);
			}
		}

		auto* planes = &data->dpvsPlanes;
		auto* dest_planes = &dest->dpvsPlanes;
		write_array(buf, 3, planes->planes, data->planeCount, dest_planes->planes);
		write_array(buf, 3, planes->nodes, data->nodeCount, dest_planes->nodes);
		write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, planes->sceneEntCellBits, planes->cellCount << 9, dest_planes->sceneEntCellBits);
		write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, planes->unk_0x20, 8 * planes->cellCount, dest_planes->unk_0x20);

		write_cells(buf, data, dest);

		if (const auto dest_groups = write_array(buf, 7, data->portalGroup, data->portalGroupCount, dest->portalGroup))
		{
			for (auto i = 0u; i < data->portalGroupCount; i++)
			{
				auto* group = &data->portalGroup[i];
				write_string(buf, group->targetName, dest_groups[i].targetName);
				write_array(buf, 1, group->gfxPortalArray, group->numPortals, dest_groups[i].gfxPortalArray);
			}
		}
		write_array(buf, 3, data->portalDistanceAnchorsAndCloseDistSquared, data->portalDistanceAnchorCount, dest->portalDistanceAnchorsAndCloseDistSquared);

		write_draw(zone, buf, &data->draw, &dest->draw);
		write_light_grid(buf, &data->lightGrid, &dest->lightGrid);
		write_probe_data(buf, &data->probeData, &dest->probeData);
		write_unknown_data(zone, buf, &data->unknownData, &dest->unknownData);
		write_voxel_trees(buf, data, dest);

		write_array(buf, 1, data->lightLists.surfaceListOffsets, 0, dest->lightLists.surfaceListOffsets);
		write_array(buf, 1, data->lightLists.smodelListOffsets, 0, dest->lightLists.smodelListOffsets);
		write_array(buf, 1, data->lightLists.lists, data->lightLists.listsSize, dest->lightLists.lists);

		if (const auto dest_frustum_lights = write_array(buf, 15, data->frustumLights, data->primaryLightCount, dest->frustumLights))
		{
			for (auto i = 0u; i < data->primaryLightCount; i++)
			{
				auto* lights = &data->frustumLights[i];
				write_array(buf, 3, lights->indices, lights->indexCount, dest_frustum_lights[i].indices);
				write_array(buf, 3, lights->vertices, lights->vertexCount, dest_frustum_lights[i].vertices);
			}
		}

		write_array(buf, 3, data->models, data->modelCount, dest->models);
		if (const auto dest_memory = write_array(buf, 15, data->materialMemory, data->materialMemoryCount, dest->materialMemory))
		{
			for (auto i = 0; i < data->materialMemoryCount; i++)
			{
				dest_memory[i].material = asset_reference(zone, ASSET_TYPE_MATERIAL, data->materialMemory[i].material);
			}
		}
		dest->sun.spriteMaterial = asset_reference(zone, ASSET_TYPE_MATERIAL, data->sun.spriteMaterial);
		dest->sun.flareMaterial = asset_reference(zone, ASSET_TYPE_MATERIAL, data->sun.flareMaterial);
		dest->outdoorImage = asset_reference(zone, ASSET_TYPE_IMAGE, data->outdoorImage);

		write_scene_runtime_arrays(buf, data, dest);
		write_shadow_geometry(buf, data->shadowGeom, data->primaryLightCount, dest->shadowGeom);
		write_shadow_geometry(buf, data->shadowGeomOptimized, data->primaryLightCount, dest->shadowGeomOptimized);
		write_light_regions(buf, data, dest);
		write_dpvs_static(zone, buf, data, dest);
		write_dpvs_dynamic(buf, data, dest);

		write_array(buf, 3, data->heroOnlyLights, data->heroOnlyLightCount, dest->heroOnlyLights);
		if (const auto dest_volumes = write_array(buf, 7, data->mdaoVolumes, data->mdaoVolumesCount, dest->mdaoVolumes))
		{
			for (auto i = 0u; i < data->mdaoVolumesCount; i++)
			{
				dest_volumes[i].volumeData = asset_reference(zone, ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData);
			}
		}

		write_string(buf, data->buildInfo.bspCommandline, dest->buildInfo.bspCommandline);
		write_string(buf, data->buildInfo.lightCommandline, dest->buildInfo.lightCommandline);
		write_string(buf, data->buildInfo.bspTimestamp, dest->buildInfo.bspTimestamp);
		write_string(buf, data->buildInfo.lightTimestamp, dest->buildInfo.lightTimestamp);
		buf->pop_stream();
	}

	void gfx_world::dump(GfxWorld* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_GFXWORLD, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_string(asset->baseName);

		write.dump_array(asset->skies, asset->skyCount);
		for (auto i = 0; asset->skies && i < asset->skyCount; i++)
		{
			write.dump_array(asset->skies[i].skyStartSurfs, asset->skies[i].skySurfCount);
			write.dump_asset(asset->skies[i].skyImage);
		}

		write.dump_array(asset->dpvsPlanes.planes, asset->planeCount);
		write.dump_array(asset->dpvsPlanes.nodes, asset->nodeCount);

		dump_cells(write, asset);

		write.dump_array(asset->portalGroup, asset->portalGroupCount);
		for (auto i = 0u; asset->portalGroup && i < asset->portalGroupCount; i++)
		{
			write.dump_string(asset->portalGroup[i].targetName);
			write.dump_array(asset->portalGroup[i].gfxPortalArray, asset->portalGroup[i].numPortals);
		}
		write.dump_array(asset->portalDistanceAnchorsAndCloseDistSquared, asset->portalDistanceAnchorCount);

		dump_draw(write, &asset->draw);
		dump_light_grid(write, &asset->lightGrid);
		dump_probe_data(write, &asset->probeData);
		dump_unknown_data(write, &asset->unknownData);
		dump_voxel_trees(write, asset);

		write.dump_array(asset->lightLists.surfaceListOffsets, 0);
		write.dump_array(asset->lightLists.smodelListOffsets, 0);
		write.dump_array(asset->lightLists.lists, asset->lightLists.listsSize);

		write.dump_array(asset->frustumLights, asset->primaryLightCount);
		for (auto i = 0u; asset->frustumLights && i < asset->primaryLightCount; i++)
		{
			write.dump_array(asset->frustumLights[i].indices, asset->frustumLights[i].indexCount);
			write.dump_array(asset->frustumLights[i].vertices, asset->frustumLights[i].vertexCount);
		}

		write.dump_array(asset->models, asset->modelCount);
		write.dump_array(asset->materialMemory, asset->materialMemoryCount);
		for (auto i = 0; asset->materialMemory && i < asset->materialMemoryCount; i++)
		{
			write.dump_asset(asset->materialMemory[i].material);
		}
		write.dump_asset(asset->sun.spriteMaterial);
		write.dump_asset(asset->sun.flareMaterial);
		write.dump_asset(asset->outdoorImage);

		dump_shadow_geometry(write, asset->shadowGeom, asset->primaryLightCount);
		dump_shadow_geometry(write, asset->shadowGeomOptimized, asset->primaryLightCount);
		dump_light_regions(write, asset);
		dump_dpvs_static(write, asset);

		write.dump_array(asset->heroOnlyLights, asset->heroOnlyLightCount);
		write.dump_array(asset->mdaoVolumes, asset->mdaoVolumesCount);
		for (auto i = 0u; asset->mdaoVolumes && i < asset->mdaoVolumesCount; i++)
		{
			write.dump_asset(asset->mdaoVolumes[i].volumeData);
		}

		write.dump_string(asset->buildInfo.bspCommandline);
		write.dump_string(asset->buildInfo.lightCommandline);
		write.dump_string(asset->buildInfo.bspTimestamp);
		write.dump_string(asset->buildInfo.lightTimestamp);
		write.close();
	}
}
