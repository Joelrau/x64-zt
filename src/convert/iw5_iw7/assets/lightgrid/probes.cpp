#include <std_include.hpp>
#include "probes.hpp"
#include <zonetool.hpp>

#include <algorithm>
#include <array>
#include <utility>
#include <cmath>
#include <cstring>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lightgrid_probes
{
	namespace
	{
		constexpr unsigned int no_index = 0xFFFFFFFF;

	constexpr float probe_epsilon = 1.0f / 32.0f;

		unsigned short to_half(const float x)
		{
			unsigned int bits;
			std::memcpy(&bits, &x, sizeof(bits));

			const auto sign = static_cast<unsigned short>((bits >> 16) & 0x8000);
			auto exponent = static_cast<int>((bits >> 23) & 0xFF) - 127 + 15;
			auto mantissa = bits & 0x7FFFFF;

			if (exponent >= 31)
			{
				return static_cast<unsigned short>(sign | 0x7BFF);
			}
			if (exponent <= 0)
			{
				return sign;
			}
			return static_cast<unsigned short>(sign | (exponent << 10) | (mantissa >> 13));
		}

		constexpr int cube_tets[6][4] = {
			{ 0, 1, 3, 7 },
			{ 0, 3, 2, 7 },
			{ 0, 2, 6, 7 },
			{ 0, 6, 4, 7 },
			{ 0, 4, 5, 7 },
			{ 0, 5, 1, 7 },
		};

		struct face_key
		{
			unsigned int v[3];
			bool operator<(const face_key& o) const
			{
				if (v[0] != o.v[0]) return v[0] < o.v[0];
				if (v[1] != o.v[1]) return v[1] < o.v[1];
				return v[2] < o.v[2];
			}
		};

		face_key make_face(unsigned int a, unsigned int b, unsigned int c)
		{
			unsigned int t[3] = { a, b, c };
			std::sort(t, t + 3);
			return face_key{ { t[0], t[1], t[2] } };
		}
	}

	namespace
	{
		void sh_basis(const float d[3], float out[9])
		{
			const float x = -d[0], y = -d[1], z = d[2];
			out[0] = 0.2820948f;
			out[1] = 0.4886025f * y;
			out[2] = 0.4886025f * z;
			out[3] = 0.4886025f * x;
			out[4] = 1.0925484f * x * y;
			out[5] = 1.0925484f * y * z;
			out[6] = 0.3153916f * ((3.0f * z * z) - 1.0f);
			out[7] = 1.0925484f * x * z;
			out[8] = 0.5462742f * ((x * x) - (y * y));
		}
	}

	void constant_sh(const float rgb[3], const float scale, float out_sh[27])
	{
		std::memset(out_sh, 0, sizeof(float) * 27);
		for (int ch = 0; ch < 3; ch++)
		{
			out_sh[ch * 9] = rgb[ch] * scale * 3.5449077f;
		}
	}

	void project_sh(const float samples[][3], const float directions[][3], const unsigned int count,
		const float scale, float out_sh[27])
	{
		std::memset(out_sh, 0, sizeof(float) * 27);
		if (!count)
		{
			return;
		}

		double ata[9][9] = {};
		double atl[9][3] = {};
		for (unsigned int i = 0; i < count; i++)
		{
			float basis[9];
			sh_basis(directions[i], basis);
			for (int r = 0; r < 9; r++)
			{
				for (int c = 0; c < 9; c++)
				{
					ata[r][c] += static_cast<double>(basis[r]) * basis[c];
				}
				for (int ch = 0; ch < 3; ch++)
				{
					atl[r][ch] += static_cast<double>(basis[r]) * samples[i][ch];
				}
			}
		}

		double m[9][12];
		for (int r = 0; r < 9; r++)
		{
			for (int c = 0; c < 9; c++) m[r][c] = ata[r][c];
			for (int ch = 0; ch < 3; ch++) m[r][9 + ch] = atl[r][ch];
		}
		for (int col = 0; col < 9; col++)
		{
			int pivot = col;
			for (int r = col + 1; r < 9; r++)
			{
				if (std::fabs(m[r][col]) > std::fabs(m[pivot][col])) pivot = r;
			}
			if (std::fabs(m[pivot][col]) < 1e-12)
			{
				return;
			}
			if (pivot != col)
			{
				for (int c = 0; c < 12; c++) std::swap(m[col][c], m[pivot][c]);
			}
			const auto inv = 1.0 / m[col][col];
			for (int c = col; c < 12; c++) m[col][c] *= inv;
			for (int r = 0; r < 9; r++)
			{
				if (r == col) continue;
				const auto f = m[r][col];
				if (f == 0.0) continue;
				for (int c = col; c < 12; c++) m[r][c] -= f * m[col][c];
			}
		}

		for (int ch = 0; ch < 3; ch++)
		{
			for (int k = 0; k < 9; k++)
			{
				out_sh[ch * 9 + k] = static_cast<float>(m[k][9 + ch] * scale);
			}
		}
	}

	void encode_probe_sh(const float sh[28], unsigned short out_coeffs[32])
	{
		std::memset(out_coeffs, 0, sizeof(unsigned short) * 32);
		for (int k = 0; k < 28; k++)
		{
			out_coeffs[k] = zonetool::half_float::float_to_half(sh[k]);
		}
	}

	probe_volume build(const build_params& params, const sampler& sample, const float sh_scale)
	{
		probe_volume out{};

		float size[3];
		for (int i = 0; i < 3; i++)
		{
			size[i] = params.bounds_max[i] - params.bounds_min[i];
			if (size[i] <= 0.0f)
			{
				return out;
			}
		}

		float origin[3];
		int root_dim[3];
		std::uint64_t probe_total = 0;

		const auto measure = [&](const int candidate_shift)
		{
			const auto root = static_cast<float>(1 << (candidate_shift + 4));
			std::uint64_t total = 1;
			for (int i = 0; i < 3; i++)
			{
				origin[i] = std::floor(params.bounds_min[i] / root) * root;
				const auto span = std::ceil(static_cast<double>(params.bounds_max[i] - origin[i]) / root);
				const auto dim = static_cast<int>(std::min(std::max(span, 1.0), 1.0e6));
				root_dim[i] = dim;
				total *= (static_cast<std::uint64_t>(dim) * 16) + 1;
				if (total > max_probes)
				{
					total = static_cast<std::uint64_t>(max_probes) + 1;
				}
			}
			return total;
		};

		const auto count_sparse_corners = [&](const int candidate_shift, const float* org,
			const int* dim)
		{
			const auto lsz = static_cast<float>(1 << candidate_shift);
			const int cx_n = dim[0] * 16, cy_n = dim[1] * 16, cz_n = dim[2] * 16;
			std::unordered_set<std::uint64_t> used;
			used.reserve(max_probes / 2);
			const std::uint64_t stride_y = static_cast<std::uint64_t>(cx_n) + 1;
			const std::uint64_t stride_z = stride_y * (static_cast<std::uint64_t>(cy_n) + 1);
			for (int cz = 0; cz < cz_n; cz++)
			{
				for (int cy = 0; cy < cy_n; cy++)
				{
					for (int cx = 0; cx < cx_n; cx++)
					{
						const float lo[3] = {
							org[0] + static_cast<float>(cx) * lsz,
							org[1] + static_cast<float>(cy) * lsz,
							org[2] + static_cast<float>(cz) * lsz,
						};
						bool inside = true;
						for (int i = 0; i < 3 && inside; i++)
						{
							if (lo[i] + lsz <= params.bounds_min[i]) inside = false;
							if (lo[i] >= params.bounds_max[i]) inside = false;
						}
						if (!inside) continue;
						if (params.cell_occupied && !params.cell_occupied(lo, lsz)) continue;
						for (int c = 0; c < 8; c++)
						{
							used.insert(static_cast<std::uint64_t>(cz + ((c >> 2) & 1)) * stride_z
								+ static_cast<std::uint64_t>(cy + ((c >> 1) & 1)) * stride_y
								+ static_cast<std::uint64_t>(cx + (c & 1)));
						}
						if (used.size() > max_probes)
						{
							return static_cast<std::uint64_t>(max_probes) + 1;
						}
					}
				}
			}
			return static_cast<std::uint64_t>(used.size());
		};

		constexpr std::uint64_t sparse_scan_cell_budget = 12ull * 1024 * 1024;
		int leaf_shift = params.leaf_shift > 0 ? params.leaf_shift : 5;
		for (; leaf_shift <= 12; leaf_shift++)
		{
			probe_total = measure(leaf_shift);
			if (probe_total <= max_probes)
			{
				break;
			}
			if (!params.cell_occupied)
			{
				continue;
			}
			const std::uint64_t cell_count = static_cast<std::uint64_t>(root_dim[0]) * 16
				* (static_cast<std::uint64_t>(root_dim[1]) * 16)
				* (static_cast<std::uint64_t>(root_dim[2]) * 16);
			if (cell_count > sparse_scan_cell_budget)
			{
				continue;
			}
			probe_total = count_sparse_corners(leaf_shift, origin, root_dim);
			if (probe_total <= max_probes)
			{
				break;
			}
		}
		if (leaf_shift > 12)
		{
			return out;
		}

		const int shift[3] = { leaf_shift + 4, leaf_shift + 2, leaf_shift };
		const auto leaf_size = static_cast<float>(1 << leaf_shift);
		const auto root_size = static_cast<float>(1 << shift[0]);

		const int cells[3] = { root_dim[0] * 16, root_dim[1] * 16, root_dim[2] * 16 };

		const auto dense_index = [&](const int x, const int y, const int z)
		{
			return static_cast<std::uint64_t>(z) * (cells[1] + 1) * (cells[0] + 1)
				+ static_cast<std::uint64_t>(y) * (cells[0] + 1)
				+ static_cast<std::uint64_t>(x);
		};

		const auto cell_present = [&](const int cx, const int cy, const int cz)
		{
			const float lo[3] = {
				origin[0] + static_cast<float>(cx) * leaf_size,
				origin[1] + static_cast<float>(cy) * leaf_size,
				origin[2] + static_cast<float>(cz) * leaf_size,
			};
			for (int i = 0; i < 3; i++)
			{
				if (lo[i] + leaf_size <= params.bounds_min[i]) return false;
				if (lo[i] >= params.bounds_max[i]) return false;
			}
			if (params.cell_occupied && !params.cell_occupied(lo, leaf_size)) return false;
			return true;
		};

		std::unordered_map<std::uint64_t, unsigned int> probe_of_corner;
		probe_of_corner.reserve(static_cast<size_t>(probe_total / 4) + 64);
		{
			std::vector<std::uint64_t> corners;
			for (int cz = 0; cz < cells[2]; cz++)
			{
				for (int cy = 0; cy < cells[1]; cy++)
				{
					for (int cx = 0; cx < cells[0]; cx++)
					{
						if (!cell_present(cx, cy, cz)) continue;
						for (int c = 0; c < 8; c++)
						{
							corners.push_back(dense_index(cx + (c & 1), cy + ((c >> 1) & 1),
								cz + ((c >> 2) & 1)));
						}
						if (corners.size() > 8ull * max_probes)
						{
							std::sort(corners.begin(), corners.end());
							corners.erase(std::unique(corners.begin(), corners.end()),
								corners.end());
						}
					}
				}
			}
			std::sort(corners.begin(), corners.end());
			corners.erase(std::unique(corners.begin(), corners.end()), corners.end());
			if (corners.empty() || corners.size() > max_probes)
			{
				return out;
			}
			probe_of_corner.reserve(corners.size() * 2);
			unsigned int next = 0;
			for (const auto key : corners)
			{
				probe_of_corner.emplace(key, next++);
			}
			out.probe_count = static_cast<unsigned int>(corners.size());
			out.probes.resize(static_cast<size_t>(out.probe_count) * 32);
			out.probe_positions.resize(static_cast<size_t>(out.probe_count) * 3);
		}

		const auto probe_index = [&](const int x, const int y, const int z)
		{
			const auto it = probe_of_corner.find(dense_index(x, y, z));
			return it == probe_of_corner.end() ? 0u : it->second;
		};

		const std::uint64_t probe_plane = static_cast<std::uint64_t>(cells[1] + 1) * (cells[0] + 1);
		double accum[3] = { 0.0, 0.0, 0.0 };
		for (const auto& kv : probe_of_corner)
		{
			const auto key = kv.first;
			const auto i = kv.second;
			const int z = static_cast<int>(key / probe_plane);
			const int y = static_cast<int>((key % probe_plane) / (cells[0] + 1));
			const int x = static_cast<int>((key % probe_plane) % (cells[0] + 1));
			float p[3] = {
				origin[0] + static_cast<float>(x) * leaf_size + probe_epsilon,
				origin[1] + static_cast<float>(y) * leaf_size + probe_epsilon,
				origin[2] + static_cast<float>(z) * leaf_size + probe_epsilon,
			};
			std::memcpy(&out.probe_positions[i * 3], p, sizeof(p));

			float sh[28] = {};
			if (sample) sample(p, sh);
			encode_probe_sh(sh, &out.probes[i * 32]);
			for (int c = 0; c < 3; c++) accum[c] += sh[c * 9];
		}
		probe_total = out.probe_count;

		float fallback_sh[28] = {};
		for (int c = 0; c < 3; c++)
		{
			fallback_sh[c * 9] = static_cast<float>(accum[c] / static_cast<double>(probe_total));
		}
		fallback_sh[27] = 1.0f;
		unsigned short fallback[32];
		encode_probe_sh(fallback_sh, fallback);
		std::memcpy(out.zone_fallback_coeffs, fallback, sizeof(out.zone_fallback_coeffs));

		struct pending_leaf { int cx, cy, cz; };
		std::vector<pending_leaf> leaves;

		const auto l2_has_content = [&](const int bx, const int by, const int bz)
		{
			for (int i = 0; i < 64; i++)
			{
				const int lx = bx + (i & 3), ly = by + ((i >> 2) & 3), lz = bz + ((i >> 4) & 3);
				if (cell_present(lx, ly, lz)) return true;
			}
			return false;
		};
		const auto l1_has_content = [&](const int bx, const int by, const int bz)
		{
			for (int i = 0; i < 64; i++)
			{
				const int cx = bx + (i & 3) * 4, cy = by + ((i >> 2) & 3) * 4, cz = bz + ((i >> 4) & 3) * 4;
				if (l2_has_content(cx, cy, cz)) return true;
			}
			return false;
		};

		const int columns = root_dim[0] * root_dim[1];
		out.top_down_view_nodes.resize(columns);

		std::vector<std::array<int, 3>> root_cells;
		for (int cy = 0; cy < root_dim[1]; cy++)
		{
			for (int cx = 0; cx < root_dim[0]; cx++)
			{
				const int col = cy * root_dim[0] + cx;
				int z_lo = -1, z_hi = -1;
				for (int cz = 0; cz < root_dim[2]; cz++)
				{
					if (l1_has_content(cx * 16, cy * 16, cz * 16))
					{
						if (z_lo < 0) z_lo = cz;
						z_hi = cz;
					}
				}
				if (z_lo < 0)
				{
					out.top_down_view_nodes[col] = { -1, 0x7FFFFFFF, static_cast<int>(0x80000000) };
					continue;
				}
				out.top_down_view_nodes[col] = { static_cast<int>(root_cells.size()), z_lo, z_hi };
				for (int cz = z_lo; cz <= z_hi; cz++)
				{
					root_cells.push_back({ cx * 16, cy * 16, cz * 16 });
				}
			}
		}

		std::vector<std::array<int, 3>> level1;
		out.internal_nodes.resize(root_cells.size());
		for (size_t r = 0; r < root_cells.size(); r++)
		{
			const auto& o = root_cells[r];
			unsigned long long mask = 0;
			const auto first = static_cast<int>(level1.size());
			for (int i = 0; i < 64; i++)
			{
				const int bx = o[0] + (i & 3) * 4, by = o[1] + ((i >> 2) & 3) * 4, bz = o[2] + ((i >> 4) & 3) * 4;
				if (!l2_has_content(bx, by, bz)) continue;
				mask |= 1ull << i;
				level1.push_back({ bx, by, bz });
			}
			auto& n = out.internal_nodes[r];
			n.first_node_index[0] = first;
			n.first_node_index[1] = first;
			n.child_node_mask[0] = static_cast<unsigned int>(mask & 0xFFFFFFFF);
			n.child_node_mask[1] = static_cast<unsigned int>(mask >> 32);
		}

		const auto level1_base = out.internal_nodes.size();
		out.internal_nodes.resize(level1_base + level1.size());
		for (size_t i = 0; i < level1.size(); i++)
		{
			const auto& o = level1[i];
			unsigned long long mask = 0;
			const auto first = static_cast<int>(leaves.size());
			for (int k = 0; k < 64; k++)
			{
				const int lx = o[0] + (k & 3), ly = o[1] + ((k >> 2) & 3), lz = o[2] + ((k >> 4) & 3);
				if (!cell_present(lx, ly, lz)) continue;
				mask |= 1ull << k;
				leaves.push_back({ lx, ly, lz });
			}
			auto& n = out.internal_nodes[level1_base + i];
			n.first_node_index[0] = static_cast<int>(first | 0x80000000);
			n.first_node_index[1] = n.first_node_index[0];
			n.child_node_mask[0] = static_cast<unsigned int>(mask & 0xFFFFFFFF);
			n.child_node_mask[1] = static_cast<unsigned int>(mask >> 32);
		}

		for (size_t r = 0; r < root_cells.size(); r++)
		{
			auto& n = out.internal_nodes[r];
			n.first_node_index[0] += static_cast<int>(level1_base);
			n.first_node_index[1] = n.first_node_index[0];
		}

		out.leaf_count = static_cast<unsigned int>(leaves.size());
		out.leaf_nodes.assign(leaves.size(), 0);
		out.light_list.assign(1, 0);

		out.leaf_size = leaf_size;
		out.leaf_bounds_min.resize(leaves.size() * 3);
		for (size_t l = 0; l < leaves.size(); l++)
		{
			out.leaf_bounds_min[l * 3 + 0] = origin[0] + static_cast<float>(leaves[l].cx) * leaf_size;
			out.leaf_bounds_min[l * 3 + 1] = origin[1] + static_cast<float>(leaves[l].cy) * leaf_size;
			out.leaf_bounds_min[l * 3 + 2] = origin[2] + static_cast<float>(leaves[l].cz) * leaf_size;
		}

		out.voxel_start_tetrahedron.assign(leaves.size(), no_index);
		out.tetrahedrons.reserve(leaves.size() * 24);

		const unsigned int corner_bit = params.corner_flag ? 0x80000000u : 0u;
		std::map<face_key, std::pair<unsigned int, int>> faces;

		for (size_t l = 0; l < leaves.size(); l++)
		{
			const auto& c = leaves[l];
			unsigned int corner[8];
			for (int i = 0; i < 8; i++)
			{
				corner[i] = probe_index(c.cx + (i & 1), c.cy + ((i >> 1) & 1), c.cz + ((i >> 2) & 1));
			}

			out.voxel_start_tetrahedron[l] = static_cast<unsigned int>(out.tetrahedrons.size() / 4);
			for (const auto& t : cube_tets)
			{
				for (int k = 0; k < 4; k++)
				{
					out.tetrahedrons.push_back(corner[t[k]] | corner_bit);
				}
			}
		}
		out.tetrahedron_count = static_cast<unsigned int>(out.tetrahedrons.size() / 4);

		out.tetrahedron_neighbors.assign(out.tetrahedrons.size(), no_index);
		for (unsigned int t = 0; t < out.tetrahedron_count; t++)
		{
			for (int k = 0; k < 4; k++)
			{
				unsigned int v[3];
				int n = 0;
				for (int j = 0; j < 4; j++)
				{
					if (j != k) v[n++] = out.tetrahedrons[t * 4 + j] & 0x7FFFFFFFu;
				}
				const auto key = make_face(v[0], v[1], v[2]);
				const auto it = faces.find(key);
				if (it == faces.end())
				{
					faces.emplace(key, std::make_pair(t, k));
				}
				else
				{
					const auto other = it->second;
					out.tetrahedron_neighbors[t * 4 + k] = other.first;
					out.tetrahedron_neighbors[other.first * 4 + other.second] = t;
					faces.erase(it);
				}
			}
		}

		for (int i = 0; i < 3; i++)
		{
			out.root_node_dimension[i] = root_dim[i];
			out.node_coord_bit_shift[i] = shift[i];
			out.bound_min[i] = origin[i];
			out.bound_max[i] = origin[i] + static_cast<float>(root_dim[i]) * root_size;
		}

		out.zone_num_probes = out.probe_count;
		out.zone_first_probe = 0;
		out.zone_num_tetrahedrons = out.tetrahedron_count;
		out.zone_first_tetrahedron = 0;
		out.zone_first_voxel_tetrahedron_index = 0;
		out.zone_num_voxel_tetrahedron_indices = out.leaf_count;

		out.valid = out.tetrahedron_count != 0;
		return out;
	}
}
