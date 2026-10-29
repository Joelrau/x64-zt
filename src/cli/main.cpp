#include <std_include.hpp>
#include "commands.hpp"
#include "game_registry.hpp"
#include "crash.hpp"

namespace zonetool::cli
{
	namespace
	{
		std::vector<std::string> split_line(const std::string& line)
		{
			std::vector<std::string> args;
			std::istringstream stream(line);

			std::string arg;
			while (stream >> std::quoted(arg))
			{
				args.emplace_back(arg);
			}

			return args;
		}

		bool is_help(const std::string& arg)
		{
			return arg == "-h" || arg == "--help" || arg == "-?" || arg == "help";
		}

		std::string game_names()
		{
			std::string names;
			for (const auto* game : enabled_games())
			{
				names += std::format("{}{}", names.empty() ? "" : ", ", game->name);
			}
			return names;
		}

		void print_commands()
		{
			std::cout << "commands:\n";
			for (const auto& command : commands())
			{
				std::cout << std::format("  {}\n      {}\n", command.usage, command.description);
			}
		}

		void print_help()
		{
			std::cout << "usage: zonetool.exe [options] <command> [arguments]\n"
				"       zonetool.exe [options]    opens the interactive console\n\n";

			print_commands();

			std::cout << std::format(
				"\noptions (put before \"zonetool.exe\" command line, not in the console):\n"
				"  -game <name>              engine code like iw6, s1, s2, h1, etc. default will auto select if in game folder\n"
				"  -path <folder>            game folder\n"
				"  -target_path <folder>     game folder of the --target game for conversion \n"
				"  -no_cache                 ignores the build cache and writes every file again\n"
				"\ndump options:\n"
				"  -dds                      writes images as DDS too\n"
				"  -dump_streamed_image      reads imagefile*.pak and writes streamed images\n"
				"  -castdump                 writes a .cast file next to each xmodel and xanim\n"
				"\nbuild options:\n"
				"  -more_memory              doubles the zone memory limit\n"
				"  -keep_mp_traverse_nodes   keeps invalid mp traverse nodes in aipaths (h2)\n"
				"\nquick start:\n"
				"  1. zonetool.exe goes in the game folder next to the game exe and fastfiles\n"
				"     (externally, use `-game <s1/h1/s2/iw6>` and `-path \"C:/path/to/game/folder\"`)\n"
				"  2. to dump, you can try `zonetool.exe dumpzone mp_shipment -game h1`\n"
				"     output goes to `dump/` folder containing mp_shipment/\n"
				"  3. to build, put assets in `zonetool/<mod>/` inside game folder (create if doesn't exist)\n"
				"     your mod's asset list csv goes in `zone_source/<name>.csv`\n"
				"     the built zone will be in game folder as `<name>.ff` OR inside `zone/<name>.ff`\n"
				"\n[!] check https://docs.auroramod.dev/zonetool-basics/ to get additional help\n\n",
				game_names(), game_names());
		}

		void run_console(const settings& settings)
		{
			std::cout << std::format("zonetool [{}] {}\n", settings.game->name, settings.game_path.string());
			print_help();

			std::string line;
			while (std::cout << "> " && std::getline(std::cin, line))
			{
				const auto args = split_line(line);
				if (!args.empty() && args.front() == "quit")
				{
					break;
				}

				if (!args.empty() && (args.front() == "clear" || args.front() == "cls"))
				{
					std::system("cls");
					continue;
				}

				try
				{
					if (!args.empty() && is_help(args.front()))
					{
						print_help();
					}
					else if (!execute(settings, args))
					{
						print_commands();
					}
				}
				catch (const std::exception& e)
				{
					std::cout << std::format("error: {}\n", e.what());
				}
			}
		}
	}

	int main(const int argc, char** argv)
	{
		std::vector<std::string> args(argv + 1, argv + argc);
		if (std::ranges::any_of(args, is_help))
		{
			print_help();
			return 0;
		}

		try
		{
			const auto settings = load_settings(args);

			if (args.empty())
			{
				run_console(settings);
				return 0;
			}

			if (!execute(settings, args))
			{
				print_commands();
				return 1;
			}

			return 0;
		}
		catch (const std::exception& e)
		{
			if (args.empty())
			{
				print_help();
			}

			std::cout << std::format("error: {}\n", e.what());
			return 1;
		}
	}
}

int main(const int argc, char** argv)
{
	zonetool::cli::install_crash_handler();
	return zonetool::cli::main(argc, argv);
}
