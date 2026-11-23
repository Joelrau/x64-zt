#include <std_include.hpp>
#include "tree.hpp"

#include <cassert>
#include <cstring>
#include <map>
#include <set>
#include <stdexcept>

namespace lightgrid_tree
{
	namespace
	{
		constexpr unsigned char voxel_index_lut[16] =
		{
			0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15,
		};

		struct mask_table_entry
		{
			unsigned short mask;
			unsigned char bit_count;
			unsigned char bit_index[8];
		};
		constexpr mask_table_entry voxel_mask_table[16] =
		{
			{ 0x0033, 4, { 0, 1, 4, 5 } },
			{ 0x00CC, 4, { 2, 3, 6, 7 } },
			{ 0x3300, 4, { 8, 9, 12, 13 } },
			{ 0xCC00, 4, { 10, 11, 14, 15 } },
			{ 0x0505, 4, { 0, 2, 8, 10 } },
			{ 0x0A0A, 4, { 1, 3, 9, 11 } },
			{ 0x5050, 4, { 4, 6, 12, 14 } },
			{ 0xA0A0, 4, { 5, 7, 13, 15 } },
			{ 0x000F, 4, { 0, 1, 2, 3 } },
			{ 0x005A, 4, { 1, 4, 3, 6 } },
			{ 0x00F0, 4, { 4, 5, 6, 7 } },
			{ 0x030C, 4, { 2, 3, 8, 9 } },
			{ 0x30C0, 4, { 6, 7, 12, 13 } },
			{ 0x0F00, 4, { 8, 9, 10, 11 } },
			{ 0x5A00, 4, { 9, 12, 11, 14 } },
			{ 0xF000, 4, { 12, 13, 14, 15 } },
		};

		int bit_length(int v)
		{
			int bits = 0;
			while (v > 0)
			{
				v >>= 1;
				bits++;
			}
			return bits;
		}

		int bit_count_field_width(int count)
		{
			if (count <= 0)
			{
				return 0;
			}
			int v = count - 1;
			if (v <= 1)
			{
				v = 1;
			}
			return bit_length(v);
		}

		int popcount16(unsigned int v)
		{
			int n = 0;
			for (; v; v &= v - 1)
			{
				n++;
			}
			return n;
		}

		class bit_writer
		{
		public:
			explicit bit_writer(std::vector<unsigned char>& out) : out_(out), start_bits_(out.size() * 8), bits_(out.size() * 8)
			{
			}

			void write(unsigned int value, int bits)
			{
				for (int i = 0; i < bits; i++)
				{
					const size_t byte = bits_ >> 3;
					if (byte >= out_.size())
					{
						out_.push_back(0);
					}
					if ((value >> i) & 1)
					{
						out_[byte] |= static_cast<unsigned char>(1 << (bits_ & 7));
					}
					bits_++;
				}
			}

			void align_byte()
			{
				bits_ = (bits_ + 7) & ~7ull;
				while (out_.size() * 8 < bits_)
				{
					out_.push_back(0);
				}
			}

			size_t bytes_written() const
			{
				return (bits_ + 7) / 8 - start_bits_ / 8;
			}

		private:
			std::vector<unsigned char>& out_;
			size_t start_bits_;
			size_t bits_;
		};

		class bit_reader
		{
		public:
			bit_reader(const unsigned char* data, size_t byte_count)
				: data_(data), bit_count_(byte_count * 8), bits_(0)
			{
			}

			unsigned int read(int bits)
			{
				unsigned int value = 0;
				for (int i = 0; i < bits; i++)
				{
					if (bits_ >= bit_count_)
					{
						overflow_ = true;
						return value;
					}
					value |= static_cast<unsigned int>((data_[bits_ >> 3] >> (bits_ & 7)) & 1) << i;
					bits_++;
				}
				return value;
			}

			void align_byte()
			{
				bits_ = (bits_ + 7) & ~7ull;
			}

			void skip_bytes(size_t bytes)
			{
				bits_ += bytes * 8;
			}

			bool overflowed() const
			{
				return overflow_;
			}

		private:
			const unsigned char* data_;
			size_t bit_count_;
			size_t bits_;
			bool overflow_ = false;
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
			int value_default;
		};

