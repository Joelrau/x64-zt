#include <std_include.hpp>
#include "gfximage.hpp"

#include "zonetool/utils/s3tc.hpp"
#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include "zonetool/utils/compression.hpp"
#include <utils/io.hpp>
#include <utils/cryptography.hpp>

//#define IMAGE_DUMP_DDS

namespace zonetool::h1
{
	namespace
	{
		bool is_map_image(const std::string& name)
		{
			return (name.size() >= 6)
				? ((name.substr(0, 6) == "*light" || name.substr(0, 6) == "*refle" ||
					name == "$outdoor")
					? true
					: false)
				: false;
		}

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
		namespace shared
		{
			enum GfxImageFileFormat : std::uint8_t
			{
				IMG_FORMAT_INVALID = 0x0,
				IMG_FORMAT_BITMAP_RGBA = 0x1,
				IMG_FORMAT_BITMAP_RGB = 0x2,
				IMG_FORMAT_BITMAP_LUMINANCE_ALPHA = 0x3,
				IMG_FORMAT_BITMAP_LUMINANCE = 0x4,
				IMG_FORMAT_BITMAP_ALPHA = 0x5,
				IMG_FORMAT_WAVELET_RGBA = 0x6,
				IMG_FORMAT_WAVELET_RGB = 0x7,
				IMG_FORMAT_WAVELET_LUMINANCE_ALPHA = 0x8,
				IMG_FORMAT_WAVELET_LUMINANCE = 0x9,
				IMG_FORMAT_WAVELET_ALPHA = 0xA,
				IMG_FORMAT_DXT1 = 0xB,
				IMG_FORMAT_DXT3 = 0xC,
				IMG_FORMAT_DXT5 = 0xD,
				IMG_FORMAT_DXN = 0xE,
				IMG_FORMAT_DXT3A_AS_LUMINANCE = 0xF,
				IMG_FORMAT_DXT5A_AS_LUMINANCE = 0x10,
				IMG_FORMAT_DXT3A_AS_ALPHA = 0x11,
				IMG_FORMAT_DXT5A_AS_ALPHA = 0x12,
				IMG_FORMAT_DXT1_AS_LUMINANCE_ALPHA = 0x13,
				IMG_FORMAT_DXN_AS_LUMINANCE_ALPHA = 0x14,
				IMG_FORMAT_DXT1_AS_LUMINANCE = 0x15,
				IMG_FORMAT_DXT1_AS_ALPHA = 0x16,
				IMG_FORMAT_COUNT = 0x17,
			};
		}
		using namespace shared;

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

		namespace iw5
		{
			enum IMG_FLAGS : std::uint32_t
			{
				IMG_FLAG_NOPICMIP = 0x1,
				IMG_FLAG_NOMIPMAPS = 0x2,
				IMG_FLAG_STREAMING = 0x4,
				IMG_FLAG_LEGACY_NORMALS = 0x8,
				IMG_FLAG_CLAMP_U = 0x10,
				IMG_FLAG_CLAMP_V = 0x20,
				IMG_FLAG_ALPHA_WEIGHTED_COLORS = 0x40,
				IMG_FLAG_DXTC_APPROX_WEIGHTS = 0x80,
				IMG_FLAG_GAMMA_NONE = 0x0,
				IMG_FLAG_GAMMA_SRGB = 0x100,
				IMG_FLAG_GAMMA_PWL = 0x200,
				IMG_FLAG_GAMMA_2 = 0x300,
				IMG_FLAG_MAPTYPE_2D = 0x0,
				IMG_FLAG_MAPTYPE_CUBE = 0x10000,
				IMG_FLAG_MAPTYPE_3D = 0x20000,
				IMG_FLAG_MAPTYPE_1D = 0x30000,
				IMG_FLAG_NORMALMAP = 0x40000,
				IMG_FLAG_INTENSITY_TO_ALPHA = 0x80000,
				IMG_FLAG_DYNAMIC = 0x1000000,
				IMG_FLAG_RENDER_TARGET = 0x2000000,
				IMG_FLAG_SYSTEMMEM = 0x4000000,
			};

