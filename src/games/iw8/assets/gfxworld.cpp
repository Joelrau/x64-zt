#include <std_include.hpp>
#include "gfxworld.hpp"

namespace zonetool::iw8
{
	namespace
	{
		unsigned int cell_bit_words(const unsigned int cell_count)
		{
			return (cell_count + 31) >> 5;
		}

		template <typename Walk>
		void walk_dpvs_planes(Walk walk, GfxWorldDpvsPlanes& planes)
		{
			walk.array(planes.planes, planes.planeCount, 3);
			walk.array(planes.nodes, planes.nodeCount, 1);

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(planes.sceneEntCellBits, planes.cellCount << 9, 3);
			walk.pop_stream();
		}

		template <typename Walk>
		void walk_cells(Walk walk, GfxWorld& asset)
		{
			const auto cells = walk.array(asset.cells, asset.dpvsPlanes.cellCount, 7);
			if (!cells)
			{
				return;
			}

			for (auto i = 0u; i < asset.dpvsPlanes.cellCount; i++)
			{
				if (const auto portals = walk.array(cells[i].portals, cells[i].portalCount, 7))
				{
					for (auto j = 0u; j < cells[i].portalCount; j++)
					{
						walk.array(portals[j].vertices, portals[j].vertexCount, 3);
					}
				}
			}
		}

		template <typename Walk>
		void walk_surfaces(Walk walk, GfxWorldSurfaces& surfaces)
		{
			if (const auto surfs = walk.array(surfaces.surfaces, surfaces.count, 7))
			{
				for (auto i = 0u; i < surfaces.count; i++)
				{
					walk.asset(ASSET_TYPE_MATERIAL, surfs[i].material);
				}
			}

			walk.array(surfaces.surfaceBounds, surfaces.count, 3);
			walk.array(surfaces.surfaceMaterials, surfaces.count, 7);
			walk.array(surfaces.surfData, surfaces.surfDataCount, 63);
		}

		template <typename Walk>
		void walk_static_models(Walk walk, GfxWorldStaticModels& smodels)
		{
			walk.array(smodels.surfaces, smodels.surfaceCount, 1);

			if (const auto models = walk.array(smodels.models, smodels.modelCount, 7))
			{
				for (auto i = 0u; i < smodels.modelCount; i++)
				{
					walk.asset(ASSET_TYPE_XMODEL, models[i].model);
				}
			}

			walk.array(smodels.clutterCollections, smodels.clutterCollectionCount, 3);
			walk.array(smodels.collections, smodels.collectionCount, 3);
			walk.array(smodels.instances, smodels.instanceCount, 3);
			walk.array(smodels.instanceFlags, smodels.instanceCount, 0);
			walk.array(smodels.collectionBounds, smodels.collectionCount, 3);
			walk_asset_array(walk, ASSET_TYPE_MATERIAL, smodels.materials, smodels.materialCount);
			walk.array(smodels.modelStaticIndirection, smodels.instanceCount, 3);
			walk.array(smodels.lmapTransforms, smodels.lmapTransformsCount, 3);
			walk.array(smodels.smodelSurfData, smodels.surfaceCount, 15);
			walk.array(smodels.smodelInstanceData, smodels.instanceCount, 15);
			walk.array(smodels.clutterTintData, smodels.clutterTintCount, 3);
			walk.array(smodels.clutterCollectionGPUData, smodels.clutterCollectionCount, 3);
			walk.array(smodels.smodelExpansionData, smodels.modelCount, 3);
			walk.array(smodels.smodelSurfMatIndirection, smodels.surfaceCount, 1);
			walk.array(smodels.smodelSurfUGBIndirection, smodels.surfaceCount, 3);
			walk.array(smodels.splinedModelInstanceBounds, smodels.splinedModelInstanceCount, 3);
			walk.array(smodels.splinedModelInstanceData, smodels.splinedModelInstanceCount, 15);
			walk.array(smodels.splineSegmentData, smodels.splineSegmentCount, 15);

			auto& visibility = smodels.collectionTransientZonePrevFrameVisibility;
			walk.array(visibility.array, visibility.wordCount, 3);
			walk.array(smodels.collectionVisData, smodels.collectionVisDataCount, 3);
			walk.array(smodels.collectionVisRefCount, smodels.collectionCount, 0);
		}

