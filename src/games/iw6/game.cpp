#include <std_include.hpp>

#include <game.hpp>

#include <xsk/gsc/engine/iw6_pc.hpp>

namespace zonetool::iw6
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter);
	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type,
		const std::string& name, const converter* converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	void build_zone(const std::filesystem::path& zone_folder, const std::string& name);

	std::string token_name(const std::uint32_t id)
	{
		static const xsk::gsc::iw6_pc::context context{xsk::gsc::instance::server};
		return context.token_name(id);
	}

	const game& definition()
	{
		static const game iw6
		{
			.name = "iw6",
			.display_name = "Call of Duty: Ghosts",
			.steam_folder = "Call of Duty Ghosts",
			.format =
			{
				.container = xfile::container::s1,
				.unsigned_magic = "IWffu100",
				.signed_magic = "IWff0100",
				.version = 565,
				.block_count = 7,
				.auth_block_size = 0x2000,
				.auth_blocks_per_group = 256,
			},
			.dump_zone = dump_zone,
			.dump_asset = dump_asset,
			.verify_zone = verify_zone,
			.build_zone = build_zone,
			.token_name = token_name,
		};

		return iw6;
	}
}
