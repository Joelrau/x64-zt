#include <std_include.hpp>
#include "iwi.hpp"
#include "iwi_wavelet.hpp"

#include "..\utils.hpp"

#include "s3tc.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include <utils/string.hpp>

using namespace zonetool;

namespace iwi
{
	namespace
	{
		std::string clean_name(const std::string& name)
		{
			auto new_name = name;
			std::replace(new_name.begin(), new_name.end(), '*', '_');
			return new_name;
		}

		DXGI_FORMAT dxgi_format_from_iwi(const GfxImageFileFormat format)
		{
			switch (format)
			{
			case IMG_FORMAT_DXT1:
				return DXGI_FORMAT_BC1_UNORM;
			case IMG_FORMAT_DXT3:
				return DXGI_FORMAT_BC2_UNORM;
			case IMG_FORMAT_DXT5:
				return DXGI_FORMAT_BC3_UNORM;
			case IMG_FORMAT_BITMAP_RGBA:
			case IMG_FORMAT_BITMAP_RGB:
			case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
			case IMG_FORMAT_BITMAP_LUMINANCE:
			case IMG_FORMAT_WAVELET_RGBA:
			case IMG_FORMAT_WAVELET_RGB:
			case IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
			case IMG_FORMAT_WAVELET_LUMINANCE:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case IMG_FORMAT_BITMAP_ALPHA:
			case IMG_FORMAT_WAVELET_ALPHA:
				return DXGI_FORMAT_A8_UNORM;
			default:
				return DXGI_FORMAT_UNKNOWN;
			}
		}

		bool is_dxt_format(const GfxImageFileFormat format)
		{
			return format == IMG_FORMAT_DXT1 || format == IMG_FORMAT_DXT3 || format == IMG_FORMAT_DXT5;
		}

		bool is_bitmap_format(const GfxImageFileFormat format)
		{
			return format >= IMG_FORMAT_BITMAP_RGBA && format <= IMG_FORMAT_BITMAP_ALPHA;
		}

		bool is_wavelet_format(const GfxImageFileFormat format)
		{
			return format >= IMG_FORMAT_WAVELET_RGBA && format <= IMG_FORMAT_WAVELET_ALPHA;
		}

		// IMG_FORMAT_WAVELET_* mirror IMG_FORMAT_BITMAP_* with the same channel layout
		GfxImageFileFormat wavelet_to_bitmap_format(const GfxImageFileFormat format)
		{
			return static_cast<GfxImageFileFormat>(format - (IMG_FORMAT_WAVELET_RGBA - IMG_FORMAT_BITMAP_RGBA));
		}

		unsigned int bitmap_channel_count(const GfxImageFileFormat format)
		{
			switch (format)
			{
			case IMG_FORMAT_BITMAP_RGBA:
				return 4;
			case IMG_FORMAT_BITMAP_RGB:
				return 3;
			case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
				return 2;
			default:
				return 1;
			}
		}

		unsigned int count_mipmaps(const unsigned int width, const unsigned int height, const unsigned int depth)
		{
			unsigned int count = 1;
			for (unsigned int res = 1; res < width || res < height || res < depth; res *= 2)
			{
				++count;
			}
			return count;
		}

		unsigned int compressed_size_for_level(const GfxImageFileFormat format, const unsigned int width, const unsigned int height)
		{
			return format == IMG_FORMAT_DXT1
				? CompressedBlockSizeDXT1(width, height)
				: CompressedBlockSizeDXT5(width, height); // dxt3 and dxt5 both use 16 bytes per 4x4 block
		}