		template <typename Walk>
		void walk_reflection_probes(Walk walk, GfxWorldReflectionProbeData& data)
		{
			walk_asset_array(walk, ASSET_TYPE_IMAGE, data.reflectionProbeImages, data.reflectionProbeCount);
			walk_asset_array(walk, ASSET_TYPE_IMAGE, data.reflectionProbeImages_octahedron, data.sharedReflectionProbeCount);

			if (const auto probes = walk.array(data.reflectionProbes, data.reflectionProbeCount, 7))
			{
				for (auto i = 0u; i < data.reflectionProbeCount; i++)
				{
					walk.string(probes[i].livePath);
					walk.array(probes[i].probeInstances, probes[i].probeInstanceCount, 1);
				}
			}

			walk.asset(ASSET_TYPE_IMAGE, data.reflectionProbeArrayImage);
			walk.asset(ASSET_TYPE_IMAGE, data.reflectionProbeArrayImage_octahedron);

			if (const auto relighting = walk.array(data.probeRelightingData, data.probeRelightingCount, 7))
			{
				for (auto i = 0u; i < data.probeRelightingCount; i++)
				{
					walk.array(relighting[i].zones, relighting[i].zoneCount, 1);
				}
			}

			walk_asset_array(walk, ASSET_TYPE_IMAGE, data.reflectionProbeGBufferImages, data.reflectionProbeGBufferImageCount);
			walk_asset_array(walk, ASSET_TYPE_IMAGE, data.reflectionProbeGBufferImages_octahedron, data.reflectionProbeGBufferImageCount);

			if (const auto instances = walk.array(data.reflectionProbeInstances, data.reflectionProbeInstanceCount, 7))
			{
				for (auto i = 0u; i < data.reflectionProbeInstanceCount; i++)
				{
					walk.string(instances[i].livePath);
				}
			}

			walk.array(data.reflectionProbeSH, 4 * data.reflectionProbeCount, 63);
		}

		template <typename Walk>
		void walk_volumetrics(Walk walk, GfxWorldVolumetrics& volumetrics)
		{
			if (const auto entries = walk.array(volumetrics.volumetrics, volumetrics.volumetricCount, 7))
			{
				for (auto i = 0u; i < volumetrics.volumetricCount; i++)
				{
					walk.string(entries[i].livePath);

					for (auto& mask : entries[i].masks)
					{
						walk.asset(ASSET_TYPE_IMAGE, mask.image);
					}
				}
			}

			walk.asset(ASSET_TYPE_IMAGE, volumetrics.volumetricMaskAtlasImage);
		}

		template <typename Walk>
		void walk_draw(Walk walk, GfxWorldDraw& draw)
		{
			walk_reflection_probes(walk, draw.reflectionProbeData);

			auto& reindex = draw.lightmapReindexData;
			walk.array(reindex.reindexElement, reindex.reindexCount, 3);
			walk.array(reindex.packedLightmap, reindex.packedLightmapCount, 3);

			walk.asset(ASSET_TYPE_IMAGE, draw.iesLookupTexture);

			if (const auto materials = walk.array(draw.decalVolumeMaterials, draw.decalVolumeMaterialCount, 7))
			{
				for (auto i = 0u; i < draw.decalVolumeMaterialCount; i++)
				{
					walk.asset(ASSET_TYPE_MATERIAL, materials[i].material);
				}
			}

			if (const auto masks = walk.array(draw.decalVolumeMasks, draw.decalVolumeMaskCount, 7))
			{
				for (auto i = 0u; i < draw.decalVolumeMaskCount; i++)
				{
					walk.asset(ASSET_TYPE_DECAL_VOLUME_MASK, masks[i].blendMapOverride);
				}
			}

			walk.array(draw.decalVolumeCollections, draw.decalVolumeCollectionCount, 3);
			walk.array(draw.materialAndMaskIndirection, draw.decalVolumeMaterialCount + draw.decalVolumeMaskCount, 3);

			for (auto& zone : draw.transientZones)
			{
				walk.asset(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, zone);
			}

			walk.array(draw.lightmapTransientIndex, draw.lightmapCount, 3);
			walk_volumetrics(walk, draw.volumetrics);
			walk.array(draw.compressedSunShadow, draw.compressedSunShadowSize, 0);
		}

		template <typename Walk>
		void walk_dynamic_lightset_lm_data(Walk walk, GfxDynamicLightsetLMData& data)
		{
			walk.array(data.chunkGLM, data.numGChunkLM, 3);
			walk.array(data.chunkGLMBF, data.numGChunkLMBF, 1);
			walk.array(data.chunkWhichLM, data.numLMHave, 1);
			walk.array(data.chunkLMStart, data.numLMHave + (data.numLMHave != 0), 3);
			walk.array(data.origTexels, 2 * data.numOrigTexels, 3);
			walk.array(data.origRecords, data.numOrigTexels, 3);
			walk.array(data.bfTexels, 2 * data.numBFLMTexels, 3);
			walk.array(data.skipRecords, data.numSkipRecords, 1);
			walk.array(data.palletteRecords, data.numSkipRecords, 3);
		}

