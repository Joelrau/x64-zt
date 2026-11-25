#include <std_include.hpp>

#include "../include.hpp"
#include "gfx_image.hpp"

#include <iw7/assets/gfximage.hpp>
#include <formats/iwi/iwi.hpp>
#include <formats/iwi/iwi_wavelet.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace gfx_image
	{
		namespace
		{
			namespace target = zonetool::iw7;

			enum image_format : std::uint8_t
			{
				format_none = 0,
				format_rgba = 1,
				format_rgb = 2,
				format_luminance_alpha = 3,
				format_luminance = 4,
				format_alpha = 5,
				format_dxt1 = 11,
				format_dxt3 = 12,
				format_dxt5 = 13,
				format_dxn = 14,
				format_d3d_argb = 200,
			};

			constexpr std::uint8_t default_gloss = 64;

			unsigned int block_bytes(const std::uint8_t format)
			{
				switch (format)
				{
				case format_dxt1: return 8;
				case format_dxt3:
				case format_dxt5:
				case format_dxn: return 16;
				default: return 0;
				}
			}

			unsigned int pixel_bytes(const std::uint8_t format)
			{
				switch (format)
				{
				case format_rgba:
				case format_d3d_argb: return 4;
				case format_rgb: return 3;
				case format_luminance_alpha: return 2;
				case format_luminance:
				case format_alpha: return 1;
				default: return 0;
				}
			}

			std::vector<std::size_t> mip_sizes(const std::uint8_t format, unsigned int width, unsigned int height)
			{
				std::vector<std::size_t> sizes;
				const auto block = block_bytes(format);
				const auto pixel = pixel_bytes(format);

				while (true)
				{
					sizes.push_back(block
						? static_cast<std::size_t>((width + 3) / 4) * ((height + 3) / 4) * block
						: static_cast<std::size_t>(width) * height * pixel);

					if (width <= 1 && height <= 1)
					{
						break;
					}

					width = std::max(1u, width / 2);
					height = std::max(1u, height / 2);
				}

				return sizes;
			}

			std::uint8_t normalize_format(const int format)
			{
				switch (static_cast<unsigned int>(format))
				{
				case 0x31545844: return format_dxt1;
				case 0x33545844: return format_dxt3;
				case 0x35545844: return format_dxt5;
				case 0x32495441: return format_dxn;
				case 0x15:
				case 0x16: return format_d3d_argb;
				case 0x32: return format_luminance;
				default: return format_none;
				}
			}

			bool read_iwi_header(const std::string& name, const std::string& file, ::iwi::iwx::GfxImageFileHeader& header)
			{
				return !file.empty() && ::iwi::read_header(name, {reinterpret_cast<const std::uint8_t*>(file.data()), file.size()}, header);
			}

			bool split_levels(const std::vector<std::size_t>& sizes, const std::size_t available,
				std::vector<std::size_t>& present, std::size_t& total)
			{
				std::size_t accumulated = 0;
				std::vector<std::size_t> suffix;
				for (auto iterator = sizes.rbegin(); iterator != sizes.rend(); ++iterator)
				{
					suffix.push_back(*iterator);
					accumulated += *iterator;

					if (accumulated == available)
					{
						present.assign(suffix.rbegin(), suffix.rend());
						total = accumulated;
						return true;
					}

					if (accumulated > available)
					{
						break;
					}
				}

				if (!sizes.empty() && available == sizes[0])
				{
					present.assign(1, sizes[0]);
					total = sizes[0];
					return true;
				}

				return false;
			}

			void bc4_decode_alpha(const std::uint8_t* block, std::uint8_t out[16])
			{
				const int a0 = block[0];
				const int a1 = block[1];

				int palette[8];
				palette[0] = a0;
				palette[1] = a1;
				if (a0 > a1)
				{
					for (auto k = 1; k <= 6; k++)
					{
						palette[1 + k] = ((7 - k) * a0 + k * a1) / 7;
					}
				}
				else
				{
					for (auto k = 1; k <= 4; k++)
					{
						palette[1 + k] = ((5 - k) * a0 + k * a1) / 5;
					}
					palette[6] = 0;
					palette[7] = 255;
				}

				std::uint64_t bits = 0;
				for (auto i = 0; i < 6; i++)
				{
					bits |= static_cast<std::uint64_t>(block[2 + i]) << (8 * i);
				}

				for (auto t = 0; t < 16; t++)
				{
					out[t] = static_cast<std::uint8_t>(palette[(bits >> (3 * t)) & 7]);
				}
			}

			void bc1_decode_green(const std::uint8_t* block, float out[16])
			{
				const unsigned int c0 = block[0] | (block[1] << 8);
				const unsigned int c1 = block[2] | (block[3] << 8);
				const auto g0 = static_cast<float>(((c0 >> 5) & 63) * 255 / 63);
				const auto g1 = static_cast<float>(((c1 >> 5) & 63) * 255 / 63);

				float palette[4];
				palette[0] = g0;
				palette[1] = g1;
				if (c0 > c1)
				{
					palette[2] = (2.0f * g0 + g1) / 3.0f;
					palette[3] = (g0 + 2.0f * g1) / 3.0f;
				}
				else
				{
					palette[2] = (g0 + g1) / 2.0f;
					palette[3] = 0.0f;
				}

				const unsigned int bits = block[4] | (block[5] << 8) | (block[6] << 16) | (block[7] << 24);
				for (auto t = 0; t < 16; t++)
				{
					out[t] = palette[(bits >> (2 * t)) & 3];
				}
			}

			constexpr int bc7_weights[4] = {0, 21, 43, 64};

			int bc7_interpolate(const int a, const int b, const int weight)
			{
				return (a * (64 - weight) + b * weight + 32) >> 6;
			}

			int bc7_quantize7(const int value)
			{
				return std::clamp((value * 127 + 127) / 255, 0, 127);
			}

			int bc7_expand7(const int value)
			{
				return (value << 1) | (value >> 6);
			}

			void bc7_fit(const std::uint8_t* values, int& low, int& high, int indices[16], const bool seven_bit)
			{
				const auto decode = [&](const int endpoint) { return seven_bit ? bc7_expand7(endpoint) : endpoint; };

				for (auto t = 0; t < 16; t++)
				{
					auto best = INT_MAX;
					for (auto i = 0; i < 4; i++)
					{
						const auto value = bc7_interpolate(decode(low), decode(high), bc7_weights[i]);
						const auto error = std::abs(value - static_cast<int>(values[t]));
						if (error < best)
						{
							best = error;
							indices[t] = i;
						}
					}
				}

				if (indices[0] >= 2)
				{
					std::swap(low, high);
					for (auto t = 0; t < 16; t++)
					{
						indices[t] = 3 - indices[t];
					}
				}
			}

			struct bit_writer
			{
				std::uint8_t* out;
				int position = 0;

				void put(const unsigned int value, const int count)
				{
					for (auto i = 0; i < count; i++, position++)
					{
						if ((value >> i) & 1)
						{
							out[position >> 3] |= static_cast<std::uint8_t>(1 << (position & 7));
						}
					}
				}
			};

			void bc7_encode_normal_block(const std::uint8_t green[16], const std::uint8_t alpha[16], std::uint8_t* out)
			{
				auto green_low = 255, green_high = 0, alpha_low = 255, alpha_high = 0;
				for (auto t = 0; t < 16; t++)
				{
					green_low = std::min<int>(green_low, green[t]);
					green_high = std::max<int>(green_high, green[t]);
					alpha_low = std::min<int>(alpha_low, alpha[t]);
					alpha_high = std::max<int>(alpha_high, alpha[t]);
				}

				auto g0 = bc7_quantize7(green_low);
				auto g1 = bc7_quantize7(green_high);
				int colour_indices[16];
				bc7_fit(green, g0, g1, colour_indices, true);

				int alpha_indices[16];
				bc7_fit(alpha, alpha_low, alpha_high, alpha_indices, false);

				const auto r7 = bc7_quantize7(255);
				const auto b7 = bc7_quantize7(0);

				std::memset(out, 0, 16);
				bit_writer bits{out};
				bits.put(1 << 5, 6);
				bits.put(0, 2);
				bits.put(r7, 7);
				bits.put(r7, 7);
				bits.put(g0, 7);
				bits.put(g1, 7);
				bits.put(b7, 7);
				bits.put(b7, 7);
				bits.put(alpha_low, 8);
				bits.put(alpha_high, 8);

				bits.put(colour_indices[0], 1);
				for (auto t = 1; t < 16; t++)
				{
					bits.put(colour_indices[t], 2);
				}

				bits.put(alpha_indices[0], 1);
				for (auto t = 1; t < 16; t++)
				{
					bits.put(alpha_indices[t], 2);
				}
			}

			void dxt5_to_bc7_normal(const std::uint8_t* source, std::uint8_t* destination, const std::size_t blocks)
			{
				for (std::size_t i = 0; i < blocks; i++)
				{
					const auto* in = source + i * 16;

					std::uint8_t x[16];
					bc4_decode_alpha(in, x);

					float y_float[16];
					bc1_decode_green(in + 8, y_float);

					std::uint8_t y[16];
					for (auto t = 0; t < 16; t++)
					{
						y[t] = static_cast<std::uint8_t>(std::clamp(y_float[t] + 0.5f, 0.0f, 255.0f));
					}

					bc7_encode_normal_block(y, x, destination + i * 16);
				}
			}

			void dxt1_to_bc3(const std::uint8_t* source, std::uint8_t* destination, const std::size_t blocks)
			{
				for (std::size_t i = 0; i < blocks; i++)
				{
					auto* out = destination + i * 16;
					std::memset(out, 0, 8);
					out[0] = default_gloss;
					out[1] = default_gloss;
					std::memcpy(out + 8, source + i * 8, 8);
				}
			}

			void bc1_decode_rgba(const std::uint8_t* block, std::uint8_t out[16][4])
			{
				const unsigned int c0 = block[0] | (block[1] << 8);
				const unsigned int c1 = block[2] | (block[3] << 8);

				const auto expand = [](const unsigned int colour, float rgba[4])
				{
					rgba[0] = static_cast<float>(((colour >> 11) & 31) * 255 / 31);
					rgba[1] = static_cast<float>(((colour >> 5) & 63) * 255 / 63);
					rgba[2] = static_cast<float>((colour & 31) * 255 / 31);
					rgba[3] = 255.0f;
				};

				float palette[4][4]{};
				expand(c0, palette[0]);
				expand(c1, palette[1]);

				for (auto k = 0; k < 3; k++)
				{
					if (c0 > c1)
					{
						palette[2][k] = (2.0f * palette[0][k] + palette[1][k]) / 3.0f;
						palette[3][k] = (palette[0][k] + 2.0f * palette[1][k]) / 3.0f;
					}
					else
					{
						palette[2][k] = (palette[0][k] + palette[1][k]) * 0.5f;
					}
				}

				palette[2][3] = 255.0f;
				palette[3][3] = c0 > c1 ? 255.0f : 0.0f;

				const unsigned int bits = block[4] | (block[5] << 8) | (block[6] << 16) | (block[7] << 24);
				for (auto t = 0; t < 16; t++)
				{
					const auto entry = (bits >> (2 * t)) & 3;
					for (auto k = 0; k < 4; k++)
					{
						out[t][k] = static_cast<std::uint8_t>(palette[entry][k]);
					}
				}
			}

			void store_block(std::vector<std::uint8_t>& rgba, const unsigned int width, const unsigned int height,
				const unsigned int block_x, const unsigned int block_y, const std::uint8_t texels[16][4])
			{
				for (auto t = 0u; t < 16; t++)
				{
					const auto x = block_x * 4 + (t % 4);
					const auto y = block_y * 4 + (t / 4);
					if (x < width && y < height)
					{
						std::memcpy(&rgba[(static_cast<std::size_t>(y) * width + x) * 4], texels[t], 4);
					}
				}
			}

			void decode_block(const std::uint8_t format, const std::uint8_t* block, std::uint8_t texels[16][4])
			{
				if (format == format_dxt1)
				{
					bc1_decode_rgba(block, texels);
					return;
				}

				bc1_decode_rgba(block + 8, texels);

				std::uint8_t alpha[16];
				if (format == format_dxt5)
				{
					bc4_decode_alpha(block, alpha);
				}
				else
				{
					for (auto t = 0; t < 16; t++)
					{
						alpha[t] = static_cast<std::uint8_t>(((block[t / 2] >> ((t & 1) * 4)) & 0xF) * 17);
					}
				}

				for (auto t = 0; t < 16; t++)
				{
					texels[t][3] = alpha[t];
				}
			}

			bool decode_level(const std::uint8_t format, const std::uint8_t* source, const unsigned int width, const unsigned int height,
				std::vector<std::uint8_t>& rgba)
			{
				const auto pixels = static_cast<std::size_t>(width) * height;
				rgba.assign(pixels * 4, 255);

				switch (format)
				{
				case format_dxt1:
				case format_dxt3:
				case format_dxt5:
				{
					const auto blocks_wide = (width + 3) / 4;
					const auto blocks_high = (height + 3) / 4;
					for (auto y = 0u; y < blocks_high; y++)
					{
						for (auto x = 0u; x < blocks_wide; x++)
						{
							std::uint8_t texels[16][4];
							decode_block(format, source + (static_cast<std::size_t>(y) * blocks_wide + x) * block_bytes(format), texels);
							store_block(rgba, width, height, x, y, texels);
						}
					}
					return true;
				}
				case format_rgba:
					std::memcpy(rgba.data(), source, pixels * 4);
					return true;
				case format_d3d_argb:
					for (std::size_t i = 0; i < pixels; i++)
					{
						rgba[i * 4 + 0] = source[i * 4 + 2];
						rgba[i * 4 + 1] = source[i * 4 + 1];
						rgba[i * 4 + 2] = source[i * 4 + 0];
						rgba[i * 4 + 3] = source[i * 4 + 3];
					}
					return true;
				case format_rgb:
					for (std::size_t i = 0; i < pixels; i++)
					{
						std::memcpy(&rgba[i * 4], source + i * 3, 3);
					}
					return true;
				case format_luminance:
				case format_alpha:
					for (std::size_t i = 0; i < pixels; i++)
					{
						std::memset(&rgba[i * 4], source[i], 3);
					}
					return true;
				default:
					return false;
				}
			}

			bool has_alpha_channel(const std::uint8_t format)
			{
				return format == format_dxt3 || format == format_dxt5 || format == format_rgba || format == format_d3d_argb;
			}

			constexpr int bc7_weights4[16] = {0, 4, 9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64};

			float project(const float texel[4], const float origin[4], const float axis[4])
			{
				auto distance = 0.0f;
				for (auto c = 0; c < 4; c++)
				{
					distance += (texel[c] - origin[c]) * axis[c];
				}

				return distance;
			}

			void principal_axis(const float texels[16][4], const float mean[4], float axis[4])
			{
				for (auto c = 0; c < 4; c++)
				{
					auto low = texels[0][c];
					auto high = texels[0][c];
					for (auto t = 1; t < 16; t++)
					{
						low = std::min(low, texels[t][c]);
						high = std::max(high, texels[t][c]);
					}

					axis[c] = high - low;
				}

				for (auto iteration = 0; iteration < 8; iteration++)
				{
					float next[4]{};
					for (auto t = 0; t < 16; t++)
					{
						const auto distance = project(texels[t], mean, axis);
						for (auto c = 0; c < 4; c++)
						{
							next[c] += (texels[t][c] - mean[c]) * distance;
						}
					}

					const auto length_squared = next[0] * next[0] + next[1] * next[1] + next[2] * next[2] + next[3] * next[3];
					if (length_squared <= 1e-9f)
					{
						return;
					}

					for (auto c = 0; c < 4; c++)
					{
						axis[c] = next[c] / std::sqrt(length_squared);
					}
				}
			}

			bc7_mode6_fit bc7_fit_mode6(const float texels[16][4], const float low[4], const float high[4])
			{
				bc7_mode6_fit best;

				for (auto p_bits = 0; p_bits < 4; p_bits++)
				{
					bc7_mode6_fit fit;
					fit.low_p = p_bits >> 1;
					fit.high_p = p_bits & 1;
					fit.error = 0.0f;

					float low_colour[4], high_colour[4], direction[4];
					auto length_squared = 0.0f;
					for (auto c = 0; c < 4; c++)
					{
						fit.low[c] = std::clamp(static_cast<int>(std::lround((low[c] - fit.low_p) * 0.5f)), 0, 127);
						fit.high[c] = std::clamp(static_cast<int>(std::lround((high[c] - fit.high_p) * 0.5f)), 0, 127);
						low_colour[c] = static_cast<float>((fit.low[c] << 1) | fit.low_p);
						high_colour[c] = static_cast<float>((fit.high[c] << 1) | fit.high_p);
						direction[c] = high_colour[c] - low_colour[c];
						length_squared += direction[c] * direction[c];
					}

					float palette[16][4];
					for (auto k = 0; k < 16; k++)
					{
						for (auto c = 0; c < 4; c++)
						{
							palette[k][c] = std::floor((low_colour[c] * (64 - bc7_weights4[k]) + high_colour[c] * bc7_weights4[k] + 32.0f) / 64.0f);
						}
					}

					for (auto t = 0; t < 16; t++)
					{
						const auto position = length_squared > 1e-6f ? project(texels[t], low_colour, direction) / length_squared : 0.0f;
						const auto guess = std::clamp(static_cast<int>(std::lround(position * 15.0f)), 0, 15);

						auto best_distance = FLT_MAX;
						for (auto k = std::max(0, guess - 1); k <= std::min(15, guess + 1); k++)
						{
							auto distance = 0.0f;
							for (auto c = 0; c < 4; c++)
							{
								const auto difference = texels[t][c] - palette[k][c];
								distance += difference * difference;
							}

							if (distance < best_distance)
							{
								best_distance = distance;
								fit.indices[t] = k;
							}
						}

						fit.error += best_distance;
					}

					if (fit.error < best.error)
					{
						best = fit;
					}
				}

				return best;
			}

			bool bc7_refit_endpoints(const float texels[16][4], const int indices[16], float low[4], float high[4])
			{
				auto low_low = 0.0f, low_high = 0.0f, high_high = 0.0f;
				float low_sum[4]{}, high_sum[4]{};
				for (auto t = 0; t < 16; t++)
				{
					const auto weight = bc7_weights4[indices[t]] / 64.0f;
					low_low += (1.0f - weight) * (1.0f - weight);
					low_high += (1.0f - weight) * weight;
					high_high += weight * weight;
					for (auto c = 0; c < 4; c++)
					{
						low_sum[c] += (1.0f - weight) * texels[t][c];
						high_sum[c] += weight * texels[t][c];
					}
				}

				const auto determinant = low_low * high_high - low_high * low_high;
				if (std::fabs(determinant) < 1e-6f)
				{
					return false;
				}

				for (auto c = 0; c < 4; c++)
				{
					low[c] = std::clamp((high_high * low_sum[c] - low_high * high_sum[c]) / determinant, 0.0f, 255.0f);
					high[c] = std::clamp((low_low * high_sum[c] - low_high * low_sum[c]) / determinant, 0.0f, 255.0f);
				}

				return true;
			}

			void bc7_encode_block(const float texels[16][4], std::uint8_t* out)
			{
				float mean[4]{};
				for (auto t = 0; t < 16; t++)
				{
					for (auto c = 0; c < 4; c++)
					{
						mean[c] += texels[t][c] / 16.0f;
					}
				}

				float axis[4];
				principal_axis(texels, mean, axis);

				auto nearest = FLT_MAX, farthest = -FLT_MAX;
				for (auto t = 0; t < 16; t++)
				{
					const auto distance = project(texels[t], mean, axis);
					nearest = std::min(nearest, distance);
					farthest = std::max(farthest, distance);
				}

				float low[4], high[4];
				for (auto c = 0; c < 4; c++)
				{
					low[c] = std::clamp(mean[c] + nearest * axis[c], 0.0f, 255.0f);
					high[c] = std::clamp(mean[c] + farthest * axis[c], 0.0f, 255.0f);
				}

				bc7_mode6_fit best;
				for (auto pass = 0; pass < 3; pass++)
				{
					const auto fit = bc7_fit_mode6(texels, low, high);
					if (fit.error < best.error)
					{
						best = fit;
					}

					if (!bc7_refit_endpoints(texels, fit.indices, low, high))
					{
						break;
					}
				}

				if (best.indices[0] > 7)
				{
					std::swap(best.low, best.high);
					std::swap(best.low_p, best.high_p);
					for (auto t = 0; t < 16; t++)
					{
						best.indices[t] = 15 - best.indices[t];
					}
				}

				std::memset(out, 0, 16);
				bit_writer bits{out};
				bits.put(1 << 6, 7);
				for (auto c = 0; c < 4; c++)
				{
					bits.put(best.low[c], 7);
					bits.put(best.high[c], 7);
				}

				bits.put(best.low_p, 1);
				bits.put(best.high_p, 1);
				bits.put(best.indices[0], 3);
				for (auto t = 1; t < 16; t++)
				{
					bits.put(best.indices[t], 4);
				}
			}

			void bc4_encode_block(const std::uint8_t values[16], std::uint8_t* out)
			{
				const auto low = *std::min_element(values, values + 16);
				const auto high = *std::max_element(values, values + 16);
				out[0] = high;
				out[1] = low;

				std::uint64_t bits = 0;
				for (auto t = 0; t < 16 && high > low; t++)
				{
					auto best = 0;
					auto best_error = FLT_MAX;
					for (auto i = 0; i < 8; i++)
					{
						const auto value = i == 0 ? high : i == 1 ? low : ((8 - i) * high + (i - 1) * low) / 7.0f;
						const auto error = std::fabs(value - values[t]);
						if (error < best_error)
						{
							best_error = error;
							best = i;
						}
					}

					bits |= static_cast<std::uint64_t>(best) << (3 * t);
				}

				std::memcpy(out + 2, &bits, 6);
			}

			const std::uint8_t* texel_at(const std::vector<std::uint8_t>& rgba, const unsigned int width, const unsigned int height,
				const unsigned int x, const unsigned int y)
			{
				return &rgba[(static_cast<std::size_t>(std::min(y, height - 1)) * width + std::min(x, width - 1)) * 4];
			}

			std::vector<std::uint8_t> bc7_encode_level(const std::vector<std::uint8_t>& rgba, const unsigned int width, const unsigned int height)
			{
				const auto blocks_wide = (width + 3) / 4;
				const auto blocks_high = (height + 3) / 4;
				std::vector<std::uint8_t> out(static_cast<std::size_t>(blocks_wide) * blocks_high * 16);

				for (auto y = 0u; y < blocks_high; y++)
				{
					for (auto x = 0u; x < blocks_wide; x++)
					{
						float texels[16][4];
						for (auto t = 0u; t < 16; t++)
						{
							const auto texel = texel_at(rgba, width, height, x * 4 + (t % 4), y * 4 + (t / 4));
							std::copy_n(texel, 4, texels[t]);
						}

						bc7_encode_block(texels, &out[(static_cast<std::size_t>(y) * blocks_wide + x) * 16]);
					}
				}

				return out;
			}

			std::vector<std::uint8_t> bc4_encode_level(const std::vector<std::uint8_t>& rgba, const unsigned int width, const unsigned int height,
				const int channel)
			{
				const auto blocks_wide = (width + 3) / 4;
				const auto blocks_high = (height + 3) / 4;
				std::vector<std::uint8_t> out(static_cast<std::size_t>(blocks_wide) * blocks_high * 8);

				for (auto y = 0u; y < blocks_high; y++)
				{
					for (auto x = 0u; x < blocks_wide; x++)
					{
						std::uint8_t values[16];
						for (auto t = 0u; t < 16; t++)
						{
							values[t] = texel_at(rgba, width, height, x * 4 + (t % 4), y * 4 + (t / 4))[channel];
						}

						bc4_encode_block(values, &out[(static_cast<std::size_t>(y) * blocks_wide + x) * 8]);
					}
				}

				return out;
			}

			unsigned int level_size(const unsigned int size, const std::size_t level)
			{
				return std::max(1u, size >> level);
			}

			const std::uint8_t* sample_nearest(const decoded_image& image, const std::size_t level, const unsigned int x, const unsigned int y,
				const unsigned int width, const unsigned int height)
			{
				const auto source_level = std::min(level, image.mips.size() - 1);
				const auto source_width = level_size(image.width, source_level);
				const auto source_height = level_size(image.height, source_level);
				return texel_at(image.mips[source_level], source_width, source_height, x * source_width / width, y * source_height / height);
			}

			std::uint8_t encode_unit(const float value)
			{
				return static_cast<std::uint8_t>(std::clamp(std::lround((value * 0.5f + 0.5f) * 255.0f), 0L, 255L));
			}

			target::GfxImage* make_packed_image(const char* name, const target::TextureSemantic semantic, const DXGI_FORMAT format,
				const decoded_image& source, const std::vector<std::vector<std::uint8_t>>& levels, utils::memory::allocator& allocator)
			{
				std::size_t total = 0;
				for (const auto& level : levels)
				{
					total += level.size();
				}

				const auto pixels = allocator.allocate_array<std::uint8_t>(total);
				std::size_t offset = 0;
				for (const auto& level : levels)
				{
					std::memcpy(pixels + offset, level.data(), level.size());
					offset += level.size();
				}

				const auto image = allocator.allocate<target::GfxImage>();
				image->name = allocator.duplicate_string(name);
				image->imageFormat = format;
				image->flags = 0;
				image->mapType = target::MAPTYPE_2D;
				image->semantic = semantic;
				image->category = target::IMG_CATEGORY_LOAD_FROM_FILE;
				image->picmip.platform[0] = 0;
				image->picmip.platform[1] = 2;
				image->dataLen1 = static_cast<unsigned int>(total);
				image->dataLen2 = image->dataLen1;
				image->width = source.width;
				image->height = source.height;
				image->depth = 1;
				image->numElements = 1;
				image->levelCount = static_cast<unsigned char>(levels.size());
				image->streamed = false;
				image->pixelData = pixels;
				return image;
			}

			target::GfxImage* build_image(const char* name, const std::uint8_t semantic_in, const std::uint8_t format,
				const unsigned short width, const unsigned short height, const std::uint8_t* data,
				const std::size_t available, utils::memory::allocator& allocator)
			{
				const auto sizes = mip_sizes(format, width, height);

				std::size_t faces = 1;
				std::vector<std::size_t> present;
				std::size_t total = 0;
				if (!split_levels(sizes, available, present, total))
				{
					if ((available % 6) == 0 && split_levels(sizes, available / 6, present, total))
					{
						faces = 6;
					}
					else
					{
						ZONETOOL_WARNING("Image \"%s\": %ux%u format %u does not account for %zu bytes", name, width, height, format, available);
						return nullptr;
					}
				}

				const auto semantic = convert_semantic(semantic_in);
				const auto* blob = data + (available - total * faces);

				DXGI_FORMAT dxgi;
				std::size_t output_size = total;
				auto transcode_normal = false;
				auto transcode_specular = false;

				if (format == format_dxt5 && semantic == target::TS_NORMAL_MAP)
				{
					dxgi = DXGI_FORMAT_BC7_UNORM;
					transcode_normal = true;
				}
				else if (format == format_dxt1 && semantic == target::TS_SPECULAR_MAP)
				{
					dxgi = DXGI_FORMAT_BC3_UNORM;
					transcode_specular = true;
					output_size = total * 2;
				}
				else
				{
					switch (format)
					{
					case format_dxt1: dxgi = DXGI_FORMAT_BC1_UNORM; break;
					case format_dxt3: dxgi = DXGI_FORMAT_BC2_UNORM; break;
					case format_dxt5: dxgi = DXGI_FORMAT_BC3_UNORM; break;
					case format_dxn: dxgi = DXGI_FORMAT_BC5_UNORM; break;
					case format_rgba:
					case format_d3d_argb: dxgi = DXGI_FORMAT_R8G8B8A8_UNORM; break;
					case format_luminance_alpha: dxgi = DXGI_FORMAT_R8G8_UNORM; break;
					case format_luminance:
					case format_alpha: dxgi = DXGI_FORMAT_R8_UNORM; break;
					case format_rgb:
						dxgi = DXGI_FORMAT_R8G8B8A8_UNORM;
						output_size = (total / 3) * 4;
						break;
					default:
						ZONETOOL_WARNING("Image \"%s\": no mapping for image format %u", name, format);
						return nullptr;
					}
				}

				const auto pixels = allocator.allocate_array<std::uint8_t>(output_size * faces);

				std::size_t source_offset = 0;
				std::vector<std::size_t> source_offsets(present.size());
				for (auto i = present.size(); i-- > 0;)
				{
					source_offsets[i] = source_offset;
					source_offset += present[i];
				}

				std::size_t destination_offset = 0;
				for (std::size_t face = 0; face < faces; face++)
				{
					for (std::size_t i = 0; i < present.size(); i++)
					{
						const auto length = present[i];
						const auto* in = blob + source_offsets[i] * faces + face * length;

						if (transcode_normal)
						{
							dxt5_to_bc7_normal(in, pixels + destination_offset, length / 16);
							destination_offset += length;
						}
						else if (transcode_specular)
						{
							dxt1_to_bc3(in, pixels + destination_offset, length / 8);
							destination_offset += length * 2;
						}
						else if (format == format_d3d_argb)
						{
							for (std::size_t q = 0; q < length / 4; q++)
							{
								pixels[destination_offset + q * 4 + 0] = in[q * 4 + 2];
								pixels[destination_offset + q * 4 + 1] = in[q * 4 + 1];
								pixels[destination_offset + q * 4 + 2] = in[q * 4 + 0];
								pixels[destination_offset + q * 4 + 3] = in[q * 4 + 3];
							}
							destination_offset += length;
						}
						else if (format == format_rgb)
						{
							for (std::size_t q = 0; q < length / 3; q++)
							{
								pixels[destination_offset + q * 4 + 0] = in[q * 3 + 0];
								pixels[destination_offset + q * 4 + 1] = in[q * 3 + 1];
								pixels[destination_offset + q * 4 + 2] = in[q * 3 + 2];
								pixels[destination_offset + q * 4 + 3] = 255;
							}
							destination_offset += (length / 3) * 4;
						}
						else
						{
							std::memcpy(pixels + destination_offset, in, length);
							destination_offset += length;
						}
					}
				}

				const auto image = allocator.allocate<target::GfxImage>();
				image->name = allocator.duplicate_string(name);
				image->imageFormat = dxgi;
				image->flags = 0;
				image->semantic = static_cast<target::TextureSemantic>(semantic);
				image->category = target::IMG_CATEGORY_LOAD_FROM_FILE;
				image->picmip.platform[0] = 0;
				image->picmip.platform[1] = 2;
				image->dataLen1 = static_cast<unsigned int>(destination_offset);
				image->dataLen2 = image->dataLen1;
				image->width = width;
				image->height = height;
				image->depth = 1;
				image->numElements = 1;
				image->mapType = faces == 6 ? target::MAPTYPE_CUBE : target::MAPTYPE_2D;
				image->levelCount = static_cast<unsigned char>(present.size());
				image->streamed = false;
				image->pixelData = pixels;

				return image;
			}
		}

		std::uint8_t convert_semantic(const std::uint8_t semantic)
		{
			constexpr std::uint8_t table[13] =
			{
				target::TS_2D,
				target::TS_FUNCTION,
				target::TS_COLOR_MAP,
				target::TS_DETAIL_MAP,
				target::TS_UNUSED_4,
				target::TS_NORMAL_MAP,
				target::TS_UNUSED_6,
				target::TS_UNUSED_7,
				target::TS_SPECULAR_MAP,
				target::TS_UNUSED_4,
				target::TS_UNUSED_4,
				target::TS_UNUSED_4,
				target::TS_DISPLACEMENT_MAP,
			};

			return semantic < std::size(table) ? table[semantic] : static_cast<std::uint8_t>(target::TS_2D);
		}

		namespace
		{
			unsigned char count_mipmaps(const unsigned int width, const unsigned int height, const unsigned int depth)
			{
				unsigned char count = 1;
				for (unsigned int size = 1; size < width || size < height || size < depth; size *= 2)
				{
					count++;
				}

				return count;
			}

			target::GfxImage* convert_file(zonetool::iw5::GfxImage* asset, utils::memory::allocator& allocator)
			{
				if (!read_file)
				{
					return nullptr;
				}

				const std::string name = asset->name.get();
				const auto file = read_file("images/" + name + ".iwi");
				::iwi::iwx::GfxImageFileHeader header{};
				if (!read_iwi_header(name, file, header))
				{
					ZONETOOL_WARNING("Image \"%s\": no usable .iwi", name.data());
					return nullptr;
				}

				const auto* bytes = reinterpret_cast<const std::uint8_t*>(file.data());
				const auto width = static_cast<unsigned short>(header.dimensions[0]);
				const auto height = static_cast<unsigned short>(header.dimensions[1]);
				if (header.format == ::iwi::IMG_FORMAT_WAVELET_RGBA || header.format == ::iwi::IMG_FORMAT_WAVELET_RGB)
				{
					const auto bitmap_format = header.format == ::iwi::IMG_FORMAT_WAVELET_RGBA ? format_rgba : format_rgb;
					const auto has_mipmaps = (header.flags & ::iwi::iwx::IMG_FLAG_NOMIPMAPS) == 0;
					const auto pixels = ::iwi::wavelet::decode_image(bytes + header.size, file.size() - header.size, width, height,
						pixel_bytes(bitmap_format), has_mipmaps ? count_mipmaps(width, height, 1) : 1);
					if (pixels.empty())
					{
						ZONETOOL_WARNING("Image \"%s\": wavelet data does not decode", name.data());
						return nullptr;
					}

					return build_image(name.data(), asset->semantic, bitmap_format, width, height, pixels.data(),
						static_cast<std::size_t>(width) * height * pixel_bytes(bitmap_format), allocator);
				}

				return build_image(name.data(), asset->semantic, header.format, width, height,
					bytes + header.size, file.size() - header.size, allocator);
			}

			bool is_map_image(const std::string& name)
			{
				return name.starts_with("*light") || name.starts_with("*refle") || name == "$outdoor";
			}

			target::GfxImage* convert_map_image(zonetool::iw5::GfxImage* asset, const std::uint8_t format, utils::memory::allocator& allocator)
			{
				const auto load_def = asset->texture.loadDef.get();
				const auto size = static_cast<unsigned int>(load_def->resourceSize);

				const auto image = allocator.allocate<target::GfxImage>();
				image->name = allocator.duplicate_string(asset->name.get());
				image->imageFormat = format == format_d3d_argb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8_UNORM;
				image->mapType = static_cast<target::MapType>(asset->mapType);
				image->semantic = static_cast<target::TextureSemantic>(convert_semantic(asset->semantic));
				image->category = static_cast<target::GfxImageCategory>(asset->category);
				image->flags = asset->flags;
				image->dataLen1 = size;
				image->dataLen2 = size;
				image->width = asset->width;
				image->height = asset->height;
				image->depth = asset->depth;
				image->numElements = 1;
				image->levelCount = (load_def->flags & 2) ? 1 : count_mipmaps(asset->width, asset->height, asset->depth);
				image->streamed = false;

				const auto pixels = allocator.allocate_array<std::uint8_t>(size);
				std::memcpy(pixels, load_def->data, size);
				if (format == format_d3d_argb)
				{
					for (unsigned int i = 0; i + 3 < size; i += 4)
					{
						std::swap(pixels[i], pixels[i + 2]);
					}
				}

				image->pixelData = pixels;
				return image;
			}

			float srgb_to_linear(const std::uint8_t value)
			{
				const auto c = static_cast<float>(value) / 255.0f;
				return c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
			}

			std::uint16_t float_to_half(float value)
			{
				if (!(value > 6.104e-5f))
				{
					return 0;
				}

				value = std::min(value, 65504.0f);

				std::uint32_t bits = 0;
				std::memcpy(&bits, &value, sizeof(bits));

				const auto exponent = ((bits >> 23) & 0xFF) - 127 + 15;
				const auto mantissa = (bits >> 13) & 0x3FF;
				return static_cast<std::uint16_t>((exponent << 10) | mantissa);
			}

			int bc6h_half_to_interpolated(const std::uint16_t half)
			{
				return (static_cast<int>(half) << 6) / 31;
			}

			int bc6h_interpolated_to_half(const int value)
			{
				return (std::clamp(value, 0, 0xFFFF) * 31) >> 6;
			}

			int bc6h_unquantize10(const int code)
			{
				if (code <= 0)
				{
					return 0;
				}

				return code >= 1023 ? 0xFFFF : (code << 6) + 32;
			}

			int bc6h_quantize10(const int value)
			{
				if (value <= 0)
				{
					return 0;
				}

				return value >= 0xFFFF ? 1023 : std::clamp(value >> 6, 0, 1023);
			}

			void put_bits(std::uint8_t* out, int& position, const unsigned int value, const int count)
			{
				for (auto i = 0; i < count; i++, position++)
				{
					if ((value >> i) & 1)
					{
						out[position >> 3] |= static_cast<std::uint8_t>(1 << (position & 7));
					}
				}
			}

			// BC6H mode 10: one region, 10-bit endpoints, 4-bit indices
			void bc6h_encode_block(const std::uint16_t texels[16][3], std::uint8_t* out)
			{
				static const int weights[16] = {0, 4, 9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64};

				int endpoint_a[3]{};
				int endpoint_b[3]{};
				int low[3]{};
				int high[3]{};
				for (auto c = 0; c < 3; c++)
				{
					auto lowest = 0xFFFF;
					auto highest = 0;
					for (auto t = 0; t < 16; t++)
					{
						const auto value = bc6h_half_to_interpolated(texels[t][c]);
						lowest = std::min(lowest, value);
						highest = std::max(highest, value);
					}

					endpoint_a[c] = bc6h_quantize10(lowest);
					endpoint_b[c] = bc6h_quantize10(highest);
					low[c] = bc6h_unquantize10(endpoint_a[c]);
					high[c] = bc6h_unquantize10(endpoint_b[c]);
				}

				int indices[16]{};
				for (auto t = 0; t < 16; t++)
				{
					auto best_error = std::numeric_limits<long long>::max();
					for (auto i = 0; i < 16; i++)
					{
						long long error = 0;
						for (auto c = 0; c < 3; c++)
						{
							const auto interpolated = (low[c] * (64 - weights[i]) + high[c] * weights[i] + 32) >> 6;
							const long long delta = bc6h_interpolated_to_half(interpolated) - static_cast<int>(texels[t][c]);
							error += delta * delta;
						}

						if (error < best_error)
						{
							best_error = error;
							indices[t] = i;
						}
					}
				}

				if (indices[0] >= 8) // first index has 3 bits
				{
					for (auto c = 0; c < 3; c++)
					{
						std::swap(endpoint_a[c], endpoint_b[c]);
					}

					for (auto& index : indices)
					{
						index = 15 - index;
					}
				}

				std::memset(out, 0, 16);
				auto position = 0;
				put_bits(out, position, 3, 5);
				for (const auto value : endpoint_a)
				{
					put_bits(out, position, value, 10);
				}

				for (const auto value : endpoint_b)
				{
					put_bits(out, position, value, 10);
				}

				put_bits(out, position, indices[0], 3);
				for (auto t = 1; t < 16; t++)
				{
					put_bits(out, position, indices[t], 4);
				}
			}

			unsigned int bc6h_surface_bytes(const unsigned int width)
			{
				const auto blocks = (width + 3) / 4;
				return blocks * blocks * 16;
			}

			void bc6h_encode_surface(const std::uint8_t* rgba, const unsigned int width, std::uint8_t* out)
			{
				const auto blocks = (width + 3) / 4;
				for (auto block_y = 0u; block_y < blocks; block_y++)
				{
					for (auto block_x = 0u; block_x < blocks; block_x++)
					{
						std::uint16_t texels[16][3]{};
						for (auto t = 0u; t < 16; t++)
						{
							const auto x = std::min(block_x * 4 + (t % 4), width - 1);
							const auto y = std::min(block_y * 4 + (t / 4), width - 1);
							const auto* pixel = rgba + (static_cast<std::size_t>(y) * width + x) * 4;
							for (auto c = 0; c < 3; c++)
							{
								texels[t][c] = float_to_half(srgb_to_linear(pixel[c]));
							}
						}

						bc6h_encode_block(texels, out);
						out += 16;
					}
				}
			}

			void upsample_2x(const std::uint8_t* source, const unsigned int width, std::uint8_t* destination)
			{
				const auto destination_width = width * 2;
				const auto last = static_cast<int>(width) - 1;

				for (auto y = 0u; y < destination_width; y++)
				{
					const auto source_y = (static_cast<float>(y) + 0.5f) * 0.5f - 0.5f;
					const auto fraction_y = source_y - std::floor(source_y);
					const auto y0 = std::clamp(static_cast<int>(std::floor(source_y)), 0, last);
					const auto y1 = std::clamp(y0 + 1, 0, last);

					for (auto x = 0u; x < destination_width; x++)
					{
						const auto source_x = (static_cast<float>(x) + 0.5f) * 0.5f - 0.5f;
						const auto fraction_x = source_x - std::floor(source_x);
						const auto x0 = std::clamp(static_cast<int>(std::floor(source_x)), 0, last);
						const auto x1 = std::clamp(x0 + 1, 0, last);

						for (auto c = 0; c < 4; c++)
						{
							const auto value =
								source[(static_cast<std::size_t>(y0) * width + x0) * 4 + c] * (1.0f - fraction_x) * (1.0f - fraction_y) +
								source[(static_cast<std::size_t>(y0) * width + x1) * 4 + c] * fraction_x * (1.0f - fraction_y) +
								source[(static_cast<std::size_t>(y1) * width + x0) * 4 + c] * (1.0f - fraction_x) * fraction_y +
								source[(static_cast<std::size_t>(y1) * width + x1) * 4 + c] * fraction_x * fraction_y;
							destination[(static_cast<std::size_t>(y) * destination_width + x) * 4 + c] =
								static_cast<std::uint8_t>(std::clamp(value + 0.5f, 0.0f, 255.0f));
						}
					}
				}
			}
		}

		target::GfxImage* generate_lightmap_second_unorm(const char* name, const unsigned short width, const unsigned short height,
			utils::memory::allocator& allocator)
		{
			const auto image = allocator.allocate<target::GfxImage>();
			image->name = allocator.duplicate_string(name);
			image->imageFormat = DXGI_FORMAT_R8G8_UNORM;
			image->flags = 0x3B;
			image->mapType = target::MAPTYPE_2D;
			image->semantic = target::TS_FUNCTION;
			image->category = target::IMG_CATEGORY_LIGHTMAP;
			image->width = width;
			image->height = height;
			image->depth = 1;
			image->numElements = 1;
			image->levelCount = 1;
			image->streamed = false;
			image->dataLen1 = static_cast<unsigned int>(width) * height * 2;
			image->dataLen2 = image->dataLen1;

			const auto pixels = allocator.allocate_array<std::uint8_t>(image->dataLen1);
			std::memset(pixels, 0x80, image->dataLen1);
			image->pixelData = pixels;

			return image;
		}

		target::GfxImage* generate_ies_lookup(utils::memory::allocator& allocator)
		{
			const auto image = allocator.allocate<target::GfxImage>();
			image->name = allocator.duplicate_string("*ieslookup");
			image->imageFormat = DXGI_FORMAT_R8_UNORM;
			image->flags = 0x18003;
			image->mapType = target::MAPTYPE_1D;
			image->semantic = target::TS_FUNCTION;
			image->category = target::IMG_CATEGORY_AUTO_GENERATED;
			image->width = 1;
			image->height = 1;
			image->depth = 1;
			image->numElements = 1;
			image->levelCount = 1;
			image->streamed = false;
			image->dataLen1 = 4;
			image->dataLen2 = 4;

			const auto pixels = allocator.allocate_array<std::uint8_t>(4);
			pixels[0] = 255;
			image->pixelData = pixels;

			return image;
		}

		target::GfxImage* generate_reflection_probe_array(const std::vector<target::GfxImage*>& probes, utils::memory::allocator& allocator)
		{
			unsigned int source_width = 0;
			unsigned int source_levels = 0;

			std::vector<const std::uint8_t*> usable(probes.size(), nullptr);
			for (std::size_t i = 0; i < probes.size(); i++)
			{
				const auto* probe = probes[i];
				if (!probe || !probe->pixelData || probe->imageFormat != DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
					|| probe->width != probe->height || !probe->width || !probe->levelCount)
				{
					ZONETOOL_WARNING("Reflection probe %zu is unusable, using a black one", i);
					continue;
				}

				if (!source_width)
				{
					source_width = probe->width;
					source_levels = probe->levelCount;
				}
				else if (probe->width != source_width || probe->levelCount != source_levels)
				{
					ZONETOOL_WARNING("Reflection probe %zu has a different size, using a black one", i);
					continue;
				}

				usable[i] = probe->pixelData;
			}

			if (!source_width)
			{
				ZONETOOL_WARNING("No usable reflection probe, *reflection_probe_array not built");
				return nullptr;
			}

			unsigned int source_face_bytes = 0;
			unsigned int face_bytes = bc6h_surface_bytes(source_width * 2);
			for (auto level = 0u; level < source_levels; level++)
			{
				const auto width = std::max(1u, source_width >> level);
				source_face_bytes += width * width * 4;
				face_bytes += bc6h_surface_bytes(width);
			}

			const auto probe_count = static_cast<unsigned int>(probes.size());
			const auto total = face_bytes * 6 * probe_count;
			const auto pixels = allocator.allocate_array<std::uint8_t>(total);
			auto* out = pixels;

			std::vector<std::uint8_t> upsampled(static_cast<std::size_t>(source_width) * source_width * 16);
			std::vector<std::uint8_t> black(source_face_bytes, 0);

			for (auto probe = 0u; probe < probe_count; probe++)
			{
				for (auto face = 0u; face < 6; face++)
				{
					const auto* source_face = usable[probe]
						? usable[probe] + static_cast<std::size_t>(face) * source_face_bytes
						: black.data();

					upsample_2x(source_face, source_width, upsampled.data());
					bc6h_encode_surface(upsampled.data(), source_width * 2, out);
					out += bc6h_surface_bytes(source_width * 2);

					const auto* level_pixels = source_face;
					for (auto level = 0u; level < source_levels; level++)
					{
						const auto width = std::max(1u, source_width >> level);
						bc6h_encode_surface(level_pixels, width, out);
						out += bc6h_surface_bytes(width);
						level_pixels += static_cast<std::size_t>(width) * width * 4;
					}
				}
			}

			const auto image = allocator.allocate<target::GfxImage>();
			image->name = allocator.duplicate_string("*reflection_probe_array");
			image->imageFormat = DXGI_FORMAT_BC6H_UF16;
			image->flags = 0x28301;
			image->mapType = target::MAPTYPE_CUBE_ARRAY;
			image->semantic = target::TS_FUNCTION;
			image->category = target::IMG_CATEGORY_AUTO_GENERATED;
			image->width = static_cast<unsigned short>(source_width * 2);
			image->height = image->width;
			image->depth = 1;
			image->numElements = static_cast<unsigned short>(probe_count);
			image->levelCount = static_cast<unsigned char>(source_levels + 1);
			image->streamed = false;
			image->dataLen1 = total;
			image->dataLen2 = total;
			image->pixelData = pixels;

			return image;
		}

		bool decode(zonetool::iw5::GfxImage* asset, decoded_image& out)
		{
			std::string file;
			const std::uint8_t* data = nullptr;
			std::size_t available = 0;
			std::uint8_t format = format_none;

			const auto load_def = asset->texture.loadDef.get();
			if (load_def && load_def->resourceSize > 0)
			{
				format = normalize_format(load_def->format);
				out.width = asset->width;
				out.height = asset->height;
				data = reinterpret_cast<const std::uint8_t*>(load_def->data);
				available = static_cast<std::size_t>(load_def->resourceSize);
			}
			else if (read_file)
			{
				file = read_file("images/"s + asset->name.get() + ".iwi");
				::iwi::iwx::GfxImageFileHeader header{};
				if (!read_iwi_header(asset->name.get(), file, header))
				{
					return false;
				}

				format = header.format;
				out.width = header.dimensions[0];
				out.height = header.dimensions[1];
				data = reinterpret_cast<const std::uint8_t*>(file.data()) + header.size;
				available = file.size() - header.size;
			}

			if (!format || !out.width || !out.height)
			{
				return false;
			}

			std::vector<std::size_t> present;
			std::size_t total = 0;
			if (!split_levels(mip_sizes(format, out.width, out.height), available, present, total))
			{
				return false;
			}

			out.has_alpha = has_alpha_channel(format);
			out.mips.clear();

			const auto* level_end = data + available;
			for (std::size_t level = 0; level < present.size(); level++)
			{
				level_end -= present[level];

				std::vector<std::uint8_t> rgba;
				if (!decode_level(format, level_end, level_size(out.width, level), level_size(out.height, level), rgba))
				{
					return false;
				}

				out.mips.push_back(std::move(rgba));
			}

			return true;
		}

		target::GfxImage* build_packed_cs(const char* name, const decoded_image& colour, utils::memory::allocator& allocator)
		{
			std::vector<std::vector<std::uint8_t>> levels;
			for (std::size_t level = 0; level < colour.mips.size(); level++)
			{
				auto rgba = colour.mips[level];
				for (std::size_t i = 3; i < rgba.size(); i += 4)
				{
					rgba[i] = 10; // dielectric reflectance
				}

				levels.push_back(bc7_encode_level(rgba, level_size(colour.width, level), level_size(colour.height, level)));
			}

			return make_packed_image(name, target::TS_COLOR_SPECULAR_MAP, DXGI_FORMAT_BC7_UNORM, colour, levels, allocator);
		}

		target::GfxImage* build_packed_ng(const char* name, const decoded_image& normal, const decoded_image* spec,
			utils::memory::allocator& allocator)
		{
			const auto gloss = spec && spec->has_alpha ? spec : nullptr; // reference reads gloss 255 from a spec without alpha

			std::vector<std::vector<std::uint8_t>> levels;
			for (std::size_t level = 0; level < normal.mips.size(); level++)
			{
				const auto width = level_size(normal.width, level);
				const auto height = level_size(normal.height, level);

				std::vector<std::uint8_t> rgba(normal.mips[level].size());
				for (auto y = 0u; y < height; y++)
				{
					for (auto x = 0u; x < width; x++)
					{
						const auto offset = (static_cast<std::size_t>(y) * width + x) * 4;
						const auto source = &normal.mips[level][offset];

						const auto normal_x = source[3] / 127.5f - 1.0f;
						const auto normal_y = source[1] / 127.5f - 1.0f;
						const auto normal_z = std::sqrt(std::max(0.0f, 1.0f - normal_x * normal_x - normal_y * normal_y));

						const auto length = std::fabs(normal_x) + std::fabs(normal_y) + normal_z;
						const auto octahedral_x = length > 1e-6f ? normal_x / length : 0.0f;
						const auto octahedral_y = length > 1e-6f ? normal_y / length : 0.0f;

						rgba[offset + 0] = gloss ? sample_nearest(*gloss, level, x, y, width, height)[3] : default_gloss;
						rgba[offset + 1] = encode_unit(octahedral_x + octahedral_y);
						rgba[offset + 2] = 255; // no occlusion
						rgba[offset + 3] = encode_unit(octahedral_x - octahedral_y);
					}
				}

				levels.push_back(bc7_encode_level(rgba, width, height));
			}

			return make_packed_image(name, target::TS_NORMAL_OCCLUSSION_GLOSS_MAP, DXGI_FORMAT_BC7_UNORM, normal, levels, allocator);
		}

		target::GfxImage* build_packed_a(const char* name, const decoded_image& colour, utils::memory::allocator& allocator)
		{
			std::vector<std::vector<std::uint8_t>> levels;
			for (std::size_t level = 0; level < colour.mips.size(); level++)
			{
				levels.push_back(bc4_encode_level(colour.mips[level], level_size(colour.width, level), level_size(colour.height, level), 3));
			}

			return make_packed_image(name, target::TS_ALPHA_REVEAL_THICKNESS_MAP, DXGI_FORMAT_BC4_UNORM, colour, levels, allocator);
		}

		target::GfxImage* convert(zonetool::iw5::GfxImage* asset, utils::memory::allocator& allocator)
		{
			const auto load_def = asset->texture.loadDef.get();
			if (!load_def || load_def->resourceSize <= 0)
			{
				return convert_file(asset, allocator);
			}

			const auto format = normalize_format(load_def->format);
			if (is_map_image(asset->name.get()) && (format == format_d3d_argb || format == format_luminance))
			{
				return convert_map_image(asset, format, allocator);
			}

			if (format == format_none)
			{
				ZONETOOL_WARNING("Image \"%s\": unknown resident format %d", asset->name.get(), load_def->format);
				return nullptr;
			}

			return build_image(asset->name.get(), asset->semantic, format, asset->width, asset->height,
				reinterpret_cast<const std::uint8_t*>(load_def->data), static_cast<std::size_t>(load_def->resourceSize), allocator);
		}

		void dump(zonetool::iw5::GfxImage* asset)
		{
			utils::memory::allocator allocator;
			const auto image = convert(asset, allocator);
			if (image)
			{
				target::gfx_image::dump(image);
			}
		}
	}
}
