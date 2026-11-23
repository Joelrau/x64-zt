#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "gfx_image.hpp"
#include "xmodel.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace gfxworld
	{
		namespace
		{
			template <typename Destination, typename Source>
			void copy_layout(Destination& destination, const Source& source)
			{
				static_assert(sizeof(Destination) == sizeof(Source));
				std::memcpy(&destination, &source, sizeof(Destination));
			}

			zonetool::iw5::GfxImage* image_reference(const x86::pointer<zonetool::iw4::GfxImage>& image, x86::allocator& allocator)
			{
				if (!image.value)
				{
					return nullptr;
				}

				const auto reference = allocator.allocate<zonetool::iw5::GfxImage>();
				reference->name.value = image->name.value;
				return reference;
			}

			zonetool::iw5::GfxImage* map_image(const x86::pointer<zonetool::iw4::GfxImage>& image)
			{
				return image.value ? gfx_image::convert(image.get()) : nullptr;
			}

			void convert_cells(zonetool::iw4::GfxWorld* asset, zonetool::iw5::GfxWorld* new_asset, x86::allocator& allocator)
			{
				const auto cell_count = asset->dpvsPlanes.cellCount;
				const auto cells = allocator.allocate<zonetool::iw5::GfxCell>(cell_count);
				const auto trees = allocator.allocate<zonetool::iw5::GfxCellTree>(cell_count);

				for (auto i = 0; i < cell_count; i++)
				{
					const auto& source = asset->cells[i];
					auto& destination = cells[i];

					copy_layout(destination.bounds, source.bounds);
					destination.portalCount = source.portalCount;
					destination.reflectionProbeCount = static_cast<unsigned char>(source.reflectionProbeCount);
					destination.reflectionProbes.value = source.reflectionProbes.value;
					destination.reflectionProbeReferenceCount = 0;

					if (source.portals.value)
					{
						const auto portals = allocator.allocate<zonetool::iw5::GfxPortal>(source.portalCount);
						for (auto j = 0; j < source.portalCount; j++)
						{
							const auto& portal = source.portals[j];
							auto& portal_destination = portals[j];

							portal_destination.writable.isQueued = portal.writable.isQueued;
							portal_destination.writable.isAncestor = portal.writable.isAncestor;
							portal_destination.writable.recursionDepth = static_cast<unsigned char>(portal.writable.recursionDepth);
							portal_destination.writable.hullPointCount = static_cast<unsigned char>(portal.writable.hullPointCount);
							portal_destination.writable.hullPoints.value = portal.writable.hullPoints.value;

							const auto parent = portal.writable.queuedParent.value;
							if (parent >= source.portals.value && (parent - source.portals.value) / sizeof(zonetool::iw4::GfxPortal) < static_cast<unsigned int>(source.portalCount))
							{
								portal_destination.writable.queuedParent = &portals[(parent - source.portals.value) / sizeof(zonetool::iw4::GfxPortal)];
							}

							std::memcpy(portal_destination.plane.coeffs, portal.plane.coeffs, sizeof(portal.plane.coeffs));
							portal_destination.vertices.value = portal.vertices.value;
							portal_destination.cellIndex = portal.cellIndex;
							portal_destination.vertexCount = static_cast<unsigned char>(portal.vertexCount);
							std::memcpy(portal_destination.hullAxis, portal.hullAxis, sizeof(portal.hullAxis));
						}
						destination.portals = portals;
					}

					const auto tree_count = asset->aabbTreeCounts[i].aabbTreeCount;
					if (asset->aabbTree[i].aabbtree.value)
					{
						const auto aabb_trees = allocator.allocate<zonetool::iw5::GfxAabbTree>(tree_count);
						for (auto tree = 0; tree < tree_count; tree++)
						{
							const auto& source_tree = asset->aabbTree[i].aabbtree[tree];
							auto& tree_destination = aabb_trees[tree];

							copy_layout(tree_destination.bounds, source_tree.bounds);
							tree_destination.childCount = source_tree.childCount;
							tree_destination.surfaceCount = source_tree.surfaceCount;
							tree_destination.startSurfIndex = source_tree.startSurfIndex;
							tree_destination.surfaceCountNoDecal = static_cast<unsigned short>(source_tree.unkn & 0xFFFF); // parked by the IW3 stage
							tree_destination.startSurfIndexNoDecal = static_cast<unsigned short>((static_cast<unsigned int>(source_tree.unkn) >> 16) & 0xFFFF);
							tree_destination.smodelIndexCount = source_tree.smodelIndexCount;
							tree_destination.smodelIndexes.value = source_tree.smodelIndexes.value;
							tree_destination.childrenOffset = source_tree.childrenOffset;
						}
						trees[i].aabbTree = aabb_trees;
					}
				}

				new_asset->cells = cells;
				new_asset->aabbTreeCounts.value = asset->aabbTreeCounts.value;
				new_asset->aabbTrees = trees;
			}

			void convert_draw(zonetool::iw4::GfxWorld* asset, zonetool::iw5::GfxWorld* new_asset, x86::allocator& allocator)
			{
				const auto& source = asset->worldDraw;
				auto& draw = new_asset->draw;

				draw.reflectionProbeCount = source.reflectionProbeCount;
				const auto images = allocator.allocate<x86::pointer<zonetool::iw5::GfxImage>>(source.reflectionProbeCount);
				for (unsigned int i = 0; i < source.reflectionProbeCount; i++)
				{
					images[i] = map_image(source.reflectionImages[i]);
				}
				draw.reflectionProbes = images;

				const auto origins = allocator.allocate<zonetool::iw5::GfxReflectionProbe>(source.reflectionProbeCount);
				for (unsigned int i = 0; i < source.reflectionProbeCount; i++)
				{
					std::memcpy(origins[i].origin, source.reflectionProbes[i].offset, sizeof(origins[i].origin));
				}
				draw.reflectionProbeOrigins = origins;
				draw.reflectionProbeTextures.value = source.reflectionProbeTextures.value;

				draw.reflectionProbeReferenceCount = 0;

				draw.lightmapCount = source.lightmapCount;
				const auto lightmaps = allocator.allocate<zonetool::iw5::GfxLightmapArray>(source.lightmapCount);
				for (auto i = 0; i < source.lightmapCount; i++)
				{
					lightmaps[i].primary = map_image(source.lightmaps[i].primary);
					lightmaps[i].secondary = map_image(source.lightmaps[i].secondary);
				}
				draw.lightmaps = lightmaps;
				draw.lightmapPrimaryTextures.value = source.lightmapPrimaryTextures.value;
				draw.lightmapSecondaryTextures.value = source.lightmapSecondaryTextures.value;

				draw.vertexCount = source.vertexCount;
				draw.vd.vertices.value = source.vd.vertices.value;
				draw.vertexLayerDataSize = source.vertexLayerDataSize;
				draw.vld.data.value = source.vld.data.value;
				draw.indexCount = static_cast<unsigned int>(source.indexCount);
				draw.indices.value = source.indices.value;
			}

			void convert_dpvs(zonetool::iw4::GfxWorld* asset, zonetool::iw5::GfxWorld* new_asset, x86::allocator& allocator)
			{
				const auto& source = asset->dpvs;
				auto& dpvs = new_asset->dpvs;

				dpvs.smodelCount = source.smodelCount;
				dpvs.staticSurfaceCount = source.staticSurfaceCount;
				dpvs.staticSurfaceCountNoDecal = source.staticSurfaceCountNoDecal;
				dpvs.litOpaqueSurfsBegin = source.litOpaqueSurfsBegin;
				dpvs.litOpaqueSurfsEnd = source.litOpaqueSurfsEnd;
				dpvs.litTransSurfsBegin = source.litTransSurfsBegin;
				dpvs.litTransSurfsEnd = source.litTransSurfsEnd;
				dpvs.shadowCasterSurfsBegin = source.shadowCasterSurfsBegin;
				dpvs.shadowCasterSurfsEnd = source.shadowCasterSurfsEnd;
				dpvs.emissiveSurfsBegin = source.emissiveSurfsBegin;
				dpvs.emissiveSurfsEnd = source.emissiveSurfsEnd;
				dpvs.smodelVisDataCount = source.smodelVisDataCount;
				dpvs.surfaceVisDataCount = source.surfaceVisDataCount;

				for (auto i = 0; i < 3; i++)
				{
					dpvs.smodelVisData[i].value = source.smodelVisData[i].value;
					dpvs.surfaceVisData[i].value = source.surfaceVisData[i].value;
				}

				dpvs.sortedSurfIndex.value = source.sortedSurfIndex.value;
				dpvs.surfaceCastsSunShadow.value = source.surfaceCastsSunShadow.value;
				dpvs.usageCount = source.usageCount;

				static_assert(sizeof(zonetool::iw5::GfxStaticModelInst) == sizeof(zonetool::iw4::GfxStaticModelInst));
				dpvs.smodelInsts.value = source.smodelInsts.value;

				const auto draw_instances = allocator.allocate<zonetool::iw5::GfxStaticModelDrawInst>(source.smodelCount);
				std::unordered_map<zonetool::iw4::XModel*, zonetool::iw5::XModel*> models;
				for (unsigned int i = 0; i < source.smodelCount; i++)
				{
					copy_layout(draw_instances[i], source.smodelDrawInsts[i]);
					if (const auto model = source.smodelDrawInsts[i].model.get())
					{
						auto& new_model = models[model];
						if (!new_model)
						{
							new_model = xmodel::convert(model, allocator);
						}

						draw_instances[i].model = new_model;
					}
				}
				dpvs.smodelDrawInsts = draw_instances;

				static_assert(sizeof(zonetool::iw5::GfxSurfaceBounds) == sizeof(zonetool::iw4::GfxSurfaceBounds));
				dpvs.surfacesBounds.value = source.surfacesBounds.value;

				static_assert(sizeof(zonetool::iw5::GfxSurface) == sizeof(zonetool::iw4::GfxSurface));
				static_assert(sizeof(zonetool::iw5::GfxSurfaceLightingAndFlagsFields) == 4);
				dpvs.surfaces.value = source.surfaces.value; // the four lighting bytes sit where the IW5 union expects them

				const auto materials = allocator.allocate<zonetool::iw5::GfxDrawSurf>(asset->surfaceCount);
				for (unsigned int i = 0; i < asset->surfaceCount; i++)
				{
					const auto& fields = source.surfaceMaterials[i].fields;
					auto& destination = materials[i].fields;
					destination.objectId = fields.objectId;
					destination.reflectionProbeIndex = fields.reflectionProbeIndex;
					destination.hasGfxEntIndex = fields.hasGfxEntIndex;
					destination.customIndex = fields.customIndex;
					destination.materialSortedIndex = fields.materialSortedIndex;
					destination.prepass = fields.prepass;
					destination.useHeroLighting = fields.useHeroLighting;
					destination.sceneLightIndex = fields.sceneLightIndex;
					destination.surfType = fields.surfType;
					destination.primarySortKey = fields.primarySortKey;
					destination.unused = fields.unused;
					destination.viewModelRender = 0;
				}
				dpvs.surfaceMaterials = materials;
			}
		}

		zonetool::iw5::GfxWorld* convert(zonetool::iw4::GfxWorld* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw5::GfxWorld>();

			COPY_POINTER_RAW(name);
			COPY_POINTER_RAW(baseName);
			COPY_VALUE(planeCount);
			COPY_VALUE(nodeCount);
			COPY_VALUE(surfaceCount);

			new_asset->skyCount = asset->skyCount;
			const auto skies = allocator.allocate<zonetool::iw5::GfxSky>(asset->skyCount);
			for (auto i = 0; i < asset->skyCount; i++)
			{
				skies[i].skySurfCount = asset->skies[i].skySurfCount;
				skies[i].skyStartSurfs.value = asset->skies[i].skyStartSurfs.value;
				skies[i].skyImage = image_reference(asset->skies[i].skyImage, allocator);
				skies[i].skySamplerState = static_cast<unsigned char>(asset->skies[i].skySamplerState);
			}
			new_asset->skies = skies;

			COPY_VALUE(lastSunPrimaryLightIndex);
			COPY_VALUE(primaryLightCount);
			COPY_VALUE(sortKeyLitDecal);
			COPY_VALUE(sortKeyEffectDecal);
			COPY_VALUE(sortKeyEffectAuto);
			COPY_VALUE(sortKeyDistortion);

			copy_layout(new_asset->dpvsPlanes, asset->dpvsPlanes);
			convert_cells(asset, new_asset, allocator);
			convert_draw(asset, new_asset, allocator);
			convert_dpvs(asset, new_asset, allocator);

			auto& grid = new_asset->lightGrid;
			const auto& source_grid = asset->lightGrid;
			grid.hasLightRegions = source_grid.hasLightRegions;
			grid.useSkyForLowZ = false;
			grid.lastSunPrimaryLightIndex = source_grid.sunPrimaryLightIndex;
			std::memcpy(grid.mins, source_grid.mins, sizeof(grid.mins));
			std::memcpy(grid.maxs, source_grid.maxs, sizeof(grid.maxs));
			grid.rowAxis = source_grid.rowAxis;
			grid.colAxis = source_grid.colAxis;
			grid.rowDataStart.value = source_grid.rowDataStart.value;
			grid.rawRowDataSize = source_grid.rawRowDataSize;
			grid.rawRowData.value = source_grid.rawRowData.value;
			grid.entryCount = source_grid.entryCount;
			grid.entries.value = source_grid.entries.value;
			grid.colorCount = source_grid.colorCount;
			grid.colors.value = source_grid.colors.value;

			copy_layout(new_asset->dpvsDyn, asset->dpvsDyn);

			COPY_VALUE(modelCount);
			static_assert(sizeof(zonetool::iw5::GfxBrushModel) == sizeof(zonetool::iw4::GfxBrushModel));
			COPY_POINTER_RAW(models);
			copy_layout(new_asset->bounds, asset->bounds);
			COPY_VALUE(checksum);
			COPY_VALUE(materialMemoryCount);
			COPY_POINTER_RAW(materialMemory);
			copy_layout(new_asset->sun, asset->sun);
			COPY_ARR(outdoorLookupMatrix);
			new_asset->outdoorImage = image_reference(asset->outdoorImage, allocator);

			new_asset->cellCasterBits.value = asset->cellCasterBits[0].value;
			new_asset->cellHasSunLitSurfsBits.value = 1; // must not be null

			COPY_POINTER_RAW(sceneDynModel);
			COPY_POINTER_RAW(sceneDynBrush);
			COPY_POINTER_RAW(primaryLightEntityShadowVis);
			new_asset->primaryLightDynEntShadowVis[0].value = asset->primaryLightDynEntShadowVis[0].value;
			new_asset->primaryLightDynEntShadowVis[1].value = asset->primaryLightDynEntShadowVis[1].value;
			new_asset->nonSunPrimaryLightForModelDynEnt.value = asset->primaryLightForModelDynEnt.value;
			COPY_POINTER_RAW(shadowGeom);
			COPY_POINTER_RAW(lightRegion);

			COPY_VALUE(mapVtxChecksum);
			new_asset->heroOnlyLightCount = 0;
			COPY_VALUE(fogTypesAllowed);

			return new_asset;
		}
	}
}