		template <typename Walk>
		void walk_dynamic_lightset(Walk walk, GfxDynamicLightset& lightset)
		{
			walk.array(lightset.basisFunctions, lightset.numBasisFunctions, 3);
			walk.array(lightset.basisFunctionLights, lightset.numBasisFunctionPrimaryLights, 3);
			walk.array(lightset.portalGIData, lightset.numPortalGI, 3);
			walk.array(lightset.productGIData, lightset.numProductGI, 1);
			walk_dynamic_lightset_lm_data(walk, lightset.lmData);
			walk_lightset_probe_data(walk, lightset.lgProbeData);
		}

		template <typename Walk>
		void walk_voxel_trees(Walk walk, GfxWorld& asset)
		{
			const auto trees = walk.array(asset.voxelTree, asset.voxelTreeCount, 7);
			if (!trees)
			{
				return;
			}

			for (auto i = 0; i < asset.voxelTreeCount; i++)
			{
				auto& tree = trees[i];
				walk.array(tree.voxelTreeHeader, 1, 15);
				walk.array(tree.voxelTopDownViewNodeArray, tree.voxelTopDownViewNodeCount, 3);
				walk.array(tree.voxelInternalNodeArray, tree.voxelInternalNodeCount, 3);
				walk.array(tree.voxelLeafNodeArray, tree.voxelLeafNodeCount, 1);
				walk.array(tree.lightListArray, tree.lightListArraySize, 3);

				walk.push_stream(XFILE_BLOCK_RUNTIME);
				walk.array(tree.voxelInternalNodeDynamicLightListCPU, 2 * tree.voxelInternalNodeCount, 3);
				walk.pop_stream();
			}
		}

		template <typename Walk>
		void walk_frustum_lights(Walk walk, GfxFrustumLights& lights)
		{
			walk.array(lights.indicesOffsets, lights.lightCount, 3);
			walk.array(lights.verticesOffsets, lights.lightCount, 3);
			walk.array(lights.indicesCounts, lights.lightCount, 1);
			walk.array(lights.verticesCounts, lights.lightCount, 1);
			walk.array(lights.indices, lights.totalIndicesCount, 3);
			walk.array(lights.vertices, 16 * lights.totalVerticesCount, 3);
		}

		template <typename Walk>
		void walk_light_view_frustums(Walk walk, GfxWorld& asset)
		{
			const auto frustums = walk.array(asset.lightViewFrustums, asset.primaryLightCount, 7);
			if (!frustums)
			{
				return;
			}

			for (auto i = 0u; i < asset.primaryLightCount; i++)
			{
				walk.array(frustums[i].planes, frustums[i].planeCount, 3);
				walk.array(frustums[i].indices, frustums[i].indexCount, 0);
				walk.array(frustums[i].vertices, frustums[i].vertexCount, 3);
			}
		}

		template <typename Walk>
		void walk_heightfields(Walk walk, GfxWorld& asset)
		{
			if (const auto heightfields = walk.array(asset.heightfields, asset.heightfieldCount, 7))
			{
				for (auto i = 0; i < asset.heightfieldCount; i++)
				{
					walk.asset(ASSET_TYPE_IMAGE, heightfields[i].image);
				}
			}
		}

		template <typename Walk>
		void walk_sky_illumination(Walk walk, GfxPrecomputedSkyIllumination& sky)
		{
			walk.asset(ASSET_TYPE_IMAGE, sky.detailMask);
			walk.asset(ASSET_TYPE_IMAGE, sky.heightMap);
			walk.asset(ASSET_TYPE_IMAGE, sky.SVDCoefficient);
			walk.asset(ASSET_TYPE_IMAGE, sky.SVDBasis);
		}

