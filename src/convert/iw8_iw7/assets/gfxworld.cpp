#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "gfximage.hpp"
#include "material.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"

#include <iw7/assets/gfxlightmap.hpp>
#include <iw7/assets/gfxworld.hpp>
#include <iw7/assets/gfxworld_tr.hpp>

#include <formats/umbra_tome.hpp>
#include <log.hpp>

#include <DirectXPackedVector.h>

namespace zonetool::iw8::converter::iw7::gfxworld
{
	namespace
	{
		namespace target = zonetool::iw7;

		template <typename T>
		T read_at(const char* data, const std::size_t offset)
		{
			T value;
			std::memcpy(&value, data + offset, sizeof(T));
			return value;
		}

		target::GfxWorldVertex convert_vertex(const GfxWorldDrawVerts& verts, const GfxWorldSurfData& data, const unsigned int index)
		{
			target::GfxWorldVertex vertex{};

			const auto xyz = read_at<std::array<float, 3>>(verts.posData, data.xyzOffset + 12 * index);
			std::memcpy(vertex.xyz, xyz.data(), sizeof(vertex.xyz));

			const auto frame = xsurface::unpack_tangent_frame(read_at<std::uint32_t>(verts.auxData, data.tangentFrameOffset + 4 * index));
			vertex.binormalSign = frame.binormal_sign;
			vertex.normal.packed = xsurface::pack_unit_vec(frame.normal, 3);
			vertex.tangent.packed = xsurface::pack_unit_vec(frame.tangent, frame.binormal_sign < 0.0f ? 3 : 0);

			const auto default_color = data.layerCount > 1 ? 0u : 0xFFFFFFFFu;
			vertex.color.packed = data.colorOffset ? read_at<std::uint32_t>(verts.auxData, data.colorOffset + 4 * index) : default_color;

			const auto tex_coord = read_at<std::array<float, 2>>(verts.auxData, data.texCoordOffset + 8 * data.layerCount * index);
			std::memcpy(vertex.texCoord, tex_coord.data(), sizeof(vertex.texCoord));

			if (data.lmapCoordOffset)
			{
				const auto lmap_coord = read_at<std::array<float, 2>>(verts.auxData, data.lmapCoordOffset + 8 * index);
				std::memcpy(vertex.lmapCoord, lmap_coord.data(), sizeof(vertex.lmapCoord));
			}

			return vertex;
		}

		std::uint32_t convert_normal_transform(const std::uint32_t packed)
		{
			std::uint32_t converted{};
			for (auto i = 0; i < 4; i++)
			{
				const auto value = static_cast<float>((packed >> (i * 8)) & 0xFF);
				converted |= static_cast<std::uint32_t>(std::lround(value * 127.0f / 127.5f)) << (i * 8);
			}

			return converted;
		}

		template <typename T>
		void append_bytes(std::vector<unsigned char>& data, const T& value)
		{
			const auto* bytes = reinterpret_cast<const unsigned char*>(&value);
			data.insert(data.end(), bytes, bytes + sizeof(T));
		}

		void append_vertex_layers(std::vector<unsigned char>& layer_data, const GfxWorldDrawVerts& verts, const GfxWorldSurfData& data,
			const unsigned int index, const unsigned int layer_count)
		{
			for (auto layer = 1u; layer < layer_count; layer++)
			{
				const auto source = layer < data.layerCount ? layer : 0u;
				append_bytes(layer_data, read_at<std::array<float, 2>>(verts.auxData, data.texCoordOffset + 8 * (data.layerCount * index + source)));
			}

			for (auto layer = 1u; layer < layer_count; layer++)
			{
				const auto offset = layer < data.layerCount ? data.normalTransformOffset[layer - 1] : 0u;
				const auto transform = offset ? read_at<std::uint32_t>(verts.auxData, offset + 4 * index) : 0xFF8080FFu;
				append_bytes(layer_data, convert_normal_transform(transform));
			}
		}

		void build_layer_data(const GfxWorld* asset, world_geometry& geometry)
		{
			const auto& surfaces = asset->surfaces;
			geometry.surface_layer_offsets.resize(surfaces.count);

			std::map<std::pair<unsigned int, unsigned int>, unsigned int> block_offsets;
			for (auto i = 0u; i < surfaces.count; i++)
			{
				const auto& surface = surfaces.surfaces[i];
				const auto* material = surface.material ? static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, surface.material->name)) : nullptr;
				const auto layer_count = material ? material::get_vertex_layer_count(material) : 1u;
				if (layer_count < 2)
				{
					continue;
				}

				const auto key = std::make_pair(static_cast<unsigned int>(surface.surfDataIndex), layer_count);
				if (const auto block_offset = block_offsets.find(key); block_offset != block_offsets.end())
				{
					geometry.surface_layer_offsets[i] = block_offset->second;
					continue;
				}

				const auto offset = static_cast<unsigned int>(geometry.layer_data.size());
				block_offsets.emplace(key, offset);
				geometry.surface_layer_offsets[i] = offset;

