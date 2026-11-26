#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "light_grid.hpp"

#include <formats/probe_volume.hpp>
#include <shared.hpp>

#include <DirectXPackedVector.h>

namespace zonetool::t9::converter::iw8::light_grid
{
	namespace
	{
		namespace target = zonetool::iw8;

		using vec3 = std::array<float, 3>;

		bool is_unset_point(const LightingGridPoint& point)
		{
			return point.sh[0] == 0x3C00 && point.sh[1] == 0xBAE5 && point.sh[2] == 0 && point.sh[3] == 0x36E5 && point.color == 0x3FFFFFFF;
		}

		sh_probe decode_point(const LightingGridPoint& point)
		{
			float intensity[4]{};
			for (auto k = 0; k < 4; k++)
			{
				const auto value = half_float::half_to_float(point.sh[k]);
				intensity[k] = std::isfinite(value) ? value : 0.0f;
			}

			const float color[3] =
			{
				static_cast<float>(point.color & 0x3FF) / 1023.0f,
				static_cast<float>((point.color >> 10) & 0x3FF) / 1023.0f,
				static_cast<float>((point.color >> 20) & 0x3FF) / 1023.0f,
			};

			sh_probe result{};
			for (auto channel = 0; channel < 3; channel++)
			{
				for (auto k = 0; k < 4; k++)
				{
					result[channel * 9 + k] = intensity[k] * color[channel];
				}
			}

			return result;
		}

		vec3 to_grid_space(const LightingGrid& grid, const vec3& point)
		{
			const auto yaw = grid.angles[1] * std::numbers::pi_v<float> / 180.0f;
			const auto cos_yaw = std::cos(yaw);
			const auto sin_yaw = std::sin(yaw);
			const auto x = point[0] - grid.origin[0];
			const auto y = point[1] - grid.origin[1];
			return {cos_yaw * x + sin_yaw * y, cos_yaw * y - sin_yaw * x, point[2] - grid.origin[2]};
		}

		float box_distance_squared(const LightingGrid& grid, const vec3& local)
		{
			auto result = 0.0f;
			for (auto a = 0; a < 3; a++)
			{
				const auto outside = std::max(std::abs(local[a]) - grid.halfSize[a], 0.0f);
				result += outside * outside;
			}

			return result;
		}

		float grid_volume(const LightingGrid& grid)
		{
			return grid.halfSize[0] * grid.halfSize[1] * grid.halfSize[2];
		}

		const LightingGridPoint& grid_point(const LightingGrid& grid, const unsigned int x, const unsigned int y, const unsigned int z)
		{
			return grid.points[(static_cast<std::size_t>(z) * grid.sizeY + y) * grid.sizeX + x];
		}

		float grid_point_coordinate(const LightingGrid& grid, const int axis, const unsigned int index, const unsigned int size)
		{
			return -grid.halfSize[axis] + (index + 0.5f) * 2.0f * grid.halfSize[axis] / size;
		}

		float interpolate_grid(const LightingGrid& grid, const vec3& local, sh_probe& result)
		{
			const unsigned int size[3] = {grid.sizeX, grid.sizeY, grid.sizeZ};
			unsigned int base[3]{};
			float fraction[3]{};
			for (auto a = 0; a < 3; a++)
			{
				const auto extent = std::max(grid.halfSize[a] * 2.0f, 1.0f);
				const auto cell = std::clamp((local[a] + grid.halfSize[a]) / extent * size[a] - 0.5f, 0.0f, size[a] - 1.0f);
				base[a] = std::min(static_cast<unsigned int>(cell), size[a] - 1);
				fraction[a] = cell - base[a];
			}

			result = {};
			auto total = 0.0f;
			for (auto corner = 0u; corner < 8; corner++)
			{
				unsigned int index[3]{};
				auto weight = 1.0f;
				for (auto a = 0; a < 3; a++)
				{
					const auto upper = (corner >> a) & 1;
					index[a] = std::min(base[a] + upper, size[a] - 1);
					weight *= upper ? fraction[a] : 1.0f - fraction[a];
				}

				const auto& point = grid_point(grid, index[0], index[1], index[2]);
				if (weight <= 0.0f || is_unset_point(point))
				{
					continue;
				}

				const auto probe = decode_point(point);
				for (auto i = 0u; i < result.size(); i++)
				{
					result[i] += probe[i] * weight;
				}

				total += weight;
			}

			if (total > 0.0f)
			{
				for (auto& value : result)
				{
					value /= total;
				}
			}

			return total;
		}

