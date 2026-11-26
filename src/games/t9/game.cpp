#include <std_include.hpp>

#include <game.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::t9
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter);
	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type,
		const std::string& name, const converter* converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	std::string inspect_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	void build_zone(const std::filesystem::path& zone_folder, const std::string& name);

	namespace
	{
		std::unique_ptr<game_files::source> open_files(const std::filesystem::path& game_path)
		{
			xfile::oodle::load(game_path);
			return game_files::open_folder(game_path / "zone");
		}
	}

	const game& definition()
	{
		static constexpr std::array<std::string_view, 1> binaries{"BlackOpsColdWar.exe"};
		static const game t9
		{
			.name = "t9",
			.display_name = "Call of Duty: Black Ops Cold War",
			.binaries = binaries,
			.open_files = open_files,
			.format =
			{
				.container = xfile::container::t9,
				.unsigned_magic = "TAFF",
				.version = 0x64,
				.block_count = 13,
			},
			.dump_zone = dump_zone,
			.dump_asset = dump_asset,
			.verify_zone = verify_zone,
			.list_zone = list_zone,
			.build_zone = build_zone,
			.inspect_zone = inspect_zone,
		};

		return t9;
	}
}
