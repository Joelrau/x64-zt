#include <std_include.hpp>

#include "light_grid.hpp"

namespace light_grid
{
	const float basis_dirs[basis_dir_count][3] =
	{
		{-0.57727051f, -0.57727051f, -0.57727051f}, {-0.22941080f, -0.68823242f, -0.68823242f},
		{0.22941084f, -0.68823242f, -0.68823242f}, {0.57727051f, -0.57727051f, -0.57727051f},
		{-0.68823242f, -0.22941080f, -0.68823242f}, {-0.30151364f, -0.30151364f, -0.90454102f},
		{0.30151370f, -0.30151364f, -0.90454102f}, {0.68823242f, -0.22941080f, -0.68823242f},
		{-0.68823242f, 0.22941084f, -0.68823242f}, {-0.30151364f, 0.30151370f, -0.90454102f},
		{0.30151370f, 0.30151370f, -0.90454102f}, {0.68823242f, 0.22941084f, -0.68823242f},
		{-0.57727051f, 0.57727051f, -0.57727051f}, {-0.22941080f, 0.68823242f, -0.68823242f},
		{0.22941084f, 0.68823242f, -0.68823242f}, {0.57727051f, 0.57727051f, -0.57727051f},
		{-0.68823242f, -0.68823242f, -0.22941080f}, {-0.30151364f, -0.90454102f, -0.30151364f},
		{0.30151370f, -0.90454102f, -0.30151364f}, {0.68823242f, -0.68823242f, -0.22941080f},
		{-0.90454102f, -0.30151364f, -0.30151364f}, {0.90454102f, -0.30151364f, -0.30151364f},
		{-0.90454102f, 0.30151370f, -0.30151364f}, {0.90454102f, 0.30151370f, -0.30151364f},
		{-0.68823242f, 0.68823242f, -0.22941080f}, {-0.30151364f, 0.90454102f, -0.30151364f},
		{0.30151370f, 0.90454102f, -0.30151364f}, {0.68823242f, 0.68823242f, -0.22941080f},
		{-0.68823242f, -0.68823242f, 0.22941084f}, {-0.30151364f, -0.90454102f, 0.30151370f},
		{0.30151370f, -0.90454102f, 0.30151370f}, {0.68823242f, -0.68823242f, 0.22941084f},
		{-0.90454102f, -0.30151364f, 0.30151370f}, {0.90454102f, -0.30151364f, 0.30151370f},
		{-0.90454102f, 0.30151370f, 0.30151370f}, {0.90454102f, 0.30151370f, 0.30151370f},
		{-0.68823242f, 0.68823242f, 0.22941084f}, {-0.30151364f, 0.90454102f, 0.30151370f},
		{0.30151370f, 0.90454102f, 0.30151370f}, {0.68823242f, 0.68823242f, 0.22941084f},
		{-0.57727051f, -0.57727051f, 0.57727051f}, {-0.22941080f, -0.68823242f, 0.68823242f},
		{0.22941084f, -0.68823242f, 0.68823242f}, {0.57727051f, -0.57727051f, 0.57727051f},
		{-0.68823242f, -0.22941080f, 0.68823242f}, {-0.30151364f, -0.30151364f, 0.90454102f},
		{0.30151370f, -0.30151364f, 0.90454102f}, {0.68823242f, -0.22941080f, 0.68823242f},
		{-0.68823242f, 0.22941084f, 0.68823242f}, {-0.30151364f, 0.30151370f, 0.90454102f},
		{0.30151370f, 0.30151370f, 0.90454102f}, {0.68823242f, 0.22941084f, 0.68823242f},
		{-0.57727051f, 0.57727051f, 0.57727051f}, {-0.22941080f, 0.68823242f, 0.68823242f},
		{0.22941084f, 0.68823242f, 0.68823242f}, {0.57727051f, 0.57727051f, 0.57727051f},
	};

	namespace
	{
		constexpr unsigned char voxel_index_table[16]{0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15};

		constexpr mask_table_entry voxel_mask_table[16] =
		{
			{0x0033, 4, {0, 1, 4, 5}}, {0x00CC, 4, {2, 3, 6, 7}}, {0x3300, 4, {8, 9, 12, 13}}, {0xCC00, 4, {10, 11, 14, 15}},
			{0x0505, 4, {0, 2, 8, 10}}, {0x0A0A, 4, {1, 3, 9, 11}}, {0x5050, 4, {4, 6, 12, 14}}, {0xA0A0, 4, {5, 7, 13, 15}},
			{0x000F, 4, {0, 1, 2, 3}}, {0x005A, 4, {1, 4, 3, 6}}, {0x00F0, 4, {4, 5, 6, 7}}, {0x030C, 4, {2, 3, 8, 9}},
			{0x30C0, 4, {6, 7, 12, 13}}, {0x0F00, 4, {8, 9, 10, 11}}, {0x5A00, 4, {9, 12, 11, 14}}, {0xF000, 4, {12, 13, 14, 15}},
		};

		int bit_length(int value)
		{
			auto bits = 0;
			for (; value > 0; value >>= 1)
			{
				bits++;
			}
			return bits;
		}

