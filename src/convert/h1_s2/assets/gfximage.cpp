#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"

#include <formats/compression.hpp>

#pragma warning(push)
#pragma warning(disable : 4459)
#include <DirectXTex.h>
#pragma warning(pop)

namespace zonetool::h1::converter::s2::gfximage
{
	namespace
	{
		constexpr DXGI_FORMAT s2_image_formats[]
		{
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			DXGI_FORMAT_A8_UNORM,
			DXGI_FORMAT_R8_UNORM,
			DXGI_FORMAT_R8G8_UNORM,
			DXGI_FORMAT_R8G8_SNORM,
			DXGI_FORMAT_BC1_UNORM,
			DXGI_FORMAT_BC1_UNORM_SRGB,
			DXGI_FORMAT_BC2_UNORM,
			DXGI_FORMAT_BC2_UNORM_SRGB,
			DXGI_FORMAT_BC3_UNORM,
			DXGI_FORMAT_BC3_UNORM_SRGB,
			DXGI_FORMAT_BC4_UNORM,
			DXGI_FORMAT_BC5_UNORM,
			DXGI_FORMAT_BC5_SNORM,
			DXGI_FORMAT_BC6H_UF16,
			DXGI_FORMAT_BC6H_SF16,
			DXGI_FORMAT_BC7_UNORM,
			DXGI_FORMAT_BC7_UNORM_SRGB,
			DXGI_FORMAT_R16G16B16A16_UNORM,
			DXGI_FORMAT_R16G16_UNORM,
			DXGI_FORMAT_R16G16_SNORM,
			DXGI_FORMAT_R16_UNORM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R16G16_FLOAT,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R16_FLOAT,
			DXGI_FORMAT_R11G11B10_FLOAT,
			DXGI_FORMAT_R10G10B10A2_UNORM,
			DXGI_FORMAT_D16_UNORM,
			DXGI_FORMAT_D32_FLOAT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
			DXGI_FORMAT_B5G6R5_UNORM,
			DXGI_FORMAT_R8_UINT,
			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT_R32G32B32A32_UINT,
		};

		std::unordered_set<std::string> unused_images;
		std::unordered_set<std::string> dumped_images;

		std::string clean_name(std::string name)
		{
			std::replace(name.begin(), name.end(), '*', '_');
			return name;
		}

		std::optional<std::uint32_t> s2_format_index(const DXGI_FORMAT format)
		{
			for (auto i = 1u; i < std::size(s2_image_formats); i++)
			{
				if (s2_image_formats[i] == format)
				{
					return i;
				}
			}

			return {};
		}

		DXGI_FORMAT srgb_format(const DXGI_FORMAT format)
		{
			switch (format)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case DXGI_FORMAT_BC1_UNORM: return DXGI_FORMAT_BC1_UNORM_SRGB;
			case DXGI_FORMAT_BC2_UNORM: return DXGI_FORMAT_BC2_UNORM_SRGB;
			case DXGI_FORMAT_BC3_UNORM: return DXGI_FORMAT_BC3_UNORM_SRGB;
			case DXGI_FORMAT_BC7_UNORM: return DXGI_FORMAT_BC7_UNORM_SRGB;
			default: return format;
			}
		}

		std::uint8_t convert_semantic(const std::uint8_t semantic)
		{
			return semantic <= TS_SPECULAR_MAP ? semantic : TS_2D;
		}

		struct image_pixels
		{
			std::string data;
			std::size_t width;
			std::size_t height;
			std::size_t levels;
		};

		struct image_data
		{
			bool streamed;
			std::vector<image_pixels> chains;
		};

		std::optional<std::string> read_stream(const XStreamFile& stream_file)
		{
			if (!stream_file.offset || stream_file.offsetEnd <= stream_file.offset)
			{
				return {};
			}

			const auto filename = stream_file.fileIndex == XStreamFile::zone_pak_file_index
				? std::format("{}.pak", filesystem::get_fastfile())
				: std::format("imagefile{}.pak", stream_file.fileIndex);

			std::ifstream file(filesystem::get_zone_path(filename) + filename, std::ios::binary);
			if (!file.is_open())
			{
				return {};
			}

			std::string buffer(stream_file.offsetEnd - stream_file.offset, '\0');
			file.seekg(stream_file.offset);
			file.read(buffer.data(), buffer.size());
			if (!file)
			{
				return {};
			}

			return compression::lz4::decompress_lz4_block(buffer);
		}