		void read_index_header(bit_reader& br, bool present, bool default_present, bool table_present,
			int default_bit_count, int fallback_bit_count, node_index_info& info)
		{
			if (present)
			{
				info.index_min = static_cast<int>(br.read(default_bit_count));
				info.index_bit_count = static_cast<int>(br.read(bit_count_field_width(default_bit_count))) + 1;
			}
			else
			{
				info.index_min = 0;
				info.index_bit_count = fallback_bit_count;
			}

			info.encoded_index_default = default_present ? static_cast<int>(br.read(info.index_bit_count)) : 0;

			info.index_table_size = 0;
			info.index_table_size_bit_count = 0;
			std::memset(info.raw_index_table, 0, sizeof(info.raw_index_table));
			if (table_present)
			{
				info.index_table_size = static_cast<int>(br.read(4)) + 1;
				info.index_table_size_bit_count = bit_count_field_width(info.index_table_size);
				info.raw_index_table[0] = 0;
				for (int i = 1; i < info.index_table_size; i++)
				{
					const int enc = static_cast<int>(br.read(info.index_bit_count));
					info.raw_index_table[i] = enc ? enc + info.index_min - 1 : 0;
				}
			}

			if (info.index_table_size)
			{
				info.raw_index_default = info.raw_index_table[info.encoded_index_default & 15];
			}
			else
			{
				info.raw_index_default = info.encoded_index_default
					? info.encoded_index_default + info.index_min - 1
					: 0;
			}
		}

		void read_chunk_header(bit_reader& br, unsigned int chunk_mask, int leaf_size_bit_count,
			int default_color_bits, int default_light_bits, chunk_header& hdr)
		{
			br.align_byte();
			hdr.format = br.read(8);

			const int leaf_count = popcount16(chunk_mask & 0xFF);
			std::memset(hdr.leaf_size, 0, sizeof(hdr.leaf_size));
			for (int i = 0; i < leaf_count; i++)
			{
				hdr.leaf_size[i] = static_cast<int>(br.read(leaf_size_bit_count));
			}

			read_index_header(br, (hdr.format & 1) != 0, (hdr.format & 2) != 0, (hdr.format & 4) != 0,
				default_color_bits, 16, hdr.color);
			read_index_header(br, (hdr.format & 8) != 0, (hdr.format & 0x10) != 0, (hdr.format & 0x20) != 0,
				default_light_bits, 1, hdr.light);

			br.align_byte();
		}

		unsigned int read_voxel_mask(bit_reader& br)
		{
			const unsigned int sel = br.read(2);
			if (sel == 0)
			{
				return br.read(16);
			}
			if (sel == 1)
			{
				return 0xFFFF;
			}
			const auto& entry = voxel_mask_table[br.read(4) & 15];
			const unsigned int bits = br.read(entry.bit_count);
			unsigned int mask = 0;
			for (int i = 0; i < entry.bit_count; i++)
			{
				if ((bits >> i) & 1)
				{
					mask |= 1u << entry.bit_index[i];
				}
			}
			return (sel == 3) ? (~mask & 0xFFFF) : mask;
		}

		unsigned int read_index_mask(bit_reader& br, bool* /*unsupported*/)
		{
			const unsigned int sel = br.read(1);
			if (sel == 0)
			{
				return br.read(16);
			}
			const auto& entry = voxel_mask_table[br.read(4) & 15];
			const unsigned int invert = br.read(1);
			const unsigned int bits = br.read(entry.bit_count);
			unsigned int mask = 0;
			for (int i = 0; i < entry.bit_count; i++)
			{
				if ((bits >> i) & 1)
				{
					mask |= 1u << entry.bit_index[i];
				}
			}
			return invert ? (~mask & 0xFFFF) : mask;
		}

