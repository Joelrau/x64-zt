#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

namespace zonetool::iw7::converter::iw8::gfximage
{
	namespace
	{
		namespace target = zonetool::iw8;

		using channel_tables = std::array<std::array<std::uint8_t, 256>, 4>;

		std::optional<DirectX::ScratchImage> load_pixels(const DXGI_FORMAT format, const std::size_t width, const std::size_t height,
			const std::size_t level_count, const std::size_t cube_count, const std::string_view pixels)
		{
			DirectX::ScratchImage image;
			const auto result = cube_count
				? image.InitializeCube(DirectX::MakeLinear(format), width, height, cube_count, level_count)
				: image.Initialize2D(DirectX::MakeLinear(format), width, height, 1, level_count);
			if (FAILED(result) || image.GetPixelsSize() != pixels.size())
			{
				return {};
			}

			std::memcpy(image.GetPixels(), pixels.data(), pixels.size());
			return image;
		}

		std::size_t count_levels(const DXGI_FORMAT format, std::size_t width, std::size_t height, const std::size_t size)
		{
			std::size_t total = 0;
			for (std::size_t level = 1;; level++)
			{
				std::size_t row_pitch, slice_pitch;
				if (FAILED(DirectX::ComputePitch(format, width, height, row_pitch, slice_pitch)))
				{
					return 0;
				}

				total += slice_pitch;
				if (total >= size)
				{
					return total == size ? level : 0;
				}

				if (width == 1 && height == 1)
				{
					return 0;
				}

				width = std::max<std::size_t>(1, width / 2);
				height = std::max<std::size_t>(1, height / 2);
			}
		}

		std::optional<DirectX::ScratchImage> load_source(const GfxImage* asset)
		{
			const auto cube_array = asset->mapType == MAPTYPE_CUBE_ARRAY;
			const auto cube = asset->mapType == MAPTYPE_CUBE || cube_array;
			if ((asset->mapType != MAPTYPE_2D && !cube) || asset->depth > 1 || (asset->numElements > 1 && !cube_array))
			{
				return {};
			}

			const auto cube_count = cube_array ? asset->numElements : cube ? 1u : 0u;
			const auto face_count = cube ? 6u * cube_count : 1u;
			const auto format = static_cast<DXGI_FORMAT>(asset->imageFormat);
			if (!asset->streamed)
			{
				if (!asset->pixelData)
				{
					return {};
				}

				return load_pixels(format, asset->width, asset->height, asset->levelCount, cube_count,
					{reinterpret_cast<const char*>(asset->pixelData), asset->dataLen1});
			}

			std::array<unsigned int, 4> streams{0, 1, 2, 3};
			std::ranges::sort(streams, std::greater{}, [&](const unsigned int i) { return asset->streams[i].width; });
			for (const auto stream : streams)
			{
				const auto& data = asset->streams[stream];
				const auto pixels = read_streamed_image_pixels(asset, stream);
				const auto levels = pixels && pixels->size() % face_count == 0
					? count_levels(format, data.width, data.height, pixels->size() / face_count)
					: 0;
				if (levels)
				{
					return load_pixels(format, data.width, data.height, levels, cube_count, *pixels);
				}
			}

			return {};
		}

		std::optional<DirectX::ScratchImage> decode(DirectX::ScratchImage source, const DXGI_FORMAT target_format)
		{
			const auto format = source.GetMetadata().format;
			if (format == target_format)
			{
				return source;
			}

			DirectX::ScratchImage decoded;
			const auto result = DirectX::IsCompressed(format)
				? DirectX::Decompress(source.GetImages(), source.GetImageCount(), source.GetMetadata(), target_format, decoded)
				: DirectX::Convert(source.GetImages(), source.GetImageCount(), source.GetMetadata(), target_format,
					DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, decoded);
			if (FAILED(result))
			{
				return {};
			}

			return decoded;
		}

