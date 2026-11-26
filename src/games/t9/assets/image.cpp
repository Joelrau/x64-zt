#include <std_include.hpp>
#include "image.hpp"
#include "../xpak.hpp"

namespace zonetool::t9
{
	namespace
	{
		std::uint32_t stream_part_end(const GfxImageStreamPart& part)
		{
			return static_cast<std::uint32_t>((part.packedEnd >> 4) & 0x1FFFFFFF);
		}

		bool is_cube(const GfxImage* asset)
		{
			return asset->mapType == MAPTYPE_CUBE || asset->mapType == MAPTYPE_CUBE_ARRAY;
		}

		std::uint32_t face_count(const GfxImage* asset)
		{
			return is_cube(asset) ? 6 : 1;
		}

		std::optional<std::size_t> level_size(const GfxImage* asset, const std::uint32_t level)
		{
			std::size_t row_pitch{};
			std::size_t slice_pitch{};
			if (FAILED(DirectX::ComputePitch(static_cast<DXGI_FORMAT>(asset->format), std::max(1, asset->width >> level),
				std::max(1, asset->height >> level), row_pitch, slice_pitch)))
			{
				return {};
			}

			const auto depth = asset->mapType == MAPTYPE_3D ? std::max(1, asset->depth >> level) : 1;
			return slice_pitch * depth * face_count(asset);
		}

		std::optional<std::uint32_t> find_level_count(const GfxImage* asset)
		{
			std::size_t total{};
			for (auto level = 0u; level < 16; level++)
			{
				const auto bytes = level_size(asset, level);
				if (!bytes)
				{
					return {};
				}

				total += *bytes;
				if (total == asset->pixelSize)
				{
					return level + 1;
				}
			}

			return {};
		}

		std::optional<std::uint32_t> find_first_level(const GfxImage* asset, const std::uint32_t level_count, const std::size_t size)
		{
			std::size_t total{};
			for (auto level = static_cast<std::int32_t>(level_count) - 1; level >= 0; level--)
			{
				const auto bytes = level_size(asset, level);
				if (!bytes)
				{
					return {};
				}

				total += *bytes;
				if (total == size)
				{
					return level;
				}
			}

			return {};
		}

		std::string clean_name(std::string name)
		{
			std::ranges::replace(name, '*', '_');
			std::ranges::replace(name, ':', '_');
			return name;
		}

		std::optional<DirectX::ScratchImage> make_image(const GfxImage* asset, const std::string& name, const std::uint8_t* pixels,
			const std::uint32_t first_level, const std::uint32_t level_count)
		{
			DirectX::TexMetadata metadata{};
			metadata.width = std::max(1, asset->width >> first_level);
			metadata.height = std::max(1, asset->height >> first_level);
			metadata.depth = asset->mapType == MAPTYPE_3D ? std::max(1, asset->depth >> first_level) : 1;
			metadata.arraySize = face_count(asset);
			metadata.mipLevels = level_count - first_level;
			metadata.format = static_cast<DXGI_FORMAT>(asset->format);
			metadata.dimension = asset->mapType == MAPTYPE_3D ? DirectX::TEX_DIMENSION_TEXTURE3D : DirectX::TEX_DIMENSION_TEXTURE2D;
			if (is_cube(asset))
			{
				metadata.miscFlags |= DirectX::TEX_MISC_TEXTURECUBE;
			}

			DirectX::ScratchImage scratch;
			if (FAILED(scratch.Initialize(metadata)))
			{
				ZONETOOL_WARNING("Cannot create the pixels of image \"%s\"", name.data());
				return {};
			}

			std::memcpy(scratch.GetPixels(), pixels, scratch.GetPixelsSize());
			return scratch;
		}
	}

	std::uint32_t stream_part_size(const GfxImage* asset, const std::uint32_t part)
	{
		const auto end = stream_part_end(asset->streamParts[part]);
		return part ? end - stream_part_end(asset->streamParts[part - 1]) : end;
	}

