#include <std_include.hpp>
#include "sh.hpp"

#include <cassert>
#include <cmath>
#include <cstring>

namespace lightgrid_sh
{
	const float grid_basis_dirs[56][3] =
	{
		{ -0.57727051f, -0.57727051f, -0.57727051f },
		{ -0.22941080f, -0.68823242f, -0.68823242f },
		{  0.22941084f, -0.68823242f, -0.68823242f },
		{  0.57727051f, -0.57727051f, -0.57727051f },
		{ -0.68823242f, -0.22941080f, -0.68823242f },
		{ -0.30151364f, -0.30151364f, -0.90454102f },
		{  0.30151370f, -0.30151364f, -0.90454102f },
		{  0.68823242f, -0.22941080f, -0.68823242f },
		{ -0.68823242f,  0.22941084f, -0.68823242f },
		{ -0.30151364f,  0.30151370f, -0.90454102f },
		{  0.30151370f,  0.30151370f, -0.90454102f },
		{  0.68823242f,  0.22941084f, -0.68823242f },
		{ -0.57727051f,  0.57727051f, -0.57727051f },
		{ -0.22941080f,  0.68823242f, -0.68823242f },
		{  0.22941084f,  0.68823242f, -0.68823242f },
		{  0.57727051f,  0.57727051f, -0.57727051f },
		{ -0.68823242f, -0.68823242f, -0.22941080f },
		{ -0.30151364f, -0.90454102f, -0.30151364f },
		{  0.30151370f, -0.90454102f, -0.30151364f },
		{  0.68823242f, -0.68823242f, -0.22941080f },
		{ -0.90454102f, -0.30151364f, -0.30151364f },
		{  0.90454102f, -0.30151364f, -0.30151364f },
		{ -0.90454102f,  0.30151370f, -0.30151364f },
		{  0.90454102f,  0.30151370f, -0.30151364f },
		{ -0.68823242f,  0.68823242f, -0.22941080f },
		{ -0.30151364f,  0.90454102f, -0.30151364f },
		{  0.30151370f,  0.90454102f, -0.30151364f },
		{  0.68823242f,  0.68823242f, -0.22941080f },
		{ -0.68823242f, -0.68823242f,  0.22941084f },
		{ -0.30151364f, -0.90454102f,  0.30151370f },
		{  0.30151370f, -0.90454102f,  0.30151370f },
		{  0.68823242f, -0.68823242f,  0.22941084f },
		{ -0.90454102f, -0.30151364f,  0.30151370f },
		{  0.90454102f, -0.30151364f,  0.30151370f },
		{ -0.90454102f,  0.30151370f,  0.30151370f },
		{  0.90454102f,  0.30151370f,  0.30151370f },
		{ -0.68823242f,  0.68823242f,  0.22941084f },
		{ -0.30151364f,  0.90454102f,  0.30151370f },
		{  0.30151370f,  0.90454102f,  0.30151370f },
		{  0.68823242f,  0.68823242f,  0.22941084f },
		{ -0.57727051f, -0.57727051f,  0.57727051f },
		{ -0.22941080f, -0.68823242f,  0.68823242f },
		{  0.22941084f, -0.68823242f,  0.68823242f },
		{  0.57727051f, -0.57727051f,  0.57727051f },
		{ -0.68823242f, -0.22941080f,  0.68823242f },
		{ -0.30151364f, -0.30151364f,  0.90454102f },
		{  0.30151370f, -0.30151364f,  0.90454102f },
		{  0.68823242f, -0.22941080f,  0.68823242f },
		{ -0.68823242f,  0.22941084f,  0.68823242f },
		{ -0.30151364f,  0.30151370f,  0.90454102f },
		{  0.30151370f,  0.30151370f,  0.90454102f },
		{  0.68823242f,  0.22941084f,  0.68823242f },
		{ -0.57727051f,  0.57727051f,  0.57727051f },
		{ -0.22941080f,  0.68823242f,  0.68823242f },
		{  0.22941084f,  0.68823242f,  0.68823242f },
		{  0.57727051f,  0.57727051f,  0.57727051f },
	};

	namespace
	{
		constexpr float eval_epsilon = 3.814697265625e-06f;

		float range_from_exponent(int e)
		{
			return (e < 0) ? (1.0f / static_cast<float>(1 << -e)) : static_cast<float>(1 << e);
		}

		int bits_for_exponent(const encoder_config& cfg, int e)
		{
			if (e <= cfg.range_exp_8bits)
			{
				return 8;
			}
			return (e <= cfg.range_exp_12bits) ? 12 : 16;
		}

		unsigned int read_u16(const unsigned char* p)
		{
			return static_cast<unsigned int>(p[0]) | (static_cast<unsigned int>(p[1]) << 8);
		}

