#include <std_include.hpp>
#include "gfxworld.hpp"

namespace zonetool::iw6
{
	GfxWorld* gfx_world::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".gfxmap";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gfxmap \"%s\"...", name.data());

		GfxWorld* asset = read.read_single<GfxWorld>();
		asset->name = read.read_string();
		asset->baseName = read.read_string();

		asset->skies = read.read_array<GfxSky>();
		for (int i = 0; i < asset->skyCount; i++)
		{
			asset->skies[i].skyStartSurfs = read.read_array<int>();
			asset->skies[i].skyImage = read.read_asset<GfxImage>();
		}

		asset->dpvsPlanes.planes = read.read_array<cplane_s>();
		asset->dpvsPlanes.nodes = read.read_array<unsigned short>();
		asset->dpvsPlanes.sceneEntCellBits = read.read_array<unsigned int>();

		asset->aabbTreeCounts = read.read_array<GfxCellTreeCount>();
		asset->aabbTrees = read.read_array<GfxCellTree>();
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			asset->aabbTrees[i].aabbTree = read.read_array<GfxAabbTree>();
			for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
			{
				asset->aabbTrees[i].aabbTree[j].smodelIndexes = read.read_array<unsigned short>();
			}
		}

		asset->cells = read.read_array<GfxCell>();
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			asset->cells[i].portals = read.read_array<GfxPortal>();
			for (int j = 0; j < asset->cells[i].portalCount; j++)
			{
				asset->cells[i].portals[j].vertices = read.read_array<float[3]>();
			}
			asset->cells[i].reflectionProbes = read.read_array<unsigned char>();
			asset->cells[i].reflectionProbeReferences = read.read_array<unsigned char>();
		}

		asset->draw.reflectionProbes = read.read_array<GfxImage*>();
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			asset->draw.reflectionProbes[i] = read.read_asset<GfxImage>();
		}

		asset->draw.reflectionProbeOrigins = read.read_array<GfxReflectionProbe>();
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			asset->draw.reflectionProbeOrigins[i].probeVolumes = read.read_array<GfxReflectionProbeVolumeData>();
		}

		asset->draw.reflectionProbeTextures = read.read_array<GfxTexture>();

		asset->draw.reflectionProbeReferenceOrigins = read.read_array<GfxReflectionProbeReferenceOrigin>();
		asset->draw.reflectionProbeReferences = read.read_array<GfxReflectionProbeReference>();

		asset->draw.lightmaps = read.read_array<GfxLightmapArray>();
		for (int i = 0; i < asset->draw.lightmapCount; i++)
		{
			asset->draw.lightmaps[i].primary = read.read_asset<GfxImage>();
			asset->draw.lightmaps[i].secondary = read.read_asset<GfxImage>();
		}

		asset->draw.lightmapPrimaryTextures = read.read_array<GfxTexture>();
		asset->draw.lightmapSecondaryTextures = read.read_array<GfxTexture>();
		asset->draw.lightmapOverridePrimary = read.read_asset<GfxImage>();
		asset->draw.lightmapOverrideSecondary = read.read_asset<GfxImage>();

		asset->draw.vd.vertices = read.read_array<GfxWorldVertex>();
		asset->draw.vld.data = read.read_array<unsigned char>();
		asset->draw.indices = read.read_array<unsigned short>();

		asset->lightGrid.rowDataStart = read.read_array<unsigned short>();
		asset->lightGrid.rawRowData = read.read_array<unsigned char>();
		asset->lightGrid.entries = read.read_array<GfxLightGridEntry>();
		asset->lightGrid.colors = read.read_array<GfxLightGridColors>();

		asset->lightGrid.stageLightingContrastGain = read.read_array<float>();
		asset->lightGrid.paletteEntryAddress = read.read_array<int>();
		asset->lightGrid.paletteBitstream = read.read_array<unsigned char>();

		asset->lightGrid.tree.p_nodeTable = read.read_array<unsigned int>();
		asset->lightGrid.tree.p_leafTable = read.read_array<unsigned char>();

		asset->models = read.read_array<GfxBrushModel>();

		asset->materialMemory = read.read_array<MaterialMemory>();
		for (int i = 0; i < asset->materialMemoryCount; i++)
		{
			asset->materialMemory[i].material = read.read_asset<Material>();
		}

		asset->sun.spriteMaterial = read.read_asset<Material>();
		asset->sun.flareMaterial = read.read_asset<Material>();

		asset->outdoorImage = read.read_asset<GfxImage>();

		asset->cellCasterBits = read.read_array<unsigned int>();
		asset->cellHasSunLitSurfsBits = read.read_array<unsigned int>();

		asset->sceneDynModel = read.read_array<GfxSceneDynModel>();
		asset->sceneDynBrush = read.read_array<GfxSceneDynBrush>();

		asset->primaryLightEntityShadowVis = read.read_array<unsigned int>();

		asset->primaryLightDynEntShadowVis[0] = read.read_array<unsigned int>();
		asset->primaryLightDynEntShadowVis[1] = read.read_array<unsigned int>();

		asset->nonSunPrimaryLightForModelDynEnt = read.read_array<unsigned short>();

		asset->shadowGeom = read.read_array<GfxShadowGeometry>();
		if (asset->shadowGeom)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->shadowGeom[i].sortedSurfIndex = read.read_array<unsigned int>();
				asset->shadowGeom[i].smodelIndex = read.read_array<unsigned short>();
			}
		}
		asset->shadowGeomOptimized = read.read_array<GfxShadowGeometry>();
		if (asset->shadowGeomOptimized)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->shadowGeomOptimized[i].sortedSurfIndex = read.read_array<unsigned int>();
				asset->shadowGeomOptimized[i].smodelIndex = read.read_array<unsigned short>();
			}
		}

		asset->lightRegion = read.read_array<GfxLightRegion>();
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			asset->lightRegion[i].hulls = read.read_array<GfxLightRegionHull>();
			for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
			{
				asset->lightRegion[i].hulls[j].axis = read.read_array<GfxLightRegionAxis>();
			}
		}

		asset->dpvs.smodelVisData[0] = read.read_array<unsigned int>();
		asset->dpvs.smodelVisData[1] = read.read_array<unsigned int>();
		asset->dpvs.smodelVisData[2] = read.read_array<unsigned int>();

		asset->dpvs.surfaceVisData[0] = read.read_array<unsigned int>();
		asset->dpvs.surfaceVisData[1] = read.read_array<unsigned int>();
		asset->dpvs.surfaceVisData[2] = read.read_array<unsigned int>();

		asset->dpvs.unknownData01[0] = read.read_array<unsigned int>();
		asset->dpvs.unknownData01[1] = read.read_array<unsigned int>();
		asset->dpvs.unknownData01[2] = read.read_array<unsigned int>();

		asset->dpvs.unknownData02[0] = read.read_array<unsigned int>();
		asset->dpvs.unknownData02[1] = read.read_array<unsigned int>();
		asset->dpvs.unknownData02[2] = read.read_array<unsigned int>();

		asset->dpvs.lodData = read.read_array<unsigned int>();

		asset->dpvs.tessellationCutoffVisData[0] = read.read_array<unsigned int>();
		asset->dpvs.tessellationCutoffVisData[1] = read.read_array<unsigned int>();
		asset->dpvs.tessellationCutoffVisData[2] = read.read_array<unsigned int>();

		asset->dpvs.sortedSurfIndex = read.read_array<unsigned int>();

		asset->dpvs.smodelInsts = read.read_array<GfxStaticModelInst>();
		asset->dpvs.surfaces = read.read_array<GfxSurface>();
		for (unsigned int i = 0; i < asset->surfaceCount; i++)
		{
			asset->dpvs.surfaces[i].material = read.read_asset<Material>();
		}
		asset->dpvs.surfacesBounds = read.read_array<GfxSurfaceBounds>();
		asset->dpvs.smodelDrawInsts = read.read_array<GfxStaticModelDrawInst>();
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			asset->dpvs.smodelDrawInsts[i].model = read.read_asset<XModel>();
			asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues =
				read.read_array<GfxStaticModelVertexLighting>();
		}

		asset->dpvs.surfaceMaterials = read.read_array<GfxDrawSurf>();
		asset->dpvs.surfaceCastsSunShadow = read.read_array<unsigned int>();
		asset->dpvs.surfaceCastsSunShadowOpt = read.read_array<unsigned int>();

		asset->dpvs.constantBuffersLit = read.read_array<char*>();
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			asset->dpvs.constantBuffersLit[i] = read.read_raw<char>();
		}
		asset->dpvs.constantBuffersAmbient = read.read_array<char*>();
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			asset->dpvs.constantBuffersAmbient[i] = read.read_raw<char>();
		}

		asset->dpvsDyn.dynEntCellBits[0] = read.read_array<unsigned int>();
		asset->dpvsDyn.dynEntCellBits[1] = read.read_array<unsigned int>();

		asset->dpvsDyn.dynEntVisData[0][0] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[1][0] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[0][1] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[1][1] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[0][2] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[1][2] = read.read_array<unsigned char>();

		asset->heroOnlyLights = read.read_array<GfxHeroOnlyLight>();

		asset->umbraTomeData = read.read_array<char>();

		read.close();

		return asset;
	}

	void gfx_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
		this->asset_ = this->parse(name, mem);

		this->base_name_ = filesystem::get_fastfile();

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse gfxmap \"%s\"", name.data());
		}
	}

	void gfx_world::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_world::load_depending(zone_base* zone)
	{
		auto data = this->asset_;

		// Skies
		if (data->skyCount)
		{
			for (int i = 0; i < data->skyCount; i++)
			{
				if (data->skies[i].skyImage)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->skies[i].skyImage->name);
				}
			}
		}

		// ReflectionImages
		if (data->draw.reflectionProbes)
		{
			for (unsigned int i = 0; i < data->draw.reflectionProbeCount; i++)
			{
				if (data->draw.reflectionProbes[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.reflectionProbes[i]->name);
				}
			}
		}

		// Lightmaps
		if (data->draw.lightmaps)
		{
			for (int i = 0; i < data->draw.lightmapCount; i++)
			{
				if (data->draw.lightmaps[i].primary)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmaps[i].primary->name);
				}

				if (data->draw.lightmaps[i].secondary)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmaps[i].secondary->name);
				}
			}
		}

		// LightmapOverridePrimary
		if (data->draw.lightmapOverridePrimary)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmapOverridePrimary->name);
		}

		// LightmapOverrideSecondary
		if (data->draw.lightmapOverrideSecondary)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmapOverrideSecondary->name);
		}

		// MaterialMemory
		if (data->materialMemory)
		{
			for (int i = 0; i < data->materialMemoryCount; i++)
			{
				if (data->materialMemory[i].material)
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->materialMemory[i].material->name);
				}
			}
		}

		// Sunflare_t
		if (data->sun.spriteMaterial)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->sun.spriteMaterial->name);
		}
		if (data->sun.flareMaterial)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->sun.flareMaterial->name);
		}

		// OutdoorImage
		if (data->outdoorImage)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->outdoorImage->name);
		}

		// Dpvs
		if (data->dpvs.smodelDrawInsts)
		{
			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				if (data->dpvs.smodelDrawInsts[i].model)
				{
					zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->dpvs.smodelDrawInsts[i].model->name);
				}
			}
		}
		if (data->dpvs.surfaces)
		{
			for (unsigned int i = 0; i < data->surfaceCount; i++)
			{
				if (data->dpvs.surfaces[i].material)
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->dpvs.surfaces[i].material->name);
				}
			}
		}
	}

	std::string gfx_world::name()
	{
		return this->name_;
	}

	std::int32_t gfx_world::type()
	{
		return ASSET_TYPE_GFXWORLD;
	}

	void gfx_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name_);
		dest->baseName = buf->write_str(this->base_name_);

		if (data->skies)
		{
			buf->align(3);
			auto* destskies = buf->write(data->skies, data->skyCount);
			for (int i = 0; i < data->skyCount; i++)
			{
				if (data->skies[i].skyStartSurfs)
				{
					buf->align(3);
					buf->write(data->skies[i].skyStartSurfs, data->skies[i].skySurfCount);
					buf->clear_pointer(&destskies[i].skyStartSurfs);
				}

				if (data->skies[i].skyImage)
				{
					destskies[i].skyImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						ASSET_TYPE_IMAGE, data->skies[i].skyImage->name));
				}
			}

			buf->clear_pointer(&dest->skies);
		}

		if (data->dpvsPlanes.planes)
		{
			dest->dpvsPlanes.planes = buf->write_s(3, data->dpvsPlanes.planes, data->planeCount);
		}

		if (data->dpvsPlanes.nodes)
		{
			buf->align(1);
			buf->write(data->dpvsPlanes.nodes, data->nodeCount);
			buf->clear_pointer(&dest->dpvsPlanes.nodes);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvsPlanes.sceneEntCellBits)
		{
			buf->align(3);
			buf->write(data->dpvsPlanes.sceneEntCellBits, data->dpvsPlanes.cellCount << 9);
			buf->clear_pointer(&dest->dpvsPlanes.sceneEntCellBits);
		}
		buf->pop_stream();

		if (data->aabbTreeCounts)
		{
			buf->align(3);
			buf->write(data->aabbTreeCounts, data->dpvsPlanes.cellCount);
			buf->clear_pointer(&dest->aabbTreeCounts);
		}

		if (data->aabbTrees)
		{
			buf->align(127);
			auto cell_tree = buf->write(data->aabbTrees, data->dpvsPlanes.cellCount);

			for (int i = 0; i < data->dpvsPlanes.cellCount; i++)
			{
				if (data->aabbTrees[i].aabbTree)
				{
					buf->align(3);
					auto gfx_aabb_tree = buf->write(data->aabbTrees[i].aabbTree,
						data->aabbTreeCounts[i].aabbTreeCount);

					for (int i2 = 0; i2 < data->aabbTreeCounts[i].aabbTreeCount; i2++)
					{
						if (data->aabbTrees[i].aabbTree[i2].smodelIndexes)
						{
							gfx_aabb_tree[i2].smodelIndexes = buf->write_s(
								1, data->aabbTrees[i].aabbTree[i2].smodelIndexes,
								data->aabbTrees[i].aabbTree[i2].smodelIndexCount);
						}
					}

					buf->clear_pointer(&cell_tree[i].aabbTree);
				}
			}

			buf->clear_pointer(&dest->aabbTrees);
		}

		if (data->cells)
		{
			buf->align(3);
			auto gfx_cell = buf->write(data->cells, data->dpvsPlanes.cellCount);

			for (int i = 0; i < data->dpvsPlanes.cellCount; i++)
			{
				if (data->cells[i].portals)
				{
					buf->align(3);
					auto gfx_portal = buf->write(data->cells[i].portals, data->cells[i].portalCount);

					for (int i2 = 0; i2 < data->cells[i].portalCount; i2++)
					{
						if (data->cells[i].portals[i2].vertices)
						{
							buf->align(3);
							buf->write(data->cells[i].portals[i2].vertices, data->cells[i].portals[i2].vertexCount);
							buf->clear_pointer(&gfx_portal[i2].vertices);
						}
					}

					buf->clear_pointer(&gfx_cell[i].portals);
				}

				if (data->cells[i].reflectionProbes)
				{
					buf->align(0);
					buf->write(data->cells[i].reflectionProbes, data->cells[i].reflectionProbeCount);
					buf->clear_pointer(&gfx_cell[i].reflectionProbes);
				}
				if (data->cells[i].reflectionProbeReferences)
				{
					buf->align(0);
					buf->write(data->cells[i].reflectionProbeReferences, data->cells[i].reflectionProbeReferenceCount);
					buf->clear_pointer(&gfx_cell[i].reflectionProbeReferences);
				}

				buf->clear_pointer(&dest->cells);
			}
		}

		if (data->draw.reflectionProbes)
		{
			buf->align(7);
			auto reflectionProbes = buf->write(data->draw.reflectionProbes,
				data->draw.reflectionProbeCount);

			for (unsigned int i = 0; i < data->draw.reflectionProbeCount; i++)
			{
				if (reflectionProbes[i])
				{
					reflectionProbes[i] = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						ASSET_TYPE_IMAGE, data->draw.reflectionProbes[i]->name));
				}
			}

			buf->clear_pointer(&dest->draw.reflectionProbes);
		}

		if (data->draw.reflectionProbeOrigins)
		{
			buf->align(3);
			auto reflectionProbeOrigins = buf->write(data->draw.reflectionProbeOrigins, data->draw.reflectionProbeCount);

			for (unsigned int i = 0; i < data->draw.reflectionProbeCount; i++)
			{
				if (reflectionProbeOrigins[i].probeVolumes)
				{
					buf->align(3);
					buf->write(data->draw.reflectionProbeOrigins[i].probeVolumes, data->draw.reflectionProbeOrigins[i].probeVolumeCount);
					buf->clear_pointer(&reflectionProbeOrigins[i].probeVolumes);
				}
			}

			buf->clear_pointer(&dest->draw.reflectionProbeOrigins);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->draw.reflectionProbeTextures)
		{
			buf->align(3);
			buf->write(data->draw.reflectionProbeTextures, data->draw.reflectionProbeCount);
			buf->clear_pointer(&dest->draw.reflectionProbeTextures);
		}
		buf->pop_stream();

		if (data->draw.reflectionProbeReferenceOrigins)
		{
			buf->align(3);
			buf->write(data->draw.reflectionProbeReferenceOrigins, data->draw.reflectionProbeReferenceCount);
			buf->clear_pointer(dest->draw.reflectionProbeReferenceOrigins);
		}

		if (data->draw.reflectionProbeReferences)
		{
			buf->align(0);
			buf->write(data->draw.reflectionProbeReferences, data->draw.reflectionProbeReferenceCount);
			buf->clear_pointer(&dest->draw.reflectionProbeReferences);
		}

		if (data->draw.lightmaps)
		{
			buf->align(3);
			auto destlightmaps = buf->write(data->draw.lightmaps, data->draw.lightmapCount);

			for (int i = 0; i < data->draw.lightmapCount; i++)
			{
				if (data->draw.lightmaps[i].primary)
				{
					destlightmaps[i].primary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						ASSET_TYPE_IMAGE, data->draw.lightmaps[i].primary->name));
				}

				if (data->draw.lightmaps[i].secondary)
				{
					destlightmaps[i].secondary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						ASSET_TYPE_IMAGE, data->draw.lightmaps[i].secondary->name));
				}
			}

			buf->clear_pointer(&dest->draw.lightmaps);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->draw.lightmapPrimaryTextures)
		{
			buf->align(3);
			buf->write(data->draw.lightmapPrimaryTextures, data->draw.lightmapCount);
			buf->clear_pointer(&dest->draw.lightmapPrimaryTextures);
		}
		buf->pop_stream();

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->draw.lightmapSecondaryTextures)
		{
			buf->align(3);
			buf->write(data->draw.lightmapSecondaryTextures, data->draw.lightmapCount);
			buf->clear_pointer(&dest->draw.lightmapSecondaryTextures);
		}
		buf->pop_stream();

		if (data->draw.lightmapOverridePrimary)
		{
			dest->draw.lightmapOverridePrimary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
				ASSET_TYPE_IMAGE, data->draw.lightmapOverridePrimary->name));
		}

		if (data->draw.lightmapOverrideSecondary)
		{
			dest->draw.lightmapOverrideSecondary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
				ASSET_TYPE_IMAGE, data->draw.lightmapOverrideSecondary->name));
		}

		if (data->draw.vd.vertices)
		{
			buf->align(3);
			buf->write(data->draw.vd.vertices, data->draw.vertexCount);
			buf->clear_pointer(&dest->draw.vd.vertices);
		}
		dest->draw.vd.worldVb = nullptr;

		if (data->draw.vld.data)
		{
			buf->align(0);
			buf->write(data->draw.vld.data, data->draw.vertexLayerDataSize);
			buf->clear_pointer(&dest->draw.vld.data);
		}
		dest->draw.vld.layerVb = nullptr;

		if (data->draw.indices)
		{
			buf->align(1);
			buf->write(data->draw.indices, data->draw.indexCount);
			buf->clear_pointer(&dest->draw.indices);
		}
		dest->draw.indexBuffer = nullptr;

		if (data->lightGrid.rowDataStart)
		{
			buf->align(1);
			buf->write(data->lightGrid.rowDataStart,
				data->lightGrid.maxs[data->lightGrid.rowAxis] - data->lightGrid.mins[data->lightGrid.rowAxis] + 1);
			buf->clear_pointer(&dest->lightGrid.rowDataStart);
		}

		if (data->lightGrid.rawRowData)
		{
			buf->align(0);
			buf->write(data->lightGrid.rawRowData, data->lightGrid.rawRowDataSize);
			buf->clear_pointer(&dest->lightGrid.rawRowData);
		}

		if (data->lightGrid.entries)
		{
			buf->align(3);
			buf->write(data->lightGrid.entries, data->lightGrid.entryCount);
			buf->clear_pointer(&dest->lightGrid.entries);
		}

		if (data->lightGrid.colors)
		{
			buf->align(3);
			buf->write(data->lightGrid.colors, data->lightGrid.colorCount);
			buf->clear_pointer(&dest->lightGrid.colors);
		}

		if (data->lightGrid.stageLightingContrastGain)
		{
			buf->align(3);
			buf->write(data->lightGrid.stageLightingContrastGain, data->lightGrid.stageCount);
			buf->clear_pointer(&dest->lightGrid.stageLightingContrastGain);
		}

		if (data->lightGrid.paletteEntryAddress)
		{
			buf->align(3);
			buf->write(data->lightGrid.paletteEntryAddress, data->lightGrid.paletteEntryCount);
			buf->clear_pointer(&dest->lightGrid.paletteEntryAddress);
		}

		if (data->lightGrid.paletteBitstream)
		{
			buf->align(0);
			buf->write(data->lightGrid.paletteBitstream, data->lightGrid.paletteBitstreamSize);
			buf->clear_pointer(&dest->lightGrid.paletteBitstream);
		}

		if (data->lightGrid.tree.p_nodeTable)
		{
			buf->align(3);
			buf->write(data->lightGrid.tree.p_nodeTable, data->lightGrid.tree.nodeCount);
			buf->clear_pointer(&dest->lightGrid.tree.p_nodeTable);
		}

		if (data->lightGrid.tree.p_leafTable)
		{
			buf->align(0);
			buf->write(data->lightGrid.tree.p_leafTable, data->lightGrid.tree.leafTableSize);
			buf->clear_pointer(&dest->lightGrid.tree.p_leafTable);
		}

		if (data->models)
		{
			buf->align(3);
			buf->write(data->models, data->modelCount);
			buf->clear_pointer(&dest->models);
		}

		if (data->materialMemory)
		{
			buf->align(3);
			auto memory = buf->write(data->materialMemory, data->materialMemoryCount);

			for (int i = 0; i < data->materialMemoryCount; i++)
			{
				if (data->materialMemory[i].material)
				{
					memory[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
						ASSET_TYPE_MATERIAL, data->materialMemory[i].material->name));
				}
			}

			buf->clear_pointer(&dest->materialMemory);
		}

		if (data->sun.spriteMaterial)
		{
			dest->sun.spriteMaterial = reinterpret_cast<Material*>(zone->get_asset_pointer(
				ASSET_TYPE_MATERIAL, data->sun.spriteMaterial->name));
		}
		if (data->sun.flareMaterial)
		{
			dest->sun.flareMaterial = reinterpret_cast<Material*>(zone->get_asset_pointer(
				ASSET_TYPE_MATERIAL, data->sun.flareMaterial->name));
		}

		if (data->outdoorImage)
		{
			dest->outdoorImage = reinterpret_cast<GfxImage*>(
				zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->outdoorImage->name));
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->cellCasterBits)
		{
			buf->align(3);
			buf->write(data->cellCasterBits,
				data->dpvsPlanes.cellCount * ((data->dpvsPlanes.cellCount + 31) >> 5));
			buf->clear_pointer(&dest->cellCasterBits);
		}

		if (data->cellHasSunLitSurfsBits)
		{
			buf->align(3);
			buf->write(data->cellHasSunLitSurfsBits, (data->dpvsPlanes.cellCount + 31) >> 5);
			buf->clear_pointer(&dest->cellHasSunLitSurfsBits);
		}

		if (data->sceneDynModel)
		{
			buf->align(3);
			buf->write(data->sceneDynModel, data->dpvsDyn.dynEntClientCount[0]);
			buf->clear_pointer(&dest->sceneDynModel);
		}

		if (data->sceneDynBrush)
		{
			buf->align(3);
			buf->write(data->sceneDynBrush, data->dpvsDyn.dynEntClientCount[1]);
			buf->clear_pointer(&dest->sceneDynBrush);
		}

		if (data->primaryLightEntityShadowVis)
		{
			buf->align(3);
			buf->write(data->primaryLightEntityShadowVis,
				((data->primaryLightCount - data->lastSunPrimaryLightIndex - 1) << 13));
			buf->clear_pointer(&dest->primaryLightEntityShadowVis);
		}

		if (data->primaryLightDynEntShadowVis[0])
		{
			buf->align(3);
			buf->write(data->primaryLightDynEntShadowVis[0],
				data->dpvsDyn.dynEntClientCount[0] * (data->primaryLightCount - data->lastSunPrimaryLightIndex - 1));
			buf->clear_pointer(&dest->primaryLightDynEntShadowVis[0]);
		}

		if (data->primaryLightDynEntShadowVis[1])
		{
			buf->align(3);
			buf->write(data->primaryLightDynEntShadowVis[1],
				data->dpvsDyn.dynEntClientCount[1] * (data->primaryLightCount - data->lastSunPrimaryLightIndex - 1));
			buf->clear_pointer(&dest->primaryLightDynEntShadowVis[1]);
		}

		if (data->nonSunPrimaryLightForModelDynEnt)
		{
			buf->align(1);
			buf->write(data->nonSunPrimaryLightForModelDynEnt, data->dpvsDyn.dynEntClientCount[0]);
			buf->clear_pointer(&dest->nonSunPrimaryLightForModelDynEnt);
		}
		buf->pop_stream();

		if (data->shadowGeom)
		{
			buf->align(3);
			auto shadow_geometry = buf->write(data->shadowGeom, data->primaryLightCount);

			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->shadowGeom[i].sortedSurfIndex)
				{
					buf->align(3);
					buf->write(data->shadowGeom[i].sortedSurfIndex, data->shadowGeom[i].surfaceCount);
					buf->clear_pointer(&shadow_geometry[i].sortedSurfIndex);
				}
				if (data->shadowGeom[i].smodelIndex)
				{
					buf->align(1);
					buf->write(data->shadowGeom[i].smodelIndex, data->shadowGeom[i].smodelCount);
					buf->clear_pointer(&shadow_geometry[i].smodelIndex);
				}
			}

			buf->clear_pointer(&dest->shadowGeom);
		}
		if (data->shadowGeomOptimized)
		{
			buf->align(3);
			auto shadow_geometry = buf->write(data->shadowGeomOptimized, data->primaryLightCount);

			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->shadowGeomOptimized[i].sortedSurfIndex)
				{
					buf->align(3);
					buf->write(data->shadowGeomOptimized[i].sortedSurfIndex, data->shadowGeomOptimized[i].surfaceCount);
					buf->clear_pointer(&shadow_geometry[i].sortedSurfIndex);
				}
				if (data->shadowGeomOptimized[i].smodelIndex)
				{
					buf->align(1);
					buf->write(data->shadowGeomOptimized[i].smodelIndex, data->shadowGeomOptimized[i].smodelCount);
					buf->clear_pointer(&shadow_geometry[i].smodelIndex);
				}
			}

			buf->clear_pointer(&dest->shadowGeom);
		}

		if (data->lightRegion)
		{
			buf->align(3);
			auto light_region = buf->write(data->lightRegion, data->primaryLightCount);

			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->lightRegion[i].hulls)
				{
					buf->align(3);
					auto light_region_hull = buf->write(data->lightRegion[i].hulls, data->lightRegion[i].hullCount);

					for (unsigned int i2 = 0; i2 < data->lightRegion[i].hullCount; i2++)
					{
						if (data->lightRegion[i].hulls[i2].axis)
						{
							buf->align(3);
							buf->write(data->lightRegion[i].hulls[i2].axis,
								data->lightRegion[i].hulls[i2].axisCount);
							buf->clear_pointer(&light_region_hull[i2].axis);
						}
					}

					buf->clear_pointer(&light_region[i].hulls);
				}
			}

			buf->clear_pointer(&dest->lightRegion);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvs.smodelVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelVisData[0], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelVisData[0]);
		}
		if (data->dpvs.smodelVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelVisData[1], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelVisData[1]);
		}
		if (data->dpvs.smodelVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelVisData[2], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelVisData[2]);
		}

		if (data->dpvs.surfaceVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceVisData[0], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceVisData[0]);
		}
		if (data->dpvs.surfaceVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceVisData[1], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceVisData[1]);
		}
		if (data->dpvs.surfaceVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceVisData[2], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceVisData[2]);
		}

		if (data->dpvs.unknownData01[0])
		{
			buf->align(127);
			buf->write(data->dpvs.unknownData01[0], data->dpvs.smodelVisDataCount + 1);
			buf->clear_pointer(&dest->dpvs.unknownData01[0]);
		}
		if (data->dpvs.unknownData01[1])
		{
			buf->align(127);
			buf->write(data->dpvs.unknownData01[1], data->dpvs.smodelVisDataCount + 1);
			buf->clear_pointer(&dest->dpvs.unknownData01[1]);
		}
		if (data->dpvs.unknownData01[2])
		{
			buf->align(127);
			buf->write(data->dpvs.unknownData01[2], data->dpvs.smodelVisDataCount + 1);
			buf->clear_pointer(&dest->dpvs.unknownData01[2]);
		}

		if (data->dpvs.unknownData02[0])
		{
			buf->align(127);
			buf->write(data->dpvs.unknownData02[0], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.unknownData02[0]);
		}
		if (data->dpvs.unknownData02[1])
		{
			buf->align(127);
			buf->write(data->dpvs.unknownData02[1], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.unknownData02[1]);
		}
		if (data->dpvs.unknownData02[2])
		{
			buf->align(127);
			buf->write(data->dpvs.unknownData02[2], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.unknownData02[2]);
		}
		buf->pop_stream();

		if (data->dpvs.lodData)
		{
			buf->align(3);
			buf->write(data->dpvs.lodData, data->dpvs.smodelCount + 1);
			buf->clear_pointer(&dest->dpvs.lodData);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvs.tessellationCutoffVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.tessellationCutoffVisData[0], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.tessellationCutoffVisData[0]);
		}
		if (data->dpvs.tessellationCutoffVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.tessellationCutoffVisData[1], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.tessellationCutoffVisData[1]);
		}
		if (data->dpvs.tessellationCutoffVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.tessellationCutoffVisData[2], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.tessellationCutoffVisData[2]);
		}
		buf->pop_stream();

		if (data->dpvs.sortedSurfIndex)
		{
			buf->align(3);
			buf->write(data->dpvs.sortedSurfIndex, data->dpvs.staticSurfaceCount);
			buf->clear_pointer(&dest->dpvs.sortedSurfIndex);
		}

		if (data->dpvs.smodelInsts)
		{
			buf->align(3);
			buf->write(data->dpvs.smodelInsts, data->dpvs.smodelCount);
			buf->clear_pointer(&dest->dpvs.smodelInsts);
		}

		if (data->dpvs.surfaces)
		{
			buf->align(3);
			auto destgfxsurfs = buf->write(data->dpvs.surfaces, data->surfaceCount);

			for (unsigned int i = 0; i < data->surfaceCount; i++)
			{
				if (data->dpvs.surfaces[i].material)
				{
					destgfxsurfs[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->dpvs.surfaces[i].material->name));
				}
			}

			buf->clear_pointer(&dest->dpvs.surfaces);
		}

		if (data->dpvs.surfacesBounds)
		{
			buf->align(3);
			buf->write(data->dpvs.surfacesBounds, data->surfaceCount);
			buf->clear_pointer(&dest->dpvs.surfacesBounds);
		}

		if (data->dpvs.smodelDrawInsts)
		{
			buf->align(3);
			auto destdrawinst = buf->write(data->dpvs.smodelDrawInsts, data->dpvs.smodelCount);

			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				if (data->dpvs.smodelDrawInsts[i].model)
				{
					destdrawinst[i].model = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->dpvs.smodelDrawInsts[i].model->name));
				}
				if (data->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues)
				{
					destdrawinst[i].vertexLightingInfo.lightingValues = buf->write_s(3,
						data->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues, data->dpvs.smodelDrawInsts[i].vertexLightingInfo.numLightingValues);
				}
			}

			buf->clear_pointer(&dest->dpvs.smodelDrawInsts);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvs.surfaceMaterials)
		{
			buf->align(7);
			buf->write(data->dpvs.surfaceMaterials, data->surfaceCount);
			buf->clear_pointer(&dest->dpvs.surfaceMaterials);
		}
		if (data->dpvs.surfaceCastsSunShadow)
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceCastsSunShadow, data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceCastsSunShadow);
		}
		if (data->dpvs.surfaceCastsSunShadowOpt)
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceCastsSunShadowOpt, data->dpvs.sunShadowOptCount * data->dpvs.sunSurfVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceCastsSunShadowOpt);
		}

		if (data->dpvs.constantBuffersLit)
		{
			buf->align(7);
			auto destvoid = buf->write(data->dpvs.constantBuffersLit, data->dpvs.smodelCount);

			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				if (data->dpvs.constantBuffersLit[i])
				{
					destvoid[i] = buf->write_s(0, data->dpvs.constantBuffersLit[i], 1);
				}
			}

			buf->clear_pointer(&dest->dpvs.constantBuffersLit);
		}

		if (data->dpvs.constantBuffersAmbient)
		{
			buf->align(7);
			auto destvoid = buf->write(data->dpvs.constantBuffersAmbient, data->dpvs.smodelCount);

			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				if (data->dpvs.constantBuffersAmbient[i])
				{
					destvoid[i] = buf->write_s(0, data->dpvs.constantBuffersAmbient[i], 1);
				}
			}

			buf->clear_pointer(&dest->dpvs.constantBuffersAmbient);
		}
		buf->pop_stream();

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvsDyn.dynEntCellBits[0])
		{
			buf->align(3);
			buf->write(data->dpvsDyn.dynEntCellBits[0],
				data->dpvsDyn.dynEntClientWordCount[0] * data->dpvsPlanes.cellCount);
			buf->clear_pointer(&dest->dpvsDyn.dynEntCellBits[0]);
		}

		if (data->dpvsDyn.dynEntCellBits[1])
		{
			buf->align(3);
			buf->write(data->dpvsDyn.dynEntCellBits[1],
				data->dpvsDyn.dynEntClientWordCount[1] * data->dpvsPlanes.cellCount);
			buf->clear_pointer(&dest->dpvsDyn.dynEntCellBits[1]);
		}

		if (data->dpvsDyn.dynEntVisData[0][0])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[0][0], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[0][0]);
		}

		if (data->dpvsDyn.dynEntVisData[1][0])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[1][0], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[1][0]);
		}

		if (data->dpvsDyn.dynEntVisData[0][1])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[0][1], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[0][1]);
		}

		if (data->dpvsDyn.dynEntVisData[1][1])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[1][1], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[1][1]);
		}

		if (data->dpvsDyn.dynEntVisData[0][2])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[0][2], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[0][2]);
		}

		if (data->dpvsDyn.dynEntVisData[1][2])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[1][2], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[1][2]);
		}
		buf->pop_stream();

		if (data->heroOnlyLights)
		{
			buf->align(3);
			buf->write(data->heroOnlyLights, data->heroOnlyLightCount);
			buf->clear_pointer(&dest->heroOnlyLights);
		}

		if (data->umbraTomeData)
		{
			buf->align(15);
			buf->write(data->umbraTomeData, data->umbraTomeSize);
			buf->clear_pointer(&dest->umbraTomeData);
		}
		dest->umbraTomePtr = nullptr;

		buf->pop_stream();
	}

	void gfx_world::dump(GfxWorld* asset)
	{
		const auto path = asset->name + ".gfxmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_string(asset->baseName);

		write.dump_array(asset->skies, asset->skyCount);
		for (int i = 0; i < asset->skyCount; i++)
		{
			write.dump_array(asset->skies[i].skyStartSurfs, asset->skies[i].skySurfCount);
			write.dump_asset(asset->skies[i].skyImage);
		}

		write.dump_array(asset->dpvsPlanes.planes, asset->planeCount);
		write.dump_array(asset->dpvsPlanes.nodes, asset->nodeCount);
		write.dump_array(asset->dpvsPlanes.sceneEntCellBits, asset->dpvsPlanes.cellCount << 9);

		write.dump_array(asset->aabbTreeCounts, asset->dpvsPlanes.cellCount);
		write.dump_array(asset->aabbTrees, asset->dpvsPlanes.cellCount);
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			write.dump_array(asset->aabbTrees[i].aabbTree, asset->aabbTreeCounts[i].aabbTreeCount);
			for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
			{
				write.dump_array(asset->aabbTrees[i].aabbTree[j].smodelIndexes,
					asset->aabbTrees[i].aabbTree[j].smodelIndexCount);
			}
		}

		write.dump_array(asset->cells, asset->dpvsPlanes.cellCount);
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			write.dump_array(asset->cells[i].portals, asset->cells[i].portalCount);
			for (int j = 0; j < asset->cells[i].portalCount; j++)
			{
				write.dump_array(asset->cells[i].portals[j].vertices, asset->cells[i].portals[j].vertexCount);
			}
			write.dump_array(asset->cells[i].reflectionProbes, asset->cells[i].reflectionProbeCount);
			write.dump_array(asset->cells[i].reflectionProbeReferences, asset->cells[i].reflectionProbeReferenceCount);
		}

		write.dump_array(asset->draw.reflectionProbes, asset->draw.reflectionProbeCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			write.dump_asset(asset->draw.reflectionProbes[i]);
		}

		write.dump_array(asset->draw.reflectionProbeOrigins, asset->draw.reflectionProbeCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			write.dump_array(asset->draw.reflectionProbeOrigins[i].probeVolumes, asset->draw.reflectionProbeOrigins[i].probeVolumeCount);
		}

		write.dump_array(asset->draw.reflectionProbeTextures, asset->draw.reflectionProbeCount);

		write.dump_array(asset->draw.reflectionProbeReferenceOrigins, asset->draw.reflectionProbeReferenceCount);
		write.dump_array(asset->draw.reflectionProbeReferences, asset->draw.reflectionProbeReferenceCount);

		write.dump_array(asset->draw.lightmaps, asset->draw.lightmapCount);
		for (int i = 0; i < asset->draw.lightmapCount; i++)
		{
			write.dump_asset(asset->draw.lightmaps[i].primary);
			write.dump_asset(asset->draw.lightmaps[i].secondary);
		}
		write.dump_array(asset->draw.lightmapPrimaryTextures, asset->draw.lightmapCount);
		write.dump_array(asset->draw.lightmapSecondaryTextures, asset->draw.lightmapCount);
		write.dump_asset(asset->draw.lightmapOverridePrimary);
		write.dump_asset(asset->draw.lightmapOverrideSecondary);

		write.dump_array(asset->draw.vd.vertices, asset->draw.vertexCount);
		write.dump_array(asset->draw.vld.data, asset->draw.vertexLayerDataSize);
		write.dump_array(asset->draw.indices, asset->draw.indexCount);

		write.dump_array(asset->lightGrid.rowDataStart,
			(asset->lightGrid.maxs[asset->lightGrid.rowAxis] - asset->lightGrid.mins[asset->lightGrid.rowAxis] + 1));
		write.dump_array(asset->lightGrid.rawRowData, asset->lightGrid.rawRowDataSize);
		write.dump_array(asset->lightGrid.entries, asset->lightGrid.entryCount);
		write.dump_array(asset->lightGrid.colors, asset->lightGrid.colorCount);

		write.dump_array(asset->lightGrid.stageLightingContrastGain, asset->lightGrid.stageCount);
		write.dump_array(asset->lightGrid.paletteEntryAddress, asset->lightGrid.paletteEntryCount);
		write.dump_array(asset->lightGrid.paletteBitstream, asset->lightGrid.paletteBitstreamSize);

		write.dump_array(asset->lightGrid.tree.p_nodeTable, asset->lightGrid.tree.nodeCount);
		write.dump_array(asset->lightGrid.tree.p_leafTable, asset->lightGrid.tree.leafTableSize);

		write.dump_array(asset->models, asset->modelCount);

		write.dump_array(asset->materialMemory, asset->materialMemoryCount);
		for (int i = 0; i < asset->materialMemoryCount; i++)
		{
			write.dump_asset(asset->materialMemory[i].material);
		}

		write.dump_asset(asset->sun.spriteMaterial);
		write.dump_asset(asset->sun.flareMaterial);

		write.dump_asset(asset->outdoorImage);

		write.dump_array(asset->cellCasterBits,
			asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5));
		write.dump_array(asset->cellHasSunLitSurfsBits, (asset->dpvsPlanes.cellCount + 31) >> 5);

		write.dump_array(asset->sceneDynModel, asset->dpvsDyn.dynEntClientCount[0]);
		write.dump_array(asset->sceneDynBrush, asset->dpvsDyn.dynEntClientCount[1]);

		write.dump_array(asset->primaryLightEntityShadowVis,
			((asset->primaryLightCount - asset->lastSunPrimaryLightIndex - 1) << 13));

		write.dump_array(asset->primaryLightDynEntShadowVis[0],
			asset->dpvsDyn.dynEntClientCount[0] * (asset->primaryLightCount - asset->lastSunPrimaryLightIndex - 1));
		write.dump_array(asset->primaryLightDynEntShadowVis[1],
			asset->dpvsDyn.dynEntClientCount[1] * (asset->primaryLightCount - asset->lastSunPrimaryLightIndex - 1));

		write.dump_array(asset->nonSunPrimaryLightForModelDynEnt, asset->dpvsDyn.dynEntClientCount[0]);

		write.dump_array(asset->shadowGeom, asset->primaryLightCount);
		if (asset->shadowGeom)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->shadowGeom[i].sortedSurfIndex, asset->shadowGeom[i].surfaceCount);
				write.dump_array(asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelCount);
			}
		}
		write.dump_array(asset->shadowGeomOptimized, asset->primaryLightCount);
		if (asset->shadowGeomOptimized)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->shadowGeomOptimized[i].sortedSurfIndex, asset->shadowGeomOptimized[i].surfaceCount);
				write.dump_array(asset->shadowGeomOptimized[i].smodelIndex, asset->shadowGeomOptimized[i].smodelCount);
			}
		}

		write.dump_array(asset->lightRegion, asset->primaryLightCount);
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			write.dump_array(asset->lightRegion[i].hulls, asset->lightRegion[i].hullCount);
			for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
			{
				write.dump_array(asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axisCount);
			}
		}

		write.dump_array(asset->dpvs.smodelVisData[0], asset->dpvs.smodelVisDataCount);
		write.dump_array(asset->dpvs.smodelVisData[1], asset->dpvs.smodelVisDataCount);
		write.dump_array(asset->dpvs.smodelVisData[2], asset->dpvs.smodelVisDataCount);

		write.dump_array(asset->dpvs.surfaceVisData[0], asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.surfaceVisData[1], asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.surfaceVisData[2], asset->dpvs.surfaceVisDataCount);

		write.dump_array(asset->dpvs.unknownData01[0], asset->dpvs.smodelVisDataCount + 1);
		write.dump_array(asset->dpvs.unknownData01[1], asset->dpvs.smodelVisDataCount + 1);
		write.dump_array(asset->dpvs.unknownData01[2], asset->dpvs.smodelVisDataCount + 1);

		write.dump_array(asset->dpvs.unknownData02[0], asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.unknownData02[1], asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.unknownData02[2], asset->dpvs.surfaceVisDataCount);

		write.dump_array(asset->dpvs.lodData, asset->dpvs.smodelCount + 1);

		write.dump_array(asset->dpvs.tessellationCutoffVisData[0], asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.tessellationCutoffVisData[1], asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.tessellationCutoffVisData[2], asset->dpvs.surfaceVisDataCount);

		write.dump_array(asset->dpvs.sortedSurfIndex, asset->dpvs.staticSurfaceCount);

		write.dump_array(asset->dpvs.smodelInsts, asset->dpvs.smodelCount);
		write.dump_array(asset->dpvs.surfaces, asset->surfaceCount);
		for (unsigned int i = 0; i < asset->surfaceCount; i++)
		{
			write.dump_asset(asset->dpvs.surfaces[i].material);
		}
		write.dump_array(asset->dpvs.surfacesBounds, asset->surfaceCount);
		write.dump_array(asset->dpvs.smodelDrawInsts, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_asset(asset->dpvs.smodelDrawInsts[i].model);
			write.dump_array(asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues,
				asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.numLightingValues);
		}

		write.dump_array(asset->dpvs.surfaceMaterials, asset->surfaceCount);
		write.dump_array(asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.surfaceCastsSunShadowOpt, asset->dpvs.sunShadowOptCount * asset->dpvs.sunSurfVisDataCount);

		write.dump_array(asset->dpvs.constantBuffersLit, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_raw(asset->dpvs.constantBuffersLit[i], 1);
		}
		write.dump_array(asset->dpvs.constantBuffersAmbient, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_raw(asset->dpvs.constantBuffersAmbient[i], 1);
		}

		write.dump_array(asset->dpvsDyn.dynEntCellBits[0],
			asset->dpvsDyn.dynEntClientWordCount[0] * asset->dpvsPlanes.cellCount);
		write.dump_array(asset->dpvsDyn.dynEntCellBits[1],
			asset->dpvsDyn.dynEntClientWordCount[1] * asset->dpvsPlanes.cellCount);

		write.dump_array(asset->dpvsDyn.dynEntVisData[0][0], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][0], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[0][1], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][1], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[0][2], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][2], asset->dpvsDyn.dynEntClientWordCount[1] * 32);

		write.dump_array(asset->heroOnlyLights, asset->heroOnlyLightCount);

		write.dump_array(asset->umbraTomeData, asset->umbraTomeSize);

		write.close();
	}
}