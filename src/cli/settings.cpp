#include <std_include.hpp>
#include "settings.hpp"
#include "game_registry.hpp"

#include <utils/io.hpp>

namespace zonetool::cli
{
	namespace
	{
		constexpr auto config_file = "zonetool.json";
		constexpr auto steam_common = R"(C:\Program Files (x86)\Steam\steamapps\common)";

		std::optional<std::string> take_option(std::vector<std::string>& args, const std::string_view option)
		{
			const auto it = std::ranges::find(args, option);
			if (it == args.end())
			{
				return {};
			}

			if (std::next(it) == args.end())
			{
				throw std::runtime_error(std::format("{} needs a value", option));
			}

			auto value = *std::next(it);
			args.erase(it, std::next(it, 2));
			return value;
		}

		std::filesystem::path executable_folder()
		{
			std::string path(MAX_PATH, '\0');
			path.resize(GetModuleFileNameA(nullptr, path.data(), static_cast<DWORD>(path.size())));
			return std::filesystem::path(path).parent_path();
		}

		json read_config()
		{
			const auto path = executable_folder() / config_file;
			if (!utils::io::file_exists(path.string()))
			{
				return json::object();
			}

			return json::parse(utils::io::read_file(path.string()));
		}

		const game* select_game(const std::optional<std::string>& requested)
		{
			if (!requested)
			{
				return enabled_games().front();
			}

			const auto* game = find_game(*requested);
			if (!game)
			{
				throw std::runtime_error(std::format("game \"{}\" is not in this build", *requested));
			}

			return game;
		}

		std::filesystem::path select_game_path(const game& game, const std::optional<std::string>& requested)
		{
			if (requested)
			{
				return *requested;
			}

			if (std::filesystem::is_directory(executable_folder() / "zone"))
			{
				return executable_folder();
			}

			if (game.steam_folder.empty())
			{
				throw std::runtime_error(std::format("set the {} game folder with -path or zonetool.json", game.name));
			}

			return std::filesystem::path(steam_common) / game.steam_folder;
		}

		std::optional<std::string> config_game(const json& config)
		{
			if (!config.contains("game"))
			{
				return {};
			}

			return config["game"].get<std::string>();
		}

		std::optional<std::string> config_game_path(const json& config, const game& game)
		{
			if (!config.contains("paths") || !config["paths"].contains(game.name))
			{
				return {};
			}

			return config["paths"][game.name].get<std::string>();
		}
	}

	std::filesystem::path settings::zone_folder() const
	{
		return this->game_path / "zone";
	}

	settings load_settings(std::vector<std::string>& args)
	{
		const auto config = read_config();

		auto requested_game = take_option(args, "-game");
		if (!requested_game)
		{
			requested_game = config_game(config);
		}

		settings result{};
		result.game = select_game(requested_game);

		auto requested_path = take_option(args, "-path");
		if (!requested_path)
		{
			requested_path = config_game_path(config, *result.game);
		}

		result.game_path = select_game_path(*result.game, requested_path).make_preferred();

		game_files::set(result.game->open_files
			? result.game->open_files(result.game_path)
			: game_files::open_folder(result.zone_folder()));

		return result;
	}
}
