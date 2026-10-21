#include <std_include.hpp>

#include <game.hpp>

#include <xsk/gsc/engine/s1_pc.hpp>

namespace zonetool::s1
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter);
	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type,
		const std::string& name, const converter* converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	void build_zone(const std::filesystem::path& zone_folder, const std::string& name);

	std::string token_name(const std::uint32_t id)
	{
		static const xsk::gsc::s1_pc::context context{xsk::gsc::instance::server};
		return context.token_name(id);
	}

	const game& definition()
	{
		static const game s1
		{
			.name = "s1",
			.display_name = "Call of Duty: Advanced Warfare",
			.steam_folder = "Call of Duty Advanced Warfare",
			.format =
			{
				.container = xfile::container::s1,
				.unsigned_magic = "S1ffu100",
				.signed_magic = "S1ff0100",
				.version = 1838,
				.block_count = 7,
			},
			.dump_zone = dump_zone,
			.dump_asset = dump_asset,
			.verify_zone = verify_zone,
			.build_zone = build_zone,
			.token_name = token_name,
		};

		return s1;
	}
}
