#include <std_include.hpp>

#include <game.hpp>

namespace zonetool::iw3
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter);
	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type,
		const std::string& name, const converter* converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);

	const game& definition()
	{
		static constexpr std::array<std::string_view, 2> binaries{"iw3mp.exe", "iw3sp.exe"};
		static const game iw3
		{
			.name = "iw3",
			.display_name = "Call of Duty 4: Modern Warfare",
			.steam_folder = "Call of Duty 4",
			.binaries = binaries,
			.format =
			{
				.container = xfile::container::iw3,
				.unsigned_magic = "IWffu100",
				.signed_magic = "IWffu100",
				.version = 5,
				.block_count = 9,
			},
			.dump_zone = dump_zone,
			.dump_asset = dump_asset,
			.verify_zone = verify_zone,
			.list_zone = list_zone,
		};

		return iw3;
	}
}