	void image::read(auto& reader, GfxImage* asset)
	{
		if (asset->flags & GFX_IMAGE_FLAG_STREAMED)
		{
			reader.push_stream(XFILE_BLOCK_STREAMER);
			reader.read_array(asset->pixels, 0xFFFF, asset->pixelSize);
			reader.pop_stream();
		}
		else
		{
			reader.push_stream(XFILE_BLOCK_PHYSICAL);
			reader.read_array(asset->pixels, 0xFF, asset->pixelSize);
			reader.pop_stream();
		}

		const auto stream_parts = reader.read_array(asset->streamParts, 7, asset->streamPartCount);
		reader.read_array(asset->firstPartData, 0xFF, stream_parts ? stream_part_size(asset, 0) : 0);
	}

	std::string read_image_pixels(const GfxImage* asset)
	{
		if (!(asset->flags & GFX_IMAGE_FLAG_STREAMED))
		{
			return asset->pixels ? std::string(reinterpret_cast<const char*>(asset->pixels), asset->pixelSize) : std::string{};
		}

		std::vector<std::string> parts;
		for (auto part = 0u; asset->streamParts && part < asset->streamPartCount; part++)
		{
			const auto size = stream_part_size(asset, part);
			if (!part && asset->firstPartData)
			{
				parts.emplace_back(reinterpret_cast<const char*>(asset->firstPartData), size);
				continue;
			}

			try
			{
				auto data = xpak::read(asset->streamParts[part].key, size);
				if (!data)
				{
					ZONETOOL_WARNING("Image \"%s\" stream part %u: key 0x%llX is not in an xpak", xhash_name(asset->name).data(), part,
						asset->streamParts[part].key);
					break;
				}

				parts.emplace_back(std::move(*data));
			}
			catch (const std::exception& error)
			{
				ZONETOOL_WARNING("Image \"%s\" stream part %u key 0x%llX size 0x%X: %s", xhash_name(asset->name).data(), part,
					asset->streamParts[part].key, size, error.what());
				break;
			}
		}

		if (parts.size() < asset->streamPartCount)
		{
			ZONETOOL_WARNING("Image \"%s\" has %zu of %u stream parts", xhash_name(asset->name).data(), parts.size(), asset->streamPartCount);
		}

		std::string pixels;
		for (auto part = parts.rbegin(); part != parts.rend(); ++part)
		{
			pixels += *part;
		}

		return pixels;
	}

	std::optional<DirectX::ScratchImage> load_image(const GfxImage* asset)
	{
		const auto name = xhash_name(asset->name);
		const auto pixels = read_image_pixels(asset);
		const auto level_count = find_level_count(asset);
		const auto first_level = !pixels.empty() && level_count ? find_first_level(asset, *level_count, pixels.size()) : std::nullopt;
		if (!first_level)
		{
			ZONETOOL_WARNING("Image \"%s\" format %u %ux%u map %u flags 0x%X size 0x%X: no level range", name.data(), asset->format,
				asset->width, asset->height, asset->mapType, asset->flags, asset->pixelSize);
			return {};
		}

		return make_image(asset, name, reinterpret_cast<const std::uint8_t*>(pixels.data()), *first_level, *level_count);
	}

	void image::dump(GfxImage* asset)
	{
		const auto image = load_image(asset);
		if (!image)
		{
			return;
		}

		const std::filesystem::path path = filesystem::get_dump_path() + "images\\" + clean_name(xhash_name(asset->name)) + ".dds";
		std::filesystem::create_directories(path.parent_path());
		if (FAILED(DirectX::SaveToDDSFile(image->GetImages(), image->GetImageCount(), image->GetMetadata(), DirectX::DDS_FLAGS_NONE,
			path.wstring().data())))
		{
			ZONETOOL_WARNING("Cannot write \"%s\"", path.string().data());
		}
	}
}

namespace zonetool::t9
{
	template void image::read(zone_reader&, GfxImage*);
	template void image::read(zone_writer&, GfxImage*);
}