		void read_leaf_channel(bit_reader& br, const node_index_info& node, bool explicit_range,
			bool explicit_default, bool has_commands, leaf_index_channel& out, bool* unsupported)
		{
			int encoded_min;
			int encoded_bits;
			bool table_mode = false;

			if (explicit_range)
			{
				encoded_min = static_cast<int>(br.read(node.index_bit_count));
				encoded_bits = static_cast<int>(br.read(bit_count_field_width(node.index_bit_count))) + 1;
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

			int encoded_default;
			if (explicit_default)
			{
				encoded_default = static_cast<int>(br.read(encoded_bits));
			}
			else if (table_mode)
			{
				encoded_default = node.encoded_index_default;
			}
			else
			{
				const int base = encoded_min ? node.index_min + encoded_min - 1 : 0;
				encoded_default = node.raw_index_default ? node.raw_index_default - base + 1 : 0;
			}

			int encoded[16];
			for (int i = 0; i < 16; i++)
			{
				encoded[i] = encoded_default;
			}

			if (has_commands)
			{
				bool run = true;
				while (run)
				{
					const unsigned int type = br.read(2);
					run = false;
					if (type == 0)
					{
						for (int i = 0; i < 16; i++)
						{
							encoded[i] = static_cast<int>(br.read(encoded_bits));
						}
					}
					else if (type == 1)
					{
						const int count = static_cast<int>(br.read(1)) + 2;
						unsigned int mask = 0;
						for (int i = 0; i < count; i++)
						{
							mask |= 1u << (br.read(4) & 15);
						}
						const int value = static_cast<int>(br.read(encoded_bits));
						for (int i = 0; i < 16; i++)
						{
							if ((mask >> i) & 1)
							{
								encoded[i] = value;
							}
						}
						run = br.read(1) != 0;
					}
					else if (type == 2)
					{
						const unsigned int mask = read_index_mask(br, unsupported);
						for (int i = 0; i < 16; i++)
						{
							if ((mask >> i) & 1)
							{
								encoded[i] = static_cast<int>(br.read(encoded_bits));
							}
						}
					}
					else
					{
						const unsigned int mask = read_index_mask(br, unsupported);
						const int value = static_cast<int>(br.read(encoded_bits));
						for (int i = 0; i < 16; i++)
						{
							if ((mask >> i) & 1)
							{
								encoded[i] = value;
							}
						}
						run = br.read(1) != 0;
					}
					if (br.overflowed())
					{
						break;
					}
				}
			}

			const bool use_table = node.index_table_size_bit_count
				&& node.index_table_size_bit_count <= encoded_bits;
			if (use_table)
			{
				out.value_default = node.raw_index_table[encoded_default & 15];
				for (int i = 0; i < 16; i++)
				{
					out.values[i] = node.raw_index_table[encoded[i] & 15];
				}
			}
			else
			{
				const int base = (encoded_min == 0) ? -1 : node.index_min + encoded_min - 2;
				out.value_default = encoded_default ? encoded_default + base : 0;
				for (int i = 0; i < 16; i++)
				{
					out.values[i] = encoded[i] ? encoded[i] + base : 0;
				}
			}
		}

		struct decoded_block
		{
			leaf_index_channel color;
			leaf_index_channel light;
			unsigned int voxels;
		};

		void read_leaf_block(bit_reader& br, const chunk_header& hdr, int block_size,
			decoded_block& out, bool* unsupported)
		{
			br.align_byte();
			const unsigned int format = block_size ? br.read(8) : 0;

			read_leaf_channel(br, hdr.color, (format & 1) != 0, (format & 2) != 0,
				(format & 4) != 0, out.color, unsupported);
			read_leaf_channel(br, hdr.light, (format & 8) != 0, (format & 0x10) != 0,
				(format & 0x20) != 0, out.light, unsupported);

			out.voxels = 0;
			if (format & 0x40)
			{
				out.voxels |= read_voxel_mask(br);
			}
			if (format & 0x80)
			{
				out.voxels |= read_voxel_mask(br) << 16;
			}
			br.align_byte();
		}

		struct leaf_block_data
		{
			unsigned int color[16];
			unsigned short light[16];
			unsigned int trace_lo_mask;
			unsigned int trace_hi_mask;

			leaf_block_data()
			{
				std::memset(this, 0, sizeof(*this));
			}
		};

		struct channel_plan
		{
			int encoded_min;     
			int encoded_bits;    
			int encoded[16];
			int encoded_default;
			bool needs_command;
		};

		void plan_channel(const unsigned int* values, int value_bits, channel_plan& plan)
		{
			unsigned int min_value = 0xFFFFFFFF;
			unsigned int max_value = 0;
			bool any = false;
			for (int i = 0; i < 16; i++)
			{
				if (values[i])
				{
					any = true;
					if (values[i] < min_value) min_value = values[i];
					if (values[i] > max_value) max_value = values[i];
				}
			}

			if (!any)
			{
				plan.encoded_min = 0;
				plan.encoded_bits = 1;
				plan.encoded_default = 0;
				std::memset(plan.encoded, 0, sizeof(plan.encoded));
				plan.needs_command = false;
				return;
			}

			const int field_max = static_cast<int>((1u << value_bits) - 1);
			plan.encoded_min = static_cast<int>(min_value) + 1;
			if (plan.encoded_min > field_max)
			{
				plan.encoded_min = field_max;
			}
			const int base = plan.encoded_min - 2;
			int max_enc = 0;
			for (int i = 0; i < 16; i++)
			{
				plan.encoded[i] = values[i] ? static_cast<int>(values[i]) - base : 0;
				if (plan.encoded[i] > max_enc)
				{
					max_enc = plan.encoded[i];
				}
			}
			plan.encoded_bits = bit_length(max_enc);
			if (plan.encoded_bits < 1) plan.encoded_bits = 1;
			if (plan.encoded_bits > (1 << bit_count_field_width(value_bits)))
			{
				throw std::runtime_error("lightgrid_tree: index range too large for one leaf");
			}

			int best_count = -1;
			plan.encoded_default = 0;
			for (int i = 0; i < 16; i++)
			{
				int count = 0;
				for (int j = 0; j < 16; j++)
				{
					if (plan.encoded[j] == plan.encoded[i])
					{
						count++;
					}
				}
				if (count > best_count)
				{
					best_count = count;
					plan.encoded_default = plan.encoded[i];
				}
			}

			plan.needs_command = false;
			for (int i = 0; i < 16; i++)
			{
				if (plan.encoded[i] != plan.encoded_default)
				{
					plan.needs_command = true;
					break;
				}
			}
		}

		void write_channel(bit_writer& bw, const channel_plan& plan, int value_bits)
		{
			bw.write(static_cast<unsigned int>(plan.encoded_min), value_bits);
			bw.write(static_cast<unsigned int>(plan.encoded_bits - 1), bit_count_field_width(value_bits));
			bw.write(static_cast<unsigned int>(plan.encoded_default), plan.encoded_bits);
			if (plan.needs_command)
			{
				bw.write(0, 2);
				for (int i = 0; i < 16; i++)
				{
					bw.write(static_cast<unsigned int>(plan.encoded[i]), plan.encoded_bits);
				}
			}
		}

		void write_voxel_mask(bit_writer& bw, unsigned int mask)
		{
			if (mask == 0xFFFF)
			{
				bw.write(1, 2);
			}
			else
			{
				bw.write(0, 2);
				bw.write(mask, 16);
			}
		}

		size_t encode_leaf_block(std::vector<unsigned char>& out, const leaf_block_data& leaf, int color_bits)
		{
			channel_plan color_plan{};
			plan_channel(leaf.color, color_bits, color_plan);

			unsigned int light_values[16];
			for (int i = 0; i < 16; i++)
			{
				light_values[i] = leaf.light[i];
			}
			channel_plan light_plan{};
			plan_channel(light_values, 16, light_plan);

			unsigned int format = 1 | 2 | 8 | 0x10;
			if (color_plan.needs_command) format |= 4;
			if (light_plan.needs_command) format |= 0x20;
			if (leaf.trace_lo_mask) format |= 0x40;
			if (leaf.trace_hi_mask) format |= 0x80;

			bit_writer bw(out);
			bw.write(format, 8);
			write_channel(bw, color_plan, color_bits);
			write_channel(bw, light_plan, 16);
			if (leaf.trace_lo_mask)
			{
				write_voxel_mask(bw, leaf.trace_lo_mask);
			}
			if (leaf.trace_hi_mask)
			{
				write_voxel_mask(bw, leaf.trace_hi_mask);
			}
			bw.align_byte();
			return bw.bytes_written();
		}

		unsigned long long pack_coord(unsigned int x, unsigned int y, unsigned int z)
		{
			return (static_cast<unsigned long long>(z) << 42)
				| (static_cast<unsigned long long>(y) << 21)
				| static_cast<unsigned long long>(x);
		}
	}

