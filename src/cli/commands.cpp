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
			command{"inspect", "inspect <zone> | inspect --all", "Shows the fastfile header and container data", inspect},
			command{"dumpzone", "dumpzone <zone> [--target <game>]", "Writes the assets of a zone to dump/<zone>/", dump_zone},
			command{"dumpmap", "dumpmap <map> [--target <game>] [--skip-common]", "Writes the map zones to dump/<map>/ (h1, h2, s1)",
				dump_map},
			command{"dumpasset", "dumpasset <type> <name|*> <zone> [--target <game>]", "Writes one asset (* = all of the type) to dump/assets/",
				dump_asset},
			command{"verifyzone", "verifyzone <zone> | verifyzone --all", "Reads every asset and checks the zone", verify_zone},
			command{"listzone", "listzone <zone> | listzone --all", "Writes the asset list to asset_listing/<game>/", list_zone},
			command{"buildzone", "buildzone <zone> [-use_usermaps]", "Builds zone/<zone>.ff from zone_source/<zone>.csv and zonetool/<zone>/",
				build_zone},
			command{"generatecsv", "generatecsv <map> [sp] [--path <dir>]...", "Writes zone_source/<map>.csv from zonetool/<map>/",
				generate_csv},
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
