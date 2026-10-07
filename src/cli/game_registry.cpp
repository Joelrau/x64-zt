#include <std_include.hpp>
#include "game_registry.hpp"

#include <enabled_games.hpp>

#define DECLARE_GAME(name) namespace zonetool::name { const game& definition(); }
ZONETOOL_GAMES(DECLARE_GAME)
#undef DECLARE_GAME

#define DECLARE_CONVERTER(source, target) namespace zonetool::convert::source##_##target { const converter& definition(); }
ZONETOOL_CONVERTERS(DECLARE_CONVERTER)
#undef DECLARE_CONVERTER

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

	const converter* find_converter(const std::string_view source, const std::string_view target)
	{
#define CONVERTER_ENTRY(source, target) &convert::source##_##target::definition(),
		static const std::vector<const converter*> converters{ZONETOOL_CONVERTERS(CONVERTER_ENTRY)};
#undef CONVERTER_ENTRY

		for (const auto* converter : converters)
		{
			if (converter->source == source && converter->target == target)
			{
				return converter;
			}
		}

		return nullptr;
	}
}