		std::size_t count_levels(const DXGI_FORMAT format, std::size_t width, std::size_t height, const std::size_t size, const std::size_t elements)
		{
			auto levels = 0ull;
			auto offset = 0ull;
			while (true)
			{
				std::size_t row_pitch{};
				std::size_t slice_pitch{};
				if (FAILED(DirectX::ComputePitch(format, width, height, row_pitch, slice_pitch)) || offset + slice_pitch * elements > size)
				{
					break;
				}

				offset += slice_pitch * elements;
				levels++;
				if (width == 1 && height == 1)
				{
					break;
				}

				width = std::max(1ull, width / 2);
				height = std::max(1ull, height / 2);
			}

			return levels;
		}

		std::size_t element_count(const GfxImage* asset)
		{
			return (asset->mapType == MAPTYPE_CUBE ? 6ull : 1ull) * std::max<std::size_t>(1, asset->numElements) * std::max<std::size_t>(1, asset->depth);
		}

		std::optional<image_pixels> read_pixels(const GfxImage* asset)
		{
			if (!asset->streamed)
			{
				if (!asset->pixelData || !asset->dataLen1)
				{
					return {};
				}

				return image_pixels{{reinterpret_cast<const char*>(asset->pixelData), asset->dataLen1}, asset->width, asset->height, asset->levelCount};
			}

			const auto files = get_image_stream_files(asset);
			for (auto i = static_cast<int>(std::min<std::size_t>(files.size(), std::size(asset->streams))) - 1; i >= 0; i--)
			{
				const auto& stream = asset->streams[i];
				if (!stream.width)
				{
					continue;
				}

				auto data = read_stream(files[i]);
				if (!data)
				{
					continue;
				}

				const auto format = static_cast<DXGI_FORMAT>(asset->imageFormat);
				const auto levels = count_levels(format, stream.width, stream.height, data->size(), element_count(asset));
				if (!levels)
				{
					return {};
				}

				return image_pixels{std::move(*data), stream.width, stream.height, levels};
			}

			return {};
		}

		bool is_2d(const GfxImage* asset)
		{
			return asset->mapType == MAPTYPE_2D && asset->depth <= 1 && asset->numElements <= 1;
		}

		std::vector<image_pixels> read_streams(const GfxImage* asset)
		{
			const auto format = static_cast<DXGI_FORMAT>(asset->imageFormat);
			const auto files = get_image_stream_files(asset);

			std::vector<image_pixels> streams;
			std::size_t level_count{};
			for (auto i = 0u; i < std::min(files.size(), std::size(asset->streams)) && asset->streams[i].width; i++)
			{
				const auto& stream = asset->streams[i];
				const auto stream_level_count = static_cast<std::size_t>(stream.levelCountAndSize.levelCount);
				auto data = read_stream(files[i]);
				if (!data || stream_level_count <= level_count
					|| count_levels(format, stream.width, stream.height, data->size(), 1) < stream_level_count - level_count)
				{
					return {};
				}

				streams.push_back({std::move(*data), stream.width, stream.height, stream_level_count - level_count});
				level_count = stream_level_count;
			}

			return streams;
		}

		HRESULT decode_image(const DirectX::Image& source, const DXGI_FORMAT format, DirectX::ScratchImage& result)
		{
			return DirectX::IsCompressed(source.format)
				? DirectX::Decompress(source, format, result)
				: DirectX::Convert(source, format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, result);
		}

		std::optional<image_data> read_image(const GfxImage* asset)
		{
			if (asset->streamed && is_2d(asset))
			{
				if (auto streams = read_streams(asset); !streams.empty())
				{
					return image_data{true, std::move(streams)};
				}
			}

			auto pixels = read_pixels(asset);
			if (!pixels)
			{
				return {};
			}

			image_data result{false};
			result.chains.push_back(std::move(*pixels));
			return result;
		}

