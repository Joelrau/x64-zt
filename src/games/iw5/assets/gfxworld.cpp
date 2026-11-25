#include <std_include.hpp>
#include "gfxworld.hpp"

namespace zonetool::iw5
{
	namespace
	{
		template <typename T>
		void read_runtime(zone_reader32& reader, x86::pointer<T>& field, const std::size_t alignment, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(field, alignment, count);
			reader.pop_stream();
		}

		template <typename T>
		void read_runtime_stream(zone_reader32& reader, x86::pointer<T>& field, const std::size_t alignment, const std::size_t element_size, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_stream(field, alignment, element_size, count);
			reader.pop_stream();
		}

		void read_planes(zone_reader32& reader, GfxWorld* asset)
		{
			auto& planes = asset->dpvsPlanes;
			reader.read_array(planes.planes, 3, asset->planeCount);
			reader.read_array(planes.nodes, 1, asset->nodeCount);
			read_runtime(reader, planes.sceneEntCellBits, 3, planes.cellCount * 512);
		}

		void read_cell_trees(zone_reader32& reader, GfxWorld* asset)
		{
			const auto cell_count = asset->dpvsPlanes.cellCount;
			const auto counts = reader.read_array(asset->aabbTreeCounts, 3, cell_count);

			if (const auto trees = reader.read_array(asset->aabbTrees, 127, cell_count))
			{
				for (auto i = 0; i < cell_count; i++)
				{
					const auto tree_count = counts ? counts[i].aabbTreeCount : 0;
					if (const auto nodes = reader.read_array(trees[i].aabbTree, 3, tree_count))
					{
						for (auto j = 0; j < tree_count; j++)
						{
							reader.read_array(nodes[j].smodelIndexes, 1, nodes[j].smodelIndexCount);
						}
					}
				}
			}
		}

		void read_cells(zone_reader32& reader, GfxWorld* asset)
		{
			const auto cell_count = asset->dpvsPlanes.cellCount;
			if (const auto cells = reader.read_array(asset->cells, 3, cell_count))
			{
				for (auto i = 0; i < cell_count; i++)
				{
					auto& cell = cells[i];
					if (const auto portals = reader.read_array(cell.portals, 3, cell.portalCount))
					{
						for (auto j = 0; j < cell.portalCount; j++)
						{
							reader.read_array(portals[j].vertices, 3, portals[j].vertexCount);
						}
					}

					reader.read_array(cell.reflectionProbes, 0, cell.reflectionProbeCount);
					reader.read_array(cell.reflectionProbeReferences, 0, cell.reflectionProbeReferenceCount);
				}
			}
		}

		void read_draw(zone_reader32& reader, GfxWorldDraw& draw)
		{
			reader.read_asset_array(ASSET_TYPE_IMAGE, draw.reflectionProbes, draw.reflectionProbeCount);
			reader.read_array(draw.reflectionProbeOrigins, 3, draw.reflectionProbeCount);
			read_runtime(reader, draw.reflectionProbeTextures, 3, draw.reflectionProbeCount);
			reader.read_array(draw.reflectionProbeReferenceOrigins, 3, draw.reflectionProbeReferenceCount);
			reader.read_array(draw.reflectionProbeReferences, 0, draw.reflectionProbeReferenceCount);

			if (const auto lightmaps = reader.read_array(draw.lightmaps, 3, draw.lightmapCount))
			{
				for (auto i = 0; i < draw.lightmapCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, lightmaps[i].primary);
					reader.read_asset(ASSET_TYPE_IMAGE, lightmaps[i].secondary);
				}
			}

			read_runtime(reader, draw.lightmapPrimaryTextures, 3, draw.lightmapCount);
			read_runtime(reader, draw.lightmapSecondaryTextures, 3, draw.lightmapCount);
			reader.read_asset(ASSET_TYPE_IMAGE, draw.lightmapOverridePrimary);
			reader.read_asset(ASSET_TYPE_IMAGE, draw.lightmapOverrideSecondary);
			reader.read_array(draw.vd.vertices, 3, draw.vertexCount);
			reader.read_array(draw.vld.data, 0, draw.vertexLayerDataSize);
			reader.read_array(draw.indices, 1, draw.indexCount);
		}

		void read_light_grid(zone_reader32& reader, GfxLightGrid& grid)
		{
			reader.read_array(grid.rowDataStart, 1, grid.maxs[grid.rowAxis] - grid.mins[grid.rowAxis] + 1);
			reader.read_array(grid.rawRowData, 0, grid.rawRowDataSize);
			reader.read_array(grid.entries, 3, grid.entryCount);
			reader.read_array(grid.colors, 3, grid.colorCount);
		}

