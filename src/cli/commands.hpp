#pragma once

#include "settings.hpp"

namespace zonetool::cli
{
	using command_handler = void(*)(const settings& settings, std::span<const std::string> args);

	struct command
	{
		std::string_view name;
		std::string_view usage;
		command_handler handler;
	};

	std::span<const command> commands();
	bool execute(const settings& settings, std::span<const std::string> args);

	void inspect(const settings& settings, std::span<const std::string> args);
}