		template <typename Transform>
		bool transform_levels(image_pixels& pixels, const DXGI_FORMAT source_format, const DXGI_FORMAT work_format,
			const DXGI_FORMAT target_format, Transform&& transform)
		{
			std::string result;
			auto width = pixels.width;
			auto height = pixels.height;
			auto offset = 0ull;
			for (auto level = 0ull; level < pixels.levels; level++)
			{
				std::size_t row_pitch{};
				std::size_t slice_pitch{};
				if (FAILED(DirectX::ComputePitch(source_format, width, height, row_pitch, slice_pitch)) || offset + slice_pitch > pixels.data.size())
				{
					return false;
				}

				const DirectX::Image source{width, height, source_format, row_pitch, slice_pitch,
					reinterpret_cast<std::uint8_t*>(pixels.data.data() + offset)};

				DirectX::ScratchImage work;
				if (FAILED(decode_image(source, work_format, work)))
				{
					return false;
				}

				transform(*work.GetImage(0, 0, 0));

				DirectX::ScratchImage compressed;
				if (FAILED(DirectX::Compress(*work.GetImage(0, 0, 0), target_format, DirectX::TEX_COMPRESS_DEFAULT,
					DirectX::TEX_THRESHOLD_DEFAULT, compressed)))
				{
					return false;
				}

				result.append(reinterpret_cast<const char*>(compressed.GetPixels()), compressed.GetPixelsSize());
				offset += slice_pitch;
				width = std::max(1ull, width / 2);
				height = std::max(1ull, height / 2);
			}

			pixels.data = std::move(result);
			return true;
		}

		bool convert_normal_map(image_pixels& pixels, const DXGI_FORMAT format)
		{
			return transform_levels(pixels, format, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_BC5_SNORM, [](const DirectX::Image& image)
			{
				for (auto y = 0u; y < image.height; y++)
				{
					const auto row = reinterpret_cast<float*>(image.pixels + y * image.rowPitch);
					for (auto x = 0u; x < image.width * 4; x++)
					{
						row[x] = row[x] * 2.0f - 1.0f;
					}
				}
			});
		}

		bool convert_specular_map(image_pixels& pixels, const DXGI_FORMAT format)
		{
			return transform_levels(pixels, format, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_BC1_UNORM, [](const DirectX::Image& image)
			{
				for (auto y = 0u; y < image.height; y++)
				{
					const auto row = image.pixels + y * image.rowPitch;
					for (auto x = 0u; x < image.width; x++)
					{
						const auto pixel = row + x * 4;
						const auto reflectance = std::max({pixel[0], pixel[1], pixel[2]}) * 49u / 32u;
						pixel[2] = pixel[3];
						pixel[0] = static_cast<std::uint8_t>(std::min(255u, reflectance));
						pixel[1] = 255;
						pixel[3] = 255;
					}
				}
			});
		}

		template <typename Convert>
		bool convert_chains(std::vector<image_pixels>& chains, Convert&& convert)
		{
			auto converted = chains;
			if (!std::ranges::all_of(converted, convert))
			{
				return false;
			}

			chains = std::move(converted);
			return true;
		}

		DXGI_FORMAT convert_pixels(const GfxImage* asset, std::vector<image_pixels>& chains)
		{
			const auto format = static_cast<DXGI_FORMAT>(asset->imageFormat);
			if (asset->semantic == TS_NORMAL_MAP && format == DXGI_FORMAT_BC5_UNORM && is_2d(asset))
			{
				if (convert_chains(chains, [&](image_pixels& pixels) { return convert_normal_map(pixels, format); }))
				{
					return DXGI_FORMAT_BC5_SNORM;
				}

				ZONETOOL_WARNING("Failed to convert normal map \"%s\"", asset->name);
			}
			else if (asset->semantic == TS_SPECULAR_MAP && is_2d(asset))
			{
				if (convert_chains(chains, [&](image_pixels& pixels) { return convert_specular_map(pixels, format); }))
				{
					return DXGI_FORMAT_BC1_UNORM;
				}

				ZONETOOL_WARNING("Failed to convert specular map \"%s\"", asset->name);
			}
			else if (asset->semantic == TS_COLOR_MAP)
			{
				return srgb_format(format);
			}

			return format;
		}

