#pragma once

namespace game
{
	enum game_mode : std::int32_t
	{
		none = -1,
		h1 = 0,
		h2 = 1,
		s1 = 2,
		iw6 = 3,
	};

	std::string get_mode_as_string();
	std::string get_mode_as_string(game_mode mode);

	game_mode get_mode_from_string(const std::string& name);
	game_mode get_mode();

	void set_mode(game_mode mode);

	game_mode get_source(const std::string& string);
	std::string add_source_postfix(const std::string& string, const game_mode source);
}
