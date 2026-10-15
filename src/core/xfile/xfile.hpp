#pragma once

#include "structs.hpp"

namespace zonetool::xfile
{
	enum class container
	{
		s1,
		iw7,
		s2,
	};

	struct format
	{
		container container;
		std::string_view unsigned_magic;
		std::string_view signed_magic;
		std::uint32_t version;
		std::size_t block_count;
		std::string (*zone_key)();
	};

	struct fastfile
	{
		std::string magic;
		std::uint32_t version;
		bool is_signed;
		std::vector<std::uint64_t> block_sizes;
		std::vector<XStreamFile> stream_files;
		std::vector<XStreamFile> shared_stream_files;
		std::vector<XStreamFileKey> stream_keys;
		std::vector<XStreamFileKey> shared_stream_keys;
		std::string payload;
	};

	fastfile read(const std::filesystem::path& path, const format& format);

	std::string write_s2(const format& format, const fastfile& file, std::string_view payload);

	std::string decompress_s2_stream(std::string_view data);
}