		zonetool::s2::GfxImage convert_header(const GfxImage* asset, const image_pixels& pixels, const std::uint32_t format_index)
		{
			zonetool::s2::GfxImage image{};
			image.imageFormat = static_cast<DXGI_FORMAT>(format_index);
			image.width = static_cast<unsigned short>(pixels.width);
			image.height = static_cast<unsigned short>(pixels.height);
			image.depth = asset->depth;
			image.numElements = asset->numElements;
			image.mapType = static_cast<zonetool::s2::MapType>(asset->mapType);
			image.semantic = convert_semantic(asset->semantic);
			image.category = asset->category;
			image.levelCount = static_cast<unsigned char>(pixels.levels);
			image.picmip = asset->picmip.platform[0];
			return image;
		}

		void dump_loaded_image(zonetool::s2::GfxImage* image, const std::string& pixels)
		{
			image->pixelData = reinterpret_cast<unsigned char*>(const_cast<char*>(pixels.data()));
			image->dataLen1 = static_cast<unsigned int>(pixels.size());
			zonetool::s2::gfx_image::dump(image);
		}

		void dump_streamed_image(const GfxImage* asset, const std::string& name, const std::vector<image_pixels>& streams, const std::uint32_t format_index)
		{
			zonetool::s2::GfxImage image{};
			image.name = name.data();
			image.imageFormat = static_cast<DXGI_FORMAT>(format_index);
			image.width = 1;
			image.height = 1;
			image.depth = 1;
			image.numElements = 1;
			image.mapType = zonetool::s2::MAPTYPE_2D;
			image.semantic = convert_semantic(asset->semantic);
			image.category = asset->category;
			image.levelCount = 1;
			image.picmip = asset->picmip.platform[0];
			image.streamed = 1;

			std::size_t level_count{};
			std::size_t pixel_size{};
			for (auto i = 0u; i < streams.size(); i++)
			{
				level_count += streams[i].levels;
				pixel_size += streams[i].data.size();

				auto& stream = image.streams[i];
				stream.width = static_cast<unsigned short>(streams[i].width);
				stream.height = static_cast<unsigned short>(streams[i].height);
				stream.levelCountAndSize.levelCount = static_cast<int>(level_count);
				stream.levelCountAndSize.pixelSize = static_cast<int>(pixel_size);
				zonetool::s2::dump_image_stream(&image, i, streams[i].data);
			}

			zonetool::s2::gfx_image::dump(&image);
		}

		void dump_converted_image(const GfxImage* asset, const std::string& name, const image_data& data, const std::uint32_t format_index)
		{
			if (data.streamed)
			{
				dump_streamed_image(asset, name, data.chains, format_index);
				return;
			}

			const auto& pixels = data.chains.front();
			auto image = convert_header(asset, pixels, format_index);
			image.name = name.data();
			dump_loaded_image(&image, pixels.data);
		}

		zonetool::s2::GfxImage generated_image(const std::string& name, const DXGI_FORMAT format, const std::size_t width, const std::size_t height,
			const std::uint8_t semantic)
		{
			zonetool::s2::GfxImage image{};
			image.name = name.data();
			image.imageFormat = static_cast<DXGI_FORMAT>(*s2_format_index(format));
			image.width = static_cast<unsigned short>(width);
			image.height = static_cast<unsigned short>(height);
			image.depth = 1;
			image.numElements = 1;
			image.mapType = zonetool::s2::MAPTYPE_2D;
			image.semantic = semantic;
			image.levelCount = 1;
			return image;
		}

		std::string dump_generated(zonetool::s2::GfxImage& image, const std::string& pixels)
		{
			dump_loaded_image(&image, pixels);
			dumped_images.insert(image.name);
			add_csv_row("image", image.name);
			return image.name;
		}

