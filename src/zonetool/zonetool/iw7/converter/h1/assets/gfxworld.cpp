#include <std_include.hpp>

#include "zonetool/iw7/converter/h1/include.hpp"
#include "gfxworld.hpp"
#include "zonetool/iw7/assets/gfxworld.hpp"

//#define HARDCODED_DATA

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace gfxworld
		{
#define COPY_VALUE_(name, name2) \
		static_assert(sizeof(new_asset->name) == sizeof(asset->name2)); \
		new_asset->name = asset->name2;

			zonetool::h1::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::GfxWorld>();
				//std::memcpy(new_asset, asset, sizeof(GfxWorld));

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_SAFE(baseName);
				COPY_VALUE(bspVersion);
				COPY_VALUE(planeCount);
				COPY_VALUE(surfaceCount);
				COPY_VALUE(skyCount);

				// skies
				std::memcpy(new_asset->skies, asset->skies, sizeof(zonetool::h1::GfxSky));
				//new_asset->skies->bounds // TODO????

				new_asset->portalGroupCount = 0; // eeeek
				COPY_VALUE(lastSunPrimaryLightIndex);
				COPY_VALUE(primaryLightCount);
				new_asset->primaryLightEnvCount = 0; // doesn't exist on IW7

#ifdef HARDCODED_DATA
				new_asset->sortKeyLitDecal = 7;
				new_asset->sortKeyEffectDecal = 44;
				new_asset->sortKeyTopDecal = 17;
				new_asset->sortKeyEffectAuto = 54;
				new_asset->sortKeyDistortion = 49;
				new_asset->sortKeyHair = 18;
				new_asset->sortKeyEffectBlend = 33;
#else
				// partially hardcoded but most of these values are found on IW7 so
				COPY_VALUE(sortKeyLitDecal);
				COPY_VALUE(sortKeyEffectDecal);
				COPY_VALUE(sortKeyTopDecal);
				COPY_VALUE(sortKeyEffectAuto);
				COPY_VALUE(sortKeyDistortion);
				new_asset->sortKeyHair = 18;		//COPY_VALUE(sortKeyHair);
				new_asset->sortKeyEffectBlend = 33;	//COPY_VALUE(sortKeyEffectBlend);
#endif

				COPY_VALUE_CAST(dpvsPlanes);

				// from what i understand and quaK explaining, only 1 transientZone is used (singleplayer uses more than 1, but thats someone else's problem)
				REINTERPRET_CAST_SAFE_TO_FROM(new_asset->aabbTreeCounts, asset->draw.transientZones[0]->aabbTreeCounts);
				REINTERPRET_CAST_SAFE_TO_FROM(new_asset->aabbTrees, asset->draw.transientZones[0]->aabbTrees);

				// cells
				COPY_VALUE_CAST(cells->bounds);
				COPY_VALUE(cells->portalCount);
				new_asset->cells->reflectionProbeCount = asset->draw.reflectionProbeData.reflectionProbeCount; // not on IW7?
				new_asset->cells->reflectionProbeReferenceCount = 0;
				COPY_VALUE_CAST(cells->portals);
				new_asset->cells->reflectionProbes = nullptr; // TODO (lol)
				new_asset->cells->reflectionProbeReferences = nullptr;

				// portalGroup
				new_asset->portalGroup = nullptr; // :3

				// not in IW7
				//COPY_VALUE(unk_vec4_count_0);
				//REINTERPRET_CAST_SAFE(unk_vec4_0);

				// draw
				//new_asset->draw

				COPY_VALUE_CAST(lightGrid);
				COPY_VALUE(modelCount);
				REINTERPRET_CAST_SAFE(models);
				COPY_VALUE_CAST(unkBounds);
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
					new_asset->dpvs.unkCount1 = 0;
					REINTERPRET_CAST_SAFE_ARR(dpvs.smodelVisData, 4);
					REINTERPRET_CAST_SAFE_ARR(dpvs.smodelUnknownVisData, 27);
					REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceVisData, 4);
					REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceUnknownVisData, 27);
					REINTERPRET_CAST_SAFE_ARR(dpvs.smodelUmbraVisData, 4);
					REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceUmbraVisData, 4);
					new_asset->dpvs.unkCount2 = asset->dpvs.smodelCount;
					REINTERPRET_CAST_SAFE(dpvs.lodData);
					REINTERPRET_CAST_SAFE(dpvs.tessellationCutoffVisData);
					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);
					REINTERPRET_CAST_SAFE(dpvs.surfaces);
					REINTERPRET_CAST_SAFE(dpvs.surfacesBounds);

					new_asset->dpvs.smodelDrawInsts = allocator.allocate_array<zonetool::h2::GfxStaticModelDrawInst>(asset->dpvs.smodelCount);
					for (auto i = 0u; i < new_asset->dpvs.smodelCount; i++)
					{
						new_asset->dpvs.smodelDrawInsts[i].model = allocator.allocate<zonetool::h2::XModel>();
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
						new_asset->dpvs.smodelDrawInsts[i].pad = 1;
						new_asset->dpvs.smodelDrawInsts[i].unk = asset->dpvs.smodelDrawInsts[i].unk1;
					}

					REINTERPRET_CAST_SAFE(dpvs.unknownSModelVisData1);
					REINTERPRET_CAST_SAFE(dpvs.unknownSModelVisData2);
					REINTERPRET_CAST_SAFE(dpvs.smodelLighting);
					REINTERPRET_CAST_SAFE(dpvs.subdivVertexLighting);
					REINTERPRET_CAST_SAFE(dpvs.surfaceMaterials);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
					COPY_VALUE(dpvs.sunShadowOptCount);
					COPY_VALUE(dpvs.sunSurfVisDataCount);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
					REINTERPRET_CAST_SAFE(dpvs.surfaceDeptAndSurf);
					REINTERPRET_CAST_SAFE(dpvs.constantBuffersLit);
					REINTERPRET_CAST_SAFE(dpvs.constantBuffersAmbient);
					new_asset->dpvs.gfx_unk = nullptr;
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

			void dump(zonetool::h1::GfxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h2::gfx_world::dump(converted_asset);
			}
		}
	}
}
