#include <std_include.hpp>
#include "mode.hpp"

#include <utils/string.hpp>

namespace game
{
	namespace
	{
		std::unordered_map<game_mode, std::string>& get_game_mode_map()
		{
			static std::unordered_map<game_mode, std::string> game_mode_map =
			{
				{none, "none"},
				{iw6, "iw6"},
				{s1, "s1"},
				{h1, "h1"},
				{h2, "h2"},
				{iw7, "iw7"},
				{t7, "t7"},
			};

			return game_mode_map;
		}

		std::string format_postfix(const std::string& name)
		{
			return "_" + name;
		}
	}

	game_mode mode_{none};

	std::string get_mode_as_string(game_mode mode)
	{
		const auto iter = get_game_mode_map().find(mode);
		if (iter != get_game_mode_map().end())
		{
			return iter->second;
		}

		return "none";
	}

	game_mode get_mode_from_string(const std::string& name)
	{
		const auto lower = utils::string::to_lower(name);
		for (const auto& [mode, game_name] : get_game_mode_map())
		{
			if (game_name == lower)
			{
				return mode;
			}
		}

		return none;
	}

	game_mode get_source(const std::string& string)
	{
		for (const auto& [mode, name] : get_game_mode_map())
		{
			const auto postfix = format_postfix(name);
			if (string.ends_with(postfix))
			{
				return mode;
			}
		}

		return none;
	}

	std::string add_source_postfix(const std::string& string, const game_mode source)
	{
		const auto current = get_source(string);
		if (current != none)
		{
			return string;
		}

		const auto game = get_mode_as_string(source);
		const auto postfix = format_postfix(game);
		return string + postfix;
	}

	std::string get_mode_as_string()
	{
		return get_mode_as_string(mode_);
	}

	game_mode get_mode()
	{
		return mode_;
	}

	void set_mode(game_mode mode)
	{
		mode_ = mode;
	}
}