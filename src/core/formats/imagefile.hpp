#pragma once

#include "../zonetool.hpp"
#include "compression.hpp"

#include <io/output_cache.hpp>

#include <utils/string.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>

namespace zonetool::imagefile
{
	std::optional<std::vector<XStreamFile>> find_reusable_streams(const std::string& fastfile, const std::string& pak_path, std::uint64_t inputs);
	void save_reusable_streams(const std::string& fastfile, const std::string& pak_path, std::uint64_t inputs, const std::vector<XStreamFile>& streams);

	template <typename T>
	bool has_new_stream(const T* image, const int stream)
	{
		return !image->image_stream_files[stream] && image->image_stream_blocks_paths[stream].has_value();
	}

	template <typename T>
	std::uint64_t get_stream_inputs(const std::vector<T*>& images, const std::uint16_t index, const int ff_version)
	{
		fingerprint result;
		result.add_value(cache_revision::pak_streams).add_value(index).add_value(ff_version);
		for (auto i = 0; i < 4; i++)
		{
			for (const auto& image : images)
			{
				result.add_value(has_new_stream(image, i));
				if (has_new_stream(image, i))
				{
					result.add_file_stamp(image->image_stream_blocks_paths[i].value());
				}
			}
		}

		return result.value();
	}

	template <typename T>
	bool reuse_streams(const std::vector<T*>& images, const std::vector<XStreamFile>& streams, zone_memory* mem)
	{
		std::size_t count{};
		for (auto i = 0; i < 4; i++)
		{
			count += std::ranges::count_if(images, [&](const T* image) { return has_new_stream(image, i); });
		}

		if (count != streams.size())
		{
			return false;
		}

		auto stream = streams.begin();
		for (auto i = 0; i < 4; i++)
		{
			for (const auto& image : images)
			{
				if (image->image_stream_files[i])
				{
					continue;
				}

				image->image_stream_files[i] = mem->allocate<XStreamFile>();
				if (image->image_stream_blocks_paths[i].has_value())
				{
					*image->image_stream_files[i] = *stream++;
				}
			}
		}

		return true;
	}

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
		if (images.size() == 0)
		{
			return;
		}

		const auto pak_path = filesystem::get_output_path() + fastfile + ".pak";
		const auto inputs = get_stream_inputs(images, index, ff_version);
		if (const auto streams = find_reusable_streams(fastfile, pak_path, inputs); streams && reuse_streams(images, *streams, mem))
		{
			ZONETOOL_INFO("Streamed images are unchanged, reusing \"%s\"", pak_path.data());
			return;
		}

		compress_images(images);

		std::vector<XStreamFile> written_streams;
		std::string image_file_buffer;

		ZONETOOL_INFO("Writing imagefile...");

		XPakHeader header{};
		std::memcpy(&header.header, ff_header.data(), ff_header.size());
		header.version = ff_version;

		image_file_buffer.append(reinterpret_cast<char*>(&header), sizeof(XPakHeader));

		const auto write_image_file = [&]
		{
			const auto header = reinterpret_cast<XPakHeader*>(image_file_buffer.data());

			const auto hash_start = reinterpret_cast<std::uint8_t*>(image_file_buffer.data() + sizeof(XPakHeader));
			const auto len = image_file_buffer.size() - sizeof(XPakHeader);

			const auto hash = utils::cryptography::sha256::compute(hash_start, len, false);
			std::memcpy(header->hash.bytes, hash.data(), sizeof(header->hash));

			utils::io::write_file(pak_path, image_file_buffer);
		};

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
				written_streams.emplace_back(*image->image_stream_files[i]);
			}
		}

		write_image_file();
		save_reusable_streams(fastfile, pak_path, inputs, written_streams);
	}
}