		bool convert_header(const std::string& name, const std::vector<std::uint8_t>& bytes, iwx::GfxImageFileHeader& header)
		{
			if (bytes.size() < 4 || std::memcmp(bytes.data(), "IWi", 3) != 0)
			{
				ZONETOOL_ERROR("\"%s.iwi\" is not a valid IWI file", name.data());
				return false;
			}

			const auto version = bytes[3];
			switch (version)
			{
			case 8: // iw4
			case 9: // iw5, codo
			{
				if (bytes.size() < sizeof(iw5::GfxImageFileHeader))
				{
					ZONETOOL_ERROR("IWI file \"%s.iwi\" is too small", name.data());
					return false;
				}

				const auto* iwi_header = reinterpret_cast<const iw5::GfxImageFileHeader*>(bytes.data());

				// the map type bits use the same encoding as iwx
				std::uint32_t flags = iwi_header->flags & iw5::IMG_FLAG_MAPTYPE_MASK;
				const auto convert_flag = [&](const iw5::GfxImageFlags iwi_flag, const iwx::GfxImageFlags iwx_flag)
				{
					if ((iwi_header->flags & iwi_flag) != 0)
					{
						flags |= iwx_flag;
					}
				};
				convert_flag(iw5::IMG_FLAG_NOPICMIP, iwx::IMG_FLAG_NOPICMIP);
				convert_flag(iw5::IMG_FLAG_NOMIPMAPS, iwx::IMG_FLAG_NOMIPMAPS);
				convert_flag(iw5::IMG_FLAG_STREAMING, iwx::IMG_FLAG_STREAMING);
				convert_flag(iw5::IMG_FLAG_LEGACY_NORMALS, iwx::IMG_FLAG_LEGACY_NORMALS);
				convert_flag(iw5::IMG_FLAG_CLAMP_U, iwx::IMG_FLAG_CLAMP_U);
				convert_flag(iw5::IMG_FLAG_CLAMP_V, iwx::IMG_FLAG_CLAMP_V);
				convert_flag(iw5::IMG_FLAG_NORMALMAP, iwx::IMG_FLAG_NORMALMAP);

				header.format = iwi_header->format;
				header.flags = static_cast<iwx::GfxImageFlags>(flags);
				std::memcpy(&header.dimensions, &iwi_header->dimensions, sizeof(header.dimensions));
				std::memcpy(&header.fileSizeForPicmip, &iwi_header->fileSizeForPicmip, sizeof(header.fileSizeForPicmip));
				header.size = sizeof(iw5::GfxImageFileHeader);
				return true;
			}
			case 6: // iw3
			{
				if (bytes.size() < sizeof(iw3::GfxImageFileHeader))
				{
					ZONETOOL_ERROR("IWI file \"%s.iwi\" is too small", name.data());
					return false;
				}

				const auto* iwi_header = reinterpret_cast<const iw3::GfxImageFileHeader*>(bytes.data());

				std::uint32_t flags = 0;
				const auto convert_flag = [&](const iw3::GfxImageFlags iwi_flag, const iwx::GfxImageFlags iwx_flag)
				{
					if ((iwi_header->flags & iwi_flag) != 0)
					{
						flags |= iwx_flag;
					}
				};
				convert_flag(iw3::IMG_FLAG_NOPICMIP, iwx::IMG_FLAG_NOPICMIP);
				convert_flag(iw3::IMG_FLAG_NOMIPMAPS, iwx::IMG_FLAG_NOMIPMAPS);
				convert_flag(iw3::IMG_FLAG_CUBEMAP, iwx::IMG_FLAG_MAPTYPE_CUBE);
				convert_flag(iw3::IMG_FLAG_VOLMAP, iwx::IMG_FLAG_MAPTYPE_3D);
				convert_flag(iw3::IMG_FLAG_STREAMING, iwx::IMG_FLAG_STREAMING);
				convert_flag(iw3::IMG_FLAG_LEGACY_NORMALS, iwx::IMG_FLAG_LEGACY_NORMALS);
				convert_flag(iw3::IMG_FLAG_CLAMP_U, iwx::IMG_FLAG_CLAMP_U);
				convert_flag(iw3::IMG_FLAG_CLAMP_V, iwx::IMG_FLAG_CLAMP_V);

				header.format = iwi_header->format;
				header.flags = static_cast<iwx::GfxImageFlags>(flags);
				std::memcpy(&header.dimensions, &iwi_header->dimensions, sizeof(header.dimensions));
				std::memcpy(&header.fileSizeForPicmip, &iwi_header->fileSizeForPicmip, sizeof(header.fileSizeForPicmip));
				header.size = sizeof(iw3::GfxImageFileHeader);
				return true;
			}
			default:
				ZONETOOL_ERROR("Unknown IWI version (%i) for image \"%s.iwi\"", static_cast<int>(version), name.data());
				return false;
			}
		}

