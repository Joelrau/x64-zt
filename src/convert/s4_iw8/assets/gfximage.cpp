#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"

#include <utils/flags.hpp>

#include <formats/image_compress.hpp>

#include <DirectXTex.h>

namespace zonetool::s4::converter::iw8::gfximage
{
	namespace
	{
		namespace target = zonetool::iw8;

		std::size_t get_max_image_size()
		{
			static const auto size = std::stoull(utils::flags::get_flag("max_image_size").value_or("0"));
			return size;
		}
	}

	std::optional<target::GfxPixelFormat> convert_format(const GfxPixelFormat format)
	{
		switch (format)
		{
		case GFX_PF_R8: return target::GFX_PF_R8;
		case GFX_PF_A8: return target::GFX_PF_A8;
		case GFX_PF_L8: return target::GFX_PF_L8;
		case GFX_PF_R8G8: return target::GFX_PF_R8G8;
		case GFX_PF_L8A8: return target::GFX_PF_L8A8;
		case GFX_PF_R8G8B8A8: return target::GFX_PF_R8G8B8A8;
		case GFX_PF_R8G8B8A8_SRGB: return target::GFX_PF_R8G8B8A8_SRGB;
		case GFX_PF_R8_SNORM: return target::GFX_PF_R8_SNORM;
		case GFX_PF_R8G8_SNORM: return target::GFX_PF_R8G8_SNORM;
		case GFX_PF_R16: return target::GFX_PF_R16;
		case GFX_PF_R16G16: return target::GFX_PF_R16G16;
		case GFX_PF_R16G16B16A16: return target::GFX_PF_R16G16B16A16;
		case GFX_PF_R16_SNORM: return target::GFX_PF_R16_SNORM;
		case GFX_PF_R16F: return target::GFX_PF_R16F;
		case GFX_PF_R16G16F: return target::GFX_PF_R16G16F;
		case GFX_PF_R16G16B16A16F: return target::GFX_PF_R16G16B16A16F;
		case GFX_PF_R32F: return target::GFX_PF_R32F;
		case GFX_PF_R32G32F: return target::GFX_PF_R32G32F;
		case GFX_PF_R32G32B32A32F: return target::GFX_PF_R32G32B32A32F;
		case GFX_PF_D16: return target::GFX_PF_D16;
		case GFX_PF_D32F: return target::GFX_PF_D32F;
		case GFX_PF_D32F_S8: return target::GFX_PF_D32F_S8;
		case GFX_PF_R8_UINT: return target::GFX_PF_R8_UINT;
		case GFX_PF_R16_UINT: return target::GFX_PF_R16_UINT;
		case GFX_PF_R32_UINT: return target::GFX_PF_R32_UINT;
		case GFX_PF_R32G32_UINT: return target::GFX_PF_R32G32_UINT;
		case GFX_PF_R32G32B32A32_UINT: return target::GFX_PF_R32G32B32A32_UINT;
		case GFX_PF_R10G10B10A2_UINT: return target::GFX_PF_R10G10B10A2_UINT;
		case GFX_PF_R5G6B5: return target::GFX_PF_R5G6B5;
		case GFX_PF_R10G10B10A2: return target::GFX_PF_R10G10B10A2;
		case GFX_PF_R9G9B9E5_SHAREDEXP: return target::GFX_PF_R9G9B9E5_SHAREDEXP;
		case GFX_PF_R11G11B10F: return target::GFX_PF_R11G11B10F;
		case GFX_PF_BC1: return target::GFX_PF_BC1;
		case GFX_PF_BC1_SRGB: return target::GFX_PF_BC1_SRGB;
		case GFX_PF_BC2: return target::GFX_PF_BC2;
		case GFX_PF_BC2_SRGB: return target::GFX_PF_BC2_SRGB;
		case GFX_PF_BC3: return target::GFX_PF_BC3;
		case GFX_PF_BC3_SRGB: return target::GFX_PF_BC3_SRGB;
		case GFX_PF_BC4: return target::GFX_PF_BC4;
		case GFX_PF_BC5: return target::GFX_PF_BC5;
		case GFX_PF_BC5S: return target::GFX_PF_BC5S;
		case GFX_PF_BC6H: return target::GFX_PF_BC6H;
		case GFX_PF_BC6HS: return target::GFX_PF_BC6HS;
		case GFX_PF_BC7: return target::GFX_PF_BC7;
		case GFX_PF_BC7_SRGB: return target::GFX_PF_BC7_SRGB;
		default: return {};
		}
	}