		int bit_count_field_width(const int count)
		{
			if (count <= 0)
			{
				return 0;
			}
			return bit_length(std::max(count - 1, 1));
		}

		int bit_count_below(const unsigned int mask, const int index)
		{
			return std::popcount(mask & ((1u << index) - 1));
		}

		void read_node_index(bit_reader& reader, const bool has_range, const bool has_default, const bool has_table,
			const int default_bit_count, const int fallback_bit_count, node_index_info& info)
		{
			info = {};
			if (has_range)
			{
				info.index_min = reader.read_int(default_bit_count);
				info.index_bit_count = reader.read_int(bit_count_field_width(default_bit_count)) + 1;
			}
			else
			{
				info.index_bit_count = fallback_bit_count;
			}

			info.encoded_index_default = has_default ? reader.read_int(info.index_bit_count) : 0;

			if (has_table)
			{
				info.index_table_size = reader.read_int(4) + 1;
				info.index_table_size_bit_count = bit_count_field_width(info.index_table_size);
				for (auto i = 1; i < info.index_table_size; i++)
				{
					const auto encoded = reader.read_int(info.index_bit_count);
					info.raw_index_table[i] = encoded ? encoded + info.index_min - 1 : 0;
				}
			}

			if (info.index_table_size)
			{
				info.raw_index_default = info.raw_index_table[info.encoded_index_default & 15];
			}
			else
			{
				info.raw_index_default = info.encoded_index_default ? info.encoded_index_default + info.index_min - 1 : 0;
			}
		}

		void read_chunk_header(bit_reader& reader, const unsigned int chunk_mask, const tree_view& tree, chunk_header& header)
		{
			reader.align_byte();
			header.format = reader.read(8);

			std::memset(header.leaf_size, 0, sizeof(header.leaf_size));
			const auto leaf_count = std::popcount(chunk_mask & 0xFF);
			for (auto i = 0; i < leaf_count; i++)
			{
				header.leaf_size[i] = reader.read_int(tree.leaf_size_bit_count);
			}

			read_node_index(reader, header.format & 0x1, header.format & 0x2, header.format & 0x4,
				tree.default_color_index_bit_count, 16, header.color);
			read_node_index(reader, header.format & 0x8, header.format & 0x10, header.format & 0x20,
				tree.default_light_index_bit_count, 1, header.light);
			reader.align_byte();
		}

		unsigned int read_table_mask_bits(bit_reader& reader, const mask_table_entry& entry, const bool invert)
		{
			const auto bits = reader.read(entry.bit_count);
			auto mask = 0u;
			for (auto i = 0; i < entry.bit_count; i++)
			{
				if ((bits >> i) & 1)
				{
					mask |= 1u << entry.bit_index[i];
				}
			}
			return invert ? (~mask & 0xFFFF) : mask;
		}

		unsigned int read_voxel_mask(bit_reader& reader)
		{
			const auto selector = reader.read(2);
			if (selector == 0)
			{
				return reader.read(16);
			}
			if (selector == 1)
			{
				return 0xFFFF;
			}
			const auto& entry = voxel_mask_table[reader.read(4) & 15];
			return read_table_mask_bits(reader, entry, selector == 3);
		}

		unsigned int read_index_mask(bit_reader& reader)
		{
			if (reader.read(1) == 0)
			{
				return reader.read(16);
			}
			const auto& entry = voxel_mask_table[reader.read(4) & 15];
			const auto invert = reader.read(1) != 0;
			return read_table_mask_bits(reader, entry, invert);
		}

		void set_masked(int values[16], const unsigned int mask, const int value)
		{
			for (auto i = 0; i < 16; i++)
			{
				if ((mask >> i) & 1)
				{
					values[i] = value;
				}
			}
		}

		void exec_leaf_commands(bit_reader& reader, const int encoded_bits, int encoded[16])
		{
			auto run = true;
			while (run && !reader.overflowed())
			{
				run = false;
				switch (reader.read(2))
				{
				case 0:
					for (auto i = 0; i < 16; i++)
					{
						encoded[i] = reader.read_int(encoded_bits);
					}
					break;
				case 1:
				{
					const auto count = reader.read_int(1) + 2;
					auto mask = 0u;
					for (auto i = 0; i < count; i++)
					{
						mask |= 1u << (reader.read(4) & 15);
					}
					set_masked(encoded, mask, reader.read_int(encoded_bits));
					run = reader.read(1) != 0;
					break;
				}
				case 2:
				{
					const auto mask = read_index_mask(reader);
					for (auto i = 0; i < 16; i++)
					{
						if ((mask >> i) & 1)
						{
							encoded[i] = reader.read_int(encoded_bits);
						}
					}
					break;
				}
				default:
				{
					const auto mask = read_index_mask(reader);
					set_masked(encoded, mask, reader.read_int(encoded_bits));
					run = reader.read(1) != 0;
					break;
				}
				}
			}
		}

