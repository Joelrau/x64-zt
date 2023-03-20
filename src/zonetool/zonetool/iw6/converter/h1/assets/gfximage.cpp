#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
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

			void dump_streamed_image(GfxImage* image)
			{
				const auto stream_file_index = *reinterpret_cast<unsigned int*>(0x141E5BF14);
				const auto stream_files = reinterpret_cast<XStreamFile*>(0x141E5BF40);

				int streami = 4;
				for (auto i = 0u; i < 4; i++)
				{
					const auto stream_file = &stream_files[stream_file_index + i];

					const auto imagefile_path = utils::string::va("zone\\imagefile%d.pak", stream_file->fileIndex);
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
						int pixel_size = image->dataLen2 / streami;
						streami--;

						int pixel_size2 = image->streams[i].pixelSizeAndLevelCount.pixelSize;

						std::string pixel_data;
						pixel_data.resize(pixel_size);

						uncompress((Bytef*)pixel_data.data(), (uLongf*)&pixel_size, (Bytef*)buffer.data(), (uLongf)buffer.size());

						const auto name = clean_name(image->name);

						auto dump_pixels = true;
						if (dump_pixels)
						{
							std::string parent_path = filesystem::get_dump_path() + "streamed_images\\";
							std::string raw_path = utils::string::va("%s%s_stream%i.pixels", parent_path.data(), name.data(), i);
							utils::io::write_file(raw_path, pixel_data, false);
						}

						auto dump_dds = false;
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

			void dump(GfxImage* asset, ZoneMemory* mem)
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