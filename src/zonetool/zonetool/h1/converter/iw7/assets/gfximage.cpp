#include <std_include.hpp>

#include "zonetool/h1/converter/iw7/include.hpp"
#include "gfximage.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>
#include "zonetool/utils/compression.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include <zlib.h>

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace gfximage
		{
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

			zonetool::iw7::TextureSemantic convert(IMG_TS ts)
			{
				switch (ts)
				{
				case TS_2D:
					return zonetool::iw7::TS_2D;
					break;
				case TS_FUNCTION:
					return zonetool::iw7::TS_FUNCTION;
					break;
				case TS_COLOR_MAP:
					return zonetool::iw7::TS_COLOR_MAP;
					break;
				case TS_DETAIL_MAP:
					return zonetool::iw7::TS_DETAIL_MAP;
					break;
				case TS_NORMAL_MAP:
					return zonetool::iw7::TS_NORMAL_MAP;
					break;
				case TS_UNUSED_2:
				case TS_UNUSED_3:
				case TS_UNUSED_4:
				case TS_UNUSED_5:
					return zonetool::iw7::TS_UNUSED_4;
					break;
				case TS_SPECULAR_MAP:
					return zonetool::iw7::TS_SPECULAR_MAP;
					break;
				case TS_OCEANFLOW_DISPLACEMENT_MAP:
					return zonetool::iw7::TS_DISPLACEMENT_MAP;
				case TS_WATER_MAP:
					return zonetool::iw7::TS_UNUSED_10;
					break;
				case TS_OCEAN_DISPLACEMENT_MAP:
					return zonetool::iw7::TS_DISPLACEMENT_MAP;
					break;
				case TS_DISPLACEMENT_MAP:
					return zonetool::iw7::TS_DISPLACEMENT_MAP;
					break;
				case TS_PARALLAX_MAP:
					return zonetool::iw7::TS_PARALLAX_MAP;
					break;
				}

				return zonetool::iw7::TS_2D;
			}

			zonetool::iw7::GfxImage* convert(GfxImage* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::iw7::GfxImage>();
				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(imageFormat);
				new_asset->flags = 0; // convert? i think the flags are not really used tho
				COPY_VALUE_CAST(mapType); // this should be the same up till the new value
				new_asset->semantic = convert((IMG_TS)asset->semantic);
				new_asset->category = zonetool::iw7::GfxImageCategory(asset->category > 4 ? asset->category + 1 : asset->category);
				COPY_VALUE_CAST(picmip);
				COPY_VALUE_CAST(cardMemory);
				COPY_VALUE(dataLen1);
				COPY_VALUE(dataLen2);
				COPY_VALUE(width);
				COPY_VALUE(height);
				COPY_VALUE(depth);
				COPY_VALUE(numElements);
				COPY_VALUE(levelCount);
				COPY_VALUE(streamed);
				new_asset->unknown1 = 0;
				REINTERPRET_CAST_SAFE(pixelData);
				COPY_ARR(streams);

				return new_asset;
			}

			void dump_streamed_image(GfxImage* image)
			{
				const auto name = clean_name(image->name);

				try
				{
					for (auto i = 0u; i < 4; i++)
					{
						std::string parent_path = filesystem::get_dump_path() + "streamed_images\\";
						std::string raw_path = utils::string::va("%s%s_stream%i.pixels", parent_path.data(), name.data(), i);
						if(utils::io::file_exists(raw_path))
						{
							// already dumped this stream, skip it to speedup the process
							continue;
						}

						const auto stream_file = &stream_files[*stream_file_index + i];

						const char* filename = nullptr;
						if (stream_file->fileIndex == 96)
						{
							const auto& fastfile = filesystem::get_fastfile();
							filename = utils::string::va("%s.pak", fastfile.data());
						}
						else
						{
							filename = utils::string::va("imagefile%d.pak", stream_file->fileIndex);
						}

						const auto folder = filesystem::get_zone_path(filename);

						const auto imagefile_path = utils::string::va("%s%s", folder.data(), filename);
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

						auto pixel_data = compression::lz4::decompress_lz4_block(buffer);
						utils::io::write_file(raw_path, pixel_data, false);

					}
				}
				catch (...)
				{
					ZONETOOL_ERROR("Failed to dump streamed image \"%s\"", image->name);
				}

				utils::memory::allocator allocator;
				auto* new_asset = convert(image, allocator);

				const auto path = "streamed_images\\"s + name + ".iw7Image"s;
				assetmanager::dumper write;
				if (!write.open(path))
				{
					return;
				}

				write.dump_single(new_asset);
				write.dump_string(new_asset->name);
				write.close();
			}

			void dump(GfxImage* asset)
			{
				if (asset->streamed)
				{
					dump_streamed_image(asset);
					return;
				}

				utils::memory::allocator allocator;
				auto* new_asset = convert(asset, allocator);

				auto path = "images\\"s + clean_name(new_asset->name) + ".iw7Image"s;
				assetmanager::dumper write;
				if (!write.open(path))
				{
					return;
				}

				write.dump_single(new_asset);
				write.dump_string(new_asset->name);

				if (new_asset->pixelData)
				{
					write.dump_array(new_asset->pixelData, new_asset->dataLen1);
				}

				write.close();
			}
		}
	}
}