			struct GfxImageFileHeader
			{
				char tag[3];
				char version;
				unsigned int flags;
				GfxImageFileFormat format;
				char unused;
				short dimensions[3];
				int fileSizeForPicmip[4];
			};
		}

		namespace iw3
		{
			enum IMG_FLAGS : std::uint32_t
			{
				IMG_FLAG_NOPICMIP = 0x1,
				IMG_FLAG_NOMIPMAPS = 0x2,
				IMG_FLAG_CUBEMAP = 0x4,
				IMG_FLAG_VOLMAP = 0x8,
				IMG_FLAG_STREAMING = 0x10,
				IMG_FLAG_LEGACY_NORMALS = 0x20,
				IMG_FLAG_CLAMP_U = 0x40,
				IMG_FLAG_CLAMP_V = 0x80,
				IMG_FLAG_DYNAMIC = 0x10000,
				IMG_FLAG_RENDER_TARGET = 0x20000,
				IMG_FLAG_SYSTEMMEM = 0x40000,
			};

			struct GfxImageFileHeader
			{
				char tag[3];
				char version;
				GfxImageFileFormat format;
				unsigned char flags;
				short dimensions[3];
				int fileSizeForPicmip[4];
			};
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
				// Source is in format: 0xAARRGGBB
				((argb & 0x00FF0000) >> 16) | //______RR
				((argb & 0x0000FF00)) | //____GG__
				((argb & 0x000000FF) << 16) | //___BB____
				((argb & 0xFF000000));         //AA______
				// Return value is in format:  0xAABBGGRR 
		}