		void read_leaf_channel(bit_reader& reader, const node_index_info& node, const bool has_range, const bool has_default,
			const bool has_commands, leaf_index_channel& out)
		{
			auto encoded_min = 0;
			auto encoded_bits = 0;
			auto table_mode = false;
			if (has_range)
			{
				encoded_min = reader.read_int(node.index_bit_count);
				encoded_bits = reader.read_int(bit_count_field_width(node.index_bit_count)) + 1;
			}
			else if (node.index_table_size_bit_count)
			{
				encoded_min = 1;
				encoded_bits = node.index_table_size_bit_count;
				table_mode = true;
			}
			else
			{
				encoded_min = node.index_min != 0;
				encoded_bits = node.index_bit_count;
			}

			auto encoded_default = 0;
			if (has_default)
			{
				encoded_default = reader.read_int(encoded_bits);
			}
			else if (table_mode)
			{
				encoded_default = node.encoded_index_default;
			}
			else
			{
				const auto base = encoded_min ? node.index_min + encoded_min - 1 : 0;
				encoded_default = node.raw_index_default ? node.raw_index_default - base + 1 : 0;
			}

			int encoded[16];
			std::fill_n(encoded, 16, encoded_default);
			if (has_commands)
			{
				exec_leaf_commands(reader, encoded_bits, encoded);
			}

			if (node.index_table_size_bit_count && node.index_table_size_bit_count <= encoded_bits)
			{
				for (auto i = 0; i < 16; i++)
				{
					out.values[i] = node.raw_index_table[encoded[i] & 15];
				}
				return;
			}

			const auto base = encoded_min ? node.index_min + encoded_min - 2 : -1;
			for (auto i = 0; i < 16; i++)
			{
				out.values[i] = encoded[i] ? encoded[i] + base : 0;
			}
		}

		bool convert_chunk(const tree_view& tree, const unsigned int chunk_mask, const int leaf_size_bit_count, const unsigned int address,
			std::vector<unsigned char>& out)
		{
			bit_reader reader(tree.leaf_table + address, tree.leaf_table_size - address);
			const auto format = reader.read(8);
			const auto leaf_count = std::popcount(chunk_mask & 0xFF);

			int leaf_size[8]{};
			auto blocks_size = 0u;
			for (auto i = 0; i < leaf_count; i++)
			{
				leaf_size[i] = reader.read_int(tree.leaf_size_bit_count);
				blocks_size += leaf_size[i];
			}

			const auto index_info_start = reader.position();
			node_index_info index_info{};
			read_node_index(reader, format & 0x1, format & 0x2, format & 0x4, tree.default_color_index_bit_count, 16, index_info);
			read_node_index(reader, format & 0x8, format & 0x10, format & 0x20, tree.default_light_index_bit_count, 1, index_info);
			const auto index_info_end = reader.position();
			reader.align_byte();

			const auto blocks_start = address + reader.position() / 8;
			if (reader.overflowed() || blocks_start + blocks_size > static_cast<std::size_t>(tree.leaf_table_size))
			{
				return false;
			}

			bit_writer writer;
			writer.write(format, 8);
			for (auto i = 0; i < leaf_count; i++)
			{
				if (leaf_size[i] >= (1 << leaf_size_bit_count))
				{
					return false;
				}

				writer.write(leaf_size[i], leaf_size_bit_count);
			}

			reader.seek(index_info_start);
			for (auto bit = index_info_start; bit < index_info_end; bit++)
			{
				writer.write(reader.read(1), 1);
			}

			writer.align_byte();
			out.insert(out.end(), writer.data().begin(), writer.data().end());
			out.insert(out.end(), tree.leaf_table + blocks_start, tree.leaf_table + blocks_start + blocks_size);
			return true;
		}

		void read_leaf_block(bit_reader& reader, const chunk_header& header, const int block_size, leaf_block& out)
		{
			reader.align_byte();
			const auto format = block_size ? reader.read(8) : 0;
			read_leaf_channel(reader, header.color, format & 0x1, format & 0x2, format & 0x4, out.color);
			read_leaf_channel(reader, header.light, format & 0x8, format & 0x10, format & 0x20, out.light);

			out.trace_mask = 0;
			if (format & 0x40)
			{
				out.trace_mask |= read_voxel_mask(reader);
			}
			if (format & 0x80)
			{
				out.trace_mask |= read_voxel_mask(reader) << 16;
			}
		}

		float range_from_exponent(const int exponent)
		{
			return exponent < 0 ? 1.0f / static_cast<float>(1 << -exponent) : static_cast<float>(1 << exponent);
		}

		int coeff_bits(const palette_config& config, const int exponent)
		{
			if (exponent <= config.range_exp_8bits)
			{
				return 8;
			}
			return exponent <= config.range_exp_12bits ? 12 : 16;
		}

		unsigned int read_u16(const unsigned char* data)
		{
			return data[0] | (data[1] << 8);
		}

		unsigned int decode_coeffs_8bits(const int channel, const unsigned char* data, const float range, palette_sh& out)
		{
			out.coeffs[0][channel] = range * 0.00390625f * data[0];
			const auto scale = range * 2.0f * 0.00390625f;
			for (auto i = 1; i < 9; i++)
			{
				out.coeffs[i][channel] = data[i] * scale - range;
			}
			return 9;
		}

