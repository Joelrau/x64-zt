#pragma once

// light grid of the IW6 engine family (IW6, S1, H1, S2), ported from zonetool iw7-ai-branch (src/X64/Utils/LightGrid)
namespace light_grid
{
	constexpr auto basis_dir_count = 56u;
	extern const float basis_dirs[basis_dir_count][3];

	struct tree_view
	{
		unsigned char max_depth;
		int node_count;
		const int* coord_min_grid_space;
		const int* coord_max_grid_space;
		int default_color_index_bit_count;
		int default_light_index_bit_count;
		const unsigned int* node_table;
		int leaf_table_size;
		const unsigned char* leaf_table;
		int leaf_size_bit_count; // 7 on S1 and H1, 6 on IW6
	};

	template <typename T>
	tree_view make_tree_view(const T& tree, const int leaf_size_bit_count)
	{
		return {tree.maxDepth, tree.nodeCount, tree.coordMinGridSpace, tree.coordMaxGridSpace, tree.defaultColorIndexBitCount,
			tree.defaultLightIndexBitCount, tree.p_nodeTable, tree.leafTableSize, tree.p_leafTable, leaf_size_bit_count};
	}

	struct voxel
	{
		unsigned int color_index; // 0 is empty
		unsigned short light_index; // primary light env index
		bool trace_low;
		bool trace_high;
	};

	bool find_voxel(const tree_view& tree, const unsigned short grid_pos[3], voxel& out, bool* stream_overflow = nullptr);

	struct palette_config
	{
		char range_exp_8bits;
		char range_exp_12bits;
		char range_exp_16bits;
	};

	struct palette_sh
	{
		float coeffs[9][3]; // basis 1, x, y, z, xz, yz, xy, 3z^2 - 1, x^2 - y^2
	};

	void decode_palette_entry(const palette_config& config, const unsigned char* stream, palette_sh& out);
	void evaluate(const palette_sh& sh, float radiance[basis_dir_count][3]);

	constexpr auto probe_sh_count = 27u; // GfxSHProbeData of IW7 and S2: orthonormal SH, red, then green, then blue
	void project_probe_sh(const float radiance[basis_dir_count][3], float out[probe_sh_count]);
}
