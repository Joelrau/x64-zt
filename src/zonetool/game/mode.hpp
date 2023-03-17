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
	game_mode get_mode();
	void set_mode(game_mode mode);
}