		void find_nearest_point(const LightingGrid& grid, const vec3& local, sh_probe& result, float& distance_squared)
		{
			for (auto z = 0u; z < grid.sizeZ; z++)
			{
				for (auto y = 0u; y < grid.sizeY; y++)
				{
					for (auto x = 0u; x < grid.sizeX; x++)
					{
						const auto& point = grid_point(grid, x, y, z);
						if (is_unset_point(point))
						{
							continue;
						}

						const auto dx = grid_point_coordinate(grid, 0, x, grid.sizeX) - local[0];
						const auto dy = grid_point_coordinate(grid, 1, y, grid.sizeY) - local[1];
						const auto dz = grid_point_coordinate(grid, 2, z, grid.sizeZ) - local[2];
						const auto distance = dx * dx + dy * dy + dz * dz;
						if (distance < distance_squared)
						{
							distance_squared = distance;
							result = decode_point(point);
						}
					}
				}
			}
		}

		bool is_usable(const LightingGrid& grid)
		{
			return grid.points && grid.sizeX && grid.sizeY && grid.sizeZ;
		}

		sh_probe sample_probe(const LightingGridSet& set, const vec3& point, probe_sample_counts& counts)
		{
			sh_probe best{};
			auto best_weight = 0.0f;
			auto best_volume = std::numeric_limits<float>::max();
			std::vector<std::pair<float, unsigned int>> nearest_order;
			nearest_order.reserve(set.gridCount);

			for (auto i = 0u; i < set.gridCount; i++)
			{
				const auto& grid = set.grids[i];
				if (!is_usable(grid))
				{
					continue;
				}

				const auto local = to_grid_space(grid, point);
				const auto distance = box_distance_squared(grid, local);
				nearest_order.emplace_back(distance, i);
				if (distance > 0.0f)
				{
					continue;
				}

				sh_probe probe{};
				const auto weight = interpolate_grid(grid, local, probe);
				const auto volume = grid_volume(grid);
				if (weight > best_weight + 0.0001f || (weight > 0.0f && std::abs(weight - best_weight) <= 0.0001f && volume < best_volume))
				{
					best = probe;
					best_weight = weight;
					best_volume = volume;
				}
			}

			if (best_weight > 0.0f)
			{
				counts.interpolated++;
				return best;
			}

			std::sort(nearest_order.begin(), nearest_order.end());
			auto best_distance = std::numeric_limits<float>::max();
			for (const auto& [distance, index] : nearest_order)
			{
				if (distance >= best_distance)
				{
					break;
				}

				const auto& grid = set.grids[index];
				find_nearest_point(grid, to_grid_space(grid, point), best, best_distance);
			}

			if (best_distance == std::numeric_limits<float>::max())
			{
				counts.missing++;
				return {};
			}

			counts.nearest++;
			return best;
		}

		const LightingGridSet* find_grid_set(const Lighting* lighting)
		{
			if (!lighting || !lighting->volumes)
			{
				return nullptr;
			}

			const LightingGridSet* result = nullptr;
			auto set_count = 0u;
			for (auto v = 0u; v < lighting->volumeCount; v++)
			{
				for (const auto& set : lighting->volumes[v].gridSets)
				{
					if (!set.gridCount || !set.grids)
					{
						continue;
					}

					set_count++;
					if (!result)
					{
						result = &set;
					}
				}
			}

			if (set_count > 1)
			{
				ZONETOOL_WARNING("lighting has %u light grid sets, only the first set is converted", set_count);
			}

			return result;
		}

