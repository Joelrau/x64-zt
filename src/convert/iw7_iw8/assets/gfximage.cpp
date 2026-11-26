#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"

#include <io/output_cache.hpp>
#include <utils/flags.hpp>

#include <formats/image_compress.hpp>

namespace zonetool::iw7::converter::iw8::gfximage
{
	namespace
	{
		namespace target = zonetool::iw8;

		using channel_tables = std::array<std::array<std::uint8_t, 256>, 4>;

		std::optional<DirectX::ScratchImage> load_pixels(const DXGI_FORMAT format, const std::size_t width, const std::size_t height, const std::size_t depth,
			const std::size_t level_count, const std::size_t cube_count, const std::string_view pixels)
		{
			DirectX::ScratchImage image;
			const auto result = cube_count
				? image.InitializeCube(DirectX::MakeLinear(format), width, height, cube_count, level_count)
				: depth > 1
				? image.Initialize3D(DirectX::MakeLinear(format), width, height, depth, level_count)
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

		std::size_t get_max_image_size()
		{
			static const auto size = std::stoull(utils::flags::get_flag("max_image_size").value_or("0"));
			return size;
		}

		std::optional<DirectX::ScratchImage> load_source(const GfxImage* asset)
		{
			const auto cube_array = asset->mapType == MAPTYPE_CUBE_ARRAY;
			const auto cube = asset->mapType == MAPTYPE_CUBE || cube_array;
			const auto volume = asset->mapType == MAPTYPE_3D;
			if ((asset->mapType != MAPTYPE_2D && !cube && !volume) || (asset->depth > 1 && !volume) || (volume && asset->streamed)
				|| (asset->numElements > 1 && !cube_array))
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

				return load_pixels(format, asset->width, asset->height, volume ? asset->depth : 1u, asset->levelCount, cube_count,
					{reinterpret_cast<const char*>(asset->pixelData), asset->dataLen1});
			}

			std::array<unsigned int, 4> streams{0, 1, 2, 3};
			std::ranges::sort(streams, std::greater{}, [&](const unsigned int i) { return asset->streams[i].width; });
			if (const auto max_size = get_max_image_size())
			{
				const auto smaller = std::ranges::find_if(streams, [&](const unsigned int i)
				{
					return std::max<std::size_t>(asset->streams[i].width, asset->streams[i].height) < max_size;
				});
				std::reverse(streams.begin(), smaller);
			}

			for (const auto stream : streams)
			{
				const auto& data = asset->streams[stream];
				const auto pixels = read_streamed_image_pixels(asset, stream);
				const auto levels = pixels && pixels->size() % face_count == 0
					? count_levels(format, data.width, data.height, pixels->size() / face_count)
					: 0;
				if (levels)
				{
					return load_pixels(format, data.width, data.height, 1, levels, cube_count, *pixels);
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

		std::uint8_t to_byte(const float value)
		{
			return static_cast<std::uint8_t>(std::lround(std::clamp(value, 0.0f, 1.0f) * 255.0f));
		}

		std::uint8_t linear_to_srgb(const float linear)
		{
			return to_byte(linear <= 0.0031308f ? linear * 12.92f : 1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f);
		}

		float srgb_to_linear(const float srgb)
		{
			return srgb <= 0.04045f ? srgb / 12.92f : std::pow((srgb + 0.055f) / 1.055f, 2.4f);
		}

		void decode_srgb_color(const DirectX::Image& image)
		{
			for (auto row = 0u; row < image.height; row++)
			{
				auto* texels = reinterpret_cast<float*>(image.pixels + row * image.rowPitch);
				for (auto column = 0u; column < image.width; column++)
				{
					for (auto channel = 0u; channel < 3; channel++)
					{
						texels[column * 4 + channel] = srgb_to_linear(texels[column * 4 + channel]);
					}
				}
			}
		}

		std::uint8_t from_squared_gamma(const int value)
		{
			return linear_to_srgb(value / 255.0f * value / 255.0f);
		}

		std::optional<DirectX::ScratchImage> read_source(const GfxImage* asset)
		{
			const auto loaded = static_cast<const GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, asset->name));
			return load_source(loaded ? loaded : asset);
		}

		std::optional<DirectX::ScratchImage> decode_source(const GfxImage* asset, std::optional<DirectX::ScratchImage> source, const DXGI_FORMAT format)
		{
			auto decoded = source ? decode(std::move(*source), format) : std::nullopt;
			if (!decoded)
			{
				ZONETOOL_WARNING("Cannot read the pixels of image \"%s\"", asset->name);
			}

			return decoded;
		}

		std::optional<DirectX::ScratchImage> read_pixels(const GfxImage* asset, const DXGI_FORMAT format)
		{
			return decode_source(asset, read_source(asset), format);
		}

		bool is_identity(const channel_tables& tables, const std::array<std::uint8_t, 4>& sources)
		{
			for (auto channel = 0u; channel < tables.size(); channel++)
			{
				if (sources[channel] != channel)
				{
					return false;
				}

				for (auto i = 0u; i < tables[channel].size(); i++)
				{
					if (tables[channel][i] != i)
					{
						return false;
					}
				}
			}

			return true;
		}

		bool is_bc7_2d(const DirectX::ScratchImage& image)
		{
			const auto& metadata = image.GetMetadata();
			return metadata.format == DXGI_FORMAT_BC7_UNORM && metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE2D && metadata.arraySize == 1
				&& metadata.width % 4 == 0 && metadata.height % 4 == 0;
		}

		bool limit_size(DirectX::ScratchImage& image)
		{
			const auto max_size = get_max_image_size();
			const auto& metadata = image.GetMetadata();
			if (!max_size || std::max(metadata.width, metadata.height) <= max_size)
			{
				return true;
			}

			auto first_level = 0u;
			auto width = metadata.width;
			auto height = metadata.height;
			while (std::max(width, height) > max_size)
			{
				width = std::max<std::size_t>(4, width / 2);
				height = std::max<std::size_t>(4, height / 2);
				first_level++;
			}

			DirectX::ScratchImage limited;
			if (first_level < metadata.mipLevels && image.GetImage(first_level, 0, 0)->width == width && image.GetImage(first_level, 0, 0)->height == height
				&& SUCCEEDED(limited.Initialize2D(metadata.format, width, height, 1, metadata.mipLevels - first_level)))
			{
				for (auto level = first_level; level < metadata.mipLevels; level++)
				{
					const auto* source = image.GetImage(level, 0, 0);
					std::memcpy(limited.GetImage(level - first_level, 0, 0)->pixels, source->pixels, source->slicePitch);
				}
			}
			else if (FAILED(DirectX::Resize(*image.GetImage(0, 0, 0), width, height, DirectX::TEX_FILTER_LINEAR | DirectX::TEX_FILTER_FORCE_NON_WIC, limited)))
			{
				return false;
			}

			image = std::move(limited);
			return true;
		}

		target::GfxImage* make_image(const std::string& name, const DirectX::ScratchImage& compressed, const target::GfxPixelFormat format,
			const std::uint32_t flags, const target::TextureSemantic semantic)
		{
			static utils::memory::allocator allocator;

			const auto& metadata = compressed.GetMetadata();
			const auto result = allocator.allocate<target::GfxImage>();
			result->name = allocator.duplicate_string(name);
			result->format = format;
			result->flags = static_cast<target::GfxImageFlags>(flags);
			result->totalSize = static_cast<unsigned int>(compressed.GetPixelsSize());
			result->width = static_cast<unsigned short>(metadata.width);
			result->height = static_cast<unsigned short>(metadata.height);
			result->depth = 1;
			result->numElements = static_cast<unsigned short>(metadata.arraySize);
			result->atlasInfo.atlasSize = {.rowCount = 1, .colCount = 1};
			result->semantic = semantic;
			result->category = target::IMG_CATEGORY_LOAD_FROM_FILE;
			result->levelCount = static_cast<unsigned char>(metadata.mipLevels);
			result->pixels.residentData = allocator.allocate_array<char>(result->totalSize);

			auto* pixels = result->pixels.residentData;
			for (auto level = 0u; level < metadata.mipLevels; level++)
			{
				for (auto item = 0u; item < metadata.arraySize; item++)
				{
					const auto* image = compressed.GetImage(level, item, 0);
					std::memcpy(pixels, image->pixels, image->slicePitch);
					pixels += image->slicePitch;
				}
			}

			return result;
		}

		target::GfxImage* convert(const GfxImage* asset, const std::string& name, const target::GfxPixelFormat format, const std::uint32_t flags,
			const target::TextureSemantic semantic, const channel_tables& tables, const std::array<std::uint8_t, 4>& sources = {0, 1, 2, 3})
		{
			auto loaded = read_source(asset);
			if (loaded && is_identity(tables, sources) && is_bc7_2d(*loaded) && limit_size(*loaded))
			{
				return make_image(name, *loaded, format, flags, semantic);
			}

			auto decoded = decode_source(asset, std::move(loaded), DXGI_FORMAT_R8G8B8A8_UNORM);
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

			if (!limit_size(*decoded))
			{
				ZONETOOL_WARNING("Cannot limit the size of image \"%s\"", asset->name);
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

			const auto compressed = image_compress::compress(*decoded, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", asset->name);
				return nullptr;
			}

			return make_image(name, *compressed, format, flags, semantic);
		}

		template <std::size_t count>
		bool read_composed_sources(const std::array<const GfxImage*, count>& sources, std::array<std::optional<DirectX::ScratchImage>, count>& pixels,
			std::size_t& width, std::size_t& height)
		{
			width = 4;
			height = 4;
			for (auto i = 0u; i < count; i++)
			{
				if (sources[i] && (pixels[i] = read_pixels(sources[i], DXGI_FORMAT_R8G8B8A8_UNORM)))
				{
					if (!limit_size(*pixels[i]))
					{
						return false;
					}

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
						return false;
					}

					source = std::move(resized);
				}
			}

			return true;
		}

		const std::uint8_t* read_texel(const std::optional<DirectX::ScratchImage>& source, const std::size_t row, const std::size_t column)
		{
			return source ? source->GetImage(0, 0, 0)->pixels + row * source->GetImage(0, 0, 0)->rowPitch + column * 4 : nullptr;
		}

		target::GfxImage* compress_composed(const std::string& name, const DirectX::Image& image, const target::GfxPixelFormat format, const std::uint32_t flags,
			const target::TextureSemantic semantic)
		{
			DirectX::ScratchImage mips;
			if (FAILED(DirectX::GenerateMipMaps(image, DirectX::TEX_FILTER_BOX | DirectX::TEX_FILTER_FORCE_NON_WIC, 0, mips)))
			{
				return nullptr;
			}

			const auto compressed = image_compress::compress(mips, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", name.data());
				return nullptr;
			}

			return make_image(name, *compressed, format, flags, semantic);
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

		std::array<float, 3> rotate_to_sky(const std::array<float, 3>& direction, const std::array<std::array<float, 3>, 3>& axis)
		{
			std::array<float, 3> result{};
			for (auto i = 0u; i < result.size(); i++)
			{
				result[i] = direction[0] * axis[i][0] + direction[1] * axis[i][1] + direction[2] * axis[i][2];
			}

			return result;
		}

		std::optional<DirectX::ScratchImage> make_octahedron(const DirectX::ScratchImage& cube, const std::array<float, 3>& rotation_angles,
			const std::array<float, 3>& color_scale)
		{
			const auto size = std::min<std::size_t>(2048, cube.GetMetadata().width * 2);

			std::array<std::array<float, 3>, 3> axis{};
			float right[3]{};
			AngleVectors(rotation_angles.data(), axis[0].data(), right, axis[2].data());
			axis[1] = {-right[0], -right[1], -right[2]};

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
					const auto color = sample_cube(cube, 0, 0, rotate_to_sky(direction, axis));
					texel[0] = std::max(color[0] * color_scale[0], 0.0f);
					texel[1] = std::max(color[1] * color_scale[1], 0.0f);
					texel[2] = std::max(color[2] * color_scale[2], 0.0f);
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
			return 0.282095f * sh[0] + 0.325735f * (-y * sh[1] + z * sh[2] - x * sh[3]) / length; // iw7 lit PS, stock iw8 bakes use 0.488603
		}

		std::array<float, 4> project_luminance_sh(const DirectX::ScratchImage& cubes, const std::size_t cube_index)
		{
			std::array<float, 4> sh{};
			auto weight_sum = 0.0f;
			for (auto row = 0u; row < 256u; row++)
			{
				for (auto column = 0u; column < 256u; column++)
				{
					const auto direction = get_octahedron_direction((column + 0.5f) / 256.0f, (row + 0.5f) / 256.0f);
					const auto [x, y, z] = direction;
					const auto length = std::sqrt(x * x + y * y + z * z);
					const auto weight = 1.0f / (length * length * length);
					const auto color = sample_cube(cubes, cube_index * 6, 0, direction);
					const auto luminance = weight * (0.2126f * std::max(color[0], 0.0f) + 0.7152f * std::max(color[1], 0.0f)
						+ 0.0722f * std::max(color[2], 0.0f));
					sh[0] += 0.282095f * luminance;
					sh[1] -= 0.488603f * y / length * luminance;
					sh[2] += 0.488603f * z / length * luminance;
					sh[3] -= 0.488603f * x / length * luminance;
					weight_sum += weight;
				}
			}

			for (auto& value : sh)
			{
				value *= 4.0f * std::numbers::pi_v<float> / weight_sum;
			}

			return sh;
		}

		float write_probe_octahedron(const DirectX::ScratchImage& cubes, const std::size_t cube_index, const DirectX::ScratchImage& result)
		{
			const auto luminance_sh = project_luminance_sh(cubes, cube_index); // TODO: own probe irradiance from stock mp_rust probe data only
			const auto cube_levels = cubes.GetMetadata().mipLevels;
			const auto min_irradiance = std::max(0.282095f * luminance_sh[0] / 3.0f, 0.001f); // r_reflectionProbeNormalizeMax 3.0
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

		std::string get_tinted_name(const GfxImage* asset, const std::array<float, 3>& tint)
		{
			const auto name = get_output_name(asset);
			if (tint == std::array{1.0f, 1.0f, 1.0f})
			{
				return name;
			}

			return std::format("{}_tint_{:x}_{:x}_{:x}", name, std::lround(tint[0] * 255.0f), std::lround(tint[1] * 255.0f), std::lround(tint[2] * 255.0f));
		}

		channel_tables make_identity_tables()
		{
			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[2][i] = tables[3][i] = static_cast<std::uint8_t>(i);
			}

			return tables;
		}

		channel_tables make_inverted_tables()
		{
			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[2][i] = tables[3][i] = static_cast<std::uint8_t>(255 - i);
			}

			return tables;
		}

		std::array<float, 256> make_linear_table(const GfxImage* asset)
		{
			const auto srgb = DirectX::IsSRGB(static_cast<DXGI_FORMAT>(asset->imageFormat));

			std::array<float, 256> table{};
			for (auto i = 0; i < 256; i++)
			{
				table[i] = srgb ? srgb_to_linear(i / 255.0f) : i / 255.0f * i / 255.0f;
			}

			return table;
		}

		channel_tables make_tinted_tables(const GfxImage* asset, const std::array<float, 3>& tint)
		{
			const auto linear = make_linear_table(asset);

			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				for (auto channel = 0u; channel < tint.size(); channel++)
				{
					tables[channel][i] = linear_to_srgb(linear[i] * tint[channel]);
				}

				tables[3][i] = static_cast<std::uint8_t>(i);
			}

			return tables;
		}