		unsigned int decode_coeffs_12bits(const int channel, const unsigned char* data, const float range, palette_sh& out)
		{
			out.coeffs[0][channel] = range * 0.000015258789f * read_u16(data);
			const auto scale = range * 2.0f * 0.00024414062f;
			const unsigned int values[8] =
			{
				static_cast<unsigned int>((data[9] >> 4) | (data[2] << 4)),
				static_cast<unsigned int>((data[10] & 0xF) | (data[3] << 4)),
				static_cast<unsigned int>((data[10] >> 4) | (data[4] << 4)),
				static_cast<unsigned int>((data[11] & 0xF) | (data[5] << 4)),
				static_cast<unsigned int>((data[11] >> 4) | (data[6] << 4)),
				static_cast<unsigned int>((data[12] & 0xF) | (data[7] << 4)),
				static_cast<unsigned int>((data[12] >> 4) | (data[8] << 4)),
				static_cast<unsigned int>((data[13] & 0xF) | (data[9] << 4)),
			};
			for (auto i = 0; i < 8; i++)
			{
				out.coeffs[i + 1][channel] = values[i] * scale - range;
			}
			return 14;
		}

		unsigned int decode_coeffs_16bits(const int channel, const unsigned char* data, const float range, palette_sh& out)
		{
			out.coeffs[0][channel] = range * 0.000015258789f * read_u16(data);
			const auto scale = range * 2.0f * 0.000015258789f;
			for (auto i = 1; i < 9; i++)
			{
				out.coeffs[i][channel] = read_u16(data + 2 * i) * scale - range;
			}
			return 18;
		}

		void palette_basis(const float dir[3], float basis[9])
		{
			const auto x = dir[0];
			const auto y = dir[1];
			const auto z = dir[2];
			basis[0] = 1.0f;
			basis[1] = x;
			basis[2] = y;
			basis[3] = z;
			basis[4] = z * x;
			basis[5] = z * y;
			basis[6] = y * x;
			basis[7] = 3.0f * z * z - 1.0f;
			basis[8] = x * x - y * y;
		}

		void probe_basis(const float dir[3], double basis[9])
		{
			const auto x = -dir[0];
			const auto y = -dir[1];
			const auto z = dir[2];
			basis[0] = 0.2820948;
			basis[1] = 0.4886025 * y;
			basis[2] = 0.4886025 * z;
			basis[3] = 0.4886025 * x;
			basis[4] = 1.0925484 * x * y;
			basis[5] = 1.0925484 * y * z;
			basis[6] = 0.3153916 * (3.0 * z * z - 1.0);
			basis[7] = 1.0925484 * x * z;
			basis[8] = 0.5462742 * (x * x - y * y);
		}

		class probe_projection
		{
		public:
			probe_projection()
			{
				double normal[9][9]{};
				for (auto d = 0u; d < basis_dir_count; d++)
				{
					probe_basis(basis_dirs[d], this->basis_[d]);
					for (auto r = 0; r < 9; r++)
					{
						for (auto c = 0; c < 9; c++)
						{
							normal[r][c] += this->basis_[d][r] * this->basis_[d][c];
						}
					}
				}
				invert(normal, this->inverse_);
			}

			void project(const float radiance[basis_dir_count][3], float out[probe_sh_count]) const
			{
				for (auto channel = 0; channel < 3; channel++)
				{
					double moments[9]{};
					for (auto d = 0u; d < basis_dir_count; d++)
					{
						for (auto k = 0; k < 9; k++)
						{
							moments[k] += this->basis_[d][k] * radiance[d][channel];
						}
					}

					for (auto k = 0; k < 9; k++)
					{
						auto value = 0.0;
						for (auto j = 0; j < 9; j++)
						{
							value += this->inverse_[k][j] * moments[j];
						}
						out[channel * 9 + k] = static_cast<float>(value);
					}
				}
			}

		private:
			double basis_[basis_dir_count][9]{};
			double inverse_[9][9]{};

			static void invert(double matrix[9][9], double inverse[9][9])
			{
				for (auto r = 0; r < 9; r++)
				{
					for (auto c = 0; c < 9; c++)
					{
						inverse[r][c] = r == c ? 1.0 : 0.0;
					}
				}

				for (auto col = 0; col < 9; col++)
				{
					auto pivot = col;
					for (auto r = col + 1; r < 9; r++)
					{
						if (std::fabs(matrix[r][col]) > std::fabs(matrix[pivot][col]))
						{
							pivot = r;
						}
					}
					std::swap(matrix[col], matrix[pivot]);
					std::swap(inverse[col], inverse[pivot]);

					const auto scale = 1.0 / matrix[col][col];
					for (auto c = 0; c < 9; c++)
					{
						matrix[col][c] *= scale;
						inverse[col][c] *= scale;
					}

					for (auto r = 0; r < 9; r++)
					{
						const auto factor = matrix[r][col];
						if (r == col || factor == 0.0)
						{
							continue;
						}
						for (auto c = 0; c < 9; c++)
						{
							matrix[r][c] -= factor * matrix[col][c];
							inverse[r][c] -= factor * inverse[col][c];
						}
					}
				}
			}
		};
	}

