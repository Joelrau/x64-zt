#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "gfxworld.hpp"

#include "zonetool/h1/assets/gfxworld.hpp"

//#define GENERATE_SUN_FILE

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace gfxworld
		{
#ifdef GENERATE_SUN_FILE
			static inline float dot_to_angle_deg(float d)
			{
				if (d > 1.0f) d = 1.0f;
				if (d < -1.0f) d = -1.0f;
				const float pi = 3.14159265358979323846f;
				return std::acosf(d) * 180.0f / pi;
			}

			static inline std::string format_float(float v, int decimals)
			{
				char buf[64];

				if (decimals == 0)
				{
					// %g prints integers as "20", not "20."
					std::snprintf(buf, sizeof(buf), "%g", v);
					return std::string(buf);
				}

				std::snprintf(buf, sizeof(buf), "%.*f", decimals, v);

				std::string s(buf);
				auto pos = s.find_last_not_of('0');
				if (pos == std::string::npos)
					return "0";

				s.erase(pos + 1);

				if (!s.empty() && s.back() == '.')
					s.pop_back();

				if (s == "-0")
					return "0";

				return s;
			}

			std::string make_sun_file(const sunflare_t& sun)
			{
				std::ostringstream out;

				const char* sprite_name = (sun.spriteMaterial && sun.spriteMaterial->name) ? sun.spriteMaterial->name : "";
				const char* flare_name = (sun.flareMaterial && sun.flareMaterial->name) ? sun.flareMaterial->name : "";

				float flare_min_angle = std::round(dot_to_angle_deg(sun.flareMinDot));
				float flare_max_angle = std::round(dot_to_angle_deg(sun.flareMaxDot));

				float blind_min_angle = std::round(dot_to_angle_deg(sun.blindMinDot));
				float blind_max_angle = std::round(dot_to_angle_deg(sun.blindMaxDot));

				float glare_min_angle = std::round(dot_to_angle_deg(sun.glareMinDot));
				float glare_max_angle = std::round(dot_to_angle_deg(sun.glareMaxDot));

				float flare_fadein = sun.flareFadeInTime / 1000.0f;
				float flare_fadeout = sun.flareFadeOutTime / 1000.0f;
				float blind_fadein = sun.blindFadeInTime / 1000.0f;
				float blind_fadeout = sun.blindFadeOutTime / 1000.0f;
				float glare_fadein = sun.glareFadeInTime / 1000.0f;
				float glare_fadeout = sun.glareFadeOutTime / 1000.0f;

				float angles[3]{};
				VectorToAngles(sun.sunFxPosition, angles);

				out << "r_sunsprite_shader \"" << sprite_name << "\"\n";
				out << "r_sunsprite_size \"" << format_float(sun.spriteSize, 4) << "\"\n";
				out << "r_sunflare_shader \"" << flare_name << "\"\n";
				out << "r_sunflare_min_size \"" << format_float(sun.flareMinSize * 2.0f, 4) << "\"\n";
				out << "r_sunflare_min_angle \"" << format_float(flare_min_angle, 0) << "\"\n";
				out << "r_sunflare_max_size \"" << format_float(sun.flareMaxSize * 2.0f, 4) << "\"\n";
				out << "r_sunflare_max_angle \"" << format_float(flare_max_angle, 0) << "\"\n";
				out << "r_sunflare_max_alpha \"" << format_float(sun.flareMaxAlpha, 4) << "\"\n";
				out << "r_sunflare_fadein \"" << format_float(flare_fadein, 3) << "\"\n";
				out << "r_sunflare_fadeout \"" << format_float(flare_fadeout, 3) << "\"\n";
				out << "r_sunblind_min_angle \"" << format_float(blind_min_angle, 0) << "\"\n";
				out << "r_sunblind_max_angle \"" << format_float(blind_max_angle, 0) << "\"\n";
				out << "r_sunblind_max_darken \"" << format_float(sun.blindMaxDarken, 5) << "\"\n";
				out << "r_sunblind_fadein \"" << format_float(blind_fadein, 3) << "\"\n";
				out << "r_sunblind_fadeout \"" << format_float(blind_fadeout, 3) << "\"\n";
				out << "r_sunglare_min_angle \"" << format_float(glare_min_angle, 0) << "\"\n";
				out << "r_sunglare_max_angle \"" << format_float(glare_max_angle, 0) << "\"\n";
				out << "r_sunglare_max_lighten \"" << format_float(sun.glareMaxLighten, 4) << "\"\n";
				out << "r_sunglare_fadein \"" << format_float(glare_fadein, 3) << "\"\n";
				out << "r_sunglare_fadeout \"" << format_float(glare_fadeout, 3) << "\"\n";

				out << "r_sun_fx_position \""
					<< format_float(angles[0], 0) << ' '
					<< format_float(angles[1], 0) << ' '
					<< format_float(angles[2], 0) << "\"\n";

				return out.str();
			}

			void dump_sun_file(const sunflare_t& sun, const std::string& output_path)
			{
				std::string sun_file_content = make_sun_file(sun);

				auto file = filesystem::file(output_path);
				file.open("w");
				file.write(sun_file_content.data(), sun_file_content.size());
				file.close();
			}
#endif

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
					// add bounds
					for (auto j = 0; j < asset->skies[i].skySurfCount; j++)
					{
						auto index = asset->dpvs.sortedSurfIndex[asset->skies[i].skyStartSurfs[j]];
						auto* surface_bounds = &asset->dpvs.surfacesBounds[index];
						memcpy(&new_asset->skies[i].bounds, &surface_bounds->bounds, sizeof(surface_bounds->bounds));

						if (1)
							break;
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

				//REINTERPRET_CAST_SAFE(draw.reflectionProbeOrigins);
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
				if (asset->lightGrid.colors)
				{
					for (unsigned int i = 0; i < 56; i++)
					{
						for (unsigned int j = 0; j < 3; j++)
						{
							auto& rgb = asset->lightGrid.colors[i].rgb[j];
							auto& dest_rgb = new_asset->lightGrid.colors[i].rgb[j];
							dest_rgb[0] = half_float::float_to_half(rgb[0] / 255.f);
							dest_rgb[1] = half_float::float_to_half(rgb[1] / 255.f);
							dest_rgb[2] = half_float::float_to_half(rgb[2] / 255.f);
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
					if (i == 0)
					{
						new_asset->lightGrid.tree[i].maxDepth = asset->lightGrid.tree.maxDepth;
						new_asset->lightGrid.tree[i].nodeCount = asset->lightGrid.tree.nodeCount;
						new_asset->lightGrid.tree[i].leafCount = asset->lightGrid.tree.leafCount;
						memcpy(new_asset->lightGrid.tree[i].coordMinGridSpace, asset->lightGrid.tree.coordMinGridSpace, sizeof(int[3]));
						memcpy(new_asset->lightGrid.tree[i].coordMaxGridSpace, asset->lightGrid.tree.coordMaxGridSpace, sizeof(int[3]));
						memcpy(new_asset->lightGrid.tree[i].coordHalfSizeGridSpace, asset->lightGrid.tree.coordHalfSizeGridSpace, sizeof(int[3]));
						new_asset->lightGrid.tree[i].defaultColorIndexBitCount = asset->lightGrid.tree.defaultColorIndexBitCount;
						new_asset->lightGrid.tree[i].defaultLightIndexBitCount = asset->lightGrid.tree.defaultLightIndexBitCount;
						new_asset->lightGrid.tree[i].p_nodeTable = asset->lightGrid.tree.p_nodeTable;
						new_asset->lightGrid.tree[i].leafTableSize = asset->lightGrid.tree.leafTableSize;
						new_asset->lightGrid.tree[i].p_leafTable = asset->lightGrid.tree.p_leafTable;

						enum leaf_table_version : std::int8_t
						{
							h2 = 0i8,
							h1 = 0i8,
							s1 = 0i8,
							iw6 = 1i8,
						};

						// with this indicator, we know the leafTable is in iw6 format.
						// this will allow us to create a patch for iw6 maps, so it will be able to read them.
						new_asset->lightGrid.tree[i].unused[0] = leaf_table_version::iw6;
					}
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
					REINTERPRET_CAST_SAFE(dpvs.surfaces);

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

					new_asset->dpvs.surfacesBounds = allocator.allocate_array<zonetool::h1::GfxSurfaceBounds>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						//memcpy(&new_asset->dpvs.surfacesBounds[i].bounds, &asset->dpvs.surfacesBounds[i].bounds, sizeof(Bounds));
						// unk data
						memcpy(&new_asset->dpvs.surfacesBounds[i].bounds, &asset->dpvs.surfacesBounds[i].bounds, sizeof(GfxSurfaceBounds));
						assert(sizeof(zonetool::h1::GfxSurfaceBounds) == sizeof(zonetool::iw6::GfxSurfaceBounds));
					}

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

						new_asset->dpvs.smodelDrawInsts[i].flags = asset->dpvs.smodelDrawInsts[i].flags;

						// g_lodDistIndexToScale
						new_asset->dpvs.smodelDrawInsts[i].flags |= zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_SCALE_9; // 1.0f

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
						if ((new_asset->dpvs.smodelDrawInsts[i].flags & zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) != 0)
						{
							if (game::get_mode() == game::game_mode::iw6)
							{
								auto& smodel_inst = asset->dpvs.smodelInsts[i];
								auto& smodel_draw_inst = asset->dpvs.smodelDrawInsts[i];

								if ((smodel_draw_inst.flags & zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) != 0 &&
									(smodel_draw_inst.flags & zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_ANIMATED_VERTS) != 0)
								{
									struct GfxLightGridRaw
									{
										unsigned int colorIndex;
										unsigned short lightIndex;
										unsigned char voxel;
									};

									GfxLightGridRaw light_grid_raw{};

									auto sample_pos = smodel_inst.lightingOrigin;
									short grid_pos[3]{};
									grid_pos[0] = static_cast<short>((static_cast<int>(std::floor(sample_pos[0])) + 0x20000) / 32);
									grid_pos[1] = static_cast<short>((static_cast<int>(std::floor(sample_pos[1])) + 0x20000) / 32);
									grid_pos[2] = static_cast<short>((static_cast<int>(std::floor(sample_pos[2])) + 0x20000) / 64);

									// void R_GetLightGrid(const GfxLightGridTree *p_tree, const unsigned short *gridPos, GfxLightGridRaw *p_lightGrid, bool allowEmptyNode)
									utils::hook::invoke<int>(0x1405FB6F0, &asset->lightGrid.tree, grid_pos, &light_grid_raw, 0);

									new_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.colorIndex = light_grid_raw.colorIndex;
									new_asset->dpvs.smodelLightingInsts[i].ambientLightingInfo.primaryLightWeight = 1.0f;
								}
							}
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