				const auto& data = surfaces.surfData[surface.surfDataIndex];
				const auto& verts = asset->draw.transientZones[data.transientZone]->drawVerts;
				for (auto j = 0u; j < geometry.blocks[surface.surfDataIndex].vertex_count; j++)
				{
					append_vertex_layers(geometry.layer_data, verts, data, j, layer_count);
				}
			}
		}

		world_geometry build_geometry(const GfxWorld* asset)
		{
			const auto& surfaces = asset->surfaces;

			std::vector<unsigned int> block_sizes(surfaces.surfDataCount);
			for (auto i = 0u; i < surfaces.count; i++)
			{
				const auto& surface = surfaces.surfaces[i];
				block_sizes[surface.surfDataIndex] = std::max<unsigned int>(block_sizes[surface.surfDataIndex], surface.tris.vertexCount);
			}

			world_geometry geometry;
			for (auto i = 0u; i < asset->draw.transientZoneCount; i++)
			{
				const auto& verts = asset->draw.transientZones[i]->drawVerts;
				geometry.zone_index_base.emplace_back(static_cast<unsigned int>(geometry.indices.size()));
				geometry.indices.insert(geometry.indices.end(), verts.indices, verts.indices + verts.indexCount);
			}

			for (auto i = 0u; i < surfaces.surfDataCount; i++)
			{
				const auto& data = surfaces.surfData[i];
				const auto& verts = asset->draw.transientZones[data.transientZone]->drawVerts;

				geometry.blocks.push_back({static_cast<unsigned int>(geometry.vertices.size()), block_sizes[i]});
				for (auto j = 0u; j < block_sizes[i]; j++)
				{
					geometry.vertices.emplace_back(convert_vertex(verts, data, j));
				}
			}

			build_layer_data(asset, geometry);
			return geometry;
		}

		float get_max_edge_length(const world_geometry& geometry, const target::srfTriangles_t& tris)
		{
			auto max_length = 0.0f;
			for (auto i = 0u; i < tris.triCount; i++)
			{
				for (auto edge = 0u; edge < 3; edge++)
				{
					const auto& a = geometry.vertices[tris.firstVertex + geometry.indices[tris.baseIndex + i * 3 + edge]].xyz;
					const auto& b = geometry.vertices[tris.firstVertex + geometry.indices[tris.baseIndex + i * 3 + (edge + 1) % 3]].xyz;
					const auto length = std::sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]));
					max_length = std::max(max_length, length);
				}
			}

			return max_length;
		}

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

		void convert_planes(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& planes = asset->dpvsPlanes;
			world->planeCount = planes.planeCount;
			world->nodeCount = planes.nodeCount;
			world->dpvsPlanes.cellCount = planes.cellCount;
			world->dpvsPlanes.nodes = planes.nodes;

			world->dpvsPlanes.planes = allocator.allocate_array<target::cplane_s>(planes.planeCount);
			for (auto i = 0u; i < planes.planeCount; i++)
			{
				world->dpvsPlanes.planes[i] = std::bit_cast<target::cplane_s>(planes.planes[i]);
			}

			world->cells = allocator.allocate_array<target::GfxCell>(planes.cellCount);
			for (auto i = 0u; i < planes.cellCount; i++)
			{
				world->cells[i] = std::bit_cast<target::GfxCell>(asset->cells[i]);
			}

			world->cellTransientInfos = allocator.allocate_array<target::GfxCellTransientInfo>(planes.cellCount);
			for (auto i = 0u; i < planes.cellCount; i++)
			{
				world->cellTransientInfos[i].aabbTreeIndex = static_cast<unsigned short>(i);
			}
		}

		std::vector<unsigned short> expand_static_models(const unsigned short* collections, const unsigned int count,
			const std::vector<static_model_range>& ranges)
		{
			std::vector<unsigned short> smodels;
			for (auto i = 0u; i < count; i++)
			{
				if (collections[i] >= ranges.size())
				{
					ZONETOOL_WARNING("Static model collection %u does not exist", collections[i]);
					continue;
				}

				const auto& range = ranges[collections[i]];
				for (auto j = 0u; j < range.count; j++)
				{
					smodels.emplace_back(static_cast<unsigned short>(range.first + j));
				}
			}

			return smodels;
		}

		unsigned short* copy_indexes(const std::vector<unsigned short>& indexes, utils::memory::allocator& allocator)
		{
			if (indexes.empty())
			{
				return nullptr;
			}

			auto* copy = allocator.allocate_array<unsigned short>(indexes.size());
			std::ranges::copy(indexes, copy);
			return copy;
		}

		target::GfxAabbTree convert_aabb_tree(const GfxAabbTree& tree, const std::vector<static_model_range>& smodel_ranges, utils::memory::allocator& allocator)
		{
			target::GfxAabbTree converted{};
			converted.bounds = std::bit_cast<target::Bounds>(tree.bounds);
			converted.childCount = tree.childCount;
			converted.surfaceCount = static_cast<unsigned short>(tree.surfaceCount);
			converted.startSurfIndex = tree.startSurfIndex;
			converted.childrenOffset = tree.childrenOffset / static_cast<int>(sizeof(GfxAabbTree)) * static_cast<int>(sizeof(target::GfxAabbTree));

			const auto smodels = expand_static_models(tree.smodelIndexes, tree.smodelIndexCount, smodel_ranges);
			converted.smodelIndexCount = static_cast<unsigned short>(smodels.size());
			converted.smodelIndexes = copy_indexes(smodels, allocator);
			return converted;
		}

		target::GfxWorldTransientZone* convert_transient_zone(const GfxWorld* asset, world_geometry& geometry,
			const std::vector<static_model_range>& smodel_ranges, utils::memory::allocator& allocator)
		{
			auto* zone = allocator.allocate<target::GfxWorldTransientZone>();
			zone->name = asset->draw.transientZones[0]->name;
			zone->vertexCount = static_cast<unsigned int>(geometry.vertices.size());
			zone->vd.vertices = geometry.vertices.data();
			zone->vertexLayerDataSize = static_cast<unsigned int>(geometry.layer_data.size());
			zone->vld.data = geometry.layer_data.empty() ? nullptr : geometry.layer_data.data();

			const auto cell_count = asset->dpvsPlanes.cellCount;
			zone->cellCount = cell_count;
			zone->aabbTreeCounts = allocator.allocate_array<target::GfxCellTreeCount>(cell_count);
			zone->aabbTrees = allocator.allocate_array<target::GfxCellTree>(cell_count);

			for (auto i = 0u; i < cell_count; i++)
			{
				const auto& info = asset->cellTransientInfos[i];
				const auto& cells = asset->draw.transientZones[info.transientZone]->drawCells;
				const auto count = cells.aabbTreeCounts[info.aabbTreeIndex].aabbTreeCount;
				const auto* trees = cells.aabbTrees[info.aabbTreeIndex].aabbTree;

				zone->aabbTreeCounts[i].aabbTreeCount = count;
				zone->aabbTrees[i].aabbTree = allocator.allocate_array<target::GfxAabbTree>(count);
				for (auto j = 0; j < count; j++)
				{
					zone->aabbTrees[i].aabbTree[j] = convert_aabb_tree(trees[j], smodel_ranges, allocator);
				}
			}

			return zone;
		}

		void convert_lightmaps(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& source = asset->draw;
			auto& draw = world->draw;
			if (source.lightmapType == GFX_LIGHTMAP_TYPE_AB_LIGHTING)
			{
				ZONETOOL_WARNING("gfxworld \"%s\" has AB lightmaps, they are not converted", asset->name);
				return;
			}

			draw.lightMapCount = source.lightmapCount;
			draw.lightMaps = allocator.allocate_array<target::GfxLightMap*>(source.lightmapCount);
			draw.lightmapReindexData = std::bit_cast<target::GfxWorldLightmapReindexData>(source.lightmapReindexData);
			for (auto i = 0u; i < source.transientZoneCount; i++)
			{
				const auto* atlas = source.transientZones[i]->tempLightmapData;
				for (auto j = 0u; atlas && j < atlas->numLightmaps; j++)
				{
					const auto index = atlas->firstLightmapIndex + j;
					const auto name = std::format("*lightmap{}", index);
					const auto textures = index < draw.lightMapCount ? gfximage::dump_lightmap(atlas->lightmaps[j], name, source.bakedLightScale) : std::nullopt;
					if (!textures)
					{
						continue;
					}

					auto* lightmap = make_named<target::GfxLightMap>(name.data(), allocator);
					for (auto k = 0u; k < textures->size(); k++)
					{
						lightmap->textures[k] = make_named<target::GfxImage>((*textures)[k].data(), allocator);
					}

					target::gfx_light_map::dump(lightmap);
					draw.lightMaps[index] = lightmap;
				}
			}
		}

		unsigned char get_lightmap_index(const target::GfxWorld* world, const GfxSurface& surface)
		{
			const auto& draw = world->draw;
			return surface.lightmapIndex < draw.lightMapCount && draw.lightMaps[surface.lightmapIndex] ? static_cast<unsigned char>(surface.lightmapIndex) : 31;
		}

		void warn_missing_lightmap(const GfxWorld* asset, const GfxSurface& surface, const unsigned char lightmap_index)
		{
			if (lightmap_index != 31 || !surface.material)
			{
				return;
			}

			const auto* material = static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, surface.material->name));
			if (material && material::uses_lightmap(material))
			{
				ZONETOOL_WARNING("A surface of gfxworld \"%s\" has no lightmap, but its material \"%s\" uses one", asset->name, material->name);
			}
		}

		void convert_surfaces(const GfxWorld* asset, const world_geometry& geometry, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& surfaces = asset->surfaces;
			auto& dpvs = world->dpvs;

			world->surfaceCount = surfaces.count;
			dpvs.staticSurfaceCount = surfaces.count;
			dpvs.litOpaqueSurfsBegin = surfaces.litOpaqueSurfsBegin;
			dpvs.litOpaqueSurfsEnd = surfaces.litOpaqueSurfsEnd;
			dpvs.litDecalSurfsBegin = surfaces.litDecalSurfsBegin;
			dpvs.litDecalSurfsEnd = surfaces.litDecalSurfsEnd;
			dpvs.litTransSurfsBegin = surfaces.litTransSurfsBegin;
			dpvs.litTransSurfsEnd = surfaces.litTransSurfsEnd;
			dpvs.emissiveSurfsBegin = surfaces.emissiveSurfsBegin;
			dpvs.emissiveSurfsEnd = surfaces.emissiveSurfsEnd;

			dpvs.surfaces = allocator.allocate_array<target::GfxSurface>(surfaces.count);
			dpvs.surfacesBounds = allocator.allocate_array<target::GfxSurfaceBounds>(surfaces.count);
			dpvs.sortedSurfIndex = allocator.allocate_array<unsigned int>(surfaces.count);
			for (auto i = 0u; i < surfaces.count; i++)
			{
				dpvs.sortedSurfIndex[i] = asset->dpvs.sortedSurfIndex[i];

				const auto& surface = surfaces.surfaces[i];
				auto& converted = dpvs.surfaces[i];

				converted.tris.vertexLayerData = geometry.surface_layer_offsets[i];
				converted.tris.firstVertex = geometry.blocks[surface.surfDataIndex].first_vertex;
				converted.tris.vertexCount = surface.tris.vertexCount;
				converted.tris.triCount = surface.tris.triCount;
				converted.tris.baseIndex = geometry.zone_index_base[surface.transientZone] + surface.tris.baseIndex;
				converted.tris.maxEdgeLength = get_max_edge_length(geometry, converted.tris);
				converted.material = make_named<target::Material>(surface.material, allocator);
				converted.lightmapIndex = get_lightmap_index(world, surface);
				warn_missing_lightmap(asset, surface, converted.lightmapIndex);
				converted.flags = (surface.flags & target::GFX_SURFACE_CASTS_SUN_SHADOW) | ((surface.flags & GFX_SURFACE_SUN_SHADOW_OPT_MASK) << 2);

				dpvs.surfacesBounds[i].bounds = std::bit_cast<target::Bounds>(surfaces.surfaceBounds[i].bounds);
			}

			dpvs.surfaceVisDataCount = (surfaces.count + 31) >> 5;

			dpvs.surfaceCastsSunShadow = allocator.allocate_array<unsigned int>(dpvs.surfaceVisDataCount);
			for (auto i = dpvs.litOpaqueSurfsBegin; i < dpvs.litOpaqueSurfsEnd; i++)
			{
				if (dpvs.surfaces[i].flags & target::GFX_SURFACE_CASTS_SUN_SHADOW)
				{
					dpvs.surfaceCastsSunShadow[i >> 5] |= 1u << (i & 31);
				}
			}

			dpvs.sunShadowOptCount = asset->dpvs.sunShadowOptCount;
			dpvs.sunSurfVisDataCount = asset->dpvs.sunSurfVisDataCount;
		}

		void convert_reflection_probes(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& source = asset->draw.reflectionProbeData;
			auto& probes = world->draw.reflectionProbeData;

			probes.reflectionProbeCount = source.reflectionProbeCount;
			probes.reflectionProbeArrayImage = make_named<target::GfxImage>(source.reflectionProbeArrayImage, allocator);
			if (!probes.reflectionProbeArrayImage && source.reflectionProbeArrayImage_octahedron)
			{
				gfximage::dump_reflection_probe_array(source.reflectionProbeArrayImage_octahedron, source.reflectionProbeSH,
					source.reflectionProbeSH ? source.reflectionProbeCount : 0, "*reflection_probe_array");
				probes.reflectionProbeArrayImage = make_named<target::GfxImage>("*reflection_probe_array", allocator);
			}
			probes.reflectionProbeLightgridSampleData = allocator.allocate_array<target::GfxReflectionProbeSampleData>(source.reflectionProbeCount);

			probes.reflectionProbes = allocator.allocate_array<target::GfxReflectionProbe>(source.reflectionProbeCount);
			for (auto i = 0u; i < source.reflectionProbeCount; i++)
			{
				const auto& probe = source.reflectionProbes[i];
				auto& converted = probes.reflectionProbes[i];

				converted.livePath = probe.livePath;
				std::memcpy(converted.origin, probe.origin, sizeof(converted.origin));
				std::memcpy(converted.angles, probe.angles, sizeof(converted.angles));
				converted.probeRelightingIndex = probe.probeRelightingIndex == 0xFFFF ? 0xFFFFFFFF : probe.probeRelightingIndex;
				converted.probeInstanceCount = probe.probeInstanceCount;
				converted.probeInstances = allocator.allocate_array<unsigned int>(probe.probeInstanceCount);
				for (auto j = 0; j < probe.probeInstanceCount; j++)
				{
					converted.probeInstances[j] = probe.probeInstances[j];
				}
			}

			probes.reflectionProbeInstanceCount = source.reflectionProbeInstanceCount;
			probes.reflectionProbeInstances = allocator.allocate_array<target::GfxReflectionProbeInstance>(source.reflectionProbeInstanceCount);
			for (auto i = 0u; i < source.reflectionProbeInstanceCount; i++)
			{
				const auto& instance = source.reflectionProbeInstances[i];
				auto& converted = probes.reflectionProbeInstances[i];

				converted.livePath = instance.livePath;
				std::memcpy(converted.probePosition, instance.probePosition, sizeof(converted.probePosition));
				converted.probeImageIndex = instance.probeImageIndex;
				converted.lodLevel = instance.lodLevel;
				converted.flags = instance.flags;
				std::memcpy(converted.probeRotation, instance.probeRotation, sizeof(converted.probeRotation));
				converted.volumeObb = std::bit_cast<target::GfxReflectionProbeObb>(instance.volumeObb);
				converted.priority = instance.priority;
				std::memcpy(converted.feather, instance.feather, sizeof(converted.feather));
				std::memcpy(converted.expandProjectionNeg, instance.expandProjectionNeg, sizeof(converted.expandProjectionNeg));
				std::memcpy(converted.expandProjectionPos, instance.expandProjectionPos, sizeof(converted.expandProjectionPos));
			}
		}

		light_grid_probe decode_light_grid_probe(const GfxSHCompressedProbeData& probe, const vec3_t& origin, const float scale)
		{
			DirectX::PackedVector::XMFLOAT3PK packed_l0{};
			packed_l0.v = probe.l0;
			DirectX::XMFLOAT3 l0{};
			DirectX::XMStoreFloat3(&l0, DirectX::PackedVector::XMLoadFloat3PK(&packed_l0));
			const std::array<float, 3> base{l0.x * scale, l0.y * scale, l0.z * scale};

			light_grid_probe result{{origin[0], origin[1], origin[2]}};
			for (auto channel = 0; channel < 3; channel++)
			{
				result.coefficients[channel * 9] = base[channel];
				for (auto i = 1; i < 9; i++)
				{
					const auto band_scale = i < 4 ? std::sqrt(3.0f) : std::sqrt(5.0f);
					result.coefficients[channel * 9 + i] = (probe.shRatios[channel][i - 1] - 127.0f) / 127.0f * base[channel] * band_scale;
				}
			}

			result.sun_visibility = probe.skyVisibility / 255.0f;
			return result;
		}

		std::vector<light_grid_probe> decode_light_grid(const GfxWorld* asset)
		{
			std::vector<light_grid_probe> probes;
			for (auto i = 0u; i < asset->draw.transientZoneCount; i++)
			{
				const auto* grid = asset->draw.transientZones[i]->gpuLightGrid.gpuLightGrid;
				for (auto j = 0u; grid && j < grid->probeCount; j++)
				{
					probes.push_back(decode_light_grid_probe(grid->probes[j], grid->probePositions[j], asset->draw.bakedLightScale));
				}
			}

			return probes;
		}

		target::GfxSHProbeData pack_light_grid_probe(const light_grid_probe& probe)
		{
			target::GfxSHProbeData result{};
			for (auto i = 0; i < 27; i++)
			{
				result.coeffs[i] = DirectX::PackedVector::XMConvertFloatToHalf(probe.coefficients[i]);
			}

			result.coeffs[27] = DirectX::PackedVector::XMConvertFloatToHalf(probe.sun_visibility);
			return result;
		}

		light_grid_probe sample_light_grid(const std::vector<light_grid_probe>& probes, const float* origin)
		{
			std::array<std::pair<float, const light_grid_probe*>, 4> nearest{};
			nearest.fill({std::numeric_limits<float>::max(), nullptr});
			for (const auto& probe : probes)
			{
				auto distance = 0.0f;
				for (auto i = 0; i < 3; i++)
				{
					distance += (probe.origin[i] - origin[i]) * (probe.origin[i] - origin[i]);
				}

				if (distance < nearest.back().first)
				{
					nearest.back() = {distance, &probe};
					std::ranges::sort(nearest, {}, &std::pair<float, const light_grid_probe*>::first);
				}
			}

			light_grid_probe sample{{origin[0], origin[1], origin[2]}};
			auto total_weight = 0.0f;
			for (const auto& [distance, probe] : nearest)
			{
				if (!probe)
				{
					continue;
				}

				const auto weight = 1.0f / (distance + 1.0f);
				for (auto i = 0; i < 27; i++)
				{
					sample.coefficients[i] += probe->coefficients[i] * weight;
				}

				sample.sun_visibility += probe->sun_visibility * weight;
				total_weight += weight;
			}

			if (total_weight <= 0.0f)
			{
				sample.sun_visibility = 1.0f;
				return sample;
			}

			for (auto& coefficient : sample.coefficients)
			{
				coefficient /= total_weight;
			}

			sample.sun_visibility /= total_weight;
			return sample;
		}

		std::optional<std::array<float, 3>> read_static_model_volume_ambient(const GfxWorld* asset, const unsigned int instance_index)
		{
			if (!asset->smodels.modelStaticIndirection)
			{
				return {};
			}

			const auto& indirection = asset->smodels.modelStaticIndirection[instance_index];
			if (indirection.worldTransient >= asset->draw.transientZoneCount || !asset->draw.transientZones[indirection.worldTransient])
			{
				return {};
			}

			const auto* atlas = asset->draw.transientZones[indirection.worldTransient]->gpuLightGrid.tempLightGridVolumeData;
			if (!atlas || indirection.lightingInstanceIndirection >= atlas->m_numVolumes || !atlas->m_textures[0].pixels)
			{
				return {};
			}

			const auto& texture = atlas->m_textures[0].desc;
			if (texture.format != GFX_PF_R11G11B10F || texture.totalSize < texture.width * texture.height * texture.depth * 4u)
			{
				return {};
			}

			const auto& packing = atlas->m_packingParams[indirection.lightingInstanceIndirection];
			const auto* texels = reinterpret_cast<const DirectX::PackedVector::XMFLOAT3PK*>(atlas->m_textures[0].pixels);
			const std::array<unsigned int, 3> size{texture.width, texture.height, texture.depth};
			std::array<unsigned int, 3> first{};
			for (auto axis = 0; axis < 3; axis++)
			{
				first[axis] = static_cast<unsigned int>(packing.m_uvBias[axis] * size[axis]);
				if (first[axis] + packing.m_volumeSize[axis] > size[axis])
				{
					return {};
				}
			}

			std::array<float, 3> ambient{};
			for (auto z = first[2]; z < first[2] + packing.m_volumeSize[2]; z++)
			{
				for (auto y = first[1]; y < first[1] + packing.m_volumeSize[1]; y++)
				{
					for (auto x = first[0]; x < first[0] + packing.m_volumeSize[0]; x++)
					{
						DirectX::XMFLOAT3 value{};
						DirectX::XMStoreFloat3(&value, DirectX::PackedVector::XMLoadFloat3PK(&texels[(z * size[1] + y) * size[0] + x]));
						ambient[0] += value.x;
						ambient[1] += value.y;
						ambient[2] += value.z;
					}
				}
			}

			const auto scale = asset->draw.bakedLightScale / (packing.m_volumeSize[0] * packing.m_volumeSize[1] * packing.m_volumeSize[2]);
			for (auto& channel : ambient)
			{
				channel *= scale;
			}

			return ambient;
		}

		void apply_ambient(light_grid_probe& probe, const std::array<float, 3>& ambient) // keeps the grid L1 direction, the LGV ratio texture is not decoded
		{
			for (auto channel = 0; channel < 3; channel++)
			{
				auto* coefficients = &probe.coefficients[channel * 9];
				const auto scale = coefficients[0] > 0.0f ? ambient[channel] / coefficients[0] : 0.0f;
				for (auto i = 1; i < 9; i++)
				{
					coefficients[i] *= scale;
				}

				coefficients[0] = ambient[channel];
			}
		}

		target::GfxSHProbeData convert_fallback_probe(const GfxSHProbeData& probe, const float scale)
		{
			target::GfxSHProbeData result{};
			for (auto i = 0; i < 27; i++)
			{
				result.coeffs[i] = DirectX::PackedVector::XMConvertFloatToHalf(DirectX::PackedVector::XMConvertHalfToFloat(probe.coeffs[i]) * scale);
			}

			result.coeffs[27] = probe.coeffs[27];
			return result;
		}

		void convert_light_grid_zones(const GfxGpuLightGrid& source, target::GfxLightGridProbeData& probe_data, const float scale,
			utils::memory::allocator& allocator)
		{
			probe_data.zoneCount = source.zoneCount;
			probe_data.zones = allocator.allocate_array<target::GfxGpuLightGridZone>(source.zoneCount);
			for (auto i = 0u; i < source.zoneCount; i++)
			{
				probe_data.zones[i].fallbackProbeData = convert_fallback_probe(source.zones[i].fallbackProbeData, scale);
			}
		}

		bool convert_voxel_start_tetrahedrons(const GfxWorld* asset, const GfxGpuLightGrid& source, target::GfxLightGridProbeData& probe_data,
			utils::memory::allocator& allocator)
		{
			auto count = 0u;
			for (auto i = 0u; i < source.zoneCount; i++)
			{
				const auto leaf_count = static_cast<unsigned int>(asset->voxelTree[i].voxelLeafNodeCount);
				if (source.zones[i].firstVoxelTetrahedronIndex + leaf_count > source.voxelStartTetrahedronCount)
				{
					return false;
				}

				count += leaf_count;
			}

			probe_data.voxelStartTetrahedronCount = count;
			probe_data.voxelStartTetrahedron = allocator.allocate_array<target::GfxGpuLightGridVoxelStartTetrahedron>(count);

			auto first = 0u;
			for (auto i = 0u; i < source.zoneCount; i++)
			{
				const auto& zone = source.zones[i];
				const auto leaf_count = static_cast<unsigned int>(asset->voxelTree[i].voxelLeafNodeCount);
				for (auto leaf = 0u; leaf < leaf_count; leaf++)
				{
					probe_data.voxelStartTetrahedron[first + leaf].index = source.voxelStartTetrahedron[zone.firstVoxelTetrahedronIndex + leaf].index;
				}

				probe_data.zones[i].firstVoxelTetrahedronIndex = first;
				probe_data.zones[i].numVoxelTetrahedronIndices = leaf_count;
				first += leaf_count;
			}

			return true;
		}

		void convert_tetrahedral_grid(const GfxGpuLightGrid& source, target::GfxLightGridProbeData& probe_data, const float scale,
			utils::memory::allocator& allocator)
		{
			probe_data.probeCount = source.probeCount;
			probe_data.probes = allocator.allocate_array<target::GfxSHProbeData>(source.probeCount);
			probe_data.probePositions = allocator.allocate_array<target::GfxGpuLightGridProbePosition>(source.probeCount);
			for (auto i = 0u; i < source.probeCount; i++)
			{
				probe_data.probes[i] = pack_light_grid_probe(decode_light_grid_probe(source.probes[i], source.probePositions[i], scale));
				std::memcpy(probe_data.probePositions[i].origin, source.probePositions[i], sizeof(probe_data.probePositions[i].origin));
			}

			for (auto i = 0u; i < source.zoneCount; i++)
			{
				const auto& zone = source.zones[i];
				auto& converted = probe_data.zones[i];
				converted.numProbes = zone.numProbes;
				converted.firstProbe = zone.firstProbe;
				converted.numTetrahedrons = zone.numTetrahedrons;
				converted.firstTetrahedron = zone.firstTetrahedron;
			}

			probe_data.tetrahedronCount = source.tetrahedronCount;
			probe_data.tetrahedronCountVisible = source.tetrahedronCountVisible;
			probe_data.tetrahedrons = allocator.allocate_array<target::GfxGpuLightGridTetrahedron>(source.tetrahedronCount);
			probe_data.tetrahedronNeighbors = allocator.allocate_array<target::GfxGpuLightGridTetrahedronNeighbors>(source.tetrahedronCount);
			for (auto i = 0u; i < source.tetrahedronCount; i++)
			{
				probe_data.tetrahedrons[i] = std::bit_cast<target::GfxGpuLightGridTetrahedron>(source.tetrahedrons[i]);
				probe_data.tetrahedronNeighbors[i] = std::bit_cast<target::GfxGpuLightGridTetrahedronNeighbors>(source.tetrahedronNeighbors[i]);
			}

			probe_data.tetrahedronVisibility = allocator.allocate_array<target::GfxGpuLightGridTetrahedronVisibility>(source.tetrahedronCountVisible);
			for (auto i = 0u; i < source.tetrahedronCountVisible; i++)
			{
				probe_data.tetrahedronVisibility[i] = std::bit_cast<target::GfxGpuLightGridTetrahedronVisibility>(source.tetrahedronVisibility[i]);
			}
		}

		void convert_light_grid(const GfxWorld* asset, target::GfxWorld* world, const std::vector<static_model_range>& smodel_ranges,
			utils::memory::allocator& allocator)
		{
			auto& grid = world->lightGrid;
			const std::array<int, 9> unknown{0, 0, 5, 5, 6, 32, 32, 64, 0};
			std::ranges::copy(unknown, grid.unk);
			grid.tableVersion = 1;
			grid.paletteVersion = 1;

			const auto* source = asset->draw.transientZones[0]->gpuLightGrid.gpuLightGrid;
			if (!source || !source->zoneCount)
			{
				return;
			}

			for (auto i = 1u; i < asset->draw.transientZoneCount; i++)
			{
				if (asset->draw.transientZones[i]->gpuLightGrid.gpuLightGrid)
				{
					ZONETOOL_WARNING("gfxworld \"%s\": the light grid of transient zone %u is not converted", asset->name, i);
				}
			}

			const auto scale = asset->draw.bakedLightScale;
			auto& probe_data = grid.probeData;
			convert_light_grid_zones(*source, probe_data, scale, allocator);
			if (source->zoneVoxelTrees || source->zoneCount > static_cast<unsigned int>(asset->voxelTreeCount)
				|| !convert_voxel_start_tetrahedrons(asset, *source, probe_data, allocator))
			{
				ZONETOOL_WARNING("gfxworld \"%s\": the light grid does not use the world voxel trees, only the fallback probes are converted", asset->name);
			}
			else
			{
				convert_tetrahedral_grid(*source, probe_data, scale, allocator);
			}

			const auto probes = decode_light_grid(asset);
			auto& dpvs = world->dpvs;
			probe_data.gpuVisibleProbesCount = dpvs.smodelCount;
			probe_data.gpuVisibleProbePositions = allocator.allocate_array<target::GfxGpuLightGridProbePosition>(dpvs.smodelCount);
			probe_data.gpuVisibleProbesData = allocator.allocate_array<target::GfxSHProbeData>(dpvs.smodelCount + 0x2000); // gfx_world::write writes 0x2000 more
			for (auto collection = 0u; collection < smodel_ranges.size(); collection++)
			{
				const auto& range = smodel_ranges[collection];
				for (auto i = range.first; i < range.first + range.count; i++)
				{
					const auto* origin = dpvs.smodelInsts[i].lightingOrigin;
					std::memcpy(probe_data.gpuVisibleProbePositions[i].origin, origin, sizeof(probe_data.gpuVisibleProbePositions[i].origin));

					auto probe = sample_light_grid(probes, origin);
					if (const auto ambient = read_static_model_volume_ambient(asset, asset->smodels.collections[collection].firstInstance + i - range.first))
					{
						apply_ambient(probe, *ambient);
					}

					probe_data.gpuVisibleProbesData[i] = pack_light_grid_probe(probe);
					dpvs.smodelDrawInsts[i].firstProbe = i;
					dpvs.smodelDrawInsts[i].probeCount = 1;
				}
			}
		}

		target::GfxShadowGeometry convert_shadow_geometry(const GfxShadowGeometry& geometry, const std::vector<static_model_range>& smodel_ranges,
			utils::memory::allocator& allocator)
		{
			auto converted = std::bit_cast<target::GfxShadowGeometry>(geometry);

			const auto smodels = expand_static_models(geometry.smodelIndex, geometry.smodelCount, smodel_ranges);
			converted.smodelCount = static_cast<unsigned short>(smodels.size());
			converted.smodelIndex = copy_indexes(smodels, allocator);
			return converted;
		}

		float unpack_quaternion_component(const unsigned short value)
		{
			return static_cast<float>(value) / 65535.0f * 2.0f - 1.0f;
		}

		target::GfxPackedPlacement convert_placement(const GfxSModelInstanceData& instance)
		{
			const auto x = unpack_quaternion_component(instance.orientation[0]);
			const auto y = unpack_quaternion_component(instance.orientation[1]);
			const auto z = unpack_quaternion_component(instance.orientation[2]);
			const auto w = unpack_quaternion_component(instance.orientation[3]);

			const float axis[3][3]
			{
				{1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y + w * z), 2.0f * (x * z - w * y)},
				{2.0f * (x * y - w * z), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z + w * x)},
				{2.0f * (x * z + w * y), 2.0f * (y * z - w * x), 1.0f - 2.0f * (x * x + y * y)},
			};

			target::GfxPackedPlacement placement{};
			for (auto i = 0; i < 3; i++)
			{
				placement.origin[i] = static_cast<float>(instance.translation[i]) / 4096.0f;
			}

			std::memcpy(placement.axis, axis, sizeof(axis));
			placement.scale = instance.scale;
			return placement;
		}

		target::Bounds transform_bounds(const Bounds& bounds, const target::GfxPackedPlacement& placement)
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

		unsigned short get_cull_dist(const XModel* model, const float scale)
		{
			if (!model->numLods)
			{
				return 0;
			}

			const auto dist = model->lodInfo[model->numLods - 1].dist * scale;
			return dist > 65535.0f ? 0 : static_cast<unsigned short>(std::max(dist, 1.0f));
		}

		std::vector<static_model_range> convert_static_models(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& smodels = asset->smodels;

			std::vector<static_model_range> ranges(smodels.collectionCount);
			std::vector<static_model_instance> instances;
			auto skipped = 0u;
			for (auto i = 0u; i < smodels.collectionCount; i++)
			{
				const auto& collection = smodels.collections[i];
				const auto* model = smodels.models[collection.smodelIndex].model;
				ranges[i].first = static_cast<unsigned int>(instances.size());
				if (!model || !xmodel::is_supported(model))
				{
					skipped += collection.instanceCount;
					continue;
				}

				ranges[i].count = collection.instanceCount;
				for (auto j = 0u; j < collection.instanceCount; j++)
				{
					instances.push_back({model, &smodels.smodelInstanceData[collection.firstInstance + j]});
				}
			}

			if (instances.size() > std::numeric_limits<unsigned short>::max())
			{
				ZONETOOL_ERROR("gfxworld \"%s\" has %zu static model instances, IW7 supports 65535", asset->name, instances.size());
				return {};
			}

			auto& dpvs = world->dpvs;
			dpvs.smodelCount = static_cast<unsigned int>(instances.size());
			dpvs.smodelVisDataCount = (dpvs.smodelCount + 31) >> 5;
			dpvs.smodelInsts = allocator.allocate_array<target::GfxStaticModelInst>(dpvs.smodelCount);
			dpvs.smodelDrawInsts = allocator.allocate_array<target::GfxStaticModelDrawInst>(dpvs.smodelCount);
			dpvs.sortedSmodelIndices = allocator.allocate_array<unsigned short>(dpvs.smodelCount);

			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				const auto& instance = instances[i];
				auto& draw = dpvs.smodelDrawInsts[i];
				draw.placement = convert_placement(*instance.data);
				draw.model = make_named<target::XModel>(instance.model->name, allocator);
				draw.modelLightmapInfo.lightmapIndex = -1; // TODO: lightmaps are not converted
				draw.flags = target::STATIC_MODEL_FLAG_SCALE_6 | target::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING | target::STATIC_MODEL_FLAG_ALLOW_FXMARK;
				draw.cullDist = get_cull_dist(instance.model, draw.placement.scale);
				draw.sunShadowFlags = 1;

				auto& inst = dpvs.smodelInsts[i];
				inst.bounds = transform_bounds(instance.model->bounds, draw.placement);
				std::memcpy(inst.lightingOrigin, inst.bounds.midPoint, sizeof(inst.lightingOrigin));

				dpvs.sortedSmodelIndices[i] = static_cast<unsigned short>(i);
			}

			std::stable_sort(dpvs.sortedSmodelIndices, dpvs.sortedSmodelIndices + dpvs.smodelCount, [&](const unsigned short a, const unsigned short b)
			{
				return std::string_view(instances[a].model->name) < std::string_view(instances[b].model->name);
			});

			ZONETOOL_INFO("Converted %u static models from %u collections, skipped %u with unsupported xmodels", dpvs.smodelCount, smodels.collectionCount, skipped);
			return ranges;
		}

		void convert_lights(const GfxWorld* asset, target::GfxWorld* world, const std::vector<static_model_range>& smodel_ranges,
			utils::memory::allocator& allocator)
		{
			const auto light_count = asset->primaryLightCount;
			world->primaryLightCount = light_count;
			world->lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
			world->movingScriptablePrimaryLightCount = asset->movingScriptablePrimaryLightCount;

			world->frustumLights = allocator.allocate_array<target::GfxFrustumLights>(light_count);
			world->lightRegion = allocator.allocate_array<target::GfxLightRegion>(light_count);

			world->lightViewFrustums = allocator.allocate_array<target::GfxLightViewFrustum>(light_count);
			for (auto i = 0u; i < light_count && asset->lightViewFrustums; i++)
			{
				const auto& frustum = asset->lightViewFrustums[i];
				auto& converted = world->lightViewFrustums[i];

				converted.planeCount = frustum.planeCount;
				converted.planes = reinterpret_cast<vec4_t*>(frustum.planes);
				converted.vertexCount = frustum.vertexCount;
				converted.vertices = reinterpret_cast<vec3_t*>(frustum.vertices);
				converted.indexCount = frustum.indexCount;
				converted.indices = allocator.allocate_array<unsigned short>(frustum.indexCount);
				std::copy_n(frustum.indices, frustum.indexCount, converted.indices);
			}

			world->shadowGeomOptimized = allocator.allocate_array<target::GfxShadowGeometry>(light_count);
			for (auto i = 0u; i < light_count && asset->shadowGeomOptimized; i++)
			{
				world->shadowGeomOptimized[i] = convert_shadow_geometry(asset->shadowGeomOptimized[i], smodel_ranges, allocator);
			}

			world->lightAABB = std::bit_cast<target::GfxLightAABB>(asset->lightAABB);
		}

		void convert_voxel_trees(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->voxelTreeCount = asset->voxelTreeCount;
			world->voxelTree = allocator.allocate_array<target::GfxVoxelTree>(asset->voxelTreeCount);
			for (auto i = 0; i < asset->voxelTreeCount; i++)
			{
				world->voxelTree[i] = std::bit_cast<target::GfxVoxelTree>(asset->voxelTree[i]);
			}
		}

		void convert_models(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->modelCount = asset->modelCount;
			world->models = allocator.allocate_array<target::GfxBrushModel>(asset->modelCount);
			for (auto i = 0; i < asset->modelCount; i++)
			{
				const auto& model = asset->models[i];
				auto& converted = world->models[i];

				converted.writable.bounds = std::bit_cast<target::Bounds>(model.bounds);
				converted.bounds = std::bit_cast<target::Bounds>(model.bounds);
				converted.radius = model.radius;
				converted.startSurfIndex = model.startSurfIndex;
				converted.surfaceCount = static_cast<unsigned short>(model.surfaceCount);
			}
		}

		void convert_heightfields(const GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->heightfieldCount = asset->heightfieldCount;
			world->heightfields = allocator.allocate_array<target::GfxHeightfield>(asset->heightfieldCount);
			for (auto i = 0; i < asset->heightfieldCount; i++)
			{
				const auto& heightfield = asset->heightfields[i];
				auto& converted = world->heightfields[i];

				converted.bounds = std::bit_cast<target::Bounds>(heightfield.bounds);
				std::memcpy(converted.lookupMatrix, heightfield.lookupMatrix, sizeof(converted.lookupMatrix));
				converted.image = make_named<target::GfxImage>(heightfield.image, allocator);
			}
		}

		void convert_dynamic_data(target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& dpvs = world->dpvs;
			dpvs.primaryLightVisDataCount = (world->primaryLightCount + 31) >> 5;
			dpvs.reflectionProbeVisDataCount = (world->draw.reflectionProbeData.reflectionProbeInstanceCount + 31) >> 5;

			auto& dyn = world->dpvsDyn;
			dyn.dynEntClientCount[0] = 64; // TODO: dynents are not converted, reserve as v1 h1_iw7 does
			dyn.dynEntClientWordCount[0] = 2;

			const auto static_light_count = world->primaryLightCount - world->lastSunPrimaryLightIndex - world->movingScriptablePrimaryLightCount - 1;
			world->staticSpotOmniPrimaryLightCountAligned = (static_light_count + 31) & ~31u;
			world->primaryLightMotionDetectBitsEntries = world->staticSpotOmniPrimaryLightCountAligned >> 4;
			world->entityMotionBitsEntries = 134;
			world->numPrimaryLightEntityShadowVisEntries = world->staticSpotOmniPrimaryLightCountAligned * 134;

			for (auto i = 0; i < 2; i++)
			{
				world->dynEntMotionBitsEntries[i] = ((dyn.dynEntClientCount[i] + 31) >> 5) * 2;
				world->numPrimaryLightDynEntShadowVisEntries[i] = (world->staticSpotOmniPrimaryLightCountAligned * dyn.dynEntClientCount[i]) >> 4;
			}

			dpvs.lodData = allocator.allocate_array<unsigned int>(dpvs.smodelCount + 1);
			std::iota(dpvs.lodData, dpvs.lodData + dpvs.smodelCount + 1, 0u);
		}

		umbra_tome::object make_umbra_object(const unsigned int user_id, const target::Bounds& bounds)
		{
			umbra_tome::object object{user_id};
			for (auto i = 0; i < 3; i++)
			{
				object.minimum[i] = bounds.midPoint[i] - bounds.halfSize[i];
				object.maximum[i] = bounds.midPoint[i] + bounds.halfSize[i];
			}

			return object;
		}

		void grow_umbra_object(umbra_tome::object& scene, const umbra_tome::object& object)
		{
			for (auto i = 0; i < 3; i++)
			{
				scene.minimum[i] = std::min(scene.minimum[i], object.minimum[i]);
				scene.maximum[i] = std::max(scene.maximum[i], object.maximum[i]);
			}
		}

		void build_umbra_tome(target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto& dpvs = world->dpvs;
			auto scene = make_umbra_object(0, world->bounds);

			std::vector<umbra_tome::object> objects;
			for (auto i = 0u; i < dpvs.staticSurfaceCount; i++)
			{
				objects.push_back(make_umbra_object(target::UMBRA_OBJECT_SURFACE << 28 | i, dpvs.surfacesBounds[dpvs.sortedSurfIndex[i]].bounds));
				grow_umbra_object(scene, objects.back());
			}

			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				objects.push_back(make_umbra_object(target::UMBRA_OBJECT_STATIC_MODEL << 28 | i, dpvs.smodelInsts[i].bounds));
				grow_umbra_object(scene, objects.back());
			}

			const auto first_scene_object = objects.size();
			for (auto i = 0u; i < world->primaryLightCount; i++)
			{
				objects.push_back({target::UMBRA_OBJECT_PRIMARY_LIGHT << 28 | i});
			}

			for (auto i = 0u; i < world->draw.reflectionProbeData.reflectionProbeInstanceCount; i++)
			{
				objects.push_back({target::UMBRA_OBJECT_REFLECTION_PROBE << 28 | i});
			}

			for (auto i = first_scene_object; i < objects.size(); i++)
			{
				std::memcpy(objects[i].minimum, scene.minimum, sizeof(scene.minimum));
				std::memcpy(objects[i].maximum, scene.maximum, sizeof(scene.maximum));
			}

			const auto tome = umbra_tome::build_single_cell(scene.minimum, scene.maximum, objects);
			if (tome.empty())
			{
				ZONETOOL_WARNING("gfxworld \"%s\": %zu umbra objects cannot be represented", world->name, objects.size());
				return;
			}

			world->umbraTomeSize = static_cast<unsigned int>(tome.size());
			world->umbraTomeData = allocator.allocate_array<char>(tome.size());
			std::memcpy(world->umbraTomeData, tome.data(), tome.size());
		}

		target::GfxWorld* convert(const GfxWorld* asset, world_geometry& geometry, utils::memory::allocator& allocator)
		{
			auto* world = allocator.allocate<target::GfxWorld>();
			world->name = asset->name;
			world->baseName = asset->baseName;
			world->bspVersion = 159;

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

			convert_planes(asset, world, allocator);
			convert_lightmaps(asset, world, allocator);
			convert_surfaces(asset, geometry, world, allocator);
			convert_reflection_probes(asset, world, allocator);
			const auto smodel_ranges = convert_static_models(asset, world, allocator);

			world->draw.iesLookupTexture = make_named<target::GfxImage>(asset->draw.iesLookupTexture, allocator);
			world->draw.transientZoneCount = 1;
			world->draw.transientZones[0] = convert_transient_zone(asset, geometry, smodel_ranges, allocator);
			world->draw.indexCount = static_cast<unsigned int>(geometry.indices.size());
			world->draw.indices = geometry.indices.data();

			convert_light_grid(asset, world, smodel_ranges, allocator);
			convert_lights(asset, world, smodel_ranges, allocator);
			convert_voxel_trees(asset, world, allocator);
			convert_heightfields(asset, world, allocator);
			convert_models(asset, world, allocator);

			world->bounds = std::bit_cast<target::Bounds>(asset->bounds);
			world->checksum = asset->checksum;
			world->sun = std::bit_cast<target::sunflare_t>(asset->sun);
			world->sun.hasValidData = false; // TODO: the sun flare materials are not converted
			world->sun.spriteMaterial = nullptr;
			world->sun.flareMaterial = nullptr;
			std::memcpy(world->outdoorLookupMatrix, asset->outdoorLookupMatrix, sizeof(world->outdoorLookupMatrix));
			world->outdoorImage = make_named<target::GfxImage>(asset->outdoorImage, allocator);
			world->materialLod0SizeThreshold = asset->materialLod0SizeThreshold;

			convert_dynamic_data(world, allocator);
			build_umbra_tome(world, allocator);
			return world;
		}

		void dump_materials(const GfxWorld* asset)
		{
			std::unordered_set<std::string_view> failed;
			for (auto i = 0u; i < asset->surfaces.count; i++)
			{
				const auto* handle = asset->surfaces.surfaces[i].material;
				if (!handle || failed.contains(handle->name))
				{
					continue;
				}

				const auto material = static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, handle->name));
				if (!material || !material::dump(material))
				{
					ZONETOOL_WARNING("Material \"%s\" of gfxworld \"%s\" is not converted", handle->name, asset->name);
					failed.emplace(handle->name);
				}
			}
		}

		bool has_transient_zones(const GfxWorld* asset)
		{
			if (!asset->draw.transientZoneCount)
			{
				return false;
			}

			for (auto i = 0u; i < asset->draw.transientZoneCount; i++)
			{
				if (!asset->draw.transientZones[i])
				{
					return false;
				}
			}

			return true;
		}
	}

	void dump(GfxWorld* asset)
	{
		if (!has_transient_zones(asset))
		{
			ZONETOOL_ERROR("gfxworld \"%s\" has no loaded transient zones", asset->name);
			return;
		}

		utils::memory::allocator allocator;
		auto geometry = build_geometry(asset);
		auto* world = convert(asset, geometry, allocator);

		dump_materials(asset);
		target::gfx_world::dump(world);
		target::gfx_world_tr::dump(world->draw.transientZones[0]);
		add_csv_row("gfx_map_trzone", world->draw.transientZones[0]->name);
		add_csv_row("gfx_map", asset->name);
	}
}
