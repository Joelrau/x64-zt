#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace lightgrid_probes
{
	constexpr unsigned int max_probes = 0x1FFFF;

	struct build_params
	{
		float bounds_min[3];
		float bounds_max[3];

		int leaf_shift = 0;

		bool corner_flag = false;

		std::function<bool(const float lo[3], float size)> cell_occupied;
	};

	struct voxel_tree_node
	{
		int first_node_index[2];
		unsigned int child_node_mask[2];
	};

	struct top_down_view_node
	{
		int first_node_index;
		int z_min;
		int z_max;
	};

	struct probe_volume
	{
		std::vector<unsigned short> probes;         
		std::vector<float> probe_positions;         
		std::vector<unsigned int> tetrahedrons;     
		std::vector<unsigned int> tetrahedron_neighbors;
		std::vector<unsigned int> voxel_start_tetrahedron;

		unsigned int probe_count = 0;
		unsigned int tetrahedron_count = 0;

		unsigned int zone_num_probes = 0;
		unsigned int zone_first_probe = 0;
		unsigned int zone_num_tetrahedrons = 0;
		unsigned int zone_first_tetrahedron = 0;
		unsigned int zone_first_voxel_tetrahedron_index = 0;
		unsigned int zone_num_voxel_tetrahedron_indices = 0;
		unsigned short zone_fallback_coeffs[29]{};

		int root_node_dimension[4]{};
		int node_coord_bit_shift[4]{};
		float bound_min[4]{};
		float bound_max[4]{};
		std::vector<top_down_view_node> top_down_view_nodes;
		std::vector<voxel_tree_node> internal_nodes;
		std::vector<unsigned short> leaf_nodes;     
		std::vector<unsigned short> light_list;     

		unsigned int leaf_count = 0;

		std::vector<float> leaf_bounds_min;         
		float leaf_size = 0.0f;

		bool valid = false;
	};

	using sampler = std::function<void(const float position[3], float out_sh[28])>;

	void project_sh(const float samples[][3], const float directions[][3], unsigned int count,
		float scale, float out_sh[27]);

	void constant_sh(const float rgb[3], float scale, float out_sh[27]);

	void encode_probe_sh(const float sh[28], unsigned short out_coeffs[32]);

	probe_volume build(const build_params& params, const sampler& sample, float sh_scale);
}
