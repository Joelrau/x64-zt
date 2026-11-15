#include <std_include.hpp>
#include "xpak.hpp"
#include "structs.hpp"

#include <io/game_files.hpp>
#include <log.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::iw8::xpak
{
	namespace
	{
		struct entry_location
		{
			std::string pak;
			std::uint64_t offset;
			std::uint64_t size;
		};

		std::unordered_map<std::uint64_t, entry_location> load_index()
		{
			const auto& files = game_files::get();

			auto paks = files.list(".xpak");
			std::ranges::stable_sort(paks, {}, [](const std::string& name) { return name.find_first_of("\\/") != std::string::npos; });

			std::unordered_map<std::uint64_t, entry_location> index;
			for (const auto& pak : paks)
			{
				XPakFileHeader header{};
				if (!files.read(pak, 0, sizeof(header), &header) || header.magic != 'IPAK' || !header.index.count)
				{
					continue;
				}

				std::vector<XPakIndexEntry> entries(header.index.count);
				if (!files.read(pak, header.index.offset, entries.size() * sizeof(XPakIndexEntry), entries.data()))
				{
					ZONETOOL_ERROR("Cannot read the index of \"%s\"", pak.data());
					continue;
				}

				for (const auto& entry : entries)
				{
					index.try_emplace(entry.key, entry_location{pak, header.data.offset + entry.offset, entry.size});
				}
			}

			ZONETOOL_INFO("Loaded %zu xpak entries", index.size());
			return index;
		}

		const std::unordered_map<std::uint64_t, entry_location>& get_index()
		{
			static const auto index = load_index();
			return index;
		}

		void decompress_blocks(const std::string& data, std::string& output)
		{
			std::size_t position{};
			while (position + sizeof(XPakBlockHeader) <= data.size())
			{
				const auto header = reinterpret_cast<const XPakBlockHeader*>(data.data() + position);
				if (!header->blockCount || header->blockCount > std::size(header->blocks))
				{
					break;
				}

				position += sizeof(XPakBlockHeader);
				auto output_offset = static_cast<std::size_t>(header->outputOffset);
				for (auto i = 0u; i < header->blockCount; i++)
				{
					const auto& block = header->blocks[i];
					const auto aligned_size = (block.compressedSize + 3) & ~3u;
					if (position + block.compressedSize > data.size())
					{
						throw std::runtime_error("xpak block is out of bounds");
					}

					const std::string_view source(data.data() + position, block.compressedSize);
					switch (block.type)
					{
					case XPAK_BLOCK_UNCOMPRESSED:
						std::memcpy(output.data() + output_offset, source.data(), std::min(source.size(), output.size() - output_offset));
						output_offset += source.size();
						break;
					case XPAK_BLOCK_OODLE:
					case XPAK_BLOCK_OODLE_ALT:
					{
						const auto size = std::min<std::size_t>(0x3FFE0, output.size() - output_offset);
						xfile::oodle::decompress(source, output.data() + output_offset, size);
						output_offset += size;
						break;
					}
					case XPAK_BLOCK_SKIP:
						break;
					default:
						throw std::runtime_error(std::format("unsupported xpak block type {}", static_cast<int>(block.type)));
					}

					position += aligned_size;
				}

				position = (position + 0x7F) & ~static_cast<std::size_t>(0x7F);
			}
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
		if (!game_files::get().read(location->second.pak, location->second.offset, data.size(), data.data()))
		{
			return {};
		}

		std::string output(size, '\0');
		decompress_blocks(data, output);
		return output;
	}
}
