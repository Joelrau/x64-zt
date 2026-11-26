#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "gfxworld.hpp"
#include "gfximage.hpp"
#include "material.hpp"
#include "dynentitylist.hpp"
#include "lights.hpp"
#include "light_grid.hpp"

#include <iw8/assets/gfxworld.hpp>
#include <iw8/assets/gfxworldtransientzone.hpp>

#include <formats/umbra_tome.hpp>
#include <shared.hpp>

namespace zonetool::t9::converter::iw8::gfxworld
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename T>
		T* make_named(const std::string& name, utils::memory::allocator& allocator)
		{
			auto* asset = allocator.allocate<T>();
			asset->name = allocator.duplicate_string(name);
			return asset;
		}

		template <typename T>
		T* make_array(const std::size_t count, utils::memory::allocator& allocator)
		{
			return count ? allocator.allocate_array<T>(count) : nullptr;
		}

		template <typename T>
		T* copy_vector(const std::vector<T>& values, utils::memory::allocator& allocator)
		{
			auto* result = make_array<T>(values.size(), allocator);
			std::copy(values.begin(), values.end(), result);
			return result;
		}

		template <typename T>
		unsigned int append(std::vector<char>& data, const T* values, const std::size_t count)
		{
			const auto offset = static_cast<unsigned int>(data.size());
			const auto* bytes = reinterpret_cast<const char*>(values);
			data.insert(data.end(), bytes, bytes + count * sizeof(T));
			return offset;
		}

		void expand_bounds(float* mins, float* maxs, const float* point)
		{
			for (auto i = 0; i < 3; i++)
			{
				mins[i] = std::min(mins[i], point[i]);
				maxs[i] = std::max(maxs[i], point[i]);
			}
		}

		placement_transform make_transform(const StaticModelPlacement& placement)
		{
			auto x = placement.quat[0];
			auto y = placement.quat[1];
			auto z = placement.quat[2];
			auto w = placement.quat[3];
			const auto length = std::sqrt(x * x + y * y + z * z + w * w);
			if (length > 0.0f)
			{
				x /= length;
				y /= length;
				z /= length;
				w /= length;
			}
			else
			{
				w = 1.0f;
			}

			const float rotation[3][3] =
			{
				{1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y - z * w), 2.0f * (x * z + y * w)},
				{2.0f * (x * y + z * w), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z - x * w)},
				{2.0f * (x * z - y * w), 2.0f * (y * z + x * w), 1.0f - 2.0f * (x * x + y * y)},
			};

			placement_transform transform{};
			for (auto axis = 0; axis < 3; axis++)
			{
				for (auto i = 0; i < 3; i++)
				{
					transform.axis[axis][i] = rotation[i][axis];
				}

				transform.origin[axis] = placement.origin[axis];
			}

			transform.scale = placement.scale > 0.0f ? placement.scale : 1.0f;
			return transform;
		}

		target::Bounds transform_bounds(const target::Bounds& bounds, const placement_transform& transform)
		{
			target::Bounds result{};
			for (auto i = 0; i < 3; i++)
			{
				result.midPoint[i] = transform.origin[i];
				for (auto axis = 0; axis < 3; axis++)
				{
					result.midPoint[i] += transform.scale * transform.axis[axis][i] * bounds.midPoint[axis];
					result.halfSize[i] += transform.scale * std::abs(transform.axis[axis][i]) * bounds.halfSize[axis];
				}
			}

			return result;
		}

		target::Bounds merge_bounds(const target::Bounds& a, const target::Bounds& b)
		{
			target::Bounds result{};
			for (auto i = 0; i < 3; i++)
			{
				const auto min = std::min(a.midPoint[i] - a.halfSize[i], b.midPoint[i] - b.halfSize[i]);
				const auto max = std::max(a.midPoint[i] + a.halfSize[i], b.midPoint[i] + b.halfSize[i]);
				result.midPoint[i] = (min + max) * 0.5f;
				result.halfSize[i] = (max - min) * 0.5f;
			}

			return result;
		}

		unsigned short pack_quaternion_component(const float value)
		{
			return static_cast<unsigned short>(std::lround(std::clamp((value + 1.0f) * 0.5f, 0.0f, 1.0f) * 65535.0f));
		}

		target::GfxSModelInstanceData convert_instance(const placed_model& model, const StaticModelPlacement& placement)
		{
			target::GfxSModelInstanceData instance{};
			for (auto i = 0; i < 3; i++)
			{
				instance.translation[i] = static_cast<int>(std::lround(model.transform.origin[i] * 4096.0f));
			}

			const auto length = std::sqrt(placement.quat[0] * placement.quat[0] + placement.quat[1] * placement.quat[1]
				+ placement.quat[2] * placement.quat[2] + placement.quat[3] * placement.quat[3]);
			for (auto i = 0; i < 4; i++)
			{
				const auto value = length > 0.0f ? placement.quat[i] / length : (i == 3 ? 1.0f : 0.0f);
				instance.orientation[i] = pack_quaternion_component(value);
			}

			instance.scale = model.transform.scale;
			return instance;
		}

		void build_static_models(const std::vector<placed_model>& models, const std::vector<StaticModelPlacement>& placements,
			target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto instance_count = static_cast<unsigned int>(models.size());

			std::vector<const xmodel::static_model_info*> unique;
			std::unordered_map<const xmodel::static_model_info*, unsigned int> model_index;
			std::vector<unsigned int> instance_model(instance_count);
			for (auto i = 0u; i < instance_count; i++)
			{
				const auto [found, added] = model_index.try_emplace(models[i].model, static_cast<unsigned int>(unique.size()));
				if (added)
				{
					unique.emplace_back(models[i].model);
				}

				instance_model[i] = found->second;
			}

			std::vector<unsigned int> first_surface(unique.size());
			std::vector<unsigned int> surface_count(unique.size());
			std::vector<unsigned int> lod_count(unique.size());
			auto total_surfaces = 0u;
			for (auto m = 0u; m < unique.size(); m++)
			{
				first_surface[m] = total_surfaces;
				for (const auto& lod : unique[m]->lods)
				{
					if (surface_count[m] + lod.surfaces.size() > 255) // GfxStaticModelExpansion::numSurfs is a byte
					{
						break;
					}

					surface_count[m] += static_cast<unsigned int>(lod.surfaces.size());
					lod_count[m]++;
				}

				total_surfaces += surface_count[m];
			}

			if (total_surfaces > 65535)
			{
				ZONETOOL_ERROR("gfxworld \"%s\" has %u static model surfaces, IW8 supports 65535", world->name, total_surfaces);
				return;
			}

			auto& smodels = world->smodels;
			smodels.modelCount = static_cast<unsigned int>(unique.size());
			smodels.collectionCount = instance_count;
			smodels.instanceCount = instance_count;
			smodels.surfaceCount = total_surfaces;

			smodels.models = make_array<target::GfxStaticModel>(smodels.modelCount, allocator);
			smodels.surfaces = make_array<target::GfxStaticModelSurface>(total_surfaces, allocator);
			smodels.collections = make_array<target::GfxStaticModelCollection>(instance_count, allocator);
			smodels.instanceFlags = make_array<unsigned char>(instance_count, allocator);
			smodels.collectionBounds = make_array<target::Bounds>(instance_count, allocator);
			smodels.modelStaticIndirection = make_array<target::GfxModelStaticIndirection>(instance_count, allocator);
			smodels.smodelSurfData = make_array<target::GfxBaseSModelSurfData>(total_surfaces, allocator);
			smodels.smodelInstanceData = make_array<target::GfxSModelInstanceData>(instance_count, allocator);
			smodels.smodelExpansionData = make_array<target::GfxStaticModelExpansion>(smodels.modelCount, allocator);
			smodels.smodelSurfMatIndirection = make_array<target::GfxSModelSurfMatIndirection>(total_surfaces, allocator);
			smodels.smodelSurfUGBIndirection = make_array<unsigned int>(total_surfaces, allocator);

			std::vector<std::string_view> materials;
			std::unordered_map<std::string_view, unsigned short> material_index;
			auto surface = 0u;
			for (auto m = 0u; m < unique.size(); m++)
			{
				const auto* info = unique[m];
				auto& model = smodels.models[m];
				model.model = make_named<target::XModel>(info->name, allocator);
				model.flags = target::STATIC_MODEL_FLAG_LIGHTPROBE_LIGHTING;
				model.firstStaticModelSurfaceIndex = static_cast<unsigned short>(first_surface[m]);

				auto& expansion = smodels.smodelExpansionData[m];
				expansion.firstStaticModelSurfaceIndex = static_cast<unsigned short>(first_surface[m]);
				expansion.numSurfs = static_cast<unsigned char>(surface_count[m]);
				expansion.numLods = static_cast<unsigned char>(lod_count[m]);
				expansion.modelBounds = info->bounds;

				auto lod_surface = 0u;
				for (auto lod = 0u; lod < lod_count[m]; lod++)
				{
					expansion.lodInfo[lod].surfOffset = static_cast<unsigned char>(lod_surface);
					expansion.lodInfo[lod].surfCount = static_cast<unsigned char>(info->lods[lod].surfaces.size());
					expansion.lodDist[lod] = lod + 1 < lod_count[m] ? info->lods[lod].dist : 1000000.0f;
					lod_surface += expansion.lodInfo[lod].surfCount;
				}

				for (auto lod = 0u; lod < lod_count[m]; lod++)
				for (auto s = 0u; s < info->lods[lod].surfaces.size(); s++)
				{
					auto& surface_entry = smodels.surfaces[surface];
					surface_entry.smodelIndex = static_cast<unsigned short>(m);
					surface_entry.lodIndex = static_cast<unsigned char>(lod);
					surface_entry.surfIndex = static_cast<unsigned char>(s);

					const auto& source = info->lods[lod].surfaces[s];
					auto found = material_index.find(source.material);
					if (found == material_index.end())
					{
						found = material_index.emplace(source.material, static_cast<unsigned short>(materials.size())).first;
						materials.emplace_back(source.material);
					}

					auto& indirection = smodels.smodelSurfMatIndirection[surface];
					indirection.smodelMaterialIndex = found->second;
					indirection.cullType = static_cast<target::GfxSModelSurfCullType>(target::SMODEL_SURF_CULL_OPAQUE & ~target::SMODEL_SURF_CULL_ENABLED); // donor techset
					indirection.emitType = static_cast<target::GfxSModelSurfEmitType>(target::SMODEL_SURF_EMIT_NONE
						| target::SMODEL_SURF_EMIT_MAIN_VIEW | target::SMODEL_SURF_EMIT_SHADOW);

					const auto& bounds = source.bounds;
					auto& data = smodels.smodelSurfData[surface];
					data.boundingBoxData[0] = bounds.midPoint[0];
					data.boundingBoxData[1] = bounds.midPoint[1];
					data.boundingBoxData[2] = bounds.midPoint[2];
					data.boundingBoxData[3] = std::max({bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2]});
					std::memcpy(data.boundingBoxHalfSize, bounds.halfSize, sizeof(data.boundingBoxHalfSize));
					data.drawDistance = expansion.lodDist[lod_count[m] - 1];

					surface++;
				}
			}

			if (materials.size() > 65535)
			{
				ZONETOOL_ERROR("gfxworld \"%s\" has more than 65535 static model materials", world->name);
				return;
			}

			smodels.materialCount = static_cast<unsigned int>(materials.size());
			smodels.materials = make_array<target::Material*>(materials.size(), allocator);
			for (auto i = 0u; i < materials.size(); i++)
			{
				smodels.materials[i] = make_named<target::Material>(std::string(materials[i]), allocator);
			}

			const auto shadow_flags = static_cast<unsigned char>((1u << std::min(world->lastSunPrimaryLightIndex, 5u)) - 1);
			for (auto i = 0u; i < instance_count; i++)
			{
				auto& collection = smodels.collections[i];
				collection.firstInstance = i;
				collection.instanceCount = 1;
				collection.smodelIndex = static_cast<unsigned short>(instance_model[i]);
				collection.flags = shadow_flags;

				smodels.collectionBounds[i] = models[i].bounds;
				smodels.modelStaticIndirection[i].lightingInstanceIndirection = i;
				smodels.smodelInstanceData[i] = convert_instance(models[i], placements[i]);
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

			ZONETOOL_INFO("gfxworld \"%s\": %u static model instances, %u models, %u surfaces, %zu materials", world->name, instance_count,
				smodels.modelCount, total_surfaces, materials.size());
		}

		const GfxWorld* find_world(const Districts* asset)
		{
			if (!asset || !asset->world)
			{
				return nullptr;
			}

			const auto* loaded = static_cast<const GfxWorld*>(db_find_asset(ASSET_TYPE_GFX_MAP, xhash_name(asset->world->name)));
			return loaded ? loaded : asset->world;
		}

		const Material* resolve_material(const Material* asset)
		{
			if (!asset || asset->techniqueSet || asset->textureCount)
			{
				return asset;
			}

			const auto* loaded = static_cast<const Material*>(db_find_asset(ASSET_TYPE_MATERIAL, xhash_name(asset->name)));
			return loaded ? loaded : asset;
		}

		std::map<std::uint32_t, std::uint32_t> get_vertex_blocks(const GfxWorld* asset)
		{
			std::map<std::uint32_t, std::uint32_t> blocks;
			for (auto i = 0; i < asset->surfaceCount; i++)
			{
				blocks.emplace(asset->draw.surfaces[i].baseVertex, 0);
			}

			auto end = asset->vertexData.vertexCount;
			for (auto block = blocks.rbegin(); block != blocks.rend(); ++block)
			{
				block->second = end > block->first ? end - block->first : 0;
				end = block->first;
			}

			return blocks;
		}

		std::optional<unsigned int> get_used_vertex_count(const GfxWorldVertexData& data, const GfxWorldSurface& surface, const std::uint32_t block_size)
		{
			const auto index_count = surface.triCount * 3u;
			if (!index_count || surface.baseIndex + index_count > data.indexCount)
			{
				return {};
			}

			auto used = 0u;
			for (auto i = 0u; i < index_count; i++)
			{
				const auto index = data.indices[surface.baseIndex + i];
				if (index >= block_size)
				{
					return {};
				}

				used = std::max(used, index + 1u);
			}

			if (used > 0xFFFF)
			{
				return {};
			}

			return used;
		}

		target::GfxWorldSurfData append_vertex_block(const GfxWorldVertexData& data, const std::uint32_t first_vertex, const std::uint32_t vertex_count,
			world_geometry& geometry)
		{
			const auto* vertices = data.vertices + first_vertex;

			target::GfxWorldSurfData result{};
			result.layerCount = 1;
			result.xyzOffset = append(geometry.positions, data.positions + first_vertex, vertex_count);

			result.tangentFrameOffset = static_cast<unsigned int>(geometry.aux.size());
			for (auto i = 0u; i < vertex_count; i++)
			{
				const auto& vertex = vertices[i];
				const auto frame = xmodel::pack_tangent_frame(xmodel::unpack_unit_vec(vertex.normal), xmodel::unpack_unit_vec(vertex.tangent),
					(vertex.tangent >> 30) == 3);
				append(geometry.aux, &frame, 1);
			}

			result.texCoordOffset = static_cast<unsigned int>(geometry.aux.size());
			for (auto i = 0u; i < vertex_count; i++)
			{
				append(geometry.aux, vertices[i].texCoord, 2);
			}

			result.colorOffset = static_cast<unsigned int>(geometry.aux.size());
			for (auto i = 0u; i < vertex_count; i++)
			{
				append(geometry.aux, &vertices[i].color, 1);
			}

			return result;
		}

		bool is_hidden(const Material* asset)
		{
			return asset && asset->drawList == DRAW_LIST_NONE;
		}

		void expand_surface_bounds(const GfxWorld* asset, float* mins, float* maxs)
		{
			const auto& data = asset->vertexData;
			if (!asset->draw.surfaces || !data.positions || !data.indices)
			{
				return;
			}

			for (auto i = 0; i < asset->surfaceCount; i++)
			{
				const auto& surface = asset->draw.surfaces[i];
				if (is_hidden(resolve_material(surface.material)))
				{
					continue;
				}

				for (auto j = 0u; j < surface.triCount * 3u && surface.baseIndex + j < data.indexCount; j++)
				{
					const auto vertex = surface.baseVertex + data.indices[surface.baseIndex + j];
					if (vertex < data.vertexCount)
					{
						expand_bounds(mins, maxs, data.positions[vertex]);
					}
				}
			}
		}

		target::GfxSurfaceBounds get_surface_bounds(const float (*positions)[3], const std::uint16_t* indices, const unsigned int index_count)
		{
			float mins[3]{FLT_MAX, FLT_MAX, FLT_MAX};
			float maxs[3]{-FLT_MAX, -FLT_MAX, -FLT_MAX};
			for (auto i = 0u; i < index_count; i++)
			{
				expand_bounds(mins, maxs, positions[indices[i]]);
			}

			target::GfxSurfaceBounds result{};
			compute(&result.bounds, mins, maxs);
			return result;
		}

		world_geometry build_world_geometry(const GfxWorld* asset, utils::memory::allocator& allocator)
		{
			world_geometry geometry{};
			geometry.positions.resize(4);
			geometry.aux.resize(4);
			if (!asset)
			{
				return geometry;
			}

			const auto& data = asset->vertexData;
			if (!asset->draw.surfaces || !data.positions || !data.vertices || !data.indices)
			{
				ZONETOOL_WARNING("gfxworld \"%s\" has no world surfaces", xhash_name(asset->name).data());
				return geometry;
			}

			const auto blocks = get_vertex_blocks(asset);
			std::vector<unsigned int> order;
			std::vector<unsigned int> decals;
			std::vector<unsigned int> trans;
			std::vector<unsigned int> vertex_counts(asset->surfaceCount);
			auto hidden = 0u;
			auto invalid = 0u;
			for (auto i = 0u; i < static_cast<unsigned int>(asset->surfaceCount); i++)
			{
				const auto& surface = asset->draw.surfaces[i];
				const auto* material = resolve_material(surface.material);
				if (is_hidden(material))
				{
					hidden++;
					continue;
				}

				const auto vertex_count = get_used_vertex_count(data, surface, blocks.at(surface.baseVertex));
				if (!material || !vertex_count)
				{
					invalid++;
					continue;
				}

				vertex_counts[i] = *vertex_count;
				switch (material::get_world_surface_list(material))
				{
				case material::world_surface_list::decal:
					decals.emplace_back(i);
					break;
				case material::world_surface_list::trans:
					trans.emplace_back(i);
					break;
				default:
					order.emplace_back(i);
					break;
				}
			}

			geometry.opaque_count = static_cast<unsigned int>(order.size());
			order.insert(order.end(), decals.begin(), decals.end());
			geometry.decal_end = static_cast<unsigned int>(order.size());
			order.insert(order.end(), trans.begin(), trans.end());

			std::unordered_map<std::uint32_t, unsigned int> block_index;
			std::unordered_map<const Material*, target::Material*> materials;
			for (auto i = 0u; i < order.size(); i++)
			{
				const auto& surface = asset->draw.surfaces[order[i]];
				const auto [block, added] = block_index.try_emplace(surface.baseVertex, static_cast<unsigned int>(geometry.surf_data.size()));
				if (added)
				{
					geometry.surf_data.emplace_back(append_vertex_block(data, surface.baseVertex, blocks.at(surface.baseVertex), geometry));
				}

				const auto* material = resolve_material(surface.material);
				auto& converted_material = materials[material];
				if (!converted_material)
				{
					converted_material = make_named<target::Material>(material::dump_world(material), allocator);
				}

				const auto* indices = data.indices + surface.baseIndex;
				const auto index_count = surface.triCount * 3u;

				auto& converted = geometry.surfaces.emplace_back();
				converted.tris.posOffset = geometry.surf_data[block->second].xyzOffset;
				converted.tris.vertexCount = static_cast<unsigned short>(vertex_counts[order[i]]);
				converted.tris.triCount = surface.triCount;
				converted.tris.baseIndex = static_cast<unsigned int>(geometry.indices.size());
				converted.material = converted_material;
				converted.surfDataIndex = block->second;
				converted.lightmapIndex = 510;
				converted.flags = static_cast<unsigned char>(i < geometry.opaque_count ? target::GFX_SURFACE_CASTS_SUN_SHADOW : 0);

				geometry.indices.insert(geometry.indices.end(), indices, indices + index_count);
				geometry.surface_bounds.emplace_back(get_surface_bounds(data.positions + surface.baseVertex, indices, index_count));
			}

			ZONETOOL_INFO("gfxworld \"%s\": %zu surfaces (%u opaque), %zu vertex blocks, %zu indices, %zu materials, %u hidden, %u invalid",
				xhash_name(asset->name).data(), geometry.surfaces.size(), geometry.opaque_count, geometry.surf_data.size(), geometry.indices.size(),
				materials.size(), hidden, invalid);
			return geometry;
		}

		void build_surfaces(const world_geometry& geometry, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto count = static_cast<unsigned int>(geometry.surfaces.size());
			auto& surfaces = world->surfaces;
			surfaces.count = count;
			surfaces.surfDataCount = static_cast<unsigned int>(geometry.surf_data.size());
			surfaces.litOpaqueSurfsBegin = 0;
			surfaces.litOpaqueSurfsEnd = geometry.opaque_count;
			surfaces.litDecalSurfsBegin = geometry.opaque_count;
			surfaces.litDecalSurfsEnd = geometry.decal_end;
			surfaces.litTransSurfsBegin = geometry.decal_end;
			surfaces.litTransSurfsEnd = count;
			surfaces.emissiveSurfsBegin = count;
			surfaces.emissiveSurfsEnd = count;

			surfaces.surfaces = copy_vector(geometry.surfaces, allocator);
			surfaces.surfaceBounds = copy_vector(geometry.surface_bounds, allocator);
			surfaces.surfaceMaterials = make_array<target::GfxDrawSurf>(count, allocator);
			surfaces.surfData = copy_vector(geometry.surf_data, allocator);
		}

		void build_cells(const target::Bounds& bounds, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& planes = world->dpvsPlanes;
			planes.cellCount = 1;
			planes.planeCount = 0;
			planes.nodeCount = 1;
			planes.nodes = allocator.allocate_array<unsigned short>(1);
			planes.nodes[0] = 1; // leaf of cell 0
			planes.sceneEntCellBits = allocator.allocate_array<unsigned int>(std::size_t(1) << 9);

			world->cellTransientInfos = allocator.allocate_array<target::GfxCellTransientInfo>(1);
			world->cells = allocator.allocate_array<target::GfxCell>(1);
			world->cells[0].bounds = bounds;
		}

		void build_cell_tree(const target::Bounds& bounds, const std::size_t surface_count, const std::size_t instance_count,
			target::GfxWorldTransientZone* zone, utils::memory::allocator& allocator)
		{
			auto& cells = zone->drawCells;
			cells.cellCount = 1;
			cells.aabbTreeCounts = allocator.allocate_array<target::GfxCellTreeCount>(1);
			cells.aabbTrees = allocator.allocate_array<target::GfxCellTree>(1);
			cells.aabbTreeCounts[0].aabbTreeCount = 1;
			cells.aabbTrees[0].aabbTree = allocator.allocate_array<target::GfxAabbTree>(1);

			auto& tree = cells.aabbTrees[0].aabbTree[0];
			tree.bounds = bounds;
			tree.startSurfIndex = 0;
			tree.surfaceCount = static_cast<unsigned int>(surface_count);
			tree.smodelIndexCount = static_cast<unsigned short>(instance_count);
			tree.smodelIndexes = make_array<unsigned short>(instance_count, allocator);
			for (auto i = 0u; i < instance_count; i++)
			{
				tree.smodelIndexes[i] = static_cast<unsigned short>(i);
			}
		}

		target::GfxWorldTransientZone* build_transient_zone(const char* name, const world_geometry& geometry, utils::memory::allocator& allocator)
		{
			auto* zone = allocator.allocate<target::GfxWorldTransientZone>();
			zone->name = name;
			zone->transientZoneIndex = 0;

			auto& verts = zone->drawVerts;
			verts.posDataSize = static_cast<unsigned int>(geometry.positions.size());
			verts.posData = copy_vector(geometry.positions, allocator);
			verts.auxDataSize = static_cast<unsigned int>(geometry.aux.size());
			verts.auxData = copy_vector(geometry.aux, allocator);
			verts.indexCount = static_cast<unsigned int>(geometry.indices.size());
			verts.indices = copy_vector(geometry.indices, allocator);
			return zone;
		}

		void build_draw(const target::Bounds& bounds, target::GfxWorldTransientZone* zone, target::GfxWorld* world)
		{
			auto& draw = world->draw;
			draw.lightGridType = zone->gpuLightGrid.gpuLightGrid ? target::GFX_LIGHT_GRID_TYPE_GPU : target::GFX_LIGHT_GRID_TYPE_NONE;
			draw.lightmapType = target::GFX_LIGHTMAP_TYPE_REGULAR_UNCOMPRESSED;
			draw.transientZoneCount = 1;
			draw.transientZones[0] = zone;
			draw.bakedLightScale = 1.0f;
			draw.iesLookupTexture = gfximage::dump_ies_lookup();

			world->materialLod0SizeThreshold = 0.5f;
			world->bounds = bounds;
		}

		void build_reflection_probe(const light_grid::sh_probe& ambient, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto* image = gfximage::dump_uniform_reflection_probe(utils::string::va("%s_reflection_probes", world->name));
			if (!image)
			{
				return;
			}

			auto& probes = world->draw.reflectionProbeData;
			probes.reflectionProbeArrayImage_octahedron = make_named<target::GfxImage>(image->name, allocator);
			probes.reflectionProbeCount = 1;
			probes.reflectionProbes = allocator.allocate<target::GfxReflectionProbe>();
			probes.reflectionProbeInstanceCount = 1;
			probes.reflectionProbeInstances = allocator.allocate<target::GfxReflectionProbeInstance>();
			probes.reflectionProbeSH = allocator.allocate_array<target::GfxSH3Vec>(4);

			auto& probe = probes.reflectionProbes[0];
			std::memcpy(probe.origin, world->bounds.midPoint, sizeof(probe.origin));
			probe.probeInstanceCount = 1;
			probe.probeInstances = allocator.allocate<unsigned short>();
			probe.probeRelightingIndex = 0xFFFF;

			auto& instance = probes.reflectionProbeInstances[0];
			std::memcpy(instance.probePosition, world->bounds.midPoint, sizeof(instance.probePosition));
			instance.flags = target::REFLECTION_PROBE_INSTANCE_NO_PARALLAX;
			instance.probeRotation[3] = 1.0f;
			std::memcpy(instance.volumeObb.center, world->bounds.midPoint, sizeof(instance.volumeObb.center));
			instance.volumeObb.xAxis[0] = 1.0f;
			instance.volumeObb.yAxis[1] = 1.0f;
			instance.volumeObb.zAxis[2] = 1.0f;
			std::memcpy(instance.volumeObb.halfSize, world->bounds.halfSize, sizeof(instance.volumeObb.halfSize));
			instance.priority = std::numeric_limits<float>::lowest();

			for (auto k = 0u; k < 9; k++)
			{
				probes.reflectionProbeSH[0].coeffs[k] = 0.2126f * ambient[k] + 0.7152f * ambient[9 + k] + 0.0722f * ambient[18 + k];
				for (auto channel = 0u; channel < 3; channel++)
				{
					probes.reflectionProbeSH[channel + 1].coeffs[k] = ambient[channel * 9 + k];
				}
			}

			ZONETOOL_WARNING("gfxworld \"%s\": T9 reflection probes are not converted, the map gets 1 uniform probe from the average light grid probe",
				world->name);
		}

		void build_lights(const Districts* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto primary_lights = lights::convert_primary_lights(asset);
			const auto light_count = static_cast<unsigned int>(primary_lights.size());
			world->primaryLightCount = light_count;
			world->lastSunPrimaryLightIndex = 1;
			world->firstMutablePrimaryLight = light_count;
			world->firstStaticScriptablesPrimaryLight = light_count;
			world->firstStaticScriptablePrimaryLight = light_count;
			world->firstMovingScriptablePrimaryLight = light_count;
			world->movingScriptablePrimaryLightCount = 0;

			world->primaryLights = make_array<target::GfxLight>(light_count, allocator);
			world->lightViewFrustums = make_array<target::GfxLightViewFrustum>(light_count, allocator);
			world->shadowGeomOptimized = make_array<target::GfxShadowGeometry>(light_count, allocator);
			lights::build_frustum_lights(primary_lights, world->frustumLights, allocator);
		}

		void build_models(const target::Bounds& bounds, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->modelCount = 1;
			world->models = allocator.allocate_array<target::GfxBrushModel>(1);
			auto& model = world->models[0];
			model.writable.bounds = bounds;
			model.bounds = bounds;
			model.radius = std::sqrt(bounds.halfSize[0] * bounds.halfSize[0] + bounds.halfSize[1] * bounds.halfSize[1]
				+ bounds.halfSize[2] * bounds.halfSize[2]);
			model.startSurfIndex = 0;
			model.surfaceCount = world->surfaces.count;
			model.mdaoVolumeIndex = -1;
		}

		void build_dpvs(target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto instance_count = world->smodels.instanceCount;

			auto& dpvs = world->dpvs;
			dpvs.smodelVisDataCount = (instance_count + 31) >> 5;
			dpvs.surfaceVisDataCount = (world->surfaces.count + 31) >> 5;
			dpvs.primaryLightVisDataCount = (world->primaryLightCount + 31) >> 5;
			dpvs.reflectionProbeVisDataCount = (world->draw.reflectionProbeData.reflectionProbeInstanceCount + 31) >> 5;
			dpvs.reflectionProbeVisData = make_array<unsigned int>(dpvs.reflectionProbeVisDataCount, allocator);

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
				dpvs.sortedSurfIndex[i] = i < world->surfaces.count ? i : 0;
			}

			dpvs.surfaceCastsSunShadow = make_array<unsigned int>(dpvs.surfaceVisDataCount, allocator);
		}

		void build_runtime_bits(target::GfxWorld* world, utils::memory::allocator& allocator)
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

			world->primaryLightMotionDetectBits = make_array<unsigned int>(world->primaryLightMotionDetectBitsEntries, allocator);
			world->entityMotionBits = make_array<unsigned int>(world->entityMotionBitsEntries, allocator);
			world->primaryLightEntityShadowVis = make_array<unsigned int>(world->numPrimaryLightEntityShadowVisEntries, allocator);
		}

		void build_umbra_tome(const target::Bounds& scene, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			umbra_tome::object scene_object{};
			for (auto i = 0; i < 3; i++)
			{
				scene_object.minimum[i] = scene.midPoint[i] - scene.halfSize[i];
				scene_object.maximum[i] = scene.midPoint[i] + scene.halfSize[i];
			}

			const auto surface_count = world->surfaces.count;
			const auto instance_count = world->smodels.instanceCount;
			const auto light_count = world->primaryLightCount;
			std::vector<umbra_tome::object> objects(surface_count + instance_count + light_count, scene_object);
			auto* object = objects.data();
			for (auto i = 0u; i < surface_count; i++)
			{
				(object++)->user_id = i;
			}

			for (auto i = 0u; i < instance_count; i++)
			{
				(object++)->user_id = 0x10000000u + i;
			}

			for (auto i = 0u; i < light_count; i++)
			{
				(object++)->user_id = 0x40000000u + i;
			}

			const auto tome = umbra_tome::build_single_cell(scene_object.minimum, scene_object.maximum, objects);
			if (tome.empty())
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %zu umbra objects cannot be represented", world->name, objects.size());
				return;
			}

			world->umbraTomeSize = static_cast<unsigned int>(tome.size());
			world->umbraTomeData = allocator.allocate_array<char>(world->umbraTomeSize);
			std::memcpy(world->umbraTomeData, tome.data(), tome.size());
		}
	}

	std::vector<placed_model> get_placed_models(const Districts* asset, std::vector<StaticModelPlacement>& placements)
	{
		std::vector<placed_model> models;
		std::unordered_map<std::uint64_t, const xmodel::static_model_info*> infos;
		auto missing = 0u;
		auto deformed = 0u;
		for (const auto& instance : read_static_models(asset))
		{
			if (instance.deformation_index != 0xFFFFFFFF)
			{
				deformed++;
				continue;
			}

			const auto [found, added] = infos.try_emplace(instance.model.value, nullptr);
			if (added)
			{
				auto* model = static_cast<XModel*>(db_find_asset(ASSET_TYPE_XMODEL, xhash_name(instance.model)));
				found->second = model ? xmodel::dump_static(model) : nullptr;
			}

			if (!found->second)
			{
				missing++;
				continue;
			}

			if (models.size() >= 65535)
			{
				ZONETOOL_WARNING("districts \"%s\": IW8 supports 65535 static models, the rest are skipped", xhash_name(asset->name).data());
				break;
			}

			placed_model placed{};
			placed.model = found->second;
			placed.transform = make_transform(instance.placement);
			placed.bounds = transform_bounds(found->second->bounds, placed.transform);
			models.emplace_back(placed);
			placements.emplace_back(instance.placement);
		}

		if (missing)
		{
			ZONETOOL_WARNING("districts \"%s\": %u static models have no converted xmodel", xhash_name(asset->name).data(), missing);
		}

		if (deformed)
		{
			ZONETOOL_WARNING("districts \"%s\": %u deformed static models are skipped, their deformation data is not known", xhash_name(asset->name).data(), deformed);
		}

		return models;
	}

	std::array<float, 3> transform_point(const placement_transform& transform, const std::array<float, 3>& point)
	{
		std::array<float, 3> result{transform.origin[0], transform.origin[1], transform.origin[2]};
		for (auto i = 0; i < 3; i++)
		{
			for (auto axis = 0; axis < 3; axis++)
			{
				result[i] += transform.scale * transform.axis[axis][i] * point[axis];
			}
		}

		return result;
	}

	zonetool::iw8::Bounds get_world_bounds(const Districts* asset, const std::vector<placed_model>& models)
	{
		float mins[3]{FLT_MAX, FLT_MAX, FLT_MAX};
		float maxs[3]{-FLT_MAX, -FLT_MAX, -FLT_MAX};
		for (const auto& model : models)
		{
			for (auto i = 0; i < 3; i++)
			{
				mins[i] = std::min(mins[i], model.bounds.midPoint[i] - model.bounds.halfSize[i]);
				maxs[i] = std::max(maxs[i], model.bounds.midPoint[i] + model.bounds.halfSize[i]);
			}
		}

		if (const auto* world = find_world(asset))
		{
			expand_surface_bounds(world, mins, maxs);
		}

		if (mins[0] > maxs[0])
		{
			return {};
		}

		target::Bounds bounds{};
		compute(&bounds, mins, maxs);
		return bounds;
	}

	std::vector<collision_triangle> get_collision_triangles(const Districts* asset)
	{
		std::vector<collision_triangle> triangles;
		const auto* world = find_world(asset);
		if (!world || !world->draw.surfaces || !world->vertexData.positions || !world->vertexData.indices)
		{
			return triangles;
		}

		const auto& data = world->vertexData;
		for (auto i = 0; i < world->surfaceCount; i++)
		{
			const auto& surface = world->draw.surfaces[i];
			const auto* material = resolve_material(surface.material);
			const auto index_count = surface.triCount * 3u;
			if (!material || is_hidden(material) || material::get_world_surface_list(material) == material::world_surface_list::decal ||
				surface.baseIndex + index_count > data.indexCount)
			{
				continue;
			}

			for (auto j = 0u; j < index_count; j += 3)
			{
				collision_triangle triangle{};
				auto valid = true;
				for (auto corner = 0u; corner < 3; corner++)
				{
					const auto vertex = surface.baseVertex + data.indices[surface.baseIndex + j + corner];
					valid = valid && vertex < data.vertexCount;
					if (valid)
					{
						std::copy_n(data.positions[vertex], 3, triangle[corner].begin());
					}
				}

				if (valid)
				{
					triangles.emplace_back(triangle);
				}
			}
		}

		return triangles;
	}

	namespace
	{
		void append_sky_shell(const target::Bounds& bounds, const float margin, const float facing, std::vector<std::array<float, 3>>& positions,
			std::vector<std::uint32_t>& frames, std::vector<unsigned short>& indices)
		{
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto first = (axis + 1) % 3;
				const auto second = (axis + 2) % 3;
				for (const auto side : {-1.0f, 1.0f})
				{
					std::array<float, 3> normal{};
					normal[axis] = side * facing;
					std::array<float, 3> tangent{};
					tangent[first] = 1.0f;

					const auto base = static_cast<unsigned short>(positions.size());
					for (const auto& [first_side, second_side] : {std::pair{-1.0f, -1.0f}, std::pair{1.0f, -1.0f}, std::pair{-1.0f, 1.0f}, std::pair{1.0f, 1.0f}})
					{
						auto& position = positions.emplace_back();
						position[axis] = bounds.midPoint[axis] + side * (bounds.halfSize[axis] + margin);
						position[first] = bounds.midPoint[first] + first_side * (bounds.halfSize[first] + margin);
						position[second] = bounds.midPoint[second] + second_side * (bounds.halfSize[second] + margin);
						frames.emplace_back(xmodel::pack_tangent_frame(normal, tangent, false));
					}

					const auto corners = normal[axis] > 0.0f
						? std::array<unsigned short, 6>{0, 1, 2, 2, 1, 3}
						: std::array<unsigned short, 6>{0, 2, 1, 1, 2, 3};
					for (const auto corner : corners)
					{
						indices.emplace_back(static_cast<unsigned short>(base + corner));
					}
				}
			}
		}

		void add_sky_surface(const target::Bounds& bounds, const std::string& material_name, world_geometry& geometry, utils::memory::allocator& allocator)
		{
			std::vector<std::array<float, 3>> positions;
			std::vector<std::uint32_t> frames;
			std::vector<unsigned short> indices;
			append_sky_shell(bounds, 1024.0f, 1.0f, positions, frames, indices);
			append_sky_shell(bounds, 2048.0f, -1.0f, positions, frames, indices);

			target::GfxWorldSurfData surf_data{};
			surf_data.layerCount = 1;
			surf_data.xyzOffset = append(geometry.positions, positions.data(), positions.size());
			surf_data.tangentFrameOffset = append(geometry.aux, frames.data(), frames.size());
			const std::vector<std::array<float, 2>> texcoords(positions.size());
			surf_data.texCoordOffset = append(geometry.aux, texcoords.data(), texcoords.size());

			target::GfxSurface surface{};
			surface.tris.posOffset = surf_data.xyzOffset;
			surface.tris.vertexCount = static_cast<unsigned short>(positions.size());
			surface.tris.triCount = static_cast<unsigned short>(indices.size() / 3);
			surface.tris.baseIndex = static_cast<unsigned int>(geometry.indices.size());
			surface.material = make_named<target::Material>(material_name, allocator);
			surface.surfDataIndex = static_cast<unsigned int>(geometry.surf_data.size());
			surface.lightmapIndex = 510;

			target::GfxSurfaceBounds surface_bounds{};
			surface_bounds.bounds = bounds;
			for (auto i = 0; i < 3; i++)
			{
				surface_bounds.bounds.halfSize[i] += 2048.0f;
			}

			geometry.surf_data.emplace_back(surf_data);
			geometry.indices.insert(geometry.indices.end(), indices.begin(), indices.end());
			geometry.surfaces.insert(geometry.surfaces.begin() + geometry.opaque_count, surface);
			geometry.surface_bounds.insert(geometry.surface_bounds.begin() + geometry.opaque_count, surface_bounds);
			ZONETOOL_INFO("gfxworld: sky surface %u with material \"%s\", %zu vertices", geometry.opaque_count, material_name.data(), positions.size());
			geometry.opaque_count++;
			geometry.decal_end++;
		}

		void insert_surface(const material::world_surface_list list, const target::GfxSurface& surface, const target::GfxSurfaceBounds& bounds,
			world_geometry& geometry)
		{
			auto index = geometry.surfaces.size();
			if (list == material::world_surface_list::decal)
			{
				index = geometry.decal_end++;
			}
			else if (list != material::world_surface_list::trans)
			{
				index = geometry.opaque_count++;
				geometry.decal_end++;
			}

			geometry.surfaces.insert(geometry.surfaces.begin() + index, surface);
			geometry.surface_bounds.insert(geometry.surface_bounds.begin() + index, bounds);
		}

		void add_glass_surface(const GlassEntry& entry, const Material* material, world_geometry& geometry, utils::memory::allocator& allocator)
		{
			auto axis = 0;
			for (auto i = 1; i < 3; i++)
			{
				if (entry.maxs[i] - entry.mins[i] < entry.maxs[axis] - entry.mins[axis])
				{
					axis = i;
				}
			}

			const auto first = (axis + 1) % 3;
			const auto second = (axis + 2) % 3;

			std::vector<std::array<float, 3>> positions;
			std::vector<std::uint32_t> frames;
			std::vector<std::array<float, 2>> texcoords;
			std::vector<unsigned short> indices;
			for (const auto side : {-1.0f, 1.0f})
			{
				std::array<float, 3> normal{};
				normal[axis] = side;
				std::array<float, 3> tangent{};
				tangent[first] = 1.0f;

				const auto base = static_cast<unsigned short>(positions.size());
				for (const auto& [u, v] : {std::pair{0.0f, 0.0f}, std::pair{1.0f, 0.0f}, std::pair{0.0f, 1.0f}, std::pair{1.0f, 1.0f}})
				{
					auto& position = positions.emplace_back();
					position[axis] = side > 0.0f ? entry.maxs[axis] : entry.mins[axis];
					position[first] = entry.mins[first] + u * (entry.maxs[first] - entry.mins[first]);
					position[second] = entry.mins[second] + v * (entry.maxs[second] - entry.mins[second]);
					frames.emplace_back(xmodel::pack_tangent_frame(normal, tangent, false));
					texcoords.push_back({u, 1.0f - v});
				}

				const auto corners = side > 0.0f
					? std::array<unsigned short, 6>{0, 2, 1, 1, 2, 3}
					: std::array<unsigned short, 6>{0, 1, 2, 2, 1, 3};
				for (const auto corner : corners)
				{
					indices.emplace_back(static_cast<unsigned short>(base + corner));
				}
			}

			target::GfxWorldSurfData surf_data{};
			surf_data.layerCount = 1;
			surf_data.xyzOffset = append(geometry.positions, positions.data(), positions.size());
			surf_data.tangentFrameOffset = append(geometry.aux, frames.data(), frames.size());
			surf_data.texCoordOffset = append(geometry.aux, texcoords.data(), texcoords.size());
			const std::vector<std::uint32_t> colors(positions.size(), 0xFFFFFFFF);
			surf_data.colorOffset = append(geometry.aux, colors.data(), colors.size());

			target::GfxSurface surface{};
			surface.tris.posOffset = surf_data.xyzOffset;
			surface.tris.vertexCount = static_cast<unsigned short>(positions.size());
			surface.tris.triCount = static_cast<unsigned short>(indices.size() / 3);
			surface.tris.baseIndex = static_cast<unsigned int>(geometry.indices.size());
			surface.material = make_named<target::Material>(material::dump_world(material), allocator);
			surface.surfDataIndex = static_cast<unsigned int>(geometry.surf_data.size());
			surface.lightmapIndex = 510;

			target::GfxSurfaceBounds surface_bounds{};
			for (auto i = 0; i < 3; i++)
			{
				surface_bounds.bounds.midPoint[i] = (entry.mins[i] + entry.maxs[i]) * 0.5f;
				surface_bounds.bounds.halfSize[i] = (entry.maxs[i] - entry.mins[i]) * 0.5f;
			}

			geometry.surf_data.emplace_back(surf_data);
			geometry.indices.insert(geometry.indices.end(), indices.begin(), indices.end());
			insert_surface(material::get_world_surface_list(material), surface, surface_bounds, geometry);
		}

		void add_glass_surfaces(const GfxWorld* world, world_geometry& geometry, utils::memory::allocator& allocator)
		{
			const auto* glasses = world ? static_cast<const Glasses*>(db_find_asset(ASSET_TYPE_GLASSES, xhash_name(world->name))) : nullptr;
			if (!glasses || !glasses->entries)
			{
				return;
			}

			auto added = 0u;
			for (auto i = 0u; i < glasses->entryCount; i++)
			{
				const auto& entry = glasses->entries[i];
				const auto* material = entry.def ? resolve_material(entry.def->materials[1]) : nullptr;
				if (!material || is_hidden(material))
				{
					ZONETOOL_WARNING("glasses \"%s\": glass %u has no material", xhash_name(glasses->name).data(), i);
					continue;
				}

				add_glass_surface(entry, material, geometry, allocator);
				added++;
			}

			ZONETOOL_WARNING("glasses \"%s\": %u glass panes are static surfaces, they do not break and have no collision", xhash_name(glasses->name).data(), added);
		}
	}

	void dump(const Districts* asset, const std::vector<placed_model>& models, const std::vector<StaticModelPlacement>& placements,
		const std::optional<std::string>& sky_material, const std::string& map_name)
	{
		utils::memory::allocator allocator;

		auto geometry = build_world_geometry(find_world(asset), allocator);
		if (models.empty() && geometry.surfaces.empty())
		{
			ZONETOOL_ERROR("map \"%s\" has no world surfaces and no static models, the gfxworld is not written", map_name.data());
			return;
		}

		const auto world_name = std::format("maps/mp/{}.d3dbsp", map_name);
		auto* world = allocator.allocate<target::GfxWorld>();
		world->name = allocator.duplicate_string(world_name);
		world->baseName = allocator.duplicate_string(map_name);
		world->bspVersion = target::BSP_VERSION;
		world->lastSunPrimaryLightIndex = 1;

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

		add_glass_surfaces(find_world(asset), geometry, allocator);

		const auto bounds = get_world_bounds(asset, models);
		if (sky_material)
		{
			add_sky_surface(bounds, *sky_material, geometry, allocator);
		}

		auto* zone = build_transient_zone(world->baseName, geometry, allocator);

		build_cells(bounds, world, allocator);
		build_cell_tree(bounds, geometry.surfaces.size(), models.size(), zone, allocator);
		build_surfaces(geometry, world, allocator);
		build_static_models(models, placements, world, allocator);
		const auto* lighting = light_grid::find_lighting(find_world(asset));
		light_grid::build_static_model_volumes(lighting, models, zone, allocator);
		const auto ambient = light_grid::build_probe_grid(lighting, bounds, world, zone, allocator);
		build_draw(bounds, zone, world);
		if (ambient)
		{
			build_reflection_probe(*ambient, world, allocator);
		}
		build_lights(asset, world, allocator);
		build_models(bounds, world, allocator);
		build_dpvs(world, allocator);
		build_runtime_bits(world, allocator);
		dynentitylist::fill_world(world, allocator);
		build_umbra_tome(bounds, world, allocator);

		target::gfx_world_transient_zone::dump(zone);
		add_csv_row("gfx_map_trzone", map_name);
		target::gfx_world::dump(world);
		add_csv_row("gfx_map", world_name);
	}
}
