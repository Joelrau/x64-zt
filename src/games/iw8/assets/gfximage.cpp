#include <std_include.hpp>
#include "gfximage.hpp"
#include "../xpak.hpp"

#include <DirectXTex.h>

#include <utils/flags.hpp>

namespace zonetool::iw8
{
	namespace
	{
		std::size_t packed_atlas_data_size(const GfxImage* asset)
		{
			return asset->flags & IMG_DISK_FLAG_PACKED_ATLAS ? asset->atlasInfo.packedAtlasDataSize : 0;
		}

		void read_resident_pixels(zone_reader& reader, GfxImage* asset)
		{
			reader.push_stream(XFILE_BLOCK_TEMP);
			if (const auto pixels = reader.read_array(asset->pixels.residentData, 15, asset->totalSize))
			{
				asset->pixels.residentData = reader.duplicate(pixels, asset->totalSize);
			}
			reader.pop_stream();
		}

		std::string clean_name(std::string name)
		{
			std::ranges::replace(name, '*', '_');
			return name;
		}

		void dump_dds(const GfxImage* image, const std::string& pixels)
		{
			const auto format = get_dxgi_format(image->format);
			if (format == DXGI_FORMAT_UNKNOWN)
			{
				ZONETOOL_WARNING("Image \"%s\" has the unknown format %d", image->name, image->format);
				return;
			}

			const auto map_type = image->flags & IMG_DISK_FLAG_MAPTYPE_MASK;
			const auto is_cube = map_type == IMG_DISK_FLAG_MAPTYPE_CUBE || map_type == IMG_DISK_FLAG_MAPTYPE_CUBE_ARRAY;
			const auto is_3d = map_type == IMG_DISK_FLAG_MAPTYPE_3D;

			DirectX::TexMetadata metadata{};
			metadata.width = image->width;
			metadata.height = image->height;
			metadata.depth = is_3d ? std::max<std::size_t>(1, image->depth) : 1;
			metadata.arraySize = get_image_item_count(image);
			metadata.mipLevels = std::max<std::size_t>(1, image->levelCount);
			metadata.format = format;
			metadata.dimension = is_3d ? DirectX::TEX_DIMENSION_TEXTURE3D : DirectX::TEX_DIMENSION_TEXTURE2D;
			if (is_cube)
			{
				metadata.miscFlags |= DirectX::TEX_MISC_TEXTURECUBE;
			}

			DirectX::ScratchImage scratch;
			if (FAILED(scratch.Initialize(metadata)))
			{
				ZONETOOL_WARNING("Cannot create the DDS of image \"%s\"", image->name);
				return;
			}

			const auto tight_pixels = remove_level_padding(image, pixels, 0, static_cast<std::uint32_t>(metadata.mipLevels));
			if (!tight_pixels || tight_pixels->size() != scratch.GetPixelsSize())
			{
				ZONETOOL_WARNING("Image \"%s\" has 0x%zX bytes of pixels, that does not match its levels", image->name, pixels.size());
				return;
			}

			std::memcpy(scratch.GetPixels(), tight_pixels->data(), tight_pixels->size());

			const std::filesystem::path path = filesystem::get_dump_path() + "images\\" + clean_name(image->name) + ".dds";
			std::filesystem::create_directories(path.parent_path());
			if (FAILED(DirectX::SaveToDDSFile(scratch.GetImages(), scratch.GetImageCount(), metadata, DirectX::DDS_FLAGS_NONE, path.wstring().data())))
			{
				ZONETOOL_WARNING("Cannot write \"%s\"", path.string().data());
			}
		}
	}

	std::uint32_t get_image_item_count(const GfxImage* image)
	{
		const auto map_type = image->flags & IMG_DISK_FLAG_MAPTYPE_MASK;
		const auto faces = map_type == IMG_DISK_FLAG_MAPTYPE_CUBE || map_type == IMG_DISK_FLAG_MAPTYPE_CUBE_ARRAY ? 6u : 1u;
		return std::max<std::uint32_t>(1, image->numElements) * faces;
	}

