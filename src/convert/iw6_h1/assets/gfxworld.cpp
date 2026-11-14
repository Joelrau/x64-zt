#include <std_include.hpp>
#include "../include.hpp"
#include "gfxworld.hpp"

#include <h1/assets/gfxworld.hpp>
#include <formats/light_grid.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace gfxworld
		{
			namespace
			{
				void convert_light_grid_tree(const GfxWorld* asset, zonetool::h1::GfxLightGridTree& tree, utils::memory::allocator& allocator)
				{
					const auto& source = asset->lightGrid.tree;
					tree.maxDepth = source.maxDepth;
					tree.nodeCount = source.nodeCount;
					tree.leafCount = source.leafCount;
					std::memcpy(tree.coordMinGridSpace, source.coordMinGridSpace, sizeof(tree.coordMinGridSpace));
					std::memcpy(tree.coordMaxGridSpace, source.coordMaxGridSpace, sizeof(tree.coordMaxGridSpace));
					std::memcpy(tree.coordHalfSizeGridSpace, source.coordHalfSizeGridSpace, sizeof(tree.coordHalfSizeGridSpace));
					tree.defaultColorIndexBitCount = source.defaultColorIndexBitCount;
					tree.defaultLightIndexBitCount = source.defaultLightIndexBitCount;

					std::vector<unsigned int> node_table;
					std::vector<unsigned char> leaf_table;
					if (!light_grid::convert_leaf_table(light_grid::make_tree_view(source, 6), 7, node_table, leaf_table))
					{
						if (source.nodeCount)
						{
							ZONETOOL_ERROR("gfxworld \"%s\": the light grid tree could not be converted", asset->name);
						}

						return;
					}

					tree.p_nodeTable = allocator.allocate_array<unsigned int>(node_table.size());
					std::memcpy(tree.p_nodeTable, node_table.data(), node_table.size() * sizeof(unsigned int));
					tree.leafTableSize = static_cast<int>(leaf_table.size());
					tree.p_leafTable = allocator.allocate_array<unsigned char>(leaf_table.size());
					std::memcpy(tree.p_leafTable, leaf_table.data(), leaf_table.size());
				}
			}

			zonetool::h1::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::GfxWorld>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_SAFE(baseName);
				new_asset->bspVersion = 111;
				COPY_VALUE(planeCount);
				COPY_VALUE(nodeCount);
				COPY_VALUE(surfaceCount);

				COPY_VALUE(skyCount);
				new_asset->skies = allocator.allocate_array<zonetool::h1::GfxSky>(asset->skyCount);
				for (int i = 0; i < asset->skyCount; i++)
				{
					if (asset->skies[i].skyImage)
					{
						new_asset->skies[i].skyImage = allocator.allocate<zonetool::h1::GfxImage>();
						new_asset->skies[i].skyImage->name = asset->skies[i].skyImage->name;
					}
					new_asset->skies[i].skySurfCount = asset->skies[i].skySurfCount;
					new_asset->skies[i].skyStartSurfs = asset->skies[i].skyStartSurfs;
					new_asset->skies[i].skySamplerState = asset->skies[i].skySamplerState;
					if (asset->skies[i].skySurfCount)
					{
						const auto index = asset->dpvs.sortedSurfIndex[asset->skies[i].skyStartSurfs[0]];
						memcpy(&new_asset->skies[i].bounds, &asset->dpvs.surfacesBounds[index].bounds, sizeof(Bounds));
					}
				}

				COPY_VALUE(lastSunPrimaryLightIndex);
				COPY_VALUE(primaryLightCount);
				COPY_VALUE(primaryLightEnvCount);
				new_asset->sortKeyLitDecal = 7;
				new_asset->sortKeyEffectDecal = 43;
				new_asset->sortKeyTopDecal = 17;
				new_asset->sortKeyEffectAuto = 53;
				new_asset->sortKeyDistortion = 48;
				new_asset->sortKeyHair = 18;
				new_asset->sortKeyEffectBlend = 33;

				COPY_VALUE(dpvsPlanes.cellCount);
				REINTERPRET_CAST_SAFE(dpvsPlanes.planes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.nodes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.sceneEntCellBits);

				REINTERPRET_CAST_SAFE(aabbTreeCounts);
				new_asset->aabbTrees = allocator.allocate_array<zonetool::h1::GfxCellTree>(asset->dpvsPlanes.cellCount);
				for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
				{
					new_asset->aabbTreeCounts[i].aabbTreeCount = asset->aabbTreeCounts[i].aabbTreeCount;
					new_asset->aabbTrees[i].aabbTree = allocator.allocate_array<zonetool::h1::GfxAabbTree>(asset->aabbTreeCounts[i].aabbTreeCount);
					for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
					{
						memcpy(&new_asset->aabbTrees[i].aabbTree[j].bounds, &asset->aabbTrees[i].aabbTree[j].bounds, sizeof(float[2][3]));

						new_asset->aabbTrees[i].aabbTree[j].startSurfIndex = asset->aabbTrees[i].aabbTree[j].startSurfIndex;
						new_asset->aabbTrees[i].aabbTree[j].surfaceCount = asset->aabbTrees[i].aabbTree[j].surfaceCount;

						new_asset->aabbTrees[i].aabbTree[j].smodelIndexCount = asset->aabbTrees[i].aabbTree[j].smodelIndexCount;
						REINTERPRET_CAST_SAFE_TO_FROM(new_asset->aabbTrees[i].aabbTree[j].smodelIndexes, asset->aabbTrees[i].aabbTree[j].smodelIndexes);

						new_asset->aabbTrees[i].aabbTree[j].childCount = asset->aabbTrees[i].aabbTree[j].childCount;
						// re-calculate childrenOffset
						auto offset = asset->aabbTrees[i].aabbTree[j].childrenOffset;
						if (offset)
						{
							int childrenIndex = offset / sizeof(GfxAabbTree);
							int childrenOffset = childrenIndex * sizeof(zonetool::h1::GfxAabbTree);
							new_asset->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
						}
					}
				}

				new_asset->cells = allocator.allocate_array<zonetool::h1::GfxCell>(new_asset->dpvsPlanes.cellCount);
				for (int i = 0; i < new_asset->dpvsPlanes.cellCount; i++)
				{
					memcpy(&new_asset->cells[i].bounds, &asset->cells[i].bounds, sizeof(float[2][3]));
					new_asset->cells[i].portalCount = static_cast<short>(asset->cells[i].portalCount);

					REINTERPRET_CAST_SAFE_TO_FROM(new_asset->cells[i].portals, asset->cells[i].portals);

					new_asset->cells[i].reflectionProbeCount = asset->cells[i].reflectionProbeCount;
					new_asset->cells[i].reflectionProbes = asset->cells[i].reflectionProbes;
					new_asset->cells[i].reflectionProbeReferenceCount = asset->cells[i].reflectionProbeReferenceCount;
					new_asset->cells[i].reflectionProbeReferences = asset->cells[i].reflectionProbeReferences;
				}

				new_asset->portalGroupCount = 0;
				new_asset->portalGroup = nullptr;

				new_asset->portalDistanceAnchorCount = 0;
				new_asset->portalDistanceAnchorsAndCloseDistSquared = nullptr;

				COPY_VALUE(draw.reflectionProbeCount);

				REINTERPRET_CAST_SAFE(draw.reflectionProbes);

				new_asset->draw.reflectionProbeOrigins = allocator.allocate_array<zonetool::h1::GfxReflectionProbe>(asset->draw.reflectionProbeCount);
				for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
				{
					memcpy(new_asset->draw.reflectionProbeOrigins[i].origin,
						asset->draw.reflectionProbeOrigins[i].origin,
						sizeof(float[3]));

					new_asset->draw.reflectionProbeOrigins[i].probeVolumeCount = 0;
					new_asset->draw.reflectionProbeOrigins[i].probeVolumes = nullptr;
				}

				new_asset->draw.reflectionProbeTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(asset->draw.reflectionProbeCount); // todo?
				COPY_VALUE(draw.reflectionProbeReferenceCount);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeReferenceOrigins);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeReferences);
				COPY_VALUE(draw.lightmapCount);
				REINTERPRET_CAST_SAFE(draw.lightmaps);
				new_asset->draw.lightmapPrimaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(asset->draw.lightmapCount); // todo?
				new_asset->draw.lightmapSecondaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(asset->draw.lightmapCount); // todo?
				REINTERPRET_CAST_SAFE(draw.lightmapOverridePrimary);
				REINTERPRET_CAST_SAFE(draw.lightmapOverrideSecondary);
				new_asset->draw.lightmapParameters.lightmapWidthPrimary = 1024;
				new_asset->draw.lightmapParameters.lightmapHeightPrimary = 1024;
				new_asset->draw.lightmapParameters.lightmapWidthSecondary = 512;
				new_asset->draw.lightmapParameters.lightmapHeightSecondary = 512;
				new_asset->draw.lightmapParameters.lightmapModelUnitsPerTexel = 8;
				COPY_VALUE(draw.trisType); // convert?
				COPY_VALUE(draw.vertexCount);
				REINTERPRET_CAST_SAFE(draw.vd.vertices);
				COPY_VALUE(draw.vertexLayerDataSize);
				REINTERPRET_CAST_SAFE(draw.vld.data);
				COPY_VALUE(draw.indexCount);
				REINTERPRET_CAST_SAFE(draw.indices);
				new_asset->draw.displacementParmsCount = 0;
				new_asset->draw.displacementParms = nullptr;

				COPY_VALUE(lightGrid.hasLightRegions);
				COPY_VALUE(lightGrid.useSkyForLowZ);
				COPY_VALUE(lightGrid.lastSunPrimaryLightIndex);
				COPY_ARR(lightGrid.mins);
				COPY_ARR(lightGrid.maxs);
				COPY_VALUE(lightGrid.rowAxis);
				COPY_VALUE(lightGrid.colAxis);
				REINTERPRET_CAST_SAFE(lightGrid.rowDataStart);
				COPY_VALUE(lightGrid.rawRowDataSize);
				REINTERPRET_CAST_SAFE(lightGrid.rawRowData);
				COPY_VALUE(lightGrid.entryCount);
				REINTERPRET_CAST_SAFE(lightGrid.entries);
				COPY_VALUE(lightGrid.colorCount);
				new_asset->lightGrid.colors = allocator.allocate_array<zonetool::h1::GfxLightGridColors>(asset->lightGrid.colorCount);
				for (auto i = 0u; asset->lightGrid.colors && i < asset->lightGrid.colorCount; i++)
				{
					for (auto j = 0u; j < light_grid::basis_dir_count; j++)
					{
						for (auto k = 0u; k < 3; k++)
						{
							new_asset->lightGrid.colors[i].rgb[j][k] = half_float::float_to_half(asset->lightGrid.colors[i].rgb[j][k] / 255.f);
						}
					}
				}

				memset(&new_asset->lightGrid.cachedSkyLightGridEntry, 0, sizeof(new_asset->lightGrid.cachedSkyLightGridEntry));
				memset(&new_asset->lightGrid.cachedSkyLightEntryAverageColor, 0, sizeof(new_asset->lightGrid.cachedSkyLightEntryAverageColor));

				COPY_VALUE(lightGrid.missingGridColorIndex);
				COPY_VALUE(lightGrid.tableVersion);
				COPY_VALUE(lightGrid.paletteVersion);
				COPY_VALUE(lightGrid.rangeExponent8BitsEncoding);
				COPY_VALUE(lightGrid.rangeExponent12BitsEncoding);
				COPY_VALUE(lightGrid.rangeExponent16BitsEncoding);

				COPY_VALUE(lightGrid.stageCount);
				REINTERPRET_CAST_SAFE(lightGrid.stageLightingContrastGain);

				COPY_VALUE(lightGrid.paletteEntryCount);
				REINTERPRET_CAST_SAFE(lightGrid.paletteEntryAddress);
				COPY_VALUE(lightGrid.paletteBitstreamSize);
				REINTERPRET_CAST_SAFE(lightGrid.paletteBitstream);

				for (unsigned int i = 0; i < 56; i++)
				{
					for (unsigned int j = 0; j < 3; j++)
					{
						new_asset->lightGrid.defaultLightGridColors.rgb[i][j] = half_float::float_to_half(asset->lightGrid.defaultLightGridColors.rgb[i][j]);
					}
				}

				for (unsigned int i = 0; i < 56; i++)
				{
					for (unsigned int j = 0; j < 3; j++)
					{
						new_asset->lightGrid.skyLightGridColors.rgb[i][j] = half_float::float_to_half(asset->lightGrid.skyLightGridColors.rgb[i][j]);
					}
				}

				for (auto i = 0; i < 3; i++)
				{
					new_asset->lightGrid.tree[i].index = static_cast<unsigned char>(i);
				}

				convert_light_grid_tree(asset, new_asset->lightGrid.tree[0], allocator);

				COPY_VALUE(modelCount);
				new_asset->models = allocator.allocate_array<zonetool::h1::GfxBrushModel>(asset->modelCount);
				for (int i = 0; i < asset->modelCount; i++)
				{
					memset(&new_asset->models[i].writable, 0, sizeof(zonetool::h1::GfxBrushModelWritable));
					memcpy(&new_asset->models[i].bounds, &asset->models[i].bounds, sizeof(Bounds));
					new_asset->models[i].radius = asset->models[i].radius;
					new_asset->models[i].startSurfIndex = asset->models[i].startSurfIndex;
					new_asset->models[i].surfaceCount = asset->models[i].surfaceCount;
					new_asset->models[i].mdaoVolumeIndex = -1;
				}

				memcpy(&new_asset->bounds, &asset->bounds, sizeof(Bounds));
				memcpy(&new_asset->shadowBounds, &asset->bounds, sizeof(Bounds));

				COPY_VALUE(checksum);
				COPY_VALUE(materialMemoryCount);
				REINTERPRET_CAST_SAFE(materialMemory);
				COPY_VALUE_CAST(sun);
				COPY_ARR(outdoorLookupMatrix);
				REINTERPRET_CAST_SAFE(outdoorImage);
				REINTERPRET_CAST_SAFE(cellCasterBits);
				REINTERPRET_CAST_SAFE(cellHasSunLitSurfsBits);
				REINTERPRET_CAST_SAFE(sceneDynModel);
				REINTERPRET_CAST_SAFE(sceneDynBrush);
				REINTERPRET_CAST_SAFE(primaryLightEntityShadowVis);
				REINTERPRET_CAST_SAFE_ARR(primaryLightDynEntShadowVis, 2);
				REINTERPRET_CAST_SAFE(nonSunPrimaryLightForModelDynEnt);
				REINTERPRET_CAST_SAFE(shadowGeom);
				REINTERPRET_CAST_SAFE(shadowGeomOptimized);
				REINTERPRET_CAST_SAFE(lightRegion);

				// dpvs
				{
					new_asset->dpvs.subdivVertexLightingInfoCount = 0;
					new_asset->dpvs.subdivVertexLighting = nullptr;

					COPY_VALUE(dpvs.smodelCount);
					COPY_VALUE(dpvs.staticSurfaceCount);
					COPY_VALUE(dpvs.litOpaqueSurfsBegin);
					COPY_VALUE(dpvs.litOpaqueSurfsEnd);
					new_asset->dpvs.unkSurfsBegin = 0;
					new_asset->dpvs.unkSurfsEnd = 0;
					COPY_VALUE(dpvs.litDecalSurfsBegin);
					COPY_VALUE(dpvs.litDecalSurfsEnd);
					COPY_VALUE(dpvs.litTransSurfsBegin);
					COPY_VALUE(dpvs.litTransSurfsEnd);
					COPY_VALUE(dpvs.shadowCasterSurfsBegin);
					COPY_VALUE(dpvs.shadowCasterSurfsEnd);
					COPY_VALUE(dpvs.emissiveSurfsBegin);
					COPY_VALUE(dpvs.emissiveSurfsEnd);
					COPY_VALUE(dpvs.smodelVisDataCount);
					COPY_VALUE(dpvs.surfaceVisDataCount);

					for (auto i = 0; i < 4; i++)
					{
						new_asset->dpvs.smodelVisData[i] = allocator.allocate_array<unsigned int>(new_asset->dpvs.smodelVisDataCount);
					}

					for (auto i = 0; i < 4; i++)
					{
						new_asset->dpvs.surfaceVisData[i] = allocator.allocate_array<unsigned int>(new_asset->dpvs.surfaceVisDataCount);
					}

					for (auto i = 0; i < 27; i++)
					{
						new_asset->dpvs.smodelUnknownVisData[i] = allocator.allocate_array<unsigned int>(new_asset->dpvs.smodelVisDataCount);
					}

					for (auto i = 0; i < 27; i++)
					{
						new_asset->dpvs.surfaceUnknownVisData[i] = allocator.allocate_array<unsigned int>(new_asset->dpvs.surfaceVisDataCount);
					}

					for (auto i = 0; i < 4; i++)
					{
						new_asset->dpvs.smodelUmbraVisData[i] = allocator.allocate_array<unsigned int>(new_asset->dpvs.smodelVisDataCount);
					}

					for (auto i = 0; i < 4; i++)
					{
						new_asset->dpvs.surfaceUmbraVisData[i] = allocator.allocate_array<unsigned int>(new_asset->dpvs.surfaceVisDataCount);
					}

					new_asset->dpvs.unknownSModelVisData1 = allocator.allocate_array<unsigned int>(new_asset->dpvs.smodelVisDataCount);
					new_asset->dpvs.unknownSModelVisData2 = allocator.allocate_array<unsigned int>(new_asset->dpvs.smodelVisDataCount * 2);

					new_asset->dpvs.lodData = asset->dpvs.lodData;

					new_asset->dpvs.tessellationCutoffVisData = allocator.allocate_array<unsigned int>(new_asset->dpvs.surfaceVisDataCount);

					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);

					new_asset->dpvs.surfaces = allocator.allocate_array<zonetool::h1::GfxSurface>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						COPY_VALUE(dpvs.surfaces[i].tris.vertexLayerData);
						COPY_VALUE(dpvs.surfaces[i].tris.firstVertex);
						COPY_VALUE(dpvs.surfaces[i].tris.maxEdgeLength);
						new_asset->dpvs.surfaces[i].tris.unk = -1;
						COPY_VALUE(dpvs.surfaces[i].tris.vertexCount);
						COPY_VALUE(dpvs.surfaces[i].tris.triCount);
						COPY_VALUE(dpvs.surfaces[i].tris.baseIndex);
						new_asset->dpvs.surfaces[i].material = reinterpret_cast<zonetool::h1::Material*>(asset->dpvs.surfaces[i].material);
						COPY_VALUE(dpvs.surfaces[i].laf.fields.lightmapIndex);
						COPY_VALUE(dpvs.surfaces[i].laf.fields.reflectionProbeIndex);
						COPY_VALUE(dpvs.surfaces[i].laf.fields.primaryLightEnvIndex);
						COPY_VALUE(dpvs.surfaces[i].laf.fields.flags); // convert?
					}

					REINTERPRET_CAST_SAFE(dpvs.surfacesBounds);

					std::vector<zonetool::h1::GfxSubdivVertexLightingInfo> subdiv_vertex_lighting_infos;
					new_asset->dpvs.smodelDrawInsts = allocator.allocate_array<zonetool::h1::GfxStaticModelDrawInst>(asset->dpvs.smodelCount);
					new_asset->dpvs.smodelLightingInsts = allocator.allocate_array<zonetool::h1::GfxStaticModelLighting>(asset->dpvs.smodelCount);
					for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
					{
						memcpy(&new_asset->dpvs.smodelDrawInsts[i].placement, &asset->dpvs.smodelDrawInsts[i].placement, sizeof(zonetool::h1::GfxPackedPlacement));
						new_asset->dpvs.smodelDrawInsts[i].model = reinterpret_cast<zonetool::h1::XModel*>(asset->dpvs.smodelDrawInsts[i].model);
						new_asset->dpvs.smodelDrawInsts[i].lightingHandle = asset->dpvs.smodelDrawInsts[i].lightingHandle;
						new_asset->dpvs.smodelDrawInsts[i].staticModelId = asset->dpvs.smodelDrawInsts[i].staticModelId;
						new_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex;
						new_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
						new_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
						new_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = asset->dpvs.smodelDrawInsts[i].sunShadowFlags;

						new_asset->dpvs.smodelDrawInsts[i].cullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
						new_asset->dpvs.smodelDrawInsts[i].reactiveMotionCullDist = new_asset->dpvs.smodelDrawInsts[i].cullDist;
						new_asset->dpvs.smodelDrawInsts[i].reactiveMotionLOD = 0;

						new_asset->dpvs.smodelDrawInsts[i].flags = static_cast<unsigned short>((asset->dpvs.smodelDrawInsts[i].flags & ~(zonetool::h1::STATIC_MODEL_FLAG_SCALE_MODIFIER_MASK | zonetool::h1::STATIC_MODEL_FLAG_ANIMATED_VERTS))
							| zonetool::h1::STATIC_MODEL_FLAG_SCALE_6);

						if ((new_asset->dpvs.smodelDrawInsts[i].flags & zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_REACTIVEMOTION) != 0)
						{
							new_asset->dpvs.smodelDrawInsts[i].reactiveMotionLOD = 6;
						}

						if (asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.cache.size || asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.flags)
						{
							new_asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.subdivVertexLightingInfoIndex =
								static_cast<int>(subdiv_vertex_lighting_infos.size());

							zonetool::h1::GfxSubdivVertexLightingInfo subdiv_info{};
							subdiv_info.vertexLightingIndex = static_cast<int>(i);
							subdiv_info.flags = asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.flags;
							subdiv_info.cache.size = asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.cache.size;
							subdiv_vertex_lighting_infos.push_back(subdiv_info);
						}
					}

					for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
					{
						if ((new_asset->dpvs.smodelDrawInsts[i].flags & zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) != 0)
						{
							new_asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.numLightingValues =
								asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.numLightingValues;
							REINTERPRET_CAST_SAFE_TO_FROM(new_asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.lightingValues,
								asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues);
						}
						else if ((new_asset->dpvs.smodelDrawInsts[i].flags & zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING) != 0)
						{
							new_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[0] =
								half_float::float_to_half(asset->dpvs.smodelDrawInsts[i].groundLighting[0]);
							new_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[1] =
								half_float::float_to_half(asset->dpvs.smodelDrawInsts[i].groundLighting[1]);
							new_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[2] =
								half_float::float_to_half(asset->dpvs.smodelDrawInsts[i].groundLighting[2]);
							new_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[3] =
								half_float::float_to_half(asset->dpvs.smodelDrawInsts[i].groundLighting[3]);
						}
						else if ((new_asset->dpvs.smodelDrawInsts[i].flags & zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING) != 0)
						{
							memcpy(&new_asset->dpvs.smodelLightingInsts[i].modelLightmapInfo,
								&asset->dpvs.smodelDrawInsts[i].modelLightmapInfo, sizeof(GfxStaticModelLightmapInfo));
						}
					}

					new_asset->dpvs.subdivVertexLightingInfoCount = static_cast<unsigned int>(subdiv_vertex_lighting_infos.size());
					new_asset->dpvs.subdivVertexLighting = allocator.allocate_array<zonetool::h1::GfxSubdivVertexLightingInfo>(new_asset->dpvs.subdivVertexLightingInfoCount);
					for (unsigned int i = 0; i < new_asset->dpvs.subdivVertexLightingInfoCount; i++)
					{
						new_asset->dpvs.subdivVertexLighting[i] = subdiv_vertex_lighting_infos[i];
					}

					new_asset->dpvs.surfaceMaterials = allocator.allocate_array<zonetool::h1::GfxDrawSurf>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++) // runtime data
					{
						new_asset->dpvs.surfaceMaterials[i].fields.objectId = asset->dpvs.surfaceMaterials[i].fields.objectId;
						new_asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex = asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex;
						new_asset->dpvs.surfaceMaterials[i].fields.hasGfxEntIndex = asset->dpvs.surfaceMaterials[i].fields.hasGfxEntIndex;
						new_asset->dpvs.surfaceMaterials[i].fields.customIndex = asset->dpvs.surfaceMaterials[i].fields.customIndex;
						new_asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex = asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex;
						new_asset->dpvs.surfaceMaterials[i].fields.tessellation = asset->dpvs.surfaceMaterials[i].fields.tessellation;
						new_asset->dpvs.surfaceMaterials[i].fields.prepass = asset->dpvs.surfaceMaterials[i].fields.prepass;
						new_asset->dpvs.surfaceMaterials[i].fields.useHeroLighting = asset->dpvs.surfaceMaterials[i].fields.useHeroLighting;
						new_asset->dpvs.surfaceMaterials[i].fields.sceneLightEnvIndex = asset->dpvs.surfaceMaterials[i].fields.sceneLightEnvIndex;
						new_asset->dpvs.surfaceMaterials[i].fields.viewModelRender = asset->dpvs.surfaceMaterials[i].fields.viewModelRender;
						new_asset->dpvs.surfaceMaterials[i].fields.surfType = asset->dpvs.surfaceMaterials[i].fields.surfType;
						new_asset->dpvs.surfaceMaterials[i].fields.primarySortKey = asset->dpvs.surfaceMaterials[i].fields.primarySortKey;
						new_asset->dpvs.surfaceMaterials[i].fields.unused = asset->dpvs.surfaceMaterials[i].fields.unused;
					}

					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
					COPY_VALUE(dpvs.sunShadowOptCount);
					COPY_VALUE(dpvs.sunSurfVisDataCount);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
					new_asset->dpvs.surfaceDeptAndSurf = allocator.allocate_array<zonetool::h1::GfxDepthAndSurf>(new_asset->dpvs.staticSurfaceCount); // runtime data
					REINTERPRET_CAST_SAFE(dpvs.constantBuffersLit);
					REINTERPRET_CAST_SAFE(dpvs.constantBuffersAmbient);

					COPY_VALUE(dpvs.usageCount);
				}

				new_asset->dpvsDyn.dynEntClientWordCount[0] = asset->dpvsDyn.dynEntClientWordCount[0];
				new_asset->dpvsDyn.dynEntClientWordCount[1] = asset->dpvsDyn.dynEntClientWordCount[1];
				new_asset->dpvsDyn.dynEntClientCount[0] = asset->dpvsDyn.dynEntClientCount[0];
				new_asset->dpvsDyn.dynEntClientCount[1] = asset->dpvsDyn.dynEntClientCount[1];
				new_asset->dpvsDyn.dynEntCellBits[0] = reinterpret_cast<unsigned int*>(asset->dpvsDyn.dynEntCellBits[0]);
				new_asset->dpvsDyn.dynEntCellBits[1] = reinterpret_cast<unsigned int*>(asset->dpvsDyn.dynEntCellBits[1]);
				new_asset->dpvsDyn.dynEntVisData[0][0] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[0][0]);
				new_asset->dpvsDyn.dynEntVisData[0][1] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[0][1]);
				new_asset->dpvsDyn.dynEntVisData[0][2] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[0][2]);
				new_asset->dpvsDyn.dynEntVisData[0][3] = allocator.allocate_array<unsigned char>(new_asset->dpvsDyn.dynEntClientWordCount[0] * 32);
				new_asset->dpvsDyn.dynEntVisData[1][0] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[1][0]);
				new_asset->dpvsDyn.dynEntVisData[1][1] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[1][1]);
				new_asset->dpvsDyn.dynEntVisData[1][2] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[1][2]);
				new_asset->dpvsDyn.dynEntVisData[1][3] = allocator.allocate_array<unsigned char>(new_asset->dpvsDyn.dynEntClientWordCount[1] * 32);

				COPY_VALUE(mapVtxChecksum);
				COPY_VALUE(heroOnlyLightCount);
				REINTERPRET_CAST_SAFE(heroOnlyLights);
				new_asset->fogTypesAllowed = zonetool::h1::FOG_NORMAL | zonetool::h1::FOG_DFOG; //COPY_VALUE(fogTypesAllowed);
				COPY_VALUE(umbraTomeSize);
				REINTERPRET_CAST_SAFE(umbraTomeData);
				COPY_VALUE_CAST(umbraTomePtr);
				new_asset->mdaoVolumesCount = 0;
				new_asset->mdaoVolumes = nullptr;

				new_asset->useLightGridDefaultModelLightingLookup = true;
				new_asset->useLightGridDefaultFXLightingLookup = true;
				memset(&new_asset->lightGridDefaultModelLightingLookup, 0, sizeof(new_asset->lightGridDefaultModelLightingLookup));
				memset(&new_asset->lightGridDefaultFXLightingLookup, 0, sizeof(new_asset->lightGridDefaultFXLightingLookup));

				new_asset->buildInfo.bspCommandline = nullptr;
				new_asset->buildInfo.lightCommandline = nullptr;
				new_asset->buildInfo.bspTimestamp = nullptr;
				new_asset->buildInfo.lightTimestamp = nullptr;

				return new_asset;
			}

			void dump(GfxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::gfx_world::dump(converted_asset);

#ifdef GENERATE_SUN_FILE
				dump_sun_file(asset->sun, utils::string::va("sun/%s.sun", asset->baseName));
#endif
			}
		}
	}
}