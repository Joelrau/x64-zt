#include <std_include.hpp>
#include "s2.hpp"

#include <io/game_files.hpp>
#include <xfile/xfile.hpp>
#include "../zone/image_pak.hpp"
#include <utils/flags.hpp>
#include <utils/io.hpp>

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

namespace zonetool::s2
{
	namespace
	{
		using image_pak::toc_entry;

		struct image_stream_location
		{
			std::filesystem::path pak;
			std::uint64_t offset;
			std::uint32_t size;
		};

		struct image_stream_entry
		{
			std::filesystem::path folder;
			toc_entry entry;
		};

		void read_toc(const std::filesystem::path& path, std::unordered_map<std::uint64_t, image_stream_entry>& entries)
		{
			std::string data;
			if (!game_files::read(path, data) || data.empty())
			{
				return;
			}

			const auto toc = image_pak::parse_toc(data);
			if (!toc)
			{
				ZONETOOL_WARNING("Image toc \"%s\" is not valid", path.string().data());
				return;
			}

			for (const auto& entry : *toc)
			{
				entries.try_emplace(entry.hash[0], path.parent_path(), entry);
			}
		}

		const std::unordered_map<std::uint64_t, image_stream_entry>& image_stream_entries()
		{
			static const auto entries = []
			{
				std::unordered_map<std::uint64_t, image_stream_entry> result;
				for (const auto& name : game_files::get().list(".toc"))
				{
					if (std::filesystem::path(name).filename().string().starts_with("imagefile"))
					{
						read_toc(game_files::get().path(name), result);
					}
				}

				return result;
			}();

			return entries;
		}

		std::optional<image_stream_location> find_zone_stream(const XStreamFileLocation& location)
		{
			const auto in_fastfile = location.fileIndex == XStreamFileLocation::fastfile_index;
			if (!location.size || (!in_fastfile && location.fileIndex != XStreamFileLocation::zone_pak_index))
			{
				return {};
			}

			return image_stream_location
			{
				.pak = filesystem::get_zone_folder() + (in_fastfile
					? filesystem::get_fastfile() + ".ff" : image_pak::get_pak_name(filesystem::get_fastfile())),
				.offset = location.offset,
				.size = location.size,
			};
		}

		std::optional<image_stream_location> find_image_stream(const XStreamFileKey& key)
		{
			if (key.slot != XStreamFileKey::unresolved_slot)
			{
				return find_zone_stream(std::bit_cast<XStreamFileLocation>(key));
			}

			const auto& entries = image_stream_entries();
			const auto entry = entries.find(key.hash);
			if (entry == entries.end() || key.index >= std::size(entry->second.entry.streams))
			{
				return {};
			}

			const auto& stream = entry->second.entry.streams[key.index];
			if (!stream.size)
			{
				return {};
			}

			return image_stream_location
			{
				.pak = entry->second.folder / std::format("imagefile{}.pak", stream.file_index),
				.offset = stream.offset,
				.size = stream.size,
			};
		}
	}

	std::optional<std::string> read_image_stream(const XStreamFileKey& key)
	{
		const auto location = find_image_stream(key);
		if (!location)
		{
			return {};
		}

		std::string data;
		data.resize(location->size);
		if (!game_files::read(location->pak, location->offset, location->size, data.data()))
		{
			return {};
		}

		return xfile::decompress_s2_stream(data);
	}

	namespace
	{
		constexpr DXGI_FORMAT image_formats[]
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

		constexpr std::uint8_t default_image_semantic = 2;
		constexpr std::uint8_t image_category_load_from_file = 3;

		DXGI_FORMAT dxgi_format_from_index(const std::uint32_t index)
		{
			return index < std::size(image_formats) ? image_formats[index] : DXGI_FORMAT_UNKNOWN;
		}

		std::optional<std::uint32_t> index_from_dxgi_format(const DXGI_FORMAT format)
		{
			for (auto i = 1u; i < std::size(image_formats); i++)
			{
				if (image_formats[i] == format)
				{
					return i;
				}
			}

			return {};
		}

