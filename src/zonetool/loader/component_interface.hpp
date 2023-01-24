#pragma once

#include "game/mode.hpp"

class component_interface
{
public:
	virtual ~component_interface()
	{
	}

	virtual void post_start()
	{
	}

	virtual void post_load()
	{
	}

	virtual void pre_destroy()
	{
	}

	virtual void post_unpack()
	{
	}

	virtual void* load_import([[maybe_unused]] const std::string& library, [[maybe_unused]] const std::string& function)
	{
		return nullptr;
	}

	virtual bool is_supported()
	{
		return true;
	}

	game::game_mode target_mode{game::game_mode::none};
	std::string component_name;
};