		std::uint64_t get_fingerprint(const std::string_view kind, const std::initializer_list<const GfxImage*> sources)
		{
			fingerprint result;
			result.add_value(cache_revision::iw7_iw8_image).add(kind);
			for (const auto* source : sources)
			{
				const auto* loaded = source ? static_cast<const GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, source->name)) : nullptr;
				const auto* image = loaded ? loaded : source;
				if (!image)
				{
					result.add_value(0);
					continue;
				}

				result.add(image->name).add_value(image->imageFormat).add_value(image->flags).add_value(image->mapType).add_value(image->width)
					.add_value(image->height).add_value(image->depth).add_value(image->numElements).add_value(image->levelCount)
					.add_value(image->streamed).add_value(image->dataLen1).add_value(image->streams);
				if (const auto max_size = get_max_image_size(); max_size && std::max<std::size_t>(image->width, image->height) > max_size)
				{
					result.add_value(max_size);
				}
			}

			return result.value();
		}

		std::vector<std::uint8_t> read_cube_volume(const GfxImage* asset, std::size_t& size)
		{
			const auto volume = read_pixels(asset, DXGI_FORMAT_R8G8B8A8_UNORM);
			if (!volume)
			{
				return {};
			}

			const auto& metadata = volume->GetMetadata();
			if (metadata.dimension != DirectX::TEX_DIMENSION_TEXTURE3D || metadata.width != metadata.height || metadata.width != metadata.depth)
			{
				ZONETOOL_WARNING("Image \"%s\" is not a cube volume", asset->name);
				return {};
			}

			size = metadata.width;
			std::vector<std::uint8_t> texels(size * size * size * 4);
			for (auto slice = 0u; slice < size; slice++)
			{
				const auto& image = *volume->GetImage(0, 0, slice);
				for (auto row = 0u; row < size; row++)
				{
					std::memcpy(&texels[(slice * size + row) * size * 4], image.pixels + row * image.rowPitch, size * 4);
				}
			}

			return texels;
		}

		std::vector<std::uint8_t> make_identity_volume(const std::size_t size)
		{
			std::vector<std::uint8_t> texels(size * size * size * 4);
			for (auto b = 0u; b < size; b++)
			{
				for (auto g = 0u; g < size; g++)
				{
					for (auto r = 0u; r < size; r++)
					{
						auto* texel = &texels[((b * size + g) * size + r) * 4];
						texel[0] = static_cast<std::uint8_t>(std::lround(r * 255.0 / (size - 1)));
						texel[1] = static_cast<std::uint8_t>(std::lround(g * 255.0 / (size - 1)));
						texel[2] = static_cast<std::uint8_t>(std::lround(b * 255.0 / (size - 1)));
						texel[3] = 255;
					}
				}
			}

			return texels;
		}

		target::GfxImage* dump_once(const std::string& name, const std::optional<std::uint64_t> fingerprint,
			const target::GfxImageSemanticSpecific semantic_specific, const std::function<target::GfxImage*()>& convert_image)
		{
			static utils::memory::allocator allocator;
			static std::unordered_map<std::string, target::GfxImage*> converted;

			const auto [entry, inserted] = converted.try_emplace(name);
			if (!inserted)
			{
				return entry->second;
			}

			auto file_name = name;
			std::ranges::replace(file_name, '*', '_');
			const auto output = std::format("{}images\\{}.iw8Image", filesystem::get_dump_path(), file_name);
			if (fingerprint && is_dump_current(output, *fingerprint))
			{
				entry->second = allocator.allocate<target::GfxImage>();
				entry->second->name = allocator.duplicate_string(name);
				entry->second->semanticSpecific = semantic_specific;
			}
			else if ((entry->second = convert_image()))
			{
				entry->second->semanticSpecific = semantic_specific;
				target::gfx_image::dump(entry->second);
				if (fingerprint)
				{
					update_dump_cache(output, *fingerprint);
				}
			}

			if (entry->second)
			{
				add_csv_row("image", name);
			}

			return entry->second;
		}
	}

	target::GfxImage* dump_color_specular(const GfxImage* asset, const std::array<float, 3>& tint)
	{
		const auto name = get_tinted_name(asset, tint);
		return dump_once(name, get_fingerprint("color_specular", {asset}), {.albedoMapScaleBias = 0xFF}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB | target::IMG_DISK_FLAG_AUTOMETALNESS,
				target::TS_METALNESS_MAP, make_tinted_tables(asset, tint));
		});
	}

	target::GfxImage* dump_color_specular(const GfxImage* color, const GfxImage* specular_gloss, const std::array<float, 3>& tint,
		const bool premultiply_alpha)
	{
		const auto name = std::format("{}_{}_cs{}", get_tinted_name(color, tint), get_output_name(specular_gloss), premultiply_alpha ? "_premultiplied" : "");
		const auto fingerprint = get_fingerprint(premultiply_alpha ? "color_specular_premultiplied" : "color_specular_2", {color, specular_gloss});
		return dump_once(name, fingerprint, {.albedoMapScaleBias = 0xFF}, [&]() -> target::GfxImage*
		{
			const std::array sources{color, specular_gloss};
			std::array<std::optional<DirectX::ScratchImage>, 2> pixels;
			std::size_t width, height;
			if (!read_composed_sources(sources, pixels, width, height) || !pixels[0] || !pixels[1])
			{
				ZONETOOL_WARNING("Cannot read the sources of image \"%s\"", name.data());
				return nullptr;
			}

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto color_linear = make_linear_table(color);
			const auto specular_linear = make_linear_table(specular_gloss);
			const auto get_luminance = [](const std::array<float, 3>& value)
			{
				return value[0] * 0.2126f + value[1] * 0.7152f + value[2] * 0.0722f;
			};

			const auto& image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					const auto* color_texel = read_texel(pixels[0], row, column);
					const auto* specular_texel = read_texel(pixels[1], row, column);

					const auto coverage = premultiply_alpha ? color_texel[3] / 255.0f : 1.0f;
					std::array<float, 3> diffuse{}, specular{}, albedo{};
					for (auto channel = 0u; channel < 3; channel++)
					{
						diffuse[channel] = color_linear[color_texel[channel]] * tint[channel] * coverage;
						specular[channel] = specular_linear[specular_texel[channel]];
						albedo[channel] = std::max(diffuse[channel] + specular[channel] - 0.1f, 0.0f);
					}

					const auto specular_luminance = get_luminance(specular);
					const auto albedo_luminance = get_luminance(albedo);
					const auto metalness = specular_luminance > 0.1f && albedo_luminance > 0.0f
						? std::min((specular_luminance - 0.1f) / albedo_luminance, 1.0f) : 0.0f;

					auto* texel = image.pixels + row * image.rowPitch + column * 4;
					for (auto channel = 0u; channel < 3; channel++)
					{
						texel[channel] = linear_to_srgb(metalness > 0.0f ? albedo[channel] : diffuse[channel]);
					}

					texel[3] = to_byte(metalness > 0.0f ? 0.1f + 0.9f * metalness : specular_luminance);
				}
			}

			return compress_composed(name, image, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB | target::IMG_DISK_FLAG_AUTOMETALNESS,
				target::TS_METALNESS_MAP);
		});
	}

	target::GfxImage* dump_color(const GfxImage* asset, const std::array<float, 3>& tint)
	{
		const auto name = get_tinted_name(asset, tint);
		return dump_once(name, get_fingerprint("color", {asset}), {}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_COLOR_MAP, make_tinted_tables(asset, tint));
		});
	}

	target::GfxImage* dump_color_premultiplied(const GfxImage* asset)
	{
		const auto name = get_output_name(asset) + "_premultiplied";
		return dump_once(name, get_fingerprint("color_premultiplied", {asset}), {}, [&]() -> target::GfxImage*
		{
			const std::array sources{asset};
			std::array<std::optional<DirectX::ScratchImage>, 1> pixels;
			std::size_t width, height;
			if (!read_composed_sources(sources, pixels, width, height) || !pixels[0])
			{
				ZONETOOL_WARNING("Cannot read the sources of image \"%s\"", name.data());
				return nullptr;
			}

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto linear = make_linear_table(asset);
			const auto& image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					const auto* source = read_texel(pixels[0], row, column);
					auto* texel = image.pixels + row * image.rowPitch + column * 4;
					for (auto channel = 0u; channel < 3; channel++)
					{
						texel[channel] = linear_to_srgb(linear[source[channel]] * source[3] / 255.0f);
					}

					texel[3] = source[3];
				}
			}

			return compress_composed(name, image, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_COLOR_MAP);
		});
	}

	target::GfxImage* dump_decal_color(const GfxImage* asset)
	{
		const auto name = get_output_name(asset) + "&defaultspec";
		return dump_once(name, get_fingerprint("decal_color", {asset}), {.albedoMapScaleBias = 9}, [&] // TODO: stock decal value
		{
			auto tables = make_tinted_tables(asset, {1.0f, 1.0f, 1.0f});
			tables[3].fill(255);
			return convert(asset, name, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB | target::IMG_DISK_FLAG_AUTOMETALNESS,
				target::TS_METALNESS_MAP, tables);
		});
	}

	target::GfxImage* dump_alpha(const GfxImage* asset)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, get_fingerprint("alpha_reveal_2", {asset}), {}, [&]() -> target::GfxImage*
		{
			const std::array sources{asset};
			std::array<std::optional<DirectX::ScratchImage>, 1> pixels;
			std::size_t width, height;
			if (!read_composed_sources(sources, pixels, width, height) || !pixels[0])
			{
				ZONETOOL_WARNING("Cannot read the sources of image \"%s\"", name.data());
				return nullptr;
			}

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto& image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					const auto* source = read_texel(pixels[0], row, column);
					const auto alpha = source[1] ? source[0] : std::uint8_t(0); // revealParams.x >= 0 gives a reveal threshold of 0
					std::memset(image.pixels + row * image.rowPitch + column * 4, alpha, 4);
				}
			}

			return compress_composed(name, image, target::GFX_PF_BC7, 0, target::TS_2D);
		});
	}

	target::GfxImage* dump_layer_mask(const GfxImage* asset)
	{
		const auto name = get_output_name(asset) + "_mask";
		return dump_once(name, get_fingerprint("layer_mask", {asset}), {}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7, 0, target::TS_2D, make_identity_tables());
		});
	}

	target::GfxImage* dump_layer_mask(const GfxImage* coverage, const GfxImage* reveal)
	{
		const auto name = std::format("{}_{}_mask", get_output_name(coverage), get_output_name(reveal));
		return dump_once(name, get_fingerprint("layer_mask_2", {coverage, reveal}), {}, [&]() -> target::GfxImage*
		{
			const std::array sources{coverage, reveal};
			std::array<std::optional<DirectX::ScratchImage>, 2> pixels;
			std::size_t width, height;
			if (!read_composed_sources(sources, pixels, width, height) || !pixels[0] || !pixels[1])
			{
				ZONETOOL_WARNING("Cannot read the sources of image \"%s\"", name.data());
				return nullptr;
			}

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto& image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					auto* texel = image.pixels + row * image.rowPitch + column * 4;
					texel[0] = read_texel(pixels[0], row, column)[3];
					texel[1] = read_texel(pixels[1], row, column)[0];
					texel[2] = 0xFF;
					texel[3] = 0xFF;
				}
			}

			return compress_composed(name, image, target::GFX_PF_BC7, 0, target::TS_2D);
		});
	}

	target::GfxImage* dump_2d(const GfxImage* asset)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, get_fingerprint("2d", {asset}), {}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_CLAMP_U
				| target::IMG_DISK_FLAG_CLAMP_V | target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_2D, make_identity_tables());
		});
	}

	target::GfxImage* dump_opacity(const GfxImage* asset)
	{
		const auto name = get_output_name(asset) + "_opacity";
		return dump_once(name, get_fingerprint("opacity", {asset}), {}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7, 0, target::TS_2D, make_identity_tables(), {3, 3, 3, 3});
		});
	}

	target::GfxImage* dump_transmission(const GfxImage* coverage, const bool coverage_in_alpha)
	{
		const auto name = get_output_name(coverage) + (coverage_in_alpha ? "_alpha_transmission" : "_transmission");
		const auto channel = static_cast<std::uint8_t>(coverage_in_alpha ? 3 : 0);
		return dump_once(name, get_fingerprint(coverage_in_alpha ? "alpha_transmission" : "transmission", {coverage}), {}, [&]
		{
			return convert(coverage, name, target::GFX_PF_BC7, 0, target::TS_2D, make_inverted_tables(), {channel, channel, channel, channel});
		});
	}

	target::GfxImage* dump_sky(const GfxImage* asset, const std::array<float, 3>& rotation_angles, const std::array<float, 3>& color_scale)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, std::nullopt, {}, [&]() -> target::GfxImage*
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

			const auto octahedron = make_octahedron(*cube, rotation_angles, color_scale);
			const auto compressed = octahedron ? image_compress::compress(*octahedron, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT) : std::nullopt;
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", asset->name);
				return nullptr;
			}

			return make_image(name, *compressed, target::GFX_PF_BC6H,
				target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_NOMIPMAPS | target::IMG_DISK_FLAG_CLAMP_U | target::IMG_DISK_FLAG_CLAMP_V | target::IMG_DISK_FLAG_OCTAHEDRON,
				target::TS_FUNCTION);
		});
	}

	target::GfxImage* dump_generated(const GfxImage* asset, const target::GfxPixelFormat format)
	{
		return dump_once(asset->name, get_fingerprint("generated", {asset}), {}, [&]() -> target::GfxImage*
		{
			const auto* loaded = static_cast<const GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, asset->name));
			const auto* image = loaded ? loaded : asset;
			const auto source_pitch = image->height ? image->dataLen1 / image->height : 0u;
			if (!image->pixelData || image->streamed || image->imageFormat != DXGI_FORMAT_R8_UNORM || image->levelCount != 1
				|| image->depth != 1 || image->numElements != 1 || source_pitch < image->width)
			{
				ZONETOOL_WARNING("Cannot convert generated image \"%s\"", asset->name);
				return nullptr;
			}

			static utils::memory::allocator allocator;
			const auto pitch = (image->width + 15u) & ~15u; // TODO: 16 byte rows from stock *ieslookup and *heightmap0 only
			const auto result = allocator.allocate<target::GfxImage>();
			result->name = allocator.duplicate_string(asset->name);
			result->format = format;
			result->flags = static_cast<target::GfxImageFlags>(image->flags);
			result->totalSize = pitch * image->height;
			result->width = image->width;
			result->height = image->height;
			result->depth = 1;
			result->numElements = 1;
			result->semantic = target::TS_FUNCTION;
			result->category = target::IMG_CATEGORY_AUTO_GENERATED;
			result->levelCount = 1;
			result->pixels.residentData = allocator.allocate_array<char>(result->totalSize);
			for (auto row = 0u; row < image->height; row++)
			{
				std::memcpy(result->pixels.residentData + row * pitch, image->pixelData + row * source_pitch, image->width);
			}

			return result;
		});
	}

	target::GfxImage* dump_grading_clut(const GfxImage* asset, const std::string& name, const std::size_t size, const std::uint64_t settings,
		const std::function<std::vector<std::uint16_t>(const std::vector<std::uint8_t>& texels, std::size_t source_size)>& convert_texels)
	{
		const auto cache_key = fingerprint().add_value(get_fingerprint("grading_clut", {asset})).add_value(settings).value();
		return dump_once(name, cache_key, {}, [&]() -> target::GfxImage*
		{
			auto source_size = size;
			const auto texels = asset ? read_cube_volume(asset, source_size) : make_identity_volume(size);
			if (texels.empty())
			{
				return nullptr;
			}

			const auto converted = convert_texels(texels, source_size);
			if (converted.size() != size * size * size * 4)
			{
				return nullptr;
			}

			static utils::memory::allocator allocator;
			const auto result = allocator.allocate<target::GfxImage>();
			result->name = allocator.duplicate_string(name);
			result->format = target::GFX_PF_R16G16B16A16;
			result->flags = static_cast<target::GfxImageFlags>(target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_NOMIPMAPS
				| target::IMG_DISK_FLAG_MAPTYPE_3D);
			result->totalSize = static_cast<unsigned int>(converted.size() * sizeof(std::uint16_t));
			result->width = static_cast<unsigned short>(size);
			result->height = static_cast<unsigned short>(size);
			result->depth = static_cast<unsigned short>(size);
			result->numElements = 1;
			result->semantic = target::TS_FUNCTION;
			result->category = target::IMG_CATEGORY_AUTO_GENERATED;
			result->levelCount = 1;
			result->unknown = 8; // TODO: stock gradingclutmp_rust_clut only
			result->pixels.residentData = allocator.allocate_array<char>(result->totalSize);
			std::memcpy(result->pixels.residentData, converted.data(), result->totalSize);
			return result;
		});
	}

	target::GfxImage* dump_reflection_probes(const GfxImage* asset, const std::size_t probe_count)
	{
		return dump_once("*reflection_probe_array_octahedron", std::nullopt, {}, [&]() -> target::GfxImage*
		{
			const auto cubes = read_pixels(asset, DXGI_FORMAT_R32G32B32A32_FLOAT);
			if (!cubes)
			{
				return nullptr;
			}

			const auto& metadata = cubes->GetMetadata();
			if (!metadata.IsCubemap() || metadata.arraySize < 6 * probe_count)
			{
				ZONETOOL_WARNING("Image \"%s\" has %zu cube faces for %zu reflection probes", asset->name, metadata.arraySize, probe_count);
				return nullptr;
			}

			DirectX::ScratchImage octahedrons;
			if (FAILED(octahedrons.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 256, 256, probe_count, 6)))
			{
				return nullptr;
			}

			auto luminance = 0.0f;
			for (auto i = 0u; i < probe_count; i++)
			{
				luminance += write_probe_octahedron(*cubes, i, octahedrons);
			}

			ZONETOOL_INFO("Image \"%s\": %zu reflection probes, mean luminance %.2f", asset->name, probe_count, luminance / probe_count);

			const auto compressed = image_compress::compress(octahedrons, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", asset->name);
				return nullptr;
			}

			auto* image = make_image("*reflection_probe_array_octahedron", *compressed, target::GFX_PF_BC6H,
				target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_MAPTYPE_ARRAY, target::TS_FUNCTION);
			image->atlasInfo = {};
			image->category = target::IMG_CATEGORY_LIGHTMAP;
			return image;
		});
	}

	target::GfxImage* dump_normal_gloss(const GfxImage* asset)
	{
		const auto name = get_output_name(asset);
		return dump_once(name, get_fingerprint("normal_gloss", {asset}), {.normalMapScaleBias = 0xFF00FF}, [&]
		{
			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[3][i] = static_cast<std::uint8_t>(i);
				tables[2][i] = to_byte(i / 255.0f * i / 255.0f);
			}

			return convert(asset, name, target::GFX_PF_BC7, target::IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS, target::TS_NORMAL_OCCLUSION_GLOSS_MAP, tables);
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
		return dump_once(name, get_fingerprint("normal_gloss_3", {normal, specular_gloss, occlusion}), {.normalMapScaleBias = 0xFF00FF},
			[&]() -> target::GfxImage*
		{
			const auto identity_normal = normal && normal->name == "$identitynormalmap"sv;
			const std::array<const GfxImage*, 3> pixel_sources{identity_normal ? nullptr : normal, specular_gloss, occlusion};

			std::array<std::optional<DirectX::ScratchImage>, 3> pixels;
			std::size_t width, height;
			if (!read_composed_sources(pixel_sources, pixels, width, height))
			{
				ZONETOOL_WARNING("Cannot resize the sources of image \"%s\"", name.data());
				return nullptr;
			}

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

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

			return compress_composed(name, image, target::GFX_PF_BC7, target::IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS, target::TS_NORMAL_OCCLUSION_GLOSS_MAP);
		});
	}

	std::optional<target::GfxLightmapRaw> convert_lightmap(const GfxLightMap* lightmap, utils::memory::allocator& allocator)
	{
		if (!lightmap->textures[0] || !lightmap->textures[1] || !lightmap->textures[2])
		{
			return {};
		}

		const auto shadow = read_pixels(lightmap->textures[0], DXGI_FORMAT_R8_UNORM);
		auto light = read_pixels(lightmap->textures[1], DXGI_FORMAT_R32G32B32A32_FLOAT);
		const auto direction = read_pixels(lightmap->textures[2], DXGI_FORMAT_R8G8_UNORM);
		if (!shadow || !light || !direction)
		{
			return {};
		}

		if (DirectX::IsSRGB(static_cast<DXGI_FORMAT>(lightmap->textures[1]->imageFormat)))
		{
			decode_srgb_color(*light->GetImage(0, 0, 0));
		}

		const auto& shadow_image = *shadow->GetImage(0, 0, 0);
		const auto& light_image = *light->GetImage(0, 0, 0);
		const auto& direction_image = *direction->GetImage(0, 0, 0);
		const auto width = direction_image.width;
		const auto height = direction_image.height;
		const auto light_width = light_image.width;
		const auto light_height = light_image.height / 2;
		if (light_image.height % 2 != 0)
		{
			ZONETOOL_WARNING("Lightmap \"%s\" has an odd light image height", lightmap->name);
			return {};
		}

		if (shadow_image.width != width || shadow_image.height != height || light_width != width || light_height != height)
		{
			ZONETOOL_WARNING("Lightmap \"%s\" has images of different sizes, resampling to %zux%zu", lightmap->name, width, height);
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
		const auto light_texel = [&](const std::size_t column, const std::size_t row, const std::size_t half)
		{
			const auto light_column = column * light_width / width;
			const auto light_row = row * light_height / height + half * light_height;
			return reinterpret_cast<const float*>(light_image.pixels + light_row * light_image.rowPitch) + light_column * 4;
		};
		const auto shadow_texel = [&](const std::size_t column, const std::size_t row)
		{
			const auto shadow_column = column * shadow_image.width / width;
			const auto shadow_row = row * shadow_image.height / height;
			return shadow_image.pixels[shadow_row * shadow_image.rowPitch + shadow_column];
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
						const auto* top_texel = light_texel(source_column, source_row, 0);
						const auto* bottom_texel = light_texel(source_column, source_row, 1);
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

						visibility += shadow_texel(source_column, source_row) / 255.0f;
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

		const auto compressed_ambient = image_compress::compress(ambient, DXGI_FORMAT_BC4_UNORM, DirectX::TEX_COMPRESS_DEFAULT);
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

	void dump_preview(const GfxImage* asset)
	{
		auto pixels = read_pixels(asset, DXGI_FORMAT_R8G8B8A8_UNORM);
		if (!pixels)
		{
			return;
		}

		const auto& source = *pixels->GetImage(0, 0, 0);
		const auto scale = std::min(1.0, 2048.0 / std::max(source.width, source.height));
		const auto width = std::max<std::size_t>(1, static_cast<std::size_t>(source.width * scale));
		const auto height = std::max<std::size_t>(1, static_cast<std::size_t>(source.height * scale));
		if (width != source.width || height != source.height)
		{
			DirectX::ScratchImage resized;
			if (FAILED(DirectX::Resize(source, width, height, DirectX::TEX_FILTER_LINEAR | DirectX::TEX_FILTER_FORCE_NON_WIC, resized)))
			{
				ZONETOOL_WARNING("Cannot resize the preview \"%s\"", asset->name);
				return;
			}

			pixels = std::move(resized);
		}

		const auto& image = *pixels->GetImage(0, 0, 0);
		const std::array<std::uint32_t, 4> header{0x4952574D, 1, static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)}; // mw120r preview.rgba v1

		filesystem::file file("preview.rgba");
		file.open("wb");
		file.write(header.data(), sizeof(header), 1);
		for (auto row = 0u; row < height; row++)
		{
			file.write(image.pixels + row * image.rowPitch, width * 4, 1);
		}

		file.close();
	}
}
