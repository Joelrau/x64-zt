#include <std_include.hpp>

#include "../../include.hpp"
#include "../comworld.hpp"
#include "light_grid.hpp"

#include <formats/light_grid.hpp>
#include <formats/probe_volume.hpp>

namespace zonetool::h1::converter::s2::gfxworld
{
	namespace
	{
		constexpr float light_grid_cell_size[3] = {32.0f, 32.0f, 64.0f};
		constexpr int light_grid_bias[3] = {4096, 4096, 2048};

		int light_grid_cell(const float position, const int axis)
		{
			return static_cast<int>(std::floor(position / light_grid_cell_size[axis])) + light_grid_bias[axis];
		}

		float probe_sun_visibility(const float visibility, const bool has_sun)
		{
			return has_sun ? std::max(visibility, 2.0f / 255.0f) : 0.0f;
		}

		zonetool::s2::GfxSHProbeData pack_probe(const std::array<float, light_grid::probe_sh_count + 1>& values)
		{
			zonetool::s2::GfxSHProbeData probe{};
			for (auto i = 0u; i < values.size(); i++)
			{
				probe.coeffs[i] = half_float::float_to_half(values[i]);
			}
			return probe;
		}

		std::array<float, light_grid::probe_sh_count + 1> default_probe(const GfxLightGrid& grid)
		{
			float radiance[light_grid::basis_dir_count][3];
			for (auto dir = 0u; dir < light_grid::basis_dir_count; dir++)
			{
				for (auto channel = 0; channel < 3; channel++)
				{
					radiance[dir][channel] = half_float::half_to_float(grid.defaultLightGridColors.rgb[dir][channel]);
				}
			}

			std::array<float, light_grid::probe_sh_count + 1> values{};
			light_grid::project_probe_sh(radiance, values.data());
			values.back() = 1.0f;
			return values;
		}

		class probe_average
		{
		public:
			void add(const std::array<float, light_grid::probe_sh_count + 1>& values)
			{
				for (auto i = 0u; i < values.size(); i++)
				{
					this->sum_[i] += values[i];
				}
				this->count_++;
			}

			unsigned int count() const
			{
				return this->count_;
			}

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> value() const
			{
				if (!this->count_)
				{
					return {};
				}

				auto result = this->sum_;
				for (auto& value : result)
				{
					value /= static_cast<float>(this->count_);
				}
				result.back() = 1.0f;
				return result;
			}

		private:
			std::array<float, light_grid::probe_sh_count + 1> sum_{};
			unsigned int count_ = 0;
		};

		class light_grid_sampler
		{
		public:
			light_grid_sampler(const GfxLightGrid& grid, const ComWorld* com_world)
				: grid_(grid)
				, tree_(light_grid::make_tree_view(grid.tree[0], 7))
				, config_{grid.rangeExponent8BitsEncoding, grid.rangeExponent12BitsEncoding, grid.rangeExponent16BitsEncoding}
			{
				if (!com_world)
				{
					return;
				}

				this->sun_envs_.resize(com_world->primaryLightEnvCount);
				for (auto i = 0u; i < com_world->primaryLightEnvCount; i++)
				{
					for (const auto light : comworld::env_light_indices(*com_world, com_world->primaryLightEnvs[i]))
					{
						if (com_world->primaryLights[light].type == GFX_LIGHT_TYPE_DIR)
						{
							this->sun_envs_[i] = true;
						}
					}
				}
			}

			bool valid() const
			{
				return this->tree_.node_table && this->grid_.paletteBitstream && this->grid_.paletteEntryAddress;
			}

			bool env_has_sun(const unsigned int env) const
			{
				return env >= this->sun_envs_.size() || this->sun_envs_[env];
			}

			bool has_sun() const
			{
				return this->sun_envs_.empty() || std::find(this->sun_envs_.begin(), this->sun_envs_.end(), true) != this->sun_envs_.end();
			}