	namespace
	{
		std::optional<std::size_t> get_level_size(const GfxImage* asset, const DXGI_FORMAT format, const std::uint32_t level)
		{
			std::size_t row_pitch{};
			std::size_t slice_pitch{};
			if (FAILED(DirectX::ComputePitch(format, std::max(1, asset->width >> level), std::max(1, asset->height >> level), row_pitch, slice_pitch)))
			{
				return {};
			}

			const auto is_3d = (asset->flags & IMG_DISK_FLAG_MAPTYPE_MASK) == IMG_DISK_FLAG_MAPTYPE_3D;
			const auto size = slice_pitch * (is_3d ? std::max(1, asset->depth >> level) : 1);
			return (size + 15) & ~static_cast<std::size_t>(15);
		}

		std::uint32_t get_first_level(const GfxImage* asset)
		{
			const auto max_size = get_max_image_size();
			if (!max_size)
			{
				return 0;
			}

			auto level = 0u;
			while (level + 1 < asset->levelCount && std::max(asset->width >> level, asset->height >> level) > static_cast<int>(max_size))
			{
				level++;
			}

			return level;
		}

		std::optional<std::string> drop_levels(const GfxImage* asset, const std::string& pixels, const std::uint32_t base_level, const std::uint32_t first_level)
		{
			const auto format = get_dxgi_format(asset->format);
			if (format == DXGI_FORMAT_UNKNOWN)
			{
				return {};
			}

			std::string result;
			std::size_t offset{};
			for (auto item = 0u; item < get_image_item_count(asset); item++)
			{
				for (auto level = base_level; level < std::max<std::uint32_t>(1, asset->levelCount); level++)
				{
					const auto size = get_level_size(asset, format, level);
					if (!size || offset + *size > pixels.size())
					{
						return {};
					}

					if (level >= first_level)
					{
						result.append(pixels, offset, *size);
					}

					offset += *size;
				}
			}

			if (offset != pixels.size())
			{
				return {};
			}

			return result;
		}

		std::optional<std::uint32_t> find_base_level(const GfxImage* asset, const std::size_t size)
		{
			const auto format = get_dxgi_format(asset->format);
			if (format == DXGI_FORMAT_UNKNOWN)
			{
				return {};
			}

			const auto level_count = std::max<std::uint32_t>(1, asset->levelCount);
			std::size_t levels_size{};
			for (auto level = level_count; level > 0; level--)
			{
				const auto level_size = get_level_size(asset, format, level - 1);
				if (!level_size)
				{
					return {};
				}

				levels_size += *level_size;
				if (levels_size == size)
				{
					return level - 1;
				}
			}

			return {};
		}

		std::optional<std::string> read_available_streams(const GfxImage* asset, std::uint32_t& base_level)
		{
			if (!(asset->flags & IMG_DISK_FLAG_STREAMED) || !asset->streams || get_image_item_count(asset) != 1)
			{
				return {};
			}

			std::string pixels;
			for (auto i = static_cast<int>(asset->streamCount) - 1; i >= 0; i--)
			{
				if (const auto part = get_image_stream_pixels(asset, i))
				{
					pixels.append(*part);
				}
				else if (!pixels.empty())
				{
					return {};
				}
			}

			const auto level = find_base_level(asset, pixels.size());
			if (pixels.empty() || !level || !*level)
			{
				return {};
			}

			base_level = *level;
			return pixels;
		}

