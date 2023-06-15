#pragma once

#include "memory.hpp"
#include "utils.hpp"
#include "compression.hpp"

#include <utils/string.hpp>
#include <utils/io.hpp>

namespace zonetool::imagefile
{
	template <typename T>
	void compress_images(const std::vector<T*>& images)
	{
		ZONETOOL_INFO("Compressing images...");

		const auto max_threads = std::thread::hardware_concurrency() * 2;
		const auto images_per_thread = std::max(1, static_cast<int>(images.size() / max_threads));

		std::vector<std::thread> threads;

		const auto start_thread = [&](const int index, const int count)
		{
			threads.emplace_back([&, index, count]
			{
				for (auto i = index; i < index + count; i++)
				{
					const auto image = images[i];
					for (auto o = 0; o < 4; o++)
					{
						auto& path = image->image_stream_blocks_paths[o];
						if (!path.has_value())
						{
							continue;
						}

						const auto block = utils::io::read_file(path.value());
						const auto compressed = compression::lz4::compress_lz4_block(block);
						image->image_stream_blocks[o].emplace(compressed);
					}
				}
			});
		};

		auto images_left = static_cast<int>(images.size());
		auto index = 0;
		while (images_left > 0)
		{
			const auto count = std::min(images_left, images_per_thread);
			start_thread(index, count);
			index += count;
			images_left -= count;
		}

		for (auto& thread : threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}

	template <typename T>
	void generate(const std::string& fastfile, std::uint16_t index, int ff_version, const std::string& ff_header,
		std::vector<T*> images, zone_memory* mem)
	{
		compress_images(images);

		if (images.size() == 0)
		{
			return;
		}

		std::string image_file_buffer;

		ZONETOOL_INFO("Writing imagefile...");

		const auto init_image_file = [&]
		{
			image_file_buffer.clear();

			XPakHeader header{};
			std::memcpy(&header.header, ff_header.data(), ff_header.size());
			header.version = ff_version;

			image_file_buffer.append(reinterpret_cast<char*>(&header), sizeof(XPakHeader));
		};

		const auto write_image_file = [&]
		{
			const auto save_path = utils::io::directory_exists("zone") ? "zone/" : "";
			const auto name = utils::string::va("%s%s.pak", save_path, fastfile.data(), index);
			utils::io::write_file(name, image_file_buffer);
			init_image_file();
		};

		init_image_file();

		for (auto i = 0; i < 4; i++)
		{
			for (const auto& image : images)
			{
				if (image->image_stream_files[i])
				{
					continue;
				}

				image->image_stream_files[i] = mem->allocate<XStreamFile>();

				if (!image->image_stream_blocks[i].has_value())
				{
					continue;
				}

				const auto& data = image->image_stream_blocks[i].value();
				const auto offset = image_file_buffer.size();
				image_file_buffer.append(reinterpret_cast<const char*>(data.data()), data.size());
				const auto offset_end = image_file_buffer.size();
				image->image_stream_blocks[i].reset();

				image->image_stream_files[i]->fileIndex = index;
				image->image_stream_files[i]->offset = offset;
				image->image_stream_files[i]->offsetEnd = offset_end;
			}
		}

		write_image_file();
	}
}