			const light_grid::tree_view& tree() const
			{
				return this->tree_;
			}

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> sample(const float origin[3])
			{
				int base[3];
				float fraction[3];
				for (auto axis = 0; axis < 3; axis++)
				{
					const auto position = origin[axis] / light_grid_cell_size[axis];
					base[axis] = light_grid_cell(origin[axis], axis);
					fraction[axis] = position - std::floor(position);
				}

				std::array<float, light_grid::probe_sh_count + 1> result{};
				auto total_weight = 0.0f;
				for (auto corner = 0; corner < 8; corner++)
				{
					auto weight = 1.0f;
					int cell[3];
					for (auto axis = 0; axis < 3; axis++)
					{
						const auto upper = (corner >> axis) & 1;
						cell[axis] = base[axis] + upper;
						weight *= upper ? fraction[axis] : 1.0f - fraction[axis];
					}

					const auto values = this->cell_values(cell);
					if (!values || weight <= 0.0f)
					{
						continue;
					}

					for (auto i = 0u; i < result.size(); i++)
					{
						result[i] += (*values)[i] * weight;
					}
					total_weight += weight;
				}

				if (total_weight > 0.0f)
				{
					for (auto& value : result)
					{
						value /= total_weight;
					}
					return result;
				}

				return this->nearest(base);
			}

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> baked(const GfxStaticModelAmbientLightingInfo& lighting)
			{
				if (lighting.colorIndex >= this->grid_.paletteEntryCount || lighting.colorIndex == this->grid_.missingGridColorIndex)
				{
					return {};
				}

				auto values = this->palette_values(lighting.colorIndex);
				values.back() = lighting.primaryLightWeight;
				return values;
			}

		private:
			const GfxLightGrid& grid_;
			light_grid::tree_view tree_;
			light_grid::palette_config config_;
			std::vector<bool> sun_envs_;
			std::unordered_map<unsigned int, std::array<float, light_grid::probe_sh_count + 1>> palette_cache_;

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> nearest(const int base[3])
			{
				std::optional<std::array<float, light_grid::probe_sh_count + 1>> best;
				auto best_distance = std::numeric_limits<int>::max();
				for (auto dz = -2; dz <= 2; dz++)
				{
					for (auto dy = -4; dy <= 4; dy++)
					{
						for (auto dx = -4; dx <= 4; dx++)
						{
							const auto distance = dx * dx + dy * dy + 4 * dz * dz;
							if (distance >= best_distance)
							{
								continue;
							}

							const int cell[3] = {base[0] + dx, base[1] + dy, base[2] + dz};
							if (const auto values = this->cell_values(cell))
							{
								best = values;
								best_distance = distance;
							}
						}
					}
				}
				return best;
			}

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> cell_values(const int cell[3])
			{
				const unsigned short grid_pos[3] =
				{
					static_cast<unsigned short>(cell[0]), static_cast<unsigned short>(cell[1]), static_cast<unsigned short>(cell[2]),
				};

				light_grid::voxel voxel{};
				if (!light_grid::find_voxel(this->tree_, grid_pos, voxel) || voxel.color_index >= this->grid_.paletteEntryCount)
				{
					return {};
				}

				auto values = this->palette_values(voxel.color_index);
				const auto is_sun = voxel.light_index < this->sun_envs_.size() && this->sun_envs_[voxel.light_index];
				values.back() = is_sun ? 1.0f : 0.0f;
				return values;
			}

			std::array<float, light_grid::probe_sh_count + 1> palette_values(const unsigned int color_index)
			{
				const auto cached = this->palette_cache_.find(color_index);
				if (cached != this->palette_cache_.end())
				{
					return cached->second;
				}

				light_grid::palette_sh sh{};
				light_grid::decode_palette_entry(this->config_, this->grid_.paletteBitstream + this->grid_.paletteEntryAddress[color_index], sh);

				float radiance[light_grid::basis_dir_count][3];
				light_grid::evaluate(sh, radiance);

				std::array<float, light_grid::probe_sh_count + 1> values{};
				light_grid::project_probe_sh(radiance, values.data());
				return this->palette_cache_.emplace(color_index, values).first->second;
			}
		};

