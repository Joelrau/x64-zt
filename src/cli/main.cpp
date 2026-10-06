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

		void print_help()
		{
			std::cout << "commands:\n";
			for (const auto& command : commands())
			{
				std::cout << std::format("  {}\n", command.usage);
			}
			std::cout << "  quit\n";
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

				try
				{
					if (!execute(settings, args))
					{
						print_help();
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
		try
		{
			std::vector<std::string> args(argv + 1, argv + argc);
			const auto settings = load_settings(args);

			if (args.empty())
			{
				run_console(settings);
				return 0;
			}

			return execute(settings, args) ? 0 : 1;
		}
		catch (const std::exception& e)
		{
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