		std::string clean_image_name(std::string name)
		{
			std::replace(name.begin(), name.end(), '*', '_');
			return name;
		}

		bool is_streamed_image(const GfxImage* image)
		{
			return image->streamed != 0;
		}

		std::optional<DirectX::ScratchImage> load_image_file(const std::string& name)
		{
			auto path = "images\\" + clean_image_name(name);
			if (filesystem::file(path + ".dds").exists())
			{
				path.append(".dds");
			}
			else if (filesystem::file(path + ".tga").exists())
			{
				path.append(".tga");
			}
			else
			{
				return {};
			}

			const auto wide_path = utils::string::convert(filesystem::get_file_path(path) + path);

			DirectX::ScratchImage image;
			const auto result = path.ends_with(".dds")
				? DirectX::LoadFromDDSFile(wide_path.data(), DirectX::DDS_FLAGS_NONE, nullptr, image)
				: DirectX::LoadFromTGAFile(wide_path.data(), nullptr, image);

			if (FAILED(result))
			{
				return {};
			}

			return image;
		}

		GfxImage* make_loaded_image(const std::string& name, const DirectX::ScratchImage& source, zone_memory* mem)
		{
			const auto& metadata = source.GetMetadata();
			const auto format = index_from_dxgi_format(metadata.format);
			if (!format)
			{
				ZONETOOL_WARNING("Image \"%s\" has a format that S2 does not support", name.data());
				return nullptr;
			}

			auto* image = mem->allocate<GfxImage>();
			image->name = mem->duplicate_string(name);
			image->imageFormat = static_cast<DXGI_FORMAT>(*format);
			image->width = static_cast<unsigned short>(metadata.width);
			image->height = static_cast<unsigned short>(metadata.height);
			image->depth = static_cast<unsigned short>(metadata.depth);
			image->levelCount = static_cast<unsigned char>(metadata.mipLevels);
			image->semantic = default_image_semantic;
			image->category = image_category_load_from_file;

			if (metadata.IsCubemap())
			{
				image->mapType = MAPTYPE_CUBE;
				image->numElements = static_cast<unsigned short>(metadata.arraySize / 6);
			}
			else if (metadata.arraySize > 1)
			{
				image->mapType = MAPTYPE_ARRAY;
				image->numElements = static_cast<unsigned short>(metadata.arraySize);
			}
			else
			{
				image->mapType = static_cast<MapType>(metadata.dimension);
				image->numElements = 1;
			}

			image->dataLen1 = static_cast<unsigned int>(source.GetPixelsSize());
			image->pixelData = mem->allocate<unsigned char>(source.GetPixelsSize());
			std::memcpy(image->pixelData, source.GetPixels(), source.GetPixelsSize());

			return image;
		}

		std::string streamed_image_path(const std::string& name, const std::size_t stream)
		{
			return std::format("streamed_images\\{}_stream{}.pixels", clean_image_name(name), stream);
		}

		bool is_toc_stream(const XStreamFileKey& key)
		{
			return key.slot == XStreamFileKey::unresolved_slot;
		}

		bool has_fastfile_streams(const GfxImage* image)
		{
			return std::ranges::any_of(get_image_stream_keys(image), [](const XStreamFileKey& key)
			{
				return !is_toc_stream(key) && find_zone_stream(std::bit_cast<XStreamFileLocation>(key));
			});
		}

		bool save_dds(const std::string& relative_path, const DirectX::Image* images, const std::size_t count, const DirectX::TexMetadata& metadata)
		{
			const auto full_path = filesystem::get_dump_path() + relative_path;
			std::filesystem::create_directories(std::filesystem::path(full_path).parent_path());
			return SUCCEEDED(DirectX::SaveToDDSFile(images, count, metadata, DirectX::DDS_FLAGS_NONE,
				utils::string::convert(full_path).data()));
		}

