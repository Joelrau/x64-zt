#include <std_include.hpp>
#include "s2.hpp"

#include <io/game_files.hpp>
#include <xfile/xfile.hpp>
#include <utils/cryptography.hpp>

namespace zonetool::s2
{
	namespace
	{
#pragma pack(push, 1)
		struct xpak_toc_header
		{
			char magic[8];
			std::uint32_t version;
			std::uint32_t entry_count;
		};

		struct xpak_toc_entry
		{
			std::uint8_t hash[16];
			std::uint64_t offset;
			std::uint32_t size;
			std::uint16_t file_index;
		};
#pragma pack(pop)

		static_assert(sizeof(xpak_toc_header) == 16);
		static_assert(sizeof(xpak_toc_entry) == 30);

		constexpr std::string_view xpak_toc_magic = "S2ffi300";

		struct xpak_location
		{
			std::filesystem::path pak;
			std::uint64_t offset;
			std::uint32_t size;
		};

		void read_xpak_toc(const std::filesystem::path& path, std::unordered_map<std::string, xpak_location>& entries)
		{
			std::string data;
			if (!game_files::read(path, data) || data.size() < sizeof(xpak_toc_header))
			{
				return;
			}

			const auto header = reinterpret_cast<const xpak_toc_header*>(data.data());
			if (std::string_view(header->magic, sizeof(header->magic)) != xpak_toc_magic
				|| sizeof(xpak_toc_header) + header->entry_count * sizeof(xpak_toc_entry) > data.size())
			{
				ZONETOOL_WARNING("Xpak toc \"%s\" is not valid", path.string().data());
				return;
			}

			const auto toc = reinterpret_cast<const xpak_toc_entry*>(data.data() + sizeof(xpak_toc_header));
			for (auto i = 0u; i < header->entry_count; i++)
			{
				entries.try_emplace(std::string(reinterpret_cast<const char*>(toc[i].hash), sizeof(toc[i].hash)), xpak_location
				{
					.pak = path.parent_path() / std::format("xpakfile{}.pak", toc[i].file_index),
					.offset = toc[i].offset,
					.size = toc[i].size,
				});
			}
		}

		const std::unordered_map<std::string, xpak_location>& xpak_entries()
		{
			static const auto entries = []
			{
				std::unordered_map<std::string, xpak_location> result;
				for (const auto& name : game_files::get().list(".toc"))
				{
					if (std::filesystem::path(name).filename().string().starts_with("xpakfile"))
					{
						read_xpak_toc(game_files::get().path(name), result);
					}
				}

				return result;
			}();

			return entries;
		}

		std::optional<std::string> decode_hash(const char* hex, const std::size_t length)
		{
			const auto nibble = [](const char c) -> int
			{
				if (c >= '0' && c <= '9') return c - '0';
				if (c >= 'a' && c <= 'f') return c - 'a' + 10;
				if (c >= 'A' && c <= 'F') return c - 'A' + 10;
				return -1;
			};

			std::string result;
			for (auto i = 0u; i + 1 < length; i += 2)
			{
				const auto high = nibble(hex[i]);
				const auto low = nibble(hex[i + 1]);
				if (high < 0 || low < 0)
				{
					return {};
				}

				result.push_back(static_cast<char>((high << 4) | low));
			}

			return result;
		}
	}

	std::optional<std::string> read_shared_stream(const XSurfaceShared* shared)
	{
		const auto hash = decode_hash(shared->hash, sizeof(shared->hash));
		if (!hash)
		{
			return {};
		}

		const auto& entries = xpak_entries();
		const auto entry = entries.find(*hash);
		if (entry == entries.end())
		{
			return {};
		}

		std::string data;
		data.resize(entry->second.size);
		if (!game_files::read(entry->second.pak, entry->second.offset, entry->second.size, data.data()))
		{
			return {};
		}

		return xfile::decompress_s2_stream(data);
	}

	std::string surface_shared_key(const XSurfaceShared* shared)
	{
		const std::string hash(shared->hash, strnlen(shared->hash, sizeof(shared->hash)));
		if (!hash.empty())
		{
			return hash;
		}

		auto key = utils::cryptography::sha1::compute(std::string(shared->data, shared->data ? shared->dataSize : 0)
			+ std::string(shared->subdivData, shared->subdivData ? shared->subdivCount * surface_subdiv_size : 0), true);
		return "data_" + key;
	}

	void xsurface_shared::init(const std::string& name, zone_memory* mem)
	{
		throw std::runtime_error(std::format("xsurfshared \"{}\" cannot be added by name, add its xmodelsurfs", name));
	}

	void xsurface_shared::init(void* asset, zone_memory* mem)
	{
		this->asset_ = static_cast<XSurfaceShared*>(asset);
		this->name_ = surface_shared_key(this->asset_);
	}

	void xsurface_shared::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		if (data->flags & XSURFACE_SHARED_STREAMED)
		{
			if (data->dataSize)
			{
				buf->push_stream(XFILE_BLOCK_RUNTIME_UNREAD);
				buf->align(255);
				buf->write_stream(data->data, data->dataSize);
				buf->clear_pointer(&dest->data);
				buf->pop_stream();
			}
		}
		else if (data->data)
		{
			buf->align(255);
			buf->write(data->data, data->dataSize);
			buf->clear_pointer(&dest->data);
		}

		if (data->subdivData)
		{
			buf->align(15);
			buf->write(data->subdivData, data->subdivCount * surface_subdiv_size);
			buf->clear_pointer(&dest->subdivData);
		}

		buf->pop_stream();
	}
}
