#include <std_include.hpp>

#include "probe_volume.hpp"

namespace probe_volume
{
	namespace
	{
		constexpr auto no_probe = 0xFFFFFFFFu;
		constexpr auto no_tetrahedron = 0xFFFFFFFFu;
		constexpr auto seed_tetrahedron = 0u;
		constexpr auto probe_offset = 1.0f / 32.0f; // stock probes sit 1/32 unit off the voxel grid
		constexpr auto first_leaf_shift = 5;
		constexpr auto last_leaf_shift = 12;
		constexpr auto node_size = 4; // children per axis of an internal node
		constexpr auto root_size_in_leaves = node_size * node_size;
		constexpr auto max_scanned_cells = 12ull * 1024 * 1024;

		constexpr int cube_tetrahedrons[6][4] = // Freudenthal split, corner bits: 1 = +x, 2 = +y, 4 = +z
		{
			{0, 1, 3, 7}, {0, 3, 2, 7}, {0, 2, 6, 7}, {0, 6, 4, 7}, {0, 4, 5, 7}, {0, 5, 1, 7},
		};

		struct cell
		{
			int x;
			int y;
			int z;
		};

		struct grid_3d
		{
			int size[3];

			std::size_t count() const
			{
				return static_cast<std::size_t>(this->size[0]) * this->size[1] * this->size[2];
			}

			std::size_t index(const int x, const int y, const int z) const
			{
				return (static_cast<std::size_t>(z) * this->size[1] + y) * this->size[0] + x;
			}
		};

		struct leaf_grid
		{
			int leaf_shift;
			float leaf_size;
			float origin[3];
			grid_3d roots;
			grid_3d blocks; // the level 1 nodes, node_size leaves per axis
			grid_3d cells;
			grid_3d corners;

			leaf_grid(const float bounds_min[3], const float bounds_max[3], const int leaf_shift)
				: leaf_shift(leaf_shift)
				, leaf_size(static_cast<float>(1 << leaf_shift))
			{
				const auto root_size = this->leaf_size * root_size_in_leaves;
				for (auto axis = 0; axis < 3; axis++)
				{
					this->origin[axis] = std::floor(bounds_min[axis] / root_size) * root_size;
					this->roots.size[axis] = std::max(1, static_cast<int>(std::ceil((bounds_max[axis] - this->origin[axis]) / root_size)));
					this->blocks.size[axis] = this->roots.size[axis] * node_size;
					this->cells.size[axis] = this->roots.size[axis] * root_size_in_leaves;
					this->corners.size[axis] = this->cells.size[axis] + 1;
				}
			}

			void cell_min(const cell& c, float out[3]) const
			{
				out[0] = this->origin[0] + static_cast<float>(c.x) * this->leaf_size;
				out[1] = this->origin[1] + static_cast<float>(c.y) * this->leaf_size;
				out[2] = this->origin[2] + static_cast<float>(c.z) * this->leaf_size;
			}

			std::size_t corner_index(const cell& c, const int corner) const
			{
				return this->corners.index(c.x + (corner & 1), c.y + ((corner >> 1) & 1), c.z + ((corner >> 2) & 1));
			}
		};

		bool overlaps(const float min[3], const float size, const float bounds_min[3], const float bounds_max[3])
		{
			for (auto axis = 0; axis < 3; axis++)
			{
				if (min[axis] + size <= bounds_min[axis] || min[axis] >= bounds_max[axis])
				{
					return false;
				}
			}
			return true;
		}

		std::vector<bool> find_present_cells(const leaf_grid& grid, const float bounds_min[3], const float bounds_max[3], const cell_test& cell_occupied)
		{
			std::vector<bool> present(grid.cells.count());
			for (auto z = 0; z < grid.cells.size[2]; z++)
			{
				for (auto y = 0; y < grid.cells.size[1]; y++)
				{
					for (auto x = 0; x < grid.cells.size[0]; x++)
					{
						float min[3];
						grid.cell_min({x, y, z}, min);
						present[grid.cells.index(x, y, z)] = overlaps(min, grid.leaf_size, bounds_min, bounds_max)
							&& (!cell_occupied || cell_occupied(min, grid.leaf_size));
					}
				}
			}
			return present;
		}

