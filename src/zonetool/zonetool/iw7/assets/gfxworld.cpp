#include <std_include.hpp>
#include "gfxworld.hpp"

namespace zonetool::iw7
{
	GfxWorld* gfx_world::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".gfxmap"s;

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
		asset->dpvsPlanes.sceneEntCellBits = mem->allocate<unsigned int>(asset->dpvsPlanes.cellCount << 9);

		asset->cellTransientInfos = read.read_array<GfxCellTransientInfo>();

		asset->cells = read.read_array<GfxCell>();
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			asset->cells[i].portals = read.read_array<GfxPortal>();
			for (int j = 0; j < asset->cells[i].portalCount; j++)
			{
				asset->cells[i].portals[j].vertices = read.read_array<float[3]>();
			}
		}

		asset->draw.reflectionProbeData.reflectionProbes = read.read_array<GfxReflectionProbe>();
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeCount; i++)
		{
			asset->draw.reflectionProbeData.reflectionProbes[i].livePath = read.read_string();
			asset->draw.reflectionProbeData.reflectionProbes[i].probeInstances = read.read_array<unsigned int>();
		}
		
		asset->draw.reflectionProbeData.reflectionProbeArrayImage = read.read_asset<GfxImage>();
		asset->draw.reflectionProbeData.probeRelightingData = read.read_array<GfxReflectionProbeRelightingData>();
		asset->draw.reflectionProbeData.reflectionProbeGBufferImages = mem->allocate<GfxImage*>(asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount);
		asset->draw.reflectionProbeData.reflectionProbeGBufferImages = read.read_array<GfxImage*>();
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount; i++)
		{
			asset->draw.reflectionProbeData.reflectionProbeGBufferImages[i] = read.read_asset<GfxImage>();
		}
		asset->draw.reflectionProbeData.reflectionProbeGBufferTextures = mem->allocate<GfxRawTexture>(asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount);
		asset->draw.reflectionProbeData.reflectionProbeInstances = read.read_array<GfxReflectionProbeInstance>();
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeInstanceCount; i++)
		{
			asset->draw.reflectionProbeData.reflectionProbeInstances[i].livePath = read.read_string();
			asset->draw.reflectionProbeData.reflectionProbeInstances[i].livePath2 = read.read_string();
		}
		asset->draw.reflectionProbeData.reflectionProbeLightgridSampleData = read.read_array<GfxReflectionProbeSampleData>();
		asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBuffer = nullptr;
		asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBufferView = nullptr;
		asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBufferRWView = nullptr;

		asset->draw.lightmapReindexData.reindexElement = read.read_array<GfxWorldLightmapReindexDataElement>();
		asset->draw.lightmapReindexData.packedLightmap = read.read_array<GfxWorldPackedLightmap>();

		asset->draw.iesLookupTexture = read.read_asset<GfxImage>();
		asset->draw.decalVolumeCollections = read.read_array<GfxDecalVolumeCollection>();
		asset->draw.lightmapOverridePrimary = read.read_asset<GfxImage>();
		asset->draw.lightmapOverrideSecondary = read.read_asset<GfxImage>();
		asset->draw.lightMaps = read.read_array<GfxLightMap*>();
		for (unsigned int i = 0; i < asset->draw.lightMapCount; i++)
		{
			asset->draw.lightMaps[i] = read.read_asset<GfxLightMap>();
		}
		asset->draw.lightmapTextures = mem->allocate<GfxTexture>(asset->draw.lightMapCount);

		asset->draw.unused1 = nullptr;
		asset->draw.unused2 = nullptr;
		asset->draw.unused3 = nullptr;

		for (auto i = 0; i < 32; i++)
		{
			asset->draw.transientZones[i] = read.read_asset<GfxWorldTransientZone>();
		}

		asset->draw.indices = read.read_array<unsigned short>();
		asset->draw.indexBuffer = nullptr;
		
		asset->draw.volumetrics.volumetrics = read.read_array<GfxVolumetric>();
		for (unsigned int i = 0; i < asset->draw.volumetrics.volumetricCount; i++)
		{
			asset->draw.volumetrics.volumetrics[i].livePath = read.read_string();
			for (auto m = 0; m < 4; m++)
			{
				asset->draw.volumetrics.volumetrics[i].masks[m].image = read.read_asset<GfxImage>();
			}
		}

		asset->lightGrid.stageLightingContrastGain = read.read_array<float>();
		asset->lightGrid.paletteEntryAddress = read.read_array<int>();
		asset->lightGrid.paletteBitstream = read.read_array<unsigned char>();

		asset->lightGrid.tree.p_nodeTable = read.read_array<unsigned int>();
		asset->lightGrid.tree.p_leafTable = read.read_array<unsigned char>();

		asset->lightGrid.probeData.gpuVisibleProbePositions = read.read_array<GfxGpuLightGridProbePosition>();
		asset->lightGrid.probeData.gpuVisibleProbesData = read.read_array<GfxProbeData>();
		asset->lightGrid.probeData.gpuVisibleProbesBuffer = nullptr;
		asset->lightGrid.probeData.gpuVisibleProbesView = nullptr;
		asset->lightGrid.probeData.gpuVisibleProbesRWView = nullptr;

		asset->lightGrid.probeData.probes = read.read_array<GfxProbeData>();
		asset->lightGrid.probeData.probesBuffer = nullptr;
		asset->lightGrid.probeData.probesView = nullptr;
		asset->lightGrid.probeData.probesRWView = nullptr;

		asset->lightGrid.probeData.probePositions = read.read_array<GfxGpuLightGridProbePosition>();
		asset->lightGrid.probeData.probePositionsBuffer = nullptr;
		asset->lightGrid.probeData.probePositionsView = nullptr;

		asset->lightGrid.probeData.zones = read.read_array<GfxGpuLightGridZone>();

		asset->lightGrid.probeData.tetrahedrons = read.read_array<GfxGpuLightGridTetrahedron>();
		asset->lightGrid.probeData.tetrahedronBuffer = nullptr;
		asset->lightGrid.probeData.tetrahedronView = nullptr;

		asset->lightGrid.probeData.tetrahedronNeighbors = read.read_array<GfxGpuLightGridTetrahedronNeighbors>();
		asset->lightGrid.probeData.tetrahedronNeighborsBuffer = nullptr;
		asset->lightGrid.probeData.tetrahedronNeighborsView = nullptr;

		asset->lightGrid.probeData.tetrahedronVisibility = read.read_array<GfxGpuLightGridTetrahedronVisibility>();
		asset->lightGrid.probeData.tetrahedronVisibilityBuffer = nullptr;
		asset->lightGrid.probeData.tetrahedronVisibilityView = nullptr;

		asset->lightGrid.probeData.voxelStartTetrahedron = read.read_array<GfxGpuLightGridVoxelStartTetrahedron>();
		asset->lightGrid.probeData.voxelStartTetrahedronBuffer = nullptr;
		asset->lightGrid.probeData.voxelStartTetrahedronView = nullptr;

		asset->frustumLights = read.read_array<GfxFrustumLights>();
		if (asset->frustumLights)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->frustumLights[i].indices = read.read_array<unsigned short>();
				asset->frustumLights[i].vertices = read.read_array<char>();
				asset->frustumLights[i].indexBuffer = nullptr;
				asset->frustumLights[i].vertexBuffer = nullptr;
			}
		}

		asset->lightViewFrustums = read.read_array<GfxLightViewFrustum>();
		if (asset->lightViewFrustums)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->lightViewFrustums[i].planes = read.read_array<vec4_t>();
				asset->lightViewFrustums[i].indices = read.read_array<unsigned short>();
				asset->lightViewFrustums[i].vertices = read.read_array<vec3_t>();
			}
		}

		asset->voxelTree = read.read_array<GfxVoxelTree>();
		for (int i = 0; i < asset->voxelTreeCount; i++)
		{
			asset->voxelTree[i].voxelTreeHeader = read.read_single<GfxVoxelTreeHeader>();
			asset->voxelTree[i].voxelTopDownViewNodeArray = read.read_array<GfxVoxelTopDownViewNode>();
			asset->voxelTree[i].voxelInternalNodeArray = read.read_array<GfxVoxelInternalNode>();
			asset->voxelTree[i].voxelLeafNodeArray = read.read_array<GfxVoxelLeafNode>();
			asset->voxelTree[i].lightListArray = read.read_array<unsigned short>();
			asset->voxelTree[i].voxelInternalNodeDynamicLightList = mem->allocate<unsigned int>(2 * asset->voxelTree[i].voxelInternalNodeCount);
		}

		asset->heightfields = read.read_array<GfxHeightfield>();
		for (int i = 0; i < asset->heightfieldCount; i++)
		{
			asset->heightfields[i].image = read.read_asset<GfxImage>();
		}

		asset->unk01.unk01 = read.read_array<unsigned int>();
		asset->unk01.unk02 = read.read_array<unsigned int>();
		asset->unk01.unk03 = read.read_array<unsigned short>();

		asset->models = read.read_array<GfxBrushModel>();

		asset->materialMemory = read.read_array<MaterialMemory>();
		for (int i = 0; i < asset->materialMemoryCount; i++)
		{
			asset->materialMemory[i].material = read.read_asset<Material>();
		}

		asset->sun.spriteMaterial = read.read_asset<Material>();
		asset->sun.flareMaterial = read.read_asset<Material>();

		asset->outdoorImage = read.read_asset<GfxImage>();
		asset->dustMaterial = read.read_asset<Material>();

		asset->cellCasterBits = mem->allocate<unsigned int>(asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5));
		asset->cellHasSunLitSurfsBits = mem->allocate<unsigned int>((asset->dpvsPlanes.cellCount + 31) >> 5);

		asset->sceneDynModel = mem->allocate<GfxSceneDynModel>(asset->dpvsDyn.dynEntClientCount[0]);
		asset->sceneDynBrush = mem->allocate<GfxSceneDynBrush>(asset->dpvsDyn.dynEntClientCount[1]);

		asset->primaryLightMotionDetectBits = mem->allocate<unsigned int>(asset->primaryLightMotionDetectBitsEntries);
		asset->entityMotionBits = mem->allocate<unsigned int>(asset->entityMotionBitsEntries);

		asset->primaryLightEntityShadowVis = mem->allocate<unsigned int>(asset->numPrimaryLightEntityShadowVisEntries);

		asset->dynEntMotionBits[0] = mem->allocate<unsigned int>(asset->dynEntMotionBitsEntries[0]);
		asset->dynEntMotionBits[1] = mem->allocate<unsigned int>(asset->dynEntMotionBitsEntries[1]);

		asset->primaryLightDynEntShadowVis[0] = mem->allocate<unsigned int>(asset->numPrimaryLightDynEntShadowVisEntries[0]);
		asset->primaryLightDynEntShadowVis[1] = mem->allocate<unsigned int>(asset->numPrimaryLightDynEntShadowVisEntries[1]);

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

		asset->lightAABB.nodeArray = read.read_array<GfxLightAABBNode>();
		asset->lightAABB.lightArray = read.read_array<unsigned short>();

		for (auto i = 0; i < 30; i++)
		{
			if (i >= 3 && i <= 11)
			{
				asset->dpvs.smodelVisData[i] = nullptr;
				continue;
			}

			asset->dpvs.smodelVisData[i] = mem->allocate<unsigned int>(asset->dpvs.smodelVisDataCount);
		}

		for (auto i = 0; i < 30; i++)
		{
			if (i >= 3 && i <= 11)
			{
				asset->dpvs.surfaceVisData[i] = nullptr;
				continue;
			}

			asset->dpvs.surfaceVisData[i] = mem->allocate<unsigned int>(asset->dpvs.surfaceVisDataCount);
		}

		asset->dpvs.primaryLightVisData[0] = mem->allocate<unsigned int>(asset->dpvs.primaryLightVisDataCount);
		asset->dpvs.reflectionProbeVisData[0] = mem->allocate<unsigned int>(asset->dpvs.reflectionProbeVisDataCount);
		asset->dpvs.volumetricVisData[0] = mem->allocate<unsigned int>(asset->dpvs.volumetricVisDataCount);
		asset->dpvs.decalVisData[0] = mem->allocate<unsigned int>(asset->dpvs.decalVisDataCount);

		asset->dpvs.lodData = read.read_array<unsigned int>();

		for (auto i = 0; i < 30; i++)
		{
			if (i >= 1)
			{
				asset->dpvs.tessellationCutoffVisData[i] = nullptr;
				continue;
			}

			asset->dpvs.tessellationCutoffVisData[i] = mem->allocate<unsigned int>(asset->dpvs.surfaceVisDataCount);
		}

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

		asset->dpvs.surfaceMaterials = mem->allocate<GfxDrawSurf>(asset->surfaceCount);

		asset->dpvs.surfaceCastsSunShadow = read.read_array<unsigned int>();
		asset->dpvs.surfaceCastsSunShadowOpt = mem->allocate<unsigned int>(asset->dpvs.sunShadowOptCount * asset->dpvs.sunSurfVisDataCount);

		asset->dpvs.sortedSmodelIndices = read.read_array<unsigned short>();

		asset->dpvs.constantBuffers = mem->allocate<char*>(asset->dpvs.smodelCount);

		asset->dpvsDyn.dynEntCellBits[0] = mem->allocate<unsigned int>(asset->dpvsDyn.dynEntClientWordCount[0] * asset->dpvsPlanes.cellCount);
		asset->dpvsDyn.dynEntCellBits[1] = mem->allocate<unsigned int>(asset->dpvsDyn.dynEntClientWordCount[1] * asset->dpvsPlanes.cellCount);

		for (auto a = 0; a < 2; a++)
		{
			for (auto i = 0; i < 30; i++)
			{
				if (i >= 3)
				{
					continue;
				}

				asset->dpvsDyn.dynEntVisData[a][i] = mem->allocate<unsigned char>(32 * asset->dpvsDyn.dynEntClientWordCount[a]);
			}
		}

		asset->heroOnlyLights = read.read_array<GfxHeroOnlyLight>();

		asset->umbraGates = read.read_array<UmbraGate>();
		asset->umbraTomeData = read.read_array<char>();
		
		asset->umbraGates2 = read.read_array<UmbraGate2>();
		asset->umbraTomeData2 = read.read_array<char>();

		asset->umbraUnkData = read.read_array<char>();

		read.close();

		return asset;
	}

	void gfx_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
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
		if (data->draw.reflectionProbeData.reflectionProbeArrayImage)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.reflectionProbeData.reflectionProbeArrayImage->name);
		}
		for (unsigned int i = 0; i < data->draw.reflectionProbeData.reflectionProbeGBufferImageCount; i++)
		{
			if (data->draw.reflectionProbeData.reflectionProbeGBufferImages[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.reflectionProbeData.reflectionProbeGBufferImages[i]->name);
			}
		}

		// IES
		if (data->draw.iesLookupTexture)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.iesLookupTexture->name);
		}

		// Lightmaps
		if (data->draw.lightmapOverridePrimary)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmapOverridePrimary->name);
		}
		if (data->draw.lightmapOverrideSecondary)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmapOverrideSecondary->name);
		}
		if (data->draw.lightMaps)
		{
			for (unsigned int i = 0; i < data->draw.lightMapCount; i++)
			{
				if (data->draw.lightMaps[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_GFXLIGHTMAP, data->draw.lightMaps[i]->name);
				}
			}
		}

		// TransientZones
		for (auto i = 0; i < 32; i++)
		{
			if (data->draw.transientZones[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, data->draw.transientZones[i]->name);
			}
		}

		// Volumetrics
		for (unsigned int i = 0; i < data->draw.volumetrics.volumetricCount; i++)
		{
			for (auto m = 0; m < 4; m++)
			{
				if (data->draw.volumetrics.volumetrics[i].masks[m].image)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.volumetrics.volumetrics[i].masks[m].image->name);
				}
			}
		}

		// HeightField
		for (int i = 0; i < data->heightfieldCount; i++)
		{
			if (data->heightfields[i].image)
			{
				zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->heightfields[i].image->name);
			}
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

		// Sunflare
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

		// DustMaterial
		if (data->dustMaterial)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->dustMaterial->name);
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
			buf->align(7);
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

		if (data->cellTransientInfos)
		{
			buf->align(127);
			buf->write(data->cellTransientInfos, data->dpvsPlanes.cellCount);
			buf->clear_pointer(&dest->cellTransientInfos);
		}

		if (data->cells)
		{
			buf->align(7);
			auto gfx_cell = buf->write(data->cells, data->dpvsPlanes.cellCount);

			for (int i = 0; i < data->dpvsPlanes.cellCount; i++)
			{
				if (data->cells[i].portals)
				{
					buf->align(7);
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

				buf->clear_pointer(&dest->cells);
			}
		}

		if (data->draw.reflectionProbeData.reflectionProbes)
		{
			buf->align(7);
			dest->draw.reflectionProbeData.reflectionProbes = buf->write(data->draw.reflectionProbeData.reflectionProbes, data->draw.reflectionProbeData.reflectionProbeCount);
			for (unsigned int i = 0; i < data->draw.reflectionProbeData.reflectionProbeCount; i++)
			{
				if (data->draw.reflectionProbeData.reflectionProbes[i].livePath)
				{
					dest->draw.reflectionProbeData.reflectionProbes[i].livePath = buf->write_str(data->draw.reflectionProbeData.reflectionProbes[i].livePath);
				}

				if (data->draw.reflectionProbeData.reflectionProbes[i].probeInstances)
				{
					buf->align(3);
					buf->write(data->draw.reflectionProbeData.reflectionProbes[i].probeInstances, data->draw.reflectionProbeData.reflectionProbes[i].probeInstanceCount);
					buf->clear_pointer(&dest->draw.reflectionProbeData.reflectionProbes[i].probeInstances);
				}
			}
			buf->clear_pointer(&dest->draw.reflectionProbeData.reflectionProbes);
		}

		if (data->draw.reflectionProbeData.reflectionProbeArrayImage)
		{
			dest->draw.reflectionProbeData.reflectionProbeArrayImage = reinterpret_cast<GfxImage*>(
				zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->draw.reflectionProbeData.reflectionProbeArrayImage->name));
		}

		if (data->draw.reflectionProbeData.probeRelightingData)
		{
			buf->align(3);
			buf->write(data->draw.reflectionProbeData.probeRelightingData, data->draw.reflectionProbeData.probeRelightingCount);
			buf->clear_pointer(&dest->draw.reflectionProbeData.probeRelightingData);
		}

		if (data->draw.reflectionProbeData.reflectionProbeGBufferImages)
		{
			buf->align(7);
			dest->draw.reflectionProbeData.reflectionProbeGBufferImages = buf->write(
				data->draw.reflectionProbeData.reflectionProbeGBufferImages, data->draw.reflectionProbeData.reflectionProbeGBufferImageCount);
			for (unsigned int i = 0; i < data->draw.reflectionProbeData.reflectionProbeGBufferImageCount; i++)
			{
				dest->draw.reflectionProbeData.reflectionProbeGBufferImages[i] = reinterpret_cast<GfxImage*>(
					zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->draw.reflectionProbeData.reflectionProbeGBufferImages[i]->name));
			}
			buf->clear_pointer(&dest->draw.reflectionProbeData.reflectionProbeGBufferImages);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->draw.reflectionProbeData.reflectionProbeGBufferTextures)
		{
			buf->align(3);
			buf->write(data->draw.reflectionProbeData.reflectionProbeGBufferTextures, data->draw.reflectionProbeData.reflectionProbeGBufferImageCount);
			buf->clear_pointer(&dest->draw.reflectionProbeData.reflectionProbeGBufferTextures);
		}
		buf->pop_stream();

		if (data->draw.reflectionProbeData.reflectionProbeInstances)
		{
			buf->align(7);
			dest->draw.reflectionProbeData.reflectionProbeInstances = buf->write(data->draw.reflectionProbeData.reflectionProbeInstances, data->draw.reflectionProbeData.reflectionProbeInstanceCount);
			for (unsigned int i = 0; i < data->draw.reflectionProbeData.reflectionProbeInstanceCount; i++)
			{
				if (data->draw.reflectionProbeData.reflectionProbeInstances[i].livePath)
				{
					dest->draw.reflectionProbeData.reflectionProbeInstances[i].livePath = buf->write_str(data->draw.reflectionProbeData.reflectionProbeInstances[i].livePath);
				}
				if (data->draw.reflectionProbeData.reflectionProbeInstances[i].livePath2)
				{
					dest->draw.reflectionProbeData.reflectionProbeInstances[i].livePath2 = buf->write_str(data->draw.reflectionProbeData.reflectionProbeInstances[i].livePath2);
				}
			}
			buf->clear_pointer(&dest->draw.reflectionProbeData.reflectionProbeInstances);
		}

		if (data->draw.reflectionProbeData.reflectionProbeLightgridSampleData)
		{
			buf->align(63);
			buf->write(data->draw.reflectionProbeData.reflectionProbeLightgridSampleData, data->draw.reflectionProbeData.reflectionProbeCount);
			buf->clear_pointer(&dest->draw.reflectionProbeData.reflectionProbeLightgridSampleData);
		}

		if (data->draw.lightmapReindexData.reindexElement)
		{
			buf->align(3);
			buf->write(data->draw.lightmapReindexData.reindexElement, data->draw.lightmapReindexData.reindexCount);
			buf->clear_pointer(&dest->draw.lightmapReindexData.reindexElement);
		}

		if (data->draw.lightmapReindexData.packedLightmap)
		{
			buf->align(3);
			buf->write(data->draw.lightmapReindexData.packedLightmap, data->draw.lightmapReindexData.packedLightmapCount);
			buf->clear_pointer(&dest->draw.lightmapReindexData.packedLightmap);
		}

		if (data->draw.iesLookupTexture)
		{
			dest->draw.iesLookupTexture = reinterpret_cast<GfxImage*>(
				zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->draw.iesLookupTexture->name));
		}

		if (data->draw.decalVolumeCollections)
		{
			buf->align(3);
			buf->write(data->draw.decalVolumeCollections, data->draw.decalVolumeCollectionCount);
			buf->clear_pointer(&dest->draw.decalVolumeCollections);
		}

		if (data->draw.lightmapOverridePrimary)
		{
			dest->draw.lightmapOverridePrimary = reinterpret_cast<GfxImage*>(
				zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->draw.lightmapOverridePrimary->name));
		}

		if (data->draw.lightmapOverrideSecondary)
		{
			dest->draw.lightmapOverrideSecondary = reinterpret_cast<GfxImage*>(
				zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->draw.lightmapOverrideSecondary->name));
		}

		if (data->draw.lightMaps)
		{
			buf->align(7);
			auto destlightmaps = buf->write(data->draw.lightMaps, data->draw.lightMapCount);

			for (unsigned int i = 0; i < data->draw.lightMapCount; i++)
			{
				if (data->draw.lightMaps[i])
				{
					destlightmaps[i] = reinterpret_cast<GfxLightMap*>(zone->get_asset_pointer(
						ASSET_TYPE_GFXLIGHTMAP, data->draw.lightMaps[i]->name));
				}
			}

			buf->clear_pointer(&dest->draw.lightMaps);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->draw.lightmapTextures)
		{
			buf->align(3);
			buf->write(data->draw.lightmapTextures, data->draw.lightMapCount);
			buf->clear_pointer(&dest->draw.lightmapTextures);
		}
		buf->pop_stream();

		for (auto i = 0; i < 32; i++)
		{
			if (data->draw.transientZones[i])
			{
				dest->draw.transientZones[i] = reinterpret_cast<GfxWorldTransientZone*>(
					zone->get_asset_pointer(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, data->draw.transientZones[i]->name));
			}
		}

		if (data->draw.indices)
		{
			buf->align(3);
			buf->write(data->draw.indices, data->draw.indexCount);
			buf->clear_pointer(&dest->draw.indices);
		}

		if (data->draw.volumetrics.volumetrics)
		{
			buf->align(7);
			auto* dest_volumetrics = buf->write(data->draw.volumetrics.volumetrics, data->draw.volumetrics.volumetricCount);
			for (unsigned int i = 0; i < data->draw.volumetrics.volumetricCount; i++)
			{
				if (data->draw.volumetrics.volumetrics[i].livePath)
				{
					dest_volumetrics[i].livePath = buf->write_str(data->draw.volumetrics.volumetrics[i].livePath);
				}

				for (auto m = 0; m < 4; m++)
				{
					if (data->draw.volumetrics.volumetrics[i].masks[m].image)
					{
						dest_volumetrics[i].masks[m].image = reinterpret_cast<GfxImage*>(
							zone->get_asset_pointer(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, data->draw.volumetrics.volumetrics[i].masks[m].image->name));
					}
				}
			}
			buf->clear_pointer(&dest->draw.volumetrics.volumetrics);
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

		if (data->lightGrid.probeData.gpuVisibleProbePositions)
		{
			buf->align(3);
			buf->write(data->lightGrid.probeData.gpuVisibleProbePositions, data->lightGrid.probeData.gpuVisibleProbesCount);
			buf->clear_pointer(&dest->lightGrid.probeData.gpuVisibleProbePositions);
		}

		if (data->lightGrid.probeData.gpuVisibleProbesData)
		{
			buf->align(63);
			buf->write_stream(data->lightGrid.probeData.gpuVisibleProbesData, (data->lightGrid.probeData.gpuVisibleProbesCount + 0x2000) * 64);
			buf->clear_pointer(&dest->lightGrid.probeData.gpuVisibleProbesData);
		}

		if (data->lightGrid.probeData.probes)
		{
			buf->align(63);
			buf->write_stream(data->lightGrid.probeData.probes, data->lightGrid.probeData.probeCount * 64);
			buf->clear_pointer(&dest->lightGrid.probeData.probes);
		}

		if (data->lightGrid.probeData.probePositions)
		{
			buf->align(3);
			buf->write(data->lightGrid.probeData.probePositions, data->lightGrid.probeData.probeCount);
			buf->clear_pointer(&dest->lightGrid.probeData.probePositions);
		}

		if (data->lightGrid.probeData.zones)
		{
			buf->align(63);
			buf->write(data->lightGrid.probeData.zones, data->lightGrid.probeData.zoneCount);
			buf->clear_pointer(&dest->lightGrid.probeData.zones);
		}

		if (data->lightGrid.probeData.tetrahedrons)
		{
			buf->align(63);
			buf->write(data->lightGrid.probeData.tetrahedrons, data->lightGrid.probeData.tetrahedronCount);
			buf->clear_pointer(&dest->lightGrid.probeData.tetrahedrons);
		}

		if (data->lightGrid.probeData.tetrahedronNeighbors)
		{
			buf->align(63);
			buf->write(data->lightGrid.probeData.tetrahedronNeighbors, data->lightGrid.probeData.tetrahedronCount);
			buf->clear_pointer(&dest->lightGrid.probeData.tetrahedronNeighbors);
		}

		if (data->lightGrid.probeData.tetrahedronVisibility)
		{
			buf->align(63);
			buf->write_stream(data->lightGrid.probeData.tetrahedronVisibility, data->lightGrid.probeData.tetrahedronCountVisible * 64);
			buf->clear_pointer(&dest->lightGrid.probeData.tetrahedronVisibility);
		}

		if (data->lightGrid.probeData.voxelStartTetrahedron)
		{
			buf->align(63);
			buf->write(data->lightGrid.probeData.voxelStartTetrahedron, data->lightGrid.probeData.voxelStartTetrahedronCount);
			buf->clear_pointer(&dest->lightGrid.probeData.voxelStartTetrahedron);
		}

		if (data->voxelTree)
		{
			buf->align(7);
			dest->voxelTree = buf->write(data->voxelTree, data->voxelTreeCount);
			for (int i = 0; i < data->voxelTreeCount; i++)
			{
				if (data->voxelTree[i].voxelTreeHeader)
				{
					buf->align(15);
					buf->write(data->voxelTree[i].voxelTreeHeader);
					buf->clear_pointer(&dest->voxelTree[i].voxelTreeHeader);
				}

				if (data->voxelTree[i].voxelTopDownViewNodeArray)
				{
					buf->align(3);
					buf->write(data->voxelTree[i].voxelTopDownViewNodeArray, data->voxelTree[i].voxelTopDownViewNodeCount);
					buf->clear_pointer(&dest->voxelTree[i].voxelTopDownViewNodeArray);
				}

				if (data->voxelTree[i].voxelInternalNodeArray)
				{
					buf->align(3);
					buf->write(data->voxelTree[i].voxelInternalNodeArray, data->voxelTree[i].voxelInternalNodeCount);
					buf->clear_pointer(&dest->voxelTree[i].voxelInternalNodeArray);
				}

				if (data->voxelTree[i].voxelLeafNodeArray)
				{
					buf->align(1);
					buf->write(data->voxelTree[i].voxelLeafNodeArray, data->voxelTree[i].voxelLeafNodeCount);
					buf->clear_pointer(&dest->voxelTree[i].voxelLeafNodeArray);
				}

				if (data->voxelTree[i].lightListArray)
				{
					buf->align(3);
					buf->write(data->voxelTree[i].lightListArray, data->voxelTree[i].lightListArraySize);
					buf->clear_pointer(&dest->voxelTree[i].lightListArray);
				}

				buf->push_stream(XFILE_BLOCK_RUNTIME);
				if (data->voxelTree[i].voxelInternalNodeDynamicLightList)
				{
					buf->align(3);
					buf->write(data->voxelTree[i].voxelInternalNodeDynamicLightList, 2 * data->voxelTree[i].voxelInternalNodeCount);
					buf->clear_pointer(&dest->voxelTree[i].voxelInternalNodeDynamicLightList);
				}
				buf->pop_stream();
			}
			buf->clear_pointer(&dest->voxelTree);
		}

		if (data->frustumLights)
		{
			buf->align(7);
			dest->frustumLights = buf->write(data->frustumLights, data->primaryLightCount);
			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->frustumLights[i].indices)
				{
					buf->align(3);
					buf->write(data->frustumLights[i].indices, data->frustumLights[i].indexCount);
					buf->clear_pointer(&dest->frustumLights[i].indices);
				}

				if (data->frustumLights[i].vertices)
				{
					buf->align(3);
					buf->write(data->frustumLights[i].vertices, 32 * data->frustumLights[i].vertexCount);
					buf->clear_pointer(&dest->frustumLights[i].vertices);
				}
			}
			buf->clear_pointer(&dest->frustumLights);
		}

		if (data->lightViewFrustums)
		{
			buf->align(7);
			dest->lightViewFrustums = buf->write(data->lightViewFrustums, data->primaryLightCount);
			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->lightViewFrustums[i].planes)
				{
					buf->align(3);
					buf->write(data->lightViewFrustums[i].planes, data->lightViewFrustums[i].planeCount);
					buf->clear_pointer(&dest->lightViewFrustums[i].planes);
				}

				if (data->lightViewFrustums[i].indices)
				{
					buf->align(1);
					buf->write(data->lightViewFrustums[i].indices, data->lightViewFrustums[i].indexCount);
					buf->clear_pointer(&dest->lightViewFrustums[i].indices);
				}

				if (data->lightViewFrustums[i].vertices)
				{
					buf->align(3);
					buf->write(data->lightViewFrustums[i].vertices, data->lightViewFrustums[i].vertexCount);
					buf->clear_pointer(&dest->lightViewFrustums[i].vertices);
				}
			}
			buf->clear_pointer(&dest->lightViewFrustums);
		}

		if (data->heightfields)
		{
			buf->align(7);
			dest->heightfields = buf->write(data->heightfields, data->heightfieldCount);
			for (int i = 0; i < data->heightfieldCount; i++)
			{
				if (data->heightfields[i].image)
				{
					dest->heightfields[i].image = reinterpret_cast<GfxImage*>(
						zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->heightfields[i].image->name));
				}
			}
			buf->clear_pointer(&dest->heightfields);
		}

		if (data->unk01.unk01)
		{
			buf->align(3);
			buf->write(data->unk01.unk01, data->unk01.unk01Count);
			buf->clear_pointer(&dest->unk01.unk01);
		}

		if (data->unk01.unk02)
		{
			buf->align(3);
			buf->write(data->unk01.unk02, data->unk01.unk02Count);
			buf->clear_pointer(&dest->unk01.unk02);
		}

		if (data->unk01.unk03)
		{
			buf->align(1);
			buf->write(data->unk01.unk03, data->unk01.unk03Count);
			buf->clear_pointer(&dest->unk01.unk03);
		}

		if (data->models)
		{
			buf->align(3);
			buf->write(data->models, data->modelCount);
			buf->clear_pointer(&dest->models);
		}

		if (data->materialMemory)
		{
			buf->align(7);
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

		if (data->dustMaterial)
		{
			dest->dustMaterial = reinterpret_cast<Material*>(
				zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->dustMaterial->name));
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

		if (data->primaryLightMotionDetectBits)
		{
			buf->align(3);
			buf->write(data->primaryLightMotionDetectBits, data->primaryLightMotionDetectBitsEntries);
			buf->clear_pointer(&dest->primaryLightMotionDetectBits);
		}

		if (data->entityMotionBits)
		{
			buf->align(3);
			buf->write(data->entityMotionBits, data->entityMotionBitsEntries);
			buf->clear_pointer(&dest->entityMotionBits);
		}

		if (data->primaryLightEntityShadowVis)
		{
			buf->align(3);
			buf->write(data->primaryLightEntityShadowVis, data->numPrimaryLightEntityShadowVisEntries);
			buf->clear_pointer(&dest->primaryLightEntityShadowVis);
		}

		if (data->dynEntMotionBits[0])
		{
			buf->align(3);
			buf->write(data->dynEntMotionBits[0], data->dynEntMotionBitsEntries[0]);
			buf->clear_pointer(&dest->dynEntMotionBits[0]);
		}

		if (data->dynEntMotionBits[1])
		{
			buf->align(3);
			buf->write(data->dynEntMotionBits[1], data->dynEntMotionBitsEntries[1]);
			buf->clear_pointer(&dest->dynEntMotionBits[1]);
		}

		if (data->primaryLightDynEntShadowVis[0])
		{
			buf->align(3);
			buf->write(data->primaryLightDynEntShadowVis[0], data->numPrimaryLightDynEntShadowVisEntries[0]);
			buf->clear_pointer(&dest->primaryLightDynEntShadowVis[0]);
		}

		if (data->primaryLightDynEntShadowVis[1])
		{
			buf->align(3);
			buf->write(data->primaryLightDynEntShadowVis[1], data->numPrimaryLightDynEntShadowVisEntries[1]);
			buf->clear_pointer(&dest->primaryLightDynEntShadowVis[1]);
		}
		buf->pop_stream();

		if (data->shadowGeomOptimized)
		{
			buf->align(7);
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

			buf->clear_pointer(&dest->shadowGeomOptimized);
		}

		if (data->lightRegion)
		{
			buf->align(7);
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

		if (data->lightAABB.nodeArray)
		{
			buf->align(3);
			buf->write(data->lightAABB.nodeArray, data->lightAABB.nodeCount);
			buf->clear_pointer(&dest->lightAABB.nodeArray);
		}
		if (data->lightAABB.lightArray)
		{
			buf->align(1);
			buf->write(data->lightAABB.lightArray, data->lightAABB.lightCount);
			buf->clear_pointer(&dest->lightAABB.lightArray);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);

#define WRITE_VISDATA(__INDEX__) \
		if (data->dpvs.smodelVisData[__INDEX__]) \
		{ \
			buf->align(127); \
			buf->write(data->dpvs.smodelVisData[__INDEX__], data->dpvs.smodelVisDataCount); \
			buf->clear_pointer(&dest->dpvs.smodelVisData[__INDEX__]); \
		}
#define WRITE_VISDATA_DISABLED(__INDEX__) \
		dest->dpvs.smodelVisData[__INDEX__] = nullptr;
		
		WRITE_VISDATA(0);
		WRITE_VISDATA(1);
		WRITE_VISDATA(2);
		WRITE_VISDATA_DISABLED(3);
		WRITE_VISDATA_DISABLED(4);
		WRITE_VISDATA_DISABLED(5);
		WRITE_VISDATA_DISABLED(6);
		WRITE_VISDATA_DISABLED(7);
		WRITE_VISDATA_DISABLED(8);
		WRITE_VISDATA_DISABLED(9);
		WRITE_VISDATA_DISABLED(10);
		WRITE_VISDATA_DISABLED(11);
		WRITE_VISDATA(12);
		WRITE_VISDATA(13);
		WRITE_VISDATA(14);
		WRITE_VISDATA(15);
		WRITE_VISDATA(16);
		WRITE_VISDATA(17);
		WRITE_VISDATA(18);
		WRITE_VISDATA(19);
		WRITE_VISDATA(20);
		WRITE_VISDATA(21);
		WRITE_VISDATA(22);
		WRITE_VISDATA(23);
		WRITE_VISDATA(24);
		WRITE_VISDATA(25);
		WRITE_VISDATA(26);
		WRITE_VISDATA(27);
		WRITE_VISDATA(28);
		WRITE_VISDATA(29);

#undef WRITE_VISDATA
#undef WRITE_VISDATA_DISABLED

#define WRITE_VISDATA(__INDEX__) \
		if (data->dpvs.surfaceVisData[__INDEX__]) \
		{ \
			buf->align(127); \
			buf->write(data->dpvs.surfaceVisData[__INDEX__], data->dpvs.surfaceVisDataCount); \
			buf->clear_pointer(&dest->dpvs.surfaceVisData[__INDEX__]); \
		}
#define WRITE_VISDATA_DISABLED(__INDEX__) \
		dest->dpvs.surfaceVisData[__INDEX__] = nullptr;

		WRITE_VISDATA(0);
		WRITE_VISDATA(1);
		WRITE_VISDATA(2);
		WRITE_VISDATA_DISABLED(3);
		WRITE_VISDATA_DISABLED(4);
		WRITE_VISDATA_DISABLED(5);
		WRITE_VISDATA_DISABLED(6);
		WRITE_VISDATA_DISABLED(7);
		WRITE_VISDATA_DISABLED(8);
		WRITE_VISDATA_DISABLED(9);
		WRITE_VISDATA_DISABLED(10);
		WRITE_VISDATA_DISABLED(11);
		WRITE_VISDATA(12);
		WRITE_VISDATA(13);
		WRITE_VISDATA(14);
		WRITE_VISDATA(15);
		WRITE_VISDATA(16);
		WRITE_VISDATA(17);
		WRITE_VISDATA(18);
		WRITE_VISDATA(19);
		WRITE_VISDATA(20);
		WRITE_VISDATA(21);
		WRITE_VISDATA(22);
		WRITE_VISDATA(23);
		WRITE_VISDATA(24);
		WRITE_VISDATA(25);
		WRITE_VISDATA(26);
		WRITE_VISDATA(27);
		WRITE_VISDATA(28);
		WRITE_VISDATA(29);

#undef WRITE_VISDATA
#undef WRITE_VISDATA_DISABLED

		if (data->dpvs.primaryLightVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.primaryLightVisData[0], data->dpvs.primaryLightVisDataCount);
			buf->clear_pointer(&dest->dpvs.primaryLightVisData[0]);
		}
		if (data->dpvs.reflectionProbeVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.reflectionProbeVisData[0], data->dpvs.reflectionProbeVisDataCount);
			buf->clear_pointer(&dest->dpvs.reflectionProbeVisData[0]);
		}
		if (data->dpvs.volumetricVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.volumetricVisData[0], data->dpvs.volumetricVisDataCount);
			buf->clear_pointer(&dest->dpvs.volumetricVisData[0]);
		}
		if (data->dpvs.decalVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.decalVisData[0], data->dpvs.decalVisDataCount);
			buf->clear_pointer(&dest->dpvs.decalVisData[0]);
		}

		buf->pop_stream();

		if (data->dpvs.lodData)
		{
			buf->align(3);
			buf->write(data->dpvs.lodData, data->dpvs.smodelCount + 1);
			buf->clear_pointer(&dest->dpvs.lodData);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		
#define WRITE_VISDATA(__INDEX__) \
		if (data->dpvs.tessellationCutoffVisData[__INDEX__]) \
		{ \
			buf->align(127); \
			buf->write(data->dpvs.tessellationCutoffVisData[__INDEX__], data->dpvs.surfaceVisDataCount); \
			buf->clear_pointer(&dest->dpvs.tessellationCutoffVisData[__INDEX__]); \
		}
#define WRITE_VISDATA_DISABLED(__INDEX__) \
		dest->dpvs.tessellationCutoffVisData[__INDEX__] = nullptr;

		WRITE_VISDATA(0);
		WRITE_VISDATA_DISABLED(1);
		WRITE_VISDATA_DISABLED(2);
		WRITE_VISDATA_DISABLED(3);
		WRITE_VISDATA_DISABLED(4);
		WRITE_VISDATA_DISABLED(5);
		WRITE_VISDATA_DISABLED(6);
		WRITE_VISDATA_DISABLED(7);
		WRITE_VISDATA_DISABLED(8);
		WRITE_VISDATA_DISABLED(9);
		WRITE_VISDATA_DISABLED(10);
		WRITE_VISDATA_DISABLED(11);
		WRITE_VISDATA_DISABLED(12);
		WRITE_VISDATA_DISABLED(13);
		WRITE_VISDATA_DISABLED(14);
		WRITE_VISDATA_DISABLED(15);
		WRITE_VISDATA_DISABLED(16);
		WRITE_VISDATA_DISABLED(17);
		WRITE_VISDATA_DISABLED(18);
		WRITE_VISDATA_DISABLED(19);
		WRITE_VISDATA_DISABLED(20);
		WRITE_VISDATA_DISABLED(21);
		WRITE_VISDATA_DISABLED(22);
		WRITE_VISDATA_DISABLED(23);
		WRITE_VISDATA_DISABLED(24);
		WRITE_VISDATA_DISABLED(25);
		WRITE_VISDATA_DISABLED(26);
		WRITE_VISDATA_DISABLED(27);
		WRITE_VISDATA_DISABLED(28);
		WRITE_VISDATA_DISABLED(29);

#undef WRITE_VISDATA
#undef WRITE_VISDATA_DISABLED

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
			buf->align(7);
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
			buf->align(15);
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
		buf->pop_stream();

		if (data->dpvs.sortedSmodelIndices)
		{
			buf->align(1);
			buf->write(data->dpvs.sortedSmodelIndices, data->dpvs.smodelCount);
			buf->clear_pointer(&dest->dpvs.sortedSmodelIndices);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvs.surfaceCastsSunShadowOpt)
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceCastsSunShadowOpt, data->dpvs.sunShadowOptCount * data->dpvs.sunSurfVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceCastsSunShadowOpt);
		}
		if (data->dpvs.constantBuffers)
		{
			buf->align(15);
			buf->write(data->dpvs.constantBuffers, data->dpvs.smodelCount);
			buf->clear_pointer(&dest->dpvs.constantBuffers);
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

#define WRITE_VISDATA(__INDEX__) \
		if (data->dpvsDyn.dynEntVisData[0][__INDEX__]) \
		{ \
			buf->align(15); \
			buf->write(data->dpvsDyn.dynEntVisData[0][__INDEX__], 32 * data->dpvsDyn.dynEntClientWordCount[0]); \
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[0][__INDEX__]); \
		} \
		if (data->dpvsDyn.dynEntVisData[1][__INDEX__]) \
		{ \
			buf->align(15); \
			buf->write(data->dpvsDyn.dynEntVisData[1][__INDEX__], 32 * data->dpvsDyn.dynEntClientWordCount[1]); \
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[1][__INDEX__]); \
		}
