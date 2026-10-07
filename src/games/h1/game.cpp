#include <std_include.hpp>

#include <game.hpp>

#include <xsk/gsc/engine/h1.hpp>

namespace zonetool::h1
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const asset_dumper converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	void build_zone(const std::filesystem::path& zone_folder, const std::string& name);

	std::string token_name(const std::uint32_t id)
	{
		static const xsk::gsc::h1::context context{xsk::gsc::instance::server};
		return context.token_name(id);
	}

	const game& definition()
	{
		static const game h1
		{
			.name = "h1",
			.display_name = "Call of Duty: Modern Warfare Remastered",
			.steam_folder = "Call of Duty Modern Warfare Remastered",
			.format =
			{
				.container = xfile::container::s1,
				.unsigned_magic = "S1ffu100",
				.signed_magic = "S1ff0100",
				.version = 66,
				.block_count = 7,
			},
			.dump_zone = dump_zone,
			.verify_zone = verify_zone,
			.build_zone = build_zone,
			.token_name = token_name,
		};

		return h1;
	}
}