	tree_data build_tree(const grid_sample* samples, size_t sample_count, leaf_size_bits size_bits)
	{
		tree_data tree{};
		tree.default_color_index_bit_count = 16;
		tree.default_light_index_bit_count = 16;

		if (!sample_count)
		{
			tree.coord_min_grid_space[0] = tree.coord_min_grid_space[1] = tree.coord_min_grid_space[2] = 1;
			tree.node_table.push_back(0);
			tree.node_count = 1;
			return tree;
		}

		for (int axis = 0; axis < 3; axis++)
		{
			tree.coord_min_grid_space[axis] = samples[0].pos[axis];
			tree.coord_max_grid_space[axis] = samples[0].pos[axis];
		}
		for (size_t i = 1; i < sample_count; i++)
		{
			for (int axis = 0; axis < 3; axis++)
			{
				const int v = samples[i].pos[axis];
				if (v < tree.coord_min_grid_space[axis]) tree.coord_min_grid_space[axis] = v;
				if (v > tree.coord_max_grid_space[axis]) tree.coord_max_grid_space[axis] = v;
			}
		}
		for (int axis = 0; axis < 3; axis++)
		{
			tree.coord_half_size_grid_space[axis] =
				(tree.coord_max_grid_space[axis] - tree.coord_min_grid_space[axis]) / 2 + 1;
		}

		unsigned int max_color = 0;
		for (size_t i = 0; i < sample_count; i++)
		{
			if (samples[i].color_index > max_color)
			{
				max_color = samples[i].color_index;
			}
		}
		int color_bits = bit_length(static_cast<int>(max_color));
		if (color_bits < 16) color_bits = 16;
		if (color_bits > 24)
		{
			throw std::runtime_error("lightgrid_tree: color index too large");
		}
		tree.default_color_index_bit_count = color_bits;

		std::map<unsigned long long, leaf_block_data> leaves;
		unsigned int max_coord = 0;
		for (size_t i = 0; i < sample_count; i++)
		{
			const auto& s = samples[i];
			const unsigned int tx = static_cast<unsigned int>(s.pos[0] - tree.coord_min_grid_space[0]) >> 2;
			const unsigned int ty = static_cast<unsigned int>(s.pos[1] - tree.coord_min_grid_space[1]) >> 2;
			const unsigned int tz = static_cast<unsigned int>(s.pos[2] - tree.coord_min_grid_space[2]);
			if (tx > max_coord) max_coord = tx;
			if (ty > max_coord) max_coord = ty;
			if (tz > max_coord) max_coord = tz;

			auto& leaf = leaves[pack_coord(tx, ty, tz)];
			const int voxel = voxel_index_lut[(s.pos[0] & 3) | ((s.pos[1] & 3) << 2)];
			leaf.color[voxel] = s.color_index;
			leaf.light[voxel] = s.light_index;
			leaf.trace_lo_mask = (leaf.trace_lo_mask & ~(1u << voxel)) | (s.trace_lo ? (1u << voxel) : 0);
			leaf.trace_hi_mask = (leaf.trace_hi_mask & ~(1u << voxel)) | (s.trace_hi ? (1u << voxel) : 0);
		}
		tree.leaf_count = static_cast<int>(leaves.size());

		int max_depth = bit_length(static_cast<int>(max_coord)) - 1;
		if (max_depth < 0)
		{
			max_depth = 0;
		}
		tree.max_depth = static_cast<unsigned char>(max_depth);

		std::vector<std::map<unsigned long long, unsigned char>> level_masks(max_depth + 1);
		for (const auto& [key, leaf] : leaves)
		{
			unsigned int cx = static_cast<unsigned int>(key & 0x1FFFFF);
			unsigned int cy = static_cast<unsigned int>((key >> 21) & 0x1FFFFF);
			unsigned int cz = static_cast<unsigned int>((key >> 42) & 0x1FFFFF);
			for (int level = max_depth; level >= 0; level--)
			{
				const int octant = (cx & 1) | ((cy & 1) << 1) | ((cz & 1) << 2);
				cx >>= 1;
				cy >>= 1;
				cz >>= 1;
				level_masks[level][pack_coord(cx, cy, cz)] |= static_cast<unsigned char>(1 << octant);
			}
		}

		struct pending_node
		{
			int level;
			unsigned long long coord;
		};
		std::vector<pending_node> order;
		order.push_back({ 0, 0 });
		tree.node_table.clear();
		tree.node_table.push_back(0);

		const int max_leaf_size = (size_bits == leaf_size_bits::h1) ? 127 : 63;

		for (size_t n = 0; n < order.size(); n++)
		{
			const pending_node node = order[n];
			const unsigned char mask = level_masks[node.level][node.coord];
			const unsigned int cx = static_cast<unsigned int>(node.coord & 0x1FFFFF);
			const unsigned int cy = static_cast<unsigned int>((node.coord >> 21) & 0x1FFFFF);
			const unsigned int cz = static_cast<unsigned int>((node.coord >> 42) & 0x1FFFFF);

			unsigned int low24;
			if (node.level < max_depth)
			{
				low24 = static_cast<unsigned int>(tree.node_table.size());
				for (int octant = 0; octant < 8; octant++)
				{
					if (!(mask & (1 << octant)))
					{
						continue;
					}
					order.push_back({ node.level + 1,
						pack_coord((cx << 1) | (octant & 1), (cy << 1) | ((octant >> 1) & 1), (cz << 1) | ((octant >> 2) & 1)) });
					tree.node_table.push_back(0);
				}
			}
			else
			{
				low24 = static_cast<unsigned int>(tree.leaf_table.size());

				std::vector<unsigned char> blocks;
				int sizes[8] = {};
				int leaf_index = 0;
				for (int octant = 0; octant < 8; octant++)
				{
					if (!(mask & (1 << octant)))
					{
						continue;
					}
					const auto it = leaves.find(pack_coord(
						(cx << 1) | (octant & 1), (cy << 1) | ((octant >> 1) & 1), (cz << 1) | ((octant >> 2) & 1)));
					assert(it != leaves.end());
					const size_t size = encode_leaf_block(blocks, it->second, color_bits);
					if (size > static_cast<size_t>(max_leaf_size))
					{
						throw std::runtime_error("lightgrid_tree: leaf block too large");
					}
					sizes[leaf_index++] = static_cast<int>(size);
				}

				bit_writer bw(tree.leaf_table);
				bw.write(0x09, 8);
				for (int i = 0; i < leaf_index; i++)
				{
					bw.write(static_cast<unsigned int>(sizes[i]), static_cast<int>(size_bits));
				}
				bw.write(0, color_bits);                                   
				bw.write(static_cast<unsigned int>(color_bits - 1),
					bit_count_field_width(color_bits));                    
				bw.write(0, 16); 
				bw.write(15, 4); 
				bw.align_byte();

				tree.leaf_table.insert(tree.leaf_table.end(), blocks.begin(), blocks.end());
			}

			if (low24 > 0xFFFFFF)
			{
				throw std::runtime_error("lightgrid_tree: node/leaf table exceeds 24-bit addressing");
			}
			tree.node_table[n] = (static_cast<unsigned int>(mask) << 24) | low24;
		}

		tree.node_count = static_cast<int>(tree.node_table.size());
		return tree;
	}