		GfxImage* make_image(GfxImage* image, zone_memory* mem, const std::string& name, const iwx::GfxImageFileHeader& header,
			const MapType map_type, const unsigned int data_len, const unsigned int level_count)
		{
			image->name = mem->duplicate_string(name);
			image->imageFormat = dxgi_format_from_iwi(header.format);
			image->mapType = map_type;
			image->flags = header.flags;
			image->width = static_cast<unsigned short>(header.dimensions[0]);
			image->height = static_cast<unsigned short>(header.dimensions[1]);
			image->depth = static_cast<unsigned short>(header.dimensions[2]);
			image->numElements = 1;
			image->levelCount = static_cast<unsigned char>(level_count);
			image->dataLen = data_len;
			image->pixelData = mem->allocate<unsigned char>(data_len);
			return image;
		}

		GfxImage* parse_cube(const std::string& name, const iwx::GfxImageFileHeader& header,
			const std::vector<std::uint8_t>& bytes, zone_memory* mem, GfxImage* image)
		{
			if (!is_dxt_format(header.format))
			{
				ZONETOOL_ERROR("IWI format (%d) is not supported for CUBE image \"%s.iwi\"", header.format, name.data());
				return nullptr;
			}

			const unsigned int width = header.dimensions[0];
			const unsigned int height = header.dimensions[1];

			const auto* pixel_data = bytes.data() + header.size;
			auto pixel_data_size = static_cast<unsigned int>(bytes.size()) - header.size;

			// iwi files store mip levels from smallest to largest; keep only the best level (all 6 faces)
			if ((header.flags & iwx::IMG_FLAG_NOMIPMAPS) == 0)
			{
				const unsigned int size_for_best_level = compressed_size_for_level(header.format, width, height) * 6;
				const unsigned int data_to_skip = header.fileSizeForPicmip[0] - size_for_best_level - header.size;

				if (data_to_skip >= pixel_data_size)
				{
					ZONETOOL_FATAL("Something went horribly wrong parsing IWI file \"%s.iwi\"", name.data());
				}

				pixel_data += data_to_skip;
				pixel_data_size -= data_to_skip;
			}

			make_image(image, mem, name, header, MAPTYPE_CUBE, pixel_data_size, 1);
			std::memcpy(image->pixelData, pixel_data, pixel_data_size);
			return image;
		}

		// expands bgr(a)-ordered channel data into rgba8 (or copies a8 through)
		void convert_bitmap_pixels(const GfxImageFileFormat format, const std::uint8_t* src, std::uint8_t* dst,
			const unsigned int pixel_count)
		{
			const unsigned int src_channels = bitmap_channel_count(format);

			switch (format)
			{
			case IMG_FORMAT_BITMAP_RGBA:
			case IMG_FORMAT_BITMAP_RGB:
				for (unsigned int i = 0; i < pixel_count; i++, src += src_channels, dst += 4)
				{
					dst[0] = src[2];
					dst[1] = src[1];
					dst[2] = src[0];
					dst[3] = src_channels == 4 ? src[3] : 255ui8;
				}
				break;
			case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
			case IMG_FORMAT_BITMAP_LUMINANCE:
				for (unsigned int i = 0; i < pixel_count; i++, src += src_channels, dst += 4)
				{
					dst[0] = src[0];
					dst[1] = src[0];
					dst[2] = src[0];
					dst[3] = src_channels == 2 ? src[1] : 255ui8;
				}
				break;
			case IMG_FORMAT_BITMAP_ALPHA:
			default:
				std::memcpy(dst, src, pixel_count);
				break;
			}
		}

		GfxImage* parse_2d_bitmap(const std::string& name, const iwx::GfxImageFileHeader& header,
			const std::vector<std::uint8_t>& bytes, zone_memory* mem, GfxImage* image)
		{
			const unsigned int width = header.dimensions[0];
			const unsigned int height = header.dimensions[1];

			const unsigned int src_channels = bitmap_channel_count(header.format);
			const unsigned int dst_channels = header.format == IMG_FORMAT_BITMAP_ALPHA ? 1 : 4;

			const auto* src = bytes.data() + header.size;
			const auto pixel_data_size = static_cast<unsigned int>(bytes.size()) - header.size;

			// iwi files store mip levels from smallest to largest; keep only the best level
			if ((header.flags & iwx::IMG_FLAG_NOMIPMAPS) == 0)
			{
				const unsigned int size_for_best_level = src_channels * width * height;
				const unsigned int data_to_skip = header.fileSizeForPicmip[0] - size_for_best_level - header.size;

				if (data_to_skip >= pixel_data_size)
				{
					ZONETOOL_FATAL("Something went horribly wrong parsing IWI file \"%s.iwi\"", name.data());
				}

				src += data_to_skip;
			}

			const unsigned int total_len = dst_channels * width * height;
			make_image(image, mem, name, header, MAPTYPE_2D, total_len, 1);
			convert_bitmap_pixels(header.format, src, image->pixelData, width * height);
			return image;
		}

