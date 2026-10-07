#include <std_include.hpp>
#include "commands.hpp"
#include "game_registry.hpp"

#include <formats/csv_generator.hpp>
#include <game_mode.hpp>

namespace zonetool::cli
{
	namespace
	{
		std::optional<std::filesystem::path> require_zone(const settings& settings, const std::span<const std::string> args,
			const std::string_view usage)
		{
			if (args.empty())
			{
				std::cout << std::format("usage: {}\n", usage);
				return {};
			}

			auto path = game_files::find_zone(args.front());
			if (!path)
			{
				std::cout << std::format("zone \"{}\" not found in {}\n", args.front(), game_files::get().description());
			}

			return path;
		}

		void verify_all(const settings& settings)
		{
			const auto zones = game_files::list_zones();

			std::size_t failed{};
			for (const auto& zone : zones)
			{
				try
				{
					settings.game->verify_zone(settings.zone_folder(), zone);
				}
				catch (const std::exception& e)
				{
					failed++;
					std::cout << std::format("{}: {}\n", zone.string(), e.what());
				}
			}

			std::cout << std::format("{} of {} zones passed\n", zones.size() - failed, zones.size());
		}
	}

	void dump_zone(const settings& settings, const std::span<const std::string> args)
	{
		constexpr std::string_view usage = "dumpzone <zone> [--target <game>]";

		const auto has_target = args.size() == 3 && args[1] == "--target";
		if (args.size() != 1 && !has_target)
		{
			std::cout << std::format("usage: {}\n", usage);
			return;
		}

		const converter* converter{};
		if (has_target && args[2] != settings.game->name)
		{
			converter = find_converter(settings.game->name, args[2]);
			if (!converter)
			{
				std::cout << std::format("no converter from {} to {} in this build\n", settings.game->name, args[2]);
				return;
			}
		}

		if (const auto path = require_zone(settings, args, usage))
		{
			settings.game->dump_zone(settings.zone_folder(), *path, converter ? converter->dump_asset : nullptr);
		}
	}

	void verify_zone(const settings& settings, const std::span<const std::string> args)
	{
		if (!args.empty() && args.front() == "--all")
		{
			verify_all(settings);
			return;
		}

		if (const auto path = require_zone(settings, args, "verifyzone <zone> | verifyzone --all"))
		{
			settings.game->verify_zone(settings.zone_folder(), *path);
			std::cout << std::format("zone \"{}\" passed\n", args.front());
		}
	}

	void build_zone(const settings& settings, const std::span<const std::string> args)
	{
		if (args.empty())
		{
			std::cout << "usage: buildzone <zone>\n";
			return;
		}

		settings.game->build_zone(settings.zone_folder(), args.front());
	}

	void generate_csv(const settings& settings, const std::span<const std::string> args)
	{
		if (args.empty())
		{
			std::cout << "usage: generatecsv <map> [sp]\n";
			return;
		}

		const auto token_name = settings.game->token_name;
		const auto get_token_name = [token_name](const std::uint32_t id)
		{
			return token_name ? token_name(id) : std::to_string(id);
		};

		auto is_sp = false;
		std::vector<std::string> extra_paths;
		for (auto i = 1u; i < args.size(); ++i)
		{
			if (args[i] == "sp")
			{
				is_sp = true;
			}
			else if (args[i] == "--path" && i + 1 < args.size())
			{
				extra_paths.emplace_back(args[++i]);
			}
		}

		csv_generator::generate_map_csv(args.front(), get_token_name, is_sp,
			::game::get_mode_from_string(std::string(settings.game->name)), extra_paths);
	}
}
