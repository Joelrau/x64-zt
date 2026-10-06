#pragma once

#include <game.hpp>

namespace zonetool::cli
{
	struct settings
	{
		const game* game{};
		std::filesystem::path game_path;

		std::filesystem::path zone_folder() const;
		std::optional<std::filesystem::path> find_zone(std::string_view name) const;
	};

	settings load_settings(std::vector<std::string>& args);
}
