#pragma once

#include "xfile/xfile.hpp"

namespace zonetool
{
	struct game
	{
		std::string_view name;
		std::string_view display_name;
		std::string_view steam_folder;
		xfile::format format;
		std::size_t block_count;
	};
}