		void dump_loaded_image_dds(const GfxImage* image)
		{
			const auto format = dxgi_format_from_index(image->imageFormat);
			const auto sides = image->mapType == MAPTYPE_CUBE ? 6 : 1;

			std::vector<DirectX::Image> images;
			auto* data = image->pixelData;
			std::size_t data_used = 0;
			for (auto element = 0; element < image->numElements; element++)
			{
				for (auto side = 0; side < sides; side++)
				{
					for (auto depth = 0; depth < image->depth; depth++)
					{
						for (auto level = 0; level < image->levelCount; level++)
						{
							DirectX::Image mip{};
							mip.pixels = data;
							mip.width = std::max(1, image->width >> level);
							mip.height = std::max(1, image->height >> level);
							mip.format = format;
							DirectX::ComputePitch(format, mip.width, mip.height, mip.rowPitch, mip.slicePitch);

							images.push_back(mip);
							data += mip.slicePitch;
							data_used += mip.slicePitch;
						}
					}
				}
			}

			if (data_used != image->dataLen1)
			{
				ZONETOOL_WARNING("Failed to dump image \"%s.dds\"", image->name);
				return;
			}

			DirectX::TexMetadata metadata{};
			metadata.width = image->width;
			metadata.height = image->height;
			metadata.depth = image->depth;
			metadata.arraySize = image->numElements * sides;
			metadata.mipLevels = image->levelCount;
			metadata.format = format;
			metadata.dimension = image->mapType > MAPTYPE_CUBE ? DirectX::TEX_DIMENSION_TEXTURE2D
				: static_cast<DirectX::TEX_DIMENSION>(image->mapType);
			if (image->mapType == MAPTYPE_CUBE)
			{
				metadata.miscFlags |= DirectX::TEX_MISC_TEXTURECUBE;
			}

			if (!save_dds("images\\" + clean_image_name(image->name) + ".dds", images.data(), images.size(), metadata))
			{
				ZONETOOL_WARNING("Failed to dump image \"%s.dds\"", image->name);
			}
		}

		void dump_streamed_image_pixels(const GfxImage* image, const bool dump_dds)
		{
			const auto keys = get_image_stream_keys(image);
			for (auto i = 0u; i < image_stream_count && i < keys.size(); i++)
			{
				if (!image->streams[i].width)
				{
					continue;
				}

				const auto pixels = read_image_stream(keys[i]);
				if (!pixels)
				{
					ZONETOOL_WARNING("Stream %u of image \"%s\" is not in the game files", i, image->name);
					continue;
				}

				if (!dump_dds)
				{
					dump_image_stream(image, i, *pixels);
					continue;
				}

				const auto format = dxgi_format_from_index(image->imageFormat);
				DirectX::Image mip{};
				mip.pixels = reinterpret_cast<std::uint8_t*>(const_cast<char*>(pixels->data()));
				mip.width = image->streams[i].width;
				mip.height = image->streams[i].height;
				mip.format = format;
				DirectX::ComputePitch(format, mip.width, mip.height, mip.rowPitch, mip.slicePitch);

				DirectX::TexMetadata metadata{};
				metadata.width = mip.width;
				metadata.height = mip.height;
				metadata.depth = 1;
				metadata.arraySize = 1;
				metadata.mipLevels = 1;
				metadata.format = format;
				metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;

				const auto path = std::format("streamed_images\\{}_stream{}.dds", clean_image_name(image->name), i);
				if (!save_dds(path, &mip, 1, metadata))
				{
					ZONETOOL_WARNING("Failed to dump image \"%s\"", path.data());
				}
			}
		}

		void dump_image_header(GfxImage* image, const std::string& path)
		{
			assetmanager::dumper write;
			if (!write.open(path))
			{
				return;
			}

			write.dump_single(image);
			write.dump_string(image->name);
			if (image->pixelData && image->dataLen1)
			{
				write.dump_array(image->pixelData, image->dataLen1);
			}

			if (is_streamed_image(image))
			{
				// separate storage per key, the dumper writes a pointer it has seen before as a reference to the first one
				std::array<XStreamFileKey, image_stream_count> keys{};
				std::ranges::copy(get_image_stream_keys(image) | std::views::take(image_stream_count), keys.begin());
				for (auto& key : keys)
				{
					write.dump_single(&key);
				}
			}

			write.close();
		}
	}

