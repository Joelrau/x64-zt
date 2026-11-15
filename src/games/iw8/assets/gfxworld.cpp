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

		void read_dpvs_planes(zone_reader& reader, GfxWorldDpvsPlanes& planes)
		{
			reader.read_array(planes.planes, 3, planes.planeCount);
			reader.read_array(planes.nodes, 1, planes.nodeCount);

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(planes.sceneEntCellBits, 3, planes.cellCount << 9);
			reader.pop_stream();
		}

		void read_cells(zone_reader& reader, GfxWorld* asset)
		{
			const auto cells = reader.read_array(asset->cells, 7, asset->dpvsPlanes.cellCount);
			if (!cells)
			{
				return;
			}

			for (auto i = 0u; i < asset->dpvsPlanes.cellCount; i++)
			{
				if (const auto portals = reader.read_array(cells[i].portals, 7, cells[i].portalCount))
				{
					for (auto j = 0u; j < cells[i].portalCount; j++)
					{
						reader.read_array(portals[j].vertices, 3, portals[j].vertexCount);
					}
				}
			}
		}

		void read_surfaces(zone_reader& reader, GfxWorldSurfaces& surfaces)
		{
			if (const auto surfs = reader.read_array(surfaces.surfaces, 7, surfaces.count))
			{
				for (auto i = 0u; i < surfaces.count; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, surfs[i].material);
				}
			}

			reader.read_array(surfaces.surfaceBounds, 3, surfaces.count);
			reader.read_array(surfaces.surfaceMaterials, 7, surfaces.count);
			reader.read_array(surfaces.surfData, 63, surfaces.surfDataCount);
		}

		void read_static_models(zone_reader& reader, GfxWorldStaticModels& smodels)
		{
			reader.read_array(smodels.surfaces, 1, smodels.surfaceCount);

			if (const auto models = reader.read_array(smodels.models, 7, smodels.modelCount))
			{
				for (auto i = 0u; i < smodels.modelCount; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, models[i].model);
				}
			}

			reader.read_array(smodels.clutterCollections, 3, smodels.clutterCollectionCount);
			reader.read_array(smodels.collections, 3, smodels.collectionCount);
			reader.read_array(smodels.instances, 3, smodels.instanceCount);
			reader.read_array(smodels.instanceFlags, 0, smodels.instanceCount);
			reader.read_array(smodels.collectionBounds, 3, smodels.collectionCount);
			reader.read_asset_array(ASSET_TYPE_MATERIAL, smodels.materials, smodels.materialCount);
			reader.read_array(smodels.modelStaticIndirection, 3, smodels.instanceCount);
			reader.read_array(smodels.lmapTransforms, 3, smodels.lmapTransformsCount);
			reader.read_array(smodels.smodelSurfData, 15, smodels.surfaceCount);
			reader.read_array(smodels.smodelInstanceData, 15, smodels.instanceCount);
			reader.read_array(smodels.clutterTintData, 3, smodels.clutterTintCount);
			reader.read_array(smodels.clutterCollectionGPUData, 3, smodels.clutterCollectionCount);
			reader.read_array(smodels.smodelExpansionData, 3, smodels.modelCount);
			reader.read_array(smodels.smodelSurfMatIndirection, 1, smodels.surfaceCount);
			reader.read_array(smodels.smodelSurfUGBIndirection, 3, smodels.surfaceCount);
			reader.read_array(smodels.splinedModelInstanceBounds, 3, smodels.splinedModelInstanceCount);
			reader.read_array(smodels.splinedModelInstanceData, 15, smodels.splinedModelInstanceCount);
			reader.read_array(smodels.splineSegmentData, 15, smodels.splineSegmentCount);

			auto& visibility = smodels.collectionTransientZonePrevFrameVisibility;
			reader.read_array(visibility.array, 3, visibility.wordCount);
			reader.read_array(smodels.collectionVisData, 3, smodels.collectionVisDataCount);
			reader.read_array(smodels.collectionVisRefCount, 0, smodels.collectionCount);
		}

		void read_reflection_probes(zone_reader& reader, GfxWorldReflectionProbeData& data)
		{
			reader.read_asset_array(ASSET_TYPE_IMAGE, data.reflectionProbeImages, data.reflectionProbeCount);
			reader.read_asset_array(ASSET_TYPE_IMAGE, data.reflectionProbeImages_octahedron, data.sharedReflectionProbeCount);

			if (const auto probes = reader.read_array(data.reflectionProbes, 7, data.reflectionProbeCount))
			{
				for (auto i = 0u; i < data.reflectionProbeCount; i++)
				{
					reader.read_string(probes[i].livePath);
					reader.read_array(probes[i].probeInstances, 1, probes[i].probeInstanceCount);
				}
			}

			reader.read_asset(ASSET_TYPE_IMAGE, data.reflectionProbeArrayImage);
			reader.read_asset(ASSET_TYPE_IMAGE, data.reflectionProbeArrayImage_octahedron);

			if (const auto relighting = reader.read_array(data.probeRelightingData, 7, data.probeRelightingCount))
			{
				for (auto i = 0u; i < data.probeRelightingCount; i++)
				{
					reader.read_array(relighting[i].zones, 1, relighting[i].zoneCount);
				}
			}

			reader.read_asset_array(ASSET_TYPE_IMAGE, data.reflectionProbeGBufferImages, data.reflectionProbeGBufferImageCount);
			reader.read_asset_array(ASSET_TYPE_IMAGE, data.reflectionProbeGBufferImages_octahedron, data.reflectionProbeGBufferImageCount);

			if (const auto instances = reader.read_array(data.reflectionProbeInstances, 7, data.reflectionProbeInstanceCount))
			{
				for (auto i = 0u; i < data.reflectionProbeInstanceCount; i++)
				{
					reader.read_string(instances[i].livePath);
				}
			}

			reader.read_array(data.reflectionProbeSH, 63, 4 * data.reflectionProbeCount);
		}

		void read_volumetrics(zone_reader& reader, GfxWorldVolumetrics& volumetrics)
		{
			if (const auto entries = reader.read_array(volumetrics.volumetrics, 7, volumetrics.volumetricCount))
			{
				for (auto i = 0u; i < volumetrics.volumetricCount; i++)
				{
					reader.read_string(entries[i].livePath);

					for (auto& mask : entries[i].masks)
					{
						reader.read_asset(ASSET_TYPE_IMAGE, mask.image);
					}
				}
			}

			reader.read_asset(ASSET_TYPE_IMAGE, volumetrics.volumetricMaskAtlasImage);
		}

		void read_draw(zone_reader& reader, GfxWorldDraw& draw)
		{
			read_reflection_probes(reader, draw.reflectionProbeData);

			auto& reindex = draw.lightmapReindexData;
			reader.read_array(reindex.reindexElement, 3, reindex.reindexCount);
			reader.read_array(reindex.packedLightmap, 3, reindex.packedLightmapCount);

			reader.read_asset(ASSET_TYPE_IMAGE, draw.iesLookupTexture);

			if (const auto materials = reader.read_array(draw.decalVolumeMaterials, 7, draw.decalVolumeMaterialCount))
			{
				for (auto i = 0u; i < draw.decalVolumeMaterialCount; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, materials[i].material);
				}
			}

			if (const auto masks = reader.read_array(draw.decalVolumeMasks, 7, draw.decalVolumeMaskCount))
			{
				for (auto i = 0u; i < draw.decalVolumeMaskCount; i++)
				{
					reader.read_asset(ASSET_TYPE_DECAL_VOLUME_MASK, masks[i].blendMapOverride);
				}
			}

			reader.read_array(draw.decalVolumeCollections, 3, draw.decalVolumeCollectionCount);
			reader.read_array(draw.materialAndMaskIndirection, 3, draw.decalVolumeMaterialCount + draw.decalVolumeMaskCount);

			for (auto& zone : draw.transientZones)
			{
				reader.read_asset(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, zone);
			}

			reader.read_array(draw.lightmapTransientIndex, 3, draw.lightmapCount);
			read_volumetrics(reader, draw.volumetrics);
			reader.read_array(draw.compressedSunShadow, 0, draw.compressedSunShadowSize);
		}

		void read_dynamic_lightset_lm_data(zone_reader& reader, GfxDynamicLightsetLMData& data)
		{
			reader.read_array(data.chunkGLM, 3, data.numGChunkLM);
			reader.read_array(data.chunkGLMBF, 1, data.numGChunkLMBF);
			reader.read_array(data.chunkWhichLM, 1, data.numLMHave);
			reader.read_array(data.chunkLMStart, 3, data.numLMHave + (data.numLMHave != 0));
			reader.read_array(data.origTexels, 3, 2 * data.numOrigTexels);
			reader.read_array(data.origRecords, 3, data.numOrigTexels);
			reader.read_array(data.bfTexels, 3, 2 * data.numBFLMTexels);
			reader.read_array(data.skipRecords, 1, data.numSkipRecords);
			reader.read_array(data.palletteRecords, 3, data.numSkipRecords);
		}

		void read_dynamic_lightset(zone_reader& reader, GfxDynamicLightset& lightset)
		{
			reader.read_array(lightset.basisFunctions, 3, lightset.numBasisFunctions);
			reader.read_array(lightset.basisFunctionLights, 3, lightset.numBasisFunctionPrimaryLights);
			reader.read_array(lightset.portalGIData, 3, lightset.numPortalGI);
			reader.read_array(lightset.productGIData, 1, lightset.numProductGI);
			read_dynamic_lightset_lm_data(reader, lightset.lmData);
			read_lightset_probe_data(reader, lightset.lgProbeData);
		}

		void read_voxel_trees(zone_reader& reader, GfxWorld* asset)
		{
			const auto trees = reader.read_array(asset->voxelTree, 7, asset->voxelTreeCount);
			if (!trees)
			{
				return;
			}

			for (auto i = 0; i < asset->voxelTreeCount; i++)
			{
				auto& tree = trees[i];
				reader.read_single(tree.voxelTreeHeader, 15);
				reader.read_array(tree.voxelTopDownViewNodeArray, 3, tree.voxelTopDownViewNodeCount);
				reader.read_array(tree.voxelInternalNodeArray, 3, tree.voxelInternalNodeCount);
				reader.read_array(tree.voxelLeafNodeArray, 1, tree.voxelLeafNodeCount);
				reader.read_array(tree.lightListArray, 3, tree.lightListArraySize);

				reader.push_stream(XFILE_BLOCK_RUNTIME);
				reader.read_array(tree.voxelInternalNodeDynamicLightListCPU, 3, 2 * tree.voxelInternalNodeCount);
				reader.pop_stream();
			}
		}

		void read_frustum_lights(zone_reader& reader, GfxFrustumLights& lights)
		{
			reader.read_array(lights.indicesOffsets, 3, lights.lightCount);
			reader.read_array(lights.verticesOffsets, 3, lights.lightCount);
			reader.read_array(lights.indicesCounts, 1, lights.lightCount);
			reader.read_array(lights.verticesCounts, 1, lights.lightCount);
			reader.read_array(lights.indices, 3, lights.totalIndicesCount);
			reader.read_array(lights.vertices, 3, 16 * lights.totalVerticesCount);
		}

		void read_light_view_frustums(zone_reader& reader, GfxWorld* asset)
		{
			const auto frustums = reader.read_array(asset->lightViewFrustums, 7, asset->primaryLightCount);
			if (!frustums)
			{
				return;
			}

			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				reader.read_array(frustums[i].planes, 3, frustums[i].planeCount);
				reader.read_array(frustums[i].indices, 0, frustums[i].indexCount);
				reader.read_array(frustums[i].vertices, 3, frustums[i].vertexCount);
			}
		}

		void read_heightfields(zone_reader& reader, GfxWorld* asset)
		{
			if (const auto heightfields = reader.read_array(asset->heightfields, 7, asset->heightfieldCount))
			{
				for (auto i = 0; i < asset->heightfieldCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, heightfields[i].image);
				}
			}
		}

		void read_sky_illumination(zone_reader& reader, GfxPrecomputedSkyIllumination& sky)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, sky.detailMask);
			reader.read_asset(ASSET_TYPE_IMAGE, sky.heightMap);
			reader.read_asset(ASSET_TYPE_IMAGE, sky.SVDCoefficient);
			reader.read_asset(ASSET_TYPE_IMAGE, sky.SVDBasis);
		}

		void read_runtime_bits(zone_reader& reader, GfxWorld* asset)
		{
			const auto cell_count = asset->dpvsPlanes.cellCount;

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(asset->cellCasterBits, 3, cell_count * cell_bit_words(cell_count));
			reader.read_array(asset->cellHasSunLitSurfsBits, 3, cell_bit_words(cell_count));
			reader.read_array(asset->sceneDynModel, 3, asset->dpvsDyn.dynEntClientCount[0]);
			reader.read_array(asset->sceneDynBrush, 3, asset->dpvsDyn.dynEntClientCount[1]);
			reader.read_array(asset->primaryLightMotionDetectBits, 3, asset->primaryLightMotionDetectBitsEntries);
			reader.read_array(asset->entityMotionBits, 3, asset->entityMotionBitsEntries);
			reader.read_array(asset->primaryLightEntityShadowVis, 3, asset->numPrimaryLightEntityShadowVisEntries);
			reader.read_array(asset->dynEntMotionBits[0], 3, asset->dynEntMotionBitsEntries[0]);
			reader.read_array(asset->dynEntMotionBits[1], 3, asset->dynEntMotionBitsEntries[1]);
			reader.read_array(asset->primaryLightDynEntShadowVis[0], 3, asset->numPrimaryLightDynEntShadowVisEntries[0]);
			reader.read_array(asset->primaryLightDynEntShadowVis[1], 3, asset->numPrimaryLightDynEntShadowVisEntries[1]);
			reader.pop_stream();
		}

		void read_shadow_geometry(zone_reader& reader, GfxWorld* asset)
		{
			const auto geometry = reader.read_array(asset->shadowGeomOptimized, 7, asset->primaryLightCount);
			if (!geometry)
			{
				return;
			}

			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				reader.read_array(geometry[i].sortedSurfIndex, 3, geometry[i].surfaceCount);
				reader.read_array(geometry[i].smodelIndex, 1, geometry[i].smodelCount);
			}
		}

		void read_light_aabb(zone_reader& reader, GfxLightAABB& aabb)
		{
			reader.read_array(aabb.nodeArray, 3, aabb.nodeCount);
			reader.read_array(aabb.lightArray, 1, aabb.lightCount);
		}

		void read_dpvs_static(zone_reader& reader, GfxWorldDpvsStatic& dpvs)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			for (auto i = 0u; i < 32; i++)
			{
				reader.read_array(dpvs.smodelVisData[i], 127, i >= 4 && i < 12 ? 0 : dpvs.smodelVisDataCount);
			}

			for (auto i = 0u; i < 32; i++)
			{
				reader.read_array(dpvs.surfaceVisData[i], 127, i >= 4 && i < 12 ? 0 : dpvs.surfaceVisDataCount);
			}

			reader.read_array(dpvs.primaryLightVisData, 127, dpvs.primaryLightVisDataCount);
			reader.read_array(dpvs.reflectionProbeVisData, 127, dpvs.reflectionProbeVisDataCount);
			reader.read_array(dpvs.volumetricVisData, 127, dpvs.volumetricVisDataCount);
			reader.read_array(dpvs.decalVisData, 127, dpvs.decalVisDataCount);
			reader.pop_stream();

			reader.read_array(dpvs.sortedSurfIndex, 3, 32 * dpvs.surfaceVisDataCount);
			reader.read_array(dpvs.sortedSmodelIndices, 1, 32 * dpvs.smodelVisDataCount + 1);

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(dpvs.surfaceCastsSunShadow, 127, dpvs.surfaceVisDataCount);
			reader.read_array(dpvs.surfaceCastsSunShadowOpt, 127, dpvs.sunShadowOptCount * dpvs.sunSurfVisDataCount);
			reader.pop_stream();
		}

		void read_dpvs_dynamic(zone_reader& reader, GfxWorldDpvsDynamic& dpvs, const unsigned int cell_count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(dpvs.dynEntCellBits[0], 3, dpvs.dynEntClientWordCount[0] * cell_count);
			reader.read_array(dpvs.dynEntCellBits[1], 3, dpvs.dynEntClientWordCount[1] * cell_count);

			for (auto i = 0u; i < 32; i++)
			{
				for (auto j = 0u; j < 2; j++)
				{
					reader.read_array(dpvs.dynEntVisData[j][i], 15, i == 3 || i >= 12 ? 0 : 32 * dpvs.dynEntClientWordCount[j]);
				}
			}
			reader.pop_stream();
		}

		void read_mdao_volumes(zone_reader& reader, GfxWorld* asset)
		{
			if (const auto volumes = reader.read_array(asset->mdaoVolumes, 7, asset->mdaoVolumeCount))
			{
				for (auto i = 0u; i < asset->mdaoVolumeCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, volumes[i].volumeData);
				}
			}
		}

		void read_stream_tree(zone_reader& reader, StreamTree& tree)
		{
			if (const auto nodes = reader.read_array(tree.nodes, 7, tree.nodeCount))
			{
				for (auto i = 0u; i < tree.nodeCount; i++)
				{
					reader.read_asset(ASSET_TYPE_STREAM_KEY, nodes[i].volumesKey);
					reader.read_array(nodes[i].infos, 3, nodes[i].mtlInfoCount + nodes[i].imgInfoCount);
				}
			}

			reader.read_asset_array(ASSET_TYPE_MATERIAL, tree.materials, tree.materialCount);
			reader.read_asset_array(ASSET_TYPE_IMAGE, tree.images, tree.imageCount);

			auto& group = tree.largeMtlGroup;
			reader.read_asset(ASSET_TYPE_STREAM_KEY, group.volumesKey);
			reader.read_array(group.infos, 3, group.mtlInfoCount + group.imgInfoCount);
		}

		void read_stream_tree_grid(zone_reader& reader, StreamTreeGrid& grid)
		{
			reader.read_asset_array(ASSET_TYPE_STREAM_KEY, grid.cellKeys, grid.rowCount * grid.columnCount);
			reader.read_asset_array(ASSET_TYPE_MATERIAL, grid.materials, grid.materialCount);
			reader.read_asset_array(ASSET_TYPE_IMAGE, grid.images, grid.imageCount);
			reader.read_asset_array(ASSET_TYPE_XMODEL, grid.xmodels, grid.xmodelCount);
			reader.read_array(grid.smodelCollectionIndices, 1, grid.smodelCollectionIndexCount);
		}
	}

	void gfx_world::read(zone_reader& reader, GfxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->baseName);
		read_dpvs_planes(reader, asset->dpvsPlanes);
		reader.read_array(asset->cellTransientInfos, 1, asset->dpvsPlanes.cellCount);
		read_cells(reader, asset);
		read_surfaces(reader, asset->surfaces);
		read_static_models(reader, asset->smodels);
		read_draw(reader, asset->draw);
		read_dynamic_lightset(reader, asset->dynamicLightset);
		read_voxel_trees(reader, asset);

		auto& self_vis = asset->mayhemSelfVis;
		reader.read_array(self_vis.selfVis, 0, self_vis.selfVisBufferSize);
		read_frustum_lights(reader, asset->frustumLights);

		reader.push_stream(XFILE_BLOCK_RUNTIME);
		reader.read_array(asset->primaryLights, 7, asset->primaryLightCount);
		reader.pop_stream();

		read_light_view_frustums(reader, asset);
		read_heightfields(reader, asset);
		reader.read_array(asset->models, 3, asset->modelCount);
		read_sky_illumination(reader, asset->precomputedSkyIllumination);
		reader.read_array(asset->lmapLookup, 3, asset->lmapLookupCount);
		reader.read_array(asset->gfxMapEntLookup, 3, asset->gfxMapEntLookupCount);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->sun.spriteMaterial);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->sun.flareMaterial);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->outdoorImage);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->dustMaterial);
		read_runtime_bits(reader, asset);
		read_shadow_geometry(reader, asset);
		read_light_aabb(reader, asset->lightAABB);
		read_dpvs_static(reader, asset->dpvs);
		read_dpvs_dynamic(reader, asset->dpvsDyn, asset->dpvsPlanes.cellCount);
		reader.read_array(asset->umbraGates, 3, asset->numUmbraGates);
		reader.read_array(asset->umbraTomeData, 15, asset->umbraTomeSize);
		read_mdao_volumes(reader, asset);
		read_stream_tree(reader, asset->streamTree);
		read_stream_tree_grid(reader, asset->materialStreamTreeGrid);
		read_stream_tree_grid(reader, asset->xmodelStreamTreeGrid);
		reader.pop_stream();
	}
}