		std::vector<unsigned int> number_probes(const leaf_grid& grid, const std::vector<bool>& present, unsigned int& probe_count)
		{
			std::vector<bool> used(grid.corners.count());
			for (auto z = 0; z < grid.cells.size[2]; z++)
			{
				for (auto y = 0; y < grid.cells.size[1]; y++)
				{
					for (auto x = 0; x < grid.cells.size[0]; x++)
					{
						if (!present[grid.cells.index(x, y, z)])
						{
							continue;
						}

						for (auto corner = 0; corner < 8; corner++)
						{
							used[grid.corner_index({x, y, z}, corner)] = true;
						}
					}
				}
			}

			std::vector<unsigned int> probe_of_corner(grid.corners.count(), no_probe);
			probe_count = 0;
			for (auto i = 0u; i < used.size(); i++)
			{
				if (used[i])
				{
					probe_of_corner[i] = probe_count++;
				}
			}
			return probe_of_corner;
		}

		internal_node make_node(const unsigned int first_child, const std::uint64_t child_mask, const bool leaf_children)
		{
			const auto low_mask = static_cast<unsigned int>(child_mask);
			const auto flag = leaf_children ? leaf_children_flag : 0u;

			internal_node node{};
			node.first_node_index[0] = first_child | flag;
			node.first_node_index[1] = (first_child + std::popcount(low_mask)) | flag;
			node.child_node_mask[0] = low_mask;
			node.child_node_mask[1] = static_cast<unsigned int>(child_mask >> 32);
			return node;
		}

		int child_index(const int x, const int y, const int z)
		{
			return (z << 4) | (y << 2) | x;
		}

		void add_probe_positions(const leaf_grid& grid, const std::vector<unsigned int>& probe_of_corner, volume& out)
		{
			for (auto z = 0; z < grid.corners.size[2]; z++)
			{
				for (auto y = 0; y < grid.corners.size[1]; y++)
				{
					for (auto x = 0; x < grid.corners.size[0]; x++)
					{
						const auto probe = probe_of_corner[grid.corners.index(x, y, z)];
						if (probe == no_probe)
						{
							continue;
						}

						float min[3];
						grid.cell_min({x, y, z}, min);
						out.probe_positions[probe] = {min[0] + probe_offset, min[1] + probe_offset, min[2] + probe_offset};
					}
				}
			}
		}