		GfxImage* parse(const std::string& name, ZoneMemory* mem)
		{
			const auto path = utils::string::va("images\\%s.iwi", clean_name(name).data());
			auto file = filesystem::file(path);
			file.open("rb");
			if (file.get_fp())
			{
				auto size = file.size();
				auto bytes = file.read_bytes(size);
				file.close();

				const auto version = bytes[3];

				if (version == 8 || version == 9) // iw4, iw5, codo
				{
					ZONETOOL_INFO("Parsing IWI image \"%s\", version (%i)", name.data(), version);

					std::vector<std::uint8_t> pixels;
					auto iwi_header = reinterpret_cast<iw5::GfxImageFileHeader*>(bytes.data());

					const bool has_mipmaps = (iwi_header->flags & iw5::IMG_FLAG_NOMIPMAPS) == 0;

					const unsigned int width = iwi_header->dimensions[0];
					const unsigned int height = iwi_header->dimensions[1];
					const unsigned int depth = iwi_header->dimensions[2];
					const unsigned int mipmaps_count = has_mipmaps ? image_count_mipmaps(width, height, depth) : 1;
					const auto dxgi_format = dxgi_format_from_iwi(iwi_header->format);

					if ((iwi_header->flags & iw5::IMG_FLAGS::IMG_FLAG_MAPTYPE_CUBE) != 0) // parse cube
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
							unsigned int data_to_skip_size = iwi_header->fileSizeForPicmip[0] - size_for_best_level - sizeof(iw5::GfxImageFileHeader);

							if (data_to_skip_size >= bytes.size() - sizeof(iw5::GfxImageFileHeader))
							{
								ZONETOOL_FATAL("Something went horribly wrong parsing IWI file \"%s.iwi\"", name.data());
							}

							pixel_data = bytes.data() + sizeof(iw5::GfxImageFileHeader) + data_to_skip_size;
							pixel_data_size = bytes.size() - sizeof(iw5::GfxImageFileHeader) - data_to_skip_size;
						}
						else
						{
							pixel_data = bytes.data() + sizeof(iw5::GfxImageFileHeader);
							pixel_data_size = bytes.size() - sizeof(iw5::GfxImageFileHeader);
						}

						const unsigned int total_len = static_cast<unsigned int>(pixel_data_size);

						// zone image
						auto* image = mem->Alloc<GfxImage>();
						image->name = mem->StrDup(name);
						image->imageFormat = dxgi_format;
						image->mapType = MAPTYPE_CUBE;
						image->semantic = 2;
						image->category = 3;
						image->flags = 2;
						image->width = static_cast<unsigned short>(width);
						image->height = static_cast<unsigned short>(height);
						image->depth = static_cast<unsigned short>(depth);
						image->dataLen1 = total_len;
						image->dataLen2 = total_len;
						image->numElements = 6;
						image->levelCount = 1;
						image->streamed = false;

						image->pixelData = mem->Alloc<unsigned char>(total_len);
						memcpy(image->pixelData, pixel_data, total_len);

						return image;
					}
					else if ((iwi_header->flags & iw5::IMG_FLAGS::IMG_FLAG_MAPTYPE_3D) != 0) // parse 3d
					{
						ZONETOOL_ERROR("3D IWI files are not supported yet! (%s.iwi)", name.data());
						return nullptr;
					}
					else if ((iwi_header->flags & iw5::IMG_FLAGS::IMG_FLAG_MAPTYPE_1D) != 0) // parse 1d
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
							pixel_data = bytes.data() + sizeof(iw5::GfxImageFileHeader);
							pixel_data_size = bytes.size() - sizeof(iw5::GfxImageFileHeader);
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
							auto* image = mem->Alloc<GfxImage>();
							image->name = mem->StrDup(name);
							image->imageFormat = dxgi_format;
							image->mapType = MAPTYPE_2D;
							image->semantic = 2;
							image->category = 3;
							image->flags = 0;
							image->width = static_cast<unsigned short>(width);
							image->height = static_cast<unsigned short>(height);
							image->depth = static_cast<unsigned short>(depth);
							image->dataLen1 = total_len;
							image->dataLen2 = total_len;
							image->numElements = 1;
							image->levelCount = 1;
							image->streamed = false;

							// skip mipmaps
							if (has_mipmaps)
							{
								unsigned int data_to_skip_size = iwi_header->fileSizeForPicmip[0] - size_for_best_level - sizeof(iw5::GfxImageFileHeader);
								pixel_data += data_to_skip_size;
							}

							image->pixelData = mem->Alloc<unsigned char>(total_len);
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
								image->pixelData = mem->Alloc<unsigned char>(total_len);
								memcpy(image->pixelData, pixel_data, total_len);
								break;
							}

							return image;
						}

						const unsigned int total_len = static_cast<unsigned int>(pixel_data_size);

						// zone image
						auto* image = mem->Alloc<GfxImage>();
						image->name = mem->StrDup(name);
						image->imageFormat = dxgi_format;
						image->mapType = MAPTYPE_2D;
						image->semantic = 2;
						image->category = 3;
						image->flags = 0;
						image->width = static_cast<unsigned short>(width);
						image->height = static_cast<unsigned short>(height);
						image->depth = static_cast<unsigned short>(depth);
						image->dataLen1 = total_len;
						image->dataLen2 = total_len;
						image->numElements = 1;
						image->levelCount = static_cast<unsigned char>(mipmaps_count);
						image->streamed = false;