		class light_grid_occupancy
		{
		public:
			explicit light_grid_occupancy(const light_grid::tree_view& tree)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					this->min_[axis] = tree.coord_min_grid_space[axis];
					this->size_[axis] = std::max(tree.coord_max_grid_space[axis] - tree.coord_min_grid_space[axis] + 1, 0);
				}

				this->occupied_.resize(static_cast<std::size_t>(this->size_[0]) * this->size_[1] * this->size_[2]);
				for (auto z = 0; z < this->size_[2]; z++)
				{
					for (auto y = 0; y < this->size_[1]; y++)
					{
						for (auto x = 0; x < this->size_[0]; x++)
						{
							const unsigned short grid_pos[3] =
							{
								static_cast<unsigned short>(this->min_[0] + x),
								static_cast<unsigned short>(this->min_[1] + y),
								static_cast<unsigned short>(this->min_[2] + z),
							};

							light_grid::voxel voxel{};
							if (!light_grid::find_voxel(tree, grid_pos, voxel))
							{
								continue;
							}

							this->occupied_[this->index(x, y, z)] = true;
							const int cell[3] = {x, y, z};
							for (auto axis = 0; axis < 3; axis++)
							{
								this->occupied_min_[axis] = std::min(this->occupied_min_[axis], cell[axis]);
								this->occupied_max_[axis] = std::max(this->occupied_max_[axis], cell[axis]);
							}
						}
					}
				}
			}

			void clip_to_voxels(float bounds_min[3], float bounds_max[3]) const
			{
				if (this->occupied_min_[0] > this->occupied_max_[0])
				{
					return;
				}

				for (auto axis = 0; axis < 3; axis++)
				{
					const auto first_cell = this->min_[axis] + this->occupied_min_[axis] - 1 - light_grid_bias[axis];
					const auto end_cell = this->min_[axis] + this->occupied_max_[axis] + 2 - light_grid_bias[axis];
					bounds_min[axis] = std::max(bounds_min[axis], static_cast<float>(first_cell) * light_grid_cell_size[axis]);
					bounds_max[axis] = std::min(bounds_max[axis], static_cast<float>(end_cell) * light_grid_cell_size[axis]);
				}
			}

			bool has_voxel_near(const float min[3], const float size) const
			{
				int first[3];
				int last[3];
				for (auto axis = 0; axis < 3; axis++)
				{
					first[axis] = std::max(light_grid_cell(min[axis], axis) - 1 - this->min_[axis], 0);
					last[axis] = std::min(light_grid_cell(min[axis] + size, axis) + 1 - this->min_[axis], this->size_[axis] - 1);
					if (first[axis] > last[axis])
					{
						return false;
					}
				}

				for (auto z = first[2]; z <= last[2]; z++)
				{
					for (auto y = first[1]; y <= last[1]; y++)
					{
						for (auto x = first[0]; x <= last[0]; x++)
						{
							if (this->occupied_[this->index(x, y, z)])
							{
								return true;
							}
						}
					}
				}
				return false;
			}

		private:
			int min_[3]{};
			int size_[3]{};
			int occupied_min_[3]{INT_MAX, INT_MAX, INT_MAX};
			int occupied_max_[3]{INT_MIN, INT_MIN, INT_MIN};
			std::vector<bool> occupied_;

