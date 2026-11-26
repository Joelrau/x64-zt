#include <std_include.hpp>

#include <game.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::s4
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter);
	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type,
		const std::string& name, const converter* converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	void build_zone(const std::filesystem::path& zone_folder, const std::string& name);

	namespace
	{
		std::unique_ptr<game_files::source> open_files(const std::filesystem::path& game_path)
		{
			xfile::oodle::load(game_path);
			if (!std::filesystem::is_directory(game_path / "Data") && std::filesystem::is_directory(game_path / "zone"))
			{
				return game_files::open_folder(game_path / "zone");
			}

			return game_files::open_casc(game_path);
		}
	}

	const game& definition()
	{
		static constexpr std::array<std::string_view, 1> binaries{"Vanguard.exe"};
		static const game s4
		{
			.name = "s4",
			.display_name = "Call of Duty: Vanguard",
			.binaries = binaries,
			.open_files = open_files,
			.format =
			{
				.container = xfile::container::s4,
				.unsigned_magic = "IWffc100",
				.signed_magic = "IWffa100",
				.version = 11,
				.block_count = 10,
			},
			.dump_zone = dump_zone,
			.dump_asset = dump_asset,
			.verify_zone = verify_zone,
			.list_zone = list_zone,
			.build_zone = build_zone,
		};

		return s4;
	}
}