	void dump_image_stream(const GfxImage* image, const std::size_t stream, const std::string_view pixels)
	{
		utils::io::write_file(filesystem::get_dump_path() + streamed_image_path(image->name, stream), std::string(pixels), false);
	}

	GfxImage* gfx_image::parse_custom(const std::string& name, zone_memory* mem)
	{
		const auto source = load_image_file(name);
		if (!source)
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing custom image \"%s\"", name.data());
		return make_loaded_image(name, *source, mem);
	}

	GfxImage* gfx_image::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (read.open("images\\" + clean_image_name(name) + ".s2Image"))
		{
			ZONETOOL_INFO("Parsing image \"%s\"...", name.data());

			auto* asset = read.read_single<GfxImage>();
			asset->name = read.read_string();
			asset->pixelData = asset->pixelData && asset->dataLen1 ? read.read_array<unsigned char>() : nullptr;

			// backwards compatibility with older h1_s2 images that copied the second H1 picmip byte into streamed
			if (asset->streamed && !asset->streams[0].width)
			{
				asset->streamed = 0;
			}

			if (is_streamed_image(asset))
			{
				for (auto i = 0u; i < image_stream_count; i++)
				{
					auto& key = this->parsed_stream_keys_[i];
					key = *read.read_single<XStreamFileKey>();

					// older dumps wrote keys 1-3 as references to key 0, a toc key always looks up its own stream of the entry
					if (is_toc_stream(key))
					{
						key.index = static_cast<std::uint16_t>(i);
					}
				}
				this->has_parsed_stream_keys_ = true;
				this->find_stream_pixels(asset);
			}
			read.close();

			std::memset(&asset->texture, 0, sizeof(asset->texture));
			return asset;
		}

		return this->parse_custom(name, mem);
	}

	void gfx_image::find_stream_pixels(const GfxImage* asset)
	{
		for (auto i = 0u; i < image_stream_count; i++)
		{
			if (!asset->streams[i].width || is_toc_stream(this->parsed_stream_keys_[i]))
			{
				continue;
			}

			this->parsed_stream_keys_[i] = {};

			const auto path = streamed_image_path(asset->name, i);
			if (!filesystem::file(path).exists())
			{
				ZONETOOL_WARNING("Stream %u of image \"%s\" has no \"%s\"", i, asset->name, path.data());
				continue;
			}

			this->stream_pixel_paths_[i] = filesystem::get_file_path(path) + path;
		}
	}

	std::span<const XStreamFileKey> gfx_image::stream_keys() const
	{
		if (this->has_parsed_stream_keys_)
		{
			return this->parsed_stream_keys_;
		}

		return get_image_stream_keys(this->asset_);
	}

	void gfx_image::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);
		std::memset(&dest->texture, 0, sizeof(dest->texture));

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);

		if (data->semantic == TS_RESIDENT)
		{
			if (data->pixelData)
			{
				buf->align(0);
				buf->write(data->pixelData, data->dataLen1);
				buf->clear_pointer(&dest->pixelData);
			}
		}
		else
		{
			buf->push_stream(XFILE_BLOCK_TEMP);
			if (data->pixelData && data->dataLen1)
			{
				buf->align(0);
				buf->write(data->pixelData, data->dataLen1);
				buf->clear_pointer(&dest->pixelData);
			}
			else
			{
				dest->pixelData = nullptr;
			}
			buf->pop_stream();
		}

		buf->pop_stream();
	}

	void gfx_image::dump(GfxImage* asset)
	{
		const auto streamed = is_streamed_image(asset);
		const auto dump_dds = utils::flags::has_flag("dds");
		const auto dump_pixels = streamed && (has_fastfile_streams(asset) || (!dump_dds && utils::flags::has_flag("dump_streamed_image")));

		if (dump_dds)
		{
			if (streamed)
			{
				dump_streamed_image_pixels(asset, true);
			}
			else if (asset->pixelData)
			{
				dump_loaded_image_dds(asset);
			}
		}

		if (dump_pixels)
		{
			dump_streamed_image_pixels(asset, false);
		}

		dump_image_header(asset, "images\\" + clean_image_name(asset->name) + ".s2Image");
	}
}
