#include <std_include.hpp>

#include "zonetool/h2/converter/h1/include.hpp"
#include "gfximage.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>
#include "zonetool/utils/compression.hpp"

#include "game/h2/game.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include <zlib.h>

namespace zonetool::h2
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

			const auto dump_pixels = true;
			const auto dump_dds = false;

			void dump_streamed_image(zonetool::h2::GfxImage* image)
			{
				for (auto i = 0u; i < 4; i++)
				{
					const auto stream_file = &stream_files[*stream_file_index + i];
					const auto db_fs = ::h2::game::DB_FSInitialize();
					const auto imagefile_path = utils::string::va("imagefile%d.pak", stream_file->fileIndex);
					const auto imagefile = db_fs->vftbl->OpenFile(db_fs, ::h2::game::Sys_Folder::SF_PAKFILE, imagefile_path);
					if (!imagefile || stream_file->offset == 0 || stream_file->offsetEnd == 0)
					{
						continue;
					}

					const auto _0 = gsl::finally([&]
					{
						db_fs->vftbl->Close(db_fs, imagefile);
					});

					std::string buffer;
					const auto size = stream_file->offsetEnd - stream_file->offset;
					buffer.resize(size);
					db_fs->vftbl->Read(db_fs, imagefile, stream_file->offset, size, buffer.data());

					try
					{
						auto pixel_data = compression::lz4::decompress_lz4_block(buffer);
						const auto name = clean_name(image->name);

						if (dump_pixels)
						{
							const auto parent_path = filesystem::get_dump_path() + "streamed_images\\";
							const std::string raw_path = utils::string::va("%s%s_stream%i.pixels",
								parent_path.data(), name.data(), i);
							utils::io::write_file(raw_path, pixel_data, false);
						}

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

							const auto parent_path = filesystem::get_dump_path() + "images\\";
							const std::string spath = utils::string::va("%s\\%s_%dx%d.dds", parent_path.data(),
								name.data(), img.width, img.height);
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

				if (dump_pixels)
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

			zonetool::h1::GfxImage* convert(zonetool::h2::GfxImage* asset, utils::memory::allocator& allocator)
			{
				return nullptr;
			}

			void dump(zonetool::h2::GfxImage* asset)
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