		std::optional<DirectX::ScratchImage> read_top_level(const GfxImage* asset)
		{
			auto base_level = 0u;
			auto pixels = get_image_pixels(asset);
			if (!pixels)
			{
				pixels = read_available_streams(asset, base_level);
			}

			const auto format = get_dxgi_format(asset->format);
			if (!pixels || format == DXGI_FORMAT_UNKNOWN || get_image_item_count(asset) != 1)
			{
				return {};
			}

			DirectX::Image level{};
			level.width = std::max(1, asset->width >> base_level);
			level.height = std::max(1, asset->height >> base_level);
			level.format = format;
			level.pixels = reinterpret_cast<std::uint8_t*>(pixels->data());
			if (FAILED(DirectX::ComputePitch(format, level.width, level.height, level.rowPitch, level.slicePitch)) || level.slicePitch > pixels->size())
			{
				return {};
			}

			DirectX::ScratchImage decoded;
			const auto result = DirectX::IsCompressed(format)
				? DirectX::Decompress(level, DXGI_FORMAT_R8G8B8A8_UNORM, decoded)
				: DirectX::Convert(level, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, decoded);
			if (FAILED(result))
			{
				return {};
			}

			return decoded;
		}

		bool resize(DirectX::ScratchImage& image, const std::size_t width, const std::size_t height)
		{
			if (image.GetMetadata().width == width && image.GetMetadata().height == height)
			{
				return true;
			}

			DirectX::ScratchImage resized;
			if (FAILED(DirectX::Resize(*image.GetImage(0, 0, 0), width, height, DirectX::TEX_FILTER_LINEAR | DirectX::TEX_FILTER_FORCE_NON_WIC, resized)))
			{
				return false;
			}

			image = std::move(resized);
			return true;
		}

		std::optional<DirectX::ScratchImage> compose_alpha_reveal(const GfxImage* alpha, const GfxImage* reveal)
		{
			auto reveal_pixels = read_top_level(reveal);
			auto alpha_pixels = alpha ? read_top_level(alpha) : std::optional<DirectX::ScratchImage>();
			if (!reveal_pixels || (alpha && !alpha_pixels))
			{
				return {};
			}

			const auto width = std::max(reveal_pixels->GetMetadata().width, alpha ? alpha_pixels->GetMetadata().width : 0);
			const auto height = std::max(reveal_pixels->GetMetadata().height, alpha ? alpha_pixels->GetMetadata().height : 0);
			DirectX::ScratchImage composed;
			if (!resize(*reveal_pixels, width, height) || (alpha && !resize(*alpha_pixels, width, height))
				|| FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return {};
			}

			const auto alpha_channel = alpha && DirectX::HasAlpha(get_dxgi_format(alpha->format)) ? 3 : 0;
			const auto* alpha_image = alpha ? alpha_pixels->GetImage(0, 0, 0) : nullptr;
			const auto& reveal_image = *reveal_pixels->GetImage(0, 0, 0);
			const auto& composed_image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					auto* texel = composed_image.pixels + row * composed_image.rowPitch + column * 4;
					texel[0] = alpha_image ? alpha_image->pixels[row * alpha_image->rowPitch + column * 4 + alpha_channel] : 0xFF;
					texel[1] = reveal_image.pixels[row * reveal_image.rowPitch + column * 4];
					texel[2] = 0;
					texel[3] = 0xFF;
				}
			}

			DirectX::ScratchImage levels;
			if (FAILED(DirectX::GenerateMipMaps(composed_image, DirectX::TEX_FILTER_BOX | DirectX::TEX_FILTER_FORCE_NON_WIC, 0, levels)))
			{
				return {};
			}

