#pragma once

namespace game
{
	enum game_mode : std::int32_t
	{
		none = -1,
		iw6,
		s1,
		h1,
		h2,
		iw7,
		count,
	};

	std::string get_mode_as_string();
	std::string get_mode_as_string(game_mode mode);

	game_mode get_mode_from_string(const std::string& name);
	game_mode get_mode();

	void set_mode(game_mode mode);

	game_mode get_source(const std::string& string);
	std::string add_source_postfix(const std::string& string, const game_mode source);
}
