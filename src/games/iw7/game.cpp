#include <std_include.hpp>

#include <game.hpp>

namespace zonetool::iw7
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const asset_dumper converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	void build_zone(const std::filesystem::path& zone_folder, const std::string& name);

	const game& definition()
	{
		static const game iw7
		{
			.name = "iw7",
			.display_name = "Call of Duty: Infinite Warfare",
			.steam_folder = "Call of Duty - Infinite Warfare",
			.format =
			{
				.container = xfile::container::iw7,
				.unsigned_magic = "IWffu100",
				.signed_magic = "IWff0100",
				.version = 1619,
				.block_count = 10,
			},
			.dump_zone = dump_zone,
			.verify_zone = verify_zone,
			.build_zone = build_zone,
		};

		return iw7;
	}
}
