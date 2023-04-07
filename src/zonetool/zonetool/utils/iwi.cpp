#include <std_include.hpp>
#include "iwi.hpp"

#include "utils.hpp"

#include "s3tc.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include <utils/io.hpp>
#include <utils/string.hpp>

using namespace zonetool;

namespace
{
	std::string clean_name(const std::string& name)
	{
		auto new_name = name;

		for (auto i = 0u; i < name.size(); i++)
		{
			switch (new_name[i])
			{
			case '*':
				new_name[i] = '_';
				break;
			}
		}

		return new_name;
	}
}

namespace iwi
{
	DXGI_FORMAT dxgi_format_from_iwi(GfxImageFileFormat format)
	{
		switch (format)
		{
		case IMG_FORMAT_DXT1:
			return DXGI_FORMAT_BC1_UNORM; // D3DFMT_DXT1
			break;
		case IMG_FORMAT_DXT3:
			return DXGI_FORMAT_BC2_UNORM; // D3DFMT_DXT3
			break;
		case IMG_FORMAT_DXT5:
			return DXGI_FORMAT_BC3_UNORM; // D3DFMT_DXT5
			break;
		case IMG_FORMAT_BITMAP_RGBA:
			return DXGI_FORMAT_R8G8B8A8_UNORM; // DXGI_FORMAT_B8G8R8A8_UNORM // D3DFMT_A8R8G8B8
			break;
		case IMG_FORMAT_BITMAP_RGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM; // DXGI_FORMAT_B8G8R8X8_UNORM // D3DFMT_X8R8G8B8
			break;
		case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
			return DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R8_UNORM; // Not available // D3DFMT_A8L8
			break;
		case IMG_FORMAT_BITMAP_LUMINANCE:
			return DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R8_UNORM; // D3DFMT_L8
			break;
		case IMG_FORMAT_BITMAP_ALPHA:
			return DXGI_FORMAT_A8_UNORM; // D3DFMT_A8
			break;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	unsigned int image_count_mipmaps(unsigned int width, unsigned int height, unsigned int depth)
	{
		unsigned int mipRes;
		unsigned int mipCount;

		mipCount = 1;
		for (mipRes = 1; mipRes < width || mipRes < height || mipRes < depth; mipRes *= 2)
			++mipCount;
		return mipCount;
	}

	inline std::uint32_t from_argb(std::uint32_t argb)
	{
		return
			((argb & 0x00FF0000) >> 16) |
			((argb & 0x0000FF00)) |
			((argb & 0x000000FF) << 16) |
			((argb & 0xFF000000));
	}

	bool fixup_normal_map(GfxImage* img_)
	{
		if (img_ == nullptr)
		{
			return false;
		}

		if (img_->pixelData == nullptr)
		{
			return false;
		}

		const auto original_size = img_->dataLen;
		const auto original_pixels = img_->pixelData;

		auto name = img_->name;

		[[maybe_unused]] auto dxgi_format = img_->imageFormat;
		int width = img_->width;
		int height = img_->height;
		[[maybe_unused]] int level_count = img_->levelCount;
		[[maybe_unused]] bool has_levels = img_->levelCount > 1;

		std::uint8_t* pixel_data = original_pixels;
		std::size_t pixel_data_size = original_size;

		std::vector<std::uint8_t> new_pixels;
		new_pixels.resize(original_size);

		unsigned int total_size = static_cast<unsigned int>(pixel_data_size);

		unsigned int w = width;
		unsigned int h = height;

		unsigned int data_left = total_size;
		unsigned int data_offset = 0;
		unsigned int i = 0;
		unsigned int x = static_cast<unsigned int>(std::pow<int, int>(2, level_count - 1));
		while (data_left)
		{
			w = std::max(1u, width / x);
			h = std::max(1u, height / x);

			unsigned int compressed_block_size = 0;
			switch (img_->imageFormat)
			{
			case DXGI_FORMAT_BC3_UNORM:
				compressed_block_size = CompressedBlockSizeDXT5(w, h);
				break;
			default:
				ZONETOOL_FATAL("Normalmap has to be in a compressed format! (%s)", name);
				break;
			}

			unsigned int data_to_skip_size = data_offset;

			if (data_to_skip_size >= total_size)
			{
				ZONETOOL_FATAL("Something went horribly wrong converting normalmap \"%s\"", name);
			}

			auto pixels_block = pixel_data + data_to_skip_size;

			// uncompress pixels
			unsigned int uncompressed_size = 4 * w * h;
			std::vector<std::uint8_t> uncompressed_pixels;
			uncompressed_pixels.resize(uncompressed_size * 16); // piece of shit
			memset(uncompressed_pixels.data(), 0, uncompressed_pixels.size());

			switch (img_->imageFormat)
			{
			case DXGI_FORMAT_BC3_UNORM:
				BlockDecompressImageDXT5(w, h, pixels_block, reinterpret_cast<unsigned int*>(uncompressed_pixels.data()));
				break;
			default:
				ZONETOOL_FATAL("Normalmap has to be in a compressed format! (%s)", name);
				break;
			}

			auto uncomp_pixels = uncompressed_pixels.data();

			// swap channels
			{
				for (auto pixel_index = 0u; pixel_index < uncompressed_size; pixel_index += 4)
				{
					[[maybe_unused]] auto r = uncomp_pixels[pixel_index + 0];
					[[maybe_unused]] auto g = uncomp_pixels[pixel_index + 1];
					[[maybe_unused]] auto b = uncomp_pixels[pixel_index + 2];
					[[maybe_unused]] auto a = uncomp_pixels[pixel_index + 3];

					std::uint8_t xy[2] = { g, a };

					uncomp_pixels[pixel_index + 0] = xy[0];
					uncomp_pixels[pixel_index + 1] = xy[1];
					uncomp_pixels[pixel_index + 2] = 128ui8;
					uncomp_pixels[pixel_index + 3] = 255ui8;
				}
			}

			// re_compress
			DirectX::Image img = {};

			img.width = w;
			img.height = h;
			img.pixels = uncomp_pixels;
			img.format = DXGI_FORMAT_R8G8B8A8_UNORM;

			size_t row_pitch{};
			size_t slice_pitch{};

			DirectX::ComputePitch(img.format, img.width, img.height, row_pitch, slice_pitch);

			img.rowPitch = row_pitch;
			img.slicePitch = slice_pitch;

			DirectX::ScratchImage sc_img{};
			DirectX::Compress(img, DXGI_FORMAT_BC5_SNORM, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, sc_img);

			// copy data
			assert(sc_img.GetPixelsSize() == compressed_block_size);
			memcpy(new_pixels.data() + data_offset, sc_img.GetPixels(), compressed_block_size);

			// continue
			data_left -= compressed_block_size;
			data_offset += compressed_block_size;
			i++;
			x = x / 2;
		}

		pixel_data = new_pixels.data();
		pixel_data_size = new_pixels.size();

		assert(original_size >= pixel_data_size);

		std::memset(img_->pixelData, 0, pixel_data_size);
		std::memcpy(img_->pixelData, pixel_data, pixel_data_size);
		img_->dataLen = static_cast<unsigned int>(pixel_data_size);

		img_->imageFormat = DXGI_FORMAT_BC5_SNORM;

		return true;
	}

	GfxImage* parse_iwi(const std::string& name, void* meme, GfxImage* img_, bool is_normal_map)
	{
		if (img_ == nullptr)
		{
			return nullptr;
		}

		zone_memory* mem = reinterpret_cast<zone_memory*>(meme);

		const auto path = utils::string::va("images\\%s.iwi", clean_name(name).data());
		auto file = filesystem::file(path);
		file.open("rb");
		if (file.get_fp())
		{
			auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			const auto version = bytes[3];

			// convert header
			iwx::GfxImageFileHeader iwx_header{};
			if (version == 8 || version == 9) // iw4, iw5, codo
			{
				const auto iwi_header = reinterpret_cast<iw5::GfxImageFileHeader*>(bytes.data());

				iwx_header.format = iwi_header->format;

				std::uint32_t flags = 0;
				const auto convert_flag = [&](const iw5::GfxImageFlags iwi_flag, const iwx::GfxImageFlags iwx_flag) -> void
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
				//convert_flag(iw5::IMG_FLAG_ALPHA_WEIGHTED_COLORS, iwx::IMG_FLAG_ALPHA_WEIGHTED_COLORS);
				//convert_flag(iw5::IMG_FLAG_DXTC_APPROX_WEIGHTS, iwx::IMG_FLAG_DXTC_APPROX_WEIGHTS);
				//convert_flag(iw5::IMG_FLAG_GAMMA_NONE, iwx::IMG_FLAG_GAMMA_NONE);
				//convert_flag(iw5::IMG_FLAG_GAMMA_SRGB, iwx::IMG_FLAG_GAMMA_SRGB);
				//convert_flag(iw5::IMG_FLAG_GAMMA_PWL, iwx::IMG_FLAG_GAMMA_PWL);
				//convert_flag(iw5::IMG_FLAG_GAMMA_2, iwx::IMG_FLAG_GAMMA_2);
				convert_flag(iw5::IMG_FLAG_MAPTYPE_2D, iwx::IMG_FLAG_MAPTYPE_2D);
				convert_flag(iw5::IMG_FLAG_MAPTYPE_CUBE, iwx::IMG_FLAG_MAPTYPE_CUBE);
				convert_flag(iw5::IMG_FLAG_MAPTYPE_3D, iwx::IMG_FLAG_MAPTYPE_3D);
				convert_flag(iw5::IMG_FLAG_MAPTYPE_1D, iwx::IMG_FLAG_MAPTYPE_1D);
				convert_flag(iw5::IMG_FLAG_NORMALMAP, iwx::IMG_FLAG_NORMALMAP);
				//convert_flag(iw5::IMG_FLAG_INTENSITY_TO_ALPHA, iwx::IMG_FLAG_INTENSITY_TO_ALPHA);
				//convert_flag(iw5::IMG_FLAG_DYNAMIC, iwx::IMG_FLAG_DYNAMIC);
				//convert_flag(iw5::IMG_FLAG_RENDER_TARGET, iwx::IMG_FLAG_RENDER_TARGET);
				//convert_flag(iw5::IMG_FLAG_SYSTEMMEM, iwx::IMG_FLAG_SYSTEMMEM);
				iwx_header.flags = static_cast<iwx::GfxImageFlags>(flags);

				memcpy(&iwx_header.dimensions, &iwi_header->dimensions, sizeof(short[3]));
				memcpy(&iwx_header.fileSizeForPicmip, &iwi_header->fileSizeForPicmip, sizeof(int[4]));

				iwx_header.size = sizeof(iw5::GfxImageFileHeader);
			}
			else if (version == 6) // iw3
			{
				const auto iwi_header = reinterpret_cast<iw3::GfxImageFileHeader*>(bytes.data());

				iwx_header.format = iwi_header->format;

				std::uint32_t flags = 0;
				const auto convert_flag = [&](const iw3::GfxImageFlags iwi_flag, const iwx::GfxImageFlags iwx_flag) -> void
				{
					if ((iwi_header->flags & iwi_flag) != 0)
					{
						flags |= iwx_flag;
					}
				};
				convert_flag(iw3::IMG_FLAG_NOPICMIP, iwx::IMG_FLAG_NOPICMIP);
				convert_flag(iw3::IMG_FLAG_NOMIPMAPS, iwx::IMG_FLAG_NOMIPMAPS);
				convert_flag(iw3::IMG_FLAG_CUBEMAP, iwx::IMG_FLAG_MAPTYPE_CUBE);
				//convert_flag(iw3::IMG_FLAG_VOLMAP, iwx::IMG_FLAG_VOLMAP);
				convert_flag(iw3::IMG_FLAG_STREAMING, iwx::IMG_FLAG_STREAMING);
				convert_flag(iw3::IMG_FLAG_LEGACY_NORMALS, iwx::IMG_FLAG_LEGACY_NORMALS);
				convert_flag(iw3::IMG_FLAG_CLAMP_U, iwx::IMG_FLAG_CLAMP_U);
				convert_flag(iw3::IMG_FLAG_CLAMP_V, iwx::IMG_FLAG_CLAMP_V);
				//convert_flag(iw3::IMG_FLAG_DYNAMIC, iwx::IMG_FLAG_DYNAMIC);
				//convert_flag(iw3::IMG_FLAG_RENDER_TARGET, iwx::IMG_FLAG_RENDER_TARGET);
				//convert_flag(iw3::IMG_FLAG_SYSTEMMEM, iwx::IMG_FLAG_SYSTEMMEM);
				iwx_header.flags = static_cast<iwx::GfxImageFlags>(flags);

				memcpy(&iwx_header.dimensions, &iwi_header->dimensions, sizeof(short[3]));
				memcpy(&iwx_header.fileSizeForPicmip, &iwi_header->fileSizeForPicmip, sizeof(int[4]));

				iwx_header.size = sizeof(iw3::GfxImageFileHeader);
			}
			else
			{
				ZONETOOL_ERROR("Unknown IWI version (%i) for image \"%s.iwi\"", static_cast<int>(version), name.data());
				return nullptr;
			}

			ZONETOOL_INFO("Parsing IWI image \"%s\", version (%i)", name.data(), version);

			const auto iwi_header = &iwx_header;

			bool has_mipmaps = (iwi_header->flags & iwx::IMG_FLAG_NOMIPMAPS) == 0;

			const unsigned int width = iwi_header->dimensions[0];
			const unsigned int height = iwi_header->dimensions[1];
			const unsigned int depth = iwi_header->dimensions[2];

			unsigned int mipmaps_count = has_mipmaps ? image_count_mipmaps(width, height, depth) : 1;

			auto dxgi_format = dxgi_format_from_iwi(iwi_header->format);

			if ((iwi_header->flags & iwx::IMG_FLAG_MAPTYPE_CUBE) != 0) // parse cube
			{
				if (iwi_header->format != IMG_FORMAT_DXT1 &&
					iwi_header->format != IMG_FORMAT_DXT3 &&
					iwi_header->format != IMG_FORMAT_DXT5)
				{
					ZONETOOL_ERROR("IWI format (%d) is not supported for CUBE image \"%s.iwi\"", iwi_header->format, name.data());
					return nullptr;
				}

				std::uint8_t* pixel_data;
				std::size_t pixel_data_size = 0;

				// skip mipmaps
				if (has_mipmaps)
				{
					unsigned int size_for_best_level = 0;
					switch (iwi_header->format)
					{
					case IMG_FORMAT_DXT1:
						size_for_best_level = CompressedBlockSizeDXT1(width, height) * 6;
						break;
					case IMG_FORMAT_DXT3:
					case IMG_FORMAT_DXT5:
						size_for_best_level = CompressedBlockSizeDXT5(width, height) * 6;
						break;
					}
					unsigned int data_to_skip_size = iwi_header->fileSizeForPicmip[0] - size_for_best_level - iwi_header->size;

					if (data_to_skip_size >= bytes.size() - iwi_header->size)
					{
						ZONETOOL_FATAL("Something went horribly wrong parsing IWI file \"%s.iwi\"", name.data());
					}

					pixel_data = bytes.data() + iwi_header->size + data_to_skip_size;
					pixel_data_size = bytes.size() - iwi_header->size - data_to_skip_size;
				}
				else
				{
					pixel_data = bytes.data() + iwi_header->size;
					pixel_data_size = bytes.size() - iwi_header->size;
				}

#ifdef DEBUG
				// uncompress pixels
				const unsigned int uncompressed_size = (4 * width * height) * 6;
				std::vector<std::uint8_t> uncompressed_pixels;
				uncompressed_pixels.resize(uncompressed_size);
				dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
				unsigned int p_1 = 0;
				unsigned int p_2 = 0;
				for (auto i = 0; i < 6; i++)
				{
					switch (iwi_header->format)
					{
					case IMG_FORMAT_DXT1:
						BlockDecompressImageDXT1(width, height,
							pixel_data + p_2,
							reinterpret_cast<unsigned int*>(uncompressed_pixels.data() + p_1));
						p_2 += CompressedBlockSizeDXT1(width, height);
						break;
					case IMG_FORMAT_DXT3:
					case IMG_FORMAT_DXT5:
						BlockDecompressImageDXT5(width, height,
							pixel_data + p_2,
							reinterpret_cast<unsigned int*>(uncompressed_pixels.data() + p_1));
						p_2 += CompressedBlockSizeDXT5(width, height);
						break;
					}
					p_1 += (4 * width * height);
				}
				pixel_data = uncompressed_pixels.data();
				pixel_data_size = uncompressed_size;

				// darken the pixels
				const bool darken_cube_pixels = true;
				if (darken_cube_pixels)
				{
					for (auto i = 0; i < pixel_data_size; i += 4)
					{
						auto r = pixel_data[i + 0];
						auto g = pixel_data[i + 1];
						auto b = pixel_data[i + 2];
						auto a = pixel_data[i + 3];

						const auto truncate = [](std::int32_t value) -> std::uint8_t
						{
							if (value < 0) return 0;
							if (value > 255) return 255;

							return static_cast<std::uint8_t>(value);
						};

						const auto neg_val = 30ui8;

						pixel_data[i + 0] = truncate(r - neg_val);
						pixel_data[i + 1] = truncate(g - neg_val);
						pixel_data[i + 2] = truncate(b - neg_val);
						pixel_data[i + 3] = a;
					}
				}
#endif

				const unsigned int total_len = static_cast<unsigned int>(pixel_data_size);

				// zone image
				auto* image = img_;
				image->name = mem->duplicate_string(name);
				image->imageFormat = dxgi_format;
				image->mapType = MAPTYPE_CUBE;
				image->flags = iwi_header->flags;
				image->width = static_cast<unsigned short>(width);
				image->height = static_cast<unsigned short>(height);
				image->depth = static_cast<unsigned short>(depth);
				image->dataLen = total_len;
				image->numElements = 6;
				image->levelCount = 1;

				image->pixelData = mem->allocate<unsigned char>(total_len);
				memcpy(image->pixelData, pixel_data, total_len);

				return image;
			}
			else if ((iwi_header->flags & iwx::IMG_FLAG_MAPTYPE_3D) != 0) // parse 3d
			{
				ZONETOOL_ERROR("3D IWI files are not supported yet! (%s.iwi)", name.data());
				return nullptr;
			}
			else if ((iwi_header->flags & iwx::IMG_FLAG_MAPTYPE_1D) != 0) // parse 1d
			{
				ZONETOOL_ERROR("1D IWI files are not supported yet! (%s.iwi)", name.data());
				return nullptr;
			}
			else // parse 2d
			{
				if (iwi_header->format != IMG_FORMAT_DXT1 &&
					iwi_header->format != IMG_FORMAT_DXT3 &&
					iwi_header->format != IMG_FORMAT_DXT5 &&
					iwi_header->format != IMG_FORMAT_BITMAP_RGBA &&
					iwi_header->format != IMG_FORMAT_BITMAP_RGB &&
					iwi_header->format != IMG_FORMAT_BITMAP_LUMINANCE_ALPHA &&
					iwi_header->format != IMG_FORMAT_BITMAP_LUMINANCE &&
					iwi_header->format != IMG_FORMAT_BITMAP_ALPHA)
				{
					ZONETOOL_ERROR("IWI format (%d) is not supported for 2D image \"%s.iwi\"", iwi_header->format, name.data());
					return nullptr;
				}

				std::uint8_t* pixel_data;
				std::size_t pixel_data_size = 0;
				{
					pixel_data = bytes.data() + iwi_header->size;
					pixel_data_size = bytes.size() - iwi_header->size;
				}

				if (iwi_header->format == IMG_FORMAT_BITMAP_RGBA ||
					iwi_header->format == IMG_FORMAT_BITMAP_RGB ||
					iwi_header->format == IMG_FORMAT_BITMAP_LUMINANCE_ALPHA ||
					iwi_header->format == IMG_FORMAT_BITMAP_LUMINANCE ||
					iwi_header->format == IMG_FORMAT_BITMAP_ALPHA)
				{
					const auto get_from_channels = [](GfxImageFileFormat from)
					{
						unsigned int channels = 0;
						switch (from)
						{
						case IMG_FORMAT_BITMAP_RGBA:
							channels = 4;
							break;
						case IMG_FORMAT_BITMAP_RGB:
							channels = 3;
							break;
						case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
							channels = 2;
							break;
						case IMG_FORMAT_BITMAP_LUMINANCE:
						case IMG_FORMAT_BITMAP_ALPHA:
							channels = 1;
							break;
						}
						return channels;
					};

					const auto get_to_channels = [](DXGI_FORMAT to)
					{
						unsigned int channels = 0;
						switch (to)
						{
						case DXGI_FORMAT_R8G8B8A8_UNORM:
							channels = 4;
							break;
						case DXGI_FORMAT_A8_UNORM:
							channels = 1;
							break;
						}
						return channels;
					};

					const unsigned int from_channels = get_from_channels(iwi_header->format);
					const unsigned int to_channels = get_to_channels(dxgi_format);
					const unsigned int total_len = to_channels * width * height;
					const unsigned int size_for_best_level = from_channels * width * height;

					// zone image
					auto* image = img_;
					image->name = mem->duplicate_string(name);
					image->imageFormat = dxgi_format;
					image->mapType = MAPTYPE_2D;
					image->flags = iwi_header->flags;
					image->width = static_cast<unsigned short>(width);
					image->height = static_cast<unsigned short>(height);
					image->depth = static_cast<unsigned short>(depth);
					image->dataLen = total_len;
					image->numElements = 1;
					image->levelCount = 1;

					// skip mipmaps
					if (has_mipmaps)
					{
						unsigned int data_to_skip_size = iwi_header->fileSizeForPicmip[0] - size_for_best_level - iwi_header->size;
						pixel_data += data_to_skip_size;
					}

					image->pixelData = mem->allocate<unsigned char>(total_len);
					unsigned int j = 0;
					switch (iwi_header->format)
					{
					case IMG_FORMAT_BITMAP_RGBA:
					case IMG_FORMAT_BITMAP_RGB:
						// RGBA
						for (unsigned int i = 0; i < total_len; i += 4)
						{
							image->pixelData[i + 0] = pixel_data[j + 0];
							image->pixelData[i + 1] = from_channels > 1 ? pixel_data[j + 1] : pixel_data[j + 0];
							image->pixelData[i + 2] = from_channels > 2 ? pixel_data[j + 2] : pixel_data[j + 0];
							image->pixelData[i + 3] = from_channels > 3 ? pixel_data[j + 3] : 255;
							j += from_channels;

							// convert
							auto argb = *(unsigned int*)(&image->pixelData[i]);
							auto rgba = from_argb(argb);
							*(unsigned int*)&image->pixelData[i] = rgba;
						}
						break;
					case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
					case IMG_FORMAT_BITMAP_LUMINANCE:
						// RGBA
						for (unsigned int i = 0; i < total_len; i += 4)
						{
							image->pixelData[i + 0] = pixel_data[j + 0];
							image->pixelData[i + 1] = pixel_data[j + 0];
							image->pixelData[i + 2] = pixel_data[j + 0];
							image->pixelData[i + 3] = from_channels > 1 ? pixel_data[j + 1] : 255;
							j += from_channels;
						}
						break;
					case IMG_FORMAT_BITMAP_ALPHA:
						// A
						image->pixelData = mem->allocate<unsigned char>(total_len);
						memcpy(image->pixelData, pixel_data, total_len);
						break;
					}

					return image;
				}

				const unsigned int total_len = static_cast<unsigned int>(pixel_data_size);

				// zone image
				auto* image = img_;
				image->name = mem->duplicate_string(name);
				image->imageFormat = dxgi_format;
				image->mapType = MAPTYPE_2D;
				image->flags = iwi_header->flags;
				image->width = static_cast<unsigned short>(width);
				image->height = static_cast<unsigned short>(height);
				image->depth = static_cast<unsigned short>(depth);
				image->dataLen = total_len;
				image->numElements = 1;
				image->levelCount = static_cast<unsigned char>(mipmaps_count);

				image->pixelData = mem->allocate<unsigned char>(total_len);
				if (has_mipmaps)
				{
					unsigned int w = width;
					unsigned int h = height;

					unsigned int data_left = total_len;
					unsigned int data_offset = 0;
					unsigned int i = 0;
					unsigned int x = 1;
					while (data_left)
					{
						w = std::max(1u, width / x);
						h = std::max(1u, height / x);

						unsigned int size_for_level = 0;
						switch (iwi_header->format)
						{
						case IMG_FORMAT_DXT1:
							size_for_level = CompressedBlockSizeDXT1(w, h);
							break;
						case IMG_FORMAT_DXT3:
						case IMG_FORMAT_DXT5:
							size_for_level = CompressedBlockSizeDXT5(w, h);
							break;
						}
						switch (dxgi_format)
						{
						case DXGI_FORMAT_R8G8B8A8_UNORM:
							size_for_level = 4 * width * height;
							break;
						}

						unsigned int data_to_skip_size = total_len - size_for_level - data_offset;

						if (data_to_skip_size >= pixel_data_size)
						{
							ZONETOOL_FATAL("Something went horribly wrong parsing IWI file \"%s.iwi\"", name.data());
						}

						memcpy(image->pixelData + data_offset, pixel_data + data_to_skip_size, size_for_level);
						data_left -= size_for_level;
						data_offset += size_for_level;
						i++;
						x = x * 2;
					}
				}
				else
				{
					memcpy(image->pixelData, pixel_data, total_len);
				}

				return image;
			}
		}
		return nullptr;
	}
}