		void log_grid_set(const LightingGridSet& set)
		{
			auto point_count = 0ull;
			auto unset_count = 0ull;
			auto tilted_count = 0u;
			for (auto i = 0u; i < set.gridCount; i++)
			{
				const auto& grid = set.grids[i];
				if (!is_usable(grid))
				{
					continue;
				}

				const auto count = static_cast<std::size_t>(grid.sizeX) * grid.sizeY * grid.sizeZ;
				point_count += count;
				for (auto p = 0u; p < count; p++)
				{
					unset_count += is_unset_point(grid.points[p]);
				}

				tilted_count += grid.angles[0] != 0.0f || grid.angles[2] != 0.0f;
			}

			ZONETOOL_INFO("light grid: %u grids, %llu probes, %llu unset probes", set.gridCount, point_count, unset_count);
			if (tilted_count)
			{
				ZONETOOL_WARNING("light grid: %u grids have pitch or roll, only the yaw is applied", tilted_count);
			}
		}

		std::uint32_t pack_r11g11b10(const float r, const float g, const float b)
		{
			DirectX::PackedVector::XMFLOAT3PK packed{};
			DirectX::PackedVector::XMStoreFloat3PK(&packed, DirectX::XMVectorSet(r, g, b, 0.0f));
			return packed.v;
		}

		vec3 unpack_r11g11b10(const std::uint32_t value)
		{
			DirectX::PackedVector::XMFLOAT3PK packed{};
			packed.v = value;
			DirectX::XMFLOAT3 result{};
			DirectX::XMStoreFloat3(&result, DirectX::PackedVector::XMLoadFloat3PK(&packed));
			return {result.x, result.y, result.z};
		}

		unsigned char pack_unit_byte(const float value)
		{
			return static_cast<unsigned char>(std::lround(std::clamp(value, 0.0f, 1.0f) * 255.0f));
		}

		float sh_ratio(const sh_probe& probe, const vec3& l0, const unsigned int channel, const unsigned int coefficient)
		{
			const auto band_scale = coefficient < 4 ? 1.0f / std::sqrt(3.0f) : 1.0f / std::sqrt(5.0f);
			const auto ratio = l0[channel] > 0.0f ? probe[channel * 9 + coefficient] / l0[channel] * band_scale : 0.0f;
			return std::clamp(ratio, -1.0f, 1.0f);
		}

		void write_volume_texel(const sh_probe& probe, const unsigned int x, const unsigned int y, const unsigned int z,
			const unsigned int width, std::uint32_t* l0_pixels, unsigned char* sh_pixels)
		{
			const auto l0 = pack_r11g11b10(probe[0], probe[9], probe[18]);
			l0_pixels[(z * width + y) * width + x] = l0;

			const auto unpacked = unpack_r11g11b10(l0);
			const unsigned int coefficient_order[8] = {2, 6, 1, 3, 7, 5, 8, 4};
			for (auto slot = 0u; slot < 24; slot++)
			{
				const auto tile = slot / 4;
				const auto channel = slot % 3;
				const auto coefficient = coefficient_order[slot / 3];
				const auto texel_x = (tile % 2) * width + x;
				const auto texel_y = (tile / 2) * width + y;
				const auto texel = (z * width * 3 + texel_y) * width * 2 + texel_x;
				sh_pixels[texel * 4 + slot % 4] = pack_unit_byte(sh_ratio(probe, unpacked, channel, coefficient) * 0.5f + 0.5f);
			}
		}

		target::GfxImageRaw make_volume_texture(const target::GfxPixelFormat format, const unsigned int width, const unsigned int height,
			const unsigned int depth, utils::memory::allocator& allocator)
		{
			target::GfxImageRaw image{};
			image.desc.format = format;
			image.desc.flags = static_cast<target::GfxImageFlags>(target::IMG_CREATE_FLAG_RW_VIEW | target::IMG_DISK_FLAG_MAPTYPE_3D
				| target::IMG_DISK_FLAG_NOMIPMAPS | target::IMG_DISK_FLAG_NOPICMIP);
			image.desc.totalSize = width * height * depth * 4;
			image.desc.width = static_cast<unsigned short>(width);
			image.desc.height = static_cast<unsigned short>(height);
			image.desc.depth = static_cast<unsigned short>(depth);
			image.desc.numElements = 1;
			image.desc.levelCount = 1;
			image.pixels = allocator.allocate_array<char>(image.desc.totalSize);
			return image;
		}