						image->pixelData = mem->Alloc<unsigned char>(total_len);
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
				else if (version == 6) // iw3
				{
					ZONETOOL_INFO("Parsing IWI image \"%s\", version (%i)", name.data(), version);

					std::vector<std::uint8_t> pixels;
					auto iwi_header = reinterpret_cast<iw3::GfxImageFileHeader*>(bytes.data());

					const bool has_mipmaps = (iwi_header->flags & iw3::IMG_FLAG_NOMIPMAPS) == 0;

					const unsigned int width = iwi_header->dimensions[0];
					const unsigned int height = iwi_header->dimensions[1];
					const unsigned int depth = iwi_header->dimensions[2];
					const unsigned int mipmaps_count = has_mipmaps ? image_count_mipmaps(width, height, depth) : 1;
					const auto dxgi_format = dxgi_format_from_iwi(iwi_header->format);

					if ((iwi_header->flags & iw3::IMG_FLAGS::IMG_FLAG_CUBEMAP) != 0) // parse cube
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
							unsigned int data_to_skip_size = iwi_header->fileSizeForPicmip[0] - size_for_best_level - sizeof(iw3::GfxImageFileHeader);

							if (data_to_skip_size >= bytes.size() - sizeof(iw3::GfxImageFileHeader))
							{
								ZONETOOL_FATAL("Something went horribly wrong parsing IWI file \"%s.iwi\"", name.data());
							}

							pixel_data = bytes.data() + sizeof(iw3::GfxImageFileHeader) + data_to_skip_size;
							pixel_data_size = bytes.size() - sizeof(iw3::GfxImageFileHeader) - data_to_skip_size;
						}
						else
						{
							pixel_data = bytes.data() + sizeof(iw3::GfxImageFileHeader);
							pixel_data_size = bytes.size() - sizeof(iw3::GfxImageFileHeader);
						}

						const unsigned int total_len = static_cast<unsigned int>(pixel_data_size);

						// zone image
						auto* image = mem->Alloc<GfxImage>();
						image->name = mem->StrDup(name);
						image->imageFormat = dxgi_format;
						image->mapType = MAPTYPE_CUBE;
						image->semantic = 2;
						image->category = 3;
						image->flags = 2;
						image->width = static_cast<unsigned short>(width);
						image->height = static_cast<unsigned short>(height);
						image->depth = static_cast<unsigned short>(depth);
						image->dataLen1 = total_len;
						image->dataLen2 = total_len;
						image->numElements = 6;
						image->levelCount = 1;
						image->streamed = false;

						image->pixelData = mem->Alloc<unsigned char>(total_len);
						memcpy(image->pixelData, pixel_data, total_len);

						return image;
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
							pixel_data = bytes.data() + sizeof(iw3::GfxImageFileHeader);
							pixel_data_size = bytes.size() - sizeof(iw3::GfxImageFileHeader);
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
							auto* image = mem->Alloc<GfxImage>();
							image->name = mem->StrDup(name);
							image->imageFormat = dxgi_format;
							image->mapType = MAPTYPE_2D;
							image->semantic = 2;
							image->category = 3;
							image->flags = 0;
							image->width = static_cast<unsigned short>(width);
							image->height = static_cast<unsigned short>(height);
							image->depth = static_cast<unsigned short>(depth);
							image->dataLen1 = total_len;
							image->dataLen2 = total_len;
							image->numElements = 1;
							image->levelCount = 1;
							image->streamed = false;

							// skip mipmaps
							if (has_mipmaps)
							{
								unsigned int data_to_skip_size = iwi_header->fileSizeForPicmip[0] - size_for_best_level - sizeof(iw3::GfxImageFileHeader);
								pixel_data += data_to_skip_size;
							}

