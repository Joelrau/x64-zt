#include <std_include.hpp>

#include "../include.hpp"
#include "gfx_image.hpp"

#include <iw7/assets/gfximage.hpp>

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
			target::GfxImage* convert_file(zonetool::iw5::GfxImage* asset, utils::memory::allocator& allocator)
			{
				if (!read_file)
				{
					return nullptr;
				}

				const std::string name = asset->name.get();
				const auto file = read_file("images/" + name + ".iwi");
				if (file.size() < 28 || std::memcmp(file.data(), "IWi", 3) != 0)
				{
					ZONETOOL_WARNING("Image \"%s\": no usable .iwi", name.data());
					return nullptr;
				}

				const auto* bytes = reinterpret_cast<const std::uint8_t*>(file.data());
				unsigned short width = 0;
				unsigned short height = 0;
				std::memcpy(&width, bytes + 6, 2);
				std::memcpy(&height, bytes + 8, 2);

				return build_image(name.data(), asset->semantic, bytes[4], width, height, bytes + 28, file.size() - 28, allocator);
			}
		}

		target::GfxImage* convert(zonetool::iw5::GfxImage* asset, utils::memory::allocator& allocator)
		{
			const auto load_def = asset->texture.loadDef.get();
			if (!load_def || load_def->resourceSize <= 0)
			{
				return convert_file(asset, allocator);
			}

			const auto format = normalize_format(load_def->format);
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
