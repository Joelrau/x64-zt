#include <std_include.hpp>
#include "zonetool/h1/converter/s1/include.hpp"
#include "gfxworld.hpp"

#include "zonetool/s1/assets/gfxworld.hpp"

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace gfxworld
		{
			zonetool::s1::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::s1::GfxWorld>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_SAFE(baseName);
				new_asset->bspVersion = 109;
				COPY_VALUE(planeCount);
				COPY_VALUE(nodeCount);
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

				COPY_VALUE(dpvsPlanes.cellCount);
				REINTERPRET_CAST_SAFE(dpvsPlanes.planes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.nodes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.sceneEntCellBits);

				REINTERPRET_CAST_SAFE(skies);
				REINTERPRET_CAST_SAFE(aabbTreeCounts);
				REINTERPRET_CAST_SAFE(aabbTrees);
				REINTERPRET_CAST_SAFE(cells);

				REINTERPRET_CAST_SAFE(portalGroup);

				COPY_VALUE(portalDistanceAnchorCount);
				REINTERPRET_CAST_SAFE(portalDistanceAnchorsAndCloseDistSquared);

				COPY_VALUE(draw.reflectionProbeCount);
				REINTERPRET_CAST_SAFE(draw.reflectionProbes);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeOrigins);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeTextures);
				COPY_VALUE(draw.reflectionProbeReferenceCount);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeReferenceOrigins);
				REINTERPRET_CAST_SAFE(draw.reflectionProbeReferences);
				COPY_VALUE(draw.lightmapCount);
				REINTERPRET_CAST_SAFE(draw.lightmaps);
				REINTERPRET_CAST_SAFE(draw.lightmapPrimaryTextures);
				REINTERPRET_CAST_SAFE(draw.lightmapSecondaryTextures);
				REINTERPRET_CAST_SAFE(draw.lightmapOverridePrimary);
				REINTERPRET_CAST_SAFE(draw.lightmapOverrideSecondary);
				COPY_ARR(draw.lightmapParameters);
				COPY_VALUE(draw.trisType);
				COPY_VALUE(draw.vertexCount);
				REINTERPRET_CAST_SAFE(draw.vd.vertices);
				COPY_VALUE(draw.vertexLayerDataSize);
				REINTERPRET_CAST_SAFE(draw.vld.data);
				COPY_VALUE(draw.indexCount);
				REINTERPRET_CAST_SAFE(draw.indices);

				COPY_VALUE_CAST(lightGrid);
				COPY_VALUE(modelCount);
				REINTERPRET_CAST_SAFE(models);
				COPY_ARR(bounds);
				COPY_ARR(shadowBounds);
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
					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);
					REINTERPRET_CAST_SAFE(dpvs.surfaces);

					new_asset->dpvs.surfaces = allocator.allocate_array<zonetool::s1::GfxSurface>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						COPY_VALUE(dpvs.surfaces[i].tris.vertexLayerData);
						COPY_VALUE(dpvs.surfaces[i].tris.firstVertex);
						COPY_VALUE(dpvs.surfaces[i].tris.maxEdgeLength);
						COPY_VALUE(dpvs.surfaces[i].tris.vertexCount);
						COPY_VALUE(dpvs.surfaces[i].tris.triCount);
						COPY_VALUE(dpvs.surfaces[i].tris.baseIndex);
						new_asset->dpvs.surfaces[i].material = reinterpret_cast<zonetool::s1::Material*>(asset->dpvs.surfaces[i].material);
						COPY_ARR(dpvs.surfaces[i].laf);
					}

					new_asset->dpvs.surfacesBounds = allocator.allocate_array<zonetool::s1::GfxSurfaceBounds>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						COPY_ARR(dpvs.surfacesBounds[i].bounds);
					}

					REINTERPRET_CAST_SAFE(dpvs.smodelDrawInsts);

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

				COPY_VALUE(useLightGridDefaultModelLightingLookup);
				COPY_VALUE(useLightGridDefaultFXLightingLookup);
				COPY_ARR(lightGridDefaultModelLightingLookup);
				COPY_ARR(lightGridDefaultFXLightingLookup);

				COPY_VALUE_CAST(buildInfo);

				return new_asset;
			}

			void dump(GfxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::s1::gfx_world::dump(converted_asset);
			}
		}
	}
}
