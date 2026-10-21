#include <std_include.hpp>

#include <game.hpp>

#include <xsk/gsc/engine/s2.hpp>

namespace zonetool::s2
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter);
	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type,
		const std::string& name, const converter* converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	void build_zone(const std::filesystem::path& zone_folder, const std::string& name);

	namespace
	{
		std::string load_zone_key()
		{
			const auto resource = FindResource(nullptr, TEXT("S2_ZONE_KEY"), RT_RCDATA); // cli/resource.rc
			if (!resource)
			{
				return {};
			}

			const auto handle = LoadResource(nullptr, resource);
			if (!handle)
			{
				return {};
			}

			return std::string(static_cast<const char*>(LockResource(handle)), SizeofResource(nullptr, resource));
		}
	}

	std::string zone_key()
	{
		static const auto key = load_zone_key();
		return key;
	}

	std::string token_name(const std::uint32_t id)
	{
		static const xsk::gsc::s2::context context{xsk::gsc::instance::server};
		return context.token_name(id);
	}

	const game& definition()
	{
		static const game s2
		{
			.name = "s2",
			.display_name = "Call of Duty: WWII",
			.steam_folder = "Call of Duty WWII",
			.format =
			{
				.container = xfile::container::s2,
				.unsigned_magic = "S1ffu100",
				.signed_magic = "S1ff0100",
				.version = 159,
				.block_count = 7,
				.zone_key = zone_key,
			},
			.dump_zone = dump_zone,
			.dump_asset = dump_asset,
			.verify_zone = verify_zone,
			.list_zone = list_zone,
			.build_zone = build_zone,
			.token_name = token_name,
		};

		return s2;
	}
}