		void read_scene_runtime(zone_reader32& reader, GfxWorld* asset)
		{
			const auto cell_count = asset->dpvsPlanes.cellCount;
			const auto& client_count = asset->dpvsDyn.dynEntClientCount;
			const auto light_count = asset->primaryLightCount - asset->lastSunPrimaryLightIndex - 1;

			read_runtime(reader, asset->cellCasterBits, 3, cell_count * ((cell_count + 31) / 32));
			read_runtime(reader, asset->cellHasSunLitSurfsBits, 3, (cell_count + 31) / 32);
			read_runtime(reader, asset->sceneDynModel, 1, client_count[0]);
			read_runtime(reader, asset->sceneDynBrush, 3, client_count[1]);
			read_runtime(reader, asset->primaryLightEntityShadowVis, 3, light_count * 8192);
			read_runtime(reader, asset->primaryLightDynEntShadowVis[0], 3, client_count[0] * light_count);
			read_runtime(reader, asset->primaryLightDynEntShadowVis[1], 3, client_count[1] * light_count);
			read_runtime(reader, asset->nonSunPrimaryLightForModelDynEnt, 0, client_count[0]);
		}

		void read_lights(zone_reader32& reader, GfxWorld* asset)
		{
			if (const auto shadows = reader.read_array(asset->shadowGeom, 3, asset->primaryLightCount))
			{
				for (auto i = 0u; i < asset->primaryLightCount; i++)
				{
					reader.read_array(shadows[i].sortedSurfIndex, 1, shadows[i].surfaceCount);
					reader.read_array(shadows[i].smodelIndex, 1, shadows[i].smodelCount);
				}
			}

			if (const auto regions = reader.read_array(asset->lightRegion, 3, asset->primaryLightCount))
			{
				for (auto i = 0u; i < asset->primaryLightCount; i++)
				{
					if (const auto hulls = reader.read_array(regions[i].hulls, 3, regions[i].hullCount))
					{
						for (auto j = 0u; j < regions[i].hullCount; j++)
						{
							reader.read_array(hulls[j].axis, 3, hulls[j].axisCount);
						}
					}
				}
			}
		}

		void read_dpvs(zone_reader32& reader, GfxWorld* asset)
		{
			auto& dpvs = asset->dpvs;
			for (auto& data : dpvs.smodelVisData)
			{
				read_runtime(reader, data, 0, dpvs.smodelCount);
			}
			for (auto& data : dpvs.surfaceVisData)
			{
				read_runtime(reader, data, 0, dpvs.staticSurfaceCount);
			}

			reader.read_array(dpvs.sortedSurfIndex, 1, dpvs.staticSurfaceCount + dpvs.staticSurfaceCountNoDecal);
			reader.read_array(dpvs.smodelInsts, 3, dpvs.smodelCount);

			if (const auto surfaces = reader.read_array(dpvs.surfaces, 3, asset->surfaceCount))
			{
				for (auto i = 0u; i < asset->surfaceCount; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, surfaces[i].material);
				}
			}

			reader.read_array(dpvs.surfacesBounds, 3, asset->surfaceCount);

			if (const auto instances = reader.read_array(dpvs.smodelDrawInsts, 3, dpvs.smodelCount))
			{
				for (auto i = 0u; i < dpvs.smodelCount; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, instances[i].model);
				}
			}

			read_runtime(reader, dpvs.surfaceMaterials, 7, asset->surfaceCount);
			read_runtime_stream(reader, dpvs.surfaceCastsSunShadow, 127, 16, dpvs.surfaceVisDataCount);
		}

		void read_dpvs_dynamic(zone_reader32& reader, GfxWorld* asset)
		{
			auto& dpvs = asset->dpvsDyn;
			for (auto i = 0; i < 2; i++)
			{
				read_runtime(reader, dpvs.dynEntCellBits[i], 3, dpvs.dynEntClientWordCount[i] * asset->dpvsPlanes.cellCount);
			}

			for (auto i = 0; i < 2; i++)
			{
				for (auto& data : dpvs.dynEntVisData[i])
				{
					read_runtime_stream(reader, data, 15, 16, 32 * dpvs.dynEntClientWordCount[i]);
				}
			}
		}
	}

	void gfx_world::read(zone_reader32& reader, GfxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->baseName);

		if (const auto skies = reader.read_array(asset->skies, 3, asset->skyCount))
		{
			for (auto i = 0; i < asset->skyCount; i++)
			{
				reader.read_array(skies[i].skyStartSurfs, 3, skies[i].skySurfCount);
				reader.read_asset(ASSET_TYPE_IMAGE, skies[i].skyImage);
			}
		}

		read_planes(reader, asset);
		read_cell_trees(reader, asset);
		read_cells(reader, asset);
		read_draw(reader, asset->draw);
		read_light_grid(reader, asset->lightGrid);
		reader.read_array(asset->models, 3, asset->modelCount);

		if (const auto memory = reader.read_array(asset->materialMemory, 3, asset->materialMemoryCount))
		{
			for (auto i = 0; i < asset->materialMemoryCount; i++)
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, memory[i].material);
			}
		}

		reader.read_asset(ASSET_TYPE_MATERIAL, asset->sun.spriteMaterial);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->sun.flareMaterial);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->outdoorImage);
		read_scene_runtime(reader, asset);
		read_lights(reader, asset);
		read_dpvs(reader, asset);
		read_dpvs_dynamic(reader, asset);
		reader.read_array(asset->heroOnlyLights, 3, asset->heroOnlyLightCount);
		reader.pop_stream();
	}
}
