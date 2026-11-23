#include <std_include.hpp>
#include "commands.hpp"
#include "game_registry.hpp"
#include "crash.hpp"

#include <io/filesystem.hpp>

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

		void print_options(const std::string_view title, const std::span<const option> options)
		{
			std::cout << std::format("\n{}:\n", title);
			for (const auto& option : options)
			{
				std::cout << std::format("  {:<26}{}\n", option.name, option.description);
			}
		}

		void print_overview()
		{
			std::cout << "commands:\n";
			for (const auto& command : commands())
			{
				std::cout << std::format("  {:<13}{}\n", command.name, command.description);
			}

			std::cout << "\nhelp <command>   usage and options of one command\n"
				"help options     options for every command\n"
				"help start       quick start\n"
				"quit             closes the console\n";
		}

		void print_command(const command& command)
		{
			std::cout << std::format("usage: {}\n  {}\n", command.usage, command.description);
			if (!command.options.empty())
			{
				print_options("options", command.options);
			}
		}

		void print_global_options()
		{
			std::cout << "usage: zonetool.exe [options] <command> [arguments]\n"
				"       zonetool.exe [options]    opens the console\n";
			print_options("options (command line only, not in the console)", global_options());
			std::cout << std::format("\ngames: {}\n", game_names());
		}

		void print_quick_start()
		{
			std::cout << "1. Put zonetool.exe in the game folder, next to the game exe.\n"
				"   From another folder, add -game <name> -path \"<game folder>\".\n"
				"2. Dump a zone: dumpzone mp_shipment\n"
				"   The assets go to dump/mp_shipment/.\n"
				"3. Build a zone: put the assets in zonetool/<zone>/ and the asset list in zone_source/<zone>.csv.\n"
				"   Then run buildzone <zone>. The zone goes to zone/<zone>.ff.\n"
				"\nMore help: https://docs.auroramod.dev/zonetool-basics/\n";
		}

		void print_help(const std::span<const std::string> topic)
		{
			if (topic.empty())
			{
				print_overview();
				return;
			}

			const auto& name = topic.front();
			if (name == "options")
			{
				print_global_options();
				return;
			}

			if (name == "start")
			{
				print_quick_start();
				return;
			}

			const auto list = commands();
			const auto command = std::ranges::find(list, name, &command::name);
			if (command == list.end())
			{
				std::cout << std::format("no help for \"{}\"\n", name);
				print_overview();
				return;
			}

			print_command(*command);
		}

		void run_console(const settings& settings)
		{
			std::cout << std::format("zonetool [{}] {}\n", settings.game->name, settings.game_path.string());
			std::cout << "Type \"help\" for the commands.\n\n";

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
						print_help(std::span(args).subspan(1));
					}
					else if (!execute(settings, args))
					{
						std::cout << "Type \"help\" for the commands.\n";
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
		if (!args.empty() && is_help(args.front()))
		{
			print_help(std::span(args).subspan(1));
			return 0;
		}

		if (std::ranges::any_of(args, is_help))
		{
			print_overview();
			return 0;
		}

		try
		{
			const auto settings = load_settings(args);
			filesystem::set_cache_game(std::string(settings.game->name));

			if (args.empty())
			{
				run_console(settings);
				return 0;
			}

			if (!execute(settings, args))
			{
				print_overview();
				return 1;
			}

			return 0;
		}
		catch (const std::exception& e)
		{
			if (args.empty())
			{
				print_quick_start();
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