		bool dump_zone_image(GfxImage* asset)
		{
			auto data = read_image(asset);
			if (!data)
			{
				ZONETOOL_WARNING("Image \"%s\" has no pixel data", asset->name);
				return false;
			}

			const auto format = convert_pixels(asset, data->chains);
			const auto format_index = s2_format_index(format);
			if (!format_index)
			{
				ZONETOOL_WARNING("Image \"%s\" has a format that S2 does not support (%d)", asset->name, format);
				return false;
			}

			dump_converted_image(asset, asset->name, *data, *format_index);
			dumped_images.insert(clean_name(asset->name));
			add_csv_row("image", asset->name);
			return true;
		}
	}

	void dump(GfxImage* asset)
	{
		if (asset->name[0] != '$')
		{
			unused_images.emplace(asset->name);
		}
	}

	std::optional<std::string> dump_used(const std::string& name)
	{
		if (dumped_images.contains(clean_name(name)))
		{
			return name;
		}

		unused_images.erase(name);
		const auto asset = db_find_x_asset_header_safe(ASSET_TYPE_IMAGE, name).image;
		if (!asset || !dump_zone_image(asset))
		{
			return {};
		}

		return name;
	}

	void finish()
	{
		if (!unused_images.empty())
		{
			ZONETOOL_INFO("Skipped %zu images that no converted asset uses", unused_images.size());
		}

		unused_images.clear();
		dumped_images.clear();
	}

	std::optional<std::string> dump_native(GfxImage* asset)
	{
		if (asset->name[0] == '$')
		{
			return asset->name;
		}

		const auto name = ::game::add_source_postfix(clean_name(asset->name), ::game::h1);
		if (dumped_images.contains(name))
		{
			return name;
		}

		// h1 makes cube textures srgb except bc7, 2d ones keep their format
		const auto format = static_cast<DXGI_FORMAT>(asset->imageFormat);
		const auto srgb = asset->mapType == MAPTYPE_CUBE && format != DXGI_FORMAT_BC7_UNORM;
		const auto data = read_image(asset);
		const auto format_index = s2_format_index(srgb ? srgb_format(format) : format);
		if (!data || !format_index)
		{
			ZONETOOL_WARNING("Image \"%s\" cannot keep its H1 encoding", asset->name);
			return {};
		}

		dump_converted_image(asset, name, *data, *format_index);
		dumped_images.insert(name);
		add_csv_row("image", name);
		return name;
	}

	std::string dump_solid(const std::string& name, const std::uint8_t semantic, const std::array<std::uint8_t, 4>& color)
	{
		if (dumped_images.contains(name))
		{
			return name;
		}

		constexpr auto size = 4u;
		std::string pixels;
		for (auto i = 0u; i < size * size; i++)
		{
			pixels.append(reinterpret_cast<const char*>(color.data()), color.size());
		}

		auto image = generated_image(name, DXGI_FORMAT_R8G8B8A8_UNORM, size, size, semantic);
		return dump_generated(image, pixels);
	}

	std::string dump_fog_spline(const std::string& name)
	{
		if (dumped_images.contains(name))
		{
			return name;
		}

		constexpr auto width = 128u;
		const std::string pixels(width * sizeof(std::uint32_t), '\xFF');

		auto image = generated_image(name, DXGI_FORMAT_R10G10B10A2_UNORM, width, 1, TS_FUNCTION);
		image.category = IMG_CATEGORY_AUTO_GENERATED;
		return dump_generated(image, pixels);
	}

	std::string dump_ies_lookup(const std::string& name)
	{
		if (dumped_images.contains(name))
		{
			return name;
		}

		constexpr std::uint16_t texel[2] = {0x3C00, 0};
		std::string pixels;
		for (auto face = 0; face < 6; face++)
		{
			pixels.append(reinterpret_cast<const char*>(texel), sizeof(texel));
		}

		auto image = generated_image(name, DXGI_FORMAT_R16G16_FLOAT, 1, 1, TS_FUNCTION);
		image.mapType = zonetool::s2::MAPTYPE_CUBE;
		image.category = IMG_CATEGORY_AUTO_GENERATED;
		return dump_generated(image, pixels);
	}

	bool is_dumped(const std::string& name)
	{
		return dumped_images.contains(clean_name(name));
	}
}
