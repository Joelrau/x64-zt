#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "material.hpp"
#include "xsurface.hpp"

#include <iw8/assets/gfxworld.hpp>
#include <iw8/assets/gfxworldtransientzone.hpp>

namespace zonetool::iw7::converter::iw8::gfxworld
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename T>
		T* make_named(const char* name, utils::memory::allocator& allocator)
		{
			if (!name)
			{
				return nullptr;
			}

			auto* asset = allocator.allocate<T>();
			asset->name = allocator.duplicate_string(name);
			return asset;
		}

		template <typename T, typename S>
		T* make_named(const S* source, utils::memory::allocator& allocator)
		{
			return source ? make_named<T>(source->name, allocator) : nullptr;
		}

		target::Material* convert_material(const Material* handle, utils::memory::allocator& allocator)
		{
			if (!handle || !handle->name)
			{
				return nullptr;
			}

			auto* material = const_cast<Material*>(handle);
			if (handle->name[0] != '*')
			{
				material::dump(material);
				return make_named<target::Material>(handle, allocator);
			}

			const auto name = std::string(handle->name);
			const auto generated = std::format("maps/mp/generated/{}", name.substr(1));
			add_csv_row("rename", std::format("{},{}", name, generated)); // '*' is not a valid file name

			const auto* original = handle->name;
			const auto _ = gsl::finally([material, original] { material->name = original; });
			material->name = generated.c_str();
			material::dump(material);
			material->name = original;
			return make_named<target::Material>(handle, allocator);
		}

		target::Material* convert_lit_material(const Material* handle, const techset::generated_techset techset, utils::memory::allocator& allocator)
		{
			const auto name = material::dump_lit(const_cast<Material*>(handle), techset);
			return name.empty() ? nullptr : make_named<target::Material>(name.c_str(), allocator);
		}

		target::GfxImage* convert_image(const GfxImage* handle, utils::memory::allocator& allocator)
		{
			if (!handle || !handle->name)
			{
				return nullptr;
			}

			const auto* image = static_cast<GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, handle->name));
			if (handle->name[0] == '*' || handle->name[0] == '$' || !image || !image->pixelData)
			{
				add_csv_row("image", ","s + handle->name); // TODO: the image is not loaded
			}

			return make_named<target::GfxImage>(handle, allocator);
		}

		template <typename T>
		T* make_array(const std::size_t count, utils::memory::allocator& allocator)
		{
			return count ? allocator.allocate_array<T>(count) : nullptr;
		}

		template <typename T>
		T* copy_array(const T* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			if (!source || !count)
			{
				return nullptr;
			}

			auto* copy = allocator.allocate_array<T>(count);
			std::memcpy(copy, source, sizeof(T) * count);
			return copy;
		}

		template <typename T, typename S>
		T* convert_array(const S* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(T) == sizeof(S));

			if (!source || !count)
			{
				return nullptr;
			}

			auto* copy = allocator.allocate_array<T>(count);
			std::memcpy(copy, source, sizeof(S) * count);
			return copy;
		}

		void append(std::vector<unsigned char>& data, const void* value, const std::size_t size)
		{
			const auto* bytes = static_cast<const unsigned char*>(value);
			data.insert(data.end(), bytes, bytes + size);
		}

		std::uint32_t pack_vertex_frame(const GfxWorldVertex& vertex)
		{
			return xsurface::pack_tangent_frame(xsurface::unpack_unit_vec(vertex.normal.packed),
				xsurface::unpack_unit_vec(vertex.tangent.packed), vertex.binormalSign < 0.0f);
		}

		std::array<float, 4> placement_quaternion(const GfxPackedPlacement& placement)
		{
			const auto& axis = placement.axis;
			std::array<float, 4> quaternion{};
			const auto trace = axis[0][0] + axis[1][1] + axis[2][2];
			if (trace > 0.0f)
			{
				const auto scale = std::sqrt(trace + 1.0f) * 2.0f;
				quaternion = {
					(axis[1][2] - axis[2][1]) / scale,
					(axis[2][0] - axis[0][2]) / scale,
					(axis[0][1] - axis[1][0]) / scale,
					scale * 0.25f,
				};
			}
			else if (axis[0][0] > axis[1][1] && axis[0][0] > axis[2][2])
			{
				const auto scale = std::sqrt(1.0f + axis[0][0] - axis[1][1] - axis[2][2]) * 2.0f;
				quaternion = {
					scale * 0.25f,
					(axis[1][0] + axis[0][1]) / scale,
					(axis[2][0] + axis[0][2]) / scale,
					(axis[1][2] - axis[2][1]) / scale,
				};
			}
			else if (axis[1][1] > axis[2][2])
			{
				const auto scale = std::sqrt(1.0f + axis[1][1] - axis[0][0] - axis[2][2]) * 2.0f;
				quaternion = {
					(axis[1][0] + axis[0][1]) / scale,
					scale * 0.25f,
					(axis[2][1] + axis[1][2]) / scale,
					(axis[2][0] - axis[0][2]) / scale,
				};
			}
			else
			{
				const auto scale = std::sqrt(1.0f + axis[2][2] - axis[0][0] - axis[1][1]) * 2.0f;
				quaternion = {
					(axis[2][0] + axis[0][2]) / scale,
					(axis[2][1] + axis[1][2]) / scale,
					scale * 0.25f,
					(axis[0][1] - axis[1][0]) / scale,
				};
			}

			const auto length = std::sqrt(quaternion[0] * quaternion[0] + quaternion[1] * quaternion[1]
				+ quaternion[2] * quaternion[2] + quaternion[3] * quaternion[3]);
			if (length > 0.0f)
			{
				for (auto& value : quaternion)
				{
					value /= length;
				}
			}

			return quaternion;
		}

		unsigned short pack_quaternion_component(const float value)
		{
			return static_cast<unsigned short>(std::lround(std::clamp((value + 1.0f) * 0.5f, 0.0f, 1.0f) * 65535.0f));
		}

		target::GfxSModelInstanceData convert_instance(const GfxPackedPlacement& placement)
		{
			target::GfxSModelInstanceData instance{};
			for (auto i = 0; i < 3; i++)
			{
				instance.translation[i] = static_cast<int>(std::lround(placement.origin[i] * 4096.0f));
			}

			const auto quaternion = placement_quaternion(placement);
			for (auto i = 0; i < 4; i++)
			{
				instance.orientation[i] = pack_quaternion_component(quaternion[i]);
			}

			instance.scale = placement.scale;
			return instance;
		}

		target::Bounds transform_bounds(const Bounds& bounds, const GfxPackedPlacement& placement)
		{
			target::Bounds result{};
			for (auto i = 0; i < 3; i++)
			{
				result.midPoint[i] = placement.origin[i];
				for (auto j = 0; j < 3; j++)
				{
					result.midPoint[i] += placement.axis[j][i] * bounds.midPoint[j] * placement.scale;
					result.halfSize[i] += std::abs(placement.axis[j][i]) * bounds.halfSize[j] * placement.scale;
				}
			}

			return result;
		}

		bool build_surface(const GfxWorld* asset, const unsigned int index,
			std::map<std::array<unsigned int, 3>, target::GfxWorldSurfData>& blocks, world_geometry& geometry)
		{
			const auto& surface = asset->dpvs.surfaces[index];
			auto& data = geometry.surf_data[index];

			const auto* zone = asset->draw.transientZones[surface.transientZone];
			if (!zone || !zone->vd.vertices || !asset->draw.indices)
			{
				return false;
			}

			const auto first_vertex = surface.tris.firstVertex;
			const auto vertex_count = static_cast<unsigned int>(surface.tris.vertexCount);
			const auto index_count = static_cast<unsigned int>(surface.tris.triCount) * 3;
			if (!vertex_count || !index_count)
			{
				return false;
			}

			if (first_vertex + vertex_count > zone->vertexCount
				|| surface.tris.baseIndex + index_count > asset->draw.indexCount)
			{
				ZONETOOL_WARNING("gfxworld \"%s\": surface %u is outside the vertex or index data", asset->name, index);
				return false;
			}

			const auto* vertices = zone->vd.vertices + first_vertex;
			const auto* indices = asset->draw.indices + surface.tris.baseIndex;

			for (auto i = 0u; i < index_count; i++)
			{
				if (indices[i] >= vertex_count)
				{
					ZONETOOL_WARNING("gfxworld \"%s\": surface %u has the vertex index %u outside its range", asset->name,
						index, indices[i]);
					return false;
				}
			}

			const std::array<unsigned int, 3> key{surface.transientZone, first_vertex, vertex_count};
			const auto found = blocks.find(key);
			if (found != blocks.end())
			{
				data = found->second;
			}
			else
			{
				data.transientZone = 0;
				data.layerCount = 1;

				data.xyzOffset = static_cast<unsigned int>(geometry.positions.size());
				for (auto i = 0u; i < vertex_count; i++)
				{
					append(geometry.positions, vertices[i].xyz, sizeof(vertices[i].xyz));
				}

				data.tangentFrameOffset = static_cast<unsigned int>(geometry.aux.size());
				for (auto i = 0u; i < vertex_count; i++)
				{
					const auto frame = pack_vertex_frame(vertices[i]);
					append(geometry.aux, &frame, sizeof(frame));
				}

				data.texCoordOffset = static_cast<unsigned int>(geometry.aux.size());
				for (auto i = 0u; i < vertex_count; i++)
				{
					append(geometry.aux, vertices[i].texCoord, sizeof(vertices[i].texCoord));
				}

				data.lmapCoordOffset = static_cast<unsigned int>(geometry.aux.size());
				for (auto i = 0u; i < vertex_count; i++)
				{
					const float coord[2]
					{
						(vertices[i].lmapCoord[0] - std::floor(vertices[i].lmapCoord[0])) * 4.0f - 1.0f,
						(vertices[i].lmapCoord[1] - std::floor(vertices[i].lmapCoord[1])) * 4.0f - 1.0f,
					};
					append(geometry.aux, coord, sizeof(coord));
				}

				data.colorOffset = static_cast<unsigned int>(geometry.aux.size());
				for (auto i = 0u; i < vertex_count; i++)
				{
					append(geometry.aux, &vertices[i].color.packed, sizeof(vertices[i].color.packed));
				}

				blocks.emplace(key, data);
			}

			geometry.base_index[index] = static_cast<unsigned int>(geometry.indices.size());
			for (auto i = 0u; i < index_count; i++)
			{
				geometry.indices.emplace_back(indices[i]);
			}

			return true;
		}

		world_geometry build_geometry(const GfxWorld* asset, const unsigned int surface_count)
		{
			world_geometry geometry;
			geometry.positions.resize(4);
			geometry.aux.resize(4);
			geometry.surf_data.resize(surface_count);
			geometry.base_index.resize(surface_count);
			geometry.valid.resize(surface_count);

			std::map<std::array<unsigned int, 3>, target::GfxWorldSurfData> blocks;
			for (auto i = 0u; i < surface_count; i++)
			{
				geometry.valid[i] = build_surface(asset, i, blocks, geometry) ? 1 : 0;
			}

			return geometry;
		}

		void convert_cells(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& source = asset->dpvsPlanes;
			const auto cell_count = static_cast<unsigned int>(source.cellCount);

			auto& planes = world->dpvsPlanes;
			planes.cellCount = cell_count;
			planes.planeCount = static_cast<unsigned short>(asset->planeCount);
			planes.nodeCount = static_cast<unsigned short>(asset->nodeCount);
			planes.planes = make_array<target::GfxWorldDpvsPlane>(planes.planeCount, allocator);
			for (auto i = 0u; i < planes.planeCount && source.planes; i++)
			{
				planes.planes[i] = std::bit_cast<target::GfxWorldDpvsPlane>(source.planes[i]);
			}

			planes.nodes = make_array<unsigned short>(planes.nodeCount, allocator);
			if (planes.nodes && source.nodes)
			{
				std::memcpy(planes.nodes, source.nodes, sizeof(unsigned short) * planes.nodeCount);
			}

			planes.sceneEntCellBits = make_array<unsigned int>(static_cast<std::size_t>(cell_count) << 9, allocator);

			world->cellTransientInfos = make_array<target::GfxCellTransientInfo>(cell_count, allocator);
			world->cells = make_array<target::GfxCell>(cell_count, allocator);
			for (auto i = 0u; i < cell_count; i++)
			{
				const auto& cell = asset->cells[i];
				auto& converted = world->cells[i];
				converted.bounds = std::bit_cast<target::Bounds>(cell.bounds);
				converted.portalCount = cell.portalCount;
				converted.portals = make_array<target::GfxPortal>(cell.portalCount, allocator);
				for (auto j = 0u; j < cell.portalCount; j++)
				{
					const auto& portal = cell.portals[j];
					auto& new_portal = converted.portals[j];
					new_portal.plane = std::bit_cast<target::DpvsPlane>(portal.plane);
					new_portal.cellIndex = portal.cellIndex;
					new_portal.closeDistance = portal.closeDistance;
					new_portal.vertexCount = static_cast<unsigned char>(portal.vertexCount);
					std::memcpy(new_portal.hullAxis, portal.hullAxis, sizeof(new_portal.hullAxis));
					new_portal.vertices = make_array<target::vec3_t>(portal.vertexCount, allocator);
					if (new_portal.vertices)
					{
						std::memcpy(new_portal.vertices, portal.vertices, sizeof(target::vec3_t) * portal.vertexCount);
					}
				}

				world->cellTransientInfos[i].aabbTreeIndex = static_cast<unsigned short>(i);
				world->cellTransientInfos[i].transientZone = 0;
			}
		}

		target::GfxAabbTree convert_aabb_tree(const GfxAabbTree& tree, utils::memory::allocator& allocator)
		{
			target::GfxAabbTree converted{};
			converted.bounds = std::bit_cast<target::Bounds>(tree.bounds);
			converted.surfaceCount = tree.surfaceCount;
			converted.startSurfIndex = tree.startSurfIndex;
			converted.childrenOffset = tree.childrenOffset / static_cast<int>(sizeof(GfxAabbTree))
				* static_cast<int>(sizeof(target::GfxAabbTree));
			converted.childCount = tree.childCount;
			converted.smodelIndexCount = tree.smodelIndexCount;
			converted.smodelIndexes = copy_array(tree.smodelIndexes, tree.smodelIndexCount, allocator);
			return converted;
		}

		void convert_cell_trees(const GfxWorld* asset, target::GfxWorldTransientZone* zone, utils::memory::allocator& allocator)
		{
			const auto cell_count = static_cast<unsigned int>(asset->dpvsPlanes.cellCount);
			zone->drawCells.cellCount = cell_count;
			zone->drawCells.aabbTreeCounts = make_array<target::GfxCellTreeCount>(cell_count, allocator);
			zone->drawCells.aabbTrees = make_array<target::GfxCellTree>(cell_count, allocator);

			for (auto i = 0u; i < cell_count; i++)
			{
				const auto& info = asset->cellTransientInfos[i];
				const auto* source_zone = asset->draw.transientZones[info.transientZone];
				if (!source_zone || !source_zone->aabbTreeCounts || !source_zone->aabbTrees)
				{
					continue;
				}

				const auto count = source_zone->aabbTreeCounts[info.aabbTreeIndex].aabbTreeCount;
				const auto* trees = source_zone->aabbTrees[info.aabbTreeIndex].aabbTree;
				if (count <= 0 || !trees)
				{
					continue;
				}

				zone->drawCells.aabbTreeCounts[i].aabbTreeCount = count;
				zone->drawCells.aabbTrees[i].aabbTree = make_array<target::GfxAabbTree>(count, allocator);
				for (auto j = 0; j < count; j++)
				{
					zone->drawCells.aabbTrees[i].aabbTree[j] = convert_aabb_tree(trees[j], allocator);
				}
			}
		}

		void convert_surfaces(const GfxWorld* asset, const world_geometry& geometry, target::GfxWorld* world,
			utils::memory::allocator& allocator)
		{
			const auto count = asset->surfaceCount;
			auto& surfaces = world->surfaces;
			surfaces.count = count;
			surfaces.surfDataCount = count;
			surfaces.litOpaqueSurfsBegin = asset->dpvs.litOpaqueSurfsBegin;
			surfaces.litOpaqueSurfsEnd = asset->dpvs.litOpaqueSurfsEnd;
			surfaces.litDecalSurfsBegin = asset->dpvs.litDecalSurfsBegin;
			surfaces.litDecalSurfsEnd = asset->dpvs.litDecalSurfsEnd;
			surfaces.litTransSurfsBegin = asset->dpvs.litTransSurfsBegin;
			surfaces.litTransSurfsEnd = asset->dpvs.litTransSurfsEnd;
			surfaces.emissiveSurfsBegin = asset->dpvs.emissiveSurfsBegin;
			surfaces.emissiveSurfsEnd = asset->dpvs.emissiveSurfsEnd;

			surfaces.surfaces = make_array<target::GfxSurface>(count, allocator);
			surfaces.surfaceBounds = make_array<target::GfxSurfaceBounds>(count, allocator);
			surfaces.surfaceMaterials = make_array<target::GfxDrawSurf>(count, allocator);
			surfaces.surfData = make_array<target::GfxWorldSurfData>(count, allocator);

			auto dropped = 0u;
			for (auto i = 0u; i < count; i++)
			{
				const auto& source = asset->dpvs.surfaces[i];
				auto& converted = surfaces.surfaces[i];
				converted.material = convert_lit_material(source.material, techset::GENERATED_TECHSET_WORLD, allocator);
				if (geometry.valid[i] && converted.material)
				{
					converted.tris.posOffset = geometry.surf_data[i].xyzOffset;
					converted.tris.vertexCount = source.tris.vertexCount;
					converted.tris.triCount = source.tris.triCount;
					converted.tris.baseIndex = geometry.base_index[i];
				}
				else if (!converted.material)
				{
					dropped++;
				}

				converted.surfDataIndex = i;
				converted.transientZone = 0;
				converted.lightmapIndex = 0;
				converted.flags = 0;
				surfaces.surfData[i] = geometry.surf_data[i];
				surfaces.surfaceBounds[i].bounds = std::bit_cast<target::Bounds>(asset->dpvs.surfacesBounds[i].bounds);
			}

			if (dropped)
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %u surfaces have a material that is not loaded", asset->name, dropped);
			}
		}

		void convert_static_models(const GfxWorld* asset, target::GfxWorld* world, std::vector<target::Bounds>& instance_bounds,
			utils::memory::allocator& allocator)
		{
			const auto instance_count = static_cast<unsigned int>(asset->dpvs.smodelCount);
			if (instance_count > 65535)
			{
				ZONETOOL_ERROR("gfxworld \"%s\" has %u static model instances, IW8 supports 65535", asset->name, instance_count);
				return;
			}

			std::vector<smodel_layout> models;
			std::unordered_map<std::string_view, unsigned int> model_index;
			std::vector<unsigned int> instance_model(instance_count);

			for (auto i = 0u; i < instance_count; i++)
			{
				const auto* handle = asset->dpvs.smodelDrawInsts[i].model;
				const auto name = handle ? std::string_view(handle->name) : std::string_view();
				const auto found = model_index.find(name);
				if (found != model_index.end())
				{
					instance_model[i] = found->second;
					continue;
				}

				instance_model[i] = static_cast<unsigned int>(models.size());
				model_index.emplace(name, instance_model[i]);
				models.emplace_back(smodel_layout{handle, 0, 0, 0, {}, {}});
			}

			auto total_surfaces = 0u;
			for (auto& entry : models)
			{
				entry.first_surface = total_surfaces;
				if (!entry.model)
				{
					continue;
				}

				auto surfaces = 0u;
				const auto lod_count = std::min<unsigned int>(entry.model->numLods, 6);
				for (auto lod = 0u; lod < lod_count; lod++)
				{
					const auto count = static_cast<unsigned int>(entry.model->lodInfo[lod].numsurfs);
					if (surfaces + count > 255) // GfxStaticModelExpansion::numSurfs is a byte
					{
						break;
					}

					entry.lod_info[entry.lod_count].surfOffset = static_cast<unsigned char>(surfaces);
					entry.lod_info[entry.lod_count].surfCount = static_cast<unsigned char>(count);
					entry.lod_dist[entry.lod_count] = entry.model->lodInfo[lod].dist;
					entry.lod_count++;
					surfaces += count;
				}

				entry.surface_count = surfaces;
				total_surfaces += surfaces;
			}

			auto& smodels = world->smodels;
			smodels.modelCount = static_cast<unsigned int>(models.size());
			smodels.collectionCount = instance_count;
			smodels.instanceCount = instance_count;
			smodels.surfaceCount = total_surfaces;

			smodels.models = make_array<target::GfxStaticModel>(smodels.modelCount, allocator);
			smodels.surfaces = make_array<target::GfxStaticModelSurface>(total_surfaces, allocator);
			smodels.collections = make_array<target::GfxStaticModelCollection>(instance_count, allocator);
			smodels.instances = make_array<target::GfxStaticModelInstance>(instance_count, allocator);
			smodels.instanceFlags = make_array<unsigned char>(instance_count, allocator);
			smodels.collectionBounds = make_array<target::Bounds>(instance_count, allocator);
			smodels.modelStaticIndirection = make_array<target::GfxModelStaticIndirection>(instance_count, allocator);
			smodels.smodelSurfData = make_array<target::GfxBaseSModelSurfData>(total_surfaces, allocator);
			smodels.smodelInstanceData = make_array<target::GfxSModelInstanceData>(instance_count, allocator);
			smodels.smodelExpansionData = make_array<target::GfxStaticModelExpansion>(smodels.modelCount, allocator);
			smodels.smodelSurfMatIndirection = make_array<target::GfxSModelSurfMatIndirection>(total_surfaces, allocator);
			smodels.smodelSurfUGBIndirection = make_array<unsigned int>(total_surfaces, allocator);

			const auto vis_words = (instance_count + 31) >> 5;
			smodels.collectionTransientZonePrevFrameVisibility.array = make_array<unsigned int>(vis_words, allocator);
			smodels.collectionTransientZonePrevFrameVisibility.wordCount = vis_words;
			smodels.collectionTransientZonePrevFrameVisibility.bitCount = instance_count;
			smodels.collectionVisRefCount = make_array<unsigned char>(instance_count, allocator);

			std::vector<const Material*> materials;
			std::unordered_map<std::string_view, unsigned short> material_index;
			auto surface = 0u;

			for (auto m = 0u; m < models.size(); m++)
			{
				const auto& entry = models[m];
				auto& model = smodels.models[m];
				model.model = make_named<target::XModel>(entry.model, allocator);
				model.flags = target::STATIC_MODEL_FLAG_LIGHTPROBE_LIGHTING;
				model.firstStaticModelSurfaceIndex = static_cast<unsigned short>(entry.first_surface);

				auto& expansion = smodels.smodelExpansionData[m];
				expansion.firstStaticModelSurfaceIndex = static_cast<unsigned short>(entry.first_surface);
				expansion.numSurfs = static_cast<unsigned char>(entry.surface_count);
				expansion.numLods = entry.lod_count;
				if (entry.model)
				{
					expansion.modelBounds = std::bit_cast<target::Bounds>(entry.model->bounds);
				}

				for (auto lod = 0u; lod < entry.lod_count; lod++)
				{
					expansion.lodInfo[lod] = entry.lod_info[lod];
					expansion.lodDist[lod] = entry.lod_dist[lod];
				}

				for (auto lod = 0u; lod < entry.lod_count; lod++)
				{
					const auto& info = entry.model->lodInfo[lod];
					for (auto s = 0u; s < info.numsurfs; s++)
					{
						auto& surface_entry = smodels.surfaces[surface];
						surface_entry.smodelIndex = static_cast<unsigned short>(m);
						surface_entry.lodIndex = static_cast<unsigned char>(lod);
						surface_entry.surfIndex = static_cast<unsigned char>(info.surfIndex + s);

						const auto* material = info.surfIndex + s < entry.model->numsurfs
							? entry.model->materialHandles[info.surfIndex + s]
							: nullptr;
						const auto name = material ? std::string_view(material->name) : std::string_view();
						auto found = material_index.find(name);
						if (found == material_index.end())
						{
							if (materials.size() >= 65535)
							{
								ZONETOOL_ERROR("gfxworld \"%s\" has more than 65535 static model materials", asset->name);
								return;
							}

							found = material_index.emplace(name, static_cast<unsigned short>(materials.size())).first;
							materials.emplace_back(material);
						}

						auto& indirection = smodels.smodelSurfMatIndirection[surface];
						indirection.smodelMaterialIndex = found->second;
						indirection.cullType = target::GFX_RASTERIZER_CULL_NONE;
						indirection.emitType = 0x0F;

						const auto& bounds = entry.model->bounds;
						auto& data = smodels.smodelSurfData[surface];
						data.boundingBoxData[0] = bounds.midPoint[0];
						data.boundingBoxData[1] = bounds.midPoint[1];
						data.boundingBoxData[2] = bounds.midPoint[2];
						data.boundingBoxData[3] = std::max({bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2]});
						std::memcpy(data.boundingBoxHalfSize, bounds.halfSize, sizeof(data.boundingBoxHalfSize));
						data.drawDistance = info.dist;

						surface++;
					}
				}
			}

			smodels.materialCount = static_cast<unsigned int>(materials.size());
			smodels.materials = make_array<target::Material*>(materials.size(), allocator);
			for (auto i = 0u; i < materials.size(); i++)
			{
				smodels.materials[i] = convert_lit_material(materials[i], techset::GENERATED_TECHSET_STATIC_MODEL, allocator);
				if (!smodels.materials[i])
				{
					ZONETOOL_WARNING("gfxworld \"%s\": static model material \"%s\" is not loaded", asset->name,
						materials[i] ? materials[i]->name : "");
				}
			}

			instance_bounds.resize(instance_count);
			for (auto i = 0u; i < instance_count; i++)
			{
				const auto& inst = asset->dpvs.smodelDrawInsts[i];
				auto& collection = smodels.collections[i];
				collection.firstInstance = i;
				collection.instanceCount = 1;
				collection.smodelIndex = static_cast<unsigned short>(instance_model[i]);
				collection.flags = (inst.flags & STATIC_MODEL_FLAG_NO_CAST_SHADOW)
					? 0
					: static_cast<unsigned char>((1u << std::min(asset->lastSunPrimaryLightIndex, 5u)) - 1);

				smodels.instanceFlags[i] = static_cast<unsigned char>(inst.flags & STATIC_MODEL_FLAG_SCALE_MODIFIER_MASK);
				smodels.collectionBounds[i] = std::bit_cast<target::Bounds>(asset->dpvs.smodelInsts[i].bounds);
				instance_bounds[i] = smodels.collectionBounds[i];

				auto& indirection = smodels.modelStaticIndirection[i];
				indirection.lightingInstanceIndirection = i;

				smodels.smodelInstanceData[i] = convert_instance(inst.placement);
			}

			const auto sorted_count = 32 * ((instance_count + 31) >> 5) + 1;
			auto& dpvs = world->dpvs;
			dpvs.sortedSmodelIndices = make_array<unsigned short>(sorted_count, allocator);
			if (dpvs.sortedSmodelIndices && instance_count)
			{
				std::vector<unsigned short> order(instance_count);
				std::iota(order.begin(), order.end(), unsigned short{});
				std::stable_sort(order.begin(), order.end(), [&](const unsigned short left, const unsigned short right)
				{
					return instance_model[left] < instance_model[right];
				});

				std::copy(order.begin(), order.end(), dpvs.sortedSmodelIndices);
			}

			ZONETOOL_INFO("gfxworld \"%s\": %u static model instances, %u models, %u surfaces", asset->name, instance_count,
				smodels.modelCount, total_surfaces);
		}

		void convert_sun(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->sun = std::bit_cast<target::sunflare_t>(asset->sun);
			world->sun.spriteMaterial = convert_material(asset->sun.spriteMaterial, allocator);
			world->sun.flareMaterial = convert_material(asset->sun.flareMaterial, allocator);
		}

		void convert_draw(const GfxWorld* asset, target::GfxWorldTransientZone* zone, target::GfxWorld* world,
			utils::memory::allocator& allocator)
		{
			auto& draw = world->draw;
			draw.iesLookupTexture = convert_image(asset->draw.iesLookupTexture, allocator);
			draw.lightGridType = 0;
			draw.lightmapType = target::GFX_LIGHTMAP_TYPE_REGULAR_UNCOMPRESSED;
			draw.lightmapCount = 0;
			draw.transientZoneCount = 1;
			draw.transientZones[0] = zone;
			draw.bakedLightScale = 1.0f;

			world->outdoorImage = convert_image(asset->outdoorImage, allocator);
			world->dustMaterial = convert_material(asset->dustMaterial, allocator);
			world->materialLod0SizeThreshold = 0.5f;
			world->checksum = asset->checksum;
			world->bounds = std::bit_cast<target::Bounds>(asset->bounds);
		}

		void convert_frustum_lights(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto light_count = world->primaryLightCount;
			auto& lights = world->frustumLights;
			lights.lightCount = light_count;
			lights.indicesOffsets = make_array<unsigned int>(light_count, allocator);
			lights.verticesOffsets = make_array<unsigned int>(light_count, allocator);
			lights.indicesCounts = make_array<unsigned short>(light_count, allocator);
			lights.verticesCounts = make_array<unsigned short>(light_count, allocator);

			for (auto i = 0u; i < light_count && asset->frustumLights; i++)
			{
				const auto& source = asset->frustumLights[i];
				lights.indicesOffsets[i] = lights.totalIndicesCount;
				lights.verticesOffsets[i] = lights.totalVerticesCount;
				lights.indicesCounts[i] = static_cast<unsigned short>(source.indexCount);
				lights.verticesCounts[i] = static_cast<unsigned short>(source.vertexCount);
				lights.totalIndicesCount += source.indexCount;
				lights.totalVerticesCount += source.vertexCount;
			}

			lights.indices = make_array<unsigned short>(lights.totalIndicesCount, allocator);
			lights.vertices = make_array<unsigned char>(16 * lights.totalVerticesCount, allocator);
			for (auto i = 0u; i < light_count && asset->frustumLights; i++)
			{
				const auto& source = asset->frustumLights[i];
				std::memcpy(&lights.indices[lights.indicesOffsets[i]], source.indices, 2 * source.indexCount);
				for (auto j = 0u; j < source.vertexCount; j++)
				{
					std::memcpy(&lights.vertices[16 * (lights.verticesOffsets[i] + j)], &source.vertices[32 * j], 16);
				}
			}
		}

		void convert_lights(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto light_count = world->primaryLightCount;

			world->lightViewFrustums = make_array<target::GfxLightViewFrustum>(light_count, allocator);
			for (auto i = 0u; i < light_count && asset->lightViewFrustums; i++)
			{
				const auto& source = asset->lightViewFrustums[i];
				auto& converted = world->lightViewFrustums[i];
				if (source.planeCount > 255 || source.indexCount > 255 || source.vertexCount > 255)
				{
					ZONETOOL_WARNING("gfxworld \"%s\": light view frustum %u does not fit the IW8 byte counts", asset->name, i);
					continue;
				}

				converted.planeCount = static_cast<unsigned char>(source.planeCount);
				converted.planes = convert_array<target::vec4_t>(source.planes, source.planeCount, allocator);
				converted.indexCount = static_cast<unsigned char>(source.indexCount);
				converted.vertices = convert_array<target::vec3_t>(source.vertices, source.vertexCount, allocator);
				converted.vertexCount = static_cast<unsigned char>(source.vertexCount);

				if (source.indices && source.indexCount)
				{
					converted.indices = make_array<unsigned char>(source.indexCount, allocator);
					for (auto j = 0u; j < source.indexCount; j++)
					{
						if (source.indices[j] > 255)
						{
							ZONETOOL_WARNING("gfxworld \"%s\": light view frustum %u index %u exceeds a byte", asset->name, i,
								source.indices[j]);
							converted.indices[j] = 0;
							continue;
						}

						converted.indices[j] = static_cast<unsigned char>(source.indices[j]);
					}
				}
			}

			convert_frustum_lights(asset, world, allocator);
			world->shadowGeomOptimized = make_array<target::GfxShadowGeometry>(light_count, allocator);
			for (auto i = 0u; i < light_count && asset->shadowGeomOptimized; i++)
			{
				const auto& source = asset->shadowGeomOptimized[i];
				auto& converted = world->shadowGeomOptimized[i];
				converted.surfaceCount = source.surfaceCount;
				converted.smodelCount = source.smodelCount;
				converted.sortedSurfIndex = copy_array(source.sortedSurfIndex, source.surfaceCount, allocator);
				converted.smodelIndex = copy_array(source.smodelIndex, source.smodelCount, allocator);
			}

			world->lightAABB.nodeCount = asset->lightAABB.nodeCount;
			world->lightAABB.lightCount = asset->lightAABB.lightCount;
			world->lightAABB.nodeArray = make_array<target::GfxLightAABBNode>(asset->lightAABB.nodeCount, allocator);
			for (auto i = 0u; i < asset->lightAABB.nodeCount && asset->lightAABB.nodeArray; i++)
			{
				world->lightAABB.nodeArray[i] = std::bit_cast<target::GfxLightAABBNode>(asset->lightAABB.nodeArray[i]);
			}

			world->lightAABB.lightArray = copy_array(asset->lightAABB.lightArray, asset->lightAABB.lightCount, allocator);
		}

		void convert_models(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->modelCount = asset->modelCount;
			world->models = make_array<target::GfxBrushModel>(asset->modelCount, allocator);
			for (auto i = 0u; i < static_cast<unsigned int>(asset->modelCount); i++)
			{
				const auto& source = asset->models[i];
				auto& converted = world->models[i];
				converted.writable.bounds = std::bit_cast<target::Bounds>(source.bounds);
				converted.bounds = std::bit_cast<target::Bounds>(source.bounds);
				converted.radius = source.radius;
				converted.startSurfIndex = source.startSurfIndex;
				converted.surfaceCount = source.surfaceCount;
				converted.mdaoVolumeIndex = -1;
			}
		}

		void convert_voxel_trees(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->voxelTreeCount = asset->voxelTreeCount;
			world->voxelTree = make_array<target::GfxVoxelTree>(asset->voxelTreeCount, allocator);
			for (auto i = 0; i < asset->voxelTreeCount && asset->voxelTree; i++)
			{
				const auto& source = asset->voxelTree[i];
				auto& converted = world->voxelTree[i];
				converted.zoneBound = std::bit_cast<target::Bounds>(source.zoneBound);
				converted.voxelTopDownViewNodeCount = source.voxelTopDownViewNodeCount;
				converted.voxelInternalNodeCount = source.voxelInternalNodeCount;
				converted.voxelLeafNodeCount = source.voxelLeafNodeCount;
				converted.lightListArraySize = source.lightListArraySize;

				if (source.voxelTreeHeader)
				{
					converted.voxelTreeHeader = allocator.allocate<target::GfxVoxelTreeHeader>();
					*converted.voxelTreeHeader = std::bit_cast<target::GfxVoxelTreeHeader>(*source.voxelTreeHeader);
				}

				converted.voxelTopDownViewNodeArray = convert_array<target::GfxVoxelTopDownViewNode>(
					source.voxelTopDownViewNodeArray, source.voxelTopDownViewNodeCount, allocator);
				converted.voxelInternalNodeArray = convert_array<target::GfxVoxelInternalNode>(
					source.voxelInternalNodeArray, source.voxelInternalNodeCount, allocator);
				converted.voxelLeafNodeArray = convert_array<target::GfxVoxelLeafNode>(
					source.voxelLeafNodeArray, source.voxelLeafNodeCount, allocator);
				converted.lightListArray = copy_array(source.lightListArray, source.lightListArraySize, allocator);
				converted.voxelInternalNodeDynamicLightListCPU = copy_array(source.voxelInternalNodeDynamicLightList,
					2 * static_cast<std::size_t>(source.voxelInternalNodeCount), allocator);
			}
		}

		void convert_heightfields(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->heightfieldCount = 0; // TODO: the heightmap images are generated and not converted
		}

		void convert_dpvs(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto surface_count = world->surfaces.count;
			const auto instance_count = world->smodels.instanceCount;

			auto& dpvs = world->dpvs;
			dpvs.smodelVisDataCount = (instance_count + 31) >> 5;
			dpvs.surfaceVisDataCount = (surface_count + 31) >> 5;
			dpvs.primaryLightVisDataCount = (world->primaryLightCount + 31) >> 5;
			dpvs.reflectionProbeVisDataCount = 0;
			dpvs.volumetricVisDataCount = 0;
			dpvs.decalVisDataCount = 0;

			for (auto i = 0u; i < 32; i++)
			{
				const auto skip = i >= 4 && i < 12;
				dpvs.smodelVisData[i] = make_array<unsigned int>(skip ? 0 : dpvs.smodelVisDataCount, allocator);
				dpvs.surfaceVisData[i] = make_array<unsigned int>(skip ? 0 : dpvs.surfaceVisDataCount, allocator);
			}

			dpvs.primaryLightVisData = make_array<unsigned int>(dpvs.primaryLightVisDataCount, allocator);

			const auto sorted_count = 32 * dpvs.surfaceVisDataCount;
			dpvs.sortedSurfIndex = make_array<unsigned int>(sorted_count, allocator);
			for (auto i = 0u; i < sorted_count; i++)
			{
				dpvs.sortedSurfIndex[i] = i < asset->dpvs.staticSurfaceCount && asset->dpvs.sortedSurfIndex
					? asset->dpvs.sortedSurfIndex[i]
					: 0;
			}

			dpvs.sunShadowOptCount = asset->dpvs.sunShadowOptCount;
			dpvs.sunSurfVisDataCount = asset->dpvs.sunSurfVisDataCount;
			dpvs.surfaceCastsSunShadow = make_array<unsigned int>(dpvs.surfaceVisDataCount, allocator);
			dpvs.surfaceCastsSunShadowOpt = make_array<unsigned int>(
				static_cast<std::size_t>(dpvs.sunShadowOptCount) * dpvs.sunSurfVisDataCount, allocator);
		}

		void convert_dpvs_dynamic(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto cell_count = world->dpvsPlanes.cellCount;
			auto& dyn = world->dpvsDyn;
			for (auto i = 0; i < 2; i++)
			{
				const auto count = asset->dpvsDyn.dynEntClientCount[i];
				dyn.dynEntClientCount[i] = count;
				dyn.dynEntClientWordCount[i] = 2 * ((count + 31) >> 5);
				dyn.dynEntCellBits[i] = make_array<unsigned int>(
					static_cast<std::size_t>(dyn.dynEntClientWordCount[i]) * cell_count, allocator);

				for (auto slot = 0u; slot < 32; slot++)
				{
					const auto skip = slot == 3 || slot >= 12;
					dyn.dynEntVisData[i][slot] = make_array<unsigned char>(skip ? 0 : 32 * dyn.dynEntClientWordCount[i],
						allocator);
				}
			}

			world->sceneDynModel = make_array<target::GfxSceneDynModel>(dyn.dynEntClientCount[0], allocator);
			world->sceneDynBrush = make_array<target::GfxSceneDynBrush>(dyn.dynEntClientCount[1], allocator);
		}

		void convert_runtime_bits(target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto cell_count = world->dpvsPlanes.cellCount;
			const auto cell_words = (cell_count + 31) >> 5;

			world->cellCasterBits = make_array<unsigned int>(static_cast<std::size_t>(cell_count) * cell_words, allocator);
			world->cellHasSunLitSurfsBits = make_array<unsigned int>(cell_words, allocator);

			const auto static_lights = static_cast<int>(world->primaryLightCount)
				- static_cast<int>(world->lastSunPrimaryLightIndex)
				- static_cast<int>(world->movingScriptablePrimaryLightCount) - 1;
			const auto aligned = static_cast<unsigned int>(std::max(static_lights, 0) + 31) & ~31u;

			world->staticSpotOmniPrimaryLightCountAligned = aligned;
			world->shadowBitsArrayPitch = aligned;
			world->primaryLightMotionDetectBitsEntries = aligned >> 4;
			world->entityMotionBitsEntries = 160;
			world->numPrimaryLightEntityShadowVisEntries = aligned * world->entityMotionBitsEntries;

			world->primaryLightMotionDetectBits = make_array<unsigned int>(world->primaryLightMotionDetectBitsEntries,
				allocator);
			world->entityMotionBits = make_array<unsigned int>(world->entityMotionBitsEntries, allocator);
			world->primaryLightEntityShadowVis = make_array<unsigned int>(world->numPrimaryLightEntityShadowVisEntries,
				allocator);

			for (auto i = 0; i < 2; i++)
			{
				world->dynEntMotionBitsEntries[i] = world->dpvsDyn.dynEntClientWordCount[i];
				world->dynEntMotionBits[i] = make_array<unsigned int>(world->dynEntMotionBitsEntries[i], allocator);

				world->numPrimaryLightDynEntShadowVisEntries[i] =
					(aligned * world->dpvsDyn.dynEntClientCount[i]) >> 4;
				world->primaryLightDynEntShadowVis[i] =
					make_array<unsigned int>(world->numPrimaryLightDynEntShadowVisEntries[i], allocator);
			}
		}

		unsigned int allocate_tome(std::vector<unsigned char>& tome, const std::size_t size)
		{
			tome.resize((tome.size() + 15) & ~std::size_t(15));
			const auto offset = static_cast<unsigned int>(tome.size());
			tome.resize(tome.size() + size);
			return offset;
		}

		template <typename T>
		void write_at(std::vector<unsigned char>& data, const std::size_t offset, const T& value)
		{
			std::memcpy(data.data() + offset, &value, sizeof(T));
		}

		unsigned int crc32c(const unsigned char* data, const std::size_t size)
		{
			auto crc = 0xFFFFFFFFu;
			for (std::size_t i = 0; i < size; i++)
			{
				crc ^= data[i];
				for (auto bit = 0; bit < 8; bit++)
				{
					crc = (crc >> 1) ^ (0x82F63B78u & (0u - (crc & 1u)));
				}
			}

			return ~crc;
		}

		unsigned int umbra_index_width(const unsigned int count)
		{
			auto width = 1u;
			while (width < 31 && (1u << width) < count)
			{
				width++;
			}

			return width;
		}

		std::vector<unsigned char> pack_umbra_object_list(const unsigned int count, const unsigned int width)
		{
			std::vector<unsigned char> result(((static_cast<std::uint64_t>(count) * width + 31) / 32) * 4);
			auto bit_offset = std::uint64_t{};
			for (auto object = 0u; object < count; object++)
			{
				for (auto bit = 0u; bit < width; bit++)
				{
					if (object & (1u << bit))
					{
						const auto current = bit_offset + bit;
						result[static_cast<std::size_t>(current) >> 3] |= static_cast<unsigned char>(1u << (current & 7));
					}
				}

				bit_offset += width;
			}

			return result;
		}

		void build_umbra_tome(const target::Bounds& scene, const unsigned int instance_count,
			const std::vector<target::Bounds>& instance_bounds, const unsigned int light_count, target::GfxWorld* world,
			utils::memory::allocator& allocator)
		{
			float minimum[3];
			float maximum[3];
			for (auto i = 0; i < 3; i++)
			{
				minimum[i] = scene.midPoint[i] - scene.halfSize[i];
				maximum[i] = scene.midPoint[i] + scene.halfSize[i];
			}

			for (const auto& bounds : instance_bounds)
			{
				for (auto i = 0; i < 3; i++)
				{
					minimum[i] = std::min(minimum[i], bounds.midPoint[i] - bounds.halfSize[i]);
					maximum[i] = std::max(maximum[i], bounds.midPoint[i] + bounds.halfSize[i]);
				}
			}

			const auto object_count = world->surfaces.count + instance_count + light_count;
			if (!object_count || object_count > 0x01000000)
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %u umbra objects cannot be represented", world->name, object_count);
				return;
			}

			std::vector<unsigned int> user_ids;
			user_ids.reserve(object_count);
			for (auto i = 0u; i < world->surfaces.count; i++)
			{
				user_ids.emplace_back(i);
			}

			for (auto i = 0u; i < instance_count; i++)
			{
				user_ids.emplace_back(0x10000000u + i);
			}

			for (auto i = 0u; i < light_count; i++)
			{
				user_ids.emplace_back(0x40000000u + i);
			}

			const auto index_width = umbra_index_width(object_count);
			const auto object_list = pack_umbra_object_list(object_count, index_width);

			target::UmbraTomeHeader header{};
			header.version = 0xD6000014;
			header.unitsPerMeter = 128.0f;
			header.treeDepth = 33;
			header.objectCount = object_count;
			header.objectCount2 = object_count;
			header.indexWidth = index_width;
			header.cellCount = 1;
			header.tileCount = 1;
			for (auto i = 0; i < 3; i++)
			{
				header.boundsMin[i] = minimum[i];
				header.boundsMax[i] = maximum[i];
				header.rootBoundsMin[i] = minimum[i];
				header.rootBoundsMax[i] = maximum[i];
			}

			std::vector<unsigned char> tome(sizeof(header));
			std::memcpy(tome.data(), &header, sizeof(header));

			const auto top_tree = allocate_tome(tome, sizeof(unsigned int));
			write_at(tome, top_tree, 3u);
			const auto top_map = allocate_tome(tome, sizeof(unsigned int));
			write_at(tome, top_map, 0u);
			write_at(tome, offsetof(target::UmbraTomeHeader, topTree), top_tree);
			write_at(tome, offsetof(target::UmbraTomeHeader, topMap), top_map);

			const auto object_bounds = allocate_tome(tome, static_cast<std::size_t>(object_count) * 24);
			const auto object_distances = allocate_tome(tome, static_cast<std::size_t>(object_count) * 32);
			for (auto object = 0u; object < object_count; object++)
			{
				const auto bounds_offset = object_bounds + static_cast<std::size_t>(object) * 24;
				std::memcpy(tome.data() + bounds_offset, minimum, sizeof(minimum));
				std::memcpy(tome.data() + bounds_offset + 12, maximum, sizeof(maximum));

				const auto distance_offset = object_distances + static_cast<std::size_t>(object) * 32;
				std::memcpy(tome.data() + distance_offset, minimum, sizeof(minimum));
				write_at(tome, distance_offset + 12, 0.0f);
				std::memcpy(tome.data() + distance_offset + 16, maximum, sizeof(maximum));
				write_at(tome, distance_offset + 28, std::numeric_limits<float>::infinity());
			}

			write_at(tome, offsetof(target::UmbraTomeHeader, objectBounds), object_bounds);
			write_at(tome, offsetof(target::UmbraTomeHeader, objectDistances), object_distances);

			const auto ids = allocate_tome(tome, static_cast<std::size_t>(object_count) * 4);
			std::memcpy(tome.data() + ids, user_ids.data(), user_ids.size() * sizeof(unsigned int));
			write_at(tome, offsetof(target::UmbraTomeHeader, userIDs), ids);

			const auto lists = allocate_tome(tome, object_list.size());
			std::memcpy(tome.data() + lists, object_list.data(), object_list.size());
			write_at(tome, offsetof(target::UmbraTomeHeader, objectLists), lists);

			const auto cell_starts = allocate_tome(tome, 8);
			write_at(tome, cell_starts, 0u);
			write_at(tome, cell_starts + 4, 1u);
			write_at(tome, offsetof(target::UmbraTomeHeader, cellStarts), cell_starts);

			const auto tile_lod_levels = allocate_tome(tome, 4);
			write_at(tome, tile_lod_levels, 1.0f);
			write_at(tome, offsetof(target::UmbraTomeHeader, tileLodLevels), tile_lod_levels);

			const auto tile_offsets = allocate_tome(tome, 4);
			write_at(tome, offsetof(target::UmbraTomeHeader, tileOffsets), tile_offsets);

			const auto tile_portal_expands = allocate_tome(tome, 4);
			write_at(tome, tile_portal_expands, 0.0f);
			write_at(tome, offsetof(target::UmbraTomeHeader, tilePortalExpands), tile_portal_expands);

			const auto tile_offset = allocate_tome(tome, sizeof(target::UmbraTomeTile));
			target::UmbraTomeTile tile{};
			std::memcpy(tile.boundsMin, minimum, sizeof(minimum));
			std::memcpy(tile.boundsMax, maximum, sizeof(maximum));
			tile.depth = 33;
			tile.unk0x34 = 1;

			auto largest_extent = 0.0;
			for (auto i = 0; i < 3; i++)
			{
				largest_extent = std::max(largest_extent, static_cast<double>(maximum[i]) - minimum[i]);
			}

			const auto required_scale = largest_extent / 65536.0;
			auto scale = static_cast<float>(required_scale);
			if (static_cast<double>(scale) < required_scale)
			{
				scale = std::nextafter(scale, std::numeric_limits<float>::infinity());
			}

			tile.scale = scale;
			tile.inverseScale = 1.0f / scale;

			const auto tile_tree = allocate_tome(tome, 4);
			write_at(tome, tile_tree, 3u);
			tile.tree = tile_tree - tile_offset;
			const auto tile_map = allocate_tome(tome, 8);
			write_at(tome, tile_map, 0u);
			tile.map = tile_map - tile_offset;
			const auto cell_indices = allocate_tome(tome, 8);
			write_at(tome, cell_indices, 0u);
			write_at(tome, cell_indices + 4, 0xFFFFFFFFu);
			tile.cellIndices = cell_indices - tile_offset;

			const auto cell_offset = allocate_tome(tome, sizeof(target::UmbraTomeCell));
			target::UmbraTomeCell cell{};
			cell.objectCount = object_count;
			cell.unk0x10 = 0xFFFFFFFFu;
			cell.unk0x14 = 0x80000000u;
			cell.unk0x1C = 0xFFFFu;
			cell.unk0x20 = 0xFFFFFFFFu;
			std::memcpy(tome.data() + cell_offset, &cell, sizeof(cell));
			tile.cell = cell_offset - tile_offset;

			tome.resize((tome.size() + 15) & ~std::size_t(15));
			tile.sizeAndFlags = (static_cast<unsigned int>(tome.size()) - tile_offset) << 8 | 3u;
			std::memcpy(tome.data() + tile_offset, &tile, sizeof(tile));
			write_at(tome, tile_offsets, tile_offset);

			const auto size = static_cast<unsigned int>(tome.size());
			write_at(tome, offsetof(target::UmbraTomeHeader, size), size);
			write_at(tome, offsetof(target::UmbraTomeHeader, checksum), crc32c(tome.data() + 8, size - 8));

			world->umbraTomeSize = size;
			world->umbraTomeData = make_array<char>(size, allocator);
			std::memcpy(world->umbraTomeData, tome.data(), size);
		}

		target::GfxWorldTransientZone* convert_transient_zone(const GfxWorld* asset, const world_geometry& geometry,
			const char* name, utils::memory::allocator& allocator)
		{
			auto* zone = allocator.allocate<target::GfxWorldTransientZone>();
			zone->name = name;
			zone->transientZoneIndex = 0;

			auto& verts = zone->drawVerts;
			verts.posDataSize = static_cast<unsigned int>(geometry.positions.size());
			verts.posData = allocator.allocate_array<char>(geometry.positions.size());
			std::memcpy(verts.posData, geometry.positions.data(), geometry.positions.size());
			verts.auxDataSize = static_cast<unsigned int>(geometry.aux.size());
			verts.auxData = allocator.allocate_array<char>(geometry.aux.size());
			std::memcpy(verts.auxData, geometry.aux.data(), geometry.aux.size());
			verts.indexCount = static_cast<unsigned int>(geometry.indices.size());
			verts.indices = copy_array(geometry.indices.data(), geometry.indices.size(), allocator);

			convert_cell_trees(asset, zone, allocator);
			return zone;
		}

		const char* transient_zone_name(const GfxWorld* asset, utils::memory::allocator& allocator)
		{
			const auto name = asset->baseName ? asset->baseName : asset->name;
			return allocator.duplicate_string(name);
		}
	}

	void dump(GfxWorld* asset)
	{
		if (!asset->draw.transientZoneCount || !asset->draw.transientZones[0])
		{
			ZONETOOL_ERROR("gfxworld \"%s\" has no loaded transient zones", asset->name);
			return;
		}

		utils::memory::allocator allocator;

		const auto surface_count = asset->surfaceCount;
		auto geometry = build_geometry(asset, surface_count);

		auto* world = allocator.allocate<target::GfxWorld>();
		world->name = asset->name;
		world->baseName = asset->baseName ? asset->baseName : asset->name;
		world->bspVersion = target::BSP_VERSION;
		world->lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
		world->primaryLightCount = asset->primaryLightCount;
		world->firstMutablePrimaryLight = asset->primaryLightCount;
		world->firstStaticScriptablesPrimaryLight = asset->primaryLightCount;
		world->firstStaticScriptablePrimaryLight = asset->primaryLightCount;
		world->firstMovingScriptablePrimaryLight = asset->primaryLightCount - asset->movingScriptablePrimaryLightCount;
		world->movingScriptablePrimaryLightCount = asset->movingScriptablePrimaryLightCount;

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

		const auto* zone_name = transient_zone_name(asset, allocator);
		auto* zone = convert_transient_zone(asset, geometry, zone_name, allocator);

		convert_cells(asset, world, allocator);
		convert_surfaces(asset, geometry, world, allocator);

		std::vector<target::Bounds> instance_bounds;
		convert_static_models(asset, world, instance_bounds, allocator);
		convert_draw(asset, zone, world, allocator);
		convert_sun(asset, world, allocator);
		world->primaryLights = make_array<target::GfxLight>(world->primaryLightCount, allocator);
		convert_lights(asset, world, allocator);
		convert_models(asset, world, allocator);
		convert_voxel_trees(asset, world, allocator);
		convert_heightfields(asset, world, allocator);
		convert_dpvs(asset, world, allocator);
		convert_dpvs_dynamic(asset, world, allocator);
		convert_runtime_bits(world, allocator);
		build_umbra_tome(world->bounds, world->smodels.instanceCount, instance_bounds, world->primaryLightCount, world,
			allocator);

		target::gfx_world_transient_zone::dump(zone);
		add_csv_row("gfx_map_trzone", zone_name);
		target::gfx_world::dump(world);
		add_csv_row("gfx_map", world->name);
	}
}
