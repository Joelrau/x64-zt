#include <std_include.hpp>

#include <game.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::iw8
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
			return game_files::open_casc(game_path);
		}
	}

	const game& definition()
	{
		static constexpr std::array<std::string_view, 1> binaries{"game_dx12_ship_replay.exe"};
		static const game iw8
		{
			.name = "iw8",
			.display_name = "Call of Duty: Modern Warfare (2019)",
			.binaries = binaries,
			.open_files = open_files,
			.format =
			{
				.container = xfile::container::iw8,
				.unsigned_magic = "IWffc100",
				.signed_magic = "IWffa100",
				.version = 11,
				.block_count = 11,
			},
			.dump_zone = dump_zone,
			.dump_asset = dump_asset,
			.verify_zone = verify_zone,
			.list_zone = list_zone,
			.build_zone = build_zone,
		};

		return iw8;
	}
}