		void write_volume_transform(const gfxworld::placement_transform& transform, const light_grid_volume& volume, target::vec4_t* rows)
		{
			for (auto a = 0; a < 3; a++)
			{
				const auto& axis = transform.axis[a];
				const auto origin = axis[0] * transform.origin[0] + axis[1] * transform.origin[1] + axis[2] * transform.origin[2];
				for (auto i = 0; i < 3; i++)
				{
					rows[a][i] = axis[i] / (transform.scale * volume.box_size[a]);
				}

				rows[a][3] = -(origin / transform.scale + volume.box_min[a]) / volume.box_size[a];
			}
		}

		vec3 volume_point(const gfxworld::placed_model& model, const light_grid_volume& volume, const unsigned int x, const unsigned int y,
			const unsigned int z)
		{
			const unsigned int cell[3] = {x, y, z};
			vec3 local{};
			for (auto a = 0; a < 3; a++)
			{
				const auto fraction = volume.dimensions[a] > 1 ? static_cast<float>(cell[a]) / (volume.dimensions[a] - 1) : 0.5f;
				local[a] = volume.box_min[a] + fraction * volume.box_size[a];
			}

			return gfxworld::transform_point(model.transform, local);
		}

		unsigned int pack_volumes(std::vector<light_grid_volume>& volumes, const unsigned int width)
		{
			std::vector<unsigned int> order(volumes.size());
			std::iota(order.begin(), order.end(), 0u);
			std::stable_sort(order.begin(), order.end(), [&](const unsigned int left, const unsigned int right)
			{
				const auto& a = volumes[left].dimensions;
				const auto& b = volumes[right].dimensions;
				return std::tie(a[2], a[1], a[0]) > std::tie(b[2], b[1], b[0]);
			});

			auto x = 0u, y = 0u, z = 0u;
			auto row_height = 0u, layer_depth = 0u;
			for (const auto index : order)
			{
				auto& volume = volumes[index];
				if (x + volume.dimensions[0] > width)
				{
					x = 0;
					y += row_height;
					row_height = 0;
				}

				if (y + volume.dimensions[1] > width)
				{
					x = 0;
					y = 0;
					z += layer_depth;
					row_height = 0;
					layer_depth = 0;
				}

				volume.start[0] = x;
				volume.start[1] = y;
				volume.start[2] = z;
				x += volume.dimensions[0];
				row_height = std::max(row_height, volume.dimensions[1]);
				layer_depth = std::max(layer_depth, volume.dimensions[2]);
			}

			return z + layer_depth;
		}

		std::vector<light_grid_volume> make_volumes(const std::vector<gfxworld::placed_model>& models)
		{
			std::vector<light_grid_volume> volumes(models.size());
			for (auto i = 0u; i < models.size(); i++)
			{
				const auto& bounds = models[i].model->bounds;
				for (auto a = 0; a < 3; a++)
				{
					volumes[i].box_size[a] = std::max(bounds.halfSize[a] * 2.0f, 1.0f);
					volumes[i].box_min[a] = bounds.midPoint[a] - volumes[i].box_size[a] * 0.5f;
				}
			}

			return volumes;
		}

		target::GfxSHCompressedProbeData compress_probe(const sh_probe& probe)
		{
			target::GfxSHCompressedProbeData result{};
			result.l0 = pack_r11g11b10(probe[0], probe[9], probe[18]);
			const auto l0 = unpack_r11g11b10(result.l0);
			for (auto channel = 0u; channel < 3; channel++)
			{
				for (auto coefficient = 1u; coefficient < 9; coefficient++)
				{
					result.shRatios[channel][coefficient - 1] = static_cast<unsigned char>(sh_ratio(probe, l0, channel, coefficient) * 127.0f + 127.5f);
				}
			}

			result.skyVisibility = 255;
			result.gpuVisibility = 255;
			return result;
		}

