#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "gfximage.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>
#include "zonetool/utils/compression.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include <zlib.h>

namespace zonetool::iw6
{
	namespace converter::h1
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

			void dump_streamed_image(GfxImage* image, const std::string& ext)
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
						const auto get_pixel_size_for_stream = [](GfxImage* image, int stream) -> int
						{
							if (stream > 0)
							{
								return image->streams[stream].levelCountAndSize.pixelSize - image->streams[stream - 1].levelCountAndSize.pixelSize;
							}

							return image->streams[stream].levelCountAndSize.pixelSize;
						};
						const auto pixel_size = get_pixel_size_for_stream(image, i);

						std::string pixel_data;
						pixel_data.resize(pixel_size);

						uncompress((Bytef*)pixel_data.data(), (uLongf*)&pixel_size, (Bytef*)buffer.data(), (uLongf)buffer.size());

						const auto name = clean_name(image->name);

						{
							std::string parent_path = filesystem::get_dump_path() + "streamed_images\\";
							std::string raw_path = utils::string::va("%s%s_stream%i.pixels", parent_path.data(), name.data(), i);
							utils::io::write_file(raw_path, pixel_data, false);
						}
					}
					catch (...)
					{
						ZONETOOL_ERROR("Failed to dump streamed image \"%s\"", image->name);
					}
				}

				{
					const auto path = "streamed_images\\"s + clean_name(image->name) + ext;
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

			void dump(GfxImage* asset, const std::string& ext)
			{
				if (asset->streamed)
				{
					dump_streamed_image(asset, ext);
					return;
				}

				auto path = "images\\"s + clean_name(asset->name) + ext;
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

			void dump(GfxImage* asset)
			{
				dump(asset, ".h1Image");
			}

			void dump_h2(GfxImage* asset)
			{
				dump(asset, ".h2Image");
			}
		}
	}
}