		unsigned int decode_coeffs_8(int ch, const unsigned char* s, float range, sh_coeffs& out)
		{
			out.v[0][ch] = (range * 0.00390625f) * static_cast<float>(s[0]);
			const float scale = (range * 2.0f) * 0.00390625f;
			for (int i = 1; i < 9; i++)
			{
				out.v[i][ch] = static_cast<float>(s[i]) * scale - range;
			}
			return 9;
		}

		unsigned int decode_coeffs_12(int ch, const unsigned char* s, float range, sh_coeffs& out)
		{
			out.v[0][ch] = (range * 0.000015258789f) * static_cast<float>(read_u16(s));
			const float scale = (range * 2.0f) * 0.00024414062f;
			const unsigned int q[8] =
			{
				static_cast<unsigned int>((s[9] >> 4) | (s[2] << 4)),
				static_cast<unsigned int>((s[10] & 0xF) | (s[3] << 4)),
				static_cast<unsigned int>((s[10] >> 4) | (s[4] << 4)),
				static_cast<unsigned int>((s[11] & 0xF) | (s[5] << 4)),
				static_cast<unsigned int>((s[11] >> 4) | (s[6] << 4)),
				static_cast<unsigned int>((s[12] & 0xF) | (s[7] << 4)),
				static_cast<unsigned int>((s[12] >> 4) | (s[8] << 4)),
				static_cast<unsigned int>((s[13] & 0xF) | (s[9] << 4)),
			};
			for (int i = 0; i < 8; i++)
			{
				out.v[i + 1][ch] = static_cast<float>(q[i]) * scale - range;
			}
			return 14;
		}

		unsigned int decode_coeffs_16(int ch, const unsigned char* s, float range, sh_coeffs& out)
		{
			out.v[0][ch] = (range * 0.000015258789f) * static_cast<float>(read_u16(s));
			const float scale = (range * 2.0f) * 0.000015258789f;
			for (int i = 1; i < 9; i++)
			{
				out.v[i][ch] = static_cast<float>(read_u16(s + 2 * i)) * scale - range;
			}
			return 18;
		}

		void basis_at(int dir, float b[9])
		{
			const float x = grid_basis_dirs[dir][0];
			const float y = grid_basis_dirs[dir][1];
			const float z = grid_basis_dirs[dir][2];
			b[0] = 1.0f;
			b[1] = x;
			b[2] = y;
			b[3] = z;
			b[4] = z * x;
			b[5] = z * y;
			b[6] = y * x;
			b[7] = 3.0f * (z * z) - 1.0f;
			b[8] = x * x - y * y;
		}

		float eval_sh(const sh_coeffs& sh, int dir, int ch)
		{
			float b[9];
			basis_at(dir, b);
			float v = 0.0f;
			for (int i = 0; i < 9; i++)
			{
				v += sh.v[i][ch] * b[i];
			}
			return v;
		}

		long quantize(double value, double range, long steps)
		{
			long q = std::lround(value * static_cast<double>(steps) / range);
			if (q < 0) q = 0;
			if (q > steps - 1) q = steps - 1;
			return q;
		}

		int select_exponent(const encoder_config& cfg, const double coeffs[9])
		{
			const double dc = coeffs[0] > 0.0 ? coeffs[0] : 0.0;
			double cmin = 0.0, cmax = 0.0;
			for (int i = 1; i < 9; i++)
			{
				if (coeffs[i] < cmin) cmin = coeffs[i];
				if (coeffs[i] > cmax) cmax = coeffs[i];
			}

			for (int e = -8; e <= 23; e++)
			{
				const double r = std::ldexp(1.0, e);
				const double n = (bits_for_exponent(cfg, e) == 8) ? 256.0 : 65536.0;
				if (dc <= r * (n - 1.0) / n && cmax <= r * (n - 2.0) / n && cmin >= -r)
				{
					return e;
				}
			}
			return 23;
		}
	}

	unsigned int decode_entry(const encoder_config& cfg, const unsigned char* stream, sh_coeffs& out)
	{
		const unsigned int header = read_u16(stream);
		const int exps[3] =
		{
			static_cast<int>((header >> 10) & 0x1F) - 8,
			static_cast<int>((header >> 5) & 0x1F) - 8,
			static_cast<int>(header & 0x1F) - 8,
		};

		unsigned int offset = 2;
		for (int ch = 0; ch < 3; ch++)
		{
			const int bits = bits_for_exponent(cfg, exps[ch]);
			const float range = range_from_exponent(exps[ch]);
			if (bits == 8)
			{
				offset += decode_coeffs_8(ch, &stream[offset], range, out);
			}
			else
			{
				offset += (offset & 1);
				offset += (bits == 12)
					? decode_coeffs_12(ch, &stream[offset], range, out)
					: decode_coeffs_16(ch, &stream[offset], range, out);
			}
		}
		return offset + (offset & 1);
	}

