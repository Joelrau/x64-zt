#include <std_include.hpp>

#include "zonetool/h1/converter/iw7/include.hpp"
#include "gfxworld.hpp"
#include "zonetool/iw7/assets/gfxworld.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace
		{
			std::string clean_name(const std::string& name)
			{
				auto new_name = name;

				for (auto i = 0u; i < name.size(); i++)
				{
					switch (new_name[i])
					{
					case '*':
						new_name[i] = '_';
						break;
					}
				}

				return new_name;
			}
		}

		namespace gfxworld
		{
//#define CONVERT
			zonetool::iw7::GfxImage* generate_reflection_probe_array_image(GfxWorldDraw* draw, utils::memory::allocator& allocator)
			{
				const std::string image_name = "*reflection_probe_array";
				const std::string image_name_clean = "_reflection_probe_array";

				std::uint8_t* data = nullptr;
				bool once = false;
				std::uint32_t width = 0;
				std::uint32_t height = 0;
				std::uint16_t depth = 0;
				std::uint32_t mip_levels = 0;
				std::int32_t format = 0;

				std::vector<DirectX::Image> images{};

				for (unsigned int image_index = 1; image_index < draw->reflectionProbeCount; image_index++)
				{
					GfxImage* probe_image = draw->reflectionProbes[image_index];
					data = probe_image->pixelData;

					if (once)
					{
						assert(width == probe_image->width &&
							height == probe_image->height &&
							depth == probe_image->depth &&
							mip_levels == probe_image->levelCount &&
							format == probe_image->imageFormat);
					}
					width = probe_image->width;
					height = probe_image->height;
					depth = probe_image->depth;
					mip_levels = probe_image->levelCount;
					format = probe_image->imageFormat;

					once = true;

					std::size_t used_data = 0;

					for (auto a = 0; a < 6; a++)
					{
						int divider = 1;
						for (auto i = 0; i < probe_image->levelCount; i++)
						{
							DirectX::Image img{};
							img.pixels = data;
							img.width = probe_image->width / divider;
							img.height = probe_image->height / divider;
							img.format = DXGI_FORMAT(probe_image->imageFormat);

							size_t rowPitch;
							size_t slicePitch;
							DirectX::ComputePitch(img.format, img.width, img.height, rowPitch, slicePitch);

							img.rowPitch = rowPitch;
							img.slicePitch = slicePitch;
							
#ifdef CONVERT
							// Doesn't work
							DirectX::ScratchImage destImage;
							auto hr = DirectX::Convert(img, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT , DirectX::TEX_THRESHOLD_DEFAULT, destImage);
							if (FAILED(hr))
							{
								ZONETOOL_FATAL("Failed to convert image \"%s\"", probe_image->name);
								return nullptr;
							}

							auto* pixels = allocator.allocate_array<unsigned char>(destImage.GetPixelsSize());
							memcpy(pixels, destImage.GetPixels(), destImage.GetPixelsSize());
							img.pixels = pixels;

							img.format = DXGI_FORMAT_R8G8B8A8_UNORM;

							DirectX::ComputePitch(img.format, img.width, img.height, rowPitch, slicePitch);

							img.rowPitch = rowPitch;
							img.slicePitch = slicePitch;
#endif

							images.push_back(img);

							data += slicePitch;
							used_data += slicePitch;
							divider += divider;
						}
					}

					assert(used_data == probe_image->dataLen1);
				}

				DirectX::TexMetadata mdata{};
				mdata.width = width;
				mdata.height = height;
				mdata.depth = depth;
				mdata.arraySize = (draw->reflectionProbeCount - 1) * 6;
				mdata.mipLevels = mip_levels;
				mdata.format = DXGI_FORMAT(format);
				mdata.dimension = DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D;
				mdata.miscFlags |= DirectX::TEX_MISC_FLAG::TEX_MISC_TEXTURECUBE;

				std::string parent_path = filesystem::get_dump_path() + "images\\";
				std::string spath = parent_path + image_name_clean + ".dds";
				std::wstring wpath(spath.begin(), spath.end());

				if (!std::filesystem::exists(parent_path))
				{
					std::filesystem::create_directories(parent_path);//
				}

#ifdef COMPRESS
				// Doesn't work
				DirectX::ScratchImage destImage;
				auto hr = DirectX::Compress(images.data(), images.size(), mdata, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, destImage);
				if (FAILED(hr))
				{
					ZONETOOL_FATAL("Failed to convert image \"%s\"", spath.data());
					return nullptr;
				}

				auto result = DirectX::SaveToDDSFile(destImage.GetImages(), destImage.GetImageCount(), destImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, wpath.data());
				if (FAILED(result))
				{
					ZONETOOL_FATAL("Failed to dump image \"%s\"", spath.data());
					return nullptr;
				}
#else
				auto result = DirectX::SaveToDDSFile(images.data(), images.size(), mdata, DirectX::DDS_FLAGS_NONE, wpath.data());
				if (FAILED(result))
				{
					ZONETOOL_FATAL("Failed to dump image \"%s\"", spath.data());
					return nullptr;
				}
#endif

				auto* image = allocator.allocate<zonetool::iw7::GfxImage>();
				image->name = allocator.duplicate_string(image_name);
				return image;
			}

			zonetool::iw7::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::iw7::GfxWorld>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_SAFE(baseName);
				new_asset->bspVersion = 159;
				COPY_VALUE(planeCount);
				COPY_VALUE(nodeCount);
				COPY_VALUE(surfaceCount);

				COPY_VALUE(skyCount);
				new_asset->skies = allocator.allocate_array<zonetool::iw7::GfxSky>(asset->skyCount);
				for (int i = 0; i < asset->skyCount; i++)
				{
					COPY_VALUE(skies[i].skySurfCount);
					REINTERPRET_CAST_SAFE(skies[i].skyStartSurfs);
					COPY_ASSET(skies[i].skyImage);
					COPY_VALUE(skies[i].skySamplerState);
				}

				COPY_VALUE(lastSunPrimaryLightIndex);
				COPY_VALUE(primaryLightCount);
				new_asset->movingScriptablePrimaryLightCount = 0;

				new_asset->sortKeyLitDecal = 7;
				new_asset->sortKeyEffectDecal = 14;
				new_asset->sortKeyTopDecal = 17;
				new_asset->sortKeyEffectAuto = 35;
				new_asset->sortKeyDistortion = 24;
				new_asset->sortKeyEffectDistortion = 36;
				new_asset->sortKey2D = 41;
				new_asset->sortKeyOpaqueBegin = 1;
				new_asset->sortKeyOpaqueEnd = 6;
				new_asset->sortKeyDecalBegin = 7;
				new_asset->sortKeyDecalEnd = 17;
				new_asset->sortKeyTransBegin = 18;
				new_asset->sortKeyTransEnd = 34;
				new_asset->sortKeyEmissiveBegin = 35;
				new_asset->sortKeyEmissiveEnd = 40;

				COPY_VALUE(dpvsPlanes.cellCount);
				REINTERPRET_CAST_SAFE(dpvsPlanes.planes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.nodes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.sceneEntCellBits);

				new_asset->cells = allocator.allocate_array<zonetool::iw7::GfxCell>(asset->dpvsPlanes.cellCount);
				for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
				{
					COPY_ARR(cells[i].bounds);
					COPY_VALUE(cells[i].portalCount);
					REINTERPRET_CAST_SAFE(cells[i].portals);
				}

				new_asset->cellTransientInfos = allocator.allocate_array<zonetool::iw7::GfxCellTransientInfo>(asset->dpvsPlanes.cellCount);
				for (unsigned short i = 0; i < asset->dpvsPlanes.cellCount; i++)
				{
					new_asset->cellTransientInfos[i].aabbTreeIndex = i;
					new_asset->cellTransientInfos[i].transientZone = 0;
				}

				assert(asset->draw.reflectionProbeCount);
				new_asset->draw.reflectionProbeData.reflectionProbeCount = asset->draw.reflectionProbeCount;
				new_asset->draw.reflectionProbeData.sharedReflectionProbeCount = 0;
				new_asset->draw.reflectionProbeData.reflectionProbes = allocator.allocate_array<zonetool::iw7::GfxReflectionProbe>(asset->draw.reflectionProbeCount);
				for (unsigned int i = 0; i < new_asset->draw.reflectionProbeData.reflectionProbeCount; i++)
				{
					new_asset->draw.reflectionProbeData.reflectionProbes[i].livePath = nullptr;
					memcpy(&new_asset->draw.reflectionProbeData.reflectionProbes[i].origin, &asset->draw.reflectionProbeOrigins[i].origin, sizeof(float[3]));
					memset(&new_asset->draw.reflectionProbeData.reflectionProbes[i].angles, 0, sizeof(float[3]));
					new_asset->draw.reflectionProbeData.reflectionProbes[i].probeInstanceCount = 0;
					new_asset->draw.reflectionProbeData.reflectionProbes[i].probeInstances = nullptr;
					new_asset->draw.reflectionProbeData.reflectionProbes[i].probeRelightingIndex = static_cast<unsigned int>(-1);
				}

				new_asset->draw.reflectionProbeData.reflectionProbeArrayImage = generate_reflection_probe_array_image(&asset->draw, allocator);

				new_asset->draw.reflectionProbeData.probeRelightingCount = 0;
				new_asset->draw.reflectionProbeData.probeRelightingData = nullptr;

				new_asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount = 0;
				new_asset->draw.reflectionProbeData.reflectionProbeGBufferImages = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeGBufferTextures = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeInstanceCount = 0;
				new_asset->draw.reflectionProbeData.reflectionProbeInstances = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleData = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBuffer = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBufferView = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBufferRWView = nullptr;

				// todo...
				new_asset->draw.lightmapReindexData;

				new_asset->draw.iesLookupTexture = allocator.allocate<zonetool::iw7::GfxImage>();
				new_asset->draw.iesLookupTexture->name = allocator.duplicate_string("*ieslookup");

				new_asset->draw.decalVolumeCollectionCount = 0;
				new_asset->draw.decalVolumeCollections = nullptr;
				
				COPY_ASSET(draw.lightmapOverridePrimary);
				COPY_ASSET(draw.lightmapOverrideSecondary);
				
				new_asset->draw.lightMapCount = asset->draw.lightmapCount;
				new_asset->draw.lightMaps = allocator.allocate_array<zonetool::iw7::GfxLightMap*>(asset->draw.lightmapCount);
				for (int i = 0; i < asset->draw.lightmapCount; i++)
				{
					new_asset->draw.lightMaps[i] = allocator.allocate<zonetool::iw7::GfxLightMap>();
					new_asset->draw.lightMaps[i]->name = allocator.duplicate_string(utils::string::va("*lightmap%d", i));
					if (asset->draw.lightmaps[i].primary) // primary
					{
						new_asset->draw.lightMaps[i]->textures[0] = allocator.allocate<zonetool::iw7::GfxImage>();
						new_asset->draw.lightMaps[i]->textures[0]->name = asset->draw.lightmaps[i].primary->name;
					}
					if (asset->draw.lightmaps[i].secondary) // secondary
					{
						new_asset->draw.lightMaps[i]->textures[1] = allocator.allocate<zonetool::iw7::GfxImage>();
						new_asset->draw.lightMaps[i]->textures[1]->name = asset->draw.lightmaps[i].secondary->name;
					}
					new_asset->draw.lightMaps[i]->textures[2] = nullptr; // secondunorm
				}
				new_asset->draw.lightmapTextures = nullptr; // runtime data, allocated elsewhere

				new_asset->draw.unused1 = nullptr;
				new_asset->draw.unused2 = nullptr;
				new_asset->draw.unused3 = nullptr;

				new_asset->draw.transientZoneCount = 1;
				new_asset->draw.transientZones[0] = allocator.allocate<zonetool::iw7::GfxWorldTransientZone>();
				new_asset->draw.transientZones[0]->name = allocator.duplicate_string(filesystem::get_fastfile());
				new_asset->draw.transientZones[0]->transientZoneIndex = 0;

				new_asset->draw.transientZones[0]->vertexCount = asset->draw.vertexCount;
				new_asset->draw.transientZones[0]->vd.vertices = allocator.allocate_array<zonetool::iw7::GfxWorldVertex>(asset->draw.vertexCount);
				for (unsigned int i = 0; i < asset->draw.vertexCount; i++)
				{
					static_assert(sizeof(zonetool::h1::GfxWorldVertex) == sizeof(zonetool::iw7::GfxWorldVertex));
					memcpy(&new_asset->draw.transientZones[0]->vd.vertices[i], &asset->draw.vd.vertices[i], sizeof(zonetool::h1::GfxWorldVertex));

					//float default_visibility[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
					//new_asset->draw.transientZones[0]->vd.vertices[i].selfVisibility.packed = self_visibility::XSurfacePackSelfVisibility(default_visibility);
				}

				new_asset->draw.transientZones[0]->vertexLayerDataSize = asset->draw.vertexLayerDataSize;
				new_asset->draw.transientZones[0]->vld.data = asset->draw.vld.data;

				new_asset->draw.transientZones[0]->cellCount = asset->dpvsPlanes.cellCount;

				new_asset->draw.transientZones[0]->aabbTreeCounts = allocator.allocate_array<zonetool::iw7::GfxCellTreeCount>(asset->dpvsPlanes.cellCount);
				new_asset->draw.transientZones[0]->aabbTrees = allocator.allocate_array<zonetool::iw7::GfxCellTree>(asset->dpvsPlanes.cellCount);
				for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
				{
					new_asset->draw.transientZones[0]->aabbTreeCounts[i].aabbTreeCount = asset->aabbTreeCounts[i].aabbTreeCount;
					new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree = allocator.allocate_array<zonetool::iw7::GfxAabbTree>(asset->aabbTreeCounts[i].aabbTreeCount);
					for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
					{
						memcpy(&new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].bounds, &asset->aabbTrees[i].aabbTree[j].bounds, sizeof(float[2][3]));

						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].startSurfIndex = asset->aabbTrees[i].aabbTree[j].startSurfIndex;
						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].surfaceCount = asset->aabbTrees[i].aabbTree[j].surfaceCount;

						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].smodelIndexCount = asset->aabbTrees[i].aabbTree[j].smodelIndexCount;
						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].smodelIndexes = asset->aabbTrees[i].aabbTree[j].smodelIndexes;

						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].childCount = asset->aabbTrees[i].aabbTree[j].childCount;

						// re-calculate childrenOffset
						auto offset = asset->aabbTrees[i].aabbTree[j].childrenOffset;
						int childrenIndex = offset / sizeof(zonetool::h1::GfxAabbTree);
						int childrenOffset = childrenIndex * sizeof(zonetool::iw7::GfxAabbTree);
						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
					}
				}

				new_asset->draw.indexCount = asset->draw.indexCount;
				new_asset->draw.indices = asset->draw.indices;

				new_asset->draw.volumetrics.volumetricCount = 0;
				new_asset->draw.volumetrics.volumetrics = nullptr;
				new_asset->lightGrid.unk;
				COPY_VALUE(lightGrid.tableVersion);
				COPY_VALUE(lightGrid.paletteVersion);
				COPY_VALUE(lightGrid.rangeExponent8BitsEncoding);
				COPY_VALUE(lightGrid.rangeExponent12BitsEncoding);
				COPY_VALUE(lightGrid.rangeExponent16BitsEncoding);
				COPY_VALUE(lightGrid.stageCount);
				COPY_VALUE(lightGrid.stageLightingContrastGain);
				COPY_VALUE(lightGrid.paletteEntryCount);
				COPY_VALUE(lightGrid.paletteEntryAddress);
				COPY_VALUE(lightGrid.paletteBitstreamSize);
				COPY_VALUE(lightGrid.paletteBitstream);
				for (unsigned int j = 0; j < 56; j++)
				{
					// fixme?...
					//auto& rgb = asset->lightGrid.skyLightGridColors.rgb[j];
					//auto& dest_rgb = new_asset->lightGrid.skyLightGridColors.rgb[j];
					//dest_rgb[0] = half_to_float(rgb[0]);
					//dest_rgb[1] = half_to_float(rgb[1]);
					//dest_rgb[2] = half_to_float(rgb[2]);
				}
				for (unsigned int j = 0; j < 56; j++)
				{
					// fixme?...
					//auto& rgb = asset->lightGrid.defaultLightGridColors.rgb[j];
					//auto& dest_rgb = new_asset->lightGrid.defaultLightGridColors.rgb[j];
					//dest_rgb[0] = half_to_float(rgb[0]);
					//dest_rgb[1] = half_to_float(rgb[1]);
					//dest_rgb[2] = half_to_float(rgb[2]);
				}
				new_asset->lightGrid.tree.maxDepth = asset->lightGrid.tree[0].maxDepth;
				new_asset->lightGrid.tree.nodeCount = asset->lightGrid.tree[0].nodeCount;
				new_asset->lightGrid.tree.leafCount = asset->lightGrid.tree[0].leafCount;
				memcpy(&new_asset->lightGrid.tree.coordMinGridSpace, &asset->lightGrid.tree[0].coordMinGridSpace, sizeof(int[3]));
				memcpy(&new_asset->lightGrid.tree.coordMaxGridSpace, &asset->lightGrid.tree[0].coordMaxGridSpace, sizeof(int[3]));
				memcpy(&new_asset->lightGrid.tree.coordHalfSizeGridSpace, &asset->lightGrid.tree[0].coordHalfSizeGridSpace, sizeof(int[3]));
				new_asset->lightGrid.tree.defaultColorIndexBitCount = asset->lightGrid.tree[0].defaultColorIndexBitCount;
				new_asset->lightGrid.tree.defaultLightIndexBitCount = asset->lightGrid.tree[0].defaultLightIndexBitCount;
				new_asset->lightGrid.tree.p_nodeTable = asset->lightGrid.tree[0].p_nodeTable;
				new_asset->lightGrid.tree.leafTableSize = asset->lightGrid.tree[0].leafTableSize;
				new_asset->lightGrid.tree.p_leafTable = asset->lightGrid.tree[0].p_leafTable;
				
				memset(&new_asset->lightGrid.probeData, 0, sizeof(zonetool::iw7::GfxLightGridProbeData));
				// fixme somehow...
				new_asset->lightGrid.probeData.zoneCount = 1;
				new_asset->lightGrid.probeData.zones = allocator.allocate_array<zonetool::iw7::GfxGpuLightGridZone>(1);
				new_asset->lightGrid.probeData.zones->numProbes = 0;
				new_asset->lightGrid.probeData.zones->firstProbe = 0;
				new_asset->lightGrid.probeData.zones->numTetrahedrons = 0;
				new_asset->lightGrid.probeData.zones->firstTetrahedron = 0;
				new_asset->lightGrid.probeData.zones->firstVoxelTetrahedronIndex = 0;
				new_asset->lightGrid.probeData.zones->numVoxelTetrahedronIndices = 0;
				memset(new_asset->lightGrid.probeData.zones->fallbackProbeData.coeffs, 0, sizeof(new_asset->lightGrid.probeData.zones->fallbackProbeData.coeffs));
				memset(new_asset->lightGrid.probeData.zones->fallbackProbeData.pad, 0, sizeof(new_asset->lightGrid.probeData.zones->fallbackProbeData.pad));

				// don't need to fill this data i guess?
				new_asset->frustumLights = allocator.allocate_array<zonetool::iw7::GfxFrustumLights>(new_asset->primaryLightCount);
				new_asset->lightViewFrustums = allocator.allocate_array<zonetool::iw7::GfxLightViewFrustum>(new_asset->primaryLightCount);

				new_asset->voxelTreeCount = new_asset->skyCount;
				new_asset->voxelTree = allocator.allocate_array<zonetool::iw7::GfxVoxelTree>(new_asset->voxelTreeCount);
				for (auto i = 0; i < new_asset->skyCount; i++)
				{
					memcpy(&new_asset->voxelTree[i].zoneBound, &asset->skies[i].bounds, sizeof(Bounds));
					
					new_asset->voxelTree[i].voxelTopDownViewNodeCount = 1;
					new_asset->voxelTree[i].voxelInternalNodeCount = 1;
					new_asset->voxelTree[i].voxelLeafNodeCount = 1;
					new_asset->voxelTree[i].lightListArraySize = 1;

					new_asset->voxelTree[i].voxelTreeHeader = allocator.allocate<zonetool::iw7::GfxVoxelTreeHeader>();
					memset(&new_asset->voxelTree[i].voxelTreeHeader->rootNodeDimension, 0, sizeof(int[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->nodeCoordBitShift, 0, sizeof(int[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->boundMin, 0, sizeof(float[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->boundMax, 0, sizeof(float[4]));

					new_asset->voxelTree[i].voxelTopDownViewNodeArray = allocator.allocate_array<zonetool::iw7::GfxVoxelTopDownViewNode>(new_asset->voxelTree[i].voxelTopDownViewNodeCount);
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->firstNodeIndex = -1;
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->zMin = 2147483647;
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->zMax = -2147483648;
					
					new_asset->voxelTree[i].voxelInternalNodeArray = allocator.allocate_array<zonetool::iw7::GfxVoxelInternalNode>(new_asset->voxelTree[i].voxelInternalNodeCount);
					new_asset->voxelTree[i].voxelInternalNodeArray->firstNodeIndex[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->firstNodeIndex[1] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->childNodeMask[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->childNodeMask[1] = 0;

					new_asset->voxelTree[i].voxelLeafNodeArray = allocator.allocate_array<zonetool::iw7::GfxVoxelLeafNode>(new_asset->voxelTree[i].voxelLeafNodeCount);
					new_asset->voxelTree[i].voxelLeafNodeArray->lightListAddress = 0;

					new_asset->voxelTree[i].lightListArray = allocator.allocate_array<unsigned short>(new_asset->voxelTree[i].lightListArraySize);
					new_asset->voxelTree[i].lightListArray[0] = 0;

					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList = allocator.allocate_array<unsigned int>(2 * new_asset->voxelTree[i].voxelInternalNodeCount);
					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList[1] = 0;
				}

				new_asset->heightfieldCount = 0;
				new_asset->heightfields = nullptr;

				new_asset->unk01.unk01Count = 0;
				new_asset->unk01.unk01 = nullptr;
				new_asset->unk01.unk02Count = 0;
				new_asset->unk01.unk02 = nullptr;
				new_asset->unk01.unk03Count = 0;
				new_asset->unk01.unk03 = nullptr;
				
				COPY_VALUE(modelCount);
				new_asset->models = allocator.allocate_array<zonetool::iw7::GfxBrushModel>(asset->modelCount);
				for (int i = 0; i < asset->modelCount; i++)
				{
					COPY_ARR(models[i].bounds);
					COPY_VALUE(models[i].radius);
					COPY_VALUE(models[i].startSurfIndex);
					COPY_VALUE(models[i].surfaceCount);
				}

				std::memcpy(&new_asset->bounds, &asset->unkBounds, sizeof(Bounds));

				COPY_VALUE(checksum);
				COPY_VALUE(materialMemoryCount);
				REINTERPRET_CAST_SAFE(materialMemory);
				COPY_VALUE_CAST(sun);
				COPY_ARR(outdoorLookupMatrix);
				COPY_ASSET(outdoorImage);
				new_asset->dustMaterial = nullptr;
				new_asset->materialLod0SizeThreshold = 0.5f;
				
				new_asset->primaryLightMotionDetectBitsEntries = 0;
				new_asset->entityMotionBitsEntries = 134;
				new_asset->staticSpotOmniPrimaryLightCountAligned = 0;
				new_asset->numPrimaryLightEntityShadowVisEntries = 0;
				new_asset->dynEntMotionBitsEntries[0] = 0;
				new_asset->dynEntMotionBitsEntries[1] = 0;
				new_asset->numPrimaryLightDynEntShadowVisEntries[0] = 0;
				new_asset->numPrimaryLightDynEntShadowVisEntries[1] = 0;

				REINTERPRET_CAST_SAFE(shadowGeomOptimized);
				REINTERPRET_CAST_SAFE(lightRegion);

				new_asset->lightAABB.nodeCount = 0;
				new_asset->lightAABB.lightCount = 0;
				new_asset->lightAABB.nodeArray = nullptr;
				new_asset->lightAABB.lightArray = nullptr;

				// dpvs
				{
					COPY_VALUE(dpvs.smodelCount);
					COPY_VALUE(dpvs.staticSurfaceCount);
					COPY_VALUE(dpvs.litOpaqueSurfsBegin);
					COPY_VALUE(dpvs.litOpaqueSurfsEnd);
					COPY_VALUE(dpvs.litDecalSurfsBegin);
					COPY_VALUE(dpvs.litDecalSurfsEnd);
					COPY_VALUE(dpvs.litTransSurfsBegin);
					COPY_VALUE(dpvs.litTransSurfsEnd);
					COPY_VALUE(dpvs.emissiveSurfsBegin);
					COPY_VALUE(dpvs.emissiveSurfsEnd);
					COPY_VALUE(dpvs.smodelVisDataCount);
					COPY_VALUE(dpvs.surfaceVisDataCount);
					new_asset->dpvs.primaryLightVisDataCount = (new_asset->primaryLightCount + 31) >> 5;
					new_asset->dpvs.reflectionProbeVisDataCount = (new_asset->draw.reflectionProbeData.reflectionProbeInstanceCount + 31) >> 5;
					new_asset->dpvs.volumetricVisDataCount = (new_asset->draw.volumetrics.volumetricCount + 31) >> 5;
					new_asset->dpvs.decalVisDataCount = (new_asset->draw.decalVolumeCollectionCount + 31) >> 5;
					REINTERPRET_CAST_SAFE(dpvs.lodData);
					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);

					new_asset->dpvs.surfaces = allocator.allocate_array<zonetool::iw7::GfxSurface>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						COPY_VALUE(dpvs.surfaces[i].tris.vertexLayerData);
						COPY_VALUE(dpvs.surfaces[i].tris.firstVertex);
						COPY_VALUE(dpvs.surfaces[i].tris.maxEdgeLength);
						COPY_VALUE(dpvs.surfaces[i].tris.vertexCount);
						COPY_VALUE(dpvs.surfaces[i].tris.triCount);
						COPY_VALUE(dpvs.surfaces[i].tris.baseIndex);
						new_asset->dpvs.surfaces[i].material = reinterpret_cast<zonetool::iw7::Material*>(asset->dpvs.surfaces[i].material);
						new_asset->dpvs.surfaces[i].lightmapIndex = asset->dpvs.surfaces[i].laf.fields.lightmapIndex;
						new_asset->dpvs.surfaces[i].flags = asset->dpvs.surfaces[i].laf.fields.flags;

						new_asset->dpvs.surfaces[i].transientZone = 0;
					}

					new_asset->dpvs.surfacesBounds = allocator.allocate_array<zonetool::iw7::GfxSurfaceBounds>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						COPY_ARR(dpvs.surfacesBounds[i].bounds);
					}

					new_asset->dpvs.smodelDrawInsts = allocator.allocate_array<zonetool::iw7::GfxStaticModelDrawInst>(asset->dpvs.smodelCount);
					for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
					{
						COPY_VALUE_CAST(dpvs.smodelDrawInsts[i].placement);

						new_asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.numLightingValues = asset->dpvs.smodelLighting[i].vertexLightingInfo.numLightingValues;
						new_asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues = 
							reinterpret_cast<zonetool::iw7::GfxStaticModelVertexLighting*>(asset->dpvs.smodelLighting[i].vertexLightingInfo.lightingValues);

						new_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.lightmapIndex = -1;

						new_asset->dpvs.smodelDrawInsts[i].lightingHandle = asset->dpvs.smodelDrawInsts[i].lightingHandle;
						new_asset->dpvs.smodelDrawInsts[i].cullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
						new_asset->dpvs.smodelDrawInsts[i].flags = asset->dpvs.smodelDrawInsts[i].flags;
						new_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex;
						new_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
						new_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
						new_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = asset->dpvs.smodelDrawInsts[i].sunShadowFlags;

						new_asset->dpvs.smodelDrawInsts[i].transientZone = 0;
					}

					//REINTERPRET_CAST_SAFE(dpvs.surfaceMaterials);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
					COPY_VALUE(dpvs.sunShadowOptCount);
					COPY_VALUE(dpvs.sunSurfVisDataCount);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
					new_asset->dpvs.sortedSmodelIndices = allocator.allocate_array<unsigned short>(asset->dpvs.smodelCount); // todo...
					//REINTERPRET_CAST_SAFE(dpvs.constantBuffers);
					COPY_VALUE(dpvs.usageCount);
				}

				COPY_ARR(dpvsDyn.dynEntClientWordCount);
				COPY_ARR(dpvsDyn.dynEntClientCount);
				COPY_ARR(dpvsDyn.dynEntCellBits);
				//dynEntVisData

				new_asset->dpvsDyn.dynEntClientWordCount[0] = 2;
				new_asset->dpvsDyn.dynEntClientCount[0] = 64;

				COPY_VALUE(mapVtxChecksum);
				COPY_VALUE(heroOnlyLightCount);
				REINTERPRET_CAST_SAFE(heroOnlyLights);

				// Umbra needs to have valid data...
				new_asset->numUmbraGates = 0;
				new_asset->umbraGates = nullptr;
				COPY_VALUE(umbraTomeSize);
				REINTERPRET_CAST_SAFE(umbraTomeData);
				COPY_VALUE_CAST(umbraTomePtr);

				new_asset->numUmbraGates2 = 0;
				new_asset->umbraGates2 = nullptr;
				new_asset->umbraTomeSize2 = 0;
				new_asset->umbraTomeData2 = nullptr;
				new_asset->umbraTomePtr2 = nullptr;
				new_asset->umbraUnkSize = 0;
				new_asset->umbraUnkData = nullptr;

				return new_asset;
			}

			void dump(GfxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::iw7::gfx_world::dump(converted_asset);

				for (int i = 0; i < asset->draw.lightmapCount; i++)
				{
					zonetool::iw7::gfx_light_map::dump(converted_asset->draw.lightMaps[i]);
				}

				zonetool::iw7::gfx_world_tr::dump(converted_asset->draw.transientZones[0]);
			}
		}
	}
}
