#include <std_include.hpp>

#include <game.hpp>

namespace zonetool::iw5
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter);
	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type,
		const std::string& name, const converter* converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);

	const game& definition()
	{
		static constexpr std::array<std::string_view, 2> binaries{"iw5mp.exe", "iw5sp.exe"};
		static const game iw5
		{
			.name = "iw5",
			.display_name = "Call of Duty: Modern Warfare 3",
			.steam_folder = "Call of Duty Modern Warfare 3",
			.binaries = binaries,
			.format =
			{
				.container = xfile::container::iw4,
				.unsigned_magic = "IWffu100",
				.signed_magic = "IWff0100",
				.version = 1,
				.block_count = 9,
				.auth_block_size = 0x2000,
				.auth_blocks_per_group = 256,
			},
			.dump_zone = dump_zone,
			.dump_asset = dump_asset,
			.verify_zone = verify_zone,
			.list_zone = list_zone,
		};

		return iw5;
	}
}