	void dilate_samples(std::vector<grid_sample>& samples, int passes, const unsigned char* sample_classes)
	{
		if (samples.empty() || passes <= 0)
		{
			return;
		}

		struct cell
		{
			grid_sample sample;
			unsigned char cls;
		};
		std::map<unsigned long long, cell> cells;
		for (size_t i = 0; i < samples.size(); i++)
		{
			const auto& s = samples[i];
			const unsigned char cls = sample_classes ? sample_classes[i] : 0;
			cells[pack_coord(s.pos[0], s.pos[1], s.pos[2])] = { s, cls };
		}

		for (int pass = 0; pass < passes; pass++)
		{
			std::set<unsigned long long> candidates;
			for (const auto& [key, c] : cells)
			{
				const int cx = static_cast<int>(key & 0x1FFFFF);
				const int cy = static_cast<int>((key >> 21) & 0x1FFFFF);
				const int cz = static_cast<int>((key >> 42) & 0x1FFFFF);
				for (int dz = -1; dz <= 1; dz++)
				{
					for (int dy = -1; dy <= 1; dy++)
					{
						for (int dx = -1; dx <= 1; dx++)
						{
							if (!dx && !dy && !dz)
							{
								continue;
							}
							const int nx = cx + dx;
							const int ny = cy + dy;
							const int nz = cz + dz;
							if (nx < 0 || ny < 0 || nz < 0 || nx > 0xFFFF || ny > 0xFFFF || nz > 0xFFFF)
							{
								continue;
							}
							const unsigned long long nkey = pack_coord(nx, ny, nz);
							if (!cells.count(nkey))
							{
								candidates.insert(nkey);
							}
						}
					}
				}
			}

			std::map<unsigned long long, cell> added;
			for (const unsigned long long ckey : candidates)
			{
				const int cx = static_cast<int>(ckey & 0x1FFFFF);
				const int cy = static_cast<int>((ckey >> 21) & 0x1FFFFF);
				const int cz = static_cast<int>((ckey >> 42) & 0x1FFFFF);

				int best_weight = 0x7FFFFFFF;
				unsigned char best_cls = 0xFF;
				const cell* best = nullptr;
				for (int dz = -1; dz <= 1; dz++)
				{
					for (int dy = -1; dy <= 1; dy++)
					{
						for (int dx = -1; dx <= 1; dx++)
						{
							if (!dx && !dy && !dz)
							{
								continue;
							}
							const int nx = cx + dx;
							const int ny = cy + dy;
							const int nz = cz + dz;
							if (nx < 0 || ny < 0 || nz < 0 || nx > 0xFFFF || ny > 0xFFFF || nz > 0xFFFF)
							{
								continue;
							}
							const auto it = cells.find(pack_coord(nx, ny, nz));
							if (it == cells.end())
							{
								continue;
							}
							const int weight = dx * dx + dy * dy + 4 * dz * dz;
							const unsigned char cls = it->second.cls;
							if (!best
								|| cls < best_cls
								|| (cls == best_cls && weight < best_weight))
							{
								best_cls = cls;
								best_weight = weight;
								best = &it->second;
							}
						}
					}
				}

				if (best)
				{
					cell nc{};
					nc.sample = best->sample;
					nc.sample.pos[0] = static_cast<unsigned short>(cx);
					nc.sample.pos[1] = static_cast<unsigned short>(cy);
					nc.sample.pos[2] = static_cast<unsigned short>(cz);
					nc.cls = best->cls;
					added[ckey] = nc;
				}
			}

			if (added.empty())
			{
				break;
			}
			for (const auto& [key, c] : added)
			{
				cells[key] = c;
			}
		}

		samples.clear();
		samples.reserve(cells.size());
		for (const auto& [key, c] : cells)
		{
			samples.push_back(c.sample);
		}
	}

