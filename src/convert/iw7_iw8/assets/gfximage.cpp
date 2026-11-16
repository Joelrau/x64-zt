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
			const std::size_t level_count, const std::string_view pixels)
		{
			DirectX::ScratchImage image;
			if (FAILED(image.Initialize2D(DirectX::MakeLinear(format), width, height, 1, level_count)) || image.GetPixelsSize() != pixels.size())
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
			if (asset->mapType != MAPTYPE_2D || asset->depth > 1 || asset->numElements > 1)
			{
				return {};
			}

			const auto format = static_cast<DXGI_FORMAT>(asset->imageFormat);
			if (!asset->streamed)
			{
				if (!asset->pixelData)
				{
					return {};
				}

				return load_pixels(format, asset->width, asset->height, asset->levelCount,
					{reinterpret_cast<const char*>(asset->pixelData), asset->dataLen1});
			}

			std::array<unsigned int, 4> streams{0, 1, 2, 3};
			std::ranges::sort(streams, std::greater{}, [&](const unsigned int i) { return asset->streams[i].width; });
			for (const auto stream : streams)
			{
				const auto& data = asset->streams[stream];
				const auto pixels = read_streamed_image_pixels(asset, stream);
				const auto levels = pixels ? count_levels(format, data.width, data.height, pixels->size()) : 0;
				if (levels)
				{
					return load_pixels(format, data.width, data.height, levels, *pixels);
				}
			}

			return {};
		}

		std::optional<DirectX::ScratchImage> decode_rgba8(DirectX::ScratchImage source)
		{
			const auto format = source.GetMetadata().format;
			if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
			{
				return source;
			}

			DirectX::ScratchImage decoded;
			const auto result = DirectX::IsCompressed(format)
				? DirectX::Decompress(source.GetImages(), source.GetImageCount(), source.GetMetadata(), DXGI_FORMAT_R8G8B8A8_UNORM, decoded)
				: DirectX::Convert(source.GetImages(), source.GetImageCount(), source.GetMetadata(), DXGI_FORMAT_R8G8B8A8_UNORM,
					DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, decoded);
			if (FAILED(result))
			{
				return {};
			}

			return decoded;
		}

		std::optional<DirectX::ScratchImage> compress_bc7(const DirectX::ScratchImage& source)
		{
			auto metadata = source.GetMetadata();
			metadata.format = DXGI_FORMAT_BC7_UNORM;

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
					if (FAILED(DirectX::Compress(part, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK, DirectX::TEX_THRESHOLD_DEFAULT, result)))
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

		target::GfxImage* convert(const GfxImage* asset, const target::GfxPixelFormat format, const std::uint32_t flags,
			const target::TextureSemantic semantic, const unsigned int scale_bias, const channel_tables& tables)
		{
			static utils::memory::allocator allocator;

			const auto loaded = static_cast<const GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, asset->name));
			auto source = load_source(loaded ? loaded : asset);
			auto decoded = source ? decode_rgba8(std::move(*source)) : std::nullopt;
			if (!decoded)
			{
				ZONETOOL_WARNING("Cannot read the pixels of image \"%s\"", asset->name);
				return nullptr;
			}

			const auto& source_metadata = decoded->GetMetadata();
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
						for (auto channel = 0u; channel < tables.size(); channel++)
						{
							texel[channel] = tables[channel][texel[channel]];
						}
					}
				}
			}

			const auto compressed = compress_bc7(*decoded);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", asset->name);
				return nullptr;
			}

			const auto& metadata = compressed->GetMetadata();
			const auto result = allocator.allocate<target::GfxImage>();
			result->name = allocator.duplicate_string(asset->name);
			result->format = format;
			result->flags = static_cast<target::GfxImageFlags>(flags);
			result->totalSize = static_cast<unsigned int>(compressed->GetPixelsSize());
			result->semanticSpecific.albedoMapScaleBias = scale_bias;
			result->width = static_cast<unsigned short>(metadata.width);
			result->height = static_cast<unsigned short>(metadata.height);
			result->depth = 1;
			result->numElements = 1;
			result->atlasInfo.atlasSize = {.rowCount = 1, .colCount = 1};
			result->semantic = semantic;
			result->category = target::IMG_CATEGORY_LOAD_FROM_FILE;
			result->levelCount = static_cast<unsigned char>(metadata.mipLevels);
			result->pixels.residentData = allocator.allocate_array<char>(result->totalSize);
			std::memcpy(result->pixels.residentData, compressed->GetPixels(), result->totalSize);
			return result;
		}

		target::GfxImage* dump_once(const GfxImage* asset, const std::function<target::GfxImage*()>& convert_image)
		{
			static std::unordered_map<std::string, target::GfxImage*> converted;

			const auto [entry, inserted] = converted.try_emplace(asset->name);
			if (inserted && (entry->second = convert_image()))
			{
				target::gfx_image::dump(entry->second);
				add_csv_row("image", asset->name);
			}

			return entry->second;
		}
	}

	target::GfxImage* dump_color_specular(const GfxImage* asset)
	{
		return dump_once(asset, [&]
		{
			const auto srgb = DirectX::IsSRGB(static_cast<DXGI_FORMAT>(asset->imageFormat));

			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[2][i] = srgb ? static_cast<std::uint8_t>(i) : from_squared_gamma(i);
				tables[3][i] = static_cast<std::uint8_t>(i);
			}

			return convert(asset, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB | target::IMG_DISK_FLAG_AUTOMETALNESS,
				target::TS_METALNESS_MAP, 0xFF, tables);
		});
	}

	target::GfxImage* dump_color(const GfxImage* asset)
	{
		return dump_once(asset, [&]
		{
			const auto srgb = DirectX::IsSRGB(static_cast<DXGI_FORMAT>(asset->imageFormat));

			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[2][i] = srgb ? static_cast<std::uint8_t>(i) : linear_to_srgb(i / 255.0f);
				tables[3][i] = static_cast<std::uint8_t>(i);
			}

			return convert(asset, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_COLOR_MAP, 0, tables);
		});
	}

	target::GfxImage* dump_normal_gloss(const GfxImage* asset)
	{
		return dump_once(asset, [&]
		{
			channel_tables tables{};
			for (auto i = 0; i < 256; i++)
			{
				tables[0][i] = tables[1][i] = tables[3][i] = static_cast<std::uint8_t>(i);
				tables[2][i] = to_byte(i / 255.0f * i / 255.0f);
			}

			return convert(asset, target::GFX_PF_BC7, target::IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS, target::TS_NORMAL_OCCLUSION_GLOSS_MAP,
				0xFF00FF, tables);
		});
	}
}
