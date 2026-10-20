#include <std_include.hpp>
#include "../s2.hpp"
#include "image_pak.hpp"

namespace zonetool::s2::image_pak
{
	std::optional<std::vector<toc_entry>> parse_toc(const std::string_view data)
	{
		if (data.size() < sizeof(toc_header))
		{
			return {};
		}

		const auto* header = reinterpret_cast<const toc_header*>(data.data());
		if (std::string_view(header->magic, sizeof(header->magic)) != toc_magic
			|| sizeof(toc_header) + std::size_t{header->entry_count} * sizeof(toc_entry) > data.size())
		{
			return {};
		}

		const auto* entries = reinterpret_cast<const toc_entry*>(data.data() + sizeof(toc_header));
		return std::vector<toc_entry>(entries, entries + header->entry_count);
	}

	std::string get_pak_name(const std::string& zone_name)
	{
		return zone_name + ".pak";
	}

	void write(const std::string& zone_name, const std::filesystem::path& folder, const std::span<const image_streams> images,
		std::vector<XStreamFileKey>& keys)
	{
		const auto pak_path = folder / get_pak_name(zone_name);

		std::vector<std::filesystem::path> streams;
		for (const auto& image : images)
		{
			for (const auto& pixels : image.pixels)
			{
				if (pixels)
				{
					streams.emplace_back(*pixels);
				}
			}
		}

		if (streams.empty())
		{
			std::filesystem::remove(pak_path);
			return;
		}

		// after every fastfile load the game checks the rsa signature of each open S1ff pak (sub_140084BD0),
		// but leaves one that starts with the toc magic alone
		ZONETOOL_INFO("Writing %zu image streams to \"%s\"...", streams.size(), pak_path.string().data());
		const auto locations = xfile::write_s2_pak(pak_path, toc_magic, streams);

		auto location = locations.begin();
		for (const auto& image : images)
		{
			for (auto i = 0u; i < toc_stream_count; i++)
			{
				if (!image.pixels[i])
				{
					continue;
				}

				XStreamFileLocation stream{};
				stream.offset = location->offset;
				stream.size = location->size;
				stream.fileIndex = XStreamFileLocation::zone_pak_index;
				keys.at(image.first_key + i) = std::bit_cast<XStreamFileKey>(stream);
				++location;
			}
		}
	}
}