		GfxImage* parse_2d_wavelet(const std::string& name, const iwx::GfxImageFileHeader& header,
			const std::vector<std::uint8_t>& bytes, zone_memory* mem, GfxImage* image)
		{
			const unsigned int width = header.dimensions[0];
			const unsigned int height = header.dimensions[1];
			const unsigned int depth = header.dimensions[2];

			const auto bitmap_format = wavelet_to_bitmap_format(header.format);
			const unsigned int channels = bitmap_channel_count(bitmap_format);
			const unsigned int dst_channels = bitmap_format == IMG_FORMAT_BITMAP_ALPHA ? 1 : 4;

			const bool has_mipmaps = (header.flags & iwx::IMG_FLAG_NOMIPMAPS) == 0;
			const unsigned int mip_count = has_mipmaps ? count_mipmaps(width, height, depth) : 1;

			const auto pixels = wavelet::decode_image(bytes.data() + header.size, bytes.size() - header.size,
				width, height, channels, mip_count);
			if (pixels.empty())
			{
				ZONETOOL_ERROR("Failed to decode wavelet IWI file \"%s.iwi\"", name.data());
				return nullptr;
			}

			const unsigned int total_len = dst_channels * width * height;
			make_image(image, mem, name, header, MAPTYPE_2D, total_len, 1);
			convert_bitmap_pixels(bitmap_format, pixels.data(), image->pixelData, width * height);
			return image;
		}

		GfxImage* parse_2d(const std::string& name, const iwx::GfxImageFileHeader& header,
			const std::vector<std::uint8_t>& bytes, zone_memory* mem, GfxImage* image)
		{
			if (is_bitmap_format(header.format))
			{
				return parse_2d_bitmap(name, header, bytes, mem, image);
			}

			if (is_wavelet_format(header.format))
			{
				return parse_2d_wavelet(name, header, bytes, mem, image);
			}

			if (!is_dxt_format(header.format))
			{
				ZONETOOL_ERROR("IWI format (%d) is not supported for 2D image \"%s.iwi\"", header.format, name.data());
				return nullptr;
			}

			const unsigned int width = header.dimensions[0];
			const unsigned int height = header.dimensions[1];
			const unsigned int depth = header.dimensions[2];

			const bool has_mipmaps = (header.flags & iwx::IMG_FLAG_NOMIPMAPS) == 0;
			const unsigned int level_count = has_mipmaps ? count_mipmaps(width, height, depth) : 1;

			const auto* pixel_data = bytes.data() + header.size;
			const auto total_len = static_cast<unsigned int>(bytes.size()) - header.size;

			make_image(image, mem, name, header, MAPTYPE_2D, total_len, level_count);

			if (!has_mipmaps)
			{
				std::memcpy(image->pixelData, pixel_data, total_len);
				return image;
			}

			// iwi files store mip levels from smallest to largest; flip them to largest first
			unsigned int data_offset = 0;
			for (unsigned int level = 0; data_offset < total_len; level++)
			{
				const unsigned int w = std::max(1u, width >> level);
				const unsigned int h = std::max(1u, height >> level);

				const unsigned int size_for_level = compressed_size_for_level(header.format, w, h);
				const unsigned int src_offset = total_len - size_for_level - data_offset;

				if (src_offset >= total_len)
				{
					ZONETOOL_FATAL("Something went horribly wrong parsing IWI file \"%s.iwi\"", name.data());
				}

				std::memcpy(image->pixelData + data_offset, pixel_data + src_offset, size_for_level);
				data_offset += size_for_level;
			}

			return image;
		}
	}

