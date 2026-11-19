#pragma once

namespace probe_volume
{
	constexpr auto max_probes = 0x1FFFFu;
	constexpr auto leaf_children_flag = 0x80000000u;
	constexpr auto node_size = 4;
	constexpr auto root_size_in_leaves = node_size * node_size;

	struct top_down_node
	{
		int first_node_index;
		int z_min;
		int z_max;
	};

	struct internal_node
	{
		unsigned int first_node_index[2];
		unsigned int child_node_mask[2];
	};

	struct volume
	{
		std::vector<std::array<float, 3>> probe_positions;
		std::vector<std::array<unsigned int, 4>> tetrahedrons;
		std::vector<std::array<unsigned int, 4>> tetrahedron_neighbors;
		std::vector<unsigned int> voxel_start_tetrahedron;

		int root_node_dimension[3];
		int node_coord_bit_shift[3];
		float bound_min[3];
		float bound_max[3];
		std::vector<top_down_node> top_down_nodes;
		std::vector<internal_node> internal_nodes;
		unsigned int leaf_count;
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
		grid_3d blocks;
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

	struct open_face
	{
		unsigned int tetrahedron;
		int opposite_vertex;
	};

	using cell_test = std::function<bool(const float min[3], float size)>;

	std::optional<volume> build(const float bounds_min[3], const float bounds_max[3], const cell_test& cell_occupied);
}
