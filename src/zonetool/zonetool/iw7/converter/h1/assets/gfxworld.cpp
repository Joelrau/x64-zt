#include <std_include.hpp>

#include "zonetool/iw7/converter/h1/include.hpp"
#include "gfxworld.hpp"
#include "zonetool/iw7/assets/gfxworld.hpp"

#include "zonetool/iw7/converter/h1/assets/gfximage.hpp"
#include <zonetool/h1/assets/gfxworld.hpp>

//#define HARDCODED_DATA
//#define DO_SHIT_PROPERLY

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

				std::memcpy(new_asset->skies, asset->skies, sizeof(zonetool::h1::GfxSky));
				/*
				for (int i = 0; i < asset->skyCount; ++i)
				{
					// add bounds
					for (auto j = 0; j < asset->skies[i].skySurfCount; j++)
					{
						auto index = asset->dpvs.sortedSurfIndex[asset->skies[i].skyStartSurfs[j]];
						auto* surface_bounds = &asset->dpvs.surfacesBounds[index];
						memcpy(&new_asset->skies[i].bounds, &surface_bounds->bounds, sizeof(surface_bounds->bounds));

						//
						break;
					}
				}
				*/

				new_asset->portalGroupCount = 0;
				COPY_VALUE(lastSunPrimaryLightIndex);
				COPY_VALUE(primaryLightCount);
				new_asset->primaryLightEnvCount = asset->primaryLightCount + 1; // doesn't exist on IW7

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
				const auto transient_zone = asset->draw.transientZones[0];
				REINTERPRET_CAST_SAFE_TO_FROM(new_asset->aabbTreeCounts, transient_zone->aabbTreeCounts);
				REINTERPRET_CAST_SAFE_TO_FROM(new_asset->aabbTrees, transient_zone->aabbTrees);

				// this is pasted straight from IW5 -> H1, but it'll help me debug it so its ok lmfao
				// you shouldn't need to paste it this hard (or at all), but i'm not interested in wonky results rn and just wanna get into a game
				new_asset->aabbTreeCounts = allocator.allocate_array<zonetool::h1::GfxCellTreeCount>(new_asset->dpvsPlanes.cellCount);
				new_asset->aabbTrees = allocator.allocate_array<zonetool::h1::GfxCellTree>(new_asset->dpvsPlanes.cellCount);
				for (int i = 0; i < new_asset->dpvsPlanes.cellCount; ++i)
				{
					new_asset->aabbTreeCounts[i].aabbTreeCount = transient_zone->aabbTreeCounts[i].aabbTreeCount;
					new_asset->aabbTrees[i].aabbTree = allocator.allocate_array<zonetool::h1::GfxAabbTree>(new_asset->aabbTreeCounts[i].aabbTreeCount);
					
					for (int j = 0; j < new_asset->aabbTreeCounts[i].aabbTreeCount; ++j)
					{
						memcpy(&new_asset->aabbTrees[i].aabbTree[j].bounds, &transient_zone->aabbTrees[i].aabbTree[j].bounds, sizeof(Bounds));

						new_asset->aabbTrees[i].aabbTree[j].startSurfIndex = transient_zone->aabbTrees[i].aabbTree[j].startSurfIndex;
						new_asset->aabbTrees[i].aabbTree[j].surfaceCount = transient_zone->aabbTrees[i].aabbTree[j].surfaceCount;

						new_asset->aabbTrees[i].aabbTree[j].smodelIndexCount = transient_zone->aabbTrees[i].aabbTree[j].smodelIndexCount;
						REINTERPRET_CAST_SAFE_TO_FROM(new_asset->aabbTrees[i].aabbTree[j].smodelIndexes, transient_zone->aabbTrees[i].aabbTree[j].smodelIndexes);

						new_asset->aabbTrees[i].aabbTree[j].childCount = transient_zone->aabbTrees[i].aabbTree[j].childCount;
						// re-calculate childrenOffset
						auto offset = transient_zone->aabbTrees[i].aabbTree[j].childrenOffset;
						int childrenIndex = offset / sizeof(zonetool::h1::GfxAabbTree);
						int childrenOffset = childrenIndex * sizeof(zonetool::h1::GfxAabbTree);
						new_asset->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
					}
				}

				/*
				// cells
				COPY_VALUE_CAST(cells->bounds);
				COPY_VALUE(cells->portalCount);
				new_asset->cells->reflectionProbeCount = asset->draw.reflectionProbeData.reflectionProbeCount; // not on IW7?
				new_asset->cells->reflectionProbeReferenceCount = 0;
				COPY_VALUE_CAST(cells->portals);
				new_asset->cells->reflectionProbes = nullptr; // TODO (lol)
				new_asset->cells->reflectionProbeReferences = nullptr;
				*/

				new_asset->cells = allocator.allocate_array<zonetool::h1::GfxCell>(new_asset->dpvsPlanes.cellCount);
				for (int i = 0; i < new_asset->dpvsPlanes.cellCount; i++)
				{
					memcpy(&new_asset->cells[i].bounds, &asset->cells[i].bounds, sizeof(Bounds));
					new_asset->cells[i].portalCount = asset->cells[i].portalCount;

					auto add_portal = [](zonetool::h1::GfxPortal* h1_portal, zonetool::iw7::GfxPortal* iw7_portal)
					{
						//h1_portal->writable.isQueued = iw7_portal->writable.isQueued;
						//h1_portal->writable.isAncestor = iw7_portal->writable.isAncestor;
						//h1_portal->writable.recursionDepth = iw7_portal->writable.recursionDepth;
						//h1_portal->writable.hullPointCount = iw7_portal->writable.hullPointCount;
						//h1_portal->writable.hullPoints = reinterpret_cast<float(*__ptr64)[2]>(iw7_portal->writable.hullPoints);
						//h1_portal->writable.queuedParent = add_portal(iw7_portal->writable.queuedParent); // mapped at runtime

						memcpy(&h1_portal->plane, &iw7_portal->plane, sizeof(zonetool::h1::DpvsPlane));
						h1_portal->vertices = reinterpret_cast<float(*__ptr64)[3]>(iw7_portal->vertices);
						h1_portal->cellIndex = iw7_portal->cellIndex;
						h1_portal->closeDistance = iw7_portal->closeDistance;
						h1_portal->vertexCount = iw7_portal->vertexCount;
						memcpy(&h1_portal->hullAxis, &iw7_portal->hullAxis, sizeof(float[2][3]));
					};

					new_asset->cells[i].portals = allocator.allocate_array<zonetool::h1::GfxPortal>(new_asset->cells[i].portalCount);
					for (int j = 0; j < new_asset->cells[i].portalCount; j++)
					{
						add_portal(&new_asset->cells[i].portals[j], &asset->cells[i].portals[j]);
					}

#ifdef DO_SHIT_PROPERLY
					new_asset->cells[i].reflectionProbeCount = asset->cells[i].reflectionProbeCount;
					new_asset->cells[i].reflectionProbes = reinterpret_cast<unsigned __int8* __ptr64>(asset->cells[i].reflectionProbes);
					new_asset->cells[i].reflectionProbeReferenceCount = asset->cells[i].reflectionProbeReferenceCount;
					new_asset->cells[i].reflectionProbeReferences = reinterpret_cast<unsigned __int8* __ptr64>(asset->cells[i].reflectionProbeReferences);
#else
					new_asset->cells[i].reflectionProbeCount = 0;
					new_asset->cells[i].reflectionProbes = nullptr;
					new_asset->cells[i].reflectionProbeReferenceCount = 0;
					new_asset->cells[i].reflectionProbeReferences = nullptr;
#endif
				}

				new_asset->portalGroup = nullptr; // :3

				new_asset->unk_vec4_count_0 = 0;
				new_asset->unk_vec4_0 = nullptr;

				auto draw = new_asset->draw;
				draw.reflectionProbeCount = asset->draw.reflectionProbeData.reflectionProbeCount;
				draw.reflectionProbes = allocator.allocate_array<zonetool::h1::GfxImage*>(new_asset->draw.reflectionProbeCount);
				draw.reflectionProbeOrigins = allocator.allocate_array<zonetool::h1::GfxReflectionProbe>(new_asset->draw.reflectionProbeCount);
				draw.reflectionProbeTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(new_asset->draw.reflectionProbeCount);