	bool find_voxel(const tree_view& tree, const unsigned short grid_pos[3], voxel& out, bool* stream_overflow)
	{
		out = {};
		if (!tree.node_table || !tree.leaf_table || tree.node_count <= 0)
		{
			return false;
		}

		for (auto axis = 0; axis < 3; axis++)
		{
			if (grid_pos[axis] < tree.coord_min_grid_space[axis] || grid_pos[axis] > tree.coord_max_grid_space[axis])
			{
				return false;
			}
		}

		const auto tx = static_cast<unsigned int>(grid_pos[0] - tree.coord_min_grid_space[0]) >> 2;
		const auto ty = static_cast<unsigned int>(grid_pos[1] - tree.coord_min_grid_space[1]) >> 2;
		const auto tz = static_cast<unsigned int>(grid_pos[2] - tree.coord_min_grid_space[2]);
		const auto voxel_index = voxel_index_table[(grid_pos[0] & 3) | ((grid_pos[1] & 3) << 2)];

		auto bit = 1u << tree.max_depth;
		auto node_index = 0u;
		auto chunk_mask = 0u;
		auto octant = 0;
		auto leaf_address = 0u;
		for (auto depth = 0;; depth++)
		{
			if (node_index >= static_cast<unsigned int>(tree.node_count))
			{
				return false;
			}

			const auto node = tree.node_table[node_index];
			chunk_mask = node >> 24;
			octant = ((tx & bit) ? 1 : 0) | ((ty & bit) ? 2 : 0) | ((tz & bit) ? 4 : 0);
			if (!(chunk_mask & (1u << octant)))
			{
				return false;
			}

			if (depth >= tree.max_depth)
			{
				leaf_address = node & 0xFFFFFF;
				break;
			}

			node_index = (node & 0xFFFFFF) + bit_count_below(chunk_mask, octant);
			bit >>= 1;
		}

		if (leaf_address >= static_cast<unsigned int>(tree.leaf_table_size))
		{
			return false;
		}

		bit_reader reader(tree.leaf_table + leaf_address, tree.leaf_table_size - leaf_address);
		chunk_header header{};
		read_chunk_header(reader, chunk_mask, tree, header);

		const auto leaf_order = bit_count_below(chunk_mask, octant);
		auto skip = 0u;
		for (auto i = 0; i < leaf_order; i++)
		{
			skip += header.leaf_size[i];
		}
		reader.skip_bytes(skip);

		leaf_block block{};
		read_leaf_block(reader, header, header.leaf_size[leaf_order], block);
		if (reader.overflowed())
		{
			if (stream_overflow)
			{
				*stream_overflow = true;
			}
			return false;
		}

		out.color_index = static_cast<unsigned int>(block.color.values[voxel_index]);
		out.light_index = static_cast<unsigned short>(block.light.values[voxel_index]);
		out.trace_low = (block.trace_mask >> voxel_index) & 1;
		out.trace_high = (block.trace_mask >> (voxel_index + 16)) & 1;
		return out.color_index != 0;
	}

	bool convert_leaf_table(const tree_view& tree, const int leaf_size_bit_count, std::vector<unsigned int>& node_table, std::vector<unsigned char>& leaf_table)
	{
		node_table.assign(tree.node_table, tree.node_table + tree.node_count);
		leaf_table.clear();
		if (!tree.node_table || !tree.leaf_table || tree.node_count <= 0)
		{
			return false;
		}

		std::unordered_map<unsigned int, unsigned int> converted_addresses;
		std::vector<std::pair<unsigned int, int>> pending{{0u, 0}};
		while (!pending.empty())
		{
			const auto [node_index, depth] = pending.back();
			pending.pop_back();
			if (node_index >= static_cast<unsigned int>(tree.node_count))
			{
				return false;
			}

			const auto node = tree.node_table[node_index];
			const auto chunk_mask = node >> 24;
			if (depth < tree.max_depth)
			{
				for (auto child = 0; child < std::popcount(chunk_mask); child++)
				{
					pending.emplace_back((node & 0xFFFFFF) + child, depth + 1);
				}

				continue;
			}

			const auto address = node & 0xFFFFFF;
			auto converted = converted_addresses.find(address);
			if (converted == converted_addresses.end())
			{
				const auto new_address = static_cast<unsigned int>(leaf_table.size());
				if (address >= static_cast<unsigned int>(tree.leaf_table_size) || new_address > 0xFFFFFF
					|| !convert_chunk(tree, chunk_mask, leaf_size_bit_count, address, leaf_table))
				{
					return false;
				}

				converted = converted_addresses.emplace(address, new_address).first;
			}

			node_table[node_index] = (node & 0xFF000000) | converted->second;
		}

		return true;
	}

	void decode_palette_entry(const palette_config& config, const unsigned char* stream, palette_sh& out)
	{
		const auto header = read_u16(stream);
		const int exponents[3] =
		{
			static_cast<int>((header >> 10) & 0x1F) - 8,
			static_cast<int>((header >> 5) & 0x1F) - 8,
			static_cast<int>(header & 0x1F) - 8,
		};

		auto offset = 2u;
		for (auto channel = 0; channel < 3; channel++)
		{
			const auto bits = coeff_bits(config, exponents[channel]);
			const auto range = range_from_exponent(exponents[channel]);
			if (bits == 8)
			{
				offset += decode_coeffs_8bits(channel, stream + offset, range, out);
				continue;
			}

			offset += offset & 1;
			offset += bits == 12
				? decode_coeffs_12bits(channel, stream + offset, range, out)
				: decode_coeffs_16bits(channel, stream + offset, range, out);
		}
	}

