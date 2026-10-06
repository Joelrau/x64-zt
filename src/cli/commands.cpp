#include <std_include.hpp>
#include "commands.hpp"

namespace zonetool::cli
{
	std::span<const command> commands()
	{
		static const std::array list
		{
			command{"inspect", "inspect <zone> | inspect --all", inspect},
		};

		return list;
	}

	bool execute(const settings& settings, const std::span<const std::string> args)
	{
		if (args.empty())
		{
			return true;
		}

		for (const auto& command : commands())
		{
			if (command.name == args.front())
			{
				command.handler(settings, args.subspan(1));
				return true;
			}
		}

		std::cout << std::format("unknown command \"{}\"\n", args.front());
		return false;
	}
}