#ifdef DO_SHIT_PROPERLY
				for (unsigned int i = 0; i < new_asset->draw.reflectionProbeCount; i++)
				{
					new_asset->draw.reflectionProbes[i] = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->draw.reflectionProbes[i]->name = asset->draw.reflectionProbeData.reflectionProbes[i];
					memcpy(&new_asset->draw.reflectionProbeOrigins[i].origin, &asset->draw.reflectionProbeOrigins[i].origin, sizeof(float[3]));
					new_asset->draw.reflectionProbeOrigins[i].probeVolumeCount = 0;
					new_asset->draw.reflectionProbeOrigins[i].probeVolumes = nullptr;
					//memcpy(&new_asset->draw.reflectionProbeTextures[i], &asset->draw.reflectionProbeTextures[i].loadDef, 20);
				}
#else
				for (unsigned int i = 0; i < new_asset->draw.reflectionProbeCount; i++)
				{
					new_asset->draw.reflectionProbes[i] = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->draw.reflectionProbes[i]->name = utils::string::va("x64zt_reflection_probe_%d", i); // theres literally no name info lmfao
					memcpy(&new_asset->draw.reflectionProbeOrigins[i].origin, &asset->draw.reflectionProbeData.reflectionProbes[i].origin, sizeof(float[3]));
					new_asset->draw.reflectionProbeOrigins[i].probeVolumeCount = 0;
					new_asset->draw.reflectionProbeOrigins[i].probeVolumes = nullptr;
					//memcpy(&new_asset->draw.reflectionProbeTextures[i], &asset->draw.reflectionProbeTextures[i].loadDef, 20);
				}
