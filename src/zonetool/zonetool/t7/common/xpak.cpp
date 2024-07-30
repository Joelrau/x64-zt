#include "std_include.hpp"
#include "zonetool/utils/utils.hpp"
#include "xpak.hpp"

#include "../structs.hpp"
#include "../variables.hpp"

#include "utils/io.hpp"
#include "utils/string.hpp"

#include "lz4.h"

namespace zonetool::t7
{
	namespace xpak
	{
		namespace
		{
#pragma pack(push, 1)
			struct XPakSection
			{
				int64_t itemCount;
				int64_t offset;
				int64_t size;
			};

			struct XPakHeader
			{
				uint32_t Magic;
				uint16_t Unknown1;
				uint16_t Version;
				uint64_t Unknown2;
				uint64_t Size;
				uint64_t FileCount;
				uint64_t DataOffset;
				uint64_t DataSize;
				uint64_t HashCount;
				uint64_t HashOffset;
				uint64_t HashSize;
				uint64_t Unknown3;
				uint64_t UnknownOffset;
				uint64_t Unknown4;
				uint64_t IndexCount;
				uint64_t IndexOffset;
				uint64_t IndexSize;
			};

			struct XPakHashEntry
			{
				uint64_t Key;
				uint64_t Offset;
				uint64_t Size;
			};

			struct XPakDataHeader
			{
				// Count and offset
				uint32_t Count;
				uint32_t Offset;

				// The commands tell what each block of data does
				uint32_t Commands[30];
			};
#pragma pack(pop)

			struct PackageCacheObject
			{
				uint64_t Offset;
				uint64_t CompressedSize;
				uint64_t UncompressedSize;
				std::string PackageFile;
			};

			std::unordered_map<std::string, std::unordered_map<uint64_t, PackageCacheObject>> xpak_cache_map;

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

			// this shit is so broken
			std::vector<std::uint8_t> extract(const void* data, const size_t size, const size_t decompressedSize)
			{
				std::vector<std::uint8_t> out_buffer;
				out_buffer.reserve(decompressedSize); // Reserve space to avoid frequent reallocations

				auto data_ptr = reinterpret_cast<const char*>(data);
				auto data_end = data_ptr + size;

				const auto write = [&](const void* data, const size_t len)
				{
					const char* char_data = reinterpret_cast<const char*>(data);
					for (size_t i = 0; i < len; i++)
					{
						out_buffer.push_back(static_cast<std::uint8_t>(char_data[i]));
					}
				};

				while (data_ptr < data_end)
				{
					if (data_ptr + sizeof(XPakDataHeader) > data_end)
					{
						// Handle error: not enough data for header
						break;
					}

					XPakDataHeader header{};
					std::memcpy(&header, data_ptr, sizeof(XPakDataHeader));
					data_ptr += sizeof(XPakDataHeader);

					if (header.Count > 30)
					{
						// fucked up shit fuck
						data_ptr -= sizeof(XPakDataHeader);
						data_ptr++;
						continue;
					}

					for (uint32_t i = 0; i < header.Count; i++)
					{
						// Unpack the command information
						const size_t blockSize = (header.Commands[i] & 0xFFFFFF);
						const size_t flag = (header.Commands[i] >> 24);

						std::string buffer;

						switch (flag)
						{
						case 0x3: // compressed (lz4)
						{
							buffer.resize(0x10000);
							const size_t decompressedResult = LZ4_decompress_safe(data_ptr, buffer.data(), static_cast<int>(blockSize), static_cast<int>(buffer.size()));
							if (decompressedResult < 0 || decompressedResult > std::numeric_limits<std::uint32_t>::max())
							{
								// Handle decompression error
								__debugbreak();
							}
							buffer.resize(decompressedResult);
							break;
						}
						case 0x0: // raw data
						{
							if (data_ptr + blockSize > data_end)
							{
								// Handle error: not enough data for block
								break;
							}
							buffer.resize(blockSize);
							std::memcpy(buffer.data(), data_ptr, blockSize);
							break;
						}
						default:
						{
							data_ptr = data_end; // idk why this shit is so fucked
							break;
						}
						}

						data_ptr += blockSize;

						write(buffer.data(), buffer.size());
					}

					//data_ptr = align_value(data_ptr, 0x80);
				}

				return out_buffer;
			}