		target::GfxSHProbeData pack_fallback_probe(const sh_probe& probe)
		{
			target::GfxSHProbeData result{};
			for (auto i = 0u; i < probe.size(); i++)
			{
				result.coeffs[i] = half_float::float_to_half(probe[i]);
			}

			result.coeffs[27] = half_float::float_to_half(1.0f); // sun visibility
			result.coeffs[28] = half_float::float_to_half(1.0f);
			return result;
		}

		float grid_radius(const LightingGrid& grid)
		{
			return std::sqrt(grid.halfSize[0] * grid.halfSize[0] + grid.halfSize[1] * grid.halfSize[1] + grid.halfSize[2] * grid.halfSize[2]);
		}

		bool is_near_grid(const LightingGridSet& set, const float* cell_min, const float cell_size)
		{
			const auto reach = cell_size * 0.5f * std::sqrt(3.0f);
			const vec3 centre{cell_min[0] + cell_size * 0.5f, cell_min[1] + cell_size * 0.5f, cell_min[2] + cell_size * 0.5f};
			for (auto i = 0u; i < set.gridCount; i++)
			{
				const auto& grid = set.grids[i];
				if (!is_usable(grid))
				{
					continue;
				}

				const auto radius = grid_radius(grid) + reach;
				const auto dx = centre[0] - grid.origin[0];
				const auto dy = centre[1] - grid.origin[1];
				const auto dz = centre[2] - grid.origin[2];
				if (dx * dx + dy * dy + dz * dz <= radius * radius && box_distance_squared(grid, to_grid_space(grid, centre)) <= reach * reach)
				{
					return true;
				}
			}

			return false;
		}

		bool get_grid_set_bounds(const LightingGridSet& set, float* mins, float* maxs)
		{
			auto found = false;
			for (auto i = 0u; i < set.gridCount; i++)
			{
				const auto& grid = set.grids[i];
				if (!is_usable(grid))
				{
					continue;
				}

				const auto radius = grid_radius(grid);
				for (auto a = 0; a < 3; a++)
				{
					mins[a] = found ? std::min(mins[a], grid.origin[a] - radius) : grid.origin[a] - radius;
					maxs[a] = found ? std::max(maxs[a], grid.origin[a] + radius) : grid.origin[a] + radius;
				}

				found = true;
			}

			return found;
		}

		target::GfxVoxelTree* make_voxel_tree(const probe_volume::volume& volume, const target::Bounds& world_bounds, utils::memory::allocator& allocator)
		{
			auto* header = allocator.allocate<target::GfxVoxelTreeHeader>();
			auto* tree = allocator.allocate<target::GfxVoxelTree>();
			for (auto a = 0; a < 3; a++)
			{
				header->rootNodeDimension[a] = volume.root_node_dimension[a];
				header->nodeCoordBitShift[a] = volume.node_coord_bit_shift[a];
				header->boundMin[a] = volume.bound_min[a];
				header->boundMax[a] = volume.bound_max[a];
				const auto zone_min = std::min(volume.bound_min[a], world_bounds.midPoint[a] - world_bounds.halfSize[a]);
				const auto zone_max = std::max(volume.bound_max[a], world_bounds.midPoint[a] + world_bounds.halfSize[a]);
				tree->zoneBound.midPoint[a] = 0.5f * (zone_min + zone_max);
				tree->zoneBound.halfSize[a] = 0.5f * (zone_max - zone_min);
			}

			const auto top_down_count = static_cast<int>(volume.top_down_nodes.size());
			auto* top_down = allocator.allocate_array<target::GfxVoxelTopDownViewNode>(top_down_count);
			for (auto i = 0; i < top_down_count; i++)
			{
				const auto& node = volume.top_down_nodes[i];
				top_down[i] = {node.first_node_index, node.z_min, node.z_max};
			}

			const auto internal_count = static_cast<int>(volume.internal_nodes.size());
			auto* internal = allocator.allocate_array<target::GfxVoxelInternalNode>(internal_count);
			for (auto i = 0; i < internal_count; i++)
			{
				const auto& node = volume.internal_nodes[i];
				for (auto half = 0; half < 2; half++)
				{
					internal[i].firstNodeIndex[half] = static_cast<int>(node.first_node_index[half]);
					internal[i].childNodeMask[half] = static_cast<int>(node.child_node_mask[half]);
				}
			}

			const unsigned short light_list[] = {16384, 1, 0};
			auto* leaves = allocator.allocate_array<target::GfxVoxelLeafNode>(volume.leaf_count);
			for (auto i = 1u; i < volume.leaf_count; i++)
			{
				leaves[i].lightListAddress = 2; // empty light list
			}

			tree->voxelTopDownViewNodeCount = top_down_count;
			tree->voxelInternalNodeCount = internal_count;
			tree->voxelLeafNodeCount = static_cast<int>(volume.leaf_count);
			tree->lightListArraySize = static_cast<int>(std::size(light_list));
			tree->voxelTreeHeader = header;
			tree->voxelTopDownViewNodeArray = top_down;
			tree->voxelInternalNodeArray = internal;
			tree->voxelLeafNodeArray = leaves;
			tree->lightListArray = allocator.allocate_array<unsigned short>(std::size(light_list));
			std::copy(std::begin(light_list), std::end(light_list), tree->lightListArray);
			tree->voxelInternalNodeDynamicLightListCPU = allocator.allocate_array<unsigned int>(2 * static_cast<std::size_t>(internal_count));
			return tree;
		}

