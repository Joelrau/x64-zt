#include <std_include.hpp>
#include "s2.hpp"

#include <io/game_files.hpp>
#include <xfile/xfile.hpp>

namespace zonetool::s2
{
	namespace
	{
#pragma pack(push, 1)
		struct toc_header
		{
			char magic[8];
			std::uint32_t version;
			std::uint32_t hash_size;
			std::uint32_t entry_count;
		};

		struct toc_stream
		{
			std::uint64_t offset;
			std::uint32_t size;
			std::uint16_t file_index;
		};

		struct toc_entry
		{
			std::uint64_t hash[2];
			toc_stream streams[4];
		};
#pragma pack(pop)

		static_assert(sizeof(toc_header) == 20);
		static_assert(sizeof(toc_entry) == 72);

		constexpr std::string_view toc_magic = "S2ffi100";

		struct image_stream_location
		{
			std::filesystem::path pak;
			std::uint64_t offset;
			std::uint32_t size;
		};

		struct image_stream_entry
		{
			std::filesystem::path folder;
			toc_entry entry;
		};

		void read_toc(const std::filesystem::path& path, std::unordered_map<std::uint64_t, image_stream_entry>& entries)
		{
			std::string data;
			if (!game_files::read(path, data) || data.size() < sizeof(toc_header))
			{
				return;
			}

			const auto header = reinterpret_cast<const toc_header*>(data.data());
			if (std::string_view(header->magic, sizeof(header->magic)) != toc_magic
				|| sizeof(toc_header) + header->entry_count * sizeof(toc_entry) > data.size())
			{
				ZONETOOL_WARNING("Image toc \"%s\" is not valid", path.string().data());
				return;
			}

			const auto toc = reinterpret_cast<const toc_entry*>(data.data() + sizeof(toc_header));
			for (auto i = 0u; i < header->entry_count; i++)
			{
				entries.try_emplace(toc[i].hash[0], path.parent_path(), toc[i]);
			}
		}

		const std::unordered_map<std::uint64_t, image_stream_entry>& image_stream_entries()
		{
			static const auto entries = []
			{
				std::unordered_map<std::uint64_t, image_stream_entry> result;
				for (const auto& name : game_files::get().list(".toc"))
				{
					if (std::filesystem::path(name).filename().string().starts_with("imagefile"))
					{
						read_toc(game_files::get().path(name), result);
					}
				}

				return result;
			}();

			return entries;
		}

		std::optional<image_stream_location> find_image_stream(const XStreamFileKey& key)
		{
			const auto& entries = image_stream_entries();
			const auto entry = entries.find(key.hash);
			if (entry == entries.end() || key.index >= std::size(entry->second.entry.streams))
			{
				return {};
			}

			const auto& stream = entry->second.entry.streams[key.index];
			if (!stream.size)
			{
				return {};
			}

			return image_stream_location
			{
				.pak = entry->second.folder / std::format("imagefile{}.pak", stream.file_index),
				.offset = stream.offset,
				.size = stream.size,
			};
		}
	}

	std::optional<std::string> read_image_stream(const XStreamFileKey& key)
	{
		if (!key.hash)
		{
			return {};
		}

		const auto location = find_image_stream(key);
		if (!location)
		{
			return {};
		}

		std::string data;
		data.resize(location->size);
		if (!game_files::read(location->pak, location->offset, location->size, data.data()))
		{
			return {};
		}

		return xfile::decompress_s2_stream(data);
	}
}
