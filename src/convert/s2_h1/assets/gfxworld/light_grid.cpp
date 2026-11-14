#include <std_include.hpp>

#include "../../include.hpp"
#include "../comworld.hpp"
#include "light_grid.hpp"

namespace zonetool::s2::converter::h1::gfxworld
{
	namespace
	{
		constexpr float light_grid_cell_size[3] = {32.0f, 32.0f, 64.0f};
		constexpr int light_grid_bias[3] = {4096, 4096, 2048};
		constexpr light_grid::palette_config palette_config{0, 0, 23};

		probe_values unpack_probe(const GfxSHProbeData& probe)
		{
			probe_values values{};
			for (auto i = 0u; i < values.size(); i++)
			{
				values[i] = half_float::half_to_float(probe.coeffs[i]);
			}

			return values;
		}

		light_grid::palette_sh to_palette_sh(const probe_values& values)
		{
			light_grid::palette_sh sh{};
			light_grid::probe_sh_to_palette(values.data(), sh);
			return sh;
		}

		float grid_position(const int cell, const int axis)
		{
			return static_cast<float>(cell - light_grid_bias[axis]) * light_grid_cell_size[axis];
		}

		std::uint64_t pack_cell(const int cell[3])
		{
			return (static_cast<std::uint64_t>(cell[2]) << 32) | (static_cast<std::uint64_t>(cell[1]) << 16) | static_cast<std::uint64_t>(cell[0]);
		}

		bool is_grid_cell(const int cell[3])
		{
			return std::all_of(cell, cell + 3, [](const int value) { return value >= 0 && value <= std::numeric_limits<unsigned short>::max(); });
		}

		std::optional<probe_tetrahedron> make_tetrahedron(const GfxLightGridProbeData& data, const GfxGpuLightGridTetrahedron& source)
		{
			probe_tetrahedron tetrahedron{};
			for (auto i = 0; i < 4; i++)
			{
				tetrahedron.probes[i] = source.indexFlags[i] & 0x7FFFFFFF;
				if (tetrahedron.probes[i] >= data.probeCount)
				{
					return {};
				}
			}

			const auto& origin = data.probePositions[tetrahedron.probes[0]].origin;
			std::memcpy(tetrahedron.origin, origin, sizeof(tetrahedron.origin));

			float edges[3][3];
			for (auto i = 0; i < 3; i++)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					edges[i][axis] = data.probePositions[tetrahedron.probes[i + 1]].origin[axis] - origin[axis];
				}
			}

			const float cross[3][3] =
			{
				{edges[1][1] * edges[2][2] - edges[1][2] * edges[2][1], edges[1][2] * edges[2][0] - edges[1][0] * edges[2][2], edges[1][0] * edges[2][1] - edges[1][1] * edges[2][0]},
				{edges[2][1] * edges[0][2] - edges[2][2] * edges[0][1], edges[2][2] * edges[0][0] - edges[2][0] * edges[0][2], edges[2][0] * edges[0][1] - edges[2][1] * edges[0][0]},
				{edges[0][1] * edges[1][2] - edges[0][2] * edges[1][1], edges[0][2] * edges[1][0] - edges[0][0] * edges[1][2], edges[0][0] * edges[1][1] - edges[0][1] * edges[1][0]},
			};

			const auto determinant = edges[0][0] * cross[0][0] + edges[0][1] * cross[0][1] + edges[0][2] * cross[0][2];
			if (std::abs(determinant) < 1e-3f)
			{
				return {};
			}

