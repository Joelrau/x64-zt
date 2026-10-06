#pragma once

#include "structs.hpp"

namespace zonetool::xfile
{
	enum class compression : std::uint8_t
	{
		zlib = 1,
		none = 3,
		block_container = 4,
	};

	struct format
	{
		std::string_view unsigned_magic;
		std::string_view signed_magic;
		std::uint32_t version;
	};

	struct fastfile
	{
		XFileHeader header;
		std::vector<XStreamFile> stream_files;
		bool is_signed;
		std::string payload;
	};

	fastfile read(const std::filesystem::path& path, const format& format);
}