	void evaluate(const palette_sh& sh, float radiance[basis_dir_count][3])
	{
		for (auto d = 0u; d < basis_dir_count; d++)
		{
			float basis[9];
			palette_basis(basis_dirs[d], basis);
			for (auto channel = 0; channel < 3; channel++)
			{
				auto value = 0.0f;
				for (auto k = 0; k < 9; k++)
				{
					value += sh.coeffs[k][channel] * basis[k];
				}
				radiance[d][channel] = std::max(value, 0.0f);
			}
		}
	}

	void project_probe_sh(const float radiance[basis_dir_count][3], float out[probe_sh_count])
	{
		static const probe_projection projection;
		projection.project(radiance, out);
	}

	void probe_sh_to_palette(const float probe[probe_sh_count], palette_sh& out)
	{
		for (auto channel = 0; channel < 3; channel++)
		{
			const auto coeffs = probe + channel * 9;
			out.coeffs[0][channel] = 0.2820948f * coeffs[0];
			out.coeffs[1][channel] = -0.4886025f * coeffs[3];
			out.coeffs[2][channel] = -0.4886025f * coeffs[1];
			out.coeffs[3][channel] = 0.4886025f * coeffs[2];
			out.coeffs[4][channel] = -1.0925484f * coeffs[7];
			out.coeffs[5][channel] = -1.0925484f * coeffs[5];
			out.coeffs[6][channel] = 1.0925484f * coeffs[4];
			out.coeffs[7][channel] = 0.3153916f * coeffs[6];
			out.coeffs[8][channel] = 0.5462742f * coeffs[8];
		}
	}

	namespace
	{
		unsigned int quantize(const float value, const float range, const int steps)
		{
			return static_cast<unsigned int>(std::clamp(std::lround(value * static_cast<float>(steps) / range), 0l, steps - 1l));
		}

		int select_exponent(const palette_config& config, const palette_sh& sh, const int channel)
		{
			auto coeff_min = 0.0f;
			auto coeff_max = 0.0f;
			for (auto i = 1; i < 9; i++)
			{
				coeff_min = std::min(coeff_min, sh.coeffs[i][channel]);
				coeff_max = std::max(coeff_max, sh.coeffs[i][channel]);
			}

			for (auto exponent = -8; exponent < 23; exponent++)
			{
				const auto range = range_from_exponent(exponent);
				const auto steps = coeff_bits(config, exponent) == 8 ? 256.0f : 65536.0f;
				if (sh.coeffs[0][channel] <= range * (steps - 1.0f) / steps && coeff_max <= range * (steps - 2.0f) / steps && coeff_min >= -range)
				{
					return exponent;
				}
			}

			return 23;
		}

		void write_u16(std::vector<unsigned char>& stream, const unsigned int value)
		{
			stream.emplace_back(static_cast<unsigned char>(value & 0xFF));
			stream.emplace_back(static_cast<unsigned char>((value >> 8) & 0xFF));
		}
	}

	int encode_palette_entry(const palette_config& config, const palette_sh& sh, std::vector<unsigned char>& stream)
	{
		if (config.range_exp_12bits != config.range_exp_8bits)
		{
			throw std::runtime_error("light grid palette: the 12 bit encoding is not supported"); // the game reader shares one byte between two coefficients
		}

		if (stream.size() & 1)
		{
			stream.emplace_back(static_cast<unsigned char>(0));
		}

		const auto start = stream.size();
		const int exponents[3] = {select_exponent(config, sh, 0), select_exponent(config, sh, 1), select_exponent(config, sh, 2)};
		write_u16(stream, static_cast<unsigned int>(((exponents[0] + 8) << 10) | ((exponents[1] + 8) << 5) | (exponents[2] + 8)));

		for (auto channel = 0; channel < 3; channel++)
		{
			const auto range = range_from_exponent(exponents[channel]);
			const auto base = std::max(sh.coeffs[0][channel], 0.0f);
			if (coeff_bits(config, exponents[channel]) == 8)
			{
				stream.emplace_back(static_cast<unsigned char>(quantize(base, range, 256)));
				for (auto i = 1; i < 9; i++)
				{
					stream.emplace_back(static_cast<unsigned char>(quantize(sh.coeffs[i][channel] + range, 2.0f * range, 256)));
				}

				continue;
			}

			if ((stream.size() - start) & 1)
			{
				stream.emplace_back(static_cast<unsigned char>(0));
			}

			write_u16(stream, quantize(base, range, 65536));
			for (auto i = 1; i < 9; i++)
			{
				write_u16(stream, quantize(sh.coeffs[i][channel] + range, 2.0f * range, 65536));
			}
		}

		return static_cast<int>(start);
	}

	namespace
	{
		struct leaf_voxels
		{
			unsigned int color[16];
			unsigned int light[16];
			unsigned int trace_low_mask;
			unsigned int trace_high_mask;
		};