			return image_compress::compress(levels, DXGI_FORMAT_BC1_UNORM, DirectX::TEX_COMPRESS_DEFAULT);
		}

		target::GfxDecalAtlasIndex get_channel_atlas_index(const std::size_t channel)
		{
			switch (channel)
			{
			case 0:
				return target::DECAL_ATLAS_BC7_SRGB;
			case 1:
				return target::DECAL_ATLAS_BC7;
			case 2:
			case 5:
				return target::DECAL_ATLAS_BC4;
			default:
				return target::DECAL_ATLAS_NONE;
			}
		}

		bool is_image(const GfxImage* image, const std::string_view name)
		{
			return image && image->name && image->name == name;
		}

		target::GfxDecalAtlasIndex find_decal_atlas_index(const GfxImage* asset)
		{
			std::set<target::GfxDecalAtlasIndex> indices;
			db_enum_assets(ASSET_TYPE_DECAL_VOLUME_MASK, [&](void* header)
			{
				if (is_image(static_cast<const GfxDecalVolumeMask*>(header)->blendMap, asset->name))
				{
					indices.insert(target::DECAL_ATLAS_BC4);
				}
			});

			db_enum_assets(ASSET_TYPE_DECAL_VOLUME_MATERIAL, [&](void* header)
			{
				const auto* material = static_cast<const GfxDecalVolumeMaterial*>(header);
				for (auto i = 0u; i < std::size(material->channels); i++)
				{
					if (is_image(material->channels[i], asset->name))
					{
						indices.insert(get_channel_atlas_index(i));
					}
				}
			});

			if (indices.size() > 1)
			{
				ZONETOOL_WARNING("Image \"%s\" is in more than one decal atlas", asset->name);
			}

			return indices.empty() ? target::DECAL_ATLAS_NONE : *indices.begin();
		}

		std::optional<std::string> convert_bc1_to_bc7(const std::string& pixels, const std::size_t width, const std::size_t height, const std::size_t level_count)
		{
			DirectX::ScratchImage decoded;
			if (FAILED(decoded.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, level_count)))
			{
				return {};
			}

			std::size_t offset{};
			for (auto level = 0u; level < level_count; level++)
			{
				DirectX::Image source{};
				source.width = std::max<std::size_t>(1, width >> level);
				source.height = std::max<std::size_t>(1, height >> level);
				source.format = DXGI_FORMAT_BC1_UNORM;
				source.pixels = reinterpret_cast<std::uint8_t*>(const_cast<char*>(pixels.data() + offset));
				if (FAILED(DirectX::ComputePitch(source.format, source.width, source.height, source.rowPitch, source.slicePitch))
					|| offset + source.slicePitch > pixels.size())
				{
					return {};
				}

				DirectX::ScratchImage level_pixels;
				if (FAILED(DirectX::Decompress(source, DXGI_FORMAT_R8G8B8A8_UNORM, level_pixels)))
				{
					return {};
				}

				std::memcpy(decoded.GetImage(level, 0, 0)->pixels, level_pixels.GetPixels(), level_pixels.GetPixelsSize());
				offset += (source.slicePitch + 15) & ~static_cast<std::size_t>(15);
			}

			const auto compressed = image_compress::compress(decoded, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_DEFAULT);
			if (offset != pixels.size() || !compressed)
			{
				return {};
			}

			std::string result;
			for (auto level = 0u; level < level_count; level++)
			{
				const auto& image = *compressed->GetImage(level, 0, 0);
				result.append(reinterpret_cast<const char*>(image.pixels), image.slicePitch);
				result.resize((result.size() + 15) & ~static_cast<std::size_t>(15));
			}

			return result;
		}

		target::TextureSemantic convert_semantic(const TextureSemantic semantic)
		{
			switch (semantic)
			{
			case TS_FUNCTION:
			case TS_SKY:
				return target::TS_FUNCTION;
			case TS_COLOR_MAP:
				return target::TS_COLOR_MAP;
			case TS_GRAY_MAP:
				return target::TS_GRAY_MAP;
			case TS_SIGNED_VELOCITY_MAP:
				return target::TS_SIGNED_VELOCITY_MAP;
			case TS_NORMAL_MAP:
				return target::TS_NORMAL_MAP;
			case TS_METALNESS_MAP:
				return target::TS_METALNESS_MAP;
			case TS_NORMAL_OCCLUSION_GLOSS_MAP:
				return target::TS_NORMAL_OCCLUSION_GLOSS_MAP;
			case TS_SIGNED_DISTANCE_FIELD:
				return target::TS_SIGNED_DISTANCE_FIELD;
			case TS_CARD_IMPOSTER_NORMAL:
				return target::TS_CARD_IMPOSTER_NORMAL;
			default:
				return target::TS_2D;
			}
		}
	}

	target::GfxImage* convert(const GfxImage* asset, utils::memory::allocator& allocator)
	{
		const auto format = convert_format(asset->format);
		if (!format)
		{
			ZONETOOL_WARNING("Image \"%s\" has the format %d, IW8 has no equivalent", asset->name, asset->format);
			return nullptr;
		}

		auto base_level = 0u;
		auto pixels = get_image_pixels(asset);
		if (!pixels)
		{
			pixels = read_available_streams(asset, base_level);
		}

		if (!pixels)
		{
			ZONETOOL_WARNING("Cannot read the pixels of image \"%s\"", asset->name);
			return nullptr;
		}

		auto first_level = std::max(base_level, get_first_level(asset));
		if (first_level)
		{
			if (auto trimmed = drop_levels(asset, *pixels, base_level, first_level))
			{
				pixels = std::move(trimmed);
			}
			else if (base_level)
			{
				ZONETOOL_WARNING("Cannot read the pixels of image \"%s\"", asset->name);
				return nullptr;
			}
			else
			{
				ZONETOOL_WARNING("Cannot reduce the size of image \"%s\"", asset->name);
				first_level = 0;
			}
		}

		if (base_level)
		{
			ZONETOOL_WARNING("Image \"%s\" has no data on disk for its top %u levels, IW8 gets the rest", asset->name, base_level);
		}

		const auto is_3d = (asset->flags & IMG_DISK_FLAG_MAPTYPE_MASK) == IMG_DISK_FLAG_MAPTYPE_3D;

		const auto result = allocator.allocate<target::GfxImage>();
		result->name = allocator.duplicate_string(asset->name);
		result->format = *format;
		result->flags = static_cast<target::GfxImageFlags>(asset->flags & ~(IMG_DISK_FLAG_STREAMED | IMG_DISK_FLAG_USE_OODLE_COMPRESSION));
		result->totalSize = static_cast<unsigned int>(pixels->size());
		result->semanticSpecific.maxMipMap = asset->semanticSpecific.maxMipMap;
		result->width = static_cast<unsigned short>(std::max(1, asset->width >> first_level));
		result->height = static_cast<unsigned short>(std::max(1, asset->height >> first_level));
		result->depth = static_cast<unsigned short>(is_3d ? std::max(1, asset->depth >> first_level) : asset->depth);
		result->numElements = asset->numElements;
		result->semantic = convert_semantic(static_cast<TextureSemantic>(asset->semantic));
		result->category = static_cast<target::GfxImageCategory>(asset->category);
		result->unknown = asset->unknown3;
		result->levelCount = static_cast<unsigned char>(asset->levelCount - first_level);
		result->decalAtlasIndex = find_decal_atlas_index(asset);

		if (result->decalAtlasIndex == target::DECAL_ATLAS_BC7_SRGB && result->format == target::GFX_PF_BC1_SRGB)
		{
			if (auto bc7 = convert_bc1_to_bc7(*pixels, result->width, result->height, std::max<std::size_t>(1, result->levelCount)))
			{
				pixels = std::move(bc7);
				result->format = target::GFX_PF_BC7_SRGB;
				result->totalSize = static_cast<unsigned int>(pixels->size());
			}
			else
			{
				ZONETOOL_WARNING("Cannot convert the decal image \"%s\" to BC7, IW8 does not draw it", asset->name);
			}
		}

		if (asset->flags & IMG_DISK_FLAG_PACKED_ATLAS)
		{
			result->atlasInfo.packedAtlasDataSize = asset->atlasInfo.packedAtlasDataSize;
			result->packedAtlasData = allocator.allocate_array<char>(asset->atlasInfo.packedAtlasDataSize);
			std::memcpy(result->packedAtlasData, asset->packedAtlasData, asset->atlasInfo.packedAtlasDataSize);
		}
		else
		{
			result->atlasInfo.atlasSize.rowCount = asset->atlasInfo.atlasSize.rowCount;
			result->atlasInfo.atlasSize.colCount = asset->atlasInfo.atlasSize.colCount;
		}

		result->pixels.residentData = allocator.allocate_array<char>(pixels->size());
		std::memcpy(result->pixels.residentData, pixels->data(), pixels->size());
		return result;
	}

	target::GfxImage* image_reference(const GfxImage* asset, utils::memory::allocator& allocator)
	{
		if (!asset)
		{
			return nullptr;
		}

		const auto result = allocator.allocate<target::GfxImage>();
		result->name = asset->name;
		return result;
	}

	target::GfxImage* dump_alpha_reveal(const GfxImage* alpha, const GfxImage* reveal, utils::memory::allocator& allocator)
	{
		static std::unordered_map<std::string, bool> dumped;
		const auto name = std::format("{}&{}", alpha ? alpha->name : "white", reveal->name);
		const auto reference = allocator.allocate<target::GfxImage>();
		reference->name = allocator.duplicate_string(name);
		if (const auto found = dumped.find(name); found != dumped.end())
		{
			return found->second ? reference : nullptr;
		}

		const auto levels = compose_alpha_reveal(alpha, reveal);
		dumped[name] = levels.has_value();
		if (!levels)
		{
			ZONETOOL_WARNING("Cannot build the alpha and reveal image \"%s\"", name.data());
			return nullptr;
		}

		std::string pixels;
		for (auto level = 0u; level < levels->GetMetadata().mipLevels; level++)
		{
			const auto& image = *levels->GetImage(level, 0, 0);
			pixels.append(reinterpret_cast<const char*>(image.pixels), image.slicePitch);
			pixels.resize((pixels.size() + 15) & ~static_cast<std::size_t>(15));
		}

		target::GfxImage result{};
		result.name = reference->name;
		result.format = target::GFX_PF_BC1;
		result.flags = static_cast<target::GfxImageFlags>(reveal->flags & ~(IMG_DISK_FLAG_STREAMED | IMG_DISK_FLAG_USE_OODLE_COMPRESSION));
		result.totalSize = static_cast<unsigned int>(pixels.size());
		result.semanticSpecific.maxMipMap = reveal->semanticSpecific.maxMipMap;
		result.width = static_cast<unsigned short>(levels->GetMetadata().width);
		result.height = static_cast<unsigned short>(levels->GetMetadata().height);
		result.depth = reveal->depth;
		result.numElements = reveal->numElements;
		result.semantic = target::TS_GRAY_MAP;
		result.category = target::IMG_CATEGORY_LOAD_FROM_FILE;
		result.levelCount = static_cast<unsigned char>(levels->GetMetadata().mipLevels);
		result.pixels.residentData = pixels.data();

		target::gfx_image::dump(&result);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_IMAGE), name);
		return reference;
	}

	bool dump(GfxImage* asset)
	{
		static std::unordered_map<std::string, bool> dumped;
		if (const auto found = dumped.find(asset->name); found != dumped.end())
		{
			return found->second;
		}

		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		if (converted)
		{
			target::gfx_image::dump(converted);
			add_csv_row(target::type_to_string(target::ASSET_TYPE_IMAGE), asset->name);
		}

		dumped[asset->name] = converted != nullptr;
		return converted != nullptr;
	}

	bool dump_used(const GfxImage* asset)
	{
		const auto loaded = static_cast<GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, asset->name));
		return loaded && dump(loaded);
	}
}
