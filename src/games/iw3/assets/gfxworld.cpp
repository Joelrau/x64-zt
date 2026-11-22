#include <std_include.hpp>
#include "gfxworld.hpp"

namespace zonetool::iw3
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

		void read_cell(zone_reader32& reader, GfxCell* cell);

		void read_portal(zone_reader32& reader, GfxPortal* portal)
		{
			if (const auto linked = reader.read_single(portal->cell, 3))
			{
				read_cell(reader, linked);
			}

			reader.read_array(portal->vertices, 3, portal->vertexCount);
		}

		void read_cell(zone_reader32& reader, GfxCell* cell)
		{
			if (const auto trees = reader.read_array(cell->aabbTree, 3, cell->aabbTreeCount))
			{
				for (auto i = 0; i < cell->aabbTreeCount; i++)
				{
					reader.read_array(trees[i].smodelIndexes, 1, trees[i].smodelIndexCount);
				}
			}

			if (const auto portals = reader.read_array(cell->portals, 3, cell->portalCount))
			{
				for (auto i = 0; i < cell->portalCount; i++)
				{
					read_portal(reader, &portals[i]);
				}
			}

			reader.read_array(cell->cullGroups, 3, cell->cullGroupCount);
			reader.read_array(cell->reflectionProbes, 0, cell->reflectionProbeCount);
		}

		void read_light_grid(zone_reader32& reader, GfxLightGrid& grid)
		{
			reader.read_array(grid.rowDataStart, 1, grid.maxs[grid.rowAxis] - grid.mins[grid.rowAxis] + 1);
			reader.read_array(grid.rawRowData, 0, grid.rawRowDataSize);
			reader.read_array(grid.entries, 3, grid.entryCount);
			reader.read_array(grid.colors, 3, grid.colorCount);
		}

		void read_dpvs_static(zone_reader32& reader, GfxWorld* asset)
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

			read_runtime(reader, dpvs.lodData, 127, 2 * dpvs.smodelVisDataCount);
			reader.read_array(dpvs.sortedSurfIndex, 1, dpvs.staticSurfaceCount + dpvs.staticSurfaceCountNoDecal);
			reader.read_array(dpvs.smodelInsts, 3, dpvs.smodelCount);

			if (const auto surfaces = reader.read_array(dpvs.surfaces, 3, asset->surfaceCount))
			{
				for (auto i = 0; i < asset->surfaceCount; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, surfaces[i].material);
				}
			}

			reader.read_array(dpvs.cullGroups, 3, asset->cullGroupCount);

			if (const auto instances = reader.read_array(dpvs.smodelDrawInsts, 3, dpvs.smodelCount))
			{
				for (auto i = 0u; i < dpvs.smodelCount; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, instances[i].model);
				}
			}

			read_runtime(reader, dpvs.surfaceMaterials, 3, dpvs.staticSurfaceCount);
			read_runtime(reader, dpvs.surfaceCastsSunShadow, 127, dpvs.surfaceVisDataCount);
		}

		void read_dpvs_dynamic(zone_reader32& reader, GfxWorld* asset)
		{
			auto& dynamic = asset->dpvsDyn;

			for (auto i = 0; i < DYNENT_DRAW_COUNT; i++)
			{
				read_runtime(reader, dynamic.dynEntCellBits[i], 3, dynamic.dynEntClientWordCount[i] * asset->dpvsPlanes.cellCount);
			}

			for (auto i = 0; i < DYNENT_DRAW_COUNT; i++)
			{
				for (auto& data : dynamic.dynEntVisData[i])
				{
					read_runtime(reader, data, 15, 32 * dynamic.dynEntClientWordCount[i]);
				}
			}
		}
	}

	void gfx_world::read(zone_reader32& reader, GfxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->baseName);
		reader.read_array(asset->indices, 1, asset->indexCount);
		reader.read_array(asset->skyStartSurfs, 3, asset->skySurfCount);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->skyImage);

		if (const auto light = reader.read_single(asset->sunLight, 3))
		{
			reader.read_asset(ASSET_TYPE_LIGHT_DEF, light->def);
		}

		if (const auto probes = reader.read_array(asset->reflectionProbes, 3, asset->reflectionProbeCount))
		{
			for (auto i = 0u; i < asset->reflectionProbeCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, probes[i].reflectionImage);
			}
		}

		read_runtime(reader, asset->reflectionProbeTextures, 3, asset->reflectionProbeCount);

		auto& planes = asset->dpvsPlanes;
		reader.read_array(planes.planes, 3, asset->planeCount);
		reader.read_array(planes.nodes, 1, asset->nodeCount);
		read_runtime(reader, planes.sceneEntCellBits, 3, planes.cellCount * 256);

		if (const auto cells = reader.read_array(asset->cells, 3, planes.cellCount))
		{
			for (auto i = 0; i < planes.cellCount; i++)
			{
				read_cell(reader, &cells[i]);
			}
		}

		if (const auto lightmaps = reader.read_array(asset->lightmaps, 3, asset->lightmapCount))
		{
			for (auto i = 0; i < asset->lightmapCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, lightmaps[i].primary);
				reader.read_asset(ASSET_TYPE_IMAGE, lightmaps[i].secondary);
			}
		}

		read_light_grid(reader, asset->lightGrid);
		read_runtime(reader, asset->lightmapPrimaryTextures, 3, asset->lightmapCount);
		read_runtime(reader, asset->lightmapSecondaryTextures, 3, asset->lightmapCount);
		reader.read_array(asset->models, 3, asset->modelCount);

		if (const auto memory = reader.read_array(asset->materialMemory, 3, asset->materialMemoryCount))
		{
			for (auto i = 0; i < asset->materialMemoryCount; i++)
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, memory[i].material);
			}
		}

		reader.read_array(asset->vd.vertices, 3, asset->vertexCount);
		reader.read_array(asset->vld.data, 0, asset->vertexLayerDataSize);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->sun.spriteMaterial);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->sun.flareMaterial);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->outdoorImage);

		const auto& dynamic = asset->dpvsDyn;
		const auto shadow_lights = asset->primaryLightCount - asset->sunPrimaryLightIndex - 1;
		read_runtime(reader, asset->cellCasterBits, 3, planes.cellCount * ((planes.cellCount + 31) / 32));
		read_runtime(reader, asset->sceneDynModel, 3, dynamic.dynEntClientCount[0]);
		read_runtime(reader, asset->sceneDynBrush, 3, dynamic.dynEntClientCount[1]);
		read_runtime(reader, asset->primaryLightEntityShadowVis, 3, shadow_lights * 4096);
		read_runtime(reader, asset->primaryLightDynEntShadowVis[0], 3, dynamic.dynEntClientCount[0] * shadow_lights);
		read_runtime(reader, asset->primaryLightDynEntShadowVis[1], 3, dynamic.dynEntClientCount[1] * shadow_lights);
		read_runtime(reader, asset->nonSunPrimaryLightForModelDynEnt, 0, dynamic.dynEntClientCount[0]);

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

		read_dpvs_static(reader, asset);
		read_dpvs_dynamic(reader, asset);
		reader.pop_stream();
	}
}