		struct channel_plan
		{
			int encoded_min;
			int encoded_bits;
			int encoded[16];
			int encoded_default;
			bool needs_commands;
		};

		channel_plan plan_channel(const unsigned int values[16], const int value_bits)
		{
			channel_plan plan{};
			plan.encoded_bits = 1;

			auto min_value = std::numeric_limits<unsigned int>::max();
			for (auto i = 0; i < 16; i++)
			{
				min_value = values[i] ? std::min(min_value, values[i]) : min_value;
			}

			if (min_value == std::numeric_limits<unsigned int>::max())
			{
				return plan;
			}

			plan.encoded_min = static_cast<int>(std::min<std::uint64_t>(min_value + 1ull, (1ull << value_bits) - 1));
			auto max_encoded = 0;
			for (auto i = 0; i < 16; i++)
			{
				plan.encoded[i] = values[i] ? static_cast<int>(values[i]) - (plan.encoded_min - 2) : 0;
				max_encoded = std::max(max_encoded, plan.encoded[i]);
			}

			plan.encoded_bits = std::max(bit_length(max_encoded), 1);
			if (plan.encoded_bits > (1 << bit_count_field_width(value_bits)))
			{
				throw std::runtime_error("light grid tree: the index range of one leaf is too large");
			}

			auto best_count = 0;
			for (auto i = 0; i < 16; i++)
			{
				const auto count = static_cast<int>(std::count(plan.encoded, plan.encoded + 16, plan.encoded[i]));
				if (count > best_count)
				{
					best_count = count;
					plan.encoded_default = plan.encoded[i];
				}
			}

			plan.needs_commands = best_count != 16;
			return plan;
		}

		void write_channel(bit_writer& writer, const channel_plan& plan, const int value_bits)
		{
			writer.write(static_cast<unsigned int>(plan.encoded_min), value_bits);
			writer.write(static_cast<unsigned int>(plan.encoded_bits - 1), bit_count_field_width(value_bits));
			writer.write(static_cast<unsigned int>(plan.encoded_default), plan.encoded_bits);
			if (!plan.needs_commands)
			{
				return;
			}

			writer.write(0, 2);
			for (const auto encoded : plan.encoded)
			{
				writer.write(static_cast<unsigned int>(encoded), plan.encoded_bits);
			}
		}

		void write_voxel_mask(bit_writer& writer, const unsigned int mask)
		{
			if (mask == 0xFFFF)
			{
				writer.write(1, 2);
				return;
			}

			writer.write(0, 2);
			writer.write(mask, 16);
		}

		std::vector<unsigned char> encode_leaf_block(const leaf_voxels& leaf, const int color_bits)
		{
			const auto color = plan_channel(leaf.color, color_bits);
			const auto light = plan_channel(leaf.light, 16);

			bit_writer writer;
			writer.write(0x1 | 0x2 | 0x8 | 0x10 | (color.needs_commands ? 0x4 : 0) | (light.needs_commands ? 0x20 : 0)
				| (leaf.trace_low_mask ? 0x40 : 0) | (leaf.trace_high_mask ? 0x80 : 0), 8);
			write_channel(writer, color, color_bits);
			write_channel(writer, light, 16);
			if (leaf.trace_low_mask)
			{
				write_voxel_mask(writer, leaf.trace_low_mask);
			}

			if (leaf.trace_high_mask)
			{
				write_voxel_mask(writer, leaf.trace_high_mask);
			}

			return writer.data();
		}

		std::uint64_t pack_tree_coord(const unsigned int x, const unsigned int y, const unsigned int z)
		{
			return (static_cast<std::uint64_t>(z) << 42) | (static_cast<std::uint64_t>(y) << 21) | x;
		}

		std::uint64_t child_tree_coord(const std::uint64_t coord, const int octant)
		{
			const auto x = static_cast<unsigned int>(coord & 0x1FFFFF);
			const auto y = static_cast<unsigned int>((coord >> 21) & 0x1FFFFF);
			const auto z = static_cast<unsigned int>((coord >> 42) & 0x1FFFFF);
			return pack_tree_coord((x << 1) | (octant & 1), (y << 1) | ((octant >> 1) & 1), (z << 1) | ((octant >> 2) & 1));
		}

		void write_chunk(const std::map<std::uint64_t, leaf_voxels>& leaves, const std::uint64_t coord, const unsigned int mask, const int color_bits,
			const int leaf_size_bit_count, std::vector<unsigned char>& leaf_table)
		{
			bit_writer header;
			header.write(0x1 | 0x8, 8);

			std::vector<unsigned char> blocks;
			for (auto octant = 0; octant < 8; octant++)
			{
				if (!(mask & (1u << octant)))
				{
					continue;
				}

				const auto block = encode_leaf_block(leaves.at(child_tree_coord(coord, octant)), color_bits);
				if (block.size() >= (1ull << leaf_size_bit_count))
				{
					throw std::runtime_error("light grid tree: a leaf block is too large");
				}

				header.write(static_cast<unsigned int>(block.size()), leaf_size_bit_count);
				blocks.insert(blocks.end(), block.begin(), block.end());
			}

			header.write(0, color_bits);
			header.write(static_cast<unsigned int>(color_bits - 1), bit_count_field_width(color_bits));
			header.write(0, 16);
			header.write(15, 4);

			leaf_table.insert(leaf_table.end(), header.data().begin(), header.data().end());
			leaf_table.insert(leaf_table.end(), blocks.begin(), blocks.end());
		}
	}