	bool lookup(const tree_view& tree, const unsigned short pos[3], raw_result& out, bool* unsupported)
	{
		out = {};
		for (int axis = 0; axis < 3; axis++)
		{
			if (pos[axis] < tree.coord_min_grid_space[axis] || pos[axis] > tree.coord_max_grid_space[axis])
			{
				return false;
			}
		}

		const unsigned int tx = static_cast<unsigned int>(pos[0] - tree.coord_min_grid_space[0]) >> 2;
		const unsigned int ty = static_cast<unsigned int>(pos[1] - tree.coord_min_grid_space[1]) >> 2;
		const unsigned int tz = static_cast<unsigned int>(pos[2] - tree.coord_min_grid_space[2]);
		const int voxel = voxel_index_lut[(pos[0] & 3) | ((pos[1] & 3) << 2)];

		unsigned int bit = 1u << tree.max_depth;
		unsigned int node_index = 0;
		unsigned int chunk_mask = 0;
		int octant = 0;
		int leaf_address = -1;
		for (int depth = 0;; depth++)
		{
			if (node_index >= static_cast<unsigned int>(tree.node_count))
			{
				return false;
			}
			const unsigned int node = tree.node_table[node_index];
			chunk_mask = node >> 24;
			octant = ((tx & bit) ? 1 : 0) | ((ty & bit) ? 2 : 0) | ((tz & bit) ? 4 : 0);
			if (!(chunk_mask & (1u << octant)))
			{
				return false;
			}
			if (depth >= tree.max_depth)
			{
				leaf_address = static_cast<int>(node & 0xFFFFFF);
				break;
			}
			node_index = (node & 0xFFFFFF) + popcount16(chunk_mask & ((1u << octant) - 1));
			bit >>= 1;
		}
		if (leaf_address < 0 || leaf_address >= tree.leaf_table_size)
		{
			return false;
		}

		bit_reader br(tree.leaf_table + leaf_address, static_cast<size_t>(tree.leaf_table_size - leaf_address));
		chunk_header hdr{};
		read_chunk_header(br, chunk_mask, static_cast<int>(tree.size_bits),
			tree.default_color_index_bit_count, tree.default_light_index_bit_count, hdr);

		const int leaf_order = popcount16(chunk_mask & ((1u << octant) - 1));
		size_t skip = 0;
		for (int i = 0; i < leaf_order; i++)
		{
			skip += static_cast<size_t>(hdr.leaf_size[i]);
		}
		br.skip_bytes(skip);

		decoded_block block{};
		read_leaf_block(br, hdr, hdr.leaf_size[leaf_order], block, unsupported);
		if (br.overflowed())
		{
			if (unsupported)
			{
				*unsupported = true;
			}
			return false;
		}

		out.color_index = static_cast<unsigned int>(block.color.values[voxel]);
		out.light_index = static_cast<unsigned short>(block.light.values[voxel]);
		out.trace_lo = ((block.voxels >> voxel) & 1) != 0;
		out.trace_hi = ((block.voxels >> (voxel + 16)) & 1) != 0;
		return out.color_index != 0;
	}