			std::size_t index(const int x, const int y, const int z) const
			{
				return (static_cast<std::size_t>(z) * this->size_[1] + y) * this->size_[0] + x;
			}
		};

		zonetool::s2::GfxStaticModelProbeGrid make_single_probe_grid(const unsigned int probe, const Bounds& bounds)
		{
			zonetool::s2::GfxStaticModelProbeGrid grid{};
			grid.firstProbe = probe;
			grid.probeCountX = 1;
			grid.probeCountY = 1;
			grid.probeCountZ = 1;
			grid.averageProbe = probe;
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto size = std::max(2.0f * bounds.halfSize[axis], 1.0f);
				const auto min = bounds.midPoint[axis] - 0.5f * size;
				grid.worldToGrid[axis][axis] = 1.0f / size;
				grid.worldToGrid[axis][3] = -min / size;
			}
			return grid;
		}

		std::optional<std::array<float, light_grid::probe_sh_count + 1>> static_model_lighting(GfxWorld* asset, light_grid_sampler& sampler, const unsigned int index)
		{
			if (asset->dpvs.smodelDrawInsts[index].flags & STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING)
			{
				if (const auto baked = sampler.baked(asset->dpvs.smodelLightingInsts[index].ambientLightingInfo))
				{
					return baked;
				}
			}

			return sampler.sample(asset->dpvs.smodelInsts[index].lightingOrigin);
		}

		void convert_static_model_probes(GfxWorld* asset, zonetool::s2::GfxWorld& world, light_grid_sampler& sampler, utils::memory::allocator& allocator)
		{
			auto& data = world.probeData;
			const auto count = asset->dpvs.smodelCount;
			if (!count)
			{
				return;
			}

			if (!sampler.valid())
			{
				ZONETOOL_WARNING("GfxWorld: the source map has no light grid, static models use its default light grid color");
			}

			const auto grids = allocator.allocate_array<zonetool::s2::GfxStaticModelProbeGrid>(count);
			const auto probes = allocator.allocate_array<zonetool::s2::GfxSHProbeData>(count);
			std::vector<std::optional<std::array<float, light_grid::probe_sh_count + 1>>> samples(count);
			probe_average average;
			for (auto i = 0u; i < count; i++)
			{
				const auto& inst = asset->dpvs.smodelInsts[i];
				grids[i] = make_single_probe_grid(i, inst.bounds);

				samples[i] = sampler.valid() ? static_model_lighting(asset, sampler, i) : std::nullopt;
				if (samples[i])
				{
					average.add(*samples[i]);
				}
			}

			const auto fallback = average.value().value_or(default_probe(asset->lightGrid));
			for (auto i = 0u; i < count; i++)
			{
				auto values = samples[i] ? *samples[i] : fallback;
				const auto has_sun = sampler.env_has_sun(asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex);
				values.back() = probe_sun_visibility(values.back(), has_sun);
				probes[i] = pack_probe(values);
			}

			data.smodelProbeGridCount = count;
			data.smodelProbeGrids = grids;
			data.smodelProbeCount = count;
			data.smodelProbes = probes;

			ZONETOOL_INFO("GfxWorld: %u of %u static model probes sampled from the light grid", average.count(), count);
		}

		zonetool::s2::GfxVoxelTree* make_voxel_tree(const probe_volume::volume& volume, utils::memory::allocator& allocator)
		{
			const auto header = allocator.allocate<zonetool::s2::GfxVoxelTreeHeader>();
			const auto tree = allocator.allocate<zonetool::s2::GfxVoxelTree>();
			for (auto axis = 0; axis < 3; axis++)
			{
				header->rootNodeDimension[axis] = volume.root_node_dimension[axis];
				header->nodeCoordBitShift[axis] = volume.node_coord_bit_shift[axis];
				header->boundMin[axis] = volume.bound_min[axis];
				header->boundMax[axis] = volume.bound_max[axis];
				tree->zoneBound.midPoint[axis] = 0.5f * (volume.bound_min[axis] + volume.bound_max[axis]);
				tree->zoneBound.halfSize[axis] = 0.5f * (volume.bound_max[axis] - volume.bound_min[axis]);
			}

			const auto top_down_count = static_cast<unsigned int>(volume.top_down_nodes.size());
			const auto top_down = allocator.allocate_array<zonetool::s2::GfxVoxelTopDownViewNode>(top_down_count);
			for (auto i = 0u; i < top_down_count; i++)
			{
				const auto& node = volume.top_down_nodes[i];
				top_down[i] = {node.first_node_index, node.z_min, node.z_max};
			}

			const auto internal_count = static_cast<unsigned int>(volume.internal_nodes.size());
			const auto internal = allocator.allocate_array<zonetool::s2::GfxVoxelInternalNode>(internal_count);
			for (auto i = 0u; i < internal_count; i++)
			{
				const auto& node = volume.internal_nodes[i];
				for (auto half = 0; half < 2; half++)
				{
					internal[i].firstNodeIndex[half] = static_cast<int>(node.first_node_index[half]);
					internal[i].childNodeMask[half] = node.child_node_mask[half];
				}
			}

			const std::uint16_t light_list[] = {16384, 1, 0};
			const auto leaves = allocator.allocate_array<zonetool::s2::GfxVoxelLeafNode>(volume.leaf_count);
			for (auto i = 1u; i < volume.leaf_count; i++)
			{
				leaves[i].lightListAddress = 2;
			}

			tree->voxelTopDownViewNodeCount = top_down_count;
			tree->voxelInternalNodeCount = internal_count;
			tree->voxelLeafNodeCount = volume.leaf_count;
			tree->lightListArraySize = static_cast<unsigned int>(std::size(light_list));
			tree->voxelTreeHeader = header;
			tree->voxelTopDownViewNodeArray = top_down;
			tree->voxelInternalNodeArray = internal;
			tree->voxelLeafNodeArray = leaves;
			tree->lightListArray = copy_array(light_list, std::size(light_list), allocator);
			tree->voxelInternalNodeDynamicLightList = allocator.allocate_array<unsigned int[2]>(internal_count);
			return tree;
		}

		void convert_probe_volume(GfxWorld* asset, zonetool::s2::GfxWorld& world, light_grid_sampler& sampler, utils::memory::allocator& allocator)
		{
			float world_min[3];
			float world_max[3];
			for (auto axis = 0; axis < 3; axis++)
			{
				world_min[axis] = asset->bounds.midPoint[axis] - asset->bounds.halfSize[axis];
				world_max[axis] = asset->bounds.midPoint[axis] + asset->bounds.halfSize[axis];
			}

			float bounds_min[3];
			float bounds_max[3];
			std::copy_n(world_min, 3, bounds_min);
			std::copy_n(world_max, 3, bounds_max);

			std::optional<light_grid_occupancy> occupancy;
			probe_volume::cell_test cell_occupied;
			if (sampler.valid())
			{
				occupancy.emplace(sampler.tree());
				occupancy->clip_to_voxels(bounds_min, bounds_max);
				cell_occupied = [&occupancy](const float min[3], const float size)
				{
					return occupancy->has_voxel_near(min, size);
				};
			}

			auto volume = probe_volume::build(bounds_min, bounds_max, cell_occupied);
			if (!volume && cell_occupied)
			{
				ZONETOOL_WARNING("GfxWorld: no light probe volume fits the light grid voxels, it covers the whole map instead");
				volume = probe_volume::build(world_min, world_max, {});
			}

			if (!volume)
			{
				ZONETOOL_WARNING("GfxWorld: no light probe volume fits the map, the world has none");
				return;
			}

			auto& data = world.probeData;
			const auto probe_count = static_cast<unsigned int>(volume->probe_positions.size());
			const auto positions = allocator.allocate_array<zonetool::s2::GfxGpuLightGridProbePosition>(probe_count);
			std::vector<std::optional<std::array<float, light_grid::probe_sh_count + 1>>> samples(probe_count);
			for (auto i = 0u; i < probe_count; i++)
			{
				std::memcpy(positions[i].origin, volume->probe_positions[i].data(), sizeof(positions[i].origin));
				samples[i] = sampler.sample(positions[i].origin);
			}

			const auto grid_names_sun = std::ranges::any_of(samples, [](const auto& sample)
			{
				return sample && sample->back() > 0.0f;
			});
			const auto sun_everywhere = !grid_names_sun && sampler.has_sun();

			probe_average average;
			for (auto& sample : samples)
			{
				if (sample)
				{
					sample->back() = probe_sun_visibility(sample->back(), sun_everywhere || sample->back() > 0.0f);
					average.add(*sample);
				}
			}

			const auto fallback = pack_probe(average.value().value_or(default_probe(asset->lightGrid)));
			const auto probes = allocator.allocate_array<zonetool::s2::GfxSHProbeData>(probe_count);
			for (auto i = 0u; i < probe_count; i++)
			{
				probes[i] = samples[i] ? pack_probe(*samples[i]) : fallback;
			}

			const auto tetrahedron_count = static_cast<unsigned int>(volume->tetrahedrons.size());
			const auto tetrahedrons = allocator.allocate_array<zonetool::s2::GfxGpuLightGridTetrahedron>(tetrahedron_count);
			const auto neighbors = allocator.allocate_array<zonetool::s2::GfxGpuLightGridTetrahedronNeighbors>(tetrahedron_count);
			for (auto i = 0u; i < tetrahedron_count; i++)
			{
				std::copy_n(volume->tetrahedrons[i].data(), 4, tetrahedrons[i].indexFlags);
				std::copy_n(volume->tetrahedron_neighbors[i].data(), 4, neighbors[i].neighbors);
			}

			const auto start_count = static_cast<unsigned int>(volume->voxel_start_tetrahedron.size());
			const auto starts = allocator.allocate_array<zonetool::s2::GfxGpuLightGridVoxelStartTetrahedron>(start_count);
			for (auto i = 0u; i < start_count; i++)
			{
				starts[i].index = volume->voxel_start_tetrahedron[i];
			}

			const auto zone = allocator.allocate<zonetool::s2::GfxGpuLightGridZone>();
			zone->numProbes = probe_count;
			zone->numTetrahedrons = tetrahedron_count;
			zone->voxelTetrahedronInternalNodeShift = volume->leaf_count;
			zone->numVoxelTetrahedronIndices = start_count;
			zone->fallbackProbeData = fallback;

			data.probeCount = probe_count;
			data.probes = probes;
			data.probePositions = positions;
			data.zoneCount = 1;
			data.zones = zone;
			data.tetrahedronCount = tetrahedron_count;
			data.tetrahedrons = tetrahedrons;
			data.tetrahedronNeighbors = neighbors;
			data.voxelStartTetrahedronCount = start_count;
			data.voxelStartTetrahedron = starts;

			world.voxelTreeCount = 1;
			world.voxelTree = make_voxel_tree(*volume, allocator);

			ZONETOOL_INFO("GfxWorld: light probe volume with %u probes (%u sampled from the light grid), %u tetrahedrons, %u voxel leaves of %d units",
				probe_count, average.count(), tetrahedron_count, volume->leaf_count, 1 << volume->node_coord_bit_shift[2]);
		}

		void convert_lighting_lookups(GfxWorld* asset, zonetool::s2::GfxWorld& world)
		{
			world.useLightGridDefaultModelLightingLookup = asset->useLightGridDefaultModelLightingLookup;
			std::memcpy(world.lightGridDefaultModelLightingLookup, asset->lightGridDefaultModelLightingLookup, sizeof(world.lightGridDefaultModelLightingLookup));

			world.useLightGridDefaultFXLightingLookup = true;
			const auto header = world.voxelTree ? world.voxelTree->voxelTreeHeader : nullptr;
			for (auto axis = 0; axis < 3; axis++)
			{
				world.lightGridDefaultFXLightingLookup[axis] = asset->useLightGridDefaultFXLightingLookup ? asset->lightGridDefaultFXLightingLookup[axis]
					: header ? 0.5f * (header->boundMin[axis] + header->boundMax[axis]) : asset->bounds.midPoint[axis];
			}
		}
	}

	void convert_light_grid(zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
	{
		auto& grid = world.lightGrid;
		grid.colAxis = 1;
		grid.rowDataStart = allocator.allocate<unsigned short>();
		grid.tableVersion = 1;
		grid.paletteVersion = 1;
	}

	void convert_light_grid_probes(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
	{
		light_grid_sampler sampler(asset->lightGrid, comworld::source_world());
		convert_static_model_probes(asset, world, sampler, allocator);
		convert_probe_volume(asset, world, sampler, allocator);
		convert_lighting_lookups(asset, world);
	}
}
