#pragma once

#include <game.hpp>

namespace zonetool::cli
{
	std::span<const game* const> enabled_games();
	const game* find_game(std::string_view name);
	const converter* find_converter(std::string_view source, std::string_view target);
}
