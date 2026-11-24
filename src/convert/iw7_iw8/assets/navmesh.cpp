#include <std_include.hpp>

#include "../include.hpp"
#include "navmesh.hpp"

#include <iw8/assets/navmesh.hpp>

namespace zonetool::iw7::converter::iw8::navmesh
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename T>
		T read_at(const std::uint8_t* data, const std::size_t offset)
		{
			T value{};
			std::memcpy(&value, data + offset, sizeof(T));
			return value;
		}

		template <typename T>
		void append(std::vector<std::uint8_t>& out, const T& value)
		{
			const auto* bytes = reinterpret_cast<const std::uint8_t*>(&value);
			out.insert(out.end(), bytes, bytes + sizeof(T));
		}

		template <typename T>
		void write_at(std::vector<std::uint8_t>& out, const std::size_t offset, const T& value)
		{
			std::memcpy(out.data() + offset, &value, sizeof(T));
		}

		void add_bounds(target::bfx_box& total, const bfx_box& bounds)
		{
			for (auto i = 0; i < 3; i++)
			{
				total.mins[i] = std::min(total.mins[i], bounds.mins[i]);
				total.maxs[i] = std::max(total.maxs[i], bounds.maxs[i]);
			}
		}

		std::uint32_t convert_edge_flags(const std::uint32_t flags, const char* name)
		{
			const auto index = flags >> 16;
			if (index == 0xFFFF)
			{
				return 0xFFFC0000 | (flags & 0xFFFF);
			}

			if (index > 0x3FFF)
			{
				ZONETOOL_WARNING("navmesh \"%s\": edge index %u does not fit in 14 bits", name, index);
			}

			return ((index & 0x3FFF) << 18) | (flags & 0xFFFF);
		}

		void convert_areas(const std::uint8_t* src, std::size_t offset, const std::size_t end, const std::uint32_t delta,
			std::vector<std::uint8_t>& out, const char* name)
		{
			while (offset < end)
			{
				const auto area = read_at<bfx_area>(src, offset);
				target::bfx_area converted{};
				std::memcpy(&converted, &area, sizeof(converted));
				append(out, converted);
				offset += sizeof(bfx_area);

				const auto edge_count = area.edge_info & 0x7F;
				for (auto i = 0u; i < edge_count; i++)
				{
					const auto edge = read_at<bfx_area_edge>(src, offset);
					target::bfx_area_edge converted_edge{};
					converted_edge.neighbor = edge.neighbor ? edge.neighbor + delta : 0;
					std::memcpy(converted_edge.vertex, edge.vertex, sizeof(converted_edge.vertex));
					converted_edge.flags = convert_edge_flags(edge.flags, name);
					converted_edge.portal = edge.portal;
					converted_edge.unknown_1c = edge.unknown_1c;
					append(out, converted_edge);
					offset += sizeof(bfx_area_edge);
				}
			}
		}

		bool convert_kd_tree(const std::uint8_t* src, std::size_t offset, const std::uint32_t tree_bytes, const std::uint32_t delta,
			std::vector<std::uint8_t>& out, const char* name)
		{
			const auto header = read_at<bfx_kd_tree_header>(src, offset);
			if (sizeof(bfx_kd_tree_header) + header.node_bytes != tree_bytes)
			{
				ZONETOOL_WARNING("navmesh \"%s\": kd-tree size %u does not match its nodes", name, tree_bytes);
				return false;
			}

			target::bfx_kd_tree_header converted{};
			std::memcpy(&converted.bounds, &header.bounds, sizeof(converted.bounds));
			converted.node_bytes = header.node_bytes;
			append(out, converted);
			offset += sizeof(bfx_kd_tree_header);

			const auto end = offset + header.node_bytes;
			while (offset < end)
			{
				const auto info = read_at<std::uint32_t>(src, offset);
				if (info & BFX_KD_TREE_LEAF)
				{
					append(out, static_cast<std::uint32_t>(target::BFX_KD_TREE_LEAF | ((info & ~BFX_KD_TREE_LEAF) + delta)));
					offset += sizeof(std::uint32_t);
					continue;
				}

				const auto split = read_at<bfx_kd_tree_split>(src, offset);
				append(out, target::bfx_kd_tree_split{split.info, split.left_max, split.right_min});
				offset += sizeof(bfx_kd_tree_split);
			}

			return offset == end;
		}

		bool convert_graph_2d(const std::uint8_t* src, const std::size_t size, std::vector<std::uint8_t>& out, target::bfx_box& bounds,
			const char* name)
		{
			const auto graph = read_at<bfx_graph_header>(src, 0);
			if (graph.version != 0x2B)
			{
				ZONETOOL_WARNING("navmesh \"%s\": 2D graph version %u is not 43", name, graph.version);
				return false;
			}

			append(out, target::bfx_graph_header{graph.endian, 0x2E, graph.layer_count});

			constexpr auto delta = static_cast<std::uint32_t>(sizeof(target::bfx_layer_header) - sizeof(bfx_layer_header));

			std::size_t offset = sizeof(bfx_graph_header);
			for (auto i = 0u; i < graph.layer_count; i++)
			{
				const auto layer = read_at<bfx_layer_header>(src, offset);
				if (layer.record_bytes || layer.link_count)
				{
					ZONETOOL_WARNING("navmesh \"%s\": layer %u has records or links, which are not converted", name, i);
					return false;
				}

				if (offset + layer.layer_bytes > size ||
					sizeof(bfx_layer_header) + layer.area_bytes + layer.kd_tree_bytes != layer.layer_bytes)
				{
					ZONETOOL_WARNING("navmesh \"%s\": layer %u has a bad size", name, i);
					return false;
				}

				target::bfx_layer_header converted{};
				converted.version = 0x2E;
				converted.index = layer.index;
				converted.area_bytes = layer.area_bytes;
				converted.kd_tree_bytes = layer.kd_tree_bytes;
				converted.record_bytes = layer.record_bytes;
				converted.unknown_14 = layer.unknown_14;
				converted.link_stride = layer.link_stride;
				converted.link_count = layer.link_count;
				converted.layer_bytes = layer.layer_bytes + delta;
				std::memcpy(converted.agent_params, layer.agent_params, sizeof(converted.agent_params));
				std::memcpy(&converted.bounds, &layer.bounds, sizeof(converted.bounds));
				converted.unknown_54 = layer.unknown_50;
				std::memcpy(converted.unknown_58, layer.unknown_54, sizeof(converted.unknown_58));
				append(out, converted);
				add_bounds(bounds, layer.bounds);

				const auto areas = offset + sizeof(bfx_layer_header);
				convert_areas(src, areas, areas + layer.area_bytes, delta, out, name);

				if (!convert_kd_tree(src, areas + layer.area_bytes, layer.kd_tree_bytes, delta, out, name))
				{
					return false;
				}

				offset += layer.layer_bytes;
			}

			return offset == size;
		}

		std::uint32_t compute_checksum(const std::uint8_t* body, const std::size_t size)
		{
			std::uint32_t sum = 0;
			for (std::size_t offset = 0; offset + 4 <= size; offset += 4)
			{
				sum += read_at<std::uint32_t>(body, offset);
			}

			return sum;
		}

		std::optional<std::vector<std::uint8_t>> convert_graph(const nav_resource_s& resource, target::bfx_box& bounds, const char* name)
		{
			const auto* src = reinterpret_cast<const std::uint8_t*>(resource.pGraphBuffer);
			const auto size = static_cast<std::size_t>(resource.graphSize);
			if (!src || size < sizeof(bfx_resource_header) + sizeof(bfx_section_header))
			{
				return {};
			}

			const auto header = read_at<bfx_resource_header>(src, 0);
			if (header.endian || header.version != 2 || header.body_size != size - sizeof(bfx_resource_header))
			{
				ZONETOOL_WARNING("navmesh \"%s\": resource header is not valid", name);
				return {};
			}

			std::vector<std::uint8_t> out(sizeof(target::bfx_resource_header));

			std::size_t offset = sizeof(bfx_resource_header);
			while (offset < size)
			{
				const auto section = read_at<bfx_section_header>(src, offset);
				const auto data = offset + sizeof(bfx_section_header);
				if (section.type != BFX_SECTION_GRAPH_2D || data + section.size > size)
				{
					ZONETOOL_WARNING("navmesh \"%s\": section type 0x%X is not converted", name, section.type);
					return {};
				}

				const auto section_offset = out.size();
				append(out, target::bfx_section_header{});

				std::vector<std::uint8_t> graph;
				if (!convert_graph_2d(src + data, section.size, graph, bounds, name))
				{
					return {};
				}

				out.insert(out.end(), graph.begin(), graph.end());
				write_at(out, section_offset, target::bfx_section_header{target::BFX_SECTION_GRAPH_2D, static_cast<std::uint32_t>(graph.size()), section.count});
				offset = data + section.size;
			}

			const auto body_size = out.size() - sizeof(target::bfx_resource_header);

			target::bfx_resource_header converted{};
			converted.version = 2;
			converted.body_size = static_cast<std::uint32_t>(body_size);
			converted.checksum = compute_checksum(out.data() + sizeof(target::bfx_resource_header), body_size);
			converted.flags = header.flags;
			write_at(out, 0, converted);
			return out;
		}

		template <typename T, typename S>
		T* copy_array(const S* source, const int count, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(T) == sizeof(S));

			if (!source || count <= 0)
			{
				return nullptr;
			}

			auto* result = allocator.allocate_array<T>(count);
			std::memcpy(result, source, sizeof(T) * count);
			return result;
		}

		target::nav_resource_s* convert_resources(NavMeshData* asset, int& count, target::bfx_box& bounds, utils::memory::allocator& allocator)
		{
			count = 0;
			if (asset->numNavResources <= 0)
			{
				return nullptr;
			}

			auto* resources = allocator.allocate_array<target::nav_resource_s>(asset->numNavResources);
			for (auto i = 0; i < asset->numNavResources; i++)
			{
				const auto& resource = asset->navResources[i];
				if (resource.bIsVolume)
				{
					ZONETOOL_WARNING("navmesh \"%s\": volume resource %d is not converted", asset->name, i);
					continue;
				}

				const auto graph = convert_graph(resource, bounds, asset->name);
				if (!graph)
				{
					ZONETOOL_WARNING("navmesh \"%s\": resource %d is not converted", asset->name, i);
					continue;
				}

				auto& converted = resources[count++];
				converted.targetName = static_cast<target::scr_string_t>(resource.targetName);
				converted.modelIdx = resource.modelIdx;
				converted.targetEntNum = resource.targetEntNum;
				converted.graphSize = static_cast<int>(graph->size());
				std::memcpy(converted.localOffsetPos, resource.localOffsetPos, sizeof(converted.localOffsetPos));
				std::memcpy(converted.localOffsetRot, resource.localOffsetRot, sizeof(converted.localOffsetRot));
				converted.offsetLayer = resource.offsetLayer;
				converted.layerFlags = resource.layerFlags;
				converted.bDockable = resource.bDockable;
				converted.pGraphBuffer = copy_array<char>(reinterpret_cast<const char*>(graph->data()), converted.graphSize, allocator);
				if (converted.bDockable)
				{
					converted.pWorkingGraph = copy_array<char>(reinterpret_cast<const char*>(graph->data()), converted.graphSize, allocator);
				}
			}

			return count ? resources : nullptr;
		}

		target::nav_obstacle_bounds_s* convert_obstacle_bounds(NavMeshData* asset, utils::memory::allocator& allocator)
		{
			if (!asset->obstacleBounds || asset->numObstacleBounds <= 0)
			{
				return nullptr;
			}

			auto* result = allocator.allocate_array<target::nav_obstacle_bounds_s>(asset->numObstacleBounds);
			for (auto i = 0; i < asset->numObstacleBounds; i++)
			{
				const auto& bounds = asset->obstacleBounds[i];
				result[i].m_NumHulls = bounds.m_NumHulls;
				if (!bounds.m_Hulls || bounds.m_NumHulls <= 0)
				{
					continue;
				}

				result[i].m_Hulls = allocator.allocate_array<target::nav_obstacle_hull_s>(bounds.m_NumHulls);
				for (auto j = 0; j < bounds.m_NumHulls; j++)
				{
					const auto& hull = bounds.m_Hulls[j];
					auto& converted = result[i].m_Hulls[j];
					converted.m_Boundaries = copy_array<target::nav_boundary_plane_s>(hull.m_Boundaries, hull.m_NumBoundaries, allocator);
					converted.m_NumBoundaries = hull.m_NumBoundaries;
					converted.m_BoundsX = hull.m_BoundsX;
					converted.m_BoundsY = hull.m_BoundsY;
					converted.m_BoundsZ = hull.m_BoundsZ;
					std::memcpy(converted.m_Offset, hull.m_Offset, sizeof(converted.m_Offset));
					converted.m_bUseBounds = hull.m_bUseBounds;
				}
			}

			return result;
		}

		target::nav_glass_bounds_s* convert_glass_bounds(NavMeshData* asset, utils::memory::allocator& allocator)
		{
			if (!asset->glassBounds || asset->numGlassBounds <= 0)
			{
				return nullptr;
			}

			auto* result = allocator.allocate_array<target::nav_glass_bounds_s>(asset->numGlassBounds);
			for (auto i = 0; i < asset->numGlassBounds; i++)
			{
				const auto& glass = asset->glassBounds[i];
				result[i].m_Boundaries = copy_array<target::nav_boundary_plane_s>(glass.m_Boundaries, glass.m_NumBoundaries, allocator);
				result[i].m_NumBoundaries = glass.m_NumBoundaries;
				std::memcpy(result[i].m_Pos, glass.m_Pos, sizeof(result[i].m_Pos));
				result[i].m_GlassPieceIndex = glass.m_GlassPieceIndex;
			}

			return result;
		}

		target::nav_modifier_s* convert_modifiers(NavMeshData* asset, utils::memory::allocator& allocator)
		{
			if (!asset->modifiers || asset->numModifiers <= 0)
			{
				return nullptr;
			}

			auto* result = allocator.allocate_array<target::nav_modifier_s>(asset->numModifiers);
			for (auto i = 0; i < asset->numModifiers; i++)
			{
				const auto& modifier = asset->modifiers[i];
				auto& converted = result[i];
				converted.m_Boundaries = copy_array<target::nav_boundary_plane_s>(modifier.m_Boundaries, modifier.m_NumBoundaries, allocator);
				converted.m_NumBoundaries = modifier.m_NumBoundaries;
				std::memcpy(converted.m_Pos, modifier.m_Pos, sizeof(converted.m_Pos));
				converted.m_TargetName = static_cast<target::scr_string_t>(modifier.m_TargetName);
				converted.m_UserFlags = modifier.m_UserFlags;
				converted.m_LayerFlags = modifier.m_LayerFlags;
				converted.m_Weight = modifier.m_Weight;
				converted.m_bActiveOnLoad = modifier.m_bActiveOnLoad;
			}

			return result;
		}

		target::nav_link_creation_data_s* convert_link_creation_data(NavMeshData* asset, utils::memory::allocator& allocator)
		{
			if (!asset->linkCreationData || asset->numLinkCreationData <= 0)
			{
				return nullptr;
			}

			auto* result = allocator.allocate_array<target::nav_link_creation_data_s>(asset->numLinkCreationData);
			for (auto i = 0; i < asset->numLinkCreationData; i++)
			{
				const auto& link = asset->linkCreationData[i];
				auto& converted = result[i];
				std::memcpy(&converted.m_Start, &link.m_Start, sizeof(converted.m_Start));
				std::memcpy(&converted.m_End, &link.m_End, sizeof(converted.m_End));
				converted.m_Animscript = static_cast<target::scr_string_t>(link.m_Animscript);
				converted.m_Target = static_cast<target::scr_string_t>(link.m_Target);
				converted.m_Parent = static_cast<target::scr_string_t>(link.m_Parent);
				converted.m_UsageFlags = link.m_UsageFlags;
				converted.m_PenaltyMult = link.m_PenaltyMult;
				converted.m_bBidirectional = link.m_bBidirectional;
			}

			return result;
		}

		target::nav_raw_custom_volume_s* convert_raw_custom_volumes(NavMeshData* asset, utils::memory::allocator& allocator)
		{
			if (!asset->rawCustomVolumes || asset->numRawCustomVolumes <= 0)
			{
				return nullptr;
			}

			auto* result = allocator.allocate_array<target::nav_raw_custom_volume_s>(asset->numRawCustomVolumes);
			for (auto i = 0; i < asset->numRawCustomVolumes; i++)
			{
				const auto& volume = asset->rawCustomVolumes[i];
				result[i].m_Tris = copy_array<target::nav_raw_custom_volume_tri_s>(volume.m_Tris, volume.m_NumTris, allocator);
				result[i].m_NumTris = volume.m_NumTris;
			}

			return result;
		}

		target::NavMeshData* convert(NavMeshData* asset, utils::memory::allocator& allocator)
		{
			auto* result = allocator.allocate<target::NavMeshData>();
			result->name = asset->name;
			result->version = 19; // minimum (0x10EB880)
			result->hasExposureData = asset->hasExposureData;

			target::bfx_box bounds{{FLT_MAX, FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX, -FLT_MAX}};
			result->navResources = convert_resources(asset, result->numNavResources, bounds, allocator);

			result->numObstacleBounds = asset->numObstacleBounds;
			result->obstacleBounds = convert_obstacle_bounds(asset, allocator);
			result->numGlassBounds = asset->numGlassBounds;
			result->glassBounds = convert_glass_bounds(asset, allocator);
			result->numModifiers = asset->numModifiers;
			result->modifiers = convert_modifiers(asset, allocator);
			result->numLinkCreationData = asset->numLinkCreationData;
			result->linkCreationData = convert_link_creation_data(asset, allocator);
			result->numVolumeSeeds = asset->numVolumeSeeds;
			result->volumeSeeds = copy_array<vec3_t>(asset->volumeSeeds, asset->numVolumeSeeds, allocator);
			result->numRawVolumes = asset->numRawVolumes;
			result->rawVolumes = copy_array<target::nav_raw_volume_s>(asset->rawVolumes, asset->numRawVolumes, allocator);
			result->numRawCustomVolumes = asset->numRawCustomVolumes;
			result->rawCustomVolumes = convert_raw_custom_volumes(asset, allocator);

			if (result->numNavResources)
			{
				for (auto i = 0; i < 3; i++)
				{
					result->miscData.m_StaticBoundsHalfSize[i] = (bounds.maxs[i] - bounds.mins[i]) * 0.5f;
					result->miscData.m_StaticBoundsMidPoint[i] = (bounds.maxs[i] + bounds.mins[i]) * 0.5f;
				}
			}

			return result;
		}
	}

	void dump(NavMeshData* asset)
	{
		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		target::nav_mesh::dump(converted);
		add_csv_row("navmesh", asset->name);
	}
}
