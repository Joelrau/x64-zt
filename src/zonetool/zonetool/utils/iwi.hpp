#pragma once

namespace iwi
{
	namespace parse
	{
		enum MapType
		{
			MAPTYPE_NONE = 0x0,
			MAPTYPE_INVALID1 = 0x1,
			MAPTYPE_1D = 0x2,
			MAPTYPE_2D = 0x3,
			MAPTYPE_3D = 0x4,
			MAPTYPE_CUBE = 0x5,
			MAPTYPE_ARRAY = 0x6,
			MAPTYPE_COUNT = 0x7,
		};

		struct GfxImage
		{
			const char* name;
			DXGI_FORMAT imageFormat;
			MapType mapType;
			unsigned char version;
			unsigned int flags;
			unsigned int dataLen;
			unsigned short width;
			unsigned short height;
			unsigned short depth;
			unsigned short numElements;
			unsigned char levelCount;
			unsigned char* pixelData;
		};
	}
	using namespace parse;

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

	namespace iw5
	{
		enum GfxImageFlags : std::uint32_t
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
			GfxImageFlags flags;
			GfxImageFileFormat format;
			char unused;
			short dimensions[3];
			int fileSizeForPicmip[4];
		};
	}

	namespace iw3
	{
		enum GfxImageFlags : std::uint8_t
		{
			IMG_FLAG_NOPICMIP = 0x1,
			IMG_FLAG_NOMIPMAPS = 0x2,
			IMG_FLAG_CUBEMAP = 0x4,
			IMG_FLAG_VOLMAP = 0x8,
			IMG_FLAG_STREAMING = 0x10,
			IMG_FLAG_LEGACY_NORMALS = 0x20,
			IMG_FLAG_CLAMP_U = 0x40,
			IMG_FLAG_CLAMP_V = 0x80,
			//IMG_FLAG_DYNAMIC = 0x10000,
			//IMG_FLAG_RENDER_TARGET = 0x20000,
			//IMG_FLAG_SYSTEMMEM = 0x40000,
		};

		struct GfxImageFileHeader
		{
			char tag[3];
			char version;
			GfxImageFileFormat format;
			GfxImageFlags flags;
			short dimensions[3];
			int fileSizeForPicmip[4];
		};
	}

	namespace iwx
	{
		enum GfxImageFlags : std::uint32_t
		{
			IMG_FLAG_NOPICMIP = 0x1,
			IMG_FLAG_NOMIPMAPS = 0x2,
			IMG_FLAG_STREAMING = 0x4,
			IMG_FLAG_LEGACY_NORMALS = 0x8,
			IMG_FLAG_CLAMP_U = 0x10,
			IMG_FLAG_CLAMP_V = 0x20,
			IMG_FLAG_MAPTYPE_2D = 0x0,
			IMG_FLAG_MAPTYPE_CUBE = 0x10000,
			IMG_FLAG_MAPTYPE_3D = 0x20000,
			IMG_FLAG_MAPTYPE_1D = 0x30000,
		};

		struct GfxImageFileHeader
		{
			GfxImageFileFormat format;
			GfxImageFlags flags;
			short dimensions[3];
			int fileSizeForPicmip[4];
			int size;
		};
	}

	GfxImage* parse_iwi(const std::string& name, void* mem, GfxImage* img_);
}