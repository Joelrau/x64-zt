#pragma once

#include <cstdint>
#include <vector>

namespace lightgrid_tree
{
	enum class leaf_size_bits : int
	{
		iw6 = 6,
		h1 = 7, 
	};

	struct grid_sample
	{
		unsigned short pos[3];
		unsigned int color_index;  
		unsigned short light_index;
		bool trace_lo;             
		bool trace_hi;             
	};

	struct raw_result
	{
		unsigned int color_index;
		unsigned short light_index;
		bool trace_lo;
		bool trace_hi;
	};

	struct tree_data
	{
		unsigned char max_depth;
		int node_count;
		int leaf_count;
		int coord_min_grid_space[3];
		int coord_max_grid_space[3];
		int coord_half_size_grid_space[3];
		int default_color_index_bit_count;
		int default_light_index_bit_count;
		std::vector<unsigned int> node_table;
		std::vector<unsigned char> leaf_table;
	};

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
		leaf_size_bits size_bits;
	};

	tree_data build_tree(const grid_sample* samples, size_t sample_count,
		leaf_size_bits size_bits = leaf_size_bits::h1);

	void dilate_samples(std::vector<grid_sample>& samples, int passes = 1,
		const unsigned char* sample_classes = nullptr);

	bool lookup(const tree_view& tree, const unsigned short pos[3], raw_result& out,
		bool* unsupported = nullptr);
	bool lookup(const tree_data& tree, const unsigned short pos[3], raw_result& out);

	struct grid_entry_ref
	{
		unsigned short pos[3];
		unsigned int entry_index;
	};

	std::vector<grid_entry_ref> enumerate_row_data(
		const unsigned short mins[3], const unsigned short maxs[3],
		unsigned int row_axis, unsigned int col_axis,
		const unsigned short* row_data_start, const unsigned char* raw_row_data);
}