		std::optional<DirectX::ScratchImage> compress(const DirectX::ScratchImage& source, const DXGI_FORMAT format, const DirectX::TEX_COMPRESS_FLAGS flags)
		{
			auto metadata = source.GetMetadata();
			metadata.format = format;

			DirectX::ScratchImage compressed;
			if (FAILED(compressed.Initialize(metadata)))
			{
				return {};
			}

			std::vector<std::pair<std::size_t, std::size_t>> strips;
			for (auto index = 0u; index < source.GetImageCount(); index++)
			{
				for (auto row = 0u; row < source.GetImages()[index].height; row += 64)
				{
					strips.emplace_back(index, row);
				}
			}

			std::atomic_size_t next_strip{};
			std::atomic_bool failed{};
			const auto compress_strips = [&]
			{
				for (auto strip = next_strip++; strip < strips.size(); strip = next_strip++)
				{
					const auto [index, row] = strips[strip];
					const auto& image = source.GetImages()[index];
					auto part = image;
					part.height = std::min<std::size_t>(64, image.height - row);
					part.slicePitch = image.rowPitch * part.height;
					part.pixels = image.pixels + image.rowPitch * row;

					DirectX::ScratchImage result;
					if (FAILED(DirectX::Compress(part, format, flags, DirectX::TEX_THRESHOLD_DEFAULT, result)))
					{
						failed = true;
						continue;
					}

					const auto& target_image = compressed.GetImages()[index];
					std::memcpy(target_image.pixels + target_image.rowPitch * (row / 4), result.GetPixels(), result.GetPixelsSize());
				}
			};

			std::vector<std::jthread> threads(std::max(1u, std::thread::hardware_concurrency()));
			for (auto& thread : threads)
			{
				thread = std::jthread(compress_strips);
			}

			threads.clear();
			if (failed)
			{
				return {};
			}

			return compressed;
		}

		std::uint8_t to_byte(const float value)
		{
			return static_cast<std::uint8_t>(std::lround(std::clamp(value, 0.0f, 1.0f) * 255.0f));
		}

		std::uint8_t linear_to_srgb(const float linear)
		{
			return to_byte(linear <= 0.0031308f ? linear * 12.92f : 1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f);
		}

		std::uint8_t from_squared_gamma(const int value)
		{
			return linear_to_srgb(value / 255.0f * value / 255.0f);
		}

		std::optional<DirectX::ScratchImage> read_pixels(const GfxImage* asset, const DXGI_FORMAT format)
		{
			const auto loaded = static_cast<const GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, asset->name));
			auto source = load_source(loaded ? loaded : asset);
			auto decoded = source ? decode(std::move(*source), format) : std::nullopt;
			if (!decoded)
			{
				ZONETOOL_WARNING("Cannot read the pixels of image \"%s\"", asset->name);
			}