							image->pixelData = mem->Alloc<unsigned char>(total_len);
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
								image->pixelData = mem->Alloc<unsigned char>(total_len);
								memcpy(image->pixelData, pixel_data, total_len);
								break;
							}

							return image;
						}

						const unsigned int total_len = static_cast<unsigned int>(pixel_data_size);

						// zone image
						auto* image = mem->Alloc<GfxImage>();
						image->name = mem->StrDup(name);
						image->imageFormat = dxgi_format;
						image->mapType = MAPTYPE_2D;
						image->semantic = 2;
						image->category = 3;
						image->flags = 0;
						image->width = static_cast<unsigned short>(width);
						image->height = static_cast<unsigned short>(height);
						image->depth = static_cast<unsigned short>(depth);
						image->dataLen1 = total_len;
						image->dataLen2 = total_len;
						image->numElements = 1;
						image->levelCount = static_cast<unsigned char>(mipmaps_count);
						image->streamed = false;

						image->pixelData = mem->Alloc<unsigned char>(total_len);
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
				else
				{
					ZONETOOL_ERROR("Unknown IWI version (%i) for image \"%s.iwi\"", static_cast<int>(version), name.data());
				}
			}
			return nullptr;
		}
	}

	namespace directxtex
	{
		bool load_image(const std::string& name, DirectX::ScratchImage* image)
		{
			std::string c_name = clean_name(name);
			c_name = utils::string::va("images\\%s", c_name.data());
			if (filesystem::file(c_name + ".dds").exists())
			{
				// DDS Found
				c_name.append(".dds");
			}
			else if (filesystem::file(c_name + ".tga").exists())
			{
				// TGA Found
				c_name.append(".tga");
			}
			else
			{
				// No image found
				return false;
			}
			std::string path = filesystem::get_file_path(c_name) + c_name;

			std::wstring wname = utils::string::convert(path);

			HRESULT hr = E_FAIL;
			if (c_name.ends_with(".dds"))
			{
				hr = LoadFromDDSFile(wname.data(), DirectX::DDS_FLAGS_NONE, nullptr, *image);
			}
			else if (c_name.ends_with(".tga"))
			{
				hr = LoadFromTGAFile(wname.data(), nullptr, *image);
			}

			if (SUCCEEDED(hr))
			{
				return true;
			}
			return false;
		}

		GfxImage* parse(const std::string& name, ZoneMemory* mem)
		{
			DirectX::ScratchImage image;
			bool result;
			int w;
			int h;
			char m_name[128]{ 0 };

			result = true;
			if (!load_image(name, &image))
			{
				w = 4096;
				result = false;
				do
				{
					h = 4096;
					do
					{
						snprintf(m_name, sizeof(m_name), "%s_%dx%d", name.data(), w, h);
						if (load_image(m_name, &image))
						{
							result = true;
							break;
						}
						h = h / 2;
					} while (h > 0);
					if (result)
					{
						break;
					}
					w = w / 2;
				} while (w > 0);
			}

			if (result)
			{
				ZONETOOL_INFO("Parsing custom image \"%s\"", name.data());

				auto* pixels = image.GetPixels();
				auto pixels_size = image.GetPixelsSize();
				const auto& metadata = image.GetMetadata();

				auto* gfx_image = mem->Alloc<GfxImage>();

				gfx_image->imageFormat = metadata.format;
				gfx_image->mapType = static_cast<MapType>(metadata.dimension);
				gfx_image->semantic = 2;
				gfx_image->category = 3;
				gfx_image->flags = 0;
				gfx_image->width = static_cast<unsigned short>(metadata.width);
				gfx_image->height = static_cast<unsigned short>(metadata.height);
				gfx_image->depth = static_cast<unsigned short>(metadata.depth);
				gfx_image->numElements = 1;
				gfx_image->levelCount = 1;
				gfx_image->streamed = 0;
				gfx_image->dataLen1 = static_cast<int>(pixels_size);
				gfx_image->dataLen2 = static_cast<int>(pixels_size);
				gfx_image->pixelData = mem->Alloc<unsigned char>(pixels_size);
				memcpy(gfx_image->pixelData, pixels, pixels_size);
				gfx_image->name = mem->StrDup(name);

				return gfx_image;
			}

			return nullptr;
		}
	}

	GfxImage* IGfxImage::parse_custom(const std::string& name, ZoneMemory* mem)
	{
		GfxImage* image = nullptr;
		image = directxtex::parse(name, mem);
		if (!image)
		{
			image = iwi::parse(name, mem);
		}

		return image;
	}

	std::optional<std::string> get_streamed_image_pixels(const std::string& name, int stream)
	{
		const auto image_path = utils::string::va("streamed_images\\%s_stream%i.pixels", 
			clean_name(name).data(), stream);
		const auto full_path = filesystem::get_file_path(image_path) + image_path;
		
		if (utils::io::file_exists(full_path))
		{
			return {utils::io::read_file(full_path)};
		}

		return {};
	}

	bool get_streamed_image_dds(const std::string& name, int stream, DirectX::ScratchImage& image)
	{
		const auto image_path = utils::string::va("streamed_images\\%s_stream%i.dds",
			clean_name(name).data(), stream);

		const auto full_path = filesystem::get_file_path(image_path) + image_path;
		const auto full_path_w = utils::string::convert(full_path);
		const auto result = DirectX::LoadFromDDSFile(full_path_w.data(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		return SUCCEEDED(result);
	}

	std::optional<std::string> get_streamed_image_dds(const std::string& name, int stream)
	{
		const auto image_path = utils::string::va("streamed_images\\%s_stream%i.dds",
			clean_name(name).data(), stream);

		DirectX::ScratchImage image{};

		const auto full_path = filesystem::get_file_path(image_path) + image_path;
		const auto full_path_w = utils::string::convert(full_path);
		const auto result = DirectX::LoadFromDDSFile(full_path_w.data(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		if (!SUCCEEDED(result))
		{
			return {};
		}

		const auto pixel_data = image.GetPixels();
		const auto size = image.GetPixelsSize();

		return {{reinterpret_cast<const char*>(pixel_data), size}};
	}

	std::optional<std::string> get_streamed_image_data(const std::string& name, int stream)
	{
		const auto pixels = get_streamed_image_pixels(name, stream);
		if (pixels.has_value())
		{
			return pixels;
		}

		const auto dds = get_streamed_image_dds(name, stream);
		if (dds.has_value())
		{
			return dds;
		}

		return {};
	}

	GfxImage* IGfxImage::parse_streamed_image(const std::string& name, ZoneMemory* mem)
	{
		const auto asset_path = utils::string::va("streamed_images\\%s.h1Image", clean_name(name).data());

		assetmanager::reader read(mem);
		if (!read.open(asset_path))
		{
			return nullptr;
		}

		GfxImage* asset = read.read_single<GfxImage>();
		asset->name = read.read_string();

		ZONETOOL_INFO("Parsing streamed image \"%s\"...", name.data());

		this->custom_streamed_image = true;

		for (auto i = 0; i < 4; i++)
		{
			const auto result = get_streamed_image_pixels(name, i);
			if (!result.has_value())
			{
				continue;
			}

			const auto& pixels = result.value();
			const auto compressed = compression::lz4::compress_lz4_block(pixels);
			this->image_stream_blocks[i] = {compressed};
		}

		read.close();

		return asset;
	}

	GfxImage* IGfxImage::parse(const std::string& name, ZoneMemory* mem)
	{
		auto path = "images\\" + clean_name(name) + ".h1Image";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing image \"%s\"...", name.data());

		auto asset = read.read_single<GfxImage>();
		asset->name = read.read_string();
		if (asset->pixelData)
		{
			asset->pixelData = read.read_array<unsigned char>();
		}

		if (asset->streamed)
		{
			// read stream file
			for (auto i = 0u; i < 4; i++)
			{
				this->image_stream_files[i] = read.read_single<XStreamFile>();
			}
		}

		read.close();

		memset(&asset->texture, 0, sizeof(asset->texture));

		return asset;
	}

	void IGfxImage::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (this->asset_)
		{
			return;
		}

		this->asset_ = this->parse_streamed_image(name, mem);
		if (this->asset_)
		{
			return;
		}

		this->asset_ = parse_custom(name.data(), mem);
		if (!this->asset_)
		{
			ZONETOOL_WARNING("Image \"%s\" not found, it will probably look messed up ingame!", name.data());

			static unsigned char default_pixel_data[4] = { 255, 0, 0, 255 };
			auto* image = mem->Alloc<GfxImage>();
			image->imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			image->mapType = MAPTYPE_2D;
			image->semantic = 0;
			image->category = 1;
			image->flags = 0;
			image->dataLen1 = sizeof(default_pixel_data);
			image->dataLen2 = sizeof(default_pixel_data);
			image->width = 1;
			image->height = 1;
			image->pixelData = default_pixel_data;
			image->depth = 1;
			image->numElements = 1;
			image->levelCount = 1;
			image->streamed = 0;
			image->name = mem->StrDup(this->name_);

			this->asset_ = image;
		}
	}

	void IGfxImage::init(void* asset, ZoneMemory* mem)
	{
		this->asset_ = reinterpret_cast<GfxImage*>(asset);
		this->name_ = this->asset_->name;

		auto parsed = this->parse(this->name_, mem);
		if (parsed)
		{
			this->asset_ = parsed;
		}
	}

	void IGfxImage::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IGfxImage::load_depending(IZone* zone)
	{
	}

	std::string IGfxImage::name()
	{
		return this->name_;
	}

	std::int32_t IGfxImage::type()
	{
		return ASSET_TYPE_IMAGE;
	}

	void IGfxImage::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->semantic != 11)
		{
			buf->push_stream(0);
			if (data->pixelData)
			{
				buf->align(3);
				buf->write_stream(data->pixelData, data->dataLen1);
				ZoneBuffer::clear_pointer(&dest->pixelData);
			}
			buf->pop_stream();
		}
		else
		{
			buf->push_stream(0);
			if (data->pixelData)
			{
				buf->align(3);
				buf->write_stream(data->pixelData, 1);
				ZoneBuffer::clear_pointer(&dest->pixelData);
			}
			buf->pop_stream();
		}

		buf->pop_stream();

		if (data->streamed)
		{
			// add stream file to header
			for (auto i = 0u; i < 4; i++)
			{
				buf->write_streamfile(reinterpret_cast<std::uintptr_t>(this->image_stream_files[i]));
			}
		}
	}