	bool fixup_normal_map(GfxImage* image)
	{
		if (image == nullptr || image->pixelData == nullptr)
		{
			return false;
		}

		const auto* name = image->name;

		if (image->imageFormat != DXGI_FORMAT_BC3_UNORM)
		{
			ZONETOOL_FATAL("Normalmap has to be in a compressed format! (%s)", name);
		}

		const unsigned int width = image->width;
		const unsigned int height = image->height;
		const unsigned int total_size = image->dataLen;
		const unsigned int level_count = std::max(1u, static_cast<unsigned int>(image->levelCount));

		std::vector<std::uint8_t> new_pixels(total_size);

		// dxt5 and bc5 both use 16 bytes per 4x4 block, so the converted data has the same size and layout.
		// mip levels are stored largest to smallest after parsing; cube maps repeat the best level once per face.
		unsigned int data_offset = 0;
		for (unsigned int i = 0; data_offset < total_size; i++)
		{
			const unsigned int level = std::min(i, level_count - 1);
			const unsigned int w = std::max(1u, width >> level);
			const unsigned int h = std::max(1u, height >> level);

			const unsigned int compressed_size = CompressedBlockSizeDXT5(w, h);
			if (data_offset + compressed_size > total_size)
			{
				ZONETOOL_FATAL("Something went horribly wrong converting normalmap \"%s\"", name);
			}

			// decompress the level; block decompression writes whole rows of 4x4 blocks,
			// so pad the buffer height to the next multiple of 4
			const unsigned int padded_height = (h + 3) & ~3u;
			std::vector<std::uint8_t> rgba(4 * w * padded_height);
			BlockDecompressImageDXT5(w, h, image->pixelData + data_offset, reinterpret_cast<unsigned int*>(rgba.data()));

			// dxt5 normal maps store the normal in (g, a); bc5 expects it in (r, g)
			for (unsigned int pixel = 0; pixel < 4 * w * h; pixel += 4)
			{
				const auto x = rgba[pixel + 1];
				const auto y = rgba[pixel + 3];

				rgba[pixel + 0] = x;
				rgba[pixel + 1] = y;
				rgba[pixel + 2] = 128ui8;
				rgba[pixel + 3] = 255ui8;
			}

			DirectX::Image dx_image{};
			dx_image.width = w;
			dx_image.height = h;
			dx_image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			dx_image.pixels = rgba.data();
			DirectX::ComputePitch(dx_image.format, dx_image.width, dx_image.height, dx_image.rowPitch, dx_image.slicePitch);

			DirectX::ScratchImage compressed{};
			if (FAILED(DirectX::Compress(dx_image, DXGI_FORMAT_BC5_SNORM, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, compressed)))
			{
				ZONETOOL_FATAL("Failed to compress normalmap \"%s\" to BC5", name);
			}

			assert(compressed.GetPixelsSize() == compressed_size);
			std::memcpy(new_pixels.data() + data_offset, compressed.GetPixels(), compressed_size);

			data_offset += compressed_size;
		}

		std::memcpy(image->pixelData, new_pixels.data(), total_size);
		image->imageFormat = DXGI_FORMAT_BC5_SNORM;

		return true;
	}

	GfxImage* parse_iwi(const std::string& name, void* mem, GfxImage* image, [[maybe_unused]] bool is_normal_map)
	{
		if (image == nullptr)
		{
			return nullptr;
		}

		auto* memory = reinterpret_cast<zone_memory*>(mem);

		const auto path = utils::string::va("images\\%s.iwi", clean_name(name).data());
		auto file = filesystem::file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return nullptr;
		}

		const auto bytes = file.read_bytes(file.size());
		file.close();

		iwx::GfxImageFileHeader header{};
		if (!convert_header(name, bytes, header))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing IWI image \"%s\", version (%i)", name.data(), static_cast<int>(bytes[3]));

		switch (header.flags & iwx::IMG_FLAG_MAPTYPE_MASK)
		{
		case iwx::IMG_FLAG_MAPTYPE_CUBE:
			return parse_cube(name, header, bytes, memory, image);
		case iwx::IMG_FLAG_MAPTYPE_3D:
			ZONETOOL_ERROR("3D IWI files are not supported yet! (%s.iwi)", name.data());
			return nullptr;
		case iwx::IMG_FLAG_MAPTYPE_1D:
			ZONETOOL_ERROR("1D IWI files are not supported yet! (%s.iwi)", name.data());
			return nullptr;
		default:
			return parse_2d(name, header, bytes, memory, image);
		}
	}
}
