#pragma once

#include <game.hpp>

namespace zonetool::cli
{
	struct settings
	{
		const game* game{};
		std::filesystem::path game_path;

		std::filesystem::path zone_folder() const;
	};

	settings load_settings(std::vector<std::string>& args);
}
