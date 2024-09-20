#include <std_include.hpp>

#include "zonetool/h2/converter/h1/include.hpp"
#include "gfxworld.hpp"
#include "zonetool/h1/assets/gfxworld.hpp"

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace gfxworld
		{
			zonetool::h1::GfxWorld* convert(zonetool::h2::GfxWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::GfxWorld>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_SAFE(baseName);
				COPY_VALUE(bspVersion);
				COPY_VALUE(planeCount);
				//COPY_VALUE(nodeCount);
				COPY_VALUE(surfaceCount);
				COPY_VALUE(skyCount);
				REINTERPRET_CAST_SAFE(skies);
				COPY_VALUE(portalGroupCount);
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

				{
					COPY_VALUE(dpvsPlanes.cellCount);
					REINTERPRET_CAST_SAFE(dpvsPlanes.planes);
					new_asset->nodeCount = 2 * asset->nodeCount;
					new_asset->dpvsPlanes.nodes = allocator.allocate_array<unsigned short>(new_asset->nodeCount);
					auto node_index = 0;
					for (auto i = 0; i < asset->nodeCount; i++)
					{
						new_asset->dpvsPlanes.nodes[node_index++] = asset->dpvsPlanes.nodes[i].unk0;
						new_asset->dpvsPlanes.nodes[node_index++] = asset->dpvsPlanes.nodes[i].unk1 * 2;
					}

					REINTERPRET_CAST_SAFE(dpvsPlanes.sceneEntCellBits);
				}

				REINTERPRET_CAST_SAFE(skies);
				REINTERPRET_CAST_SAFE(aabbTreeCounts);
				REINTERPRET_CAST_SAFE(aabbTrees);
				REINTERPRET_CAST_SAFE(cells);

				new_asset->portalGroup = allocator.allocate_array<zonetool::h1::GfxPortalGroup>(new_asset->portalGroupCount);
				for (auto i = 0u; i < new_asset->portalGroupCount; i++)
				{
					COPY_VALUE(portalGroup[i].targetName);
					REINTERPRET_CAST_SAFE(portalGroup[i].gfxPortalArray);
					COPY_VALUE(portalGroup[i].numPortals);
				}

				COPY_VALUE(portalDistanceAnchorCount);
				REINTERPRET_CAST_SAFE(portalDistanceAnchorsAndCloseDistSquared);
				COPY_VALUE_CAST(draw);
				COPY_VALUE_CAST(lightGrid);
				COPY_VALUE(modelCount);
				REINTERPRET_CAST_SAFE(models);
				COPY_VALUE_CAST(bounds);
				COPY_VALUE_CAST(shadowBounds);
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
					COPY_VALUE(dpvs.smodelCount);
					COPY_VALUE(dpvs.subdivVertexLightingInfoCount);
					COPY_VALUE(dpvs.staticSurfaceCount);
					COPY_VALUE(dpvs.litOpaqueSurfsBegin);
					COPY_VALUE(dpvs.litOpaqueSurfsEnd);
					COPY_VALUE(dpvs.unkSurfsBegin);
					COPY_VALUE(dpvs.unkSurfsEnd);
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
					REINTERPRET_CAST_SAFE_ARR(dpvs.smodelVisData, 4);
					REINTERPRET_CAST_SAFE_ARR(dpvs.smodelUnknownVisData, 27);
					REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceVisData, 4);
					REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceUnknownVisData, 27);
					REINTERPRET_CAST_SAFE_ARR(dpvs.smodelUmbraVisData, 4);
					REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceUmbraVisData, 4);
					REINTERPRET_CAST_SAFE(dpvs.lodData);
					REINTERPRET_CAST_SAFE(dpvs.tessellationCutoffVisData);
					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);
					REINTERPRET_CAST_SAFE(dpvs.surfaces);
					REINTERPRET_CAST_SAFE(dpvs.surfacesBounds);

					new_asset->dpvs.smodelDrawInsts = allocator.allocate_array<zonetool::h1::GfxStaticModelDrawInst>(asset->dpvs.smodelCount);
					for (auto i = 0u; i < new_asset->dpvs.smodelCount; i++)
					{
						new_asset->dpvs.smodelDrawInsts[i].model = allocator.allocate<zonetool::h1::XModel>();
						new_asset->dpvs.smodelDrawInsts[i].model->name = asset->dpvs.smodelDrawInsts[i].model->name;
						COPY_VALUE_CAST(dpvs.smodelDrawInsts[i].placement);
						COPY_VALUE(dpvs.smodelDrawInsts[i].cullDist);
						COPY_VALUE(dpvs.smodelDrawInsts[i].flags);
						COPY_VALUE(dpvs.smodelDrawInsts[i].lightingHandle);
						COPY_VALUE(dpvs.smodelDrawInsts[i].staticModelId);
						COPY_VALUE(dpvs.smodelDrawInsts[i].primaryLightEnvIndex);
						COPY_VALUE(dpvs.smodelDrawInsts[i].reflectionProbeIndex);
						COPY_VALUE(dpvs.smodelDrawInsts[i].firstMtlSkinIndex);
						COPY_VALUE(dpvs.smodelDrawInsts[i].sunShadowFlags);
						new_asset->dpvs.smodelDrawInsts[i].reactiveMotionCullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
						new_asset->dpvs.smodelDrawInsts[i].reactiveMotionLOD = asset->dpvs.smodelDrawInsts[i].unk;
					}

					REINTERPRET_CAST_SAFE(dpvs.unknownSModelVisData1);
					REINTERPRET_CAST_SAFE(dpvs.unknownSModelVisData2);
					REINTERPRET_CAST_SAFE(dpvs.smodelLightingInsts);
					REINTERPRET_CAST_SAFE(dpvs.subdivVertexLighting);
					REINTERPRET_CAST_SAFE(dpvs.surfaceMaterials);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
					COPY_VALUE(dpvs.sunShadowOptCount);
					COPY_VALUE(dpvs.sunSurfVisDataCount);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
					REINTERPRET_CAST_SAFE(dpvs.surfaceDeptAndSurf);
					REINTERPRET_CAST_SAFE(dpvs.constantBuffersLit);
					REINTERPRET_CAST_SAFE(dpvs.constantBuffersAmbient);
					COPY_VALUE(dpvs.usageCount);
				}

				COPY_VALUE_CAST(dpvsDyn);
				COPY_VALUE(mapVtxChecksum);
				COPY_VALUE(heroOnlyLightCount);
				REINTERPRET_CAST_SAFE(heroOnlyLights);
				COPY_VALUE(fogTypesAllowed);
				COPY_VALUE(umbraTomeSize);
				REINTERPRET_CAST_SAFE(umbraTomeData);
				COPY_VALUE_CAST(umbraTomePtr);
				COPY_VALUE(mdaoVolumesCount);
				REINTERPRET_CAST_SAFE(mdaoVolumes);
				COPY_VALUE_CAST(buildInfo);

				return new_asset;
			}

			void dump(zonetool::h2::GfxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::gfx_world::dump(converted_asset);
			}
		}
	}
}
