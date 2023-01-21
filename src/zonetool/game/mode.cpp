#include <std_include.hpp>
#include "mode.hpp"

namespace game
{
	game_mode mode_{none};

	std::string get_mode_as_string(game_mode mode)
	{
		switch (mode)
		{
		case none:
			return "none";
		case h1:
			return "h1";
		case h2:
			return "h2";
		case s1:
			return "s1";
		}
		return {};
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