			return decoded;
		}

		target::GfxImage* make_image(const std::string& name, const DirectX::ScratchImage& compressed, const target::GfxPixelFormat format,
			const std::uint32_t flags, const target::TextureSemantic semantic, const unsigned int scale_bias)
		{
			static utils::memory::allocator allocator;

			const auto& metadata = compressed.GetMetadata();
			const auto result = allocator.allocate<target::GfxImage>();
			result->name = allocator.duplicate_string(name);
			result->format = format;
			result->flags = static_cast<target::GfxImageFlags>(flags);
			result->totalSize = static_cast<unsigned int>(compressed.GetPixelsSize());
			result->semanticSpecific.albedoMapScaleBias = scale_bias;
			result->width = static_cast<unsigned short>(metadata.width);
			result->height = static_cast<unsigned short>(metadata.height);
			result->depth = 1;
			result->numElements = static_cast<unsigned short>(metadata.arraySize);
			result->atlasInfo.atlasSize = {.rowCount = 1, .colCount = 1};
			result->semantic = semantic;
			result->category = target::IMG_CATEGORY_LOAD_FROM_FILE;
			result->levelCount = static_cast<unsigned char>(metadata.mipLevels);
			result->pixels.residentData = allocator.allocate_array<char>(result->totalSize);
			std::memcpy(result->pixels.residentData, compressed.GetPixels(), result->totalSize);
			return result;
		}

		target::GfxImage* convert(const GfxImage* asset, const std::string& name, const target::GfxPixelFormat format, const std::uint32_t flags,
			const target::TextureSemantic semantic, const unsigned int scale_bias, const channel_tables& tables,
			const std::array<std::uint8_t, 4>& sources = {0, 1, 2, 3})
		{
			auto decoded = read_pixels(asset, DXGI_FORMAT_R8G8B8A8_UNORM);
			if (!decoded)
			{
				return nullptr;
			}

			const auto& source_metadata = decoded->GetMetadata();
			if (source_metadata.arraySize != 1)
			{
				ZONETOOL_WARNING("Image \"%s\" is not a 2D image", asset->name);
				return nullptr;
			}

			if (source_metadata.width % 4 || source_metadata.height % 4)
			{
				ZONETOOL_WARNING("Image \"%s\" is %zux%zu, BC7 needs a multiple of 4", asset->name, source_metadata.width, source_metadata.height);
				return nullptr;
			}

			for (auto i = 0u; i < decoded->GetImageCount(); i++)
			{
				const auto& image = decoded->GetImages()[i];
				for (auto row = 0u; row < image.height; row++)
				{
					auto* texel = image.pixels + row * image.rowPitch;
					for (auto column = 0u; column < image.width; column++, texel += 4)
					{
						const std::array<std::uint8_t, 4> source{texel[0], texel[1], texel[2], texel[3]};
						for (auto channel = 0u; channel < tables.size(); channel++)
						{
							texel[channel] = tables[channel][source[sources[channel]]];
						}
					}
				}
			}

			const auto compressed = compress(*decoded, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", asset->name);
				return nullptr;
			}

			return make_image(name, *compressed, format, flags, semantic, scale_bias);
		}

		std::array<float, 3> get_octahedron_direction(const float u, const float v)
		{
			auto x = u * 2.0f - 1.0f;
			auto y = v * 2.0f - 1.0f;
			const auto z = 1.0f - std::abs(x) - std::abs(y);
			if (z < 0.0f)
			{
				const auto folded_x = (1.0f - std::abs(y)) * (x >= 0.0f ? 1.0f : -1.0f);
				y = (1.0f - std::abs(x)) * (y >= 0.0f ? 1.0f : -1.0f);
				x = folded_x;
			}

			return {x, y, z};
		}

		std::array<float, 4> sample_cube(const DirectX::ScratchImage& cube, const std::size_t first_face, const std::size_t level,
			const std::array<float, 3>& direction)
		{
			const auto [x, y, z] = direction;
			const auto abs_x = std::abs(x);
			const auto abs_y = std::abs(y);
			const auto abs_z = std::abs(z);

			std::size_t face{};
			float s{}, t{}, major{};
			if (abs_x >= abs_y && abs_x >= abs_z)
			{
				face = x > 0.0f ? 0 : 1;
				s = x > 0.0f ? -z : z;
				t = -y;
				major = abs_x;
			}
			else if (abs_y >= abs_z)
			{
				face = y > 0.0f ? 2 : 3;
				s = x;
				t = y > 0.0f ? z : -z;
				major = abs_y;
			}
			else
			{
				face = z > 0.0f ? 4 : 5;
				s = z > 0.0f ? x : -x;
				t = -y;
				major = abs_z;
			}

			const auto& image = *cube.GetImage(level, first_face + face, 0);
			const auto max_column = static_cast<float>(image.width - 1);
			const auto max_row = static_cast<float>(image.height - 1);
			const auto column = std::clamp((s / major + 1.0f) * 0.5f * image.width - 0.5f, 0.0f, max_column);
			const auto row = std::clamp((t / major + 1.0f) * 0.5f * image.height - 0.5f, 0.0f, max_row);
			const auto column_0 = static_cast<std::size_t>(column);
			const auto row_0 = static_cast<std::size_t>(row);
			const auto column_1 = std::min(column_0 + 1, image.width - 1);
			const auto row_1 = std::min(row_0 + 1, image.height - 1);
			const auto column_weight = column - column_0;
			const auto row_weight = row - row_0;

			const auto texel = [&](const std::size_t texel_column, const std::size_t texel_row)
			{
				return reinterpret_cast<const float*>(image.pixels + texel_row * image.rowPitch) + texel_column * 4;
			};

			std::array<float, 4> result{};
			for (auto channel = 0u; channel < result.size(); channel++)
			{
				const auto top = std::lerp(texel(column_0, row_0)[channel], texel(column_1, row_0)[channel], column_weight);
				const auto bottom = std::lerp(texel(column_0, row_1)[channel], texel(column_1, row_1)[channel], column_weight);
				result[channel] = std::lerp(top, bottom, row_weight);
			}

			return result;
		}

		std::optional<DirectX::ScratchImage> make_octahedron(const DirectX::ScratchImage& cube)
		{
			const auto size = std::min<std::size_t>(2048, cube.GetMetadata().width * 2);

			DirectX::ScratchImage result;
			if (FAILED(result.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, size, size, 1, 1)))
			{
				return {};
			}

			const auto& image = *result.GetImage(0, 0, 0);
			for (auto row = 0u; row < size; row++)
			{
				auto* texel = reinterpret_cast<float*>(image.pixels + row * image.rowPitch);
				for (auto column = 0u; column < size; column++, texel += 4)
				{
					const auto direction = get_octahedron_direction((column + 0.5f) / size, (row + 0.5f) / size);
					const auto color = sample_cube(cube, 0, 0, direction);
					texel[0] = std::max(color[0], 0.0f);
					texel[1] = std::max(color[1], 0.0f);
					texel[2] = std::max(color[2], 0.0f);
					texel[3] = 1.0f;
				}
			}

			return result;
		}

		std::array<float, 2> wrap_octahedron_border(float u, float v)
		{
			if (u < 0.0f || u > 1.0f)
			{
				u = u < 0.0f ? -u : 2.0f - u;
				v = 1.0f - v;
			}

			if (v < 0.0f || v > 1.0f)
			{
				v = v < 0.0f ? -v : 2.0f - v;
				u = 1.0f - u;
			}

			return {u, v};
		}

		float evaluate_sh_l1(const std::array<float, 4>& sh, const std::array<float, 3>& direction)
		{
			const auto [x, y, z] = direction;
			const auto length = std::sqrt(x * x + y * y + z * z);
			return 0.282095f * sh[0] + 0.488603f * (-y * sh[1] + z * sh[2] - x * sh[3]) / length;
		}

		float write_probe_octahedron(const DirectX::ScratchImage& cubes, const std::size_t cube_index, const std::array<float, 4>& luminance_sh,
			const DirectX::ScratchImage& result)
		{
			const auto cube_levels = cubes.GetMetadata().mipLevels;
			const auto min_irradiance = std::max(0.1f * 0.282095f * luminance_sh[0], 0.001f);
			auto luminance_sum = 0.0f;
			for (auto level = 0u; level < result.GetMetadata().mipLevels; level++)
			{
				const auto& image = *result.GetImage(level, cube_index, 0);
				const auto border = static_cast<float>(4u << std::min(level, 3u));
				const auto scale = 1.0f - border / 128.0f;
				const auto offset = border / 256.0f;
				for (auto row = 0u; row < image.height; row++)
				{
					auto* texel = reinterpret_cast<float*>(image.pixels + row * image.rowPitch);
					for (auto column = 0u; column < image.width; column++, texel += 4)
					{
						const auto [u, v] = wrap_octahedron_border(((column + 0.5f) / image.width - offset) / scale,
							((row + 0.5f) / image.height - offset) / scale);
						const auto direction = get_octahedron_direction(u, v);
						const auto color = sample_cube(cubes, cube_index * 6, std::min<std::size_t>(level, cube_levels - 1), direction);
						const auto irradiance = std::max(evaluate_sh_l1(luminance_sh, direction), min_irradiance);
						for (auto channel = 0u; channel < 3; channel++)
						{
							texel[channel] = std::max(color[channel], 0.0f) / irradiance;
						}

						texel[3] = 1.0f;
						if (!level)
						{
							luminance_sum += 0.2126f * texel[0] + 0.7152f * texel[1] + 0.0722f * texel[2];
						}
					}
				}
			}

			const auto& top = *result.GetImage(0, cube_index, 0);
			return luminance_sum / static_cast<float>(top.width * top.height);
		}

		std::array<float, 3> decode_lightmap_direction(const std::uint8_t* texel)
		{
			const auto u = texel[0] / 255.0f * 2.0f - 1.0f;
			const auto v = texel[1] / 255.0f * 2.0f - 1.0f;
			const auto x = (u + v) * 0.5f;
			const auto y = (u - v) * 0.5f;
			const auto z = 1.0f - std::abs(x) - std::abs(y);
			const auto length = std::sqrt(x * x + y * y + z * z);
			return {x / length, y / length, z / length};
		}

		void encode_lightmap_direction(const std::array<float, 3>& direction, std::uint8_t* texel)
		{
			const auto [x, y, z] = direction;
			const auto sum = std::abs(x) + std::abs(y) + std::max(z, 0.0f);
			const auto u = x / sum;
			const auto v = y / sum;
			texel[0] = to_byte((u + v + 1.0f) * 0.5f);
			texel[1] = to_byte((u - v + 1.0f) * 0.5f);
		}

		target::GfxImageRaw make_raw_image(const DirectX::ScratchImage& image, const target::GfxPixelFormat format, utils::memory::allocator& allocator)
		{
			const auto& metadata = image.GetMetadata();
			target::GfxImageRaw raw{};
			raw.desc.format = format;
			raw.desc.flags = static_cast<target::GfxImageFlags>(target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_NOMIPMAPS);
			raw.desc.totalSize = static_cast<unsigned int>(image.GetPixelsSize());
			raw.desc.width = static_cast<unsigned short>(metadata.width);
			raw.desc.height = static_cast<unsigned short>(metadata.height);
			raw.desc.depth = 1;
			raw.desc.numElements = 1;
			raw.desc.levelCount = 1;
			raw.pixels = allocator.allocate_array<char>(raw.desc.totalSize);
			std::memcpy(raw.pixels, image.GetPixels(), raw.desc.totalSize);
			return raw;
		}

		std::string get_output_name(const GfxImage* asset)
		{
			return asset->name[0] == '$' ? "iw7_"s + (asset->name + 1) : asset->name;
		}

		target::GfxImage* dump_once(const std::string& name, const std::function<target::GfxImage*()>& convert_image)
		{
			static std::unordered_map<std::string, target::GfxImage*> converted;

			const auto [entry, inserted] = converted.try_emplace(name);
			if (inserted && (entry->second = convert_image()))
			{
				target::gfx_image::dump(entry->second);
				add_csv_row("image", name);
			}

			return entry->second;
		}
	}

	target::GfxImage* dump_color_specular(const GfxImage* asset)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, [&]
		{
			const auto srgb = DirectX::IsSRGB(static_cast<DXGI_FORMAT>(asset->imageFormat));

			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[2][i] = srgb ? static_cast<std::uint8_t>(i) : from_squared_gamma(i);
				tables[3][i] = static_cast<std::uint8_t>(i);
			}

			return convert(asset, name, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB | target::IMG_DISK_FLAG_AUTOMETALNESS,
				target::TS_METALNESS_MAP, 0xFF, tables);
		});
	}

	target::GfxImage* dump_color(const GfxImage* asset)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, [&]
		{
			const auto srgb = DirectX::IsSRGB(static_cast<DXGI_FORMAT>(asset->imageFormat));

			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[2][i] = srgb ? static_cast<std::uint8_t>(i) : linear_to_srgb(i / 255.0f);
				tables[3][i] = static_cast<std::uint8_t>(i);
			}

			return convert(asset, name, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_COLOR_MAP, 0, tables);
		});
	}

	target::GfxImage* dump_alpha(const GfxImage* asset)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, [&]
		{
			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[2][i] = tables[3][i] = static_cast<std::uint8_t>(i);
			}

			return convert(asset, name, target::GFX_PF_BC7, 0, target::TS_2D, 0, tables, {0, 0, 0, 0});
		});
	}

	target::GfxImage* dump_opacity(const GfxImage* asset)
	{
		const auto name = get_output_name(asset) + "_opacity";
		return dump_once(name, [&]
		{
			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[2][i] = tables[3][i] = static_cast<std::uint8_t>(i);
			}

			return convert(asset, name, target::GFX_PF_BC7, 0, target::TS_2D, 0, tables, {3, 3, 3, 3});
		});
	}

	target::GfxImage* dump_sky(const GfxImage* asset)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, [&]() -> target::GfxImage*
		{
			const auto cube = read_pixels(asset, DXGI_FORMAT_R32G32B32A32_FLOAT);
			if (!cube)
			{
				return nullptr;
			}

			if (!cube->GetMetadata().IsCubemap())
			{
				ZONETOOL_WARNING("Sky image \"%s\" is not a cube map", asset->name);
				return nullptr;
			}

			const auto octahedron = make_octahedron(*cube);
			const auto compressed = octahedron ? compress(*octahedron, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT) : std::nullopt;
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", asset->name);
				return nullptr;
			}

			return make_image(name, *compressed, target::GFX_PF_BC6H,
				target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_NOMIPMAPS | target::IMG_DISK_FLAG_CLAMP_U | target::IMG_DISK_FLAG_CLAMP_V | target::IMG_DISK_FLAG_OCTAHEDRON,
				target::TS_FUNCTION, 0);
		});
	}

	target::GfxImage* dump_reflection_probes(const GfxImage* asset, const std::vector<std::array<float, 4>>& luminance_sh)
	{
		return dump_once("*reflection_probe_array_octahedron", [&]() -> target::GfxImage*
		{
			const auto cubes = read_pixels(asset, DXGI_FORMAT_R32G32B32A32_FLOAT);
			if (!cubes)
			{
				return nullptr;
			}

			const auto& metadata = cubes->GetMetadata();
			if (!metadata.IsCubemap() || metadata.arraySize < 6 * luminance_sh.size())
			{
				ZONETOOL_WARNING("Image \"%s\" has %zu cube faces for %zu reflection probes", asset->name, metadata.arraySize, luminance_sh.size());
				return nullptr;
			}

			DirectX::ScratchImage octahedrons;
			if (FAILED(octahedrons.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 256, 256, luminance_sh.size(), 6)))
			{
				return nullptr;
			}

			auto luminance = 0.0f;
			for (auto i = 0u; i < luminance_sh.size(); i++)
			{
				luminance += write_probe_octahedron(*cubes, i, luminance_sh[i], octahedrons);
			}

			ZONETOOL_INFO("Image \"%s\": %zu reflection probes, mean luminance %.2f", asset->name, luminance_sh.size(),
				luminance / luminance_sh.size());

			const auto compressed = compress(octahedrons, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", asset->name);
				return nullptr;
			}

			auto* image = make_image("*reflection_probe_array_octahedron", *compressed, target::GFX_PF_BC6H,
				target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_MAPTYPE_ARRAY, target::TS_FUNCTION, 0);
			image->atlasInfo = {};
			image->category = target::IMG_CATEGORY_LIGHTMAP;
			return image;
		});
	}

	target::GfxImage* dump_normal_gloss(const GfxImage* asset)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, [&]
		{
			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[3][i] = static_cast<std::uint8_t>(i);
				tables[2][i] = to_byte(i / 255.0f * i / 255.0f);
			}

			return convert(asset, name, target::GFX_PF_BC7, target::IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS, target::TS_NORMAL_OCCLUSION_GLOSS_MAP,
				0xFF00FF, tables);
		});
	}

	target::GfxImage* dump_normal_gloss(const GfxImage* normal, const GfxImage* specular_gloss, const GfxImage* occlusion)
	{
		const std::array<const GfxImage*, 3> sources{normal, specular_gloss, occlusion};

		std::string name;
		for (const auto* source : sources)
		{
			name += source ? get_output_name(source) + "_" : "";
		}

		name = name.empty() ? "iw7_flat_ng" : name + "ng";
		return dump_once(name, [&]() -> target::GfxImage*
		{
			std::array<std::optional<DirectX::ScratchImage>, 3> pixels;
			std::size_t width = 4, height = 4;
			for (auto i = 0u; i < sources.size(); i++)
			{
				if (sources[i] && (pixels[i] = read_pixels(sources[i], DXGI_FORMAT_R8G8B8A8_UNORM)))
				{
					width = std::max(width, pixels[i]->GetMetadata().width);
					height = std::max(height, pixels[i]->GetMetadata().height);
				}
			}

			width = (width + 3) & ~3ull;
			height = (height + 3) & ~3ull;
			for (auto& source : pixels)
			{
				DirectX::ScratchImage resized;
				if (source && (source->GetMetadata().width != width || source->GetMetadata().height != height))
				{
					if (FAILED(DirectX::Resize(*source->GetImage(0, 0, 0), width, height, DirectX::TEX_FILTER_LINEAR | DirectX::TEX_FILTER_FORCE_NON_WIC, resized)))
					{
						ZONETOOL_WARNING("Cannot resize the sources of image \"%s\"", name.data());
						return nullptr;
					}

					source = std::move(resized);
				}
			}

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto read_texel = [](const std::optional<DirectX::ScratchImage>& source, const std::size_t row, const std::size_t column) -> const std::uint8_t*
			{
				return source ? source->GetImage(0, 0, 0)->pixels + row * source->GetImage(0, 0, 0)->rowPitch + column * 4 : nullptr;
			};

			const auto& image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					const auto* normal_texel = read_texel(pixels[0], row, column);
					const auto* specular_gloss_texel = read_texel(pixels[1], row, column);
					const auto* occlusion_texel = read_texel(pixels[2], row, column);

					auto* texel = image.pixels + row * image.rowPitch + column * 4;
					texel[0] = specular_gloss_texel ? specular_gloss_texel[3] : 0;
					texel[1] = normal_texel ? normal_texel[1] : 128;
					texel[2] = occlusion_texel ? to_byte(occlusion_texel[0] / 255.0f * occlusion_texel[0] / 255.0f) : 255;
					texel[3] = normal_texel ? normal_texel[3] : 128;
				}
			}

			DirectX::ScratchImage mips;
			if (FAILED(DirectX::GenerateMipMaps(image, DirectX::TEX_FILTER_BOX | DirectX::TEX_FILTER_FORCE_NON_WIC, 0, mips)))
			{
				return nullptr;
			}

			const auto compressed = compress(mips, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", name.data());
				return nullptr;
			}

			return make_image(name, *compressed, target::GFX_PF_BC7, target::IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS, target::TS_NORMAL_OCCLUSION_GLOSS_MAP,
				0xFF00FF);
		});
	}

	std::optional<target::GfxLightmapRaw> convert_lightmap(const GfxLightMap* lightmap, utils::memory::allocator& allocator)
	{
		if (!lightmap->textures[0] || !lightmap->textures[1] || !lightmap->textures[2])
		{
			return {};
		}

		const auto shadow = read_pixels(lightmap->textures[0], DXGI_FORMAT_R8_UNORM);
		const auto light = read_pixels(lightmap->textures[1], DXGI_FORMAT_R32G32B32A32_FLOAT);
		const auto direction = read_pixels(lightmap->textures[2], DXGI_FORMAT_R8G8_UNORM);
		if (!shadow || !light || !direction)
		{
			return {};
		}

		const auto& shadow_image = *shadow->GetImage(0, 0, 0);
		const auto& light_image = *light->GetImage(0, 0, 0);
		const auto& direction_image = *direction->GetImage(0, 0, 0);
		const auto width = direction_image.width;
		const auto height = direction_image.height;
		if (shadow_image.width != width || shadow_image.height != height || light_image.width != width || light_image.height != height * 2)
		{
			ZONETOOL_WARNING("Lightmap \"%s\" has images of different sizes", lightmap->name);
			return {};
		}

		const auto scale = std::max<std::size_t>(1, std::max(width, height) / 1024);
		const auto target_width = width / scale;
		const auto target_height = height / scale;

		DirectX::ScratchImage ambient, irradiance, direction_shadow;
		if (FAILED(ambient.Initialize2D(DXGI_FORMAT_R8_UNORM, target_width, target_height, 1, 1))
			|| FAILED(irradiance.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, target_width, target_height, 1, 1))
			|| FAILED(direction_shadow.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, target_width, target_height, 1, 1)))
		{
			return {};
		}

		const auto& ambient_image = *ambient.GetImage(0, 0, 0);
		const auto& irradiance_image = *irradiance.GetImage(0, 0, 0);
		const auto& direction_shadow_image = *direction_shadow.GetImage(0, 0, 0);
		const auto light_texel = [&](const std::size_t column, const std::size_t row)
		{
			return reinterpret_cast<const float*>(light_image.pixels + row * light_image.rowPitch) + column * 4;
		};

		for (auto row = 0u; row < target_height; row++)
		{
			for (auto column = 0u; column < target_width; column++)
			{
				std::array<float, 3> top{}, bottom{}, average_direction{};
				auto visibility = 0.0f;
				auto weight = 0.0f;
				for (auto source_row = row * scale; source_row < (row + 1) * scale; source_row++)
				{
					for (auto source_column = column * scale; source_column < (column + 1) * scale; source_column++)
					{
						const auto* top_texel = light_texel(source_column, source_row);
						const auto* bottom_texel = light_texel(source_column, source_row + height);
						if (top_texel[0] + top_texel[1] + top_texel[2] + bottom_texel[0] + bottom_texel[1] + bottom_texel[2] <= 0.0f)
						{
							continue;
						}

						const auto texel_direction = decode_lightmap_direction(direction_image.pixels + source_row * direction_image.rowPitch + source_column * 2);
						for (auto channel = 0u; channel < 3; channel++)
						{
							top[channel] += top_texel[channel];
							bottom[channel] += bottom_texel[channel];
							average_direction[channel] += texel_direction[channel];
						}

						visibility += shadow_image.pixels[source_row * shadow_image.rowPitch + source_column] / 255.0f;
						weight += 1.0f;
					}
				}

				auto* ambient_texel = ambient_image.pixels + row * ambient_image.rowPitch + column;
				auto* irradiance_texel = reinterpret_cast<float*>(irradiance_image.pixels + row * irradiance_image.rowPitch) + column * 4;
				auto* direction_texel = direction_shadow_image.pixels + row * direction_shadow_image.rowPitch + column * 4;

				const auto length = std::sqrt(average_direction[0] * average_direction[0] + average_direction[1] * average_direction[1]
					+ average_direction[2] * average_direction[2]);
				if (weight == 0.0f || length < 0.0001f)
				{
					*ambient_texel = 255;
					irradiance_texel[0] = irradiance_texel[1] = irradiance_texel[2] = 0.0f;
					irradiance_texel[3] = 1.0f;
					encode_lightmap_direction({0.0f, 0.0f, 1.0f}, direction_texel);
					direction_texel[2] = 255;
					direction_texel[3] = 255;
					continue;
				}

				for (auto& value : average_direction)
				{
					value /= length;
				}

				const auto direction_z = std::max(average_direction[2], 0.001f);
				auto top_sum = 0.0f;
				auto irradiance_sum = 0.0f;
				for (auto channel = 0u; channel < 3; channel++)
				{
					irradiance_texel[channel] = (top[channel] + bottom[channel] * direction_z) / weight;
					top_sum += top[channel] / weight;
					irradiance_sum += irradiance_texel[channel];
				}

				irradiance_texel[3] = 1.0f;
				*ambient_texel = to_byte(irradiance_sum > 0.0f ? top_sum / irradiance_sum : 1.0f);
				encode_lightmap_direction(average_direction, direction_texel);
				direction_texel[2] = to_byte(visibility / weight);
				direction_texel[3] = 255;
			}
		}

		const auto compressed_ambient = compress(ambient, DXGI_FORMAT_BC4_UNORM, DirectX::TEX_COMPRESS_DEFAULT);
		const auto packed_irradiance = decode(std::move(irradiance), DXGI_FORMAT_R11G11B10_FLOAT);
		if (!compressed_ambient || !packed_irradiance)
		{
			ZONETOOL_WARNING("Cannot encode lightmap \"%s\"", lightmap->name);
			return {};
		}

		target::GfxLightmapRaw result{};
		result.textures[0] = make_raw_image(*compressed_ambient, target::GFX_PF_BC4, allocator);
		result.textures[1] = make_raw_image(*packed_irradiance, target::GFX_PF_R11G11B10F, allocator);
		result.textures[2] = make_raw_image(direction_shadow, target::GFX_PF_R8G8B8A8, allocator);
		return result;
	}
}