		template <typename Walk>
		void walk_runtime_bits(Walk walk, GfxWorld& asset)
		{
			const auto cell_count = asset.dpvsPlanes.cellCount;

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(asset.cellCasterBits, cell_count * cell_bit_words(cell_count), 3);
			walk.array(asset.cellHasSunLitSurfsBits, cell_bit_words(cell_count), 3);
			walk.array(asset.sceneDynModel, asset.dpvsDyn.dynEntClientCount[0], 3);
			walk.array(asset.sceneDynBrush, asset.dpvsDyn.dynEntClientCount[1], 3);
			walk.array(asset.primaryLightMotionDetectBits, asset.primaryLightMotionDetectBitsEntries, 3);
			walk.array(asset.entityMotionBits, asset.entityMotionBitsEntries, 3);
			walk.array(asset.primaryLightEntityShadowVis, asset.numPrimaryLightEntityShadowVisEntries, 3);
			walk.array(asset.dynEntMotionBits[0], asset.dynEntMotionBitsEntries[0], 3);
			walk.array(asset.dynEntMotionBits[1], asset.dynEntMotionBitsEntries[1], 3);
			walk.array(asset.primaryLightDynEntShadowVis[0], asset.numPrimaryLightDynEntShadowVisEntries[0], 3);
			walk.array(asset.primaryLightDynEntShadowVis[1], asset.numPrimaryLightDynEntShadowVisEntries[1], 3);
			walk.pop_stream();
		}

		template <typename Walk>
		void walk_shadow_geometry(Walk walk, GfxWorld& asset)
		{
			const auto geometry = walk.array(asset.shadowGeomOptimized, asset.primaryLightCount, 7);
			if (!geometry)
			{
				return;
			}

			for (auto i = 0u; i < asset.primaryLightCount; i++)
			{
				walk.array(geometry[i].sortedSurfIndex, geometry[i].surfaceCount, 3);
				walk.array(geometry[i].smodelIndex, geometry[i].smodelCount, 1);
			}
		}

		template <typename Walk>
		void walk_light_aabb(Walk walk, GfxLightAABB& aabb)
		{
			walk.array(aabb.nodeArray, aabb.nodeCount, 3);
			walk.array(aabb.lightArray, aabb.lightCount, 1);
		}

		template <typename Walk>
		void walk_dpvs_static(Walk walk, GfxWorldDpvsStatic& dpvs)
		{
			walk.push_stream(XFILE_BLOCK_RUNTIME);
			for (auto i = 0u; i < 32; i++)
			{
				walk.array(dpvs.smodelVisData[i], i >= 4 && i < 12 ? 0 : dpvs.smodelVisDataCount, 127);
			}

			for (auto i = 0u; i < 32; i++)
			{
				walk.array(dpvs.surfaceVisData[i], i >= 4 && i < 12 ? 0 : dpvs.surfaceVisDataCount, 127);
			}

			walk.array(dpvs.primaryLightVisData, dpvs.primaryLightVisDataCount, 127);
			walk.array(dpvs.reflectionProbeVisData, dpvs.reflectionProbeVisDataCount, 127);
			walk.array(dpvs.volumetricVisData, dpvs.volumetricVisDataCount, 127);
			walk.array(dpvs.decalVisData, dpvs.decalVisDataCount, 127);
			walk.pop_stream();

			walk.array(dpvs.sortedSurfIndex, 32 * dpvs.surfaceVisDataCount, 3);
			walk.array(dpvs.sortedSmodelIndices, 32 * dpvs.smodelVisDataCount + 1, 1);

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(dpvs.surfaceCastsSunShadow, dpvs.surfaceVisDataCount, 127);
			walk.array(dpvs.surfaceCastsSunShadowOpt, dpvs.sunShadowOptCount * dpvs.sunSurfVisDataCount, 127);
			walk.pop_stream();
		}

		template <typename Walk>
		void walk_dpvs_dynamic(Walk walk, GfxWorldDpvsDynamic& dpvs, const unsigned int cell_count)
		{
			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(dpvs.dynEntCellBits[0], dpvs.dynEntClientWordCount[0] * cell_count, 3);
			walk.array(dpvs.dynEntCellBits[1], dpvs.dynEntClientWordCount[1] * cell_count, 3);

			for (auto i = 0u; i < 32; i++)
			{
				for (auto j = 0u; j < 2; j++)
				{
					walk.array(dpvs.dynEntVisData[j][i], i == 3 || i >= 12 ? 0 : 32 * dpvs.dynEntClientWordCount[j], 15);
				}
			}
			walk.pop_stream();
		}

		template <typename Walk>
		void walk_mdao_volumes(Walk walk, GfxWorld& asset)
		{
			if (const auto volumes = walk.array(asset.mdaoVolumes, asset.mdaoVolumeCount, 7))
			{
				for (auto i = 0u; i < asset.mdaoVolumeCount; i++)
				{
					walk.asset(ASSET_TYPE_IMAGE, volumes[i].volumeData);
				}
			}
		}

