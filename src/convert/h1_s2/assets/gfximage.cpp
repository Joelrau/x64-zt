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
				const auto hr = DirectX::IsCompressed(source_format)
					? DirectX::Decompress(source, work_format, work)
					: DirectX::Convert(source, work_format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, work);
				if (FAILED(hr))
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

		bool is_2d(const GfxImage* asset)
		{
			return asset->mapType == MAPTYPE_2D && asset->depth <= 1 && asset->numElements <= 1;
		}

		void dump_image(zonetool::s2::GfxImage* image, const std::string& pixels)
		{
			image->pixelData = reinterpret_cast<unsigned char*>(const_cast<char*>(pixels.data()));
			image->dataLen1 = static_cast<unsigned int>(pixels.size());
			zonetool::s2::gfx_image::dump(image);
		}

		// S2 sky shaders read a 2D texture with uv = 0.5 + 0.5 * dir.xy / (len + |0.919831 * dir.z + 0.080169 * len|)
		constexpr auto sky_z_scale = 0.919831f;
		constexpr auto sky_size = 1024u;

		std::array<float, 3> sky_direction(const float px, const float py)
		{
			const auto r2 = std::min(px * px + py * py, 1.0f);
			const auto c = 1.0f + 0.080169f;
			const auto qa = r2 * sky_z_scale * sky_z_scale + 1.0f;
			const auto qb = 2.0f * r2 * sky_z_scale * c;
			const auto qc = r2 * c * c - 1.0f;
			const auto z = (-qb + std::sqrt(std::max(0.0f, qb * qb - 4.0f * qa * qc))) / (2.0f * qa);
			const auto d = c + sky_z_scale * z;
			return {px * d, py * d, z};
		}

		std::tuple<std::size_t, float, float> cube_face(const std::array<float, 3>& dir)
		{
			const auto [x, y, z] = dir;
			const auto ax = std::abs(x);
			const auto ay = std::abs(y);
			const auto az = std::abs(z);
			std::size_t face{};
			float sc{}, tc{}, ma{};
			if (ax >= ay && ax >= az)
			{
				face = x >= 0.0f ? 0 : 1;
				sc = x >= 0.0f ? -z : z;
				tc = -y;
				ma = ax;
			}
			else if (ay >= az)
			{
				face = y >= 0.0f ? 2 : 3;
				sc = x;
				tc = y >= 0.0f ? z : -z;
				ma = ay;
			}
			else
			{
				face = z >= 0.0f ? 4 : 5;
				sc = z >= 0.0f ? x : -x;
				tc = -y;
				ma = az;
			}

			return {face, (sc / ma + 1.0f) * 0.5f, (tc / ma + 1.0f) * 0.5f};
		}

		std::optional<std::string> bake_sky(const GfxImage* asset, image_pixels& pixels)
		{
			const auto format = static_cast<DXGI_FORMAT>(asset->imageFormat);
			std::size_t row_pitch{};
			std::size_t slice_pitch{};
			if (FAILED(DirectX::ComputePitch(format, pixels.width, pixels.height, row_pitch, slice_pitch)) || slice_pitch * 6 > pixels.data.size())
			{
				return {};
			}

			std::vector<DirectX::ScratchImage> faces(6);
			for (auto i = 0u; i < faces.size(); i++)
			{
				const DirectX::Image source{pixels.width, pixels.height, format, row_pitch, slice_pitch,
					reinterpret_cast<std::uint8_t*>(pixels.data.data() + slice_pitch * i)};
				const auto hr = DirectX::IsCompressed(format)
					? DirectX::Decompress(source, DXGI_FORMAT_R32G32B32A32_FLOAT, faces[i])
					: DirectX::Convert(source, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, faces[i]);
				if (FAILED(hr))
				{
					return {};
				}
			}

			DirectX::ScratchImage sky;
			if (FAILED(sky.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, sky_size, sky_size, 1, 1)))
			{
				return {};
			}

			const auto& target = *sky.GetImage(0, 0, 0);
			for (auto y = 0u; y < sky_size; y++)
			{
				const auto row = reinterpret_cast<float*>(target.pixels + y * target.rowPitch);
				for (auto x = 0u; x < sky_size; x++)
				{
					const auto px = (x + 0.5f) / sky_size * 2.0f - 1.0f;
					const auto py = (y + 0.5f) / sky_size * 2.0f - 1.0f;
					const auto [face, u, v] = cube_face(sky_direction(px, py));

					const auto& source = *faces[face].GetImage(0, 0, 0);
					const auto sx = std::min(static_cast<std::size_t>(u * source.width), source.width - 1);
					const auto sy = std::min(static_cast<std::size_t>(v * source.height), source.height - 1);
					const auto texel = reinterpret_cast<const float*>(source.pixels + sy * source.rowPitch) + sx * 4;
					std::copy_n(texel, 4, row + x * 4);
				}
			}

			DirectX::ScratchImage mips;
			if (FAILED(DirectX::GenerateMipMaps(target, DirectX::TEX_FILTER_DEFAULT, 0, mips)))
			{
				return {};
			}

			DirectX::ScratchImage compressed;
			if (FAILED(DirectX::Compress(mips.GetImages(), mips.GetImageCount(), mips.GetMetadata(), DXGI_FORMAT_BC1_UNORM,
				DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, compressed)))
			{
				return {};
			}

			pixels.width = sky_size;
			pixels.height = sky_size;
			pixels.levels = compressed.GetMetadata().mipLevels;
			return std::string{reinterpret_cast<const char*>(compressed.GetPixels()), compressed.GetPixelsSize()};
		}
	}

	void dump(GfxImage* asset)
	{
		if (asset->name[0] == '$')
		{
			return;
		}

		auto pixels = read_pixels(asset);
		if (!pixels)
		{
			ZONETOOL_WARNING("Image \"%s\" has no pixel data", asset->name);
			return;
		}

		auto format = static_cast<DXGI_FORMAT>(asset->imageFormat);
		if (asset->semantic == TS_NORMAL_MAP && format == DXGI_FORMAT_BC5_UNORM && is_2d(asset))
		{
			if (convert_normal_map(*pixels, format))
			{
				format = DXGI_FORMAT_BC5_SNORM;
			}
			else
			{
				ZONETOOL_WARNING("Failed to convert normal map \"%s\"", asset->name);
			}
		}
		else if (asset->semantic == TS_SPECULAR_MAP && is_2d(asset))
		{
			if (convert_specular_map(*pixels, format))
			{
				format = DXGI_FORMAT_BC1_UNORM;
			}
			else
			{
				ZONETOOL_WARNING("Failed to convert specular map \"%s\"", asset->name);
			}
		}
		else if (asset->semantic == TS_COLOR_MAP)
		{
			format = srgb_format(format);
		}

		const auto format_index = s2_format_index(format);
		if (!format_index)
		{
			ZONETOOL_WARNING("Image \"%s\" has a format that S2 does not support (%d)", asset->name, format);
			return;
		}

		zonetool::s2::GfxImage image{};
		image.name = asset->name;
		image.imageFormat = static_cast<DXGI_FORMAT>(*format_index);
		image.width = static_cast<unsigned short>(pixels->width);
		image.height = static_cast<unsigned short>(pixels->height);
		image.depth = asset->depth;
		image.numElements = asset->numElements;
		image.mapType = static_cast<zonetool::s2::MapType>(asset->mapType);
		image.semantic = convert_semantic(asset->semantic);
		image.category = asset->category;
		image.levelCount = static_cast<unsigned char>(pixels->levels);
		image.picmip = asset->picmip.platform[0];

		dump_image(&image, pixels->data);
		dumped_images.insert(clean_name(asset->name));
		add_csv_row("image", asset->name);
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

		zonetool::s2::GfxImage image{};
		image.name = name.data();
		image.imageFormat = static_cast<DXGI_FORMAT>(*s2_format_index(DXGI_FORMAT_R8G8B8A8_UNORM));
		image.width = size;
		image.height = size;
		image.depth = 1;
		image.numElements = 1;
		image.mapType = zonetool::s2::MAPTYPE_2D;
		image.semantic = semantic;
		image.levelCount = 1;

		dump_image(&image, pixels);
		dumped_images.insert(name);
		add_csv_row("image", name);
		return name;
	}

	std::optional<std::string> dump_sky(GfxImage* asset)
	{
		const auto name = clean_name(asset->name) + "_s2sky";
		if (dumped_images.contains(name))
		{
			return name;
		}

		auto pixels = read_pixels(asset);
		const auto data = pixels ? bake_sky(asset, *pixels) : std::nullopt;
		if (!data)
		{
			ZONETOOL_WARNING("Failed to convert sky cubemap \"%s\"", asset->name);
			return {};
		}

		zonetool::s2::GfxImage image{};
		image.name = name.data();
		image.imageFormat = static_cast<DXGI_FORMAT>(*s2_format_index(DXGI_FORMAT_BC1_UNORM_SRGB));
		image.width = static_cast<unsigned short>(pixels->width);
		image.height = static_cast<unsigned short>(pixels->height);
		image.depth = 1;
		image.numElements = 1;
		image.mapType = zonetool::s2::MAPTYPE_2D;
		image.semantic = TS_COLOR_MAP;
		image.category = asset->category;
		image.levelCount = static_cast<unsigned char>(pixels->levels);

		dump_image(&image, *data);
		dumped_images.insert(name);
		add_csv_row("image", name);
		return name;
	}

	bool is_dumped(const std::string& name)
	{
		return dumped_images.contains(clean_name(name));
	}
}