	bool lookup(const tree_data& tree, const unsigned short pos[3], raw_result& out)
	{
		tree_view view{};
		view.max_depth = tree.max_depth;
		view.node_count = tree.node_count;
		view.coord_min_grid_space = tree.coord_min_grid_space;
		view.coord_max_grid_space = tree.coord_max_grid_space;
		view.default_color_index_bit_count = tree.default_color_index_bit_count;
		view.default_light_index_bit_count = tree.default_light_index_bit_count;
		view.node_table = tree.node_table.data();
		view.leaf_table_size = static_cast<int>(tree.leaf_table.size());
		view.leaf_table = tree.leaf_table.data();
		view.size_bits = leaf_size_bits::h1;
		return lookup(view, pos, out, nullptr);
	}

	std::vector<grid_entry_ref> enumerate_row_data(
		const unsigned short mins[3], const unsigned short maxs[3],
		unsigned int row_axis, unsigned int col_axis,
		const unsigned short* row_data_start, const unsigned char* raw_row_data)
	{
		std::vector<grid_entry_ref> refs;
		const unsigned int row_count = static_cast<unsigned int>(maxs[row_axis]) - mins[row_axis] + 1;

		for (unsigned int row = 0; row < row_count; row++)
		{
			const unsigned short start = row_data_start[row];
			if (start == 0xFFFF)
			{
				continue;
			}

			const unsigned char* data = &raw_row_data[4u * start];
			unsigned short col_start, col_count, z_start, z_count;
			unsigned int first_entry;
			std::memcpy(&col_start, data, 2);
			std::memcpy(&col_count, data + 2, 2);
			std::memcpy(&z_start, data + 4, 2);
			std::memcpy(&z_count, data + 6, 2);
			std::memcpy(&first_entry, data + 8, 4);

			const bool wide_z = z_count > 0xFF;
			const unsigned char* run = data + 12;
			unsigned int entry_base = first_entry;
			unsigned int col = 0;

			while (col < col_count)
			{
				const unsigned int count = run[0];
				const unsigned int height = run[1];
				if (!count)
				{
					break;
				}
				if (height)
				{
					const unsigned int z_base = run[2] + (wide_z ? (static_cast<unsigned int>(run[3]) << 8) : 0);
					for (unsigned int c = 0; c < count; c++)
					{
						for (unsigned int z = 0; z < height; z++)
						{
							grid_entry_ref ref{};
							ref.pos[row_axis] = static_cast<unsigned short>(mins[row_axis] + row);
							ref.pos[col_axis] = static_cast<unsigned short>(col_start + col + c);
							ref.pos[2] = static_cast<unsigned short>(z_start + z_base + z);
							ref.entry_index = entry_base + c * height + z;
							refs.push_back(ref);
						}
					}
					entry_base += count * height;
					run += 3 + (wide_z ? 1 : 0);
				}
				else
				{
					run += 2;
				}
				col += count;
			}
		}
		return refs;
	}
}