		void compute_bounding_volume(target::GfxGpuLightGrid* grid)
		{
			const auto diagonal = 1.0f / std::sqrt(3.0f);
			const float directions[7][2] =
			{
				{1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},
				{diagonal, diagonal}, {diagonal, -diagonal}, {-diagonal, diagonal}, {-diagonal, -diagonal},
			};

			for (auto d = 0; d < 7; d++)
			{
				const auto x = directions[d][0];
				const auto y = directions[d][1];
				const auto z = std::sqrt(std::max(1.0f - x * x - y * y, 0.0f));

				auto min = std::numeric_limits<float>::max();
				auto max = std::numeric_limits<float>::lowest();
				for (auto i = 0u; i < grid->probeCount; i++)
				{
					const auto& position = grid->probePositions[i];
					const auto distance = position[0] * x + position[1] * y + position[2] * z;
					min = std::min(min, distance);
					max = std::max(max, distance);
				}

				grid->boundingVolume[d][0] = x;
				grid->boundingVolume[d][1] = y;
				grid->boundingVolume[d][2] = min;
				grid->boundingVolume[d][3] = max;
			}
		}
	}

	const Lighting* find_lighting(const GfxWorld* world)
	{
		if (!world || !world->lighting)
		{
			return nullptr;
		}

		const auto* loaded = static_cast<const Lighting*>(db_find_asset(ASSET_TYPE_LIGHTING, xhash_name(world->lighting->name)));
		return loaded ? loaded : world->lighting;
	}