	int encode_entry(const encoder_config& cfg, const sh_coeffs& sh, std::vector<unsigned char>& stream)
	{
		assert(cfg.range_exp_12bits == cfg.range_exp_8bits);

		const int start = static_cast<int>(stream.size());

		int exps[3];
		for (int ch = 0; ch < 3; ch++)
		{
			double c[9];
			for (int i = 0; i < 9; i++)
			{
				c[i] = sh.v[i][ch];
			}
			exps[ch] = select_exponent(cfg, c);
		}

		const unsigned int header =
			(static_cast<unsigned int>(exps[0] + 8) << 10) |
			(static_cast<unsigned int>(exps[1] + 8) << 5) |
			static_cast<unsigned int>(exps[2] + 8);
		stream.push_back(static_cast<unsigned char>(header & 0xFF));
		stream.push_back(static_cast<unsigned char>((header >> 8) & 0xFF));

		for (int ch = 0; ch < 3; ch++)
		{
			const int bits = bits_for_exponent(cfg, exps[ch]);
			const double range = std::ldexp(1.0, exps[ch]);
			const double dc = sh.v[0][ch] > 0.0f ? sh.v[0][ch] : 0.0;

			if (bits == 8)
			{
				stream.push_back(static_cast<unsigned char>(quantize(dc, range, 256)));
				for (int i = 1; i < 9; i++)
				{
					stream.push_back(static_cast<unsigned char>(quantize(sh.v[i][ch] + range, 2.0 * range, 256)));
				}
			}
			else
			{
				if (stream.size() & 1)
				{
					stream.push_back(0);
				}
				const long dc_q = quantize(dc, range, 65536);
				stream.push_back(static_cast<unsigned char>(dc_q & 0xFF));
				stream.push_back(static_cast<unsigned char>((dc_q >> 8) & 0xFF));
				for (int i = 1; i < 9; i++)
				{
					const long q = quantize(sh.v[i][ch] + range, 2.0 * range, 65536);
					stream.push_back(static_cast<unsigned char>(q & 0xFF));
					stream.push_back(static_cast<unsigned char>((q >> 8) & 0xFF));
				}
			}
		}

		if (stream.size() & 1)
		{
			stream.push_back(0);
		}
		return start;
	}

	void sh_to_ldr_colors(const sh_coeffs& sh, unsigned char rgb[56][3])
	{
		for (int d = 0; d < 56; d++)
		{
			for (int ch = 0; ch < 3; ch++)
			{
				float v = eval_sh(sh, d, ch);
				v = (v > eval_epsilon) ? v : eval_epsilon;
				float b = std::sqrt(v) * 127.5f;
				if (b > 255.0f)
				{
					b = 255.0f;
				}
				rgb[d][ch] = static_cast<unsigned char>(static_cast<int>(b));
			}
		}
	}

	void sh_to_hdr_colors(const sh_coeffs& sh, float rgb[56][3])
	{
		for (int d = 0; d < 56; d++)
		{
			for (int ch = 0; ch < 3; ch++)
			{
				const float v = eval_sh(sh, d, ch);
				rgb[d][ch] = (v > 0.0f) ? v : 0.0f;
			}
		}
	}

	namespace
	{
		void solve_weighted_fit(const float rgb[56][3], const double weights[3][56], sh_coeffs& out)
		{
			double g[3][9][9] = {};
			double rhs[9][3] = {};

			for (int d = 0; d < 56; d++)
			{
				float bf[9];
				basis_at(d, bf);
				double b[9];
				for (int i = 0; i < 9; i++)
				{
					b[i] = bf[i];
				}
				for (int ch = 0; ch < 3; ch++)
				{
					const double w = weights[ch][d];
					for (int i = 0; i < 9; i++)
					{
						for (int j = 0; j < 9; j++)
						{
							g[ch][i][j] += w * b[i] * b[j];
						}
						rhs[i][ch] += w * b[i] * static_cast<double>(rgb[d][ch]);
					}
				}
			}

			for (int ch = 0; ch < 3; ch++)
			{
				auto& m = g[ch];
				for (int col = 0; col < 9; col++)
				{
					int pivot = col;
					for (int row = col + 1; row < 9; row++)
					{
						if (std::fabs(m[row][col]) > std::fabs(m[pivot][col]))
						{
							pivot = row;
						}
					}
					if (pivot != col)
					{
						for (int j = 0; j < 9; j++) std::swap(m[col][j], m[pivot][j]);
						std::swap(rhs[col][ch], rhs[pivot][ch]);
					}
					const double inv = 1.0 / m[col][col];
					for (int row = 0; row < 9; row++)
					{
						if (row == col)
						{
							continue;
						}
						const double f = m[row][col] * inv;
						if (f == 0.0)
						{
							continue;
						}
						for (int j = col; j < 9; j++)
						{
							m[row][j] -= f * m[col][j];
						}
						rhs[row][ch] -= f * rhs[col][ch];
					}
				}
				for (int i = 0; i < 9; i++)
				{
					out.v[i][ch] = static_cast<float>(rhs[i][ch] / m[i][i]);
				}
			}
		}
	}

