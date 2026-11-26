#include <std_include.hpp>
#include "gfxworld.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_unknown_list(Walk walk, GfxWorldUnknownList& list)
		{
			if (const auto entries = walk.array(list.entries, list.count, 7))
			{
				for (auto i = 0u; i < list.count; i++)
				{
					walk.array(entries[i].data, entries[i].count, 1);
				}
			}
		}

		template <typename Walk>
		void walk_surfaces(Walk walk, GfxWorldSurfaces& surfaces)
		{
			walk.array(surfaces.surfData, surfaces.surfDataCount, 3);
			walk.array(surfaces.surfaces, surfaces.count, 3);
			walk.array(surfaces.surfaceBounds, surfaces.count, 3);
			walk.array(surfaces.surfaceUnknown, surfaces.count, 1);
			walk_asset_array(walk, ASSET_TYPE_MATERIAL, surfaces.materials, surfaces.materialCount);

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(surfaces.materialRuntime, surfaces.materialCount, 3);
			walk.pop_stream();

			walk.array(surfaces.ugbData, surfaces.ugbCount, 63);
			walk.array(surfaces.ugbData2, surfaces.ugbCount, 3);
			walk.array(surfaces.ugbData3, surfaces.ugbCount, 63);
			walk.array(surfaces.surfaceData, surfaces.count, 63);
			walk.array(surfaces.indices.dataA, surfaces.indices.countA, 3);
			walk.array(surfaces.indices.dataB, surfaces.indices.countB, 3);
			walk.array(surfaces.unknownB, surfaces.unknownCountB, 3);

			if (const auto entries = walk.array(surfaces.unknownA, surfaces.unknownCountA, 7))
			{
				for (auto i = 0u; i < surfaces.unknownCountA; i++)
				{
					walk.asset(ASSET_TYPE_STREAM_KEY, entries[i].streamKey);
				}
			}

			walk.array(surfaces.unknownABits, (surfaces.unknownCountA + 31) >> 5, 3);
			walk.array(surfaces.lodGroups, surfaces.lodGroupCount, 3);
			walk.array(surfaces.materialData, surfaces.materialCount, 3);
			walk.array(surfaces.unknownD, surfaces.unknownCountD, 63);
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
			walk_temp_array(walk, smodels.splinedModelInstanceData, smodels.splinedModelInstanceCount, 15);
			walk_temp_array(walk, smodels.splineSegmentData, smodels.splineSegmentCount, 15);
			walk.array(smodels.splinedModelInstanceBounds, smodels.splinedModelInstanceCount, 3);

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			auto& visibility = smodels.collectionTransientZonePrevFrameVisibility;
			walk.array(visibility.array, visibility.wordCount, 3);
			walk.array(smodels.collectionVisData, smodels.collectionVisDataCount, 3);
			walk.array(smodels.collectionVisRefCount, smodels.collectionCount, 0);
			walk.pop_stream();

			walk.array(smodels.unknownData, smodels.unknownCount, 1);
		}

		template <typename Walk>
		void walk_dynamic_lightset_lm_data(Walk walk, GfxDynamicLightsetLMData& data)
		{
			walk.array(data.chunkGLM, data.numGChunkLM, 3);
			walk.array(data.chunkGLMBF, data.numGChunkLMBF, 1);
			walk.array(data.chunkWhichLM, data.numLMHave, 1);
			walk.array(data.chunkLMStart, data.numLMHave + (data.numLMHave != 0), 3);
			walk_temp_array(walk, data.origTexels, 2 * data.numOrigTexels, 3);
			walk_temp_array(walk, data.origRecords, data.numOrigTexels, 3);
			walk_temp_array(walk, data.bfTexels, 2 * data.numBFLMTexels, 3);
			walk.array(data.skipRecords, data.numSkipRecords, 1);
			walk.array(data.palletteRecords, data.numSkipRecords, 3);
			walk.array(data.backingChunkData, 2 * data.numGChunkLM, 3);
		}

		template <typename Walk>
		void walk_dynamic_lightset(Walk walk, GfxDynamicLightset& lightset)
		{
			walk.array(lightset.basisFunctions, lightset.numBasisFunctions, 3);
			walk.array(lightset.basisFunctionLights, lightset.numBasisFunctionPrimaryLights, 3);
			walk.array(lightset.unknownData, lightset.unknownCount, 3);
			walk.array(lightset.unknownData2, lightset.unknownCount2, 3);
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
			walk.array(lights.lightUnknown, lights.lightCount, 3);
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
		void walk_dpvs_static(Walk walk, GfxWorldDpvsStatic& dpvs)
		{
			walk.array(dpvs.sortedSurfIndex, 32 * dpvs.surfaceVisDataCount, 3);
			walk.array(dpvs.sortedSmodelIndices, 32 * dpvs.smodelVisDataCount + 1, 1);

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			for (auto i = 0u; i < 32; i++)
			{
				walk.array(dpvs.smodelVisData[i], i >= 4 && i < 12 ? 0 : dpvs.smodelVisDataCount, 127);
			}

			for (auto i = 0u; i < 32; i++)
			{
				walk.array(dpvs.surfaceVisData[i], i >= 4 && i < 12 ? 0 : dpvs.surfaceVisDataCount, 127);
			}

			walk.array(dpvs.visData8, dpvs.count8, 127);
			walk.array(dpvs.visData14, dpvs.count14, 127);
			walk.array(dpvs.visData18, dpvs.count18, 127);
			walk.array(dpvs.visData28, dpvs.count28, 127);
			walk.array(dpvs.visData2C, dpvs.count2C, 127);
			walk.array(dpvs.visData4, dpvs.surfaceVisDataCount, 127);
			walk.array(dpvs.visData10x0C, dpvs.count10 * dpvs.countC, 127);
			walk.array(dpvs.unknownData, dpvs.count20, 7);
			walk.array(dpvs.data1C, dpvs.count1C, 3);
			walk.array(dpvs.data20, dpvs.count20, 3);
			walk.array(dpvs.data24, dpvs.count24, 3);
			walk.array(dpvs.data2E8, dpvs.count2E8, 3);
			walk.array(dpvs.data2EC, dpvs.count2EC, 3);
			walk.array(dpvs.data2F8, dpvs.count2F8, 3);
			walk.array(dpvs.data2FC, dpvs.count2FC, 3);
			walk.array(dpvs.data300, dpvs.count300, 3);
			walk.array(dpvs.data304, dpvs.count304, 3);

			for (auto i = 0u; i < 12; i++)
			{
				walk.array(dpvs.data2F0[i], i == 3 ? 0 : dpvs.count2F0, 1);
				walk.array(dpvs.data2F4[i], i == 3 ? 0 : dpvs.count2F4, 1);
			}

			walk.array(dpvs.extraData2F0, dpvs.count2F0, 3);
			walk.array(dpvs.extraData2F4, dpvs.count2F4, 3);
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
				walk.array(geometry[i].data0, geometry[i].count0, 1);
				walk.array(geometry[i].data2, geometry[i].count2, 3);
				walk.array(geometry[i].data4, geometry[i].count4, 3);

				walk.push_stream(XFILE_BLOCK_RUNTIME);
				walk.array(geometry[i].data6, geometry[i].count6, 3);
				walk.pop_stream();
			}
		}

		template <typename Walk>
		void walk_light_aabb(Walk walk, GfxLightAABB& aabb)
		{
			walk.array(aabb.nodeArray, aabb.nodeCount, 3);
			walk.array(aabb.lightArray, aabb.lightCount, 3);
			walk.array(aabb.unknownData, aabb.unknownCount, 1);
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
		void walk_water(Walk walk, GfxWorldWater& water)
		{
			walk.asset(ASSET_TYPE_MATERIAL, water.material);
			walk.asset(ASSET_TYPE_WATER_SYSTEM, water.waterSystem);

			if (const auto materials = walk.array(water.materials, water.materialCount, 15))
			{
				for (auto i = 0u; i < water.materialCount; i++)
				{
					walk.asset(ASSET_TYPE_MATERIAL, materials[i]);
				}
			}

			walk.array(water.materialData, water.materialCount, 15);

			auto& tessellation = water.tessellation;
			walk.array(tessellation.data, tessellation.count, 3);
			walk.array(tessellation.data2, tessellation.count2, 3);
			walk.array(tessellation.data3, tessellation.count2 / 3, 3);

			walk.array(water.data, water.count + water.count2, 3);
		}

		template <typename Walk>
		void walk_unknown_grid(Walk walk, GfxWorldUnknownGrid& grid)
		{
			walk.asset(ASSET_TYPE_IMAGE, grid.image);
			walk.array(grid.data, grid.dims[0] * grid.dims[1] * grid.dims[2], 3);
			walk.array(grid.data2, grid.dims2[0] * grid.dims2[1] * grid.dims2[2] * 3, 3);
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

			walk_temp_array(walk, data.reflectionProbeSH, 4 * data.reflectionProbeCount, 3);
		}

		template <typename Walk>
		void walk_decal_volumes(Walk walk, GfxWorldDecalVolumes& decals)
		{
			walk_asset_array(walk, ASSET_TYPE_MATERIAL, decals.decalVolumeMaterials, decals.decalVolumeMaterialCount);
			walk_asset_array(walk, ASSET_TYPE_DECAL_VOLUME_MASK, decals.decalVolumeMasks, decals.decalVolumeMaskCount);
			walk.array(decals.decalVolumeCollections, decals.decalVolumeCollectionCount, 3);
			walk.array(decals.materialAndMaskIndirection, decals.decalVolumeMaterialCount + decals.decalVolumeMaskCount, 3);
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
		void walk_compressed_sun_shadow(Walk walk, GfxCompressedSunShadow& shadow)
		{
			walk_temp_array(walk, shadow.compressedSunShadow, shadow.compressedSunShadowSize, 0);
			walk.array(shadow.data, shadow.count, 3);
			walk.array(shadow.data2, shadow.count2, 0);
		}

		template <typename Walk>
		void walk_gfx_world(Walk walk, GfxWorld& asset)
		{
			walk.string(asset.baseName);
			walk_unknown_list(walk, asset.unknownList);
			walk_surfaces(walk, asset.surfaces);
			walk_static_models(walk, asset.smodels);
			walk_dynamic_lightset(walk, asset.dynamicLightset);
			walk_voxel_trees(walk, asset);
			walk_temp_array(walk, asset.mayhemSelfVis.selfVis, asset.mayhemSelfVis.selfVisBufferSize, 0);
			walk_frustum_lights(walk, asset.frustumLights);
			walk_light_view_frustums(walk, asset);
			walk_heightfields(walk, asset);
			walk_dpvs_static(walk, asset.dpvs);

			for (auto& image : asset.sky.images)
			{
				walk.asset(ASSET_TYPE_IMAGE, image);
			}

			walk.array(asset.gfxMapEntLookup, asset.gfxMapEntLookupCount, 3);
			walk.asset(ASSET_TYPE_MATERIAL, asset.sun.spriteMaterial);
			walk.asset(ASSET_TYPE_MATERIAL, asset.sun.flareMaterial);
			walk.asset(ASSET_TYPE_IMAGE, asset.outdoorImage);
			walk.asset(ASSET_TYPE_MATERIAL, asset.dustMaterial);
			walk_shadow_geometry(walk, asset);
			walk_light_aabb(walk, asset.lightAABB);
			walk.array(asset.umbraGates, asset.numUmbraGates, 3);
			walk.array(asset.umbraTomeData, asset.umbraTomeSize, 15);
			walk_mdao_volumes(walk, asset);
			walk_stream_tree(walk, asset.streamTree);
			walk_stream_tree_grid(walk, asset.materialStreamTreeGrid);
			walk_stream_tree_grid(walk, asset.xmodelStreamTreeGrid);
			walk_water(walk, asset.water);
			walk_unknown_grid(walk, asset.unknownGrid);
			walk.asset(ASSET_TYPE_IMAGE, asset.unknownImage);
			walk.array(asset.unknownData, asset.unknownCount, 3);
			walk.array(asset.unknownData2, asset.unknownCount2, 3);
			walk.array(asset.unknownData3, asset.unknownCount3, 0);
			walk_reflection_probes(walk, asset.reflectionProbeData);

			auto& reindex = asset.lightmapReindexData;
			walk.array(reindex.reindexElement, reindex.reindexCount, 3);
			walk.array(reindex.packedLightmap, reindex.packedLightmapCount, 3);

			walk.asset(ASSET_TYPE_IMAGE, asset.iesLookupTexture);
			walk_decal_volumes(walk, asset.decalVolumes);

			for (auto& zone : asset.transientZones)
			{
				walk.asset(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, zone);
			}

			walk.array(asset.lightmapTransientIndex, asset.lightmapCount, 3);
			walk_volumetrics(walk, asset.volumetrics);
			walk_compressed_sun_shadow(walk, asset.compressedSunShadow);
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
