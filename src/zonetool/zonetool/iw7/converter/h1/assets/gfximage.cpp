#include <std_include.hpp>

#include "zonetool/iw7/converter/h1/include.hpp"
#include "gfximage.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>
#include "zonetool/utils/compression.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include <zlib.h>

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace gfximage
		{
			namespace
			{
				std::unordered_map<std::uint8_t, std::uint8_t> mapped_semantics =
				{
					{zonetool::iw7::TS_2D, zonetool::h1::TS_2D},
					{zonetool::iw7::TS_FUNCTION, zonetool::h1::TS_FUNCTION},
					{zonetool::iw7::TS_COLOR_MAP, zonetool::h1::TS_COLOR_MAP},
					{zonetool::iw7::TS_DETAIL_MAP, zonetool::h1::TS_DETAIL_MAP},
					{zonetool::iw7::TS_UNUSED_4, zonetool::h1::TS_UNUSED_2},
					{zonetool::iw7::TS_NORMAL_MAP, zonetool::h1::TS_NORMAL_MAP},
					{zonetool::iw7::TS_UNUSED_6, zonetool::h1::TS_UNUSED_3},
					{zonetool::iw7::TS_UNUSED_7, zonetool::h1::TS_UNUSED_4},
					{zonetool::iw7::TS_SPECULAR_MAP, zonetool::h1::TS_SPECULAR_MAP},
					//{zonetool::iw7::TS_SPECULAR_OCCLUSION_MAP,	zonetool::h1::TS_2D},
					//{zonetool::iw7::TS_UNUSED_10,					zonetool::h1::TS_2D},
					//{zonetool::iw7::TS_THINFILM_MAP,				zonetool::h1::TS_2D},
					//{zonetool::iw7::TS_DISPLACEMENT_MAP,			zonetool::h1::TS_2D},
					//{zonetool::iw7::TS_PARALLAX_MAP,				zonetool::h1::TS_2D},
					//{zonetool::iw7::TS_COLOR_SPECULAR_MAP,		zonetool::h1::TS_2D},
					//{zonetool::iw7::TS_NORMAL_OCCLUSSION_GLOSS_MAP,	zonetool::h1::TS_2D},
					//{zonetool::iw7::TS_ALPHA_REVEAL_THICKNESS_MAP,	zonetool::h1::TS_2D},
				};

				std::uint8_t convert_iw7_to_h1_category(std::uint8_t iw7_id)
				{
					if (iw7_id >= zonetool::h1::IMG_CATEGORY_FIRST_UNMANAGED)
					{
						iw7_id += 1; // h1 has water
					}
					return iw7_id;
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

			void dump_streamed_image(GfxImage* image)
			{
				for (auto i = 0u; i < 4; i++)
				{
					const auto stream_file = &stream_files[*stream_file_index + i];

					const std::string filename = utils::string::va("imagefile%d.pak", stream_file->fileIndex);
					const auto folder = filesystem::get_zone_path(filename);

					const auto imagefile_path = utils::string::va("%s%s", folder.data(), filename.data());
					std::ifstream imagefile;
					imagefile.open(imagefile_path, std::ios::binary);

					if (!imagefile.is_open() || stream_file->offset == 0 || stream_file->offsetEnd == 0)
					{
						continue;
					}

					std::string buffer;
					const auto size = stream_file->offsetEnd - stream_file->offset;
					buffer.resize(size);
					imagefile.seekg(stream_file->offset);
					imagefile.read(buffer.data(), size);

					try
					{
						auto pixel_data = compression::lz4::decompress_lz4_block(buffer);
						const auto name = clean_name(image->name);

						const auto dump_pixels = true;
						if (dump_pixels)
						{
							std::string parent_path = filesystem::get_dump_path() + "streamed_images\\";
							std::string raw_path = utils::string::va("%s%s_stream%i.pixels", parent_path.data(), name.data(), i);
							utils::io::write_file(raw_path, pixel_data, false);
						}

						const auto dump_dds = false;
						if (dump_dds)
						{
							DirectX::Image img = {};

							img.width = image->streams[i].width;
							img.height = image->streams[i].height;
							img.pixels = reinterpret_cast<uint8_t*>(pixel_data.data());
							img.format = DXGI_FORMAT(image->imageFormat);

							size_t row_pitch{};
							size_t slice_pitch{};

							DirectX::ComputePitch(img.format, img.width, img.height, row_pitch, slice_pitch);

							img.rowPitch = row_pitch;
							img.slicePitch = slice_pitch;

							const auto parent_path = filesystem::get_dump_path() + "streamed_images\\";
							const std::string spath = utils::string::va("%s\\%s_stream%i.dds", parent_path.data(),
								name.data(), i);
							const std::wstring wpath(spath.begin(), spath.end());
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
					catch (...)
					{
						ZONETOOL_ERROR("Failed to dump streamed image \"%s\"", image->name);
					}
				}

				{
					const auto path = "streamed_images\\"s + clean_name(image->name) + ".h1Image"s;
					assetmanager::dumper write;
					if (!write.open(path))
					{
						return;
					}

					write.dump_single(image);
					write.dump_string(image->name);
					write.close();
				}
			}

#define COPY_VALUE_GFXIMAGE(name) \
		new_asset->name = asset->name; \

#define COPY_ARR_GFXIMAGE(name) \
		std::memcpy(new_asset->name, asset->name, sizeof(new_asset->name)); \

			zonetool::h1::GfxImage* convert(GfxImage* asset, utils::memory::allocator& allocator)
			{
				auto new_asset = allocator.allocate<zonetool::h1::GfxImage>();

				std::memcpy(new_asset, asset, sizeof(GfxImage));

				// TODO
				if (asset->mapType == MAPTYPE_CUBE_ARRAY)
				{
					ZONETOOL_INFO("using MAPTYPE_NONE instead of MAPTYPE_CUBE_ARRAY for IW7 -> H1 conversion");
					asset->mapType = MAPTYPE_NONE;
				}
				new_asset->mapType = static_cast<zonetool::h1::MapType>(asset->mapType);
				new_asset->semantic = mapped_semantics[asset->semantic];
				new_asset->category = convert_iw7_to_h1_category(asset->category);
				COPY_VALUE_GFXIMAGE(flags);
				std::memcpy(new_asset->picmip.platform, asset->picmip.platform, sizeof(asset->picmip.platform));

				COPY_ARR_GFXIMAGE(pixelData);
				std::memcpy(new_asset->streams, asset->streams, 4 * sizeof(GfxImageStreamData));
				COPY_VALUE_GFXIMAGE(name);

				return new_asset;
			}

			void dump(GfxImage* asset)
			{
				if (asset->streamed)
				{
					dump_streamed_image(asset);
					return;
				}

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

				write.close();
			}
		}
	}
}