			for (auto i = 0; i < 3; i++)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					tetrahedron.inverse[i][axis] = cross[i][axis] / determinant;
				}
			}

			return tetrahedron;
		}

		std::optional<unsigned int> nearest_corner_probe(const probe_tetrahedron& tetrahedron, const float position[3])
		{
			float weights[4]{1.0f, 0.0f, 0.0f, 0.0f};
			for (auto i = 0; i < 3; i++)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					weights[i + 1] += tetrahedron.inverse[i][axis] * (position[axis] - tetrahedron.origin[axis]);
				}

				weights[0] -= weights[i + 1];
			}

			const auto best = std::max_element(weights, weights + 4);
			if (*std::min_element(weights, weights + 4) < -0.001f)
			{
				return {};
			}

			return tetrahedron.probes[best - weights];
		}

		class world_grid
		{
		public:
			world_grid(const GfxWorld* asset, probe_palette& palette)
				: data_(asset->probeData)
				, palette_(palette)
			{
				const auto has_sun = asset->lastSunPrimaryLightIndex >= 1 && asset->primaryLightCount > 1;
				this->sun_env_ = comworld::get_light_env(has_sun ? std::vector<std::uint16_t>{1} : std::vector<std::uint16_t>{});
				this->no_sun_env_ = comworld::get_light_env({});
			}

			void add_tetrahedrons()
			{
				for (auto i = 0u; this->data_.tetrahedrons && this->data_.probePositions && i < this->data_.tetrahedronCount; i++)
				{
					if (const auto tetrahedron = make_tetrahedron(this->data_, this->data_.tetrahedrons[i]))
					{
						this->add_tetrahedron(*tetrahedron);
					}
				}
			}

			void add_probes()
			{
				for (auto i = 0u; this->data_.probePositions && i < this->data_.probeCount; i++)
				{
					int cell[3];
					for (auto axis = 0; axis < 3; axis++)
					{
						cell[axis] = static_cast<int>(std::lround(this->data_.probePositions[i].origin[axis] / light_grid_cell_size[axis])) + light_grid_bias[axis];
					}

					this->add_sample(cell, i);
				}
			}

			std::vector<light_grid::grid_sample> samples() const
			{
				std::vector<light_grid::grid_sample> samples;
				samples.reserve(this->samples_.size());
				for (const auto& [cell, sample] : this->samples_)
				{
					samples.emplace_back(sample);
				}

				return samples;
			}

			const GfxSHProbeData* nearest_probe(const float position[3]) const
			{
				const GfxSHProbeData* nearest = nullptr;
				auto nearest_distance = std::numeric_limits<float>::max();
				for (auto i = 0u; this->data_.probes && this->data_.probePositions && i < this->data_.probeCount; i++)
				{
					const auto& origin = this->data_.probePositions[i].origin;
					const auto distance = std::hypot(origin[0] - position[0], origin[1] - position[1], origin[2] - position[2]);
					if (distance < nearest_distance)
					{
						nearest_distance = distance;
						nearest = &this->data_.probes[i];
					}
				}

				return nearest;
			}

		private:
			const GfxLightGridProbeData& data_;
			probe_palette& palette_;
			std::uint16_t sun_env_{};
			std::uint16_t no_sun_env_{};
			std::unordered_map<std::uint64_t, light_grid::grid_sample> samples_;

			void add_sample(const int cell[3], const unsigned int probe)
			{
				if (!is_grid_cell(cell) || this->samples_.contains(pack_cell(cell)))
				{
					return;
				}

				light_grid::grid_sample sample{};
				for (auto axis = 0; axis < 3; axis++)
				{
					sample.grid_pos[axis] = static_cast<unsigned short>(cell[axis]);
				}

				const auto sun_visibility = half_float::half_to_float(this->data_.probes[probe].coeffs[light_grid::probe_sh_count]);
				sample.value.color_index = this->palette_.add(this->data_.probes[probe]);
				sample.value.light_index = sun_visibility > 0.0f ? this->sun_env_ : this->no_sun_env_;
				this->samples_.emplace(pack_cell(cell), sample);
			}

			void add_tetrahedron(const probe_tetrahedron& tetrahedron)
			{
				int first[3];
				int last[3];
				for (auto axis = 0; axis < 3; axis++)
				{
					auto min = std::numeric_limits<float>::max();
					auto max = std::numeric_limits<float>::lowest();
					for (const auto probe : tetrahedron.probes)
					{
						min = std::min(min, this->data_.probePositions[probe].origin[axis]);
						max = std::max(max, this->data_.probePositions[probe].origin[axis]);
					}

					first[axis] = static_cast<int>(std::ceil(min / light_grid_cell_size[axis])) + light_grid_bias[axis];
					last[axis] = static_cast<int>(std::floor(max / light_grid_cell_size[axis])) + light_grid_bias[axis];
				}

				for (auto z = first[2]; z <= last[2]; z++)
				{
					for (auto y = first[1]; y <= last[1]; y++)
					{
						for (auto x = first[0]; x <= last[0]; x++)
						{
							const int cell[3] = {x, y, z};
							const float position[3] = {grid_position(x, 0), grid_position(y, 1), grid_position(z, 2)};
							if (const auto probe = nearest_corner_probe(tetrahedron, position))
							{
								this->add_sample(cell, *probe);
							}
						}
					}
				}
			}
		};

		const GfxSHProbeData* static_model_probe(const GfxWorld* asset, const world_grid& grid, const unsigned int index)
		{
			const auto& data = asset->probeData;
			const auto slot = asset->dpvs.smodelLightingInsts[index].lightGridInfo.slot;
			if ((asset->dpvs.smodelDrawInsts[index].flags & STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) && data.smodelProbeGrids && data.smodelProbes
				&& slot < data.smodelProbeGridCount && data.smodelProbeGrids[slot].averageProbe < data.smodelProbeCount)
			{
				return &data.smodelProbes[data.smodelProbeGrids[slot].averageProbe];
			}

			return grid.nearest_probe(asset->dpvs.smodelInsts[index].lightingOrigin);
		}

		unsigned int convert_static_model_lighting(const GfxWorld* asset, zonetool::h1::GfxWorld* world, const world_grid& grid, probe_palette& palette)
		{
			auto baked = 0u;
			auto& dpvs = world->dpvs;
			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				const auto probe = static_model_probe(asset, grid, i);
				if (!probe)
				{
					continue;
				}

				auto& lighting = dpvs.smodelLightingInsts[i].ambientLightingInfo;
				lighting.colorIndex = palette.add(*probe);
				lighting.primaryLightWeight = std::clamp(half_float::half_to_float(probe->coeffs[light_grid::probe_sh_count]), 0.0f, 1.0f);
				dpvs.smodelDrawInsts[i].flags |= zonetool::h1::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING | zonetool::h1::STATIC_MODEL_FLAG_ANIMATED_VERTS;
				baked++;
			}

			return baked;
		}

		void write_tree(const light_grid::tree_data& tree, zonetool::h1::GfxLightGridTree& new_tree, utils::memory::allocator& allocator)
		{
			new_tree.maxDepth = tree.max_depth;
			new_tree.nodeCount = static_cast<int>(tree.node_table.size());
			new_tree.leafCount = tree.leaf_count;
			std::memcpy(new_tree.coordMinGridSpace, tree.coord_min_grid_space, sizeof(new_tree.coordMinGridSpace));
			std::memcpy(new_tree.coordMaxGridSpace, tree.coord_max_grid_space, sizeof(new_tree.coordMaxGridSpace));
			std::memcpy(new_tree.coordHalfSizeGridSpace, tree.coord_half_size_grid_space, sizeof(new_tree.coordHalfSizeGridSpace));
			new_tree.defaultColorIndexBitCount = tree.default_color_index_bit_count;
			new_tree.defaultLightIndexBitCount = tree.default_light_index_bit_count;
			new_tree.p_nodeTable = allocator.allocate_array<unsigned int>(tree.node_table.size());
			std::memcpy(new_tree.p_nodeTable, tree.node_table.data(), tree.node_table.size() * sizeof(unsigned int));
			new_tree.leafTableSize = static_cast<int>(tree.leaf_table.size());
			if (!tree.leaf_table.empty())
			{
				new_tree.p_leafTable = allocator.allocate_array<unsigned char>(tree.leaf_table.size());
				std::memcpy(new_tree.p_leafTable, tree.leaf_table.data(), tree.leaf_table.size());
			}
		}

		light_grid::tree_data build_tree(const GfxWorld* asset, const std::vector<light_grid::grid_sample>& samples)
		{
			try
			{
				return light_grid::build_tree(samples, 7);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_WARNING("GfxWorld \"%s\": %s, the light grid has no voxels", asset->name, e.what());
				return light_grid::build_tree({}, 7);
			}
		}

		void set_default_colors(const probe_values& average, zonetool::h1::GfxLightGrid& grid)
		{
			float radiance[light_grid::basis_dir_count][3];
			light_grid::evaluate(to_palette_sh(average), radiance);
			for (auto dir = 0u; dir < light_grid::basis_dir_count; dir++)
			{
				for (auto channel = 0; channel < 3; channel++)
				{
					grid.skyLightGridColors.rgb[dir][channel] = half_float::float_to_half(radiance[dir][channel]);
					grid.defaultLightGridColors.rgb[dir][channel] = half_float::float_to_half(radiance[dir][channel]);
				}
			}
		}
	}

	unsigned int probe_palette::add(const GfxSHProbeData& probe)
	{
		const std::string key(reinterpret_cast<const char*>(probe.coeffs), light_grid::probe_sh_count * sizeof(probe.coeffs[0]));
		const auto [entry, inserted] = this->indices_.try_emplace(key, static_cast<unsigned int>(this->probes_.size()) + 1);
		if (inserted)
		{
			this->probes_.emplace_back(unpack_probe(probe));
		}

		return entry->second;
	}

	unsigned int probe_palette::missing_index() const
	{
		return static_cast<unsigned int>(this->probes_.size()) + 1;
	}

	probe_values probe_palette::average() const
	{
		probe_values average{};
		for (const auto& probe : this->probes_)
		{
			for (auto i = 0u; i < average.size(); i++)
			{
				average[i] += probe[i] / static_cast<float>(this->probes_.size());
			}
		}

		return average;
	}

	void probe_palette::write(zonetool::h1::GfxLightGrid& grid, utils::memory::allocator& allocator) const
	{
		std::vector<int> addresses;
		std::vector<unsigned char> stream;
		const auto average = to_palette_sh(this->average());
		addresses.emplace_back(light_grid::encode_palette_entry(palette_config, average, stream));
		for (const auto& probe : this->probes_)
		{
			addresses.emplace_back(light_grid::encode_palette_entry(palette_config, to_palette_sh(probe), stream));
		}

		addresses.emplace_back(light_grid::encode_palette_entry(palette_config, average, stream));

		grid.tableVersion = 1;
		grid.paletteVersion = 1;
		grid.rangeExponent8BitsEncoding = palette_config.range_exp_8bits;
		grid.rangeExponent12BitsEncoding = palette_config.range_exp_12bits;
		grid.rangeExponent16BitsEncoding = palette_config.range_exp_16bits;
		grid.missingGridColorIndex = this->missing_index();
		grid.paletteEntryCount = static_cast<unsigned int>(addresses.size());
		grid.paletteEntryAddress = allocator.allocate_array<int>(addresses.size());
		std::memcpy(grid.paletteEntryAddress, addresses.data(), addresses.size() * sizeof(int));
		grid.paletteBitstreamSize = static_cast<unsigned int>(stream.size());
		grid.paletteBitstream = allocator.allocate_array<unsigned char>(stream.size());
		std::memcpy(grid.paletteBitstream, stream.data(), stream.size());
	}

	void convert_light_grid(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
	{
		probe_palette palette;
		world_grid grid(asset, palette);
		if (asset->probeData.probes)
		{
			grid.add_tetrahedrons();
			grid.add_probes();
		}

		const auto samples = grid.samples();
		const auto baked_models = convert_static_model_lighting(asset, world, grid, palette);
		if (palette.missing_index() == 1)
		{
			ZONETOOL_WARNING("GfxWorld \"%s\" has no light probes, the light grid is empty", asset->name);
			return;
		}

		auto& new_grid = world->lightGrid;
		for (auto i = 0u; i < std::size(new_grid.tree); i++)
		{
			new_grid.tree[i] = {};
			new_grid.tree[i].index = static_cast<unsigned char>(i);
		}

		write_tree(build_tree(asset, samples), new_grid.tree[0], allocator);
		palette.write(new_grid, allocator);
		set_default_colors(palette.average(), new_grid);

		ZONETOOL_INFO("GfxWorld light grid: %zu voxels from %u probes and %u tetrahedrons, %u palette entries, %u of %u static models with a baked probe",
			samples.size(), asset->probeData.probeCount, asset->probeData.tetrahedronCount, new_grid.paletteEntryCount, baked_models, world->dpvs.smodelCount);
	}
}
