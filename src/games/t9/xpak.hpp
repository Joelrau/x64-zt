#pragma once

namespace zonetool::t9::xpak
{
	struct entry_location
	{
		std::uint64_t key;
		std::uint64_t offset;
		std::uint32_t file;
		std::uint32_t size;
		bool compressed;
	};

	struct pak_index
	{
		std::vector<std::string> files;
		std::vector<entry_location> entries;
	};

	std::optional<std::string> read(std::uint64_t key, std::size_t size);}