		void add_tree(const leaf_grid& grid, const std::vector<bool>& present, std::vector<cell>& leaves, volume& out)
		{
			std::vector<bool> block_content(grid.blocks.count());
			std::vector<bool> root_content(grid.roots.count());
			for (auto z = 0; z < grid.cells.size[2]; z++)
			{
				for (auto y = 0; y < grid.cells.size[1]; y++)
				{
					for (auto x = 0; x < grid.cells.size[0]; x++)
					{
						if (present[grid.cells.index(x, y, z)])
						{
							block_content[grid.blocks.index(x / node_size, y / node_size, z / node_size)] = true;
							root_content[grid.roots.index(x / root_size_in_leaves, y / root_size_in_leaves, z / root_size_in_leaves)] = true;
						}
					}
				}
			}

			std::vector<cell> roots;
			out.top_down_nodes.resize(static_cast<std::size_t>(grid.roots.size[0]) * grid.roots.size[1]);
			for (auto y = 0; y < grid.roots.size[1]; y++)
			{
				for (auto x = 0; x < grid.roots.size[0]; x++)
				{
					auto& column = out.top_down_nodes[static_cast<std::size_t>(y) * grid.roots.size[0] + x];
					column = {-1, std::numeric_limits<int>::max(), std::numeric_limits<int>::min()};
					for (auto z = 0; z < grid.roots.size[2]; z++)
					{
						if (root_content[grid.roots.index(x, y, z)])
						{
							column.z_min = std::min(column.z_min, z);
							column.z_max = std::max(column.z_max, z);
						}
					}

					if (column.z_min > column.z_max)
					{
						continue;
					}

					column.first_node_index = static_cast<int>(out.internal_nodes.size() + roots.size());
					for (auto z = column.z_min; z <= column.z_max; z++)
					{
						roots.push_back({x, y, z});
					}
				}
			}

			const auto first_block_node = static_cast<unsigned int>(out.internal_nodes.size() + roots.size());
			std::vector<cell> blocks;
			for (const auto& root : roots)
			{
				std::uint64_t mask = 0;
				const auto first_child = first_block_node + static_cast<unsigned int>(blocks.size());
				for (auto z = 0; z < node_size; z++)
				{
					for (auto y = 0; y < node_size; y++)
					{
						for (auto x = 0; x < node_size; x++)
						{
							const cell block{root.x * node_size + x, root.y * node_size + y, root.z * node_size + z};
							if (block_content[grid.blocks.index(block.x, block.y, block.z)])
							{
								mask |= 1ull << child_index(x, y, z);
							}
						}
					}
				}

				for (auto child = 0; child < 64; child++)
				{
					if (mask & (1ull << child))
					{
						blocks.push_back({root.x * node_size + (child & 3), root.y * node_size + ((child >> 2) & 3), root.z * node_size + (child >> 4)});
					}
				}
				out.internal_nodes.push_back(make_node(first_child, mask, false));
			}

			for (const auto& block : blocks)
			{
				std::uint64_t mask = 0;
				const auto first_child = static_cast<unsigned int>(leaves.size());
				for (auto child = 0; child < 64; child++)
				{
					const cell leaf{block.x * node_size + (child & 3), block.y * node_size + ((child >> 2) & 3), block.z * node_size + (child >> 4)};
					if (present[grid.cells.index(leaf.x, leaf.y, leaf.z)])
					{
						mask |= 1ull << child;
						leaves.push_back(leaf);
					}
				}
				out.internal_nodes.push_back(make_node(first_child, mask, true));
			}
		}

		void add_tetrahedrons(const leaf_grid& grid, const std::vector<unsigned int>& probe_of_corner, const std::vector<cell>& leaves, volume& out)
		{
			out.voxel_start_tetrahedron.assign(leaves.size() + out.internal_nodes.size(), no_tetrahedron);
			out.voxel_start_tetrahedron[0] = seed_tetrahedron;
			out.voxel_start_tetrahedron[leaves.size()] = seed_tetrahedron;

			for (auto leaf = 1u; leaf < leaves.size(); leaf++)
			{
				unsigned int corner_probe[8];
				for (auto corner = 0; corner < 8; corner++)
				{
					corner_probe[corner] = probe_of_corner[grid.corner_index(leaves[leaf], corner)];
				}

				out.voxel_start_tetrahedron[leaf] = static_cast<unsigned int>(out.tetrahedrons.size());
				for (const auto& tetrahedron : cube_tetrahedrons)
				{
					out.tetrahedrons.push_back({corner_probe[tetrahedron[0]], corner_probe[tetrahedron[1]], corner_probe[tetrahedron[2]], corner_probe[tetrahedron[3]]});
				}
			}

			// a node walk starts at the tetrahedrons of its first leaf, the internal nodes follow the leaves
			for (auto node = out.internal_nodes.size() - 1; node > 0; node--)
			{
				const auto& internal = out.internal_nodes[node];
				if (!internal.child_node_mask[0] && !internal.child_node_mask[1])
				{
					continue;
				}

				const auto first_child = internal.first_node_index[0] & ~leaf_children_flag;
				const auto child_entry = (internal.first_node_index[0] & leaf_children_flag) ? first_child : static_cast<unsigned int>(leaves.size()) + first_child;
				out.voxel_start_tetrahedron[leaves.size() + node] = out.voxel_start_tetrahedron[child_entry];
			}
		}

