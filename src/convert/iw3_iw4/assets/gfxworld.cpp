#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "gfx_image.hpp"
#include "xmodel.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace gfxworld
	{
		namespace
		{
			void compute_bounds(zonetool::iw4::Bounds& bounds, const float* mins, const float* maxs)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					bounds.midPoint[axis] = (mins[axis] + maxs[axis]) * 0.5f;
					bounds.halfSize[axis] = (maxs[axis] - mins[axis]) * 0.5f;
				}
			}

			zonetool::iw4::GfxImage* image_reference(const x86::pointer<zonetool::iw3::GfxImage>& image, x86::allocator& allocator)
			{
				if (!image.value)
				{
					return nullptr;
				}

				const auto reference = allocator.allocate<zonetool::iw4::GfxImage>();
				reference->name.value = image->name.value;
				return reference;
			}

			zonetool::iw4::GfxImage* map_image(const x86::pointer<zonetool::iw3::GfxImage>& image, x86::allocator& allocator)
			{
				return image.value ? gfx_image::convert(image.get(), allocator) : nullptr;
			}

			template <typename Destination, typename Source>
			void copy_layout(Destination& destination, const Source& source)
			{
				static_assert(sizeof(Destination) == sizeof(Source));
				std::memcpy(&destination, &source, sizeof(Destination));
			}

			// The IW4 aabb tree has no no-decal pair, so it is parked in the unknown field for the IW5 stage.
			void convert_aabb_tree(zonetool::iw4::GfxAabbTree& destination, const zonetool::iw3::GfxAabbTree& source)
			{
				compute_bounds(destination.bounds, source.mins, source.maxs);
				destination.unkn = static_cast<int>(source.surfaceCountNoDecal | (static_cast<unsigned int>(source.startSurfIndexNoDecal) << 16));
				destination.childCount = source.childCount;
				destination.surfaceCount = source.surfaceCount;
				destination.startSurfIndex = source.startSurfIndex;
				destination.smodelIndexCount = source.smodelIndexCount;
				destination.smodelIndexes.value = source.smodelIndexes.value;
				destination.childrenOffset = source.childrenOffset;
			}

			void convert_cells(zonetool::iw3::GfxWorld* asset, zonetool::iw4::GfxWorld* new_asset, x86::allocator& allocator)
			{
				const auto cell_count = asset->dpvsPlanes.cellCount;
				const auto cells = allocator.allocate<zonetool::iw4::GfxCell>(cell_count);
				const auto counts = allocator.allocate<zonetool::iw4::GfxCellTreeCount>(cell_count);
				const auto trees = allocator.allocate<zonetool::iw4::GfxCellTree>(cell_count);

				for (auto i = 0; i < cell_count; i++)
				{
					const auto& source = asset->cells[i];
					counts[i].aabbTreeCount = source.aabbTreeCount;

					compute_bounds(cells[i].bounds, source.mins, source.maxs);
					cells[i].portalCount = source.portalCount;
					cells[i].reflectionProbeCount = source.reflectionProbeCount;
					cells[i].reflectionProbes.value = source.reflectionProbes.value;

					if (source.aabbTree.value)
					{
						const auto aabb_trees = allocator.allocate<zonetool::iw4::GfxAabbTree>(source.aabbTreeCount);
						for (auto tree = 0; tree < source.aabbTreeCount; tree++)
						{
							convert_aabb_tree(aabb_trees[tree], source.aabbTree[tree]);
						}
						trees[i].aabbtree = aabb_trees;
					}

					if (!source.portals.value)
					{
						continue;
					}

					const auto portals = allocator.allocate<zonetool::iw4::GfxPortal>(source.portalCount);
					cells[i].portals = portals;

					for (auto j = 0; j < source.portalCount; j++)
					{
						const auto& portal = source.portals[j];
						auto& destination = portals[j];

						const auto target = (portal.cell.value - asset->cells.value) / sizeof(zonetool::iw3::GfxCell);
						destination.cellIndex = target < static_cast<unsigned int>(cell_count) ? static_cast<unsigned short>(target) : 0;
						destination.vertices.value = portal.vertices.value;
						destination.vertexCount = portal.vertexCount;

						destination.writable.isQueued = portal.writable.isQueued;
						destination.writable.isAncestor = portal.writable.isAncestor;
						destination.writable.recursionDepth = portal.writable.recursionDepth;
						destination.writable.hullPointCount = portal.writable.hullPointCount;
						destination.writable.hullPoints.value = portal.writable.hullPoints.value;

						const auto parent = portal.writable.queuedParent.value;
						if (parent >= source.portals.value && (parent - source.portals.value) / sizeof(zonetool::iw3::GfxPortal) < static_cast<unsigned int>(source.portalCount))
						{
							destination.writable.queuedParent = &portals[(parent - source.portals.value) / sizeof(zonetool::iw3::GfxPortal)];
						}

						std::memcpy(destination.plane.coeffs, portal.plane.coeffs, sizeof(destination.plane.coeffs));
						std::memcpy(destination.hullAxis, portal.hullAxis, sizeof(destination.hullAxis));
					}
				}

				new_asset->cells = cells;
				new_asset->aabbTreeCounts = counts;
				new_asset->aabbTree = trees;
			}

			void convert_draw(zonetool::iw3::GfxWorld* asset, zonetool::iw4::GfxWorld* new_asset, x86::allocator& allocator)
			{
				auto& draw = new_asset->worldDraw;

				draw.lightmapCount = asset->lightmapCount;
				const auto lightmaps = allocator.allocate<zonetool::iw4::GfxLightmapArray>(asset->lightmapCount);
				for (auto i = 0; i < asset->lightmapCount; i++)
				{
					lightmaps[i].primary = map_image(asset->lightmaps[i].primary, allocator);
					lightmaps[i].secondary = map_image(asset->lightmaps[i].secondary, allocator);
				}
				draw.lightmaps = lightmaps;

				draw.lightmapPrimaryTextures.value = asset->lightmapPrimaryTextures.value;
				draw.lightmapSecondaryTextures.value = asset->lightmapSecondaryTextures.value;

				draw.skyImage = image_reference(asset->skyImage, allocator);
				draw.outdoorImage = image_reference(asset->outdoorImage, allocator);

				draw.vertexCount = asset->vertexCount;
				draw.vd.vertices.value = asset->vd.vertices.value;
				draw.vertexLayerDataSize = asset->vertexLayerDataSize;
				draw.vld.data.value = asset->vld.data.value;

				draw.reflectionProbeCount = asset->reflectionProbeCount;
				const auto images = allocator.allocate<x86::pointer<zonetool::iw4::GfxImage>>(asset->reflectionProbeCount);
				const auto probes = allocator.allocate<zonetool::iw4::GfxReflectionProbe>(asset->reflectionProbeCount);
				for (unsigned int i = 0; i < asset->reflectionProbeCount; i++)
				{
					images[i] = map_image(asset->reflectionProbes[i].reflectionImage, allocator);
					std::memcpy(probes[i].offset, asset->reflectionProbes[i].origin, sizeof(probes[i].offset));
				}
				draw.reflectionImages.value = x86::address_space::offset_of(images);
				draw.reflectionProbes = probes;

				// Indices are rewritten so every surface owns a contiguous run in surface order.
				draw.indexCount = asset->indexCount;
				const auto indices = allocator.allocate<unsigned short>(asset->indexCount);
				auto base_index = 0u;
				for (auto i = 0; i < asset->surfaceCount; i++)
				{
					const auto& tris = asset->dpvs.surfaces[i].tris;
					const auto count = static_cast<unsigned int>(tris.triCount) * 3;
					if (base_index + count > static_cast<unsigned int>(asset->indexCount))
					{
						ZONETOOL_WARNING("gfxworld: surface %d runs past the index buffer", i);
						break;
					}

					std::memcpy(&indices[base_index], &asset->indices[tris.baseIndex], count * sizeof(unsigned short));
					base_index += count;
				}

				if (base_index != static_cast<unsigned int>(asset->indexCount))
				{
					ZONETOOL_WARNING("gfxworld: %u of %d indices belong to a surface", base_index, asset->indexCount);
				}
				draw.indices = indices;
			}

			void convert_dpvs(zonetool::iw3::GfxWorld* asset, zonetool::iw4::GfxWorld* new_asset, x86::allocator& allocator)
			{
				auto& source = asset->dpvs;
				auto& dpvs = new_asset->dpvs;

				const auto surface_count = asset->surfaceCount;
				const auto materials = allocator.allocate<zonetool::iw4::GfxDrawSurf>(surface_count);
				for (auto i = 0; i < surface_count; i++)
				{
					const auto& fields = source.surfaceMaterials[i].fields;
					auto& destination = materials[i].fields;
					destination.objectId = fields.objectId;
					destination.reflectionProbeIndex = fields.reflectionProbeIndex;
					destination.customIndex = fields.customIndex;
					destination.materialSortedIndex = fields.materialSortedIndex;
					destination.prepass = fields.prepass;
					destination.sceneLightIndex = fields.primaryLightIndex;
					destination.surfType = fields.surfType;
					destination.primarySortKey = fields.primarySortKey;
					destination.unused = fields.unused;
				}
				dpvs.surfaceMaterials = materials;

				dpvs.surfaceCastsSunShadow.value = source.surfaceCastsSunShadow.value;
				dpvs.usageCount = source.usageCount;

				dpvs.smodelCount = source.smodelCount;
				dpvs.staticSurfaceCount = source.staticSurfaceCount;
				dpvs.staticSurfaceCountNoDecal = source.staticSurfaceCountNoDecal;

				dpvs.litOpaqueSurfsBegin = source.litSurfsBegin;
				dpvs.litOpaqueSurfsEnd = source.decalSurfsEnd; // IW3 has no translucent or shadow caster split
				dpvs.litTransSurfsBegin = source.decalSurfsEnd;
				dpvs.litTransSurfsEnd = source.decalSurfsEnd;
				dpvs.shadowCasterSurfsBegin = source.decalSurfsEnd;
				dpvs.shadowCasterSurfsEnd = source.decalSurfsEnd;
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

				const auto model_instances = allocator.allocate<zonetool::iw4::GfxStaticModelInst>(source.smodelCount);
				const auto draw_instances = allocator.allocate<zonetool::iw4::GfxStaticModelDrawInst>(source.smodelCount);
				std::unordered_map<zonetool::iw3::XModel*, zonetool::iw4::XModel*> models;
				for (unsigned int i = 0; i < source.smodelCount; i++)
				{
					const auto& instance = source.smodelInsts[i];
					auto& destination = model_instances[i];
					compute_bounds(destination.bounds, instance.mins, instance.maxs);
					std::memcpy(destination.lightingOrigin, destination.bounds.midPoint, sizeof(destination.lightingOrigin)); // IW3 samples the grid at the model position

					const auto& draw = source.smodelDrawInsts[i];
					auto& draw_destination = draw_instances[i];
					std::memcpy(draw_destination.placement.origin, draw.placement.origin, sizeof(draw.placement.origin));
					std::memcpy(draw_destination.placement.axis, draw.placement.axis, sizeof(draw.placement.axis));
					draw_destination.placement.scale = draw.placement.scale;
					if (const auto model = draw.model.get())
					{
						auto& new_model = models[model];
						if (!new_model)
						{
							new_model = xmodel::convert(model, allocator);
						}

						draw_destination.model = new_model;
					}

					std::memcpy(draw_destination.cacheId, draw.smodelCacheIndex, sizeof(draw_destination.cacheId));

					draw_destination.cullDist = static_cast<unsigned short>(std::min(draw.cullDist, 65535.0f));
					draw_destination.reflectionProbeIndex = draw.reflectionProbeIndex;
					draw_destination.primaryLightIndex = draw.primaryLightIndex;
					draw_destination.lightingHandle = draw.lightingHandle;

					draw_destination.flags = 0;
					if (draw.flags & 0x1) // no shadow, the IW3 bit stays set because the IW4 renderer still reads it
					{
						draw_destination.flags |= 0x10 | 0x1;
					}

					std::memcpy(&draw_destination.groundLighting, &instance.groundLighting, sizeof(draw_destination.groundLighting));
					if (draw_destination.groundLighting.packed > 0)
					{
						draw_destination.flags |= 0x20;
					}
				}
				dpvs.smodelInsts = model_instances;
				dpvs.smodelDrawInsts = draw_instances;

				const auto surfaces = allocator.allocate<zonetool::iw4::GfxSurface>(surface_count);
				const auto surface_bounds = allocator.allocate<zonetool::iw4::GfxSurfaceBounds>(surface_count);
				auto base_index = 0u;
				for (auto i = 0; i < surface_count; i++)
				{
					const auto& surface = source.surfaces[i];
					copy_layout(surfaces[i].tris, surface.tris);
					surfaces[i].material.value = surface.material.value;
					surfaces[i].lightmapIndex = surface.lightmapIndex;
					surfaces[i].reflectionProbeIndex = surface.reflectionProbeIndex;
					surfaces[i].primaryLightIndex = surface.primaryLightIndex;
					surfaces[i].flags = surface.flags;
					compute_bounds(surface_bounds[i].bounds, surface.bounds[0], surface.bounds[1]);

					surfaces[i].tris.baseIndex = base_index; // matches the rewritten index runs
					base_index += static_cast<unsigned int>(surface.tris.triCount) * 3;
				}

				dpvs.surfaces = surfaces;
				dpvs.surfacesBounds = surface_bounds;
			}
		}

		zonetool::iw4::GfxWorld* convert(zonetool::iw3::GfxWorld* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::GfxWorld>();

			COPY_POINTER_RAW(name);
			COPY_POINTER_RAW(baseName);
			COPY_VALUE(planeCount);
			COPY_VALUE(nodeCount);
			new_asset->surfaceCount = static_cast<unsigned int>(asset->surfaceCount);

			const auto sky = allocator.allocate<zonetool::iw4::GfxSky>();
			sky->skySurfCount = asset->skySurfCount;
			sky->skyStartSurfs.value = asset->skyStartSurfs.value;
			sky->skyImage = image_reference(asset->skyImage, allocator);
			sky->skySamplerState = asset->skySamplerState;
			new_asset->skyCount = 1;
			new_asset->skies = sky;

			new_asset->lastSunPrimaryLightIndex = asset->sunPrimaryLightIndex;
			COPY_VALUE(primaryLightCount);

			new_asset->sortKeyLitDecal = 0x6;
			new_asset->sortKeyEffectDecal = 0x27;
			new_asset->sortKeyEffectAuto = 0x30;
			new_asset->sortKeyDistortion = 0x2b;

			copy_layout(new_asset->dpvsPlanes, asset->dpvsPlanes);
			convert_cells(asset, new_asset, allocator);
			convert_draw(asset, new_asset, allocator);
			convert_dpvs(asset, new_asset, allocator);

			copy_layout(new_asset->lightGrid, asset->lightGrid);
			copy_layout(new_asset->dpvsDyn, asset->dpvsDyn);

			new_asset->modelCount = asset->modelCount;
			const auto models = allocator.allocate<zonetool::iw4::GfxBrushModel>(asset->modelCount);
			for (auto i = 0; i < asset->modelCount; i++)
			{
				const auto& source = asset->models[i];
				compute_bounds(models[i].bounds, source.bounds[0], source.bounds[1]);
				models[i].writable.bounds = models[i].bounds;

				const auto& half = models[i].bounds.halfSize;
				models[i].radius = std::sqrt(half[0] * half[0] + half[1] * half[1] + half[2] * half[2]);
				models[i].surfaceCount = source.surfaceCount;
				models[i].startSurfIndex = source.startSurfIndex;
				models[i].surfaceCountNoDecal = source.surfaceCountNoDecal;
			}
			new_asset->models = models;

			compute_bounds(new_asset->bounds, asset->mins, asset->maxs);
			new_asset->checksum = 0xC0D40001; // marks a converted map

			new_asset->materialMemoryCount = asset->materialMemoryCount;
			const auto memory = allocator.allocate<zonetool::iw4::MaterialMemory>(asset->materialMemoryCount);
			for (auto i = 0; i < asset->materialMemoryCount; i++)
			{
				memory[i].material.value = asset->materialMemory[i].material.value;
				memory[i].memory = asset->materialMemory[i].memory;
			}
			new_asset->materialMemory = memory;

			copy_layout(new_asset->sun, asset->sun);
			COPY_ARR(outdoorLookupMatrix);

			new_asset->outdoorImage = new_asset->worldDraw.outdoorImage;

			new_asset->cellCasterBits[0].value = asset->cellCasterBits.value;
			new_asset->cellCasterBits[1].value = 1; // must not be null

			new_asset->sceneDynModel.value = asset->sceneDynModel.value;
			new_asset->sceneDynBrush.value = asset->sceneDynBrush.value;

			COPY_POINTER_RAW(primaryLightEntityShadowVis);
			new_asset->primaryLightDynEntShadowVis[0].value = asset->primaryLightDynEntShadowVis[0].value;
			new_asset->primaryLightDynEntShadowVis[1].value = asset->primaryLightDynEntShadowVis[1].value;
			new_asset->primaryLightForModelDynEnt.value = asset->nonSunPrimaryLightForModelDynEnt.value;

			new_asset->shadowGeom.value = asset->shadowGeom.value;
			new_asset->lightRegion.value = asset->lightRegion.value;

			new_asset->fogTypesAllowed = 0x1; // IW3 has no distance fog type

			return new_asset;
		}
	}
}