	std::optional<std::string> remove_level_padding(const GfxImage* image, const std::string_view pixels, const std::uint32_t first_level,
		const std::uint32_t level_count)
	{
		const auto format = get_dxgi_format(image->format);
		if (format == DXGI_FORMAT_UNKNOWN)
		{
			return {};
		}

		const auto is_3d = (image->flags & IMG_DISK_FLAG_MAPTYPE_MASK) == IMG_DISK_FLAG_MAPTYPE_3D;

		std::string result;
		result.reserve(pixels.size());

		std::size_t offset{};
		for (auto item = 0u; item < get_image_item_count(image); item++)
		{
			for (auto level = first_level; level < first_level + level_count; level++)
			{
				std::size_t row_pitch{};
				std::size_t slice_pitch{};
				if (FAILED(DirectX::ComputePitch(format, std::max(1, image->width >> level), std::max(1, image->height >> level), row_pitch, slice_pitch)))
				{
					return {};
				}

				const auto size = slice_pitch * (is_3d ? std::max(1, image->depth >> level) : 1);
				if (offset + size > pixels.size())
				{
					return {};
				}

				result.append(pixels.substr(offset, size));
				offset += (size + 15) & ~static_cast<std::size_t>(15);
			}
		}

		if (offset != pixels.size())
		{
			return {};
		}

		return result;
	}