#ifdef IMAGE_DUMP_DDS
	void dump_image_dds(GfxImage* image)
	{
		if (!image->streamed)
		{
			DirectX::Image img = {};
			img.pixels = image->pixelData;
			img.width = image->width;
			img.height = image->height;
			img.format = DXGI_FORMAT(image->imageFormat);

			size_t rowPitch;
			size_t slicePitch;
			DirectX::ComputePitch(img.format, img.width, img.height, rowPitch, slicePitch);

			img.rowPitch = rowPitch;
			img.slicePitch = slicePitch;

			std::string parent_path = filesystem::get_dump_path() + "images\\";
			std::string spath = parent_path + clean_name(image->name) + ".dds";
			std::wstring wpath(spath.begin(), spath.end());

			if (!std::filesystem::exists(parent_path))
			{
				std::filesystem::create_directories(parent_path);
			}

			auto result = DirectX::SaveToDDSFile(img, DirectX::DDS_FLAGS_NONE, wpath.data());
			if (FAILED(result))
			{
				ZONETOOL_WARNING("Failed to dump image \"%s.dds\"", image->name);
			}
		}
	}
#endif

	void IGfxImage::dump(GfxImage* asset)
	{
#ifdef IMAGE_DUMP_DDS
		dump_image_dds(asset);
#endif

		auto path = "images\\"s + clean_name(asset->name) + ".h1Image"s;
		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		if (asset->semantic == 11)
		{
			ZONETOOL_WARNING("GfxImage (%s) was not dumped succesfully!", asset->name);
			write.close();
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		if (asset->pixelData)
		{
			write.dump_array(asset->pixelData, asset->dataLen1);
		}

		if (asset->streamed)
		{
			auto streamFileIndex = *reinterpret_cast<unsigned int*>(0x143274884);
			auto streamFiles = reinterpret_cast<XStreamFile*>(0x1432748B0);

			for (auto i = 0u; i < 4; i++)
			{
				auto streamFile = &streamFiles[streamFileIndex + i];
				write.dump_single(streamFile);
			}
		}
		write.close();
	}
}