		template <typename Walk>
		void walk_stream_tree(Walk walk, StreamTree& tree)
		{
			if (const auto nodes = walk.array(tree.nodes, tree.nodeCount, 7))
			{
				for (auto i = 0u; i < tree.nodeCount; i++)
				{
					walk.asset(ASSET_TYPE_STREAM_KEY, nodes[i].volumesKey);
					walk.array(nodes[i].infos, nodes[i].mtlInfoCount + nodes[i].imgInfoCount, 3);
				}
			}

			walk_asset_array(walk, ASSET_TYPE_MATERIAL, tree.materials, tree.materialCount);
			walk_asset_array(walk, ASSET_TYPE_IMAGE, tree.images, tree.imageCount);

			auto& group = tree.largeMtlGroup;
			walk.asset(ASSET_TYPE_STREAM_KEY, group.volumesKey);
			walk.array(group.infos, group.mtlInfoCount + group.imgInfoCount, 3);
		}

		template <typename Walk>
		void walk_stream_tree_grid(Walk walk, StreamTreeGrid& grid)
		{
			walk_asset_array(walk, ASSET_TYPE_STREAM_KEY, grid.cellKeys, grid.rowCount * grid.columnCount);
			walk_asset_array(walk, ASSET_TYPE_MATERIAL, grid.materials, grid.materialCount);
			walk_asset_array(walk, ASSET_TYPE_IMAGE, grid.images, grid.imageCount);
			walk_asset_array(walk, ASSET_TYPE_XMODEL, grid.xmodels, grid.xmodelCount);
			walk.array(grid.smodelCollectionIndices, grid.smodelCollectionIndexCount, 1);
		}

		template <typename Walk>
		void walk_gfx_world(Walk walk, GfxWorld& asset)
		{
			walk.string(asset.baseName);
			walk_dpvs_planes(walk, asset.dpvsPlanes);
			walk.array(asset.cellTransientInfos, asset.dpvsPlanes.cellCount, 1);
			walk_cells(walk, asset);
			walk_surfaces(walk, asset.surfaces);
			walk_static_models(walk, asset.smodels);
			walk_draw(walk, asset.draw);
			walk_dynamic_lightset(walk, asset.dynamicLightset);
			walk_voxel_trees(walk, asset);

			auto& self_vis = asset.mayhemSelfVis;
			walk.array(self_vis.selfVis, self_vis.selfVisBufferSize, 0);
			walk_frustum_lights(walk, asset.frustumLights);

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(asset.primaryLights, asset.primaryLightCount, 7);
			walk.pop_stream();

			walk_light_view_frustums(walk, asset);
			walk_heightfields(walk, asset);
			walk.array(asset.models, asset.modelCount, 3);
			walk_sky_illumination(walk, asset.precomputedSkyIllumination);
			walk.array(asset.lmapLookup, asset.lmapLookupCount, 3);
			walk.array(asset.gfxMapEntLookup, asset.gfxMapEntLookupCount, 3);
			walk.asset(ASSET_TYPE_MATERIAL, asset.sun.spriteMaterial);
			walk.asset(ASSET_TYPE_MATERIAL, asset.sun.flareMaterial);
			walk.asset(ASSET_TYPE_IMAGE, asset.outdoorImage);
			walk.asset(ASSET_TYPE_MATERIAL, asset.dustMaterial);
			walk_runtime_bits(walk, asset);
			walk_shadow_geometry(walk, asset);
			walk_light_aabb(walk, asset.lightAABB);
			walk_dpvs_static(walk, asset.dpvs);
			walk_dpvs_dynamic(walk, asset.dpvsDyn, asset.dpvsPlanes.cellCount);
			walk.array(asset.umbraGates, asset.numUmbraGates, 3);
			walk.array(asset.umbraTomeData, asset.umbraTomeSize, 15);
			walk_mdao_volumes(walk, asset);
			walk_stream_tree(walk, asset.streamTree);
			walk_stream_tree_grid(walk, asset.materialStreamTreeGrid);
			walk_stream_tree_grid(walk, asset.xmodelStreamTreeGrid);
		}
	}

	void gfx_world::read(zone_reader& reader, GfxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_gfx_world(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void gfx_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_gfx_world(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void gfx_world::dump(GfxWorld* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".gfxmap"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_gfx_world(dump_walk{dump}, *asset);
		dump.close();
	}

	GfxWorld* gfx_world::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".gfxmap"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gfxworld \"%s\"...", name.data());

		auto* asset = read.read_single<GfxWorld>();
		asset->name = read.read_string();
		walk_gfx_world(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void gfx_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<GfxWorld>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void gfx_world::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_gfx_world(depend_walk{zone}, *this->asset_);
	}

	std::string gfx_world::name()
	{
		return this->name_;
	}

	std::int32_t gfx_world::type()
	{
		return ASSET_TYPE_GFXWORLD;
	}
}
