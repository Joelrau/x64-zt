#include <std_include.hpp>

#include <game.hpp>

#include <log.hpp>
#include <utils/io.hpp>

#include <xsk/gsc/engine/s2.hpp>

namespace zonetool::s2
{
	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter);
	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type,
		const std::string& name, const converter* converter);
	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	void build_zone(const std::filesystem::path& zone_folder, const std::string& name);

	namespace
	{
		constexpr std::string_view zone_key_file = "s2_zone_key.der";
		constexpr std::size_t zone_key_size = 0x4A7;

		std::filesystem::path zone_key_folder;
		constexpr std::string_view zone_key_prefix = "\x30\x82\x04\xA3\x02\x01\x00\x02\x82\x01\x01\x00"sv;

		std::string read_key_file(const std::filesystem::path& path)
		{
			std::string data;
			if (!std::filesystem::is_regular_file(path) || !utils::io::read_file(path.string(), &data))
			{
				return {};
			}

			return data;
		}

		std::string find_key_in_executable(const std::filesystem::path& path)
		{
			const auto data = read_key_file(path);
			const auto offset = data.find(zone_key_prefix);
			if (offset == std::string::npos || offset + zone_key_size > data.size())
			{
				return {};
			}

			return data.substr(offset, zone_key_size);
		}

		std::string find_zone_key()
		{
			const auto game_folder = zone_key_folder.empty()
				? std::filesystem::absolute(game_files::get().path("..")).lexically_normal()
				: zone_key_folder;

			for (const auto& folder : {std::filesystem::current_path(), game_folder})
			{
				if (auto key = read_key_file(folder / zone_key_file); !key.empty())
				{
					return key;
				}
			}

			if (!std::filesystem::is_directory(game_folder))
			{
				return {};
			}

			for (const auto& entry : std::filesystem::directory_iterator(game_folder))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".exe")
				{
					if (auto key = find_key_in_executable(entry.path()); !key.empty())
					{
						ZONETOOL_INFO("Using the zone key from %s", entry.path().filename().string().data());
						return key;
					}
				}
			}

			return {};
		}
	}

	void set_zone_key_folder(const std::filesystem::path& folder)
	{
		zone_key_folder = folder;
	}

	std::string zone_key()
	{
		static const auto key = find_zone_key();
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
			.build_zone = build_zone,
			.token_name = token_name,
		};

		return s2;
	}
}
