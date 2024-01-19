#pragma once

#include "game/mode.hpp"

enum component_priority : std::int32_t
{
	min = 0,
};

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

	virtual component_priority priority()
	{
		return component_priority::min;
	}

	game::game_mode target_mode{game::game_mode::none};
	std::string component_name;
};
