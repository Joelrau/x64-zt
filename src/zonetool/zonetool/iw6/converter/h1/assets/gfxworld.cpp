#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
#include "zonetool/iw6/converter/h1/include.hpp"
#include "gfxworld.hpp"

#include "zonetool/h1/assets/gfxworld.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace gfxworld
		{
			zonetool::h1::GfxWorld* convert(GfxWorld* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::GfxWorld>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_SAFE(baseName);
				new_asset->bspVersion = 111;
				COPY_VALUE(planeCount);
				COPY_VALUE(nodeCount);
				COPY_VALUE(surfaceCount);

				COPY_VALUE(skyCount);
				new_asset->skies = mem->Alloc<zonetool::h1::GfxSky>(asset->skyCount);
				for (int i = 0; i < asset->skyCount; i++)
				{
					if (asset->skies[i].skyImage)
					{
						new_asset->skies[i].skyImage = mem->Alloc<zonetool::h1::GfxImage>();
						new_asset->skies[i].skyImage->name = asset->skies[i].skyImage->name;
					}
					new_asset->skies[i].skySurfCount = asset->skies[i].skySurfCount;
					new_asset->skies[i].skyStartSurfs = asset->skies[i].skyStartSurfs;
					new_asset->skies[i].skySamplerState = asset->skies[i].skySamplerState;
					// add bounds
					for (auto j = 0; j < asset->skies[i].skySurfCount; j++)
					{
						auto index = asset->dpvs.sortedSurfIndex[asset->skies[i].skyStartSurfs[j]];
						auto* surface_bounds = &asset->dpvs.surfacesBounds[index];
						memcpy(&new_asset->skies[i].bounds, &surface_bounds->bounds, sizeof(surface_bounds->bounds));

						if(1)
						break;
					}
				}


				new_asset->portalGroupCount = 0;
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
				new_asset->aabbTrees = mem->Alloc<zonetool::h1::GfxCellTree>(asset->dpvsPlanes.cellCount);
				for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
				{
					new_asset->aabbTreeCounts[i].aabbTreeCount = asset->aabbTreeCounts[i].aabbTreeCount;
					new_asset->aabbTrees[i].aabbTree = mem->Alloc<zonetool::h1::GfxAabbTree>(asset->aabbTreeCounts[i].aabbTreeCount);
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
						int childrenIndex = offset / sizeof(GfxAabbTree);
						int childrenOffset = childrenIndex * sizeof(zonetool::h1::GfxAabbTree);
						new_asset->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
					}
				}

				new_asset->cells = mem->Alloc<zonetool::h1::GfxCell>(new_asset->dpvsPlanes.cellCount);
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

				new_asset->portalGroup = nullptr;

				new_asset->unk_vec4_count_0 = 0;
				new_asset->unk_vec4_0 = nullptr;

				COPY_VALUE(draw.reflectionProbeCount);
				REINTERPRET_CAST_SAFE(draw.reflectionProbes);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeOrigins);
				new_asset->draw.reflectionProbeTextures = mem->Alloc<zonetool::h1::GfxRawTexture>(asset->draw.reflectionProbeCount); // todo?
				COPY_VALUE(draw.reflectionProbeReferenceCount);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeReferenceOrigins);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeReferences);
				COPY_VALUE(draw.lightmapCount);
				REINTERPRET_CAST_SAFE(draw.lightmaps);
				new_asset->draw.lightmapPrimaryTextures = mem->Alloc<zonetool::h1::GfxRawTexture>(asset->draw.lightmapCount); // todo?
				new_asset->draw.lightmapSecondaryTextures = mem->Alloc<zonetool::h1::GfxRawTexture>(asset->draw.lightmapCount); // todo?
				REINTERPRET_CAST_SAFE(draw.lightmapOverridePrimary);
				REINTERPRET_CAST_SAFE(draw.lightmapOverrideSecondary);
				new_asset->draw.u1[0] = 1024; new_asset->draw.u1[1] = 1024; // u1
				new_asset->draw.u2[0] = 512; new_asset->draw.u2[1] = 512; // u2
				new_asset->draw.u3 = 8; // u3
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
				if (asset->lightGrid.colors)
				{
					new_asset->lightGrid.colors = mem->Alloc<zonetool::h1::GfxLightGridColors>(asset->lightGrid.colorCount);
					// todo?
				}
				memset(new_asset->lightGrid.__pad0, 0, sizeof(new_asset->lightGrid.__pad0)); // unknown data

				// --experimental--

				new_asset->lightGrid.tableVersion = 1;
				new_asset->lightGrid.paletteVersion = 1;
				new_asset->lightGrid.paletteEntryCount = asset->lightGrid.entryCount;
				new_asset->lightGrid.paletteEntryAddress = mem->Alloc<int>(new_asset->lightGrid.paletteEntryCount);
				for (unsigned int i = 0; i < new_asset->lightGrid.paletteEntryCount; i++)
				{
					new_asset->lightGrid.paletteEntryAddress[i] = 30; // 0, 30, 86, 116 //asset->lightGrid.entries[i].colorsIndex;
				}

				// mp_character_room palettebitstream
				std::uint8_t paletteBitStream[] =
				{
				8, 33, 195, 128, 128, 124, 128, 128, 128, 129, 128, 199, 128, 128, 124, 128, 128, 128, 129, 128, 216, 128, 128, 124, 128, 128, 128, 129, 128, 0, 8, 33, 187, 128, 128, 130, 128, 128, 128, 128, 128, 191, 128, 128, 130, 128, 128, 128, 128, 128, 209, 128, 128, 130, 128, 128, 128, 128, 128, 0, 0, 0, 82, 80, 18, 64, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 214, 106, 18, 64, 1, 0, 0, 0, 8, 33, 195, 128, 128, 124, 128, 128, 128, 129, 128, 199, 128, 128, 124, 128, 128, 128, 129, 128, 216, 128, 128, 124, 128, 128, 128, 129, 128, 0, 198, 24, 241, 248, 8, 128, 128, 128, 128, 90, 128, 241, 8, 128, 248, 128, 128, 128, 147, 184, 241, 128, 248, 8, 128, 128, 128, 147, 72, 0,
				};

				new_asset->lightGrid.paletteBitstreamSize = sizeof(paletteBitStream);
				new_asset->lightGrid.paletteBitstream = mem->Alloc<unsigned char>(new_asset->lightGrid.paletteBitstreamSize);
				memcpy(new_asset->lightGrid.paletteBitstream, paletteBitStream, sizeof(paletteBitStream));

				new_asset->lightGrid.missingGridColorIndex = new_asset->lightGrid.paletteEntryCount - 1;

				new_asset->lightGrid.rangeExponent8BitsEncoding = 0;
				new_asset->lightGrid.rangeExponent12BitsEncoding = 4;
				new_asset->lightGrid.rangeExponent16BitsEncoding = 23;

				new_asset->lightGrid.stageCount = 1;
				new_asset->lightGrid.stageLightingContrastGain = mem->Alloc<float>(1);
				new_asset->lightGrid.stageLightingContrastGain[0] = 0.3f;

				for (char i = 0; i < 3; i++)
				{
					new_asset->lightGrid.tree[i].maxDepth = i;
				}

				// ----

				/*COPY_VALUE(lightGrid.missingGridColorIndex);
				COPY_VALUE(lightGrid.tableVersion);
				COPY_VALUE(lightGrid.paletteVersion);
				COPY_VALUE(lightGrid.rangeExponent8BitsEncoding);
				COPY_VALUE(lightGrid.rangeExponent12BitsEncoding);
				COPY_VALUE(lightGrid.rangeExponent16BitsEncoding);
				COPY_VALUE(lightGrid.stageCount);

				GfxLightGrid;
				static_assert(offsetof(zonetool::h1::GfxLightGrid, paletteVersion) == 116);

				REINTERPRET_CAST_SAFE(lightGrid.stageLightingContrastGain);
				COPY_VALUE(lightGrid.paletteEntryCount);
				REINTERPRET_CAST_SAFE(lightGrid.paletteEntryAddress);
				COPY_VALUE(lightGrid.paletteBitstreamSize);
				REINTERPRET_CAST_SAFE(lightGrid.paletteBitstream);*/
				/*for (auto i = 0; i < 56; i++)
				{
					for (auto j = 0; j < 6; j++) // 56[3] -> 56[6] (float->char)
					{
						new_asset->lightGrid.skyLightGridColors.colorVec6[i][j] = asset->lightGrid.skyLightGridColors.colorVec3[i][j];
						if (j > 2)
						{
							new_asset->lightGrid.skyLightGridColors.colorVec6[i][j] = 0;
						}
					}
				}
				for (auto i = 0; i < 56; i++)
				{
					for (auto j = 0; j < 6; j++) // 56[3] -> 56[6] (float->char)
					{
						new_asset->lightGrid.defaultLightGridColors.colorVec6[i][j] = asset->lightGrid.defaultLightGridColors.colorVec3[i][j];
						if (j > 2)
						{
							new_asset->lightGrid.defaultLightGridColors.colorVec6[i][j] = 0;
						}
					}
				}*/
				for (char i = 0; i < 3; i++)
				{
					if (i != 0)
					{
						new_asset->lightGrid.tree[i].maxDepth = i;
					}
					else
					{
						/*new_asset->lightGrid.tree[i].maxDepth = asset->lightGrid.tree.maxDepth;
						new_asset->lightGrid.tree[i].nodeCount = asset->lightGrid.tree.nodeCount;
						new_asset->lightGrid.tree[i].leafCount = asset->lightGrid.tree.leafCount;
						memcpy(new_asset->lightGrid.tree[i].coordMinGridSpace, asset->lightGrid.tree.coordMinGridSpace, sizeof(int[3]));
						memcpy(new_asset->lightGrid.tree[i].coordMaxGridSpace, asset->lightGrid.tree.coordMaxGridSpace, sizeof(int[3]));
						memcpy(new_asset->lightGrid.tree[i].coordHalfSizeGridSpace, asset->lightGrid.tree.coordHalfSizeGridSpace, sizeof(int[3]));
						new_asset->lightGrid.tree[i].defaultColorIndexBitCount = asset->lightGrid.tree.defaultColorIndexBitCount;
						new_asset->lightGrid.tree[i].defaultLightIndexBitCount = asset->lightGrid.tree.defaultLightIndexBitCount;
						new_asset->lightGrid.tree[i].p_nodeTable = asset->lightGrid.tree.p_nodeTable;
						new_asset->lightGrid.tree[i].leafTableSize = asset->lightGrid.tree.leafTableSize;
						new_asset->lightGrid.tree[i].p_leafTable = asset->lightGrid.tree.p_leafTable;*/
					}
				}

				COPY_VALUE(modelCount);
				new_asset->models = mem->Alloc<zonetool::h1::GfxBrushModel>(asset->modelCount);
				for (int i = 0; i < asset->modelCount; i++)
				{
					memset(&new_asset->models[i].writable, 0, sizeof(zonetool::h1::GfxBrushModelWritable));
					memcpy(&new_asset->models[i].bounds, &asset->models[i].bounds, sizeof(zonetool::h1::Bounds));
					new_asset->models[i].startSurfIndex = asset->models[i].startSurfIndex;
					new_asset->models[i].surfaceCount = asset->models[i].surfaceCount;
					new_asset->models[i].mdaoVolumeIndex = -1;
				}

				memcpy(&new_asset->unkBounds, &asset->bounds, sizeof(zonetool::h1::Bounds));
				memcpy(&new_asset->shadowBounds, &asset->bounds, sizeof(zonetool::h1::Bounds));

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
						new_asset->dpvs.smodelVisData[i] = mem->Alloc<unsigned int>(new_asset->dpvs.smodelVisDataCount);
					}

					for (auto i = 0; i < 4; i++)
					{
						new_asset->dpvs.surfaceVisData[i] = mem->Alloc<unsigned int>(new_asset->dpvs.surfaceVisDataCount);
					}

					for (auto i = 0; i < 27; i++)
					{
						new_asset->dpvs.smodelUnknownVisData[i] = mem->Alloc<unsigned int>(new_asset->dpvs.smodelVisDataCount);
					}

					for (auto i = 0; i < 27; i++)
					{
						new_asset->dpvs.surfaceUnknownVisData[i] = mem->Alloc<unsigned int>(new_asset->dpvs.surfaceVisDataCount);
					}

					for (auto i = 0; i < 4; i++)
					{
						new_asset->dpvs.smodelUmbraVisData[i] = mem->Alloc<unsigned int>(new_asset->dpvs.smodelVisDataCount);
					}

					for (auto i = 0; i < 4; i++)
					{
						new_asset->dpvs.surfaceUmbraVisData[i] = mem->Alloc<unsigned int>(new_asset->dpvs.surfaceVisDataCount);
					}

					new_asset->dpvs.unknownSModelVisData1 = mem->Alloc<unsigned int>(new_asset->dpvs.smodelVisDataCount);
					new_asset->dpvs.unknownSModelVisData2 = mem->Alloc<unsigned int>(new_asset->dpvs.smodelVisDataCount * 2);

					new_asset->dpvs.lodData = mem->Alloc<unsigned int>(new_asset->dpvs.smodelCount + 1);
					new_asset->dpvs.tessellationCutoffVisData = mem->Alloc<unsigned int>(new_asset->dpvs.surfaceVisDataCount);

					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);
					REINTERPRET_CAST_SAFE(dpvs.surfaces);

					new_asset->dpvs.surfaces = mem->Alloc<zonetool::h1::GfxSurface>(asset->surfaceCount);
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
						COPY_ARR(dpvs.surfaces[i].laf);
					}

					new_asset->dpvs.surfacesBounds = mem->Alloc<zonetool::h1::GfxSurfaceBounds>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						COPY_ARR(dpvs.surfacesBounds[i].bounds);
						// unk data
					}

					new_asset->dpvs.smodelDrawInsts = mem->Alloc<zonetool::h1::GfxStaticModelDrawInst>(asset->dpvs.smodelCount);
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
						new_asset->dpvs.smodelDrawInsts[i].unk0 = new_asset->dpvs.smodelDrawInsts[i].cullDist;
						new_asset->dpvs.smodelDrawInsts[i].unk1 = 0;

						new_asset->dpvs.smodelDrawInsts[i].flags = asset->dpvs.smodelDrawInsts[i].flags; // convert?
					}

					new_asset->dpvs.smodelLighting = mem->Alloc<zonetool::h1::GfxStaticModelLighting>(asset->dpvs.smodelCount);
					for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
					{
						if ((new_asset->dpvs.smodelDrawInsts[i].flags & STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) != 0)
						{
							new_asset->dpvs.smodelLighting[i].vertexLightingInfo.numLightingValues =
								asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.numLightingValues;
							REINTERPRET_CAST_SAFE_TO_FROM(new_asset->dpvs.smodelLighting[i].vertexLightingInfo.lightingValues,
								asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues);
						}
						else if ((new_asset->dpvs.smodelDrawInsts[i].flags & STATIC_MODEL_FLAG_GROUND_LIGHTING) != 0)
						{
							// check
							memcpy(&new_asset->dpvs.smodelLighting[i].modelGroundLightingInfo.groundLighting,
								&asset->dpvs.smodelDrawInsts[i].groundLighting, sizeof(float[4]));
						}
						else if ((new_asset->dpvs.smodelDrawInsts[i].flags & STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING) != 0)
						{
							memcpy(&new_asset->dpvs.smodelLighting[i].modelLightmapInfo, 
								&asset->dpvs.smodelDrawInsts[i].modelLightmapInfo, sizeof(GfxStaticModelLightmapInfo));
						}
						else if ((new_asset->dpvs.smodelDrawInsts[i].flags & STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) != 0)
						{
							ZONETOOL_WARNING("Houston, we have a problem");
						}
					}
					
					REINTERPRET_CAST_SAFE(dpvs.surfaceMaterials); // convert?
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
					COPY_VALUE(dpvs.sunShadowOptCount);
					COPY_VALUE(dpvs.sunSurfVisDataCount);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
					new_asset->dpvs.surfaceDeptAndSurf = mem->Alloc<zonetool::h1::GfxDepthAndSurf>(new_asset->dpvs.staticSurfaceCount); // todo?
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
				new_asset->dpvsDyn.dynEntVisData[0][3] = mem->Alloc<unsigned char>(new_asset->dpvsDyn.dynEntClientWordCount[0] * 32);
				new_asset->dpvsDyn.dynEntVisData[1][0] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[1][0]);
				new_asset->dpvsDyn.dynEntVisData[1][1] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[1][1]);
				new_asset->dpvsDyn.dynEntVisData[1][2] = reinterpret_cast<unsigned char*>(asset->dpvsDyn.dynEntVisData[1][2]);
				new_asset->dpvsDyn.dynEntVisData[1][3] = mem->Alloc<unsigned char>(new_asset->dpvsDyn.dynEntClientWordCount[1] * 32);

				COPY_VALUE(mapVtxChecksum);
				COPY_VALUE(heroOnlyLightCount);
				REINTERPRET_CAST_SAFE(heroOnlyLights);
				COPY_VALUE(fogTypesAllowed);
				COPY_VALUE(umbraTomeSize);
				REINTERPRET_CAST_SAFE(umbraTomeData);
				COPY_VALUE_CAST(umbraTomePtr);
				new_asset->mdaoVolumesCount = 0;
				new_asset->mdaoVolumes = nullptr;

				//unk1
				//unk2
				//unk3

				new_asset->buildInfo.args0 = nullptr;
				new_asset->buildInfo.args1 = nullptr;
				new_asset->buildInfo.buildStartTime = nullptr;
				new_asset->buildInfo.buildEndTime = nullptr;

				return new_asset;
			}

			void dump(GfxWorld* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IGfxWorld::dump(converted_asset);
			}
		}
	}
}