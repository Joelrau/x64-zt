#include <std_include.hpp>
#include "commands.hpp"

namespace zonetool::cli
{
	namespace
	{
		bool is_flag(const std::string& arg)
		{
			return arg.size() > 1 && arg[0] == '-' && arg[1] != '-';
		}

		std::vector<std::string> without_flags(const std::span<const std::string> args)
		{
			std::vector<std::string> result;
			std::ranges::copy_if(args, std::back_inserter(result), [](const std::string& arg)
			{
				return !is_flag(arg);
			});
			return result;
		}
	}

	std::span<const command> commands()
	{
		static const std::array list
		{
			command{"inspect", "inspect <zone> | inspect --all", inspect},
			command{"dumpzone", "dumpzone <zone> [--target <game>]", dump_zone},
			command{"verifyzone", "verifyzone <zone> | verifyzone --all", verify_zone},
			command{"buildzone", "buildzone <zone>", build_zone},
			command{"generatecsv", "generatecsv <map> [sp] [--path <dir>]...", generate_csv},
		};

		return list;
	}

	bool execute(const settings& settings, const std::span<const std::string> all_args)
	{
		const auto args = without_flags(all_args);
		if (args.empty())
		{
			return true;
		}

		for (const auto& command : commands())
		{
			if (command.name == args.front())
			{
				command.handler(settings, std::span(args).subspan(1));
				return true;
			}
		}

		std::cout << std::format("unknown command \"{}\"\n", args.front());
		return false;
	}
}
