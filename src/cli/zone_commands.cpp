#include <std_include.hpp>
#include "commands.hpp"
#include "game_registry.hpp"

#include <formats/csv_generator.hpp>
#include <game_mode.hpp>
#include <utils/io.hpp>

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

		std::optional<std::string_view> target_argument(const std::span<const std::string> args, const std::size_t positional_count)
		{
			if (args.size() == positional_count)
			{
				return std::string_view{};
			}

			if (args.size() == positional_count + 2 && args[positional_count] == "--target")
			{
				return args[positional_count + 1];
			}

			return {};
		}

		std::optional<const converter*> select_converter(const settings& settings, const std::string_view target_name)
		{
			if (target_name.empty() || target_name == settings.game->name)
			{
				return nullptr;
			}

			const auto* converter = find_converter(settings.game->name, target_name);
			if (!converter)
			{
				std::cout << std::format("no converter from {} to {} in this build\n", settings.game->name, target_name);
				return {};
			}

			if (converter->setup)
			{
				const auto* target = find_game(converter->target);
				if (!target)
				{
					std::cout << std::format("game {} is not in this build\n", converter->target);
					return {};
				}

				converter->setup(settings.game_folder(*target));
			}

			return converter;
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

		std::filesystem::path listing_path(const settings& settings, const std::filesystem::path& zone_path)
		{
			auto relative = zone_path.lexically_relative(game_files::get().path("").parent_path());
			if (relative.empty() || *relative.begin() == "..")
			{
				relative = zone_path.filename();
			}

			return std::filesystem::path("asset_listing") / settings.game->name / relative.replace_extension(".zone");
		}

		void write_listing(const settings& settings, const std::filesystem::path& zone_path)
		{
			const auto path = listing_path(settings, zone_path);
			if (!utils::io::write_file(path.string(), settings.game->list_zone(settings.zone_folder(), zone_path)))
			{
				throw std::runtime_error(std::format("failed to write {}", path.string()));
			}

			std::cout << std::format("{}\n", path.string());
		}

		void list_all(const settings& settings)
		{
			const auto zones = game_files::list_zones();

			std::size_t failed{};
			for (const auto& zone : zones)
			{
				try
				{
					write_listing(settings, zone);
				}
				catch (const std::exception& e)
				{
					failed++;
					std::cout << std::format("{}: {}\n", zone.string(), e.what());
				}
			}

			std::cout << std::format("{} of {} zones listed\n", zones.size() - failed, zones.size());
		}
	}

	void dump_zone(const settings& settings, const std::span<const std::string> args)
	{
		constexpr std::string_view usage = "dumpzone <zone> [--target <game>]";

		const auto target = target_argument(args, 1);
		if (!target)
		{
			std::cout << std::format("usage: {}\n", usage);
			return;
		}

		const auto path = require_zone(settings, args, usage);
		if (!path)
		{
			return;
		}

		const auto converter = select_converter(settings, *target);
		if (!converter)
		{
			return;
		}

		settings.game->dump_zone(settings.zone_folder(), *path, *converter);
	}

	void dump_asset(const settings& settings, const std::span<const std::string> args)
	{
		constexpr std::string_view usage = "dumpasset <type> <name> <zone> [--target <game>]";

		const auto target = target_argument(args, 3);
		if (!target)
		{
			std::cout << std::format("usage: {}\n", usage);
			return;
		}

		const auto path = require_zone(settings, args.subspan(2), usage);
		if (!path)
		{
			return;
		}

		const auto converter = select_converter(settings, *target);
		if (!converter)
		{
			return;
		}

		settings.game->dump_asset(settings.zone_folder(), *path, args[0], args[1], *converter);
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

	void list_zone(const settings& settings, const std::span<const std::string> args)
	{
		if (!settings.game->list_zone)
		{
			std::cout << std::format("listzone is not supported for {}\n", settings.game->name);
			return;
		}

		if (!args.empty() && args.front() == "--all")
		{
			list_all(settings);
			return;
		}

		if (const auto path = require_zone(settings, args, "listzone <zone> | listzone --all"))
		{
			write_listing(settings, *path);
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