	DXGI_FORMAT get_dxgi_format(const GfxPixelFormat format)
	{
		switch (format)
		{
		case GFX_PF_R8: return DXGI_FORMAT_R8_UNORM;
		case GFX_PF_A8: return DXGI_FORMAT_A8_UNORM;
		case GFX_PF_L8: return DXGI_FORMAT_R8_UNORM;
		case GFX_PF_R8G8: return DXGI_FORMAT_R8G8_UNORM;
		case GFX_PF_L8A8: return DXGI_FORMAT_R8G8_UNORM;
		case GFX_PF_R8G8B8A8: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case GFX_PF_R8G8B8A8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case GFX_PF_R8_SNORM: return DXGI_FORMAT_R8_SNORM;
		case GFX_PF_R8G8_SNORM: return DXGI_FORMAT_R8G8_SNORM;
		case GFX_PF_R16: return DXGI_FORMAT_R16_UNORM;
		case GFX_PF_R16G16: return DXGI_FORMAT_R16G16_UNORM;
		case GFX_PF_R16G16B16A16: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case GFX_PF_R16_SNORM: return DXGI_FORMAT_R16_SNORM;
		case GFX_PF_R16F: return DXGI_FORMAT_R16_FLOAT;
		case GFX_PF_R16G16F: return DXGI_FORMAT_R16G16_FLOAT;
		case GFX_PF_R16G16B16A16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case GFX_PF_R32F: return DXGI_FORMAT_R32_FLOAT;
		case GFX_PF_R32G32F: return DXGI_FORMAT_R32G32_FLOAT;
		case GFX_PF_R32G32B32A32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case GFX_PF_D16: return DXGI_FORMAT_D16_UNORM;
		case GFX_PF_D32F: return DXGI_FORMAT_D32_FLOAT;
		case GFX_PF_D32F_S8: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case GFX_PF_R8_UINT: return DXGI_FORMAT_R8_UINT;
		case GFX_PF_R16_UINT: return DXGI_FORMAT_R16_UINT;
		case GFX_PF_R32_UINT: return DXGI_FORMAT_R32_UINT;
		case GFX_PF_R32G32_UINT: return DXGI_FORMAT_R32G32_UINT;
		case GFX_PF_R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
		case GFX_PF_R10G10B10A2_UINT: return DXGI_FORMAT_R10G10B10A2_UINT;
		case GFX_PF_R5G6B5: return DXGI_FORMAT_B5G6R5_UNORM;
		case GFX_PF_R10G10B10A2: return DXGI_FORMAT_R10G10B10A2_UNORM;
		case GFX_PF_R9G9B9E5_SHAREDEXP: return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		case GFX_PF_R11G11B10F: return DXGI_FORMAT_R11G11B10_FLOAT;
		case GFX_PF_BC1: return DXGI_FORMAT_BC1_UNORM;
		case GFX_PF_BC1_SRGB: return DXGI_FORMAT_BC1_UNORM_SRGB;
		case GFX_PF_BC2: return DXGI_FORMAT_BC2_UNORM;
		case GFX_PF_BC2_SRGB: return DXGI_FORMAT_BC2_UNORM_SRGB;
		case GFX_PF_BC3: return DXGI_FORMAT_BC3_UNORM;
		case GFX_PF_BC3_SRGB: return DXGI_FORMAT_BC3_UNORM_SRGB;
		case GFX_PF_BC4: return DXGI_FORMAT_BC4_UNORM;
		case GFX_PF_BC5: return DXGI_FORMAT_BC5_UNORM;
		case GFX_PF_BC5S: return DXGI_FORMAT_BC5_SNORM;
		case GFX_PF_BC6H: return DXGI_FORMAT_BC6H_UF16;
		case GFX_PF_BC6HS: return DXGI_FORMAT_BC6H_SF16;
		case GFX_PF_BC7: return DXGI_FORMAT_BC7_UNORM;
		case GFX_PF_BC7_SRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;
		case GFX_PF_R8G8B8A8_SNORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
		case GFX_PF_R1: return DXGI_FORMAT_R1_UNORM;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	std::optional<std::string> get_image_pixels(const GfxImage* image)
	{
		if (!(image->flags & IMG_DISK_FLAG_STREAMED))
		{
			if (!image->pixels.residentData)
			{
				return {};
			}

			return std::string(image->pixels.residentData, image->totalSize);
		}

		std::string pixels;
		for (auto i = static_cast<int>(std::size(image->streams)) - 1; i >= 0; i--)
		{
			if (!image->streams[i].xpakEntry.key)
			{
				continue;
			}

			const auto part = get_image_stream_pixels(image, i);
			if (!part)
			{
				return {};
			}

			pixels.append(*part);
		}

		if (pixels.empty())
		{
			return {};
		}

		return pixels;
	}

	std::optional<std::string> get_image_stream_pixels(const GfxImage* image, const int stream)
	{
		const auto& data = image->streams[stream];
		if (!data.xpakEntry.key)
		{
			return {};
		}

		const auto smaller_parts_size = stream > 0 ? image->streams[stream - 1].pixelSize : 0;
		return xpak::read(data.xpakEntry.key, data.pixelSize - smaller_parts_size);
	}

	void gfx_image::read(zone_reader& reader, GfxImage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->packedAtlasData, 0, packed_atlas_data_size(asset));

		if (const auto fallback = reader.read_single(asset->fallback, 7))
		{
			reader.read_array(fallback->pixels, 15, fallback->size);
		}

		if (!(asset->flags & IMG_DISK_FLAG_STREAMED))
		{
			read_resident_pixels(reader, asset);
		}
		reader.pop_stream();
	}

	void gfx_image::dump(GfxImage* asset)
	{
		const auto streamed = (asset->flags & IMG_DISK_FLAG_STREAMED) != 0;
		const auto dump_dds_file = utils::flags::has_flag("dds");

		std::optional<std::string> pixels;
		if (!streamed || dump_dds_file || utils::flags::has_flag("dump_streamed_image"))
		{
			pixels = get_image_pixels(asset);
			if (streamed && !pixels)
			{
				ZONETOOL_WARNING("Cannot find the streamed pixels of image \"%s\"", asset->name);
			}
		}

		if (dump_dds_file && pixels)
		{
			dump_dds(asset, *pixels);
		}

		assetmanager::dumper write;
		if (!write.open("images\\" + clean_name(asset->name) + ".iw8Image"))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->packedAtlasData, static_cast<std::uint32_t>(packed_atlas_data_size(asset)));
		write.dump_single(asset->fallback);
		if (asset->fallback)
		{
			write.dump_array(asset->fallback->pixels, asset->fallback->size);
		}

		write.dump_array(pixels ? pixels->data() : nullptr, pixels ? static_cast<std::uint32_t>(pixels->size()) : 0);
		write.close();
	}
}
