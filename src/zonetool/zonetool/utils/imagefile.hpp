#pragma once

#include "memory.hpp"
#include "utils.hpp"

#include <utils/string.hpp>
#include <utils/io.hpp>

namespace zonetool::imagefile
{
	template <typename T>
	void generate(const std::string& fastfile, std::uint16_t index, int ff_version, const std::string& ff_header,
		std::vector<T*> images, ZoneMemory* mem)
	{
		if (images.size() == 0)
		{
			return;
		}

		std::string image_file_buffer;

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

				image->image_stream_files[i] = mem->Alloc<XStreamFile>();

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
