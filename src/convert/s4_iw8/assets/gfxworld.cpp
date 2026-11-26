#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"

#include "gfximage.hpp"
#include "mapents.hpp"
#include "material.hpp"
#include "xsurface.hpp"

#include <iw8/assets/gfxworld.hpp>
#include <iw8/assets/gfxworldtransientzone.hpp>

#include <formats/umbra_tome.hpp>

namespace zonetool::s4::converter::iw8::gfxworld
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename Target, typename Source>
		Target* name_reference(const Source* asset, utils::memory::allocator& allocator)
		{
			if (!asset || !asset->name)
			{
				return nullptr;
			}

			const auto reference = allocator.allocate<Target>();
			reference->name = allocator.duplicate_string(asset->name);
			return reference;
		}

		template <typename T>
		T* make_array(const std::size_t count, utils::memory::allocator& allocator)
		{
			return count ? allocator.allocate_array<T>(count) : nullptr;
		}

		template <typename Target, typename Source>
		Target* copy_array(const Source* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(Target) == sizeof(Source));
			if (!source || !count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<Target>(count);
			std::memcpy(result, source, sizeof(Target) * count);
			return result;
		}

		void append(std::vector<unsigned char>& data, const void* value, const std::size_t size)
		{
			const auto* bytes = static_cast<const unsigned char*>(value);
			data.insert(data.end(), bytes, bytes + size);
		}

		bool append_stream(world_geometry& geometry, const GfxWorldDrawVerts& verts, const unsigned int offset, const std::size_t size, unsigned int& converted_offset)
		{
			if (!offset)
			{
				return true;
			}

			if (offset + size > verts.posDataSize)
			{
				return false;
			}

			converted_offset = static_cast<unsigned int>(geometry.aux.size());
			append(geometry.aux, verts.posData + offset, size);
			return true;
		}

		bool append_colors(world_geometry& geometry, const GfxWorldDrawVerts& verts, const unsigned int offset, const unsigned int vertex_count, const int layer_count, unsigned int& converted_offset)
		{
			if (layer_count > 1)
			{
				return append_stream(geometry, verts, offset, 4ull * vertex_count, converted_offset);
			}

			if (!offset)
			{
				return true;
			}

			if (offset + 4ull * vertex_count > verts.posDataSize)
			{
				return false;
			}

			converted_offset = static_cast<unsigned int>(geometry.aux.size());
			const auto* colors = reinterpret_cast<const std::uint32_t*>(verts.posData + offset);
			for (auto i = 0u; i < vertex_count; i++)
			{
				const auto color = xsurface::convert_vertex_color(static_cast<std::uint8_t>(colors[i]));
				append(geometry.aux, &color, sizeof(color));
			}

			return true;
		}

		bool build_surf_data(const GfxWorldUgbSurfData& source, const GfxWorldDrawVerts& verts, const int layer_count, world_geometry& geometry, target::GfxWorldSurfData& data)
		{
			const auto vertex_count = source.vertexCount;
			if (!source.xyzOffset || source.xyzOffset + sizeof(std::uint64_t) * vertex_count > verts.posDataSize || source.layerCount > 8)
			{
				return false;
			}

			data.layerCount = source.layerCount;
			data.xyzOffset = static_cast<unsigned int>(geometry.positions.size());

			const auto* packed_positions = reinterpret_cast<const std::uint64_t*>(verts.posData + source.xyzOffset);
			for (auto i = 0u; i < vertex_count; i++)
			{
				float xyz[3]{};
				for (auto axis = 0; axis < 3; axis++)
				{
					const auto value = static_cast<float>((packed_positions[i] >> (21 * axis)) & 0x1FFFFF);
					xyz[axis] = source.xyzOrigin[axis] + source.xyzScale * value;
				}

				append(geometry.positions, xyz, sizeof(xyz));
			}

			auto valid = append_stream(geometry, verts, source.tangentFrameOffset, 4ull * vertex_count, data.tangentFrameOffset);
			valid &= append_stream(geometry, verts, source.texCoordOffset, 8ull * source.layerCount * vertex_count, data.texCoordOffset);
			for (auto layer = 0u; layer < std::size(data.normalTransformOffset); layer++)
			{
				valid &= append_stream(geometry, verts, source.normalTransformOffset[layer], 4ull * vertex_count, data.normalTransformOffset[layer]);
			}

			valid &= append_stream(geometry, verts, source.lmapCoordOffset, 8ull * vertex_count, data.lmapCoordOffset);
			valid &= append_colors(geometry, verts, source.colorOffset, vertex_count, layer_count, data.colorOffset);
			return valid;
		}

		std::vector<int> get_surf_data_layer_counts(const GfxWorldSurfaces& surfaces)
		{
			std::vector<int> layer_counts(surfaces.ugbCount, 1);
			for (auto i = 0u; i < surfaces.count; i++)
			{
				const auto& surface = surfaces.surfaces[i];
				if (surface.ugbIndex < surfaces.ugbCount && surface.materialIndex < surfaces.materialCount)
				{
					const auto layer_count = material::layer_count(surfaces.materials[surface.materialIndex]);
					layer_counts[surface.ugbIndex] = std::max(layer_counts[surface.ugbIndex], layer_count);
				}
			}

			return layer_counts;
		}

		world_geometry build_geometry(const GfxWorld* asset, const GfxWorldDrawVerts& verts)
		{
			world_geometry geometry;
			geometry.positions.resize(4);
			geometry.aux.resize(4);
			geometry.surf_data.resize(asset->surfaces.ugbCount);
			geometry.valid.resize(asset->surfaces.ugbCount);

			const auto layer_counts = get_surf_data_layer_counts(asset->surfaces);
			for (auto i = 0u; i < asset->surfaces.ugbCount; i++)
			{
				geometry.valid[i] = build_surf_data(asset->surfaces.ugbData[i], verts, layer_counts[i], geometry, geometry.surf_data[i]) ? 1 : 0;
			}

			return geometry;
		}

		void convert_surfaces(const GfxWorld* asset, const GfxWorldDrawVerts& verts, const world_geometry& geometry, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& source = asset->surfaces;
			auto& surfaces = world->surfaces;
			surfaces.count = source.count;
			surfaces.surfDataCount = source.ugbCount;
			surfaces.litOpaqueSurfsBegin = source.litOpaqueSurfsBegin;
			surfaces.litOpaqueSurfsEnd = source.litOpaqueSurfsEnd;
			surfaces.litDecalSurfsBegin = source.litDecalSurfsBegin;
			surfaces.litDecalSurfsEnd = source.litDecalSurfsEnd;
			surfaces.litTransSurfsBegin = source.litTransSurfsBegin;
			surfaces.litTransSurfsEnd = source.litTransSurfsEnd;
			surfaces.emissiveSurfsBegin = source.emissiveSurfsBegin;
			surfaces.emissiveSurfsEnd = source.emissiveSurfsEnd;

			surfaces.surfaces = make_array<target::GfxSurface>(source.count, allocator);
			surfaces.surfaceBounds = make_array<target::GfxSurfaceBounds>(source.count, allocator);
			surfaces.surfaceMaterials = make_array<target::GfxDrawSurf>(source.count, allocator);
			surfaces.surfData = copy_array<target::GfxWorldSurfData>(geometry.surf_data.data(), geometry.surf_data.size(), allocator);

			std::vector<target::Material*> materials(source.materialCount);
			for (auto i = 0u; i < source.materialCount; i++)
			{
				materials[i] = material::reference(source.materials[i], allocator);
			}

			auto dropped = 0u;
			for (auto i = 0u; i < source.count; i++)
			{
				const auto& surface = source.surfaces[i];
				auto& converted = surfaces.surfaces[i];
				converted.surfDataIndex = surface.ugbIndex < source.ugbCount ? surface.ugbIndex : 0;
				converted.lightmapIndex = 510;
				converted.flags = surface.flags;
				surfaces.surfaceBounds[i].bounds = std::bit_cast<target::Bounds>(source.surfaceBounds[i]);

				const auto last_index = surface.baseIndex + 3ull * surface.triCount;
				if (surface.ugbIndex >= source.ugbCount || !geometry.valid[surface.ugbIndex] || surface.materialIndex >= source.materialCount
					|| !materials[surface.materialIndex] || last_index > verts.indexCount)
				{
					dropped++;
					continue;
				}

				converted.material = materials[surface.materialIndex];
				converted.tris.posOffset = geometry.surf_data[surface.ugbIndex].xyzOffset;
				converted.tris.vertexCount = surface.vertexCount;
				converted.tris.triCount = surface.triCount;
				converted.tris.baseIndex = surface.baseIndex;
			}

			if (dropped)
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %u surfaces have no geometry or no material", asset->name, dropped);
			}
		}

		target::GfxStaticModelExpansion convert_expansion(const GfxStaticModelExpansion& source)
		{
			target::GfxStaticModelExpansion converted{};
			converted.firstStaticModelSurfaceIndex = source.firstStaticModelSurfaceIndex;
			converted.numSurfs = source.numSurfs;
			converted.numLods = std::min<unsigned char>(source.numLods, 6);
			converted.modelBounds = std::bit_cast<target::Bounds>(source.modelBounds);
			for (auto lod = 0u; lod < converted.numLods; lod++)
			{
				converted.lodInfo[lod].surfOffset = source.lodInfo[lod].surfOffset;
				converted.lodInfo[lod].surfCount = source.lodInfo[lod].surfCount;
				converted.lodDist[lod] = source.lodDist[lod];
			}

			if (source.numLods > 6)
			{
				converted.numSurfs = source.lodInfo[6].surfOffset;
				converted.lodDist[converted.numLods - 1] = source.lodDist[source.numLods - 1];
			}

			return converted;
		}

		target::GpuSplinedModelInstance convert_splined_instance(const GpuSplinedModelInstance& source)
		{
			target::GpuSplinedModelInstance converted{};
			converted.modelXExtent = source.modelXExtent;
			converted.instanceLength = source.instanceLength;
			std::memcpy(converted.modelSplineOrigin, source.modelSplineOrigin, sizeof(converted.modelSplineOrigin));
			converted.distFromStartNode = source.distFromStartNode;
			converted.modelScale = source.modelScale;
			converted.upDownOffset = source.upDownOffset;
			converted.leftRightOffset = source.leftRightOffset;
			converted.segmentBegin = source.segmentBegin;
			converted.segmentEnd = source.segmentEnd;
			converted.spliningAxisType = source.spliningAxisType;
			return converted;
		}

		target::GpuSplineSegment convert_spline_segment(const GpuSplineSegment& source)
		{
			target::GpuSplineSegment converted{};
			converted.distFromStartNode = source.distFromStartNode;
			converted.length = source.length;
			std::memcpy(converted.bezierEval, source.bezierEval, sizeof(converted.bezierEval));
			std::memcpy(converted.entBezierDerEval, source.entBezierDerEval, sizeof(converted.entBezierDerEval));
			std::memcpy(converted.entToAligned, source.entToAligned, sizeof(converted.entToAligned));
			std::memcpy(converted.modelToWld, source.modelToWld, sizeof(converted.modelToWld));
			std::memcpy(converted.modelToWld_origin, source.modelToWld_origin, sizeof(converted.modelToWld_origin));
			std::memcpy(converted.bankAngleBezierEval, source.bankAngleBezierEval, sizeof(converted.bankAngleBezierEval));
			return converted;
		}

		void make_brush_models(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto* ents = static_cast<const MapEnts*>(db_find_asset(ASSET_TYPE_MAP_ENTS, asset->name));
			const auto submodel_count = ents ? ents->numSubModels : 0;
			world->modelCount = std::max(1, static_cast<int>(submodel_count));
			world->models = make_array<target::GfxBrushModel>(world->modelCount, allocator);

			for (auto i = 1u; i < submodel_count; i++)
			{
				auto& model = world->models[i];
				model.bounds = std::bit_cast<target::Bounds>(ents->cmodels[i].bounds);
				model.radius = ents->cmodels[i].radius;
				model.startSurfIndex = world->surfaces.count;
				model.mdaoVolumeIndex = -1;
			}

			auto& world_model = world->models[0];
			world_model.bounds = world->bounds;
			world_model.radius = std::hypot(world->bounds.halfSize[0], world->bounds.halfSize[1], world->bounds.halfSize[2]);
			world_model.surfaceCount = world->surfaces.count;
			world_model.mdaoVolumeIndex = -1;
		}

		void convert_static_models(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& source = asset->smodels;
			auto& smodels = world->smodels;
			smodels.surfaceCount = source.surfaceCount;
			smodels.modelCount = source.modelCount;
			smodels.collectionCount = source.collectionCount;
			smodels.clutterCollectionCount = source.clutterCollectionCount;
			smodels.instanceCount = source.instanceCount;
			smodels.clutterTintCount = source.clutterTintCount;
			smodels.lmapTransformsCount = source.lmapTransformsCount;
			smodels.materialCount = source.materialCount;

			smodels.surfaces = copy_array<target::GfxStaticModelSurface>(source.surfaces, source.surfaceCount, allocator);
			smodels.clutterCollections = copy_array<target::GfxClutterCollection>(source.clutterCollections, source.clutterCollectionCount, allocator);
			smodels.collections = copy_array<target::GfxStaticModelCollection>(source.collections, source.collectionCount, allocator);
			smodels.instances = copy_array<target::GfxStaticModelInstance>(source.instances, source.instanceCount, allocator);
			smodels.instanceFlags = copy_array<unsigned char>(source.instanceFlags, source.instanceCount, allocator);
			smodels.collectionBounds = copy_array<target::Bounds>(source.collectionBounds, source.collectionCount, allocator);
			smodels.modelStaticIndirection = copy_array<target::GfxModelStaticIndirection>(source.modelStaticIndirection, source.instanceCount, allocator);
			smodels.lmapTransforms = copy_array<target::GfxLightmapTransformPacked>(source.lmapTransforms, source.lmapTransformsCount, allocator);
			smodels.smodelSurfData = copy_array<target::GfxBaseSModelSurfData>(source.smodelSurfData, source.surfaceCount, allocator);
			smodels.smodelInstanceData = copy_array<target::GfxSModelInstanceData>(source.smodelInstanceData, source.instanceCount, allocator);
			smodels.clutterTintData = copy_array<target::vec3_t>(source.clutterTintData, source.clutterTintCount, allocator);
			smodels.clutterCollectionGPUData = copy_array<target::GfxClutterCollectionGPUData>(source.clutterCollectionGPUData, source.clutterCollectionCount, allocator);
			smodels.smodelSurfMatIndirection = copy_array<target::GfxSModelSurfMatIndirection>(source.smodelSurfMatIndirection, source.surfaceCount, allocator);
			smodels.smodelSurfUGBIndirection = make_array<unsigned int>(source.surfaceCount, allocator);
			smodels.collectionVisRefCount = make_array<unsigned char>(source.collectionCount, allocator);

			smodels.models = make_array<target::GfxStaticModel>(source.modelCount, allocator);
			smodels.smodelExpansionData = make_array<target::GfxStaticModelExpansion>(source.modelCount, allocator);
			auto clamped = 0u;
			for (auto i = 0u; i < source.modelCount; i++)
			{
				smodels.models[i].model = name_reference<target::XModel>(source.models[i].model, allocator);
				smodels.models[i].flags = source.models[i].flags;
				smodels.models[i].firstMtlSkinIndex = source.models[i].firstMtlSkinIndex;
				smodels.models[i].firstStaticModelSurfaceIndex = source.models[i].firstStaticModelSurfaceIndex;
				smodels.smodelExpansionData[i] = convert_expansion(source.smodelExpansionData[i]);
				clamped += source.smodelExpansionData[i].numLods > 6 ? 1 : 0;
			}

			smodels.materials = make_array<target::Material*>(source.materialCount, allocator);
			for (auto i = 0u; i < source.materialCount; i++)
			{
				smodels.materials[i] = material::reference(source.materials[i], allocator);
			}

			if (clamped)
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %u static models lose LOD 7, LOD 6 takes its distance", asset->name, clamped);
			}

			smodels.splinedModelInstanceCount = source.splinedModelInstanceCount;
			smodels.splineSegmentCount = source.splineSegmentCount;
			smodels.splinedModelInstanceBounds = copy_array<target::Bounds>(source.splinedModelInstanceBounds, source.splinedModelInstanceCount, allocator);
			smodels.splinedModelInstanceData = make_array<target::GpuSplinedModelInstance>(source.splinedModelInstanceCount, allocator);
			for (auto i = 0u; i < source.splinedModelInstanceCount; i++)
			{
				smodels.splinedModelInstanceData[i] = convert_splined_instance(source.splinedModelInstanceData[i]);
			}

			smodels.splineSegmentData = make_array<target::GpuSplineSegment>(source.splineSegmentCount, allocator);
			for (auto i = 0u; i < source.splineSegmentCount; i++)
			{
				smodels.splineSegmentData[i] = convert_spline_segment(source.splineSegmentData[i]);
			}
		}

		void make_single_cell(target::GfxWorld* world, target::GfxWorldTransientZone* zone, utils::memory::allocator& allocator)
		{
			auto& planes = world->dpvsPlanes;
			planes.cellCount = 1;
			planes.nodeCount = 1;
			planes.nodes = make_array<unsigned short>(1, allocator);
			planes.nodes[0] = 1;
			planes.sceneEntCellBits = make_array<unsigned int>(1ull << 9, allocator);

			world->cells = make_array<target::GfxCell>(1, allocator);
			world->cells[0].bounds = world->bounds;
			world->cellTransientInfos = make_array<target::GfxCellTransientInfo>(1, allocator);

			const auto instance_count = world->smodels.collectionCount;
			auto* tree = allocator.allocate<target::GfxAabbTree>();
			tree->bounds = world->bounds;
			tree->surfaceCount = world->surfaces.count;
			tree->smodelIndexCount = static_cast<unsigned short>(std::min(instance_count, 65535u));
			tree->smodelIndexes = make_array<unsigned short>(tree->smodelIndexCount, allocator);
			for (auto i = 0u; i < tree->smodelIndexCount; i++)
			{
				tree->smodelIndexes[i] = static_cast<unsigned short>(i);
			}

			zone->drawCells.cellCount = 1;
			zone->drawCells.aabbTreeCounts = make_array<target::GfxCellTreeCount>(1, allocator);
			zone->drawCells.aabbTreeCounts[0].aabbTreeCount = 1;
			zone->drawCells.aabbTrees = make_array<target::GfxCellTree>(1, allocator);
			zone->drawCells.aabbTrees[0].aabbTree = tree;
		}

		void convert_dpvs(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto surface_count = world->surfaces.count;
			const auto collection_count = world->smodels.collectionCount;

			auto& dpvs = world->dpvs;
			dpvs.smodelVisDataCount = (collection_count + 31) >> 5;
			dpvs.surfaceVisDataCount = (surface_count + 31) >> 5;
			dpvs.primaryLightVisDataCount = (world->primaryLightCount + 31) >> 5;
			dpvs.reflectionProbeVisDataCount = (world->draw.reflectionProbeData.reflectionProbeInstanceCount + 31) >> 5;

			for (auto i = 0u; i < 32; i++)
			{
				const auto skip = i >= 4 && i < 12;
				dpvs.smodelVisData[i] = make_array<unsigned int>(skip ? 0 : dpvs.smodelVisDataCount, allocator);
				dpvs.surfaceVisData[i] = make_array<unsigned int>(skip ? 0 : dpvs.surfaceVisDataCount, allocator);
			}

			dpvs.primaryLightVisData = make_array<unsigned int>(dpvs.primaryLightVisDataCount, allocator);
			dpvs.reflectionProbeVisData = make_array<unsigned int>(dpvs.reflectionProbeVisDataCount, allocator);

			dpvs.sortedSurfIndex = make_array<unsigned int>(32ull * dpvs.surfaceVisDataCount, allocator);
			for (auto i = 0u; i < surface_count; i++)
			{
				dpvs.sortedSurfIndex[i] = asset->dpvs.sortedSurfIndex ? asset->dpvs.sortedSurfIndex[i] : i;
			}

			dpvs.sortedSmodelIndices = make_array<unsigned short>(32ull * dpvs.smodelVisDataCount + 1, allocator);
			for (auto i = 0u; i < collection_count; i++)
			{
				dpvs.sortedSmodelIndices[i] = asset->dpvs.sortedSmodelIndices ? asset->dpvs.sortedSmodelIndices[i] : static_cast<unsigned short>(i);
			}

			auto sun_shadow_opt_bits = 0u;
			for (auto i = 0u; i < surface_count; i++)
			{
				sun_shadow_opt_bits |= (world->surfaces.surfaces[i].flags & target::GFX_SURFACE_SUN_SHADOW_OPT_MASK) >> 1;
			}

			dpvs.sunShadowOptCount = std::bit_width(sun_shadow_opt_bits);
			dpvs.sunSurfVisDataCount = dpvs.surfaceVisDataCount;
			dpvs.surfaceCastsSunShadow = make_array<unsigned int>(dpvs.surfaceVisDataCount, allocator);
			dpvs.surfaceCastsSunShadowOpt = make_array<unsigned int>(static_cast<std::size_t>(dpvs.sunShadowOptCount) * dpvs.sunSurfVisDataCount, allocator);
		}

		void make_dpvs_dynamic(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto* ents = static_cast<const MapEnts*>(db_find_asset(ASSET_TYPE_MAP_ENTS, asset->name));
			const auto scriptable_dynents = ents ? static_cast<unsigned int>(mapents::make_scriptable_dynents(ents).size()) : 0;
			const auto cell_count = world->dpvsPlanes.cellCount;
			auto& dyn = world->dpvsDyn;
			for (auto i = 0; i < 2; i++)
			{
				const auto count = ents ? ents->dynEntCount[i] + (i == 0 ? scriptable_dynents : 0) : 0;
				dyn.dynEntClientCount[i] = count;
				dyn.dynEntClientWordCount[i] = 2 * ((count + 31) >> 5);
				dyn.dynEntCellBits[i] = make_array<unsigned int>(static_cast<std::size_t>(dyn.dynEntClientWordCount[i]) * cell_count, allocator);

				for (auto slot = 0u; slot < 32; slot++)
				{
					const auto skip = slot == 3 || slot >= 12;
					dyn.dynEntVisData[i][slot] = make_array<unsigned char>(skip ? 0 : 32 * dyn.dynEntClientWordCount[i], allocator);
				}
			}

			world->sceneDynModel = make_array<target::GfxSceneDynModel>(dyn.dynEntClientCount[0], allocator);
			world->sceneDynBrush = make_array<target::GfxSceneDynBrush>(dyn.dynEntClientCount[1], allocator);
		}

		void make_runtime_bits(target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto cell_count = world->dpvsPlanes.cellCount;
			const auto cell_words = (cell_count + 31) >> 5;
			world->cellCasterBits = make_array<unsigned int>(static_cast<std::size_t>(cell_count) * cell_words, allocator);
			world->cellHasSunLitSurfsBits = make_array<unsigned int>(cell_words, allocator);

			const auto static_lights = static_cast<int>(world->primaryLightCount) - static_cast<int>(world->lastSunPrimaryLightIndex)
				- static_cast<int>(world->movingScriptablePrimaryLightCount) - 1;
			const auto aligned = static_cast<unsigned int>(std::max(static_lights, 0) + 31) & ~31u;

			world->staticSpotOmniPrimaryLightCountAligned = aligned;
			world->shadowBitsArrayPitch = aligned;
			world->primaryLightMotionDetectBitsEntries = aligned >> 4;
			world->entityMotionBitsEntries = 160;
			world->numPrimaryLightEntityShadowVisEntries = aligned * world->entityMotionBitsEntries;

			world->primaryLightMotionDetectBits = make_array<unsigned int>(world->primaryLightMotionDetectBitsEntries, allocator);
			world->entityMotionBits = make_array<unsigned int>(world->entityMotionBitsEntries, allocator);
			world->primaryLightEntityShadowVis = make_array<unsigned int>(world->numPrimaryLightEntityShadowVisEntries, allocator);

			for (auto i = 0; i < 2; i++)
			{
				world->dynEntMotionBitsEntries[i] = world->dpvsDyn.dynEntClientWordCount[i];
				world->dynEntMotionBits[i] = make_array<unsigned int>(world->dynEntMotionBitsEntries[i], allocator);
				world->numPrimaryLightDynEntShadowVisEntries[i] = (aligned * world->dpvsDyn.dynEntClientCount[i]) >> 4;
				world->primaryLightDynEntShadowVis[i] = make_array<unsigned int>(world->numPrimaryLightDynEntShadowVisEntries[i], allocator);
			}

			world->primaryLights = make_array<target::GfxLight>(world->primaryLightCount, allocator);
			world->shadowGeomOptimized = make_array<target::GfxShadowGeometry>(world->primaryLightCount, allocator);
		}

		target::Bounds surface_bounds(const target::GfxWorld* world)
		{
			float mins[3]{FLT_MAX, FLT_MAX, FLT_MAX};
			float maxs[3]{-FLT_MAX, -FLT_MAX, -FLT_MAX};
			for (auto i = 0u; i < world->surfaces.count; i++)
			{
				const auto& bounds = world->surfaces.surfaceBounds[i].bounds;
				for (auto axis = 0; axis < 3; axis++)
				{
					mins[axis] = std::min(mins[axis], bounds.midPoint[axis] - bounds.halfSize[axis]);
					maxs[axis] = std::max(maxs[axis], bounds.midPoint[axis] + bounds.halfSize[axis]);
				}
			}

			target::Bounds result{};
			for (auto axis = 0; axis < 3; axis++)
			{
				result.midPoint[axis] = (mins[axis] + maxs[axis]) * 0.5f;
				result.halfSize[axis] = (maxs[axis] - mins[axis]) * 0.5f;
			}

			return result;
		}

		void make_light_aabb(target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto first_light = world->lastSunPrimaryLightIndex + 1;
			const auto end_light = world->primaryLightCount - world->movingScriptablePrimaryLightCount;
			if (!world->surfaces.count || first_light >= end_light)
			{
				return;
			}

			auto& tree = world->lightAABB;
			tree.lightCount = static_cast<unsigned short>(end_light - first_light);
			tree.lightArray = make_array<unsigned short>(tree.lightCount, allocator);
			for (auto i = 0u; i < tree.lightCount; i++)
			{
				tree.lightArray[i] = static_cast<unsigned short>(first_light + i);
			}

			tree.nodeCount = 1;
			tree.nodeArray = make_array<target::GfxLightAABBNode>(1, allocator);
			tree.nodeArray[0].bound = surface_bounds(world);
			tree.nodeArray[0].firstChild = 0x8000; // leaf (0x1959E50)
			tree.nodeArray[0].childCount = tree.lightCount;
		}

		void build_umbra_tome(target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			umbra_tome::object scene{};
			for (auto i = 0; i < 3; i++)
			{
				scene.minimum[i] = world->bounds.midPoint[i] - world->bounds.halfSize[i];
				scene.maximum[i] = world->bounds.midPoint[i] + world->bounds.halfSize[i];
			}

			const auto collection_count = world->smodels.collectionCount;
			const auto probe_count = world->draw.reflectionProbeData.reflectionProbeInstanceCount;
			const auto decal_count = world->draw.decalVolumeCollectionCount;
			std::vector<umbra_tome::object> objects(world->surfaces.count + collection_count + world->primaryLightCount + probe_count + decal_count, scene);
			auto* object = objects.data();
			for (auto i = 0u; i < world->surfaces.count; i++)
			{
				(object++)->user_id = i;
			}

			for (auto i = 0u; i < collection_count; i++)
			{
				(object++)->user_id = 0x10000000u + i;
			}

			for (auto i = 0u; i < world->primaryLightCount; i++)
			{
				(object++)->user_id = 0x40000000u + i;
			}

			for (auto i = 0u; i < probe_count; i++)
			{
				(object++)->user_id = 0x50000000u + i;
			}

			for (auto i = 0u; i < decal_count; i++)
			{
				(object++)->user_id = 0x60000000u + i;
			}

			const auto tome = umbra_tome::build_single_cell(scene.minimum, scene.maximum, objects);
			if (tome.empty())
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %zu umbra objects cannot be represented", world->name, objects.size());
				return;
			}

			world->umbraTomeSize = static_cast<unsigned int>(tome.size());
			world->umbraTomeData = make_array<char>(world->umbraTomeSize, allocator);
			std::memcpy(world->umbraTomeData, tome.data(), tome.size());
		}

		unsigned int convert_umbra_user_id(const GfxWorld* asset, const target::GfxWorld* world, const unsigned int user_id)
		{
			const auto index = user_id & UMBRA_USER_ID_INDEX_MASK;
			switch (user_id & UMBRA_USER_ID_TYPE_MASK)
			{
			case UMBRA_USER_ID_SURFACE:
				if (index < world->surfaces.count)
				{
					return target::UMBRA_USER_ID_SURFACE | index;
				}
				break;
			case UMBRA_USER_ID_SMODEL_COLLECTION:
				if (index < world->smodels.collectionCount)
				{
					return target::UMBRA_USER_ID_SMODEL_COLLECTION | index;
				}
				break;
			case UMBRA_USER_ID_PRIMARY_LIGHT:
				if (index < world->primaryLightCount)
				{
					return target::UMBRA_USER_ID_PRIMARY_LIGHT | index;
				}
				break;
			case UMBRA_USER_ID_REFLECTION_PROBE:
				if (index < world->draw.reflectionProbeData.reflectionProbeInstanceCount)
				{
					return target::UMBRA_USER_ID_REFLECTION_PROBE | index;
				}
				break;
			case UMBRA_USER_ID_DECAL_VOLUME:
				if (index < world->draw.decalVolumeCollectionCount)
				{
					return target::UMBRA_USER_ID_DECAL_VOLUME | index;
				}
				break;
			case UMBRA_USER_ID_SURFACE_LOD_GROUP:
				if (index < asset->surfaces.lodGroupCount && asset->surfaces.lodGroups[index].highSurface < world->surfaces.count)
				{
					return target::UMBRA_USER_ID_SURFACE | asset->surfaces.lodGroups[index].highSurface;
				}
				break;
			}

			return target::UMBRA_USER_ID_UNUSED;
		}

		std::optional<unsigned int> umbra_user_id_count(const umbra_tome::UmbraTomeHeader* header)
		{
			auto count = header->objectCount;
			if (header->userIDStarts)
			{
				if (header->userIDStarts > header->size || count >= (header->size - header->userIDStarts) / sizeof(unsigned int))
				{
					return std::nullopt;
				}

				const auto* starts = reinterpret_cast<const unsigned int*>(reinterpret_cast<const char*>(header) + header->userIDStarts);
				count = starts[header->objectCount];
			}

			if (header->userIDs > header->size || count > (header->size - header->userIDs) / sizeof(unsigned int))
			{
				return std::nullopt;
			}

			return count;
		}

		bool convert_umbra_tome(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			if (!asset->umbraTomeData || asset->umbraTomeSize < sizeof(umbra_tome::UmbraTomeHeader))
			{
				return false;
			}

			const auto* source_header = reinterpret_cast<const umbra_tome::UmbraTomeHeader*>(asset->umbraTomeData);
			const auto user_id_count = source_header->size == asset->umbraTomeSize ? umbra_user_id_count(source_header) : std::nullopt;
			if (!user_id_count)
			{
				ZONETOOL_WARNING("gfxworld \"%s\": umbra tome header does not match its size %u", asset->name, asset->umbraTomeSize);
				return false;
			}

			world->umbraTomeSize = asset->umbraTomeSize;
			world->umbraTomeData = copy_array<char>(asset->umbraTomeData, asset->umbraTomeSize, allocator);

			const auto* header = reinterpret_cast<const umbra_tome::UmbraTomeHeader*>(world->umbraTomeData);
			auto* user_ids = reinterpret_cast<unsigned int*>(world->umbraTomeData + header->userIDs);
			auto lod_groups = 0u;
			auto unused = 0u;
			for (auto i = 0u; i < *user_id_count; i++)
			{
				if ((user_ids[i] & UMBRA_USER_ID_TYPE_MASK) == UMBRA_USER_ID_SURFACE_LOD_GROUP)
				{
					lod_groups++;
				}

				user_ids[i] = convert_umbra_user_id(asset, world, user_ids[i]);
				if (user_ids[i] == target::UMBRA_USER_ID_UNUSED)
				{
					unused++;
				}
			}

			if (lod_groups)
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %u umbra surface lod groups draw their high surface only", asset->name, lod_groups);
			}

			if (unused)
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %u umbra objects have no target", asset->name, unused);
			}

			return true;
		}

		void convert_light_frustums(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& source = asset->frustumLights;
			auto& lights = world->frustumLights;
			lights.lightCount = source.lightCount;
			lights.totalIndicesCount = source.totalIndicesCount;
			lights.totalVerticesCount = source.totalVerticesCount;
			lights.indicesOffsets = copy_array<unsigned int>(source.indicesOffsets, source.lightCount, allocator);
			lights.verticesOffsets = copy_array<unsigned int>(source.verticesOffsets, source.lightCount, allocator);
			lights.indicesCounts = copy_array<unsigned short>(source.indicesCounts, source.lightCount, allocator);
			lights.verticesCounts = copy_array<unsigned short>(source.verticesCounts, source.lightCount, allocator);
			lights.indices = copy_array<unsigned short>(source.indices, source.totalIndicesCount, allocator);
			lights.vertices = copy_array<unsigned char>(source.vertices, 16ull * source.totalVerticesCount, allocator);

			world->lightViewFrustums = make_array<target::GfxLightViewFrustum>(world->primaryLightCount, allocator);
			for (auto i = 0u; asset->lightViewFrustums && i < world->primaryLightCount; i++)
			{
				const auto& frustum = asset->lightViewFrustums[i];
				auto& converted = world->lightViewFrustums[i];
				converted.planeCount = frustum.planeCount;
				converted.indexCount = frustum.indexCount;
				converted.vertexCount = frustum.vertexCount;
				converted.planes = copy_array<target::vec4_t>(frustum.planes, frustum.planeCount, allocator);
				converted.indices = copy_array<unsigned char>(frustum.indices, frustum.indexCount, allocator);
				converted.vertices = copy_array<target::vec3_t>(frustum.vertices, frustum.vertexCount, allocator);
			}

			if (source.lightCount != world->primaryLightCount)
			{
				ZONETOOL_WARNING("gfxworld \"%s\" has %u frustum lights for %u primary lights", asset->name, source.lightCount, world->primaryLightCount);
			}
		}

		void convert_heightfields(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->heightfieldCount = asset->heightfields ? asset->heightfieldCount : 0;
			world->heightfields = copy_array<target::GfxHeightfield>(asset->heightfields, world->heightfieldCount, allocator);

			for (auto i = 0; i < world->heightfieldCount; i++)
			{
				world->heightfields[i].image = gfximage::image_reference(asset->heightfields[i].image, allocator);
			}
		}

		bool convert_raw_image(const GfxImageRaw& raw, target::GfxImageRaw& converted, utils::memory::allocator& allocator)
		{
			if (!raw.pixels || !raw.desc.totalSize)
			{
				return true;
			}

			const auto format = gfximage::convert_format(raw.desc.format);
			if (!format)
			{
				return false;
			}

			converted.desc.format = *format;
			converted.desc.flags = static_cast<target::GfxImageFlags>(raw.desc.flags);
			converted.desc.totalSize = raw.desc.totalSize;
			converted.desc.width = raw.desc.width;
			converted.desc.height = raw.desc.height;
			converted.desc.depth = raw.desc.depth;
			converted.desc.numElements = raw.desc.numElements;
			converted.desc.levelCount = raw.desc.levelCount;
			converted.pixels = copy_array<char>(raw.pixels, raw.desc.totalSize, allocator);
			return true;
		}

		void make_full_ambient_image(const target::GfxImageRaw& irradiance, target::GfxImageRaw& ambient, utils::memory::allocator& allocator)
		{
			const auto block_count = ((irradiance.desc.width + 3u) / 4u) * ((irradiance.desc.height + 3u) / 4u);

			ambient = {};
			ambient.desc.format = target::GFX_PF_BC4;
			ambient.desc.flags = static_cast<target::GfxImageFlags>(target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_NOMIPMAPS);
			ambient.desc.totalSize = block_count * 8u;
			ambient.desc.width = irradiance.desc.width;
			ambient.desc.height = irradiance.desc.height;
			ambient.desc.depth = 1;
			ambient.desc.numElements = 1;
			ambient.desc.levelCount = 1;
			ambient.pixels = allocator.allocate_array<char>(ambient.desc.totalSize);

			for (auto block = 0u; block < block_count; block++)
			{
				ambient.pixels[block * 8u] = static_cast<char>(0xFF);
				ambient.pixels[block * 8u + 1u] = static_cast<char>(0xFF);
			}
		}

		bool convert_lightmap_atlas(const GfxLightmapAtlasDataTemp& source, target::GfxLightmapAtlasDataTemp& atlas, utils::memory::allocator& allocator)
		{
			atlas.transientZone = source.transientZone;
			atlas.numLightmaps = source.numLightmaps;
			atlas.firstLightmapIndex = source.firstLightmapIndex;
			atlas.lightmapType = static_cast<target::GfxLightmapType>(source.lightmapType);

			for (auto i = 0u; i < std::size(source.lightmaps); i++)
			{
				for (auto texture = 0u; texture < std::size(source.lightmaps[i].textures); texture++)
				{
					if (!convert_raw_image(source.lightmaps[i].textures[texture], atlas.lightmaps[i].textures[texture], allocator))
					{
						return false;
					}
				}

				auto& textures = atlas.lightmaps[i].textures;
				if (source.lightmapType == GFX_LIGHTMAP_TYPE_AB_LIGHTING && textures[1].pixels)
				{
					make_full_ambient_image(textures[1], textures[0], allocator); // S4 has no texture 0 (sub_2A944A0)
				}
			}

			return true;
		}

		void convert_lightmaps(const GfxWorld* asset, const GfxWorldTransientZone* source_zone, target::GfxWorld* world, target::GfxWorldTransientZone* zone,
			utils::memory::allocator& allocator)
		{
			const auto* source = source_zone->tempLightmapData;
			if (!source || !asset->lightmapCount)
			{
				return;
			}

			const auto atlas = allocator.allocate<target::GfxLightmapAtlasDataTemp>();
			if (asset->lightmapCount != 1 || !convert_lightmap_atlas(*source, *atlas, allocator))
			{
				ZONETOOL_WARNING("gfxworld \"%s\" loses its %d lightmaps", asset->name, asset->lightmapCount);
				return;
			}

			zone->tempLightmapData = atlas;

			auto& draw = world->draw;
			draw.lightmapType = atlas->lightmapType;
			draw.lightmapCount = asset->lightmapCount;
			draw.lightmapTransientIndex = copy_array<unsigned int>(asset->lightmapTransientIndex, asset->lightmapCount, allocator);

			const auto& reindex = asset->lightmapReindexData;
			draw.lightmapReindexData.imagePixelSize = reindex.imagePixelSize;
			draw.lightmapReindexData.reindexCount = reindex.reindexCount;
			draw.lightmapReindexData.reindexElement = copy_array<target::GfxWorldLightmapReindexDataElement>(reindex.reindexElement, reindex.reindexCount, allocator);
			draw.lightmapReindexData.packedLightmapCount = reindex.packedLightmapCount;
			draw.lightmapReindexData.packedLightmap = copy_array<target::GfxWorldPackedLightmap>(reindex.packedLightmap, reindex.packedLightmapCount, allocator);

			auto& surfaces = world->surfaces;
			for (auto i = 0u; i < surfaces.count; i++)
			{
				auto& surface = surfaces.surfaces[i];
				if (surface.material && surfaces.surfData[surface.surfDataIndex].lmapCoordOffset)
				{
					surface.lightmapIndex = 0;
				}
			}
		}

		int count_voxel_leaves(const GfxVoxelInternalNode* nodes, const int node_count)
		{
			auto count = 1;
			for (auto i = 0; i < node_count; i++)
			{
				for (auto half = 0; half < 2; half++)
				{
					if (nodes[i].firstNodeIndex[half] < 0)
					{
						count += std::popcount(static_cast<unsigned int>(nodes[i].childNodeMask[half]));
					}
				}
			}

			return count;
		}

		void add_voxel_leaves(const GfxVoxelTree& tree, const int node, const int level, const int origin[3], std::vector<voxel_leaf>& leaves)
		{
			if (node < 0 || node >= tree.voxelInternalNodeCount || level > 3)
			{
				return;
			}

			const auto& header = *tree.voxelTreeHeader;
			const auto size = 1 << header.nodeCoordBitShift[level];
			for (auto half = 0; half < 2; half++)
			{
				const auto first = tree.voxelInternalNodeArray[node].firstNodeIndex[half];
				const auto mask = static_cast<unsigned int>(tree.voxelInternalNodeArray[node].childNodeMask[half]);
				auto child = 0;

				for (auto bit = 0; bit < 32; bit++)
				{
					if (!(mask >> bit & 1))
					{
						continue;
					}

					const int cell[3]{origin[0] + (bit & 3) * size, origin[1] + ((bit >> 2) & 3) * size, origin[2] + ((bit >> 4) + 2 * half) * size};
					if (first < 0)
					{
						voxel_leaf leaf{};
						leaf.index = (first & 0x7FFFFFFF) + child;
						leaf.size = static_cast<float>(size);
						for (auto axis = 0; axis < 3; axis++)
						{
							leaf.mins[axis] = header.boundMin[axis] + static_cast<float>(cell[axis]);
						}

						leaves.push_back(leaf);
					}
					else
					{
						add_voxel_leaves(tree, first + child, level + 1, cell, leaves);
					}

					child++;
				}
			}
		}

		std::vector<voxel_leaf> collect_voxel_leaves(const GfxVoxelTree& tree)
		{
			std::vector<voxel_leaf> leaves;
			const auto& header = *tree.voxelTreeHeader;
			const auto size = 1 << header.nodeCoordBitShift[0];

			for (auto y = 0; y < header.rootNodeDimension[1]; y++)
			{
				for (auto x = 0; x < header.rootNodeDimension[0]; x++)
				{
					const auto index = x + y * header.rootNodeDimension[0];
					if (index >= tree.voxelTopDownViewNodeCount)
					{
						continue;
					}

					const auto& column = tree.voxelTopDownViewNodeArray[index];
					for (auto z = column.zMin; column.firstNodeIndex >= 0 && z <= column.zMax; z++)
					{
						const int origin[3]{x * size, y * size, z * size};
						add_voxel_leaves(tree, column.firstNodeIndex + z - column.zMin, 1, origin, leaves);
					}
				}
			}

			return leaves;
		}

		bool light_reaches_leaf(const ComPrimaryLight& light, const voxel_leaf& leaf)
		{
			auto distance = 0.0f;
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto outside = std::max({leaf.mins[axis] - light.origin[axis], light.origin[axis] - (leaf.mins[axis] + leaf.size), 0.0f});
				distance += outside * outside;
			}

			return distance <= light.radius * light.radius;
		}

		std::vector<unsigned short> make_light_list(const GfxWorld* asset, const ComWorld* com_world, const voxel_leaf& leaf)
		{
			std::vector<unsigned short> list{0};
			const auto end_light = std::min(asset->firstMovingScriptablePrimaryLight, com_world->primaryLightCount);

			for (const auto type : {GFX_LIGHT_TYPE_SPOT, GFX_LIGHT_TYPE_OMNI})
			{
				auto count = 0;
				for (auto i = asset->lastSunPrimaryLightIndex + 1; i < end_light && count < 127; i++)
				{
					const auto& light = com_world->primaryLights[i];
					if (light.type == type && light_reaches_leaf(light, leaf))
					{
						list.push_back(static_cast<unsigned short>(i));
						count++;
					}
				}

				list[0] = static_cast<unsigned short>(type == GFX_LIGHT_TYPE_SPOT ? count << 7 : list[0] | count);
			}

			return list;
		}

		void make_light_lists(const GfxWorld* asset, const GfxVoxelTree& tree, target::GfxVoxelTree& converted, utils::memory::allocator& allocator)
		{
			std::vector<unsigned short> lists{0x4000, 1, 0}; // stock list of leaf 0, empty list
			for (auto leaf = 1; leaf < converted.voxelLeafNodeCount; leaf++)
			{
				converted.voxelLeafNodeArray[leaf].lightListAddress = 2;
			}

			const auto* com_world = static_cast<const ComWorld*>(db_find_asset(ASSET_TYPE_COMWORLD, asset->name));
			if (!com_world || !com_world->primaryLights)
			{
				ZONETOOL_WARNING("gfxworld \"%s\" has no light lists in its voxel tree", asset->name);
			}
			else
			{
				std::map<std::vector<unsigned short>, unsigned short> addresses;
				for (const auto& leaf : collect_voxel_leaves(tree))
				{
					const auto list = make_light_list(asset, com_world, leaf);
					if (leaf.index >= converted.voxelLeafNodeCount || !list[0])
					{
						continue;
					}

					auto address = addresses.find(list);
					if (address == addresses.end())
					{
						if (lists.size() + list.size() > 0xFFFF)
						{
							ZONETOOL_WARNING("gfxworld \"%s\" has too many light lists in its voxel tree", asset->name);
							break;
						}

						address = addresses.emplace(list, static_cast<unsigned short>(lists.size())).first;
						lists.insert(lists.end(), list.begin(), list.end());
					}

					converted.voxelLeafNodeArray[leaf.index].lightListAddress = address->second;
				}
			}

			converted.lightListArraySize = static_cast<int>(lists.size());
			converted.lightListArray = copy_array<unsigned short>(lists.data(), lists.size(), allocator);
		}

		void convert_voxel_trees(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->voxelTreeCount = asset->voxelTree ? asset->voxelTreeCount : 0;
			world->voxelTree = make_array<target::GfxVoxelTree>(world->voxelTreeCount, allocator);

			for (auto i = 0; i < world->voxelTreeCount; i++)
			{
				const auto& tree = asset->voxelTree[i];
				auto& converted = world->voxelTree[i];

				converted.zoneBound = std::bit_cast<target::Bounds>(tree.zoneBound);
				converted.voxelTopDownViewNodeCount = tree.voxelTopDownViewNodeCount;
				converted.voxelInternalNodeCount = tree.voxelInternalNodeCount;
				converted.voxelTreeHeader = copy_array<target::GfxVoxelTreeHeader>(tree.voxelTreeHeader, 1, allocator);
				converted.voxelTopDownViewNodeArray = copy_array<target::GfxVoxelTopDownViewNode>(tree.voxelTopDownViewNodeArray, tree.voxelTopDownViewNodeCount, allocator);
				converted.voxelInternalNodeArray = copy_array<target::GfxVoxelInternalNode>(tree.voxelInternalNodeArray, tree.voxelInternalNodeCount, allocator);
				converted.voxelInternalNodeDynamicLightListCPU = make_array<unsigned int>(2ull * tree.voxelInternalNodeCount, allocator);

				converted.voxelLeafNodeCount = count_voxel_leaves(tree.voxelInternalNodeArray, tree.voxelInternalNodeCount);
				converted.voxelLeafNodeArray = make_array<target::GfxVoxelLeafNode>(converted.voxelLeafNodeCount, allocator);
				if (tree.voxelTreeHeader)
				{
					make_light_lists(asset, tree, converted, allocator);
				}

				ZONETOOL_INFO("gfxworld \"%s\": voxel tree %d has %d leaves and %d light list values", asset->name, i, converted.voxelLeafNodeCount,
					converted.lightListArraySize);
			}
		}

		void convert_grid_voxel_tree(const GfxGpuLightGridVoxelTree& tree, target::GfxGpuLightGridVoxelTree& converted, utils::memory::allocator& allocator)
		{
			converted.topDownViewNodeCount = tree.topDownViewNodeCount;
			converted.internalNodeCount = tree.internalNodeCount;
			converted.treeHeader = copy_array<target::GfxVoxelTreeHeader>(tree.treeHeader, 1, allocator);
			converted.topDownViewNodes = copy_array<target::GfxVoxelTopDownViewNode>(tree.topDownViewNodes, tree.topDownViewNodeCount, allocator);
			converted.internalNodes = copy_array<target::GfxVoxelInternalNode>(tree.internalNodes, tree.internalNodeCount, allocator);
		}

		target::GfxGpuLightGrid* convert_gpu_light_grid(const GfxGpuLightGrid& grid, utils::memory::allocator& allocator)
		{
			const auto converted = allocator.allocate<target::GfxGpuLightGrid>();

			converted->probeCount = grid.probeCount;
			converted->probes = copy_array<target::GfxSHCompressedProbeData>(grid.probes, grid.probeCount, allocator);
			converted->probePositions = copy_array<target::vec3_t>(grid.probePositions, grid.probeCount, allocator);
			converted->fallbackProbeUseCustomSample = grid.fallbackProbeUseCustomSample;
			std::memcpy(converted->fallbackProbeCustomSamplePos, grid.fallbackProbeCustomSamplePos, sizeof(grid.fallbackProbeCustomSamplePos));

			converted->zoneCount = grid.zoneCount;
			converted->zones = copy_array<target::GfxGpuLightGridZone>(grid.zones, grid.zoneCount, allocator);
			if (grid.zoneVoxelTrees)
			{
				converted->zoneVoxelTrees = make_array<target::GfxGpuLightGridVoxelTree>(grid.zoneCount, allocator);
				for (auto i = 0u; i < grid.zoneCount; i++)
				{
					convert_grid_voxel_tree(grid.zoneVoxelTrees[i], converted->zoneVoxelTrees[i], allocator);
				}
			}

			converted->tetrahedronCount = grid.tetrahedronCount;
			converted->tetrahedronCountVisible = grid.tetrahedronCountVisible;
			converted->tetrahedrons = copy_array<target::GfxGpuLightGridTetrahedron>(grid.tetrahedrons, grid.tetrahedronCount, allocator);
			converted->tetrahedronNeighbors = copy_array<target::GfxGpuLightGridTetrahedronNeighbors>(grid.tetrahedronNeighbors, grid.tetrahedronCount, allocator);
			converted->tetrahedronVisibility =
				copy_array<target::GfxGpuLightGridTetrahedronVisibility>(grid.tetrahedronVisibility, grid.tetrahedronCountVisible, allocator);
			converted->voxelStartTetrahedronCount = grid.voxelStartTetrahedronCount;
			converted->voxelStartTetrahedron =
				copy_array<target::GfxGpuLightGridVoxelStartTetrahedron>(grid.voxelStartTetrahedron, grid.voxelStartTetrahedronCount, allocator);
			std::memcpy(converted->boundingVolume, grid.boundingVolume, sizeof(grid.boundingVolume));

			return converted;
		}

		target::GfxLightGridVolumeAtlasDataTemp* convert_light_grid_volumes(const GfxLightGridVolumeAtlasDataTemp& source, utils::memory::allocator& allocator)
		{
			const auto atlas = allocator.allocate<target::GfxLightGridVolumeAtlasDataTemp>();
			atlas->m_widthAndHeight = source.m_widthAndHeight;
			atlas->m_depth = source.m_depth;
			atlas->m_numVolumes = source.m_numVolumes;
			atlas->m_worldTransient = source.m_worldTransient;

			for (auto i = 0u; i < std::size(source.m_textures); i++)
			{
				if (!convert_raw_image(source.m_textures[i], atlas->m_textures[i], allocator))
				{
					return nullptr;
				}
			}

			atlas->m_packingParams = copy_array<target::LightGridVolumePackingParams>(source.m_packingParams, source.m_numVolumes, allocator);
			atlas->m_transformParams = copy_array<target::LightGridVolumeTransformParams>(source.m_transformParams, source.m_numVolumes, allocator);
			return atlas;
		}

		void convert_light_grid(const GfxWorld* asset, const GfxWorldTransientZone* source_zone, target::GfxWorld* world, target::GfxWorldTransientZone* zone,
			utils::memory::allocator& allocator)
		{
			const auto& source = source_zone->gpuLightGrid;
			auto& data = zone->gpuLightGrid;

			if (source.gpuLightGrid)
			{
				data.gpuLightGrid = convert_gpu_light_grid(*source.gpuLightGrid, allocator);
				world->draw.lightGridType = target::GFX_LIGHT_GRID_TYPE_GPU;

				const auto& grid = *source.gpuLightGrid;
				for (auto i = 0u; i < grid.zoneCount && !grid.zoneVoxelTrees; i++)
				{
					const auto leaves = i < static_cast<unsigned int>(world->voxelTreeCount) ? world->voxelTree[i].voxelLeafNodeCount : 0;
					if (grid.zones[i].voxelTetrahedronInternalNodeShift != static_cast<unsigned int>(leaves))
					{
						ZONETOOL_WARNING("gfxworld \"%s\": light grid zone %u has %u voxel leaves, the voxel tree has %d", asset->name, i,
							grid.zones[i].voxelTetrahedronInternalNodeShift, leaves);
					}
				}
			}

			data.staticModelVolumeCount = source.staticModelVolumes ? source.staticModelVolumeCount : 0;
			data.staticModelVolumes = copy_array<target::GfxStaticModelLightGridVolume>(source.staticModelVolumes, data.staticModelVolumeCount, allocator);

			if (source.SMLGppZone)
			{
				ZONETOOL_WARNING("gfxworld \"%s\" loses the dynamic lightset probes of its static models", asset->name);
			}

			if (source.tempLightGridVolumeData)
			{
				data.tempLightGridVolumeData = convert_light_grid_volumes(*source.tempLightGridVolumeData, allocator);
				if (!data.tempLightGridVolumeData)
				{
					ZONETOOL_WARNING("gfxworld \"%s\" loses its light grid volume textures", asset->name);
				}
			}
		}

		void convert_decal_volumes(const GfxWorld* asset, const GfxWorldTransientZone* source_zone, target::GfxWorld* world, target::GfxWorldTransientZone* zone,
			utils::memory::allocator& allocator)
		{
			const auto& source = asset->decalVolumes;
			const auto* decals = source_zone->decalVolumes;
			if (!decals || !decals->decalVolumes || !source.decalVolumeCollections || decals->decalVolumeCount != source.mapDecalVolumeCount)
			{
				return;
			}

			auto& draw = world->draw;
			draw.mapDecalVolumeCount = source.mapDecalVolumeCount;
			draw.decalVolumeCollectionCount = source.decalVolumeCollectionCount;
			draw.decalVolumeCollections = copy_array<target::GfxDecalVolumeCollection>(source.decalVolumeCollections, source.decalVolumeCollectionCount, allocator);

			draw.decalVolumeMaterialCount = source.decalVolumeMaterials ? source.decalVolumeMaterialCount : 0;
			draw.decalVolumeMaterials = make_array<target::GfxStaticDecalVolumeMaterial>(draw.decalVolumeMaterialCount, allocator);
			for (auto i = 0u; i < draw.decalVolumeMaterialCount; i++)
			{
				draw.decalVolumeMaterials[i].material = material::reference(source.decalVolumeMaterials[i], allocator);
			}

			draw.decalVolumeMaskCount = source.decalVolumeMasks ? source.decalVolumeMaskCount : 0;
			draw.decalVolumeMasks = make_array<target::GfxStaticDecalVolumeMask>(draw.decalVolumeMaskCount, allocator);
			for (auto i = 0u; i < draw.decalVolumeMaskCount; i++)
			{
				draw.decalVolumeMasks[i].blendMapOverride = name_reference<target::GfxDecalVolumeMask>(source.decalVolumeMasks[i], allocator);
			}

			draw.materialAndMaskIndirection = make_array<unsigned int>(draw.decalVolumeMaterialCount + draw.decalVolumeMaskCount, allocator);

			world->dpvs.decalVisDataCount = (draw.decalVolumeCollectionCount + 31) >> 5;
			world->dpvs.decalVisData = make_array<unsigned int>(world->dpvs.decalVisDataCount, allocator);

			const auto converted = allocator.allocate<target::GfxDecalVolumeTransientData>();
			converted->decalVolumeCount = decals->decalVolumeCount;
			converted->firstCollection = decals->firstCollection;
			converted->collectionCount = decals->collectionCount;
			converted->decalVolumes = copy_array<target::GfxStaticDecalVolumePacked>(decals->decalVolumes, decals->decalVolumeCount, allocator);
			converted->collectionBounds = copy_array<target::Bounds>(decals->collectionBounds, decals->collectionCount, allocator);
			converted->decalVolumeUids = copy_array<target::GfxStaticDecalVolumeUid>(decals->decalVolumeUids, decals->decalVolumeCount, allocator);
			zone->decalVolumes = converted;
		}

		void convert_reflection_probes(const GfxWorld* asset, const GfxWorldTransientZone* source_zone, target::GfxWorld* world, target::GfxWorldTransientZone* zone,
			utils::memory::allocator& allocator)
		{
			const auto& source = asset->reflectionProbeData;
			auto& data = world->draw.reflectionProbeData;
			if (source.reflectionProbeImages || source.reflectionProbeImages_octahedron || source.probeRelightingCount || source.reflectionProbeGBufferImageCount)
			{
				ZONETOOL_WARNING("gfxworld \"%s\" loses reflection probe images and relighting data", asset->name);
			}

			data.reflectionProbeCount = source.reflectionProbeCount;
			data.sharedReflectionProbeCount = source.sharedReflectionProbeCount;
			data.reflectionProbes = copy_array<target::GfxReflectionProbe>(source.reflectionProbes, source.reflectionProbeCount, allocator);
			for (auto i = 0u; i < source.reflectionProbeCount && data.reflectionProbes; i++)
			{
				const auto& probe = source.reflectionProbes[i];
				data.reflectionProbes[i].livePath = probe.livePath ? allocator.duplicate_string(probe.livePath) : nullptr;
				data.reflectionProbes[i].probeInstances = copy_array<unsigned short>(probe.probeInstances, probe.probeInstanceCount, allocator);
			}

			data.reflectionProbeArrayImage = gfximage::image_reference(source.reflectionProbeArrayImage, allocator);
			data.reflectionProbeArrayImage_octahedron = gfximage::image_reference(source.reflectionProbeArrayImage_octahedron, allocator);

			data.reflectionProbeInstanceCount = source.reflectionProbeInstanceCount;
			data.reflectionProbeInstances = copy_array<target::GfxReflectionProbeInstance>(source.reflectionProbeInstances, source.reflectionProbeInstanceCount, allocator);
			for (auto i = 0u; i < source.reflectionProbeInstanceCount && data.reflectionProbeInstances; i++)
			{
				const auto path = source.reflectionProbeInstances[i].livePath;
				data.reflectionProbeInstances[i].livePath = path ? allocator.duplicate_string(path) : nullptr;
			}

			data.reflectionProbeSH = copy_array<target::GfxSH3Vec>(source.reflectionProbeSH, 4ull * source.reflectionProbeCount, allocator);

			const auto* probes = source_zone->reflectionProbes;
			if (!probes)
			{
				return;
			}

			auto* converted = allocator.allocate<target::GfxReflectionProbeTransientData>();
			converted->reflectionProbeInstanceCount = probes->reflectionProbeInstanceCount;
			converted->exclusiveProbeBucketCount = probes->exclusiveProbeBucketCount;
			converted->exclusiveProbeCount = probes->exclusiveProbeCount;
			converted->reflectionProbeInstances = copy_array<unsigned short>(probes->reflectionProbeInstances, probes->reflectionProbeInstanceCount, allocator);
			converted->exclusiveProbeBuckets = make_array<target::GfxImageReflectionProbe>(probes->exclusiveProbeBucketCount, allocator);
			for (auto i = 0; i < probes->exclusiveProbeBucketCount && probes->exclusiveProbeBuckets; i++)
			{
				const auto& bucket = probes->exclusiveProbeBuckets[i];
				converted->exclusiveProbeBuckets[i].totalSize = bucket.totalSize;
				converted->exclusiveProbeBuckets[i].elementCount = bucket.elementCount;
				converted->exclusiveProbeBuckets[i].pixels = copy_array<char>(bucket.pixels, bucket.totalSize, allocator);
			}

			zone->reflectionProbes = converted;
			ZONETOOL_INFO("gfxworld \"%s\": %u reflection probes, %u instances, %u zone instances, %u exclusive buckets with %u probes", asset->name,
				data.reflectionProbeCount, data.reflectionProbeInstanceCount, converted->reflectionProbeInstanceCount, converted->exclusiveProbeBucketCount,
				converted->exclusiveProbeCount);
		}

		void convert_sun(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(target::sunflare_t) == sizeof(sunflare_t));
			std::memcpy(&world->sun, &asset->sun, sizeof(world->sun));
			world->sun.spriteMaterial = material::reference(asset->sun.spriteMaterial, allocator);
			world->sun.flareMaterial = material::reference(asset->sun.flareMaterial, allocator);
			std::memcpy(world->outdoorLookupMatrix, asset->outdoorLookupMatrix, sizeof(world->outdoorLookupMatrix));
			world->outdoorImage = gfximage::image_reference(asset->outdoorImage, allocator);
			world->dustMaterial = material::reference(asset->dustMaterial, allocator);
		}
	}

	void dump(GfxWorld* asset)
	{
		const auto* source_zone = asset->transientZones[0];
		if (!source_zone || !source_zone->drawVerts.posData)
		{
			ZONETOOL_ERROR("gfxworld \"%s\" has no loaded transient zone", asset->name);
			return;
		}

		utils::memory::allocator allocator;
		const auto geometry = build_geometry(asset, source_zone->drawVerts);

		auto* world = allocator.allocate<target::GfxWorld>();
		world->name = bsp_name(asset->name, allocator);
		world->baseName = asset->baseName ? asset->baseName : asset->name;
		world->bspVersion = target::BSP_VERSION;
		world->lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
		world->primaryLightCount = asset->primaryLightCount;
		world->firstMutablePrimaryLight = asset->firstMutablePrimaryLight;
		world->mutablePrimaryLightCount = asset->mutablePrimaryLightCount;
		world->firstStaticScriptablesPrimaryLight = asset->firstStaticScriptablesPrimaryLight;
		world->staticScriptablesPrimaryLightCount = asset->staticScriptablesPrimaryLightCount;
		world->firstStaticScriptablePrimaryLight = asset->firstStaticScriptablePrimaryLight;
		world->staticScriptablePrimaryLightCount = asset->staticScriptablePrimaryLightCount;
		world->firstMovingScriptablePrimaryLight = asset->firstMovingScriptablePrimaryLight;
		world->movingScriptablePrimaryLightCount = asset->movingScriptablePrimaryLightCount;
		world->bounds = std::bit_cast<target::Bounds>(asset->bounds);

		world->sortKeyLitDecal = 7;
		world->sortKeyEffectDecal = 14;
		world->sortKeyTopDecal = 17;
		world->sortKeyEffectAuto = 35;
		world->sortKeyDistortion = 24;
		world->sortKeyEffectDistortion = 36;
		world->sortKey2D = 41;
		world->sortKeyOpaqueBegin = 1;
		world->sortKeyOpaqueEnd = 6;
		world->sortKeyDecalBegin = 7;
		world->sortKeyDecalEnd = 17;
		world->sortKeyTransBegin = 18;
		world->sortKeyTransEnd = 34;
		world->sortKeyEmissiveBegin = 35;
		world->sortKeyEmissiveEnd = 40;

		auto* zone = allocator.allocate<target::GfxWorldTransientZone>();
		zone->name = allocator.duplicate_string(source_zone->name);

		auto& verts = zone->drawVerts;
		verts.posDataSize = static_cast<unsigned int>(geometry.positions.size());
		verts.posData = copy_array<char>(geometry.positions.data(), geometry.positions.size(), allocator);
		verts.auxDataSize = static_cast<unsigned int>(geometry.aux.size());
		verts.auxData = copy_array<char>(geometry.aux.data(), geometry.aux.size(), allocator);
		verts.indexCount = source_zone->drawVerts.indexCount;
		verts.indices = copy_array<unsigned short>(source_zone->drawVerts.indices, source_zone->drawVerts.indexCount, allocator);

		convert_surfaces(asset, source_zone->drawVerts, geometry, world, allocator);
		convert_static_models(asset, world, allocator);
		make_brush_models(asset, world, allocator);
		make_single_cell(world, zone, allocator);
		convert_sun(asset, world, allocator);

		world->draw.transientZoneCount = 1;
		world->draw.transientZones[0] = zone;
		world->draw.bakedLightScale = 1.0f;
		world->draw.iesLookupTexture = gfximage::image_reference(asset->iesLookupTexture, allocator);
		convert_reflection_probes(asset, source_zone, world, zone, allocator);
		convert_lightmaps(asset, source_zone, world, zone, allocator);
		convert_voxel_trees(asset, world, allocator);
		convert_light_grid(asset, source_zone, world, zone, allocator);

		convert_dpvs(asset, world, allocator);
		make_dpvs_dynamic(asset, world, allocator);
		convert_decal_volumes(asset, source_zone, world, zone, allocator);
		make_runtime_bits(world, allocator);
		make_light_aabb(world, allocator);
		convert_light_frustums(asset, world, allocator);
		convert_heightfields(asset, world, allocator);
		if (!convert_umbra_tome(asset, world, allocator))
		{
			build_umbra_tome(world, allocator);
		}

		ZONETOOL_INFO("gfxworld \"%s\": %u surfaces, %u vertex buffers, %u static model collections, %u models", asset->name, world->surfaces.count,
			world->surfaces.surfDataCount, world->smodels.collectionCount, world->smodels.modelCount);

		target::gfx_world_transient_zone::dump(zone);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE), zone->name);
		target::gfx_world::dump(world);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_GFXWORLD), world->name);
	}
}
