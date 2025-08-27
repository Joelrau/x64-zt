#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "gfximage.hpp"

#include "zonetool/h1/assets/gfximage.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include <utils/io.hpp>
#include <utils/cryptography.hpp>
#include <utils/flags.hpp>
#include <lz4.h>

namespace zonetool::iw7
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

			namespace
			{
				size_t align_value(size_t value, unsigned int alignment)
				{
					const auto diff = value % alignment;
					return diff != 0
						? value + (alignment - diff)
						: value;
				}

				const char* align_value(const char* value, unsigned int alignment)
				{
					return reinterpret_cast<const char*>(
						align_value(reinterpret_cast<size_t>(value), alignment));
				}
			}

			std::vector<std::uint8_t> decompress_lz4_block(const void* data, const size_t size)
			{
				std::vector<std::uint8_t> out_buffer;

				auto data_ptr = reinterpret_cast<const char*>(data);
				auto end_ptr = data_ptr + size;

				const auto write = [&](void* data, const size_t len)
				{
					for (auto i = 0ull; i < len; i++)
					{
						out_buffer.push_back(reinterpret_cast<char*>(data)[i]);
					}
				};

				auto first_block = true;

				XBlockCompressionDataHeader header{};

				while (data_ptr < end_ptr)
				{
					if (first_block)
					{
						header = *reinterpret_cast<XBlockCompressionDataHeader*>(
							reinterpret_cast<size_t>(data_ptr));

						data_ptr += sizeof(XBlockCompressionDataHeader);
					}

					XBlockCompressionBlockHeader block_header{};
					block_header = *reinterpret_cast<XBlockCompressionBlockHeader*>(reinterpret_cast<size_t>(data_ptr));
					data_ptr += sizeof(XBlockCompressionBlockHeader);

					if (header.blockSizeAndType.compressionType != 4)
					{
						throw std::runtime_error("invalid compression type");
					}

					std::string buffer;
					buffer.resize(block_header.uncompressedSize);

					const auto read_count = static_cast<unsigned int>(LZ4_decompress_safe(data_ptr, buffer.data(),
						block_header.compressedSize, static_cast<unsigned int>(block_header.uncompressedSize)));

					if (read_count != block_header.uncompressedSize)
					{
						throw std::runtime_error("bad read");
					}

					first_block = false;

					data_ptr += block_header.compressedSize;
					data_ptr = align_value(data_ptr, 4);

					write(buffer.data(), buffer.size());
				}

				return out_buffer;
			}

			void dump_streamed_image(zonetool::h1::GfxImage* image, bool is_self = false, bool dump_dds = false)
			{
				const auto index = (*g_streamZoneMem)->streamed_image_index;
				for (auto i = 0u; i < 4; i++)
				{
					const auto stream_file = &(*g_streamZoneMem)->streamed_images[index + i];

					std::string filename = utils::string::va("imagefile%d.pak", stream_file->fileIndex);
					if (is_self)
					{
						filename = utils::string::va("%s.pak", filesystem::get_fastfile().data());
					}
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

					XFileCompressorHeader compress_header{};
					compress_header = *(XFileCompressorHeader*)(buffer.data());

					if (compress_header.compressor != DB_COMPRESSOR_BLOCK) __debugbreak();

					try
					{
						auto pixel_data = decompress_lz4_block(buffer.data() + sizeof(XFileCompressorHeader), buffer.size() - sizeof(XFileCompressorHeader));
						const auto name = clean_name(image->name);

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
						else
						{
							std::string parent_path = filesystem::get_dump_path() + "streamed_images\\";
							std::string raw_path = utils::string::va("%s%s_stream%i.pixels", parent_path.data(), name.data(), i);
							std::string pixel_data_str = std::string(pixel_data.begin(), pixel_data.end());
							utils::io::write_file(raw_path, pixel_data_str, false);
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

			void dump_image_dds(zonetool::h1::GfxImage* image, zonetool::iw7::MapType maptype)
			{
				if (image->streamed)
				{
					dump_streamed_image(image, (*g_streamZoneMem)->streamed_images[(*g_streamZoneMem)->streamed_image_index].fileIndex == 431, true);
					return;
				}

				auto* data = image->pixelData;
				std::size_t data_used = 0;

				const auto sides = maptype == MAPTYPE_CUBE || maptype == MAPTYPE_CUBE_ARRAY ? 6 : 1;

				std::vector<DirectX::Image> images{};
				for (int idx = 0; idx < image->numElements; idx++)
				{
					for (int s = 0; s < sides; s++)
					{
						for (int d = 0; d < image->depth; d++)
						{
							for (int i = 0; i < image->levelCount; i++)
							{
								DirectX::Image img{};
								img.pixels = data;

								img.width = std::max(1, image->width >> i);
								img.height = std::max(1, image->height >> i);
								img.format = DXGI_FORMAT(image->imageFormat);

								size_t rowPitch = 0;
								size_t slicePitch = 0;
								DirectX::ComputePitch(img.format, img.width, img.height, rowPitch, slicePitch);

								img.rowPitch = rowPitch;
								img.slicePitch = slicePitch;

								images.push_back(img);

								data += slicePitch;
								data_used += slicePitch;
							}
						}
					}
				}

				if (data_used != image->dataLen1)
				{
					ZONETOOL_WARNING("Failed to dump image \"%s.dds\"", image->name);
					return;
				}

				DirectX::TexMetadata mdata{};
				mdata.width = image->width;
				mdata.height = image->height;
				mdata.depth = image->depth;
				mdata.arraySize = image->numElements * sides;
				mdata.mipLevels = image->levelCount;
				mdata.format = DXGI_FORMAT(image->imageFormat);
				mdata.dimension = image->mapType > 4 ? DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D : (DirectX::TEX_DIMENSION)image->mapType;

				if (maptype == MAPTYPE_CUBE)
				{
					mdata.miscFlags |= DirectX::TEX_MISC_FLAG::TEX_MISC_TEXTURECUBE;
				}

				std::string parent_path = filesystem::get_dump_path() + "images\\";
				std::string spath = parent_path + clean_name(image->name) + ".dds";
				std::wstring wpath(spath.begin(), spath.end());

				if (!std::filesystem::exists(parent_path))
				{
					std::filesystem::create_directories(parent_path);
				}

				auto result = DirectX::SaveToDDSFile(images.data(), images.size(), mdata, DirectX::DDS_FLAGS_NONE, wpath.data());
				if (FAILED(result))
				{
					ZONETOOL_WARNING("Failed to dump image \"%s\"", spath.data());
				}
			}

			static std::unordered_map<std::uint8_t, std::uint8_t> mapped_semantic =
			{
				{zonetool::iw7::TextureSemantic::TS_2D, zonetool::h1::IMG_TS::TS_2D},
				{zonetool::iw7::TextureSemantic::TS_FUNCTION, zonetool::h1::IMG_TS::TS_FUNCTION},
				{zonetool::iw7::TextureSemantic::TS_COLOR_MAP, zonetool::h1::IMG_TS::TS_COLOR_MAP},
				{zonetool::iw7::TextureSemantic::TS_DETAIL_MAP, zonetool::h1::IMG_TS::TS_DETAIL_MAP},
				{zonetool::iw7::TextureSemantic::TS_UNUSED_4, zonetool::h1::IMG_TS::TS_UNUSED_2},
				{zonetool::iw7::TextureSemantic::TS_NORMAL_MAP, zonetool::h1::IMG_TS::TS_NORMAL_MAP},
				{zonetool::iw7::TextureSemantic::TS_UNUSED_6, zonetool::h1::IMG_TS::TS_UNUSED_3},
				{zonetool::iw7::TextureSemantic::TS_UNUSED_7, zonetool::h1::IMG_TS::TS_UNUSED_4},
				{zonetool::iw7::TextureSemantic::TS_SPECULAR_MAP, zonetool::h1::IMG_TS::TS_SPECULAR_MAP},
				{zonetool::iw7::TextureSemantic::TS_SPECULAR_OCCLUSION_MAP, zonetool::h1::IMG_TS::TS_2D},
				{zonetool::iw7::TextureSemantic::TS_UNUSED_10, zonetool::h1::IMG_TS::TS_UNUSED_5},
				{zonetool::iw7::TextureSemantic::TS_THINFILM_MAP, zonetool::h1::IMG_TS::TS_2D},
				{zonetool::iw7::TextureSemantic::TS_DISPLACEMENT_MAP, zonetool::h1::IMG_TS::TS_DISPLACEMENT_MAP},
				{zonetool::iw7::TextureSemantic::TS_PARALLAX_MAP, zonetool::h1::IMG_TS::TS_PARALLAX_MAP},
				{zonetool::iw7::TextureSemantic::TS_COLOR_SPECULAR_MAP, zonetool::h1::IMG_TS::TS_2D},
				{zonetool::iw7::TextureSemantic::TS_NORMAL_OCCLUSSION_GLOSS_MAP, zonetool::h1::IMG_TS::TS_2D},
				{zonetool::iw7::TextureSemantic::TS_ALPHA_REVEAL_THICKNESS_MAP, zonetool::h1::IMG_TS::TS_2D},
			};

			static std::unordered_map<std::uint8_t, std::uint8_t> mapped_category =
			{
				{zonetool::iw7::GfxImageCategory::IMG_CATEGORY_UNKNOWN, zonetool::h1::IMG_CATEGORY::IMG_CATEGORY_UNKNOWN},
				{zonetool::iw7::GfxImageCategory::IMG_CATEGORY_AUTO_GENERATED, zonetool::h1::IMG_CATEGORY::IMG_CATEGORY_AUTO_GENERATED},
				{zonetool::iw7::GfxImageCategory::IMG_CATEGORY_LIGHTMAP, zonetool::h1::IMG_CATEGORY::IMG_CATEGORY_LIGHTMAP},
				{zonetool::iw7::GfxImageCategory::IMG_CATEGORY_LOAD_FROM_FILE, zonetool::h1::IMG_CATEGORY::IMG_CATEGORY_LOAD_FROM_FILE},
				{zonetool::iw7::GfxImageCategory::IMG_CATEGORY_RAW, zonetool::h1::IMG_CATEGORY::IMG_CATEGORY_RAW},
				{zonetool::iw7::GfxImageCategory::IMG_CATEGORY_FIRST_UNMANAGED, zonetool::h1::IMG_CATEGORY::IMG_CATEGORY_FIRST_UNMANAGED},
				{zonetool::iw7::GfxImageCategory::IMG_CATEGORY_RENDERTARGET, zonetool::h1::IMG_CATEGORY::IMG_CATEGORY_RENDERTARGET},
			};

			zonetool::h1::GfxImage* convert(zonetool::iw7::GfxImage* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::GfxImage>();
				new_asset->name = asset->name;

				memcpy(&new_asset->texture, &asset->texture, 24);
				new_asset->imageFormat = asset->imageFormat;

				new_asset->mapType = static_cast<zonetool::h1::MapType>(asset->mapType);
				if (asset->mapType == MapType::MAPTYPE_CUBE_ARRAY)
				{
					new_asset->mapType = zonetool::h1::MapType::MAPTYPE_ARRAY; // fixme
				}

				new_asset->flags = asset->flags & 0x2;
				new_asset->semantic = mapped_semantic[asset->semantic];
				new_asset->category = mapped_category[asset->semantic];

				COPY_VALUE_CAST(picmip);
				COPY_VALUE_CAST(cardMemory);

				COPY_VALUE(dataLen1);
				COPY_VALUE(dataLen2);

				COPY_VALUE(width);
				COPY_VALUE(height);
				COPY_VALUE(depth);

				COPY_VALUE(numElements);
				COPY_VALUE(levelCount);
				new_asset->streamed = asset->streamed ? asset->streamed : 0;

				COPY_VALUE(pixelData);
				COPY_ARR(streams);

				return new_asset;
			}

			void dump(zonetool::iw7::GfxImage* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);

				if (utils::flags::has_flag("dds"))
				{
					dump_image_dds(converted_asset, asset->mapType);
					return;
				}

				if (asset->streamed)
				{
					dump_streamed_image(converted_asset, (*g_streamZoneMem)->streamed_images[(*g_streamZoneMem)->streamed_image_index].fileIndex == 431);
					return;
				}

				zonetool::h1::gfx_image::dump(converted_asset);
			}
		}
	}
}