#define WRITE_VISDATA_DISABLED(__INDEX__) \
		dest->dpvsDyn.dynEntVisData[0][__INDEX__] = nullptr; \
		dest->dpvsDyn.dynEntVisData[1][__INDEX__] = nullptr;

		WRITE_VISDATA(0);
		WRITE_VISDATA(1);
		WRITE_VISDATA(2);
		WRITE_VISDATA_DISABLED(3);
		WRITE_VISDATA_DISABLED(4);
		WRITE_VISDATA_DISABLED(5);
		WRITE_VISDATA_DISABLED(6);
		WRITE_VISDATA_DISABLED(7);
		WRITE_VISDATA_DISABLED(8);
		WRITE_VISDATA_DISABLED(9);
		WRITE_VISDATA_DISABLED(10);
		WRITE_VISDATA_DISABLED(11);
		WRITE_VISDATA_DISABLED(12);
		WRITE_VISDATA_DISABLED(13);
		WRITE_VISDATA_DISABLED(14);
		WRITE_VISDATA_DISABLED(15);
		WRITE_VISDATA_DISABLED(16);
		WRITE_VISDATA_DISABLED(17);
		WRITE_VISDATA_DISABLED(18);
		WRITE_VISDATA_DISABLED(19);
		WRITE_VISDATA_DISABLED(20);
		WRITE_VISDATA_DISABLED(21);
		WRITE_VISDATA_DISABLED(22);
		WRITE_VISDATA_DISABLED(23);
		WRITE_VISDATA_DISABLED(24);
		WRITE_VISDATA_DISABLED(25);
		WRITE_VISDATA_DISABLED(26);
		WRITE_VISDATA_DISABLED(27);
		WRITE_VISDATA_DISABLED(28);
		WRITE_VISDATA_DISABLED(29);