		std::uint64_t face_key(unsigned int a, unsigned int b, unsigned int c)
		{
			if (a > b) std::swap(a, b);
			if (b > c) std::swap(b, c);
			if (a > b) std::swap(a, b);
			return (static_cast<std::uint64_t>(a) << 34) | (static_cast<std::uint64_t>(b) << 17) | c;
		}

		void add_neighbors(volume& out)
		{
			struct open_face
			{
				unsigned int tetrahedron;
				int opposite_vertex;
			};

			out.tetrahedron_neighbors.assign(out.tetrahedrons.size(), {no_tetrahedron, no_tetrahedron, no_tetrahedron, no_tetrahedron});
			std::unordered_map<std::uint64_t, open_face> open_faces;
			for (auto i = 0u; i < out.tetrahedrons.size(); i++)
			{
				const auto& probes = out.tetrahedrons[i];
				for (auto vertex = 0; vertex < 4; vertex++)
				{
					const auto key = face_key(probes[(vertex + 1) & 3], probes[(vertex + 2) & 3], probes[(vertex + 3) & 3]);
					const auto other = open_faces.find(key);
					if (other == open_faces.end())
					{
						open_faces.emplace(key, open_face{i, vertex});
						continue;
					}

					out.tetrahedron_neighbors[i][vertex] = other->second.tetrahedron;
					out.tetrahedron_neighbors[other->second.tetrahedron][other->second.opposite_vertex] = i;
					open_faces.erase(other);
				}
			}
		}

		volume make_volume(const leaf_grid& grid, const std::vector<bool>& present, const std::vector<unsigned int>& probe_of_corner, const unsigned int probe_count)
		{
			volume out{};
			out.probe_positions.resize(probe_count);
			add_probe_positions(grid, probe_of_corner, out);

			std::vector<cell> leaves(1); // stock maps leave leaf 0 and internal node 0 unused
			out.internal_nodes.resize(1);
			add_tree(grid, present, leaves, out);
			add_tetrahedrons(grid, probe_of_corner, leaves, out);
			add_neighbors(out);

			const int shift[3] = {grid.leaf_shift + 4, grid.leaf_shift + 2, grid.leaf_shift};
			const auto root_size = grid.leaf_size * root_size_in_leaves;
			for (auto axis = 0; axis < 3; axis++)
			{
				out.root_node_dimension[axis] = grid.roots.size[axis];
				out.node_coord_bit_shift[axis] = shift[axis];
				out.bound_min[axis] = grid.origin[axis];
				out.bound_max[axis] = grid.origin[axis] + static_cast<float>(grid.roots.size[axis]) * root_size;
			}
			out.leaf_count = static_cast<unsigned int>(leaves.size());
			return out;
		}
	}

	std::optional<volume> build(const float bounds_min[3], const float bounds_max[3], const cell_test& cell_occupied)
	{
		for (auto leaf_shift = first_leaf_shift; leaf_shift <= last_leaf_shift; leaf_shift++)
		{
			const leaf_grid grid(bounds_min, bounds_max, leaf_shift);
			if (grid.cells.count() > max_scanned_cells)
			{
				continue;
			}

			const auto present = find_present_cells(grid, bounds_min, bounds_max, cell_occupied);
			auto probe_count = 0u;
			const auto probe_of_corner = number_probes(grid, present, probe_count);
			if (!probe_count)
			{
				return {};
			}

			if (probe_count <= max_probes)
			{
				return make_volume(grid, present, probe_of_corner, probe_count);
			}
		}
		return {};
	}
}
