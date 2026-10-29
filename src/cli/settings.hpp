#pragma once

#include <game.hpp>

namespace zonetool::cli
{
	struct settings
	{
		const game* game{};
		std::filesystem::path game_path;
		std::optional<std::string> target_path;
		json config;

		std::filesystem::path zone_folder() const;
		std::filesystem::path game_folder(const zonetool::game& target_game) const;
	};

	settings load_settings(std::vector<std::string>& args);
}
