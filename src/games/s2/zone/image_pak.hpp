#pragma once

#include <xfile/xfile.hpp>

namespace zonetool::s2::image_pak
{
	constexpr std::size_t toc_stream_count = 4;

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

	// the game keeps hash[0] (looked up by XStreamFileKey::hash) and drops hash[1]
	struct toc_entry
	{
		std::uint64_t hash[2];
		toc_stream streams[toc_stream_count];
	};
#pragma pack(pop)

	static_assert(sizeof(toc_header) == 20);
	static_assert(sizeof(toc_entry) == 72);

	constexpr std::string_view toc_magic = "S2ffi100";

	std::optional<std::vector<toc_entry>> parse_toc(std::string_view data);

	std::string get_pak_name(const std::string& zone_name);

	struct image_streams
	{
		std::size_t first_key;
		std::array<std::optional<std::filesystem::path>, toc_stream_count> pixels;
	};

	// writes the zone's own image streams to <zone>.pak in folder and points their keys at it
	// with XStreamFileLocation::zone_pak_index, which the client resolves to the pak of the zone being loaded
	void write(const std::string& zone_name, const std::filesystem::path& folder, std::span<const image_streams> images,
		std::vector<XStreamFileKey>& keys);
}
