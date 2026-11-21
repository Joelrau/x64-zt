#include <set>
#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "gfxworld.hpp"
#include <unordered_map>
#include "clipmap.hpp"

#include "zonetool/h1/assets/gfxworld.hpp"

// IW7 -> H1 GfxWorld, FIRST PASS (untested).
//
// What is converted for real:  planes/nodes, skies, cells + aabb trees (merged from the transient zones),
//   vertices / vertex layer data (merged from the transient zones), indices, surfaces, brush models,
//   sun, static model placements, bounds.
// What is STUBBED (IW7 data has no H1 equivalent):
//   lightmaps (none), light grid (flat, 1-node), reflection probes (1 stub), vis data (zeroed),
//   static model lighting (default light grid colour), portal groups, mdao.

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace gfxworld
		{
			namespace
			{
				constexpr unsigned char NO_LIGHTMAP = 0xFF; // TODO verify against H1 behaviour
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

				// zones holding the streamed world data
				const unsigned int zone_count = std::min<unsigned int>(asset->draw.transientZoneCount, 32);
				std::vector<unsigned int> vertex_base(zone_count + 1, 0);
				std::vector<unsigned int> layer_base(zone_count + 1, 0);
				for (unsigned int z = 0; z < zone_count; z++)
				{
					const auto* zone = asset->draw.transientZones[z];
					vertex_base[z + 1] = vertex_base[z] + (zone ? zone->vertexCount : 0);
					layer_base[z + 1] = layer_base[z] + (zone ? zone->vertexLayerDataSize : 0);
				}

				// skies
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
					if (asset->skies[i].skySurfCount > 0 && asset->dpvs.sortedSurfIndex && asset->dpvs.surfacesBounds)
					{
						const auto index = asset->dpvs.sortedSurfIndex[asset->skies[i].skyStartSurfs[0]];
						memcpy(&new_asset->skies[i].bounds, &asset->dpvs.surfacesBounds[index].bounds, sizeof(Bounds));
					}
				}

				COPY_VALUE(lastSunPrimaryLightIndex);
				COPY_VALUE(primaryLightCount);
				new_asset->primaryLightEnvCount = 0; // set from ComWorld at runtime; see comworld converter
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

				// cells: each cell's aabb trees live in the transient zone named by cellTransientInfos
				const int cell_count = asset->dpvsPlanes.cellCount;
				new_asset->aabbTreeCounts = allocator.allocate_array<zonetool::h1::GfxCellTreeCount>(cell_count);
				new_asset->aabbTrees = allocator.allocate_array<zonetool::h1::GfxCellTree>(cell_count);
				for (int i = 0; i < cell_count; i++)
				{
					if (!asset->cellTransientInfos) break;

					const auto& info = asset->cellTransientInfos[i];
					if (info.transientZone >= zone_count) continue;
					const auto* zone = asset->draw.transientZones[info.transientZone];
					if (!zone || !zone->aabbTreeCounts || !zone->aabbTrees) continue;

					const int tree_count = zone->aabbTreeCounts[info.aabbTreeIndex].aabbTreeCount;
					const auto* src_trees = zone->aabbTrees[info.aabbTreeIndex].aabbTree;
					new_asset->aabbTreeCounts[i].aabbTreeCount = tree_count;
					new_asset->aabbTrees[i].aabbTree = allocator.allocate_array<zonetool::h1::GfxAabbTree>(tree_count);
					for (int j = 0; j < tree_count; j++)
					{
						auto& d = new_asset->aabbTrees[i].aabbTree[j];
						const auto& s = src_trees[j];
						memcpy(&d.bounds, &s.bounds, sizeof(Bounds));
						d.startSurfIndex = static_cast<int>(s.startSurfIndex);
						d.surfaceCount = s.surfaceCount;
						d.smodelIndexCount = s.smodelIndexCount;
						REINTERPRET_CAST_SAFE_TO_FROM(d.smodelIndexes, s.smodelIndexes);
						d.childCount = s.childCount;
						if (s.childrenOffset)
						{
							// IW7 node is 56 bytes, H1 node is 48 bytes
							const int child_index = s.childrenOffset / static_cast<int>(sizeof(GfxAabbTree));
							d.childrenOffset = child_index * static_cast<int>(sizeof(zonetool::h1::GfxAabbTree));
						}
					}
				}

				new_asset->cells = allocator.allocate_array<zonetool::h1::GfxCell>(cell_count);
				for (int i = 0; i < cell_count; i++)
				{
					memcpy(&new_asset->cells[i].bounds, &asset->cells[i].bounds, sizeof(Bounds));
					new_asset->cells[i].portalCount = asset->cells[i].portalCount;
					REINTERPRET_CAST_SAFE_TO_FROM(new_asset->cells[i].portals, asset->cells[i].portals);
					new_asset->cells[i].reflectionProbeCount = 0;
					new_asset->cells[i].reflectionProbes = nullptr;
					new_asset->cells[i].reflectionProbeReferenceCount = 0;
					new_asset->cells[i].reflectionProbeReferences = nullptr;
				}

				new_asset->portalGroupCount = 0;
				new_asset->portalGroup = nullptr;
				new_asset->portalDistanceAnchorCount = 0;
				new_asset->portalDistanceAnchorsAndCloseDistSquared = nullptr;

				// reflection probes: STUB (IW7 stores one array image, H1 wants one image per probe)
				new_asset->draw.reflectionProbeCount = 1;
				new_asset->draw.reflectionProbes = allocator.allocate_array<zonetool::h1::GfxImage*>(1);
				new_asset->draw.reflectionProbes[0] = allocator.allocate<zonetool::h1::GfxImage>();
				new_asset->draw.reflectionProbes[0]->name = allocator.duplicate_string("*reflection_probe0");
				new_asset->draw.reflectionProbeOrigins = allocator.allocate_array<zonetool::h1::GfxReflectionProbe>(1);
				new_asset->draw.reflectionProbeTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(1);
				new_asset->draw.reflectionProbeReferenceCount = 0;
				new_asset->draw.reflectionProbeReferenceOrigins = nullptr;
				new_asset->draw.reflectionProbeReferences = nullptr;

				// lightmaps: STUB (none)
				new_asset->draw.lightmapCount = 1;
				new_asset->draw.lightmaps = allocator.allocate_array<zonetool::h1::GfxLightmapArray>(1);
				new_asset->draw.lightmaps[0].primary = allocator.allocate<zonetool::h1::GfxImage>();
				new_asset->draw.lightmaps[0].primary->name = allocator.duplicate_string("*lightmap0_primary");
				new_asset->draw.lightmaps[0].secondary = allocator.allocate<zonetool::h1::GfxImage>();
				new_asset->draw.lightmaps[0].secondary->name = allocator.duplicate_string("*lightmap0_secondary");
				new_asset->draw.lightmapPrimaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(1);
				new_asset->draw.lightmapSecondaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(1);
				new_asset->draw.lightmapOverridePrimary = nullptr;
				new_asset->draw.lightmapOverrideSecondary = nullptr;
				new_asset->draw.lightmapParameters.lightmapWidthPrimary = 1024;
				new_asset->draw.lightmapParameters.lightmapHeightPrimary = 1024;
				new_asset->draw.lightmapParameters.lightmapWidthSecondary = 512;
				new_asset->draw.lightmapParameters.lightmapHeightSecondary = 512;
				new_asset->draw.lightmapParameters.lightmapModelUnitsPerTexel = 8;
				new_asset->draw.trisType = 0;

				// vertices + layer data, concatenated across zones
				new_asset->draw.vertexCount = vertex_base[zone_count];
				new_asset->draw.vd.vertices = allocator.allocate_array<zonetool::h1::GfxWorldVertex>(new_asset->draw.vertexCount);
				new_asset->draw.vertexLayerDataSize = layer_base[zone_count];
				new_asset->draw.vld.data = allocator.allocate_array<unsigned char>(new_asset->draw.vertexLayerDataSize);
				for (unsigned int z = 0; z < zone_count; z++)
				{
					const auto* zone = asset->draw.transientZones[z];
					if (!zone) continue;
					if (zone->vd.vertices && zone->vertexCount)
					{
						memcpy(&new_asset->draw.vd.vertices[vertex_base[z]], zone->vd.vertices,
							sizeof(zonetool::h1::GfxWorldVertex) * zone->vertexCount);
					}
					if (zone->vld.data && zone->vertexLayerDataSize)
					{
						memcpy(&new_asset->draw.vld.data[layer_base[z]], zone->vld.data, zone->vertexLayerDataSize);
					}
				}

				COPY_VALUE(draw.indexCount);
				REINTERPRET_CAST_SAFE(draw.indices);
				new_asset->draw.displacementParmsCount = 0;
				new_asset->draw.displacementParms = nullptr;

				// light grid: STUB, flat 1-node tree (IW7 probe/tetrahedra grid has no H1 equivalent)
				new_asset->lightGrid.hasLightRegions = false;
				new_asset->lightGrid.useSkyForLowZ = false;
				new_asset->lightGrid.lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
				new_asset->lightGrid.colorCount = 1;
				new_asset->lightGrid.colors = allocator.allocate_array<zonetool::h1::GfxLightGridColors>(1);
				new_asset->lightGrid.entryCount = 1;
				new_asset->lightGrid.entries = allocator.allocate_array<zonetool::h1::GfxLightGridEntry>(1);
				new_asset->lightGrid.rawRowDataSize = 4;
				new_asset->lightGrid.rawRowData = allocator.allocate_array<unsigned char>(4);
				new_asset->lightGrid.rowDataStart = allocator.allocate_array<unsigned short>(1);
				new_asset->lightGrid.missingGridColorIndex = 0;
				new_asset->lightGrid.tableVersion = asset->lightGrid.tableVersion;
				new_asset->lightGrid.paletteVersion = asset->lightGrid.paletteVersion;
				new_asset->lightGrid.rangeExponent8BitsEncoding = asset->lightGrid.rangeExponent8BitsEncoding;
				new_asset->lightGrid.rangeExponent12BitsEncoding = asset->lightGrid.rangeExponent12BitsEncoding;
				new_asset->lightGrid.rangeExponent16BitsEncoding = asset->lightGrid.rangeExponent16BitsEncoding;
				new_asset->lightGrid.stageCount = 0;
				new_asset->lightGrid.stageLightingContrastGain = nullptr;
				for (unsigned int i = 0; i < 56; i++)
				{
					for (unsigned int j = 0; j < 3; j++)
					{
						new_asset->lightGrid.defaultLightGridColors.rgb[i][j] = half_float::float_to_half(asset->lightGrid.defaultLightGridColors.rgb[i][j]);
						new_asset->lightGrid.skyLightGridColors.rgb[i][j] = half_float::float_to_half(asset->lightGrid.skyLightGridColors.rgb[i][j]);
					}
				}
				// v58: the stub grid had all-zero colours, so static models (STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) were black
				// wherever the sun didn't hit them. Give the single grid entry + defaults a flat ambient instead.
				for (unsigned int i = 0; i < 56; i++)
				{
					for (unsigned int j = 0; j < 3; j++)
					{
						new_asset->lightGrid.colors[0].rgb[i][j] = 0x3800;
						if (new_asset->lightGrid.defaultLightGridColors.rgb[i][j] == 0)
							new_asset->lightGrid.defaultLightGridColors.rgb[i][j] = half_float::float_to_half(0.5f);
						if (new_asset->lightGrid.skyLightGridColors.rgb[i][j] == 0)
							new_asset->lightGrid.skyLightGridColors.rgb[i][j] = half_float::float_to_half(0.5f);
					}
				}
				for (auto i = 0; i < 3; i++)
				{
					auto& tree = new_asset->lightGrid.tree[i];
					tree.index = static_cast<unsigned char>(i);
					tree.maxDepth = 0;
					tree.unused[0] = 0;
					tree.nodeCount = 1;
					tree.leafCount = 1;
					tree.p_nodeTable = allocator.allocate_array<unsigned int>(1);
					tree.leafTableSize = 4;
					tree.p_leafTable = allocator.allocate_array<unsigned char>(4);
				}

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
				if (asset->outdoorImage)
				{
					new_asset->outdoorImage = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->outdoorImage->name = asset->outdoorImage->name;
				}
				// runtime-block bit arrays: engine memsets them at load, so they must exist (IW7 stores none)
				{
					const unsigned int cc = std::max<int>(new_asset->dpvsPlanes.cellCount, 1);
					const unsigned int words = (cc + 31) >> 5;
					new_asset->cellCasterBits = allocator.allocate_array<unsigned int>(cc * words);
					new_asset->cellHasSunLitSurfsBits = allocator.allocate_array<unsigned int>(words);
				}
				new_asset->sceneDynModel = nullptr;
				new_asset->sceneDynBrush = nullptr;
				{
					// v30: engine clears/sets bits here per non-sun primary light (65536 entity bits each); size matches the H1 writer
					const unsigned int span = new_asset->primaryLightCount > static_cast<unsigned int>(asset->lastSunPrimaryLightIndex)
						? new_asset->primaryLightCount - static_cast<unsigned int>(asset->lastSunPrimaryLightIndex) : 1u;
					const unsigned int bytes = (span << 13) - 0x2000;
					new_asset->primaryLightEntityShadowVis = allocator.allocate_array<unsigned int>(std::max<unsigned int>(bytes, 0x800)); // dump_array count is in elements
				}
				new_asset->primaryLightDynEntShadowVis[0] = nullptr;
				new_asset->primaryLightDynEntShadowVis[1] = nullptr;
				new_asset->nonSunPrimaryLightForModelDynEnt = nullptr;
				// engine zeroes one entry per primary light at load -> must be allocated (no shadow surfaces)
				new_asset->shadowGeom = allocator.allocate_array<zonetool::h1::GfxShadowGeometry>(std::max<unsigned int>(asset->primaryLightCount, 1));
				new_asset->shadowGeomOptimized = allocator.allocate_array<zonetool::h1::GfxShadowGeometry>(std::max<unsigned int>(asset->primaryLightCount, 1));
				new_asset->lightRegion = allocator.allocate_array<zonetool::h1::GfxLightRegion>(asset->primaryLightCount);

				// dpvs
				{
					auto& d = new_asset->dpvs;
					const auto& s = asset->dpvs;

					d.smodelCount = s.smodelCount;
					d.subdivVertexLightingInfoCount = 0;
					d.subdivVertexLighting = nullptr;
					d.staticSurfaceCount = s.staticSurfaceCount;
					d.litOpaqueSurfsBegin = s.litOpaqueSurfsBegin;
					d.litOpaqueSurfsEnd = s.litOpaqueSurfsEnd;
					d.unkSurfsBegin = 0;
					d.unkSurfsEnd = 0;
					d.litDecalSurfsBegin = s.litDecalSurfsBegin;
					d.litDecalSurfsEnd = s.litDecalSurfsEnd;
					d.litTransSurfsBegin = s.litTransSurfsBegin;
					d.litTransSurfsEnd = s.litTransSurfsEnd;
					d.shadowCasterSurfsBegin = 0;
					d.shadowCasterSurfsEnd = 0;
					d.emissiveSurfsBegin = s.emissiveSurfsBegin;
					d.emissiveSurfsEnd = s.emissiveSurfsEnd;
					d.smodelVisDataCount = s.smodelVisDataCount;
					d.surfaceVisDataCount = s.surfaceVisDataCount;

					// vis data is runtime-filled; allocate zeroed buffers of the right size
					for (auto i = 0; i < 4; i++)
					{
						d.smodelVisData[i] = allocator.allocate_array<unsigned int>(d.smodelVisDataCount);
						d.surfaceVisData[i] = allocator.allocate_array<unsigned int>(d.surfaceVisDataCount);
						d.smodelUmbraVisData[i] = allocator.allocate_array<unsigned int>(d.smodelVisDataCount);
						d.surfaceUmbraVisData[i] = allocator.allocate_array<unsigned int>(d.surfaceVisDataCount);
					}
					for (auto i = 0; i < 27; i++)
					{
						d.smodelUnknownVisData[i] = allocator.allocate_array<unsigned int>(d.smodelVisDataCount);
						d.surfaceUnknownVisData[i] = allocator.allocate_array<unsigned int>(d.surfaceVisDataCount);
					}
					d.unknownSModelVisData1 = allocator.allocate_array<unsigned int>(d.smodelVisDataCount);
					d.unknownSModelVisData2 = allocator.allocate_array<unsigned int>(d.smodelVisDataCount * 2);
					d.lodData = s.lodData;
					d.tessellationCutoffVisData = allocator.allocate_array<unsigned int>(d.surfaceVisDataCount);

					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);

					// surfaces: vertex/layer offsets are zone-relative in IW7, make them global
					d.surfaces = allocator.allocate_array<zonetool::h1::GfxSurface>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						const auto& src_surf = s.surfaces[i];
						auto& dst_surf = d.surfaces[i];
						const unsigned int z = std::min<unsigned int>(src_surf.transientZone, zone_count);

						dst_surf.tris.vertexLayerData = src_surf.tris.vertexLayerData + layer_base[z];
						dst_surf.tris.firstVertex = src_surf.tris.firstVertex + vertex_base[z];
						dst_surf.tris.maxEdgeLength = src_surf.tris.maxEdgeLength;
						dst_surf.tris.unk = -1;
						dst_surf.tris.vertexCount = src_surf.tris.vertexCount;
						dst_surf.tris.triCount = src_surf.tris.triCount;
						dst_surf.tris.baseIndex = src_surf.tris.baseIndex;
						dst_surf.material = reinterpret_cast<zonetool::h1::Material*>(src_surf.material);
						// v37: KOTH/hardpoint zone grid slabs are script-toggled in IW7 but would render permanently (solid red) here
						if (src_surf.material && src_surf.material->name && (std::string(src_surf.material->name).find("caulk") != std::string::npos || std::string(src_surf.material->name).find("koth_grid") != std::string::npos || std::string(src_surf.material->name).find("blockout") != std::string::npos /* v53: KOTH zone grids/blockout are script-toggled brush models; hide again now that brush models render */ ))
						{
							{ static std::set<std::string> logged_hidden; if (logged_hidden.insert(src_surf.material->name).second) ZONETOOL_INFO("v46 hidden surface material: %s tech=%s", src_surf.material->name, (src_surf.material->techniqueSet && src_surf.material->techniqueSet->name) ? src_surf.material->techniqueSet->name : "?"); }
							dst_surf.tris.triCount = 0;
						}
						dst_surf.laf.fields.lightmapIndex = 0; // v32: placeholder lightmap 0 exists
						dst_surf.laf.fields.reflectionProbeIndex = 0;
						dst_surf.laf.fields.primaryLightEnvIndex = 0;
						dst_surf.laf.fields.flags = src_surf.flags; // TODO convert
					}

					d.surfacesBounds = allocator.allocate_array<zonetool::h1::GfxSurfaceBounds>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						memcpy(&d.surfacesBounds[i], &s.surfacesBounds[i], sizeof(zonetool::h1::GfxSurfaceBounds));
					}

					d.smodelDrawInsts = allocator.allocate_array<zonetool::h1::GfxStaticModelDrawInst>(s.smodelCount);
					d.smodelLightingInsts = allocator.allocate_array<zonetool::h1::GfxStaticModelLighting>(s.smodelCount);
					for (unsigned int i = 0; i < s.smodelCount; i++)
					{
						const auto& src_inst = s.smodelDrawInsts[i];
						auto& dst_inst = d.smodelDrawInsts[i];

						memcpy(&dst_inst.placement, &src_inst.placement, sizeof(zonetool::h1::GfxPackedPlacement));
						dst_inst.model = reinterpret_cast<zonetool::h1::XModel*>(src_inst.model);
						{
							// v40: nodraw/blockout/caulk proxy models render as solid red (their image is missing) -> scale to zero
							const auto* xm = src_inst.model;
							bool all_proxy = xm && xm->numsurfs > 0;
							for (unsigned int mi = 0; xm && mi < xm->numsurfs; mi++)
							{
								const auto* mh = xm->materialHandles ? xm->materialHandles[mi] : nullptr;
								const std::string mn = (mh && mh->name) ? mh->name : "";
								if (mn.find("nodraw") == std::string::npos && mn.find("blockout") == std::string::npos &&
									mn.find("caulk") == std::string::npos && mn.find("koth_grid") == std::string::npos)
								{
									all_proxy = false;
									break;
								}
							}
							if (all_proxy) dst_inst.placement.scale = 0.0f;
						}
						dst_inst.cullDist = src_inst.cullDist;
						dst_inst.reactiveMotionCullDist = src_inst.cullDist;
						dst_inst.reactiveMotionLOD = 0;
						dst_inst.lightingHandle = src_inst.lightingHandle;
						dst_inst.staticModelId = static_cast<unsigned short>(i);
						dst_inst.primaryLightEnvIndex = src_inst.primaryLightEnvIndex;
						dst_inst.reflectionProbeIndex = 0;
						dst_inst.firstMtlSkinIndex = 0; // v28: IW7 skin index (e.g. 11) indexes a table H1 does not have
						dst_inst.sunShadowFlags = src_inst.sunShadowFlags;

						// IW7 flag bits do not map to H1 yet: use light-grid lighting with the (stub) default colour
						dst_inst.flags = zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_SCALE_9
							| zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING;
						d.smodelLightingInsts[i].ambientLightingInfo.colorIndex = 0;
						d.smodelLightingInsts[i].ambientLightingInfo.primaryLightWeight = 1.0f;
					}

					d.surfaceMaterials = allocator.allocate_array<zonetool::h1::GfxDrawSurf>(asset->surfaceCount);
					std::unordered_map<const void*, unsigned int> mat_index_map;
					const auto mat_index_of = [&](const void* m) -> unsigned int
					{
						const auto it = mat_index_map.find(m);
						if (it != mat_index_map.end()) return it->second;
						const auto idx = static_cast<unsigned int>(mat_index_map.size());
						mat_index_map[m] = idx;
						return idx;
					};
					for (unsigned int i = 0; i < asset->surfaceCount; i++) // runtime data
					{
						memset(&d.surfaceMaterials[i], 0, sizeof(d.surfaceMaterials[i])); // v27: IW7 packed bits are not valid H1 draw-surf bits
						// v32: fill the H1 draw-surf fields the renderer sorts/draws by
						d.surfaceMaterials[i].fields.objectId = static_cast<unsigned __int64>(i) & 0xFFFF;
						d.surfaceMaterials[i].fields.materialSortedIndex = static_cast<unsigned __int64>(mat_index_of(s.surfaces[i].material)) & 0x3FFF; // v33: dense per-material index in surface order
						{
							const auto* sm = s.surfaces[i].material;
							const unsigned int msk = (sm && sm->name) ? (static_cast<unsigned int>(sm->info.sortKey) & 0x3Fu) : 0u;
							d.surfaceMaterials[i].fields.primarySortKey = msk; // v34: material sort key
							if (i < 30 || i % 2000 == 0)
								ZONETOOL_INFO("v34 surf %u mat=%s mat_sortKey=%u surf_sortKey=%u flags=%u lm=%u", i, (sm && sm->name) ? sm->name : "(null)", msk, (unsigned)s.surfaces[i].sortKey, (unsigned)s.surfaces[i].flags, (unsigned)s.surfaces[i].lightmapIndex);
						}
						d.surfaceMaterials[i].fields.tessellation = s.surfaceMaterials ? (static_cast<unsigned __int64>(s.surfaceMaterials[i].fields.tessellation) & 3) : 0;
					}

					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
					COPY_VALUE(dpvs.sunShadowOptCount);
					COPY_VALUE(dpvs.sunSurfVisDataCount);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
					d.surfaceDeptAndSurf = allocator.allocate_array<zonetool::h1::GfxDepthAndSurf>(d.staticSurfaceCount); // runtime data
					d.constantBuffersLit = allocator.allocate_array<zonetool::h1::GfxWorldDpvsVoid>(s.smodelCount);
					d.constantBuffersAmbient = allocator.allocate_array<zonetool::h1::GfxWorldDpvsVoid>(s.smodelCount);
					for (unsigned int cb = 0; cb < s.smodelCount; cb++)
					{
						d.constantBuffersLit[cb] = allocator.allocate<char>();
						d.constantBuffersAmbient[cb] = allocator.allocate<char>();
					}
					COPY_VALUE(dpvs.usageCount);
				}

				for (int i = 0; i < 2; i++)
				{
					new_asset->dpvsDyn.dynEntClientWordCount[i] = 0; // v18: dynents not converted
					new_asset->dpvsDyn.dynEntClientCount[i] = 0;
					new_asset->dpvsDyn.dynEntCellBits[i] = nullptr;
					for (int j = 0; j < 4; j++)
					{
						new_asset->dpvsDyn.dynEntVisData[i][j] = allocator.allocate_array<unsigned char>(new_asset->dpvsDyn.dynEntClientWordCount[i] * 32);
					}
				}

				COPY_VALUE(mapVtxChecksum);
				COPY_VALUE(heroOnlyLightCount);
				REINTERPRET_CAST_SAFE(heroOnlyLights);
				new_asset->fogTypesAllowed = zonetool::h1::FOG_NORMAL | zonetool::h1::FOG_DFOG;

				// umbra: IW7 tome is a newer format than H1 understands -> drop it (no occlusion culling)
				new_asset->umbraTomeSize = 0;
				new_asset->umbraTomeData = nullptr;
				memset(&new_asset->umbraTomePtr, 0, sizeof(new_asset->umbraTomePtr));
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
				clipmap::dump_from_gfxworld(asset);
			}
		}
	}
}