	void hdr_colors_to_sh(const float rgb[56][3], sh_coeffs& out)
	{
		double weights[3][56];
		for (int ch = 0; ch < 3; ch++)
		{
			for (int d = 0; d < 56; d++)
			{
				weights[ch][d] = 1.0;
			}
		}
		solve_weighted_fit(rgb, weights, out);

		constexpr double weight_floor = 6.2e-05;
		for (int pass = 0; pass < 2; pass++)
		{
			for (int ch = 0; ch < 3; ch++)
			{
				for (int d = 0; d < 56; d++)
				{
					double v = eval_sh(out, d, ch);
					if (v < weight_floor)
					{
						v = weight_floor;
					}
					weights[ch][d] = 1.0 / v;
				}
			}
			solve_weighted_fit(rgb, weights, out);
		}
	}

	void ldr_colors_to_hdr(const unsigned char rgb[56][3], float out[56][3])
	{
		for (int d = 0; d < 56; d++)
		{
			for (int ch = 0; ch < 3; ch++)
			{
				const float s = static_cast<float>(rgb[d][ch]) * (1.0f / 127.5f);
				out[d][ch] = s * s;
			}
		}
	}

	void ldr_colors_to_sh(const unsigned char rgb[56][3], sh_coeffs& out)
	{
		float hdr[56][3];
		ldr_colors_to_hdr(rgb, hdr);
		hdr_colors_to_sh(hdr, out);
	}

	palette build_palette_from_ldr(const unsigned char* colors, unsigned int color_count)
	{
		palette pal{};
		pal.config = default_encode_config;
		pal.entry_address.reserve(color_count);

		for (unsigned int i = 0; i < color_count; i++)
		{
			const auto& rgb = *reinterpret_cast<const unsigned char(*)[56][3]>(colors + 168ull * i);
			sh_coeffs sh{};
			ldr_colors_to_sh(rgb, sh);
			pal.entry_address.push_back(encode_entry(pal.config, sh, pal.bitstream));
		}
		return pal;
	}

	void extract_palette_to_ldr(const encoder_config& cfg, const unsigned char* bitstream,
		const int* entry_address, unsigned int entry_count, unsigned char* out_colors)
	{
		for (unsigned int i = 0; i < entry_count; i++)
		{
			sh_coeffs sh{};
			decode_entry(cfg, &bitstream[entry_address[i]], sh);
			auto& out = *reinterpret_cast<unsigned char(*)[56][3]>(out_colors + 168ull * i);
			sh_to_ldr_colors(sh, out);
		}
	}

	roundtrip_stats verify_sh_roundtrip(const encoder_config& cfg, const unsigned char* bitstream,
		const int* entry_address, unsigned int entry_count)
	{
		roundtrip_stats stats{};
		stats.entries = entry_count;

		double error_sum = 0.0;
		for (unsigned int i = 0; i < entry_count; i++)
		{
			sh_coeffs sh{};
			decode_entry(cfg, &bitstream[entry_address[i]], sh);
			unsigned char ldr[56][3];
			sh_to_ldr_colors(sh, ldr);

			sh_coeffs refit{};
			ldr_colors_to_sh(ldr, refit);
			std::vector<unsigned char> stream;
			encode_entry(default_encode_config, refit, stream);
			sh_coeffs decoded{};
			decode_entry(default_encode_config, stream.data(), decoded);
			unsigned char ldr2[56][3];
			sh_to_ldr_colors(decoded, ldr2);

			for (int d = 0; d < 56; d++)
			{
				for (int ch = 0; ch < 3; ch++)
				{
					const int err = std::abs(static_cast<int>(ldr[d][ch]) - static_cast<int>(ldr2[d][ch]));
					if (err > stats.max_byte_error)
					{
						stats.max_byte_error = err;
					}
					error_sum += err;
				}
			}
		}

		stats.avg_byte_error = entry_count ? error_sum / (entry_count * 56.0 * 3.0) : 0.0;
		return stats;
	}
}
