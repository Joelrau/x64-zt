#pragma once

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
		int leaf_size_bit_count;
	};

	template <typename T>
	tree_view make_tree_view(const T& tree, const int leaf_size_bit_count)
	{
		return {tree.maxDepth, tree.nodeCount, tree.coordMinGridSpace, tree.coordMaxGridSpace, tree.defaultColorIndexBitCount,
			tree.defaultLightIndexBitCount, tree.p_nodeTable, tree.leafTableSize, tree.p_leafTable, leaf_size_bit_count};
	}

	struct voxel
	{
		unsigned int color_index;
		unsigned short light_index;
		bool trace_low;
		bool trace_high;
	};

	bool find_voxel(const tree_view& tree, const unsigned short grid_pos[3], voxel& out, bool* stream_overflow = nullptr);

	struct grid_sample
	{
		unsigned short grid_pos[3];
		voxel value;
	};

	struct tree_data
	{
		unsigned char max_depth;
		int leaf_count;
		int coord_min_grid_space[3];
		int coord_max_grid_space[3];
		int coord_half_size_grid_space[3];
		int default_color_index_bit_count;
		int default_light_index_bit_count;
		std::vector<unsigned int> node_table;
		std::vector<unsigned char> leaf_table;
	};

	tree_data build_tree(const std::vector<grid_sample>& samples, int leaf_size_bit_count);
	bool convert_leaf_table(const tree_view& tree, int leaf_size_bit_count, std::vector<unsigned int>& node_table, std::vector<unsigned char>& leaf_table);

	struct palette_config
	{
		char range_exp_8bits;
		char range_exp_12bits;
		char range_exp_16bits;
	};

	struct palette_sh
	{
		float coeffs[9][3];
	};

	void decode_palette_entry(const palette_config& config, const unsigned char* stream, palette_sh& out);
	int encode_palette_entry(const palette_config& config, const palette_sh& sh, std::vector<unsigned char>& stream);
	void evaluate(const palette_sh& sh, float radiance[basis_dir_count][3]);

	constexpr auto probe_sh_count = 27u;
	void project_probe_sh(const float radiance[basis_dir_count][3], float out[probe_sh_count]);
	void probe_sh_to_palette(const float probe[probe_sh_count], palette_sh& out);

	struct mask_table_entry
	{
		unsigned short mask;
		unsigned char bit_count;
		unsigned char bit_index[8];
	};

	class bit_reader
	{
	public:
		bit_reader(const unsigned char* data, const std::size_t size)
			: data_(data)
			, bit_count_(size * 8)
		{
		}

		unsigned int read(const int bits)
		{
			auto value = 0u;
			for (auto i = 0; i < bits; i++)
			{
				if (this->position_ >= this->bit_count_)
				{
					this->overflow_ = true;
					return value;
				}
				value |= static_cast<unsigned int>((this->data_[this->position_ >> 3] >> (this->position_ & 7)) & 1) << i;
				this->position_++;
			}
			return value;
		}

		int read_int(const int bits)
		{
			return static_cast<int>(this->read(bits));
		}

		void align_byte()
		{
			this->position_ = (this->position_ + 7) & ~7ull;
		}

		void skip_bytes(const std::size_t bytes)
		{
			this->position_ += bytes * 8;
		}

		void seek(const std::size_t bit)
		{
			this->position_ = bit;
		}

		std::size_t position() const
		{
			return this->position_;
		}

		bool overflowed() const
		{
			return this->overflow_;
		}

	private:
		const unsigned char* data_;
		std::size_t bit_count_;
		std::size_t position_ = 0;
		bool overflow_ = false;
	};

	class bit_writer
	{
	public:
		void write(const unsigned int value, const int bits)
		{
			for (auto i = 0; i < bits; i++)
			{
				if ((this->position_ & 7) == 0)
				{
					this->data_.emplace_back(static_cast<unsigned char>(0));
				}

				this->data_.back() |= static_cast<unsigned char>(((value >> i) & 1) << (this->position_ & 7));
				this->position_++;
			}
		}

		void align_byte()
		{
			this->position_ = (this->position_ + 7) & ~7ull;
		}

		const std::vector<unsigned char>& data() const
		{
			return this->data_;
		}

	private:
		std::vector<unsigned char> data_;
		std::size_t position_ = 0;
	};

	struct node_index_info
	{
		int index_bit_count;
		int index_min;
		int encoded_index_default;
		int index_table_size;
		int index_table_size_bit_count;
		int raw_index_table[16];
		int raw_index_default;
	};

	struct chunk_header
	{
		unsigned int format;
		int leaf_size[8];
		node_index_info color;
		node_index_info light;
	};

	struct leaf_index_channel
	{
		int values[16];
	};

	struct leaf_block
	{
		leaf_index_channel color;
		leaf_index_channel light;
		unsigned int trace_mask;
	};
}
