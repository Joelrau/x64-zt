#pragma once

#include "structs.hpp"

namespace zonetool::xfile
{
	enum class container
	{
		s1,
		iw7,
	};

	struct format
	{
		container container;
		std::string_view unsigned_magic;
		std::string_view signed_magic;
		std::uint32_t version;
		std::size_t block_count;
	};

	struct fastfile
	{
		std::string magic;
		std::uint32_t version;
		bool is_signed;
		std::vector<std::uint64_t> block_sizes;
		std::vector<XStreamFile> stream_files;
		std::vector<XStreamFile> shared_stream_files;
		std::string payload;
	};

	fastfile read(const std::filesystem::path& path, const format& format);
}
