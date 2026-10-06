#include <std_include.hpp>
#include "game_registry.hpp"

#include <enabled_games.hpp>

#define DECLARE_GAME(name) namespace zonetool::name { const game& definition(); }
ZONETOOL_GAMES(DECLARE_GAME)
#undef DECLARE_GAME

namespace zonetool::cli
{
	std::span<const game* const> enabled_games()
	{
#define GAME_ENTRY(name) &name::definition(),
		static const std::array games{ZONETOOL_GAMES(GAME_ENTRY)};
#undef GAME_ENTRY
		return games;
	}

	const game* find_game(const std::string_view name)
	{
		for (const auto* game : enabled_games())
		{
			if (game->name == name)
			{
				return game;
			}
		}

		return nullptr;
	}
}