	void build_static_model_volumes(const Lighting* lighting, const std::vector<gfxworld::placed_model>& models,
		target::GfxWorldTransientZone* zone, utils::memory::allocator& allocator)
	{
		const auto count = static_cast<unsigned int>(models.size());
		if (!count)
		{
			return;
		}

		auto volumes = make_volumes(models);
		const auto width = 64u;
		auto spacing = 128.0f;
		auto depth = 0u;
		while (true)
		{
			for (auto i = 0u; i < count; i++)
			{
				auto& volume = volumes[i];
				for (auto a = 0; a < 3; a++)
				{
					const auto extent = volume.box_size[a] * models[i].transform.scale;
					volume.dimensions[a] = std::clamp(static_cast<unsigned int>(std::ceil(extent / spacing)), 1u, 4u);
				}
			}

			depth = pack_volumes(volumes, width);
			if (depth <= 12)
			{
				break;
			}

			spacing *= 1.25f;
		}

		const auto* set = find_grid_set(lighting);
		if (set)
		{
			log_grid_set(*set);
		}
		else
		{
			ZONETOOL_WARNING("the T9 lighting has no light grid, %u static model light grid volumes get no indirect light", count);
		}

		auto* atlas = allocator.allocate<target::GfxLightGridVolumeAtlasDataTemp>();
		atlas->m_widthAndHeight = width;
		atlas->m_depth = depth;
		atlas->m_numVolumes = count;
		atlas->m_textures[0] = make_volume_texture(target::GFX_PF_R11G11B10F, width, width, depth, allocator);
		atlas->m_textures[1] = make_volume_texture(target::GFX_PF_R8G8B8A8, width * 2, width * 3, depth, allocator);
		atlas->m_packingParams = allocator.allocate_array<target::LightGridVolumePackingParams>(count);
		atlas->m_transformParams = allocator.allocate_array<target::LightGridVolumeTransformParams>(count);

		auto* l0_pixels = reinterpret_cast<std::uint32_t*>(atlas->m_textures[0].pixels);
		auto* sh_pixels = reinterpret_cast<unsigned char*>(atlas->m_textures[1].pixels);

		auto& light_grid = zone->gpuLightGrid;
		light_grid.staticModelVolumeCount = count;
		light_grid.staticModelVolumes = allocator.allocate_array<target::GfxStaticModelLightGridVolume>(count);
		light_grid.tempLightGridVolumeData = atlas;

		probe_sample_counts counts{};
		const float texture_size[3] = {static_cast<float>(width), static_cast<float>(width), static_cast<float>(depth)};
		auto first_probe = 0u;
		for (auto i = 0u; i < count; i++)
		{
			const auto& model = models[i];
			const auto& volume = volumes[i];
			for (auto z = 0u; z < volume.dimensions[2]; z++)
			{
				for (auto y = 0u; y < volume.dimensions[1]; y++)
				{
					for (auto x = 0u; x < volume.dimensions[0]; x++)
					{
						const auto probe = set ? sample_probe(*set, volume_point(model, volume, x, y, z), counts) : sh_probe{};
						write_volume_texel(probe, volume.start[0] + x, volume.start[1] + y, volume.start[2] + z, width, l0_pixels, sh_pixels);
					}
				}
			}

			auto& packing = atlas->m_packingParams[i];
			for (auto a = 0; a < 3; a++)
			{
				packing.m_uvScale[a] = volume.dimensions[a] > 1 ? (volume.dimensions[a] - 1) / texture_size[a] : 0.0f;
				packing.m_uvBias[a] = (volume.start[a] + 0.5f) / texture_size[a];
				packing.m_volumeSize[a] = static_cast<unsigned char>(volume.dimensions[a]);
			}

			packing.m_basisType = 1;
			write_volume_transform(model.transform, volume, atlas->m_transformParams[i].m_volumeTransform);

			auto& model_volume = light_grid.staticModelVolumes[i];
			const auto probe_count = volume.dimensions[0] * volume.dimensions[1] * volume.dimensions[2];
			model_volume.firstProbe = first_probe;
			model_volume.singleProbe = probe_count == 1 ? first_probe : 0xFFFFFFFF;
			for (auto a = 0; a < 3; a++)
			{
				model_volume.m_volumeSize[a] = static_cast<unsigned short>(volume.dimensions[a]);
			}

			write_volume_transform(model.transform, volume, model_volume.volumeTransform);
			first_probe += probe_count;
		}

		ZONETOOL_INFO("light grid volumes: %u volumes, %u probes, atlas %ux%ux%u, spacing %.0f", count, first_probe, width, width, depth,
			spacing);
		ZONETOOL_INFO("light grid volumes: %u probes interpolated, %u probes from the nearest grid probe, %u probes without data",
			counts.interpolated, counts.nearest, counts.missing);
		if (counts.nearest || counts.missing)
		{
			ZONETOOL_WARNING("light grid volumes: %u probes are outside the T9 light grid or have only unset neighbors", counts.nearest + counts.missing);
		}

		ZONETOOL_WARNING("light grid volumes: T9 probes have no L2 coefficients, the IW8 L2 ratios are 0");
	}