#undef WRITE_VISDATA
#undef WRITE_VISDATA_DISABLED

		buf->pop_stream();

		if (data->heroOnlyLights)
		{
			buf->align(3);
			buf->write(data->heroOnlyLights, data->heroOnlyLightCount);
			buf->clear_pointer(&dest->heroOnlyLights);
		}

		if (data->umbraGates)
		{
			buf->align(3);
			buf->write(data->umbraGates, data->numUmbraGates);
			buf->clear_pointer(&dest->umbraGates);
		}

		if (data->umbraTomeData)
		{
			buf->align(15);
			buf->write(data->umbraTomeData, data->umbraTomeSize);
			buf->clear_pointer(&dest->umbraTomeData);
		}
		dest->umbraTomePtr = nullptr;

		if (data->umbraGates2)
		{
			buf->align(3);
			buf->write(data->umbraGates2, data->numUmbraGates2);
			buf->clear_pointer(&dest->umbraGates2);
		}

		if (data->umbraTomeData2)
		{
			buf->align(15);
			buf->write(data->umbraTomeData2, data->umbraTomeSize2);
			buf->clear_pointer(&dest->umbraTomeData2);
		}
		dest->umbraTomePtr2 = nullptr;

		if (data->umbraUnkData)
		{
			buf->align(15);
			buf->write(data->umbraUnkData, data->umbraUnkSize);
			buf->clear_pointer(&dest->umbraUnkData);
		}

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

		write.dump_array(asset->cellTransientInfos, asset->dpvsPlanes.cellCount);

		write.dump_array(asset->cells, asset->dpvsPlanes.cellCount);
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			write.dump_array(asset->cells[i].portals, asset->cells[i].portalCount);
			for (int j = 0; j < asset->cells[i].portalCount; j++)
			{
				write.dump_array(asset->cells[i].portals[j].vertices, asset->cells[i].portals[j].vertexCount);
			}
		}

		write.dump_array(asset->draw.reflectionProbeData.reflectionProbes, asset->draw.reflectionProbeData.reflectionProbeCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeCount; i++)
		{
			write.dump_string(asset->draw.reflectionProbeData.reflectionProbes[i].livePath);
			write.dump_array(asset->draw.reflectionProbeData.reflectionProbes[i].probeInstances, asset->draw.reflectionProbeData.reflectionProbes[i].probeInstanceCount);
		}

		write.dump_asset(asset->draw.reflectionProbeData.reflectionProbeArrayImage);
		write.dump_array(asset->draw.reflectionProbeData.probeRelightingData, asset->draw.reflectionProbeData.probeRelightingCount);
		write.dump_array(asset->draw.reflectionProbeData.reflectionProbeGBufferImages, asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount; i++)
		{
			 write.dump_asset(asset->draw.reflectionProbeData.reflectionProbeGBufferImages[i]);
		}
		write.dump_array(asset->draw.reflectionProbeData.reflectionProbeInstances, asset->draw.reflectionProbeData.reflectionProbeInstanceCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeData.reflectionProbeInstanceCount; i++)
		{
			write.dump_string(asset->draw.reflectionProbeData.reflectionProbeInstances[i].livePath);
			write.dump_string(asset->draw.reflectionProbeData.reflectionProbeInstances[i].livePath2);
		}
		write.dump_array(asset->draw.reflectionProbeData.reflectionProbeLightgridSampleData, asset->draw.reflectionProbeData.reflectionProbeCount);

		write.dump_array(asset->draw.lightmapReindexData.reindexElement, asset->draw.lightmapReindexData.reindexCount);
		write.dump_array(asset->draw.lightmapReindexData.packedLightmap, asset->draw.lightmapReindexData.packedLightmapCount);

		write.dump_asset(asset->draw.iesLookupTexture);
		write.dump_array(asset->draw.decalVolumeCollections, asset->draw.decalVolumeCollectionCount);
		write.dump_asset(asset->draw.lightmapOverridePrimary);
		write.dump_asset(asset->draw.lightmapOverrideSecondary);
		write.dump_array(asset->draw.lightMaps, asset->draw.lightMapCount);
		for (unsigned int i = 0; i < asset->draw.lightMapCount; i++)
		{
			write.dump_asset(asset->draw.lightMaps[i]);
		}

		for (auto i = 0; i < 32; i++)
		{
			write.dump_asset(asset->draw.transientZones[i]);
		}

		write.dump_array(asset->draw.indices, asset->draw.indexCount);

		write.dump_array(asset->draw.volumetrics.volumetrics, asset->draw.volumetrics.volumetricCount);
		for (unsigned int i = 0; i < asset->draw.volumetrics.volumetricCount; i++)
		{
			write.dump_string(asset->draw.volumetrics.volumetrics[i].livePath);
			for (auto m = 0; m < 4; m++)
			{
				write.dump_asset(asset->draw.volumetrics.volumetrics[i].masks[m].image);
			}
		}

		write.dump_array(asset->lightGrid.stageLightingContrastGain, asset->lightGrid.stageCount);
		write.dump_array(asset->lightGrid.paletteEntryAddress, asset->lightGrid.paletteEntryCount);
		write.dump_array(asset->lightGrid.paletteBitstream, asset->lightGrid.paletteBitstreamSize);

		write.dump_array(asset->lightGrid.tree.p_nodeTable, asset->lightGrid.tree.nodeCount);
		write.dump_array(asset->lightGrid.tree.p_leafTable, asset->lightGrid.tree.leafTableSize);

		write.dump_array(asset->lightGrid.probeData.gpuVisibleProbePositions, asset->lightGrid.probeData.gpuVisibleProbesCount);
		write.dump_array(asset->lightGrid.probeData.gpuVisibleProbesData, (asset->lightGrid.probeData.gpuVisibleProbesCount + 0x2000));

		write.dump_array(asset->lightGrid.probeData.probes, asset->lightGrid.probeData.probeCount);

		write.dump_array(asset->lightGrid.probeData.probePositions, asset->lightGrid.probeData.probeCount);

		write.dump_array(asset->lightGrid.probeData.zones, asset->lightGrid.probeData.zoneCount);

		write.dump_array(asset->lightGrid.probeData.tetrahedrons, asset->lightGrid.probeData.tetrahedronCount);

		write.dump_array(asset->lightGrid.probeData.tetrahedronNeighbors, asset->lightGrid.probeData.tetrahedronCount);

		write.dump_array(asset->lightGrid.probeData.tetrahedronVisibility, asset->lightGrid.probeData.tetrahedronCountVisible);

		write.dump_array(asset->lightGrid.probeData.voxelStartTetrahedron, asset->lightGrid.probeData.voxelStartTetrahedronCount);

		write.dump_array(asset->frustumLights, asset->primaryLightCount);
		if (asset->frustumLights)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->frustumLights[i].indices, asset->frustumLights[i].indexCount);
				write.dump_array(asset->frustumLights[i].vertices, 32 * asset->frustumLights[i].vertexCount);
			}
		}

		write.dump_array(asset->lightViewFrustums, asset->primaryLightCount);
		if (asset->lightViewFrustums)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->lightViewFrustums[i].planes, asset->lightViewFrustums[i].planeCount);
				write.dump_array(asset->lightViewFrustums[i].indices, asset->lightViewFrustums[i].indexCount);
				write.dump_array(asset->lightViewFrustums[i].vertices, asset->lightViewFrustums[i].vertexCount);
			}
		}

		write.dump_array(asset->voxelTree, asset->voxelTreeCount);
		for (int i = 0; i < asset->voxelTreeCount; i++)
		{
			write.dump_single(asset->voxelTree[i].voxelTreeHeader);
			write.dump_array(asset->voxelTree[i].voxelTopDownViewNodeArray, asset->voxelTree[i].voxelTopDownViewNodeCount);
			write.dump_array(asset->voxelTree[i].voxelInternalNodeArray, asset->voxelTree[i].voxelInternalNodeCount);
			write.dump_array(asset->voxelTree[i].voxelLeafNodeArray, asset->voxelTree[i].voxelLeafNodeCount);
			write.dump_array(asset->voxelTree[i].lightListArray, asset->voxelTree[i].lightListArraySize);
		}

		write.dump_array(asset->heightfields, asset->heightfieldCount);
		for (int i = 0; i < asset->heightfieldCount; i++)
		{
			write.dump_asset(asset->heightfields[i].image);
		}

		write.dump_array(asset->unk01.unk01, asset->unk01.unk01Count);
		write.dump_array(asset->unk01.unk02, asset->unk01.unk02Count);
		write.dump_array(asset->unk01.unk03, asset->unk01.unk03Count);

		write.dump_array(asset->models, asset->modelCount);

		write.dump_array(asset->materialMemory, asset->materialMemoryCount);
		for (int i = 0; i < asset->materialMemoryCount; i++)
		{
			write.dump_asset(asset->materialMemory[i].material);
		}

		write.dump_asset(asset->sun.spriteMaterial);
		write.dump_asset(asset->sun.flareMaterial);

		write.dump_asset(asset->outdoorImage);
		write.dump_asset(asset->dustMaterial);

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

		write.dump_array(asset->lightAABB.nodeArray, asset->lightAABB.nodeCount);
		write.dump_array(asset->lightAABB.lightArray, asset->lightAABB.lightCount);

		write.dump_array(asset->dpvs.lodData, asset->dpvs.smodelCount + 1);

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

		write.dump_array(asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceVisDataCount);

		write.dump_array(asset->dpvs.sortedSmodelIndices, asset->dpvs.smodelCount);

		write.dump_array(asset->heroOnlyLights, asset->heroOnlyLightCount);

		write.dump_array(asset->umbraGates, asset->numUmbraGates);
		write.dump_array(asset->umbraTomeData, asset->umbraTomeSize);

		write.dump_array(asset->umbraGates2, asset->numUmbraGates2);
		write.dump_array(asset->umbraTomeData2, asset->umbraTomeSize2);

		write.dump_array(asset->umbraUnkData, asset->umbraUnkSize);

		write.close();
	}
}