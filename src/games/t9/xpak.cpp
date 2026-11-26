#include <std_include.hpp>
#include "xpak.hpp"
#include "structs.hpp"

#include <io/game_files.hpp>
#include <log.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::t9::xpak
{
	namespace
	{
		std::optional<std::uint32_t> find_file(pak_index& index, std::unordered_map<std::string, std::optional<std::uint32_t>>& known,
			const std::string& name)
		{
			const auto [file, inserted] = known.try_emplace(name);
			if (inserted && game_files::get().exists(name))
			{
				file->second = static_cast<std::uint32_t>(index.files.size());
				index.files.emplace_back(name);
			}

			return file->second;
		}

		void add_pak(pak_index& index, std::unordered_map<std::string, std::optional<std::uint32_t>>& known, const std::string& pak)
		{
			const auto& files = game_files::get();

			XPakHeader header{};
			if (!files.read(pak, 0, sizeof(header), &header) || header.magic != 'IPAK' || header.version != 0x10 || !header.index.count)
			{
				return;
			}

			std::vector<XPakIndexEntry> entries(header.index.count);
			std::vector<XPakXSub> xsubs(header.xsubCount);
			if (!files.read(pak, header.index.offset, entries.size() * sizeof(XPakIndexEntry), entries.data())
				|| !files.read(pak, sizeof(XPakHeader), xsubs.size() * sizeof(XPakXSub), xsubs.data()))
			{
				ZONETOOL_ERROR("Cannot read the index of \"%s\"", pak.data());
				return;
			}

			const auto stem = pak.substr(0, pak.size() - std::strlen(".xpak"));
			const auto offset_bits = 32 - header.xsubBits;
			for (const auto& entry : entries)
			{
				const auto high = static_cast<std::uint32_t>(entry.packed >> 32);
				const auto size = static_cast<std::uint32_t>((entry.packed >> 1) & 0x3FFFFFFF);
				const auto compressed = (entry.packed & XPAK_ENTRY_COMPRESSED) != 0;

				std::optional<std::uint32_t> file;
				std::uint64_t offset{};
				if (header.mode == XPAK_MODE_XSUB)
				{
					const auto xsub = high >> offset_bits;
					if (xsub >= xsubs.size())
					{
						continue;
					}

					file = find_file(index, known, std::format("{}-{:05}.xsub", stem, xsubs[xsub].number));
					offset = static_cast<std::uint64_t>(high & ((1u << offset_bits) - 1)) << 7;
				}
				else
				{
					file = find_file(index, known, pak);
					offset = static_cast<std::uint64_t>(high) << 7;
				}

				if (file)
				{
					index.entries.emplace_back(entry_location{entry.key, offset, *file, size, compressed});
				}
			}
		}

		pak_index load_index()
		{
			pak_index index;
			std::unordered_map<std::string, std::optional<std::uint32_t>> known;
			for (const auto& pak : game_files::get().list(".xpak"))
			{
				add_pak(index, known, pak);
			}

			std::ranges::stable_sort(index.entries, {}, &entry_location::key);
			const auto duplicates = std::ranges::unique(index.entries, {}, &entry_location::key);
			index.entries.erase(duplicates.begin(), duplicates.end());

			ZONETOOL_INFO("Loaded %zu xpak entries from %zu files", index.entries.size(), index.files.size());
			return index;
		}

		const pak_index& get_index()
		{
			static const auto index = load_index();
			return index;
		}

		std::size_t decompress_block(const XSubBlock block, const std::string& data, const std::size_t position, std::string& output,
			const std::size_t output_offset)
		{
			const std::string_view source(data.data() + position, block.size);
			switch (block.type)
			{
			case XSUB_BLOCK_UNCOMPRESSED:
				std::memcpy(output.data() + output_offset, source.data(), std::min(source.size(), output.size() - output_offset));
				return source.size();
			case XSUB_BLOCK_OODLE:
			case XSUB_BLOCK_OODLE_ALT:
			{
				std::uint32_t size{};
				std::memcpy(&size, source.data(), sizeof(size));
				if (size > output.size() - output_offset)
				{
					throw std::runtime_error("xsub block is larger than the output");
				}

				xfile::oodle::decompress(source.substr(sizeof(size)), output.data() + output_offset, size);
				return size;
			}
			case XSUB_BLOCK_SKIP:
				return 0;
			default:
				throw std::runtime_error(std::format("unsupported xsub block type {}", static_cast<int>(block.type)));
			}
		}

		bool is_chunk_start(const std::string& data, const std::size_t position, const std::size_t output_offset)
		{
			if (position + offsetof(XSubChunkHeader, blocks) > data.size())
			{
				return false;
			}

			const auto header = reinterpret_cast<const XSubChunkHeader*>(data.data() + position);
			return header->outputOffset == output_offset;
		}

		void decompress_chunks(const std::string& data, std::string& output)
		{
			std::size_t position{};
			std::size_t output_offset{};
			while (position + offsetof(XSubChunkHeader, blocks) <= data.size() && output_offset < output.size())
			{
				const auto header = reinterpret_cast<const XSubChunkHeader*>(data.data() + position);
				if (header->outputOffset != output_offset)
				{
					throw std::runtime_error("xsub chunk is out of order");
				}

				auto block_position = position + offsetof(XSubChunkHeader, blocks) + std::max(header->blockCount, 30u) * sizeof(XSubBlock);
				if (block_position > data.size())
				{
					throw std::runtime_error("xsub chunk header is out of bounds");
				}

				for (auto i = 0u; i < header->blockCount; i++)
				{
					const auto block = header->blocks[i];
					if (block_position + block.size > data.size())
					{
						throw std::runtime_error("xsub block is out of bounds");
					}

					output_offset += decompress_block(block, data, block_position, output, output_offset);
					block_position += block.size;
				}

				position = (block_position + 0x7F) & ~static_cast<std::size_t>(0x7F);
				if (!is_chunk_start(data, position, output_offset))
				{
					position = (block_position + 0xFFFFF) & ~static_cast<std::size_t>(0xFFFFF);
				}
			}

			if (output_offset != output.size())
			{
				throw std::runtime_error(std::format("xsub data has 0x{:X} of 0x{:X} bytes", output_offset, output.size()));
			}
		}
	}

	std::optional<std::string> read(const std::uint64_t key, const std::size_t size)
	{
		const auto& index = get_index();
		const auto location = std::ranges::lower_bound(index.entries, key, {}, &entry_location::key);
		if (location == index.entries.end() || location->key != key)
		{
			return {};
		}

		std::string data(location->size, '\0');
		if (!game_files::get().read(index.files[location->file], location->offset, data.size(), data.data()))
		{
			return {};
		}

		if (!location->compressed)
		{
			if (data.size() < size)
			{
				throw std::runtime_error(std::format("xpak data has 0x{:X} of 0x{:X} bytes", data.size(), size));
			}

			data.resize(size);
			return data;
		}

		std::string output(size, '\0');
		decompress_chunks(data, output);
		return output;
	}
}
