#pragma once

// tetrahedral light probe volume and voxel tree of IW7 and S2, ported from zonetool iw7-ai-branch (src/X64/Utils/LightGrid/LightGridProbes)
namespace probe_volume
{
	constexpr auto max_probes = 0x1FFFFu; // the probe index of a tetrahedron is 17 bits
	constexpr auto leaf_children_flag = 0x80000000u;

	struct top_down_node
	{
		int first_node_index;
		int z_min;
		int z_max;
	};

	struct internal_node
	{
		unsigned int first_node_index[2]; // one base per mask half
		unsigned int child_node_mask[2];
	};

	struct volume
	{
		std::vector<std::array<float, 3>> probe_positions;
		std::vector<std::array<unsigned int, 4>> tetrahedrons;
		std::vector<std::array<unsigned int, 4>> tetrahedron_neighbors;
		std::vector<unsigned int> voxel_start_tetrahedron; // leaves, then internal nodes

		int root_node_dimension[3];
		int node_coord_bit_shift[3];
		float bound_min[3];
		float bound_max[3];
		std::vector<top_down_node> top_down_nodes;
		std::vector<internal_node> internal_nodes;
		unsigned int leaf_count;
	};

	using cell_test = std::function<bool(const float min[3], float size)>;

	std::optional<volume> build(const float bounds_min[3], const float bounds_max[3], const cell_test& cell_occupied);
}