#endif

				// none of this data exists in IW7 lmfao
				new_asset->draw.reflectionProbeReferenceCount = 0;
				new_asset->draw.reflectionProbeReferenceOrigins = nullptr;
				new_asset->draw.reflectionProbeReferences = nullptr;

				new_asset->draw.lightmapCount = asset->draw.lightMapCount;
				new_asset->draw.lightmaps = allocator.allocate_array<zonetool::h1::GfxLightmapArray>(new_asset->draw.lightmapCount);
				new_asset->draw.lightmapPrimaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(new_asset->draw.lightmapCount);
				new_asset->draw.lightmapSecondaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(new_asset->draw.lightmapCount);
				for (int i = 0; i < new_asset->draw.lightmapCount; i++)
				{
					new_asset->draw.lightmaps[i].primary = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->draw.lightmaps[i].primary->name = asset->draw.lightMaps[i]->name;
					new_asset->draw.lightmaps[i].secondary = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->draw.lightmaps[i].secondary->name = utils::string::va("x64zt_secondary_%s", asset->draw.lightMaps[i]->name); // idk????

					//memcpy(&new_asset->draw.lightmapPrimaryTextures[i], &asset->draw.lightmapPrimaryTextures[i].loadDef, 20);
					//memcpy(&new_asset->draw.lightmapSecondaryTextures[i], &asset->draw.lightmapSecondaryTextures[i].loadDef, 20);
				}

				if (asset->draw.lightmapOverridePrimary)
				{
					new_asset->draw.lightmapOverridePrimary = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->draw.lightmapOverridePrimary->name = asset->draw.lightmapOverridePrimary->name;
				}
				else
				{
					new_asset->draw.lightmapOverridePrimary = nullptr;
				}

				if (asset->draw.lightmapOverrideSecondary)
				{
					new_asset->draw.lightmapOverrideSecondary = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->draw.lightmapOverrideSecondary->name = asset->draw.lightmapOverrideSecondary->name;
				}
				else
				{
					new_asset->draw.lightmapOverrideSecondary = nullptr;
				}

				new_asset->draw.u1[0] = 1024; new_asset->draw.u1[1] = 1024; // u1
				new_asset->draw.u2[0] = 512; new_asset->draw.u2[1] = 512; // u2
				new_asset->draw.u3 = 8; // u3

				new_asset->draw.trisType = 0; // dunno
				new_asset->draw.vertexCount = transient_zone->vertexCount;
				new_asset->draw.vd.vertices = allocator.allocate_array<zonetool::h1::GfxWorldVertex>(new_asset->draw.vertexCount);

				new_asset->draw.vertexLayerDataSize = transient_zone->vertexLayerDataSize;
				REINTERPRET_CAST_SAFE_TO_FROM(new_asset->draw.vld.data, transient_zone->vld.data);

				new_asset->draw.indexCount = asset->draw.indexCount;
				REINTERPRET_CAST_SAFE_TO_FROM(new_asset->draw.indices, asset->draw.indices);

				new_asset->draw.displacementParmsCount = 0;
				new_asset->draw.displacementParms = nullptr;

				/*
				new_asset->lightGrid.hasLightRegions = 0;
				new_asset->lightGrid.useSkyForLowZ = 0;
				new_asset->lightGrid.lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex; // wrong but idk
				memcpy(&new_asset->lightGrid.mins, &asset->lightGrid.mins, sizeof(short[3]));
				memcpy(&new_asset->lightGrid.maxs, &asset->lightGrid.maxs, sizeof(short[3]));
				new_asset->lightGrid.rowAxis = asset->lightGrid.rowAxis;
				new_asset->lightGrid.colAxis = asset->lightGrid.colAxis;
				REINTERPRET_CAST_SAFE(new_asset->lightGrid.rowDataStart, asset->lightGrid.rowDataStart);
				new_asset->lightGrid.rawRowDataSize = asset->lightGrid.rawRowDataSize;
				REINTERPRET_CAST_SAFE(new_asset->lightGrid.rawRowData, asset->lightGrid.rawRowData);
				new_asset->lightGrid.entryCount = asset->lightGrid.entryCount;
				new_asset->lightGrid.entries = mem.allocate<H1::GfxLightGridEntry>(new_asset->lightGrid.entryCount);
				for (unsigned int i = 0; i < new_asset->lightGrid.entryCount; i++)
				{
					new_asset->lightGrid.entries[i].colorsIndex = asset->lightGrid.entries[i].colorsIndex;
					new_asset->lightGrid.entries[i].primaryLightEnvIndex = asset->lightGrid.entries[i].primaryLightIndex;
					new_asset->lightGrid.entries[i].unused = 0;
					new_asset->lightGrid.entries[i].needsTrace = asset->lightGrid.entries[i].needsTrace;
				}
				new_asset->lightGrid.colorCount = asset->lightGrid.colorCount;
				new_asset->lightGrid.colors = mem.allocate<H1::GfxLightGridColors>(new_asset->lightGrid.colorCount);
				for (unsigned int i = 0; i < new_asset->lightGrid.colorCount; i++)
				{
					for (unsigned int j = 0; j < 56; j++)
					{
						auto& rgb = asset->lightGrid.colors[i].rgb[j];
						auto& dest_rgb = new_asset->lightGrid.colors[i].rgb[j];
						dest_rgb[0] = float_to_half(rgb[0] / 255.f);
						dest_rgb[1] = float_to_half(rgb[1] / 255.f);
						dest_rgb[2] = float_to_half(rgb[2] / 255.f);
					}
				}
				*/

				return new_asset;
			}

			void dump(zonetool::iw7::GfxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::gfx_world::dump(converted_asset);
			}
		}
	}
}