	std::optional<sh_probe> build_probe_grid(const Lighting* lighting, const target::Bounds& world_bounds, target::GfxWorld* world,
		target::GfxWorldTransientZone* zone, utils::memory::allocator& allocator)
	{
		const auto* set = find_grid_set(lighting);
		float bounds_min[3]{};
		float bounds_max[3]{};
		if (!set || !get_grid_set_bounds(*set, bounds_min, bounds_max))
		{
			ZONETOOL_WARNING("the T9 lighting has no light grid, the IW8 map gets no GPU light grid");
			return {};
		}

		const auto volume = probe_volume::build(bounds_min, bounds_max, [&](const float* cell_min, const float cell_size)
		{
			return is_near_grid(*set, cell_min, cell_size);
		});
		if (!volume)
		{
			ZONETOOL_WARNING("no probe volume fits the T9 light grid, the IW8 map gets no GPU light grid");
			return {};
		}

		const auto probe_count = static_cast<unsigned int>(volume->probe_positions.size());
		const auto tetrahedron_count = static_cast<unsigned int>(volume->tetrahedrons.size());
		const auto start_count = static_cast<unsigned int>(volume->voxel_start_tetrahedron.size());

		auto* grid = allocator.allocate<target::GfxGpuLightGrid>();
		grid->probeCount = probe_count;
		grid->probes = allocator.allocate_array<target::GfxSHCompressedProbeData>(probe_count);
		grid->probePositions = allocator.allocate_array<target::vec3_t>(probe_count);
		grid->fallbackProbeCustomSamplePos[2] = 131072.0f;

		probe_sample_counts counts{};
		sh_probe average{};
		for (auto i = 0u; i < probe_count; i++)
		{
			const auto& position = volume->probe_positions[i];
			std::copy(position.begin(), position.end(), grid->probePositions[i]);

			const auto probe = sample_probe(*set, position, counts);
			grid->probes[i] = compress_probe(probe);
			for (auto k = 0u; k < average.size(); k++)
			{
				average[k] += probe[k] / static_cast<float>(probe_count);
			}
		}

		grid->zoneCount = 1;
		grid->zones = allocator.allocate<target::GfxGpuLightGridZone>();
		auto& grid_zone = grid->zones[0];
		grid_zone.numProbes = probe_count;
		grid_zone.numTetrahedrons = tetrahedron_count;
		grid_zone.voxelTetrahedronInternalNodeShift = volume->leaf_count;
		grid_zone.numVoxelTetrahedronIndices = start_count;
		grid_zone.fallbackProbeData = pack_fallback_probe(average);

		grid->tetrahedronCount = tetrahedron_count;
		grid->tetrahedrons = allocator.allocate_array<target::GfxGpuLightGridTetrahedron>(tetrahedron_count);
		grid->tetrahedronNeighbors = allocator.allocate_array<target::GfxGpuLightGridTetrahedronNeighbors>(tetrahedron_count);
		for (auto i = 0u; i < tetrahedron_count; i++)
		{
			std::copy_n(volume->tetrahedrons[i].data(), 4, grid->tetrahedrons[i].indexFlags);
			std::copy_n(volume->tetrahedron_neighbors[i].data(), 4, grid->tetrahedronNeighbors[i].neighbors);
		}

		grid->voxelStartTetrahedronCount = start_count;
		grid->voxelStartTetrahedron = allocator.allocate_array<target::GfxGpuLightGridVoxelStartTetrahedron>(start_count);
		for (auto i = 0u; i < start_count; i++)
		{
			grid->voxelStartTetrahedron[i].index = volume->voxel_start_tetrahedron[i];
		}

		compute_bounding_volume(grid);
		zone->gpuLightGrid.gpuLightGrid = grid;
		world->voxelTreeCount = 1;
		world->voxelTree = make_voxel_tree(*volume, world_bounds, allocator);

		ZONETOOL_INFO("gpu light grid: %u probes, %u tetrahedrons, %u voxel leaves of %d units", probe_count, tetrahedron_count, volume->leaf_count,
			1 << volume->node_coord_bit_shift[2]);
		ZONETOOL_INFO("gpu light grid: %u probes interpolated, %u probes from the nearest grid probe, %u probes without data", counts.interpolated,
			counts.nearest, counts.missing);
		ZONETOOL_WARNING("gpu light grid: the voxel tree has no local light lists and the tetrahedrons have no visibility data");

		return average;
	}
}
