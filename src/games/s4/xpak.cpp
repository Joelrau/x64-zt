#include <std_include.hpp>
#include "xpak.hpp"
#include "structs.hpp"

#include <io/game_files.hpp>
#include <log.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::s4::xpak
{
	namespace
	{
		struct entry_location
		{
			std::filesystem::path file;
			std::uint64_t offset;
			std::uint64_t size;
			bool compressed;
		};

		void add_entries(std::unordered_map<std::uint64_t, entry_location>& index, const std::filesystem::path& index_file, const std::filesystem::path& data_file)
		{
			XSubFileHeader header{};
			if (!game_files::read(index_file, 0, sizeof(header), &header) || header.magic != 'IPAK' || !header.hashCount)
			{
				return;
			}

			if (header.hashOffset + header.hashCount * sizeof(XSubHashEntry) > header.fileSize)
			{
				ZONETOOL_ERROR("The hash table of \"%s\" is out of bounds", index_file.string().data());
				return;
			}

			std::vector<XSubHashEntry> entries(header.hashCount);
			if (!game_files::read(index_file, header.hashOffset, entries.size() * sizeof(XSubHashEntry), entries.data()))
			{
				ZONETOOL_ERROR("Cannot read the hash table of \"%s\"", index_file.string().data());
				return;
			}

			for (const auto& entry : entries)
			{
				index.try_emplace(entry.key, entry_location{data_file, static_cast<std::uint64_t>(entry.offset) << 7, entry.size, entry.compressed != 0});
			}
		}

		std::unordered_map<std::uint64_t, entry_location> load_index()
		{
			const auto& files = game_files::get();

			std::unordered_map<std::uint64_t, entry_location> index;
			for (const auto& file : files.list(".xsub"))
			{
				add_entries(index, file, file);
			}

			const auto xsub_count = index.size();
			const auto cache = files.path("") / ".." / "xpak_cache";
			for (auto i = 0; std::filesystem::is_regular_file(cache / std::format("cache_{}_1.xpak", i)); i++)
			{
				const auto file = cache / std::format("cache_{}_1.xpak", i);
				add_entries(index, file, std::filesystem::path(file).replace_extension(".xpakdata"));
			}

			ZONETOOL_INFO("Loaded %zu xsub entries and %zu xpak_cache entries", xsub_count, index.size() - xsub_count);
			return index;
		}

		const std::unordered_map<std::uint64_t, entry_location>& get_index()
		{
			static const auto index = load_index();
			return index;
		}

		bool decompress_chunk(const std::uint64_t key, const std::string_view chunk, std::string& output)
		{
			const auto header = reinterpret_cast<const XSubBlockHeader*>(chunk.data());
			if (header->magic != 0xF01D || header->key != key
				|| sizeof(XSubBlockHeader) + header->blockCount * sizeof(XSubBlock) > chunk.size())
			{
				return false;
			}

			const auto blocks = reinterpret_cast<const XSubBlock*>(chunk.data() + sizeof(XSubBlockHeader));
			for (auto i = 0u; i < header->blockCount; i++)
			{
				const auto& block = blocks[i];
				if (static_cast<std::size_t>(block.decompressedOffset) + block.decompressedSize > output.size()
					|| static_cast<std::size_t>(block.compressedOffset) + block.compressedSize > chunk.size())
				{
					return false;
				}

				const std::string_view source(chunk.data() + block.compressedOffset, block.compressedSize);
				switch (block.type)
				{
				case XSUB_BLOCK_UNCOMPRESSED:
					std::memcpy(output.data() + block.decompressedOffset, source.data(), std::min(source.size(), static_cast<std::size_t>(block.decompressedSize)));
					break;
				case XSUB_BLOCK_OODLE:
					xfile::oodle::decompress(source, output.data() + block.decompressedOffset, block.decompressedSize);
					break;
				default:
					ZONETOOL_ERROR("Unsupported xsub block type %d", static_cast<int>(block.type));
					return false;
				}
			}

			return true;
		}

		bool decompress_chunks(const std::uint64_t key, const std::string& data, std::string& output)
		{
			std::size_t offset{};
			do
			{
				if (offset + sizeof(XSubBlockHeader) > data.size())
				{
					return false;
				}

				const auto header = reinterpret_cast<const XSubBlockHeader*>(data.data() + offset);
				if (header->nextOffset && (header->nextOffset <= offset || header->nextOffset > data.size()))
				{
					return false;
				}

				const auto end = header->nextOffset ? header->nextOffset : data.size();
				if (!decompress_chunk(key, std::string_view(data).substr(offset, end - offset), output))
				{
					return false;
				}

				offset = header->nextOffset;
			} while (offset);

			return true;
		}
	}

	std::optional<std::string> read(const std::uint64_t key, const std::size_t size)
	{
		const auto& index = get_index();
		const auto location = index.find(key);
		if (location == index.end())
		{
			return {};
		}

		std::string data(location->second.size, '\0');
		if (!game_files::read(location->second.file, location->second.offset, data.size(), data.data()))
		{
			return {};
		}

		if (!location->second.compressed)
		{
			data.resize(std::min(data.size(), size));
			return data;
		}

		std::string output(size, '\0');
		if (!decompress_chunks(key, data, output))
		{
			return {};
		}

		return output;
	}
}