			std::vector<std::uint8_t> decompress_xpak_data(std::string compressed_data, size_t decompressedSize)
			{
				try
				{
					return extract(compressed_data.data(), compressed_data.size(), decompressedSize);
				}
				catch (const std::exception& e)
				{
					ZONETOOL_FATAL("%s", e.what());
				}
			}

			void populate_xpak_cache_internal(const std::string& pak_path)
			{
				if (xpak_cache_map.contains(pak_path)) return;

				filesystem::file file(pak_path);
				file.open("rb", false, false);

				if (!file.get_fp()) return;

				XPakHeader header{};
				file.read(&header);

				if (header.Magic != 0x4950414b)
				{
					__debugbreak();
				}

				file.seek(header.HashOffset, SEEK_SET);
				XPakHashEntry entry{};

				for (uint64_t i = 0; i < header.HashCount; i++)
				{
					file.read(&entry);

					// Prepare a cache entry
					PackageCacheObject NewObject{};
					// Set data
					NewObject.Offset = header.DataOffset + entry.Offset;
					NewObject.CompressedSize = entry.Size & 0xFFFFFFFFFFFFFF; // 0x80 in last 8 bits in some entries in new XPAKs
					NewObject.UncompressedSize = 0;
					NewObject.PackageFile = pak_path;
					// Append to database

					xpak_cache_map[pak_path].insert(std::make_pair(entry.Key, NewObject));
				}

				file.close();
			}

			void populate_xpak_cache_iterator(const std::string& path)
			{
				if (!std::filesystem::is_directory(path))
				{
					return;
				}

				for (auto const& dir_entry : std::filesystem::directory_iterator{ path })
				{
					if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".xpak")
					{
						const auto pakfile = dir_entry.path().string();

						populate_xpak_cache_internal(pakfile);
					}
				}
			}

			void try_populate_xpak_cache_for_zone(const std::string& name)
			{
				auto pak_name = name + ".xpak";
				auto pak_path = "zone/" + pak_name;

				if (!utils::io::file_exists(pak_path))
				{
					pak_path = "../" + pak_path;
					if (!utils::io::file_exists(pak_path)) return;
					//return;
				}

				populate_xpak_cache_internal(pak_path);
			}

			void populate_xpak_cache_for_loaded_zones()
			{
				int index = *g_zoneCount;
				auto* zone = &g_zones[index];
				auto zone_name = &g_zoneNames[index];

				while (index > 0)
				{
					try_populate_xpak_cache_for_zone(zone_name->name);

					index--;
					zone--;
					zone_name--;
				}
			}

			std::vector<std::uint8_t> get_data(uint64_t key, const unsigned int expected_size)
			{
				for (auto& map : xpak_cache_map)
				{
					if (map.second.contains(key))
					{
						auto obj = map.second[key];
						filesystem::file file(obj.PackageFile);
						file.open("rb", false, false);

						if (!file.get_fp())
						{
							return {};
						}

						std::string buffer;
						buffer.resize(obj.CompressedSize);
						file.seek(obj.Offset, SEEK_SET);
						file.read(buffer.data(), obj.CompressedSize);

						auto data = decompress_xpak_data(buffer, expected_size);
						if (data.size() == expected_size)
						{
							return data;
						}
					}
				}

				return {};
			}
		}

		std::vector<std::uint8_t> get_data_for_xpak_key(uint64_t key, const unsigned int expected_size)
		{
			/*auto zone_count_changed = []() -> bool
			{
				static unsigned int zone_count = 0;
				if (zone_count != *g_zoneCount)
				{
					zone_count = *g_zoneCount;
					return true;
				}
				return false;
			};

			if (zone_count_changed())
			{
				clear_cache();
			}*/

			if (xpak::xpak_cache_map.empty())
			{
				xpak::populate_xpak_cache_iterator("../zone/");
				xpak::populate_xpak_cache_iterator("zone/");

				//xpak::populate_xpak_cache_for_loaded_zones();
			}

			auto data = xpak::get_data(key, expected_size);

			return data;
		}

		void clear_cache()
		{
			xpak::xpak_cache_map.clear();
		}
	}
}