	tree_data build_tree(const std::vector<grid_sample>& samples, const int leaf_size_bit_count)
	{
		tree_data tree{};
		tree.default_color_index_bit_count = 16;
		tree.default_light_index_bit_count = 16;
		if (samples.empty())
		{
			std::fill_n(tree.coord_min_grid_space, 3, 1);
			tree.node_table.emplace_back(0u);
			return tree;
		}

		auto max_color = 0u;
		std::fill_n(tree.coord_min_grid_space, 3, std::numeric_limits<int>::max());
		for (const auto& sample : samples)
		{
			max_color = std::max(max_color, sample.value.color_index);
			for (auto axis = 0; axis < 3; axis++)
			{
				tree.coord_min_grid_space[axis] = std::min<int>(tree.coord_min_grid_space[axis], sample.grid_pos[axis]);
				tree.coord_max_grid_space[axis] = std::max<int>(tree.coord_max_grid_space[axis], sample.grid_pos[axis]);
			}
		}

		for (auto axis = 0; axis < 3; axis++)
		{
			tree.coord_half_size_grid_space[axis] = (tree.coord_max_grid_space[axis] - tree.coord_min_grid_space[axis]) / 2 + 1;
		}

		const auto color_bits = std::max(bit_length(static_cast<int>(max_color)), 16);
		if (color_bits > 24)
		{
			throw std::runtime_error("light grid tree: the color index is too large");
		}

		tree.default_color_index_bit_count = color_bits;

		std::map<std::uint64_t, leaf_voxels> leaves;
		auto max_coord = 0u;
		for (const auto& sample : samples)
		{
			const auto x = static_cast<unsigned int>(sample.grid_pos[0] - tree.coord_min_grid_space[0]) >> 2;
			const auto y = static_cast<unsigned int>(sample.grid_pos[1] - tree.coord_min_grid_space[1]) >> 2;
			const auto z = static_cast<unsigned int>(sample.grid_pos[2] - tree.coord_min_grid_space[2]);
			max_coord = std::max({max_coord, x, y, z});

			auto& leaf = leaves[pack_tree_coord(x, y, z)];
			const auto index = voxel_index_table[(sample.grid_pos[0] & 3) | ((sample.grid_pos[1] & 3) << 2)];
			leaf.color[index] = sample.value.color_index;
			leaf.light[index] = sample.value.light_index;
			leaf.trace_low_mask = (leaf.trace_low_mask & ~(1u << index)) | (sample.value.trace_low ? 1u << index : 0u);
			leaf.trace_high_mask = (leaf.trace_high_mask & ~(1u << index)) | (sample.value.trace_high ? 1u << index : 0u);
		}

		tree.leaf_count = static_cast<int>(leaves.size());
		const auto max_depth = std::max(bit_length(static_cast<int>(max_coord)) - 1, 0);
		tree.max_depth = static_cast<unsigned char>(max_depth);

		std::vector<std::map<std::uint64_t, unsigned char>> level_masks(max_depth + 1);
		for (const auto& [coord, leaf] : leaves)
		{
			auto x = static_cast<unsigned int>(coord & 0x1FFFFF);
			auto y = static_cast<unsigned int>((coord >> 21) & 0x1FFFFF);
			auto z = static_cast<unsigned int>((coord >> 42) & 0x1FFFFF);
			for (auto level = max_depth; level >= 0; level--)
			{
				const auto octant = (x & 1) | ((y & 1) << 1) | ((z & 1) << 2);
				x >>= 1;
				y >>= 1;
				z >>= 1;
				level_masks[level][pack_tree_coord(x, y, z)] |= static_cast<unsigned char>(1u << octant);
			}
		}

		std::vector<std::pair<int, std::uint64_t>> nodes{{0, 0ull}};
		tree.node_table.emplace_back(0u);
		for (auto n = 0ull; n < nodes.size(); n++)
		{
			const auto [level, coord] = nodes[n];
			const unsigned int mask = level_masks[level][coord];

			auto address = static_cast<unsigned int>(tree.node_table.size());
			if (level < max_depth)
			{
				for (auto octant = 0; octant < 8; octant++)
				{
					if (mask & (1u << octant))
					{
						nodes.emplace_back(level + 1, child_tree_coord(coord, octant));
						tree.node_table.emplace_back(0u);
					}
				}
			}
			else
			{
				address = static_cast<unsigned int>(tree.leaf_table.size());
				write_chunk(leaves, coord, mask, color_bits, leaf_size_bit_count, tree.leaf_table);
			}

			if (address > 0xFFFFFF)
			{
				throw std::runtime_error("light grid tree: the node or leaf table is too large");
			}

			tree.node_table[n] = (mask << 24) | address;
		}

		return